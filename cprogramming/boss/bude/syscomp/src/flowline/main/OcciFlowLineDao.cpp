#include "OcciFlowLineDao.h"
#include "base/OcciUtil.h"
#include "flowline/Definition.h"
#include "flowline/Adaptable.h"
#include "flowline/Task.h"
#include "flowline/Checker.h"
#include "flowline/Reader.h"
#include "flowline/Writer.h"
#include "flowline/ResourceRepository.h"
#include "flowline/DataProcessor.h"
#include "flowline/DataBuffer.h"
#include "flowline/Context.h"
#include "flowline/ResourceProcessor.h"
#include "SoAdapterFactory.h"
#include "flowline/FlowLineException.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "flowline/Metadata.h"

#include <sys/types.h>

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>

namespace flowline {
const string TASK_CLS = "Task";
const string RESOURCE_REPOSITORY_CLS = "ResourceRepository";
const string RESOURCE_PROCESSOR_CLS = "ResourceProcessor";
const string DATA_BUFFER_CLS = "DataBuffer";
const string DATA_PROCESSOR_CLS = "DataProcessor";
const string READER_CLS = "Reader";
const string WRITER_CLS = "Writer";
const string CHECKER_CLS = "Checker";
const int SEQ_STEP = 10;

OcciFlowLineDao::~OcciFlowLineDao() {
	 map<string, Definition*>::iterator it = m_definitions.begin();
	 for(;it != m_definitions.end(); it++) {
	 	delete it->second;
	 }
	 
     map<int, Metadata*>::iterator it2 = m_metadatas.begin();
     for(; it2 != m_metadatas.end(); it2++) {
     	delete it2->second;
     }
     
    if ( m_conn )
        m_env->terminateConnection ( m_conn );
    if ( m_env )
        Environment::terminateEnvironment ( m_env );
}


OcciFlowLineDao::OcciFlowLineDao() : m_env( 0 ), m_conn( 0 ) {}

void OcciFlowLineDao::setAdapterFactory( AdapterFactory* factory ) {
    m_factory = factory;
}

void OcciFlowLineDao::init(const Properties* env) {
    //TODO 以后从连接中获取连接
    m_env = Environment::createEnvironment ( Environment::DEFAULT );
    m_conn = m_env->createConnection ( 
    	env->getProperty("flowline.db.username"), 
    	env->getProperty("flowline.db.password"), 
    	env->getProperty("flowline.db.name"));
    DEFAULT_TIMESTAMP = time2timestamp( 0 );
    static const string sqlAlter = "alter session set nls_date_format ='YYYYMMDDHH24MISS'";
    StatementVar stmt( m_conn, sqlAlter );
    stmt->executeUpdate();
}

/**
 * 外部不需要释放, 由dao管理.
 */
Definition& OcciFlowLineDao::getDefinition( const string name ) {
	map<string, Definition*>::iterator it = m_definitions.find(name);
	if(it != m_definitions.end())
		return *it->second;

    FL_TRACE_P1( "加载流程%s", name.c_str() );

    static const string sqlStmt = 
    	"select ID, NAME, DESCRIPTION, BASE_TIME, DELIVERY_CYCLE,DELIVERY_CYCLE_UNIT, "
        "VALID_FROM, VALID_TO, BATCH_CYCLE, TIME_OUT, BLOCK_SIZE, DELAY "
        "from TD_FL_DEFINITION where NAME=:name";

    StatementVar stmt( m_conn, sqlStmt );
    stmt->setString ( 1, name );
    ResultSetVar rset( stmt );
    if ( rset->next () ) {
    	Definition& def = *(m_definitions[name] = new Definition());
        def.setId( rset->getInt ( 1 ) );
        def.setName( name );
        def.setDescription( rset->getString( 3 ) );
        def.setDeliveryCycle( Cycle(
                                   timestamp2time( rset->getString( 4 ) ),
                                   rset->getInt( 5 ),
                                   rset->getString( 6 ).size() == 0? Cycle::U_SECOND : ( Cycle::Unit ) rset->getString( 6 ).at( 0 ) ) );
        def.setValidFrom( timestamp2time( rset->getString( 7 ) ) );
        def.setValidTo( timestamp2time( rset->getString( 8 ) ) );
        def.setBatchCycle( rset->getInt( 9 ) );
        def.setConditionTimeOut( rset->getInt( 10 ) );
        def.setBlockSize(rset->getInt(11));
        def.setDeliveryDelay(rset->getInt(12));
        initExtAttribute( def );
	    initRelationship( def );
	    initCheckers(def);
	    //    initDeliveryChecker(def);
	    def.init(*this);
	
    return def;
    } else {//cannot find excuet  in  database 
        THROW_P1(ExecutionException,  "没有找到流程定义(%s).", name.c_str() );
    }
}

int OcciFlowLineDao::nextId( const string& key ) {
    pair<int, int>& p = m_nextIds[ key ];
    if ( p.first >= p.second ) {
        try {
            static const string sqlStmt = "select next_id from TF_FL_SEQUENCE where KEY=:key for update";
            StatementVar stmt( m_conn, sqlStmt );
            stmt->setString ( 1, key );
            ResultSetVar rset( stmt );
            if ( rset->next () ) {
                p.first = rset->getInt ( 1 ) ;
                p.second = p.first + SEQ_STEP;
                static const string upSqlStmt = "update TF_FL_SEQUENCE set next_id = :nxt where KEY=:key";
                StatementVar upStmt( m_conn, upSqlStmt );
                upStmt->setInt( 1, p.second );
                upStmt->setString( 2, key );
                upStmt->executeUpdate();
            } else {
                p.first = 1;
                p.second = p.first + SEQ_STEP;
                static const string inSqlStmt = "insert into TF_FL_SEQUENCE (key, next_id) values(:key, :nxt)";
                StatementVar inStmt( m_conn, inSqlStmt );
                inStmt->setInt( 2, p.second );
                inStmt->setString( 1, key );
                inStmt->executeUpdate();
            }

            m_conn->commit();
        } catch ( ... ) {
            m_conn->rollback();
            throw;
        }
    }
    cout << key << " " << p.first << " " << p.second << endl;
    return p.first++;
}
int OcciFlowLineDao::logExecution( int cycleLogId, int batchLogId, Executor& actor, time_t begin, time_t end ) {
    try {
        return internalLogExecution( cycleLogId, batchLogId, actor, begin, end );
    } catch ( ... ) {
        m_conn->rollback();
        throw;
    }
}

int OcciFlowLineDao::internalLogExecution( int cycleLogId, int batchLogId, Executor& actor, time_t begin, time_t end ) {
    int id = nextId( "sys.executionLogId" );
    static const string upSqlStmt = "insert into TL_FL_EXECUTION_LOG "
                                    "(ID, BATCH_LOG_ID, NODE_ID, START_TIME, END_TIME) "
                                    "values(:id, :batch_log_id, :nodeId, :start_time, :end_time)";
    cout<<"value is "<<id<<"/"<<batchLogId<<"/"<<actor.getId()<<"/"<<time2timestamp( begin )<<"/"<<time2timestamp( end )<<endl;
    StatementVar upStmt( m_conn, upSqlStmt );
    upStmt->setInt( 1, id );
    upStmt->setInt( 2, batchLogId );
    upStmt->setInt( 3, actor.getId() );
    upStmt->setString( 4, time2timestamp( begin ) );
    upStmt->setString( 5, time2timestamp( end ) );
    //juyf 为测试多对一，临时注释一下
    upStmt->executeUpdate();
    m_conn->commit();

    return id;
}

 

int OcciFlowLineDao::logExecution( int cycleLogId, int batchLogId, Executor& actor, vector<pair<PResource, int> >& inputs, vector<PResource>& outputs, time_t begin, time_t end ) {


    static const string inSqlStmt = "insert into TL_FL_INPUT_LOG "
                                    "(EXECUTION_ID, RESOURCE_NAME, RECORD_COUNT, REPOSITORY_ID, Used_Time, cycle_log_id) "
                                    "values(:exec_id, :res_name, :res_count, :rep_id, :used_time, :cycle_log_id)";
    static const string outSqlStmt = "insert into TL_FL_OUTPUT_LOG "
                                     "(EXECUTION_ID, RESOURCE_NAME, RECORD_COUNT, REPOSITORY_ID, cycle_log_id) "
                                     "values(:exec_id, :res_name, :res_count, :rep_id, :cycle_log_id)";
    try {
        int id = internalLogExecution( cycleLogId, batchLogId, actor, begin, end );
        //juyf 测试多对一 注释
        StatementVar inStmt( m_conn, inSqlStmt );
        for ( int j = 0; j < inputs.size(); j++ ) {
            inStmt->setInt( 1, id );
            inStmt->setString( 2, inputs[ j ].first ->getName() );
            inStmt->setInt( 3, inputs[ j ].first ->getRecordCount() );
            inStmt->setInt( 4, inputs[ j ].first ->getRepository() ->getId() );
            inStmt->setInt( 5, inputs[ j ].second );
            inStmt->setInt( 6, cycleLogId );
            inStmt->executeUpdate();
        }   
      
        StatementVar outStmt( m_conn, outSqlStmt );
        for ( int j = 0; j < outputs.size(); j++ ) {
            outStmt->setInt( 1, id );
            outStmt->setString( 2, outputs[ j ] ->getName() );
            outStmt->setInt( 3, outputs[ j ] ->getRecordCount() );
            outStmt->setInt( 4, outputs[ j ] ->getRepository() ->getId() );
            outStmt->setInt( 5, cycleLogId );
            outStmt->executeUpdate();
        }
       
        m_conn->commit();
        return id;
    } catch ( ... ) {
        m_conn->rollback();
        throw;
    }
}


int OcciFlowLineDao::logOpenBatch( int cycleLogId ) {
    static const string sqlStmt = "insert into tl_fl_batch_log (id, cycle_log_id, start_time, end_time)"
                                  "values (:id, :cycle_log_id, :stime, :etime)";
    try {
        int id = nextId( "sys.batchLogId" );
        StatementVar stmt( m_conn, sqlStmt );
        stmt->setInt( 1, id );
        stmt->setInt( 2, cycleLogId );
        stmt->setString( 3, time2timestamp( time( 0 ) ) );
        stmt->setString( 4, time2timestamp( time( 0 ) ) );
        stmt->executeUpdate();
        m_conn->commit();
        return id;
    } catch ( ... ) {
        m_conn->rollback();
        throw;
    }
}

void OcciFlowLineDao::logCloseBatchs( int cycleLogId ) {
    static const string sqlStmt = "update tl_fl_batch_log set end_time = :tm where cycle_log_id = :cid";
    try {
        StatementVar stmt( m_conn, sqlStmt );
        stmt->setString( 1, time2timestamp( time( 0 ) ) );
        stmt->setInt( 2, cycleLogId );
        stmt->executeUpdate();
        m_conn->commit();
    } catch ( ... ) {
        m_conn->rollback();
        throw;
    }
}

int OcciFlowLineDao::logOpenCycle( Definition& def, const Cycle::Instance& ci ) {
    int cycleLogId;
    static const string sqlStmt = "insert into tl_fl_cycle_log (id, definition_id, cycle_id, start_time, end_time, plan_start_time, plan_end_time)"
                                  "values (:id, :defid, :cycleid, :stime, :etime, :pstime, :petime)";
    try {
        cycleLogId = nextId( "sys.cycleLogId" );
        StatementVar stmt( m_conn, sqlStmt );
        stmt->setInt( 1, cycleLogId );
        stmt->setInt( 2, def.getId() );
        stmt->setString( 3, ci.getId() );
        stmt->setString( 4, time2timestamp( time( 0 ) ) );
        stmt->setString( 5, DEFAULT_TIMESTAMP );
        stmt->setString( 6, time2timestamp( ci.getBegin() ) );
        stmt->setString( 7, time2timestamp( ci.getEnd() ) );
        stmt->executeUpdate();
        m_conn->commit();
    } catch ( ... ) {
        m_conn->rollback();
        throw;
    }
    return cycleLogId;
}

void OcciFlowLineDao::logCloseCycle( int cycleLogId ) {
    static const string sqlStmt = "update tl_fl_cycle_log set end_time = :tm where id = :id";
    try {
        StatementVar stmt( m_conn, sqlStmt );
        stmt->setString( 1, time2timestamp( time( 0 ) ) );
        stmt->setInt( 2, cycleLogId );
        stmt->executeUpdate();
        m_conn->commit();
    } catch ( ... ) {
        m_conn->rollback();
        throw;
    }
}

void OcciFlowLineDao::getUnfinishedCycleInstances( Definition& def, vector<pair<int, Cycle::Instance> >& logid_cycleinstance ) {
    static const string sqlStmt = "select id, cycle_id, PLAN_START_TIME,PLAN_END_TIME from tl_fl_cycle_log where definition_id=:defid and end_time=:time order by cycle_id";
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt( 1, def.getId() );
    stmt->setString( 2, DEFAULT_TIMESTAMP );

    FL_TRACE_P1( "为%s查找未完工的周期", def.getName().c_str() );
    ResultSetVar rset( stmt );
    while ( rset->next() ) {
        Cycle::Instance ci;
        ci.setId( rset->getString( 2 ) );
        ci.setBegin( timestamp2time( rset->getString( 3 ) ) );
        ci.setEnd( timestamp2time( rset->getString( 4 ) ) );

        logid_cycleinstance.push_back( pair<int, Cycle::Instance> ( rset->getInt( 1 ), ci ) );
    }
}

Node* OcciFlowLineDao::createNode( const string& className ) {
    if ( className == TASK_CLS )
        return new Task();
    else if ( className == READER_CLS )
        return new Reader();
    else if ( className == WRITER_CLS )
        return new Writer();
    else if ( className == RESOURCE_REPOSITORY_CLS ) {
        return new ResourceRepository();
    } else if ( className == RESOURCE_PROCESSOR_CLS )
        return new ResourceProcessor();
    else if ( className == DATA_PROCESSOR_CLS )
        return new DataProcessor();
    else if ( className == DATA_BUFFER_CLS )
        return new DataBuffer();
    else
        THROW_P1( DefinitionException, "无效的类名(%s)", className.c_str() );
}

Node* OcciFlowLineDao::getNode( Definition& def, int id ) {
    Node * node = def.getNodeById( id );
    if ( node != 0 )
        return node;
        
	FL_TRACE_P1("加载节点%d", id);
	
    static const string sqlStmt = "select n.NAME, n.DESCRIPTION," 
    	"n.CLASS_NAME, n.ADAPTER_CLASS_NAME, s.STAGE," 
    	"n.METADATA_ID from TD_FL_NODE n, TD_FL_STAGE s where n.ID=s.NODE_ID and s.DEFINITION_ID=:defid and ID=:id and n.CLASS_NAME <> 'Checker'";
    	
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt ( 1, def.getId() );
    stmt->setInt ( 2, id);
    ResultSetVar rset( stmt );
    if ( rset->next () ) {
        string className = rset->getString( 3 );
        node = createNode( className );
        node->setId( id );
        node->setName( rset->getString( 1 ) );
        node->setDescription( rset->getString( 2 ) );
        def.addNode( node );
        //
        initExtAttribute(* node );

        Executor* executor = dynamic_cast<Executor*>( node );
        if ( executor ) {
            executor->setDelivery( rset->getString( 5 ).at(0) == 'D' );
        }

        Storage* storage = dynamic_cast<Storage*>( node );
        if(storage) {
        	if(rset->getInt(6) != 0) {
        		storage->setMetadata(&getMetadata(rset->getInt(6)));
        	}
        }

        Adaptable* adaptable = dynamic_cast<Adaptable*>( node );
        if ( adaptable ) {
            initAdaptable( *adaptable, rset->getString( 4 ) );
        }
        return node;
    }

    THROW_P1( DefinitionException, "无法定位节点(%d)", id );
}

void OcciFlowLineDao::initAdaptable( Adaptable& element, const string& adapterClassName ) {
    FL_TRACE_P1("初始化适配器%s", adapterClassName.c_str());
    Adapter* adapter = m_factory->create(adapterClassName);
    if ( adapter != 0 ) {
        ElementAdapter * ea = dynamic_cast<ElementAdapter*>( adapter );
        if ( ea == 0 ) {
            delete adapter;
            THROW_P1( DefinitionException, "适配器(%s)类型不匹配, 必须要继承ElementAdapter", adapterClassName.c_str() );
        }
        FL_TRACE_P2( "节点'%s'使用的适配器是'%s'", element.getName().c_str(), adapterClassName.c_str() );
        element.setAdapter( ea );
        //adapter->init(*this);
    } else {
        THROW_P2( DefinitionException, "无法为(%s)定位适配器(%s)", element.getName().c_str(), adapterClassName.c_str() );
    }
}

void OcciFlowLineDao::initRelationship( Definition& def ) {
	FL_TRACE_P1("初始化流程关系%s", def.getName().c_str());
	
    static const string sqlStmt = "select FROM_NODE_ID, TO_NODE_ID, TYPE, ID from TD_FL_RELATIONSHIP where DEFINITION_ID=:id";
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt( 1, def.getId() );
    ResultSetVar rset( stmt );

    while ( rset->next() ) {
        int formId = rset->getInt ( 1 ) ;
        int toId = rset->getInt ( 2 ) ;

        Node* from = getNode( def, formId );
        Node* to = getNode( def, toId );
        //cout << to->getName() << endl;
        if(rset->getString(3) != "Reference") {
        	FL_TRACE_P2("为节点%s添加后续节点%s", from->getName().c_str(), to->getName().c_str()); 
        	to->addPreviousNode( from );
        } else {
        	DataProcessor* dataProcessor = dynamic_cast<DataProcessor*>(from);
        	ResourceRepository* repository = dynamic_cast<ResourceRepository*>(to);
        	
        	Element conn;
        	conn.setId(rset->getInt(4));
        	initExtAttribute(conn);
        	FL_TRACE_P2("为节点%s添加关联%s", dataProcessor->getName().c_str(), repository->getName()); 
        	dataProcessor->addReference(repository, conn.getExtendedAttribute("condition"));
        }
    }
}


void OcciFlowLineDao::initCheckers( Definition& def ) {
	FL_TRACE_P1("初始化交付条件%s", def.getName().c_str());
	
    static const string sqlStmt = "select n.ID, n.NAME, n.DESCRIPTION,n.ADAPTER_CLASS_NAME " 
    	"from TD_FL_NODE n where definition_id=:id and class_name='Checker'";
    	
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt( 1, def.getId() );
    ResultSetVar rset( stmt );

    while ( rset->next() ) {
        Checker* checker = new Checker();
        checker->setId( rset->getInt(1) );
        checker->setName( rset->getString( 2 ) );
        checker->setDescription( rset->getString( 3 ) );
        initExtAttribute(* checker );
        initAdaptable( *checker, rset->getString( 4 ) );
        def.addDeliveryChecker(checker);
    }
}


void OcciFlowLineDao::initExtAttribute( Element& e ) {
	FL_TRACE_P1("初始化扩展属性%d", e.getId());
    
    static const string sqlStmt = "select NAME, VALUE from TD_FL_EXT_ATTRIBUTE where ELEMENT_ID = :id";
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt ( 1, e.getId() );
    ResultSetVar rset( stmt );

    while ( rset->next () ) {
    	FL_TRACE_P3("set %s.%s=%s", e.getName().c_str(), rset->getString( 1 ).c_str(), rset->getString( 2 ).c_str());
        e.setExtendedAttribute( rset->getString( 1 ), rset->getString( 2 ) );
    }
}

 bool OcciFlowLineDao::hasPassed(int cycleLogId, int checkerId) {
 	FL_TRACE_P2("查找是否已经检查通过, cycleLogId=%d, checkerId=%d", cycleLogId, checkerId);
 	
    static const string sqlStmt = "select PASSED from TD_FL_CHECK_STATE "
    	"where CYCLE_LOG_ID =:cycleLogId and CHECKER_ID = :checkerId and PASSED = 1";    	
    	
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt ( 1, cycleLogId );
    stmt->setInt ( 2, checkerId );
    ResultSetVar rset( stmt );

    return rset->next ();
}
    
 void OcciFlowLineDao::logPassed(int cycleLogId, int checkerId) {
 	static const string sqlStmt = "insert into TD_FL_CHECK_STATE (CYCLE_LOG_ID, CHECKER_ID, PASSED) "
	 	"values (:cyclelogid, :checkerid, 1) ";    	
    	
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt ( 1, cycleLogId );
    stmt->setInt ( 2, checkerId );
    stmt->executeUpdate();
}


const Metadata& OcciFlowLineDao::getMetadata( int id ) {
	map<int, Metadata*>::iterator it = m_metadatas.find(id);
	if(it !=  m_metadatas.end()) {
		return  *it->second;
	}
	
	FL_TRACE_P1("加载元数据%d", id);
	
    static const string sqlStmt = "select NAME, DESCRIPTION "
    								"from TD_FL_METADATA where ID = :id";
    StatementVar stmt( m_conn, sqlStmt );
    stmt->setInt( 1, id );
    ResultSetVar rset( stmt );
    if ( rset->next() ) {
        Metadata & rd  = *(m_metadatas[id] = new Metadata());
        rd.setId( id );
        rd.setName( rset->getString( 1 ) );
        rd.setDescription( rset->getString( 2 ) );
        initExtAttribute( rd );
		
        FL_TRACE_P1("加载所有的Field, metadataId=%d", id);
        static const string fldSqlStmt = "select name, type, length, precision, format, description"
           " from TD_FL_FIELD where METADATA_ID = :id order by field_order";
        StatementVar fldStmt ( m_conn, fldSqlStmt );
        fldStmt->setInt( 1, id );
        ResultSetVar fldSet ( fldStmt );
        while ( fldSet->next() ) {
            Field f;
            f.name = fldSet->getString( 1 );
            f.type = fldSet->getString( 2 ).at( 0 );
            f.length = fldSet->getInt( 3 );
            f.precision = fldSet->getInt( 4 );
            f.format = fldSet->getString( 5 );
            f.description = fldSet->getString( 6 );
            FL_TRACE_P4("添加字段%s, type=%d, length=%d, precision=%d", 
            	f.name.c_str(), f.type, f.length, f.precision);
            rd.addField( f );
        }
        return rd;
    }

    THROW_P1( DefinitionException, "找不到资源定义%d", id );

}

void OcciFlowLineDao::loadHosts() {
    static const string sqlStmt = 
    	"select host.ID,host.IP,acc.USERNAME,acc.PASSWORD, acc.id "
    	"from TD_FL_HOST host, TD_FL_SYS_ACCESS acc  "
    	"where host.id = acc.sys_id(+)";
    if ( m_hosts.size() == 0 ) {
    	FL_TRACE("加载主机信息");
        StatementVar stmt ( m_conn, sqlStmt );
        ResultSetVar rset ( stmt );
        while ( rset->next() ) {
            Host host;
            host.id = rset->getInt( 1 );
            host.ip = rset->getString( 2 );
            host.username = rset->getString( 3 );
            host.password = rset->getString( 4 );
            host.isLocalHost = isLocalHost(host.ip);
            if(rset->getInt(5) == 0) //该主机没有access, 所以产生一个负数加到map中
            	m_hosts.insert( pair<int, Host>(-m_hosts.size(), host ) );
            else	
            	m_hosts.insert( pair<int, Host>( rset->getInt(5), host ) );
            FL_TRACE_P3("加载主机(%d) %s %s", host.id, host.ip.c_str(), host.isLocalHost? "是本机" : "不是本机");
        }
    }
}


const Host& OcciFlowLineDao::getHost( int accessId ) {
    loadHosts();
    map<int, Host>::iterator it = m_hosts.find( accessId );
    if ( it == m_hosts.end() )
        THROW_P1( DefinitionException, "不存在主机访问(accessId=%d)", accessId );
    return it->second;
}

const Host& OcciFlowLineDao::getHostByHostId( int hostId ) {
    loadHosts();
    map<int, Host>::iterator it = m_hosts.begin();
    for(; it != m_hosts.end(); it++) {    	
    	if(it->second.id == hostId)
    		return it->second;
    }
    THROW_P1( DefinitionException, "不存在主机(hostId=%d)", hostId );
}

bool OcciFlowLineDao::isLocalHost( const string& ip ) {
    if ( StringUtil::equalsIgnoreCase( ip, "localhost" ) )
        return true;

    if ( ip == "127.0.0.1" )
        return true;

	//取本机名
    char hostname[ 32 ];
    if ( gethostname( hostname, 32 ) ) {
        THROW_P1( ExecutionException, "获取本机名失败%s", strerror( errno ) );
    }
	//比较主机名
    if ( strcmp( hostname, ip.c_str() ) == 0 )
        return true;

	//比较ip
    in_addr_t ip2 = inet_addr( ip.c_str() );

    hostent* host = gethostbyname( hostname );
    if ( host != NULL ) {
        return ( ip2 == ( ( in_addr * ) * host->h_addr_list ) ->s_addr );
    } else {
        THROW_P1( ExecutionException, "获取本机IP失败%s", strerror( errno ) );
    }
}


const Database& OcciFlowLineDao::getDatabase( int accessId ) {
    static const string sqlStmt = 
    	"select db.ID,db.HOST_ID,db.DB_NAME,acc.USERNAME,acc.PASSWORD,acc.id "
    	"from TD_FL_DATABASE db, TD_FL_SYS_ACCESS acc "
    	"where db.id = acc.sys_id";
    	
    if ( m_databases.size() == 0 ) {
    	FL_TRACE("加载数据库信息");
        StatementVar stmt ( m_conn, sqlStmt );
        ResultSetVar rset ( stmt );
        while ( rset->next() ) {
            Database db( getHostByHostId( rset->getInt( 2 ) ) );            
            db.id = rset->getInt( 1 );
            db.name = rset->getString( 3 );
            db.username = rset->getString( 4 );
            db.password = rset->getString( 5 );
            m_databases.insert( pair<int, Database>( rset->getInt(6), db ) );
            FL_TRACE_P1("加载数据库%d", db.id);
        }
    }
    map<int, Database>::iterator it = m_databases.find( accessId );
    if ( it == m_databases.end() )
        THROW_P1( DefinitionException, "不存在数据库访问(accessId=%d)", accessId );
    return it->second;
}

void OcciFlowLineDao::signProcessedResources(Executor& executor, 
    vector<pair<PResource, bool> >& resources, time_t ago) {
    static const string sqlStmt = "select sysdate from tl_fl_input_log i, tl_fl_execution_log e "
    	"where i.execution_id = e.id and i.resource_name = :name and e.begin_time >= :time";
    
    if(resources.size() == 0)
    	return ;
    	
    StatementVar stmt ( m_conn, sqlStmt );	
    for(int i=0; i<resources.size(); i++) {
    	stmt->setString(1, resources[i].first->getName());
    	stmt->setString(2, time2timestamp(time(0) - ago));
    	ResultSetVar rset ( stmt );
    	resources[i].second = rset->next();
    }
}

string OcciFlowLineDao::time2timestamp( time_t st ) {
    tm ltm;
    localtime_r( &st, &ltm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d%02d%02d%02d%02d", ltm.tm_year + 1900, ltm.tm_mon + 1,
             ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec );
    return buffer;
}

time_t OcciFlowLineDao::timestamp2time( const string& timestamp ) {
   tm ltm;
    //获取
    ltm.tm_year = ( timestamp[ 0 ] - '0' ) * 1000 + ( timestamp[ 1 ] - '0' ) * 100 + ( timestamp[ 2 ] - '0' ) * 10 + ( timestamp[ 3 ] - '0' ) - 1900;
    ltm.tm_mon = ( timestamp[ 4 ] - '0' ) * 10 + ( timestamp[ 5 ] - '0' ) - 1;
    ltm.tm_mday = ( timestamp[ 6 ] - '0' ) * 10 + ( timestamp[ 7 ] - '0' );
    ltm.tm_hour = ( timestamp[ 8 ] - '0' ) * 10 + ( timestamp[ 9 ] - '0' );
    ltm.tm_min = ( timestamp[ 10 ] - '0' ) * 10 + ( timestamp[ 11 ] - '0' );
    ltm.tm_sec = ( timestamp[ 12 ] - '0' ) * 10 + ( timestamp[ 13 ] - '0' );

 //           printf("转换%s:%04d-%02d-%02d %02d:%02d:%02d\n", timestamp.c_str(), ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
   // cout<<"mktime :"<<mktime(&ltm)<<endl;
    return mktime( &ltm );
}

}



