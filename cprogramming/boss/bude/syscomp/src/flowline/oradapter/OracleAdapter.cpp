#include "config-all.h"
#include "flowline/adapter/ResourceRepositoryAdapter.h"
#include "flowline/Resource.h"
#include "flowline/Context.h"
#include "flowline/flbase.h"
#include "flowline/Metadata.h"
#include "base/OcciUtil.h"
#include "flowline/DataSet.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "flowline/scripts.h"
#include "base/supdebug.h"
#include <string>
#include <unistd.h>
#include <ctime>
#include <cstdio>
USING_NAMESPACE(std);
namespace flowline {
namespace oradapter {
const size_t MAX_TRANS_FACTOR = 10;
using namespace flowline;
using namespace flowline::adapter;
string time2timestamp( const time_t st ) {
    tm ltm;
    localtime_r( &st, &ltm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d%02d%02d%02d%02d", ltm.tm_year + 1900, ltm.tm_mon + 1,
            ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec );
    return buffer;
}

time_t timestamp2time( const string& timestamp ) {
    tm ltm;
    //获取
    ltm.tm_year = ( timestamp[ 0 ] - '0' ) * 1000 + ( timestamp[ 1 ] - '0' ) * 100 + ( timestamp[ 2 ] - '0' ) * 10 + ( timestamp[ 3 ] - '0' ) - 1900;
    ltm.tm_mon = ( timestamp[ 4 ] - '0' ) * 10 + ( timestamp[ 5 ] - '0' ) - 1;
    ltm.tm_mday = ( timestamp[ 6 ] - '0' ) * 10 + ( timestamp[ 7 ] - '0' );
    ltm.tm_hour = ( timestamp[ 8 ] - '0' ) * 10 + ( timestamp[ 9 ] - '0' );
    ltm.tm_min = ( timestamp[ 10 ] - '0' ) * 10 + ( timestamp[ 11 ] - '0' );
    ltm.tm_sec = ( timestamp[ 12 ] - '0' ) * 10 + ( timestamp[ 13 ] - '0' );
    //        printf("转换%s:%04d-%02d-%02d %02d:%02d:%02d\n", timestamp.c_str(), ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
    return mktime( &ltm );
}


enum { NONE, TRUNCATE, DELETE, CLOSED, IN_READ, READ_FINISHED, IN_WRITE};

class OraResourceRepositoryAdapter : public ResourceRepositoryAdapter {
public:
 //  friend
class OraResource : public Resource {
        Statement* m_stmt;
        ResultSet * m_result;
        int m_count;
        int m_transactionSize;
        string m_name;
        OraResourceRepositoryAdapter* m_repository;
        int state; //CLOSED, IN_READ, READ_FINISHED, IN_WRITE
    public:
        OraResource( OraResourceRepositoryAdapter* repository, const string& name ) :
        m_stmt(0), m_result(0), m_count( -1 ), m_transactionSize(0), m_name( name ), m_repository( repository ), state(CLOSED) {
        	//cout << ".....xxxxxx........"<<endl;
        }

        virtual ~OraResource() {
            if(m_result) {
            		m_stmt->closeResultSet( m_result );
                	m_result = 0;
        		}
        		
                if(m_stmt) {
                	m_repository->getConnection()->terminateStatement( m_stmt );
                	m_stmt = 0;
                }
        }

        virtual const string& getName() const {
            return m_name;
        }

        virtual int getMaxRecordCount() const {
            return -1;
        }

        virtual int getRecordCount() const {
            return m_count;
        }
        
        Statement* createStatement(Context& ctx, string sql) {
        	const vector<string>& params = m_repository->parseParam(sql);
							FL_TRACE_P1("创建Statement(%s)", sql.c_str());
	            Statement* stmt = m_repository->getConnection()->createStatement( sql );
	            //cout<<"params values ...AAAAAAAAAAAAAAAAAAAAAAAAaa...."<<params.size()<<endl;
	            for ( int i = 0; i < params.size(); i++ ) {
	                if ( params[ i ] == "cycleBegin" ) {
	                    stmt->setString(i+1, time2timestamp( ctx.getCycleInstance().getBegin() ) );
	                } else if ( params[ i ] == "cycleEnd" ) {
	                    stmt->setString(i+1, time2timestamp( ctx.getCycleInstance().getEnd() ) );
	                } else if ( params[ i ] == "cycleId" ) {
	                    stmt->setString(i+1,  ctx.getCycleInstance().getId()  );
	                } else {
	                    THROW_P1( DefinitionException, "不支持的查询变量%s", params[ i ].c_str() );
	                }
	            }
	            return stmt;
        }
		
        void prepareRead( Context& ctx ) {
        	FP_BEGIN(prepareRead);
            string tempTable = m_repository->getTempTable(ctx);
        	if(StringUtil::trim(tempTable).size() != 0) {
        		ostringstream sql;
        		//删除临时表
        		FL_TRACE("删除临时表");
        		sql << "drop table " << tempTable;
                        cout<<"sql value"<<sql.str()<<endl;//zzc
        		try {
	        		StatementVar dropStmt(m_repository->getConnection(), sql.str());
	        		dropStmt->executeUpdate();
        		} catch(...) {
                        } //忽略错误, 可能表根本就不存在
        		
        		//创建临时表
        		FL_TRACE("创建临时表");
        		//sql.seekp(0);
        		sql.str(string());
        		sql << "create table " << tempTable;
                string tempTableSpace = m_repository->getTempTableSpace(ctx);
        		if(StringUtil::trim(tempTableSpace).size() != 0) {
        			sql << " tablespace " << tempTableSpace;
        		}
        		sql << " nologging as " <<  m_repository->getQuerySql(ctx);
        		Statement* createStmt  = createStatement(ctx, sql.str());
        		try {
        			createStmt->executeUpdate();
        		} catch(...) {
        			m_repository->getConnection()->terminateStatement(createStmt);
        			throw;
        		}
        		m_repository->getConnection()->terminateStatement(createStmt);
        		
        		//创建查询
        		FL_TRACE("创建查询");
        		//sql.seekp(0);
        		sql.str(string());
        		sql << "select ";
	        	const vector<flowline::Field>& fields = m_repository->getFields();
	        	for(int i=0; i<fields.size(); i++) {
	        		if(i!=0) sql << ",";
	        		sql << fields[i].name;
	        	}
	        	sql << " from " << m_repository->getTempTable(ctx);
	        	m_stmt = createStatement(ctx, sql.str());
        	} else {
	            m_stmt = createStatement(ctx, m_repository->getQuerySql(ctx));
        	}
        	cout<<(m_stmt->getSQL()).c_str()<<endl;
        	m_stmt->setPrefetchRowCount( ctx.getDefinition().getBlockSize() );
            m_result = m_stmt->executeQuery();
       		FP_END(prepareRead);  
        }

        PDataSet createDataSet(Context& ctx ) {
        		FP_BEGIN(CREATE_DATASET);
            PDataSet dataset = m_repository->getMetadata()->createDataSet();            
            dataset->reserve(ctx.getDefinition().getBlockSize());
            FP_END(CREATE_DATASET);
            return dataset;
        }

        virtual PDataSet read( Context& ctx ) {
            FP_BEGIN(READ_ORA);
        	//第一次读
            if ( state == CLOSED) {
                prepareRead( ctx );
                state = IN_READ;
                m_count = 0;
            } else if(state == READ_FINISHED) {
            	return PDataSet();
            } 
            
            ASSERT(state == IN_READ);

            PDataSet dataset( createDataSet(ctx) );
            //填充数据
            size_t blockSize = ctx.getDefinition().getBlockSize();
            for ( size_t i = 0; i < blockSize && m_result->next(); i++, m_count++ ) {
                DataSet::iterator it = dataset->pushBack();
                for ( int j = 0; j < dataset->fieldCount(); j++ ) {
                    const dataset::Field& f = dataset->getField( j );
                    switch ( f.getDataType() ) {
                    case FTINT:                    	                    	
                        ( *it ) [ j ] = m_result->getInt( j+1 );
                        break;
                    case FTINT64: 
                    	  ( *it ) [ j ] = StringUtil::toValue<int64_t>(m_result->getString( j+1)); 
                    	  break;                    
                    case FTSTRING:
                        ( *it ) [ j ] = m_result->getString( j+1 ).c_str();
                        break;
                    case FTDOUBLE:
                        ( *it ) [ j ] = m_result->getDouble( j+1 );
                        break;
                    }
                }
            }
		
            //说明取完毕
            	
            if ( dataset->rowCount() != blockSize ) {
            	state = READ_FINISHED;
                
            }
      //cout << "----------------read-----------------------------------------" <<endl;
	  	//cout <<"............dataset...................." <<*dataset << endl; 
			//cout << "----------------over-----------------------------------------" <<endl;
            FP_END(READ_ORA);
            return dataset; 
        }
        
        void prepareWrite(Context& ctx, PDataSet dataset) {
        	stringstream buffer;
        	cout<<"221 row -------------"<<endl;
        	if(m_repository->m_inaction == TRUNCATE) {
        		cout<<"truncate table"<<m_repository->getTableName(ctx)<<endl;
        		buffer << "truncate table " << m_repository->getTableName(ctx);
        		StatementVar trunStmt(m_repository->getConnection(), buffer.str());
	       		trunStmt->executeUpdate();
        	} else if(m_repository->m_inaction == DELETE) {
                string sqlDelete = m_repository->getDeleteStatement(ctx);
        		if(StringUtil::trim(sqlDelete).size() == 0) {
        			THROW(DefinitionException, "定义了插入前需要删除动作, 但没有为他配置语句");
        		}
        		Statement* delStmt = createStatement(ctx, sqlDelete);
        		try {
        	
        			delStmt->executeUpdate();
        	
        			m_repository->getConnection()->terminateStatement(delStmt);
        		}catch(...) {
        			m_repository->getConnection()->terminateStatement(delStmt);
        			throw;
        		}
        	}
        
        	//buffer.seekp(0);
          buffer.str(string());
        	buffer << "insert into " << m_repository->getTableName(ctx) << " (";
        	const vector<flowline::Field>& fields = m_repository->getFields();
        	for(int i=0; i<fields.size(); i++) {
        		if(i!=0) buffer << ",";
        		buffer << fields[i].name;
        	}
        	buffer << ") values (";
        	for(int i=0; i<fields.size(); i++) {
        		if(i!=0) buffer << ",";        		
        		buffer <<":"<< fields[i].name;
        	}
        	buffer << ")";
        	
        	const string& sql = buffer.str();
        	
            m_stmt = m_repository->getConnection()->createStatement(sql);
           
            FL_TRACE_P1("设置BlockSize=%d", ctx.getDefinition().getBlockSize());    
            m_stmt->setMaxIterations( ctx.getDefinition().getBlockSize() );          
             
            FL_TRACE_P1("设置语句:%s",sql.c_str());  
            ASSERT(!sql.empty());
          
            for(int i=0; i<dataset->fieldCount(); i++) {
            	const dataset::Field& f = dataset->getField(i);
            	if(f.getDataType() == FTSTRING) {
        			m_stmt->setMaxParamSize(i+1, f.getLength());
            	} else if(f.getDataType() == FTINT64) { //因为int64 使用 string插入
            		m_stmt->setMaxParamSize(i+1, 19);
            	}
        	} 
        	//FL_TRACE_P1("执行写入 %s", sql.c_str());
        }

        virtual void write( Context& ctx, PDataSet dataset ) {
          FP_BEGIN(WRITE_ORA);
        	size_t blockSize = ctx.getDefinition().getBlockSize();
        	cout<<"dataset->rowCount() vaule 282 / state "<<dataset->rowCount()<<"/"<<state<<endl;
        	//if(dataset->rowCount()==0) return ;//zzc
        	//第一次写
            if ( state == CLOSED) {
                prepareWrite( ctx, dataset);
                state = IN_WRITE;
                m_count = 0;                
                m_transactionSize = 0;                
            }            
            if(dataset->rowCount()==0) return ;//zzc--change--- for file record is null can del execute trunc table
            ASSERT(state == IN_WRITE);
            
          //  cout <<"dataset  value  is   ....."<<endl;
           // cout<< *dataset << endl;//zzc
					  
					FP_BEGIN(MAPPING);  
            const vector<flowline::Field>& fields = m_repository->getFields();
            vector<int> fieldMapping; //下标是sql的参数下标, int是dataset的下标
            for ( int i = 0; i < fields.size(); i++ ) {
                int at  =dataset->getFieldIndex(fields[i].name.c_str());
                if(at == -1)
                	THROW_P1(ExecutionException, "字段%s, 在dataset中找不到", fields[i].name.c_str());
                fieldMapping.push_back(at);
            }
		   
		   		FP_END(MAPPING);
		   		
            DataSet::iterator it = dataset->begin();
            for(int b=0;;) {
            	FP_BEGIN(SET_STAT);
            	for(int j=0; j<fields.size(); j++) {
            		
            	  int idx = fieldMapping[j];
            		const dataset::Field& f = dataset->getField(idx);
            		switch ( f.getDataType() ) {
                    case FTINT:
	                      m_stmt->setInt( j+1, (*it)[idx]); 
                        break;
                    case FTINT64:                    	
                        m_stmt->setString( j+1,  StringUtil::toString<int64_t>((*it)[idx]));
                        break;    
                    case FTSTRING:
	                      m_stmt->setString( j+1, string((*it)[idx].c_str()));                       
	                      break;
                    case FTDOUBLE:                    	   
                        m_stmt->setDouble( j+1,(*it)[idx]);
                        break;
   
                }
            	}
            	FP_END(SET_STAT); 
                //FL_TRACE_P1("执行写入%s", "完毕2!");     
            	b++;
            	it++;
            	m_count++;
            	m_transactionSize ++;            	
            	
             	if(b >= blockSize || it==dataset->end()) {
             		FP_BEGIN(executeUpdate);
            	 	m_stmt->executeUpdate();      
            	 	FP_END(executeUpdate);
            	 	b = 0;
            	 	if(!m_repository->m_commitByResource && m_transactionSize >= m_repository->m_maxTransactionSize)  {
            	 	  FL_TRACE_P1("+++ commit: %d", m_repository->m_commitByResource);
            	 		FP_BEGIN(COMMIT);
            	 		//m_repository->getConnection()->commit(); 
            	 		FP_END(COMMIT);
            	 		//m_transactionSize = 0;
            	 		commit();
            	 	}
            	 	
            	 	if(it==dataset->end()) {
            	 		break;	
            	 	}
             	} else {
            		m_stmt->addIteration();
            	}
            
            }
            FP_END(WRITE_ORA); 
        }
        
        virtual void close() {      
            FL_TRACE("do close");
                        //Yuaneg 20050901 状态是in_write都进行提交操作
        	       //if(state == IN_WRITE && m_repository->m_commitByResource!=0)
        		if(state == IN_WRITE)
        	          commit();	
        		state = CLOSED;
        }
        
        void commit() {          
          if(m_transactionSize != 0) {
              FL_TRACE("do commit");
        			m_repository->getConnection()->commit();
        			m_transactionSize = 0;
        	}
        }
        
        void rollback() {
          if(m_transactionSize != 0) {
              FL_TRACE("do rollback");
              m_repository->getConnection()->rollback();
        			m_transactionSize = 0;
        	}
        }
    };


    OraResourceRepositoryAdapter() :m_env(0),m_database(0),m_connection(0),m_metadata(0) {
    }
    
    virtual ~OraResourceRepositoryAdapter() {
    }

    virtual void init(FlowLineDao& dao) {
    	m_metadata = getMetadata();
    	m_tableName_ = getTarget()->getExtendedAttribute( "tableName" );
    	if(m_tableName_.size() == 0)
			  m_tableName_ = m_metadata->getExtendedAttribute( "tableName");
          if(m_tableName_.size() == 0 )m_tableName_ = "tablename is null"; 		
		m_querySql_ = getTarget()->getExtendedAttribute( "querySql" );
		m_tempTable_ = getTarget()->getExtendedAttribute( "tempTable" );
		m_tempTableSpace_ = getTarget()->getExtendedAttribute( "tempTablespace");
		m_inaction = atoi(getTarget()->getExtendedAttribute( "inaction").c_str());
    m_deleteStatement_ = getTarget()->getExtendedAttribute( "deleteStatement");
		m_commitByResource = atoi(getTarget()->getExtendedAttribute( "commitByResource").c_str());
		cout << "commitByResource====" << getTarget()->getExtendedAttribute( "commitByResource") 
		  << " " << atoi(getTarget()->getExtendedAttribute( "commitByResource").c_str()) 
		  << " " << m_commitByResource<<endl;
		m_maxTransactionSize = atoi(getTarget()->getExtendedAttribute( "maxTransactionSize").c_str());
		//初始化连接
		string id = getTarget()->getExtendedAttribute( "accessId" );
        m_database = &dao.getDatabase( atoi( id.c_str() ) );

        m_env = Environment::createEnvironment ( Environment::DEFAULT );
        printf("连接数据库DB=%s,USER=%s,pass=%s", m_database->name.c_str(), m_database->username.c_str(),m_database->password.c_str());
        FL_TRACE_P2("连接数据库DB=%s,USER=%s", m_database->name.c_str(), m_database->username.c_str());
        m_connection = m_env->createConnection ( m_database->username, m_database->password, m_database->name );

        static const string sqlAlter = "alter session set nls_date_format ='YYYYMMDDHH24MISS'";
        StatementVar stmt( m_connection, sqlAlter );	
        stmt->executeUpdate();
    }
    
    

    virtual void list( Context& ctx, vector<PResource>& result ) {
        result.push_back(create(ctx) );
    }

    virtual void afterTargetCommitted( Context& ctx, PResource source ) { 
    	FL_TRACE("afterTargetCommitted");       
    }

    virtual PResource recover( const string& name ) {
        THROW(ExecutionException, "数据库无法做恢复处理");
        //return PResource( new OraResource(this, name ) );
    }

    virtual PResource create( Context& ctx ) {    		
        return PResource( new OraResource(this,  getTableName(ctx)) );
    }

    virtual void prepareCommit( Context& ctx, PResource target ) {
    	FL_TRACE("prepareCommit");        
    }

    virtual void commit( Context& ctx, PResource target ) {
      if(m_commitByResource) {
         ((OraResource*)target.get())->commit();
      }   
    	FL_TRACE("commit");
    }

    virtual void rollback( Context& ctx, PResource target ) {
      if(m_commitByResource) {
         ((OraResource*)target.get())->rollback();
      }
    	FL_TRACE("rollback");
    }

    const vector<flowline::Field>& getFields() {
        return m_metadata->fields();
    }
    
    Connection* getConnection() {
    	ASSERT(m_connection != 0);
    	return m_connection;
    }
    
   const vector<string>& parseParam(const string& sql) {    	
    	map<string, vector<string> >::iterator it =  m_sql2params.find(sql);
    	if(it != m_sql2params.end()) return it->second;
    	vector<string>& params = m_sql2params[sql];
		//-------zzc ----change
			string query = sql;
			int j;
            for ( int i = sql.find( ":" ); i != string::npos; i = sql.find( ":", i + 1 ) ) {
                for (  j = i + 1; j < sql.length(); j++ ) {
                    char ch = sql[j];
                    if ( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9' ) || ch == '_' )
                        continue;
                    if(ch == ' ')break;    
                }
             params.push_back( sql.substr( i + 1, j - i - 1 ) );
            }
        //----------------zzc-------change-----    
            return params;
		}
    string getQuerySql(Context& ctx) {
        string querySql= m_querySql_;
        int sqs;
        sqs = querySql.find("js:");//zzc -change -8-30
        bool isJS = StringUtil::trim(querySql).find("js:") == 0;
        if(isJS) 
            querySql = m_querySql_.substr(3);
            
        if(sqs < 0){    //zzc --change --8-30   
	        if(querySql.size() == 0 || 
	           (querySql.find("select") != 0 && querySql.find("SELECT"))) {
	            stringstream buffer;
	            buffer << "select ";
	            const vector<flowline::Field>& fields = getFields();
	            for(int i=0; i<fields.size(); i++) {
	                if(i!=0) buffer << ",";
	                buffer << fields[i].name;
	            }
	            buffer << " from " << getTableName(ctx);
	            if(querySql.size() != 0) {
	                buffer << " where " << querySql;
	            }
	            querySql = buffer.str();
	        }
        }
        if(isJS) {
            m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
            m_jsContainer.get(querySql).execute(querySql);
        }
        return querySql;
    } 
    
    string getTableName(Context& ctx) {
            if(m_tableName_.find("js:") == 0) {
                string qs;
                m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
                m_jsContainer.get(m_tableName_.substr(3)).execute(qs);
                return qs;
            }
            return m_tableName_;    
    }
    
    string getTempTable(Context& ctx) {
            if(m_tempTable_.find("js:") == 0) {
                string qs;
                m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
                m_jsContainer.get(m_tempTable_.substr(3)).execute(qs);
                return qs;
            }
            return m_tempTable_;    
    } 
    
    string getTempTableSpace(Context& ctx) {
            if(m_tempTableSpace_.find("js:") == 0) {
                string qs;
                m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
                m_jsContainer.get(m_tempTableSpace_.substr(3)).execute(qs);
                return qs;
            }
            return m_tempTableSpace_;    
    }  
    
    string getDeleteStatement(Context& ctx) {
            if(m_deleteStatement_.find("js:") == 0) {
                string qs;
                m_jsContainer.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
                m_jsContainer.get(m_deleteStatement_.substr(3)).execute(qs);
                return qs;
            }
            return m_deleteStatement_;    
    }   
public:
    //插入的配置
    int m_inaction; 
    //提交方式
    bool m_commitByResource;
    int m_maxTransactionSize;
private:
	//基本属性
    Environment *m_env;
    const Database* m_database;
    Connection* m_connection;
    const Metadata* m_metadata;
	string m_tableName_;

	//查询的配置
    string m_querySql_;
    string m_tempTable_;    
    string m_tempTableSpace_;
    
    string m_deleteStatement_;
    
    //记录sql解析后的参数
    map<string, vector<string> > m_sql2params;
    RowJSContainer m_jsContainer;
};


REGISTER_CLASS("OracleRepositoryAdapter", OraResourceRepositoryAdapter, Adapter);

}               
}
