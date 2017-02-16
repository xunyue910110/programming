#ifndef _OCCIFLOWLINEDAO_H_
#define _OCCIFLOWLINEDAO_H_
#include "flowline/FlowLineDao.h"
#include "flowline/SystemInfo.h"
#include "base/Properties.h"

	namespace oracle {
		namespace occi {
			class Environment;
			class Connection;
		}
	}
namespace flowline {
   using namespace oracle::occi;
   
   class Metadata;
/**
 * 
 */
class OcciFlowLineDao : public FlowLineDao{
    string DEFAULT_TIMESTAMP;
    
    Environment* m_env;
    Connection* m_conn;
    map<string, Definition*> m_definitions;
    map<int, Metadata*> m_metadatas;
    map<string, pair<int, int> > m_nextIds;    
    AdapterFactory* m_factory;
	map<int, Database> m_databases;
	map<int, Host> m_hosts;
public:
    OcciFlowLineDao();
    virtual ~OcciFlowLineDao();

    virtual void setAdapterFactory( AdapterFactory* factory );

    virtual void init(const Properties* env) ;
    /**
     * 外部不需要释放, 由dao管理.
     */
    virtual Definition& getDefinition( const string name );

    virtual int nextId( const string& key );

    virtual int logExecution( int cycleLogId ,int batchId, Executor& actor, time_t begin, time_t end ) ;

    virtual int logExecution( int cycleLogId ,int batchId, Executor& actor, vector<pair<PResource, int> >& inputs, vector<PResource>& outputs, time_t begin, time_t end ) ;
	virtual const Host& getHost(int id);
	virtual const Database& getDatabase(int id);

    virtual int logOpenBatch( int cycleLogId );

    virtual void logCloseBatchs( int cycleLogId );
    virtual int logOpenCycle( Definition& def, const Cycle::Instance& ci );

    virtual void logCloseCycle( int cycleLogId );

    virtual void getUnfinishedCycleInstances( Definition& def, vector<pair<int, Cycle::Instance> >& logid_cycleinstance ) ;
	virtual const Metadata& getMetadata( int id );
//20060116-change -z	
	//virtual bool OcciFlowLineDao::hasPassed(int cycleLogId, int checkerId);   
	virtual bool hasPassed(int cycleLogId, int checkerId);   
 	//virtual void OcciFlowLineDao::logPassed(int cycleLogId, int checkerId);
 	virtual void logPassed(int cycleLogId, int checkerId);
	
	/**
     * 标记已经处理过的资源
     * @param executor 执行体
     * @param resource 需要标记的资源, 如果已经处理过, 在pair的second置为true, 否则false
     * @param after 从当前时间完全多长时间之内查询 >= current - ago
     */ 	
    virtual void signProcessedResources(Executor& executor, 
    	vector<pair<PResource, bool> >& resources, time_t ago);
private:
	int internalLogExecution(int cycleLogId ,int batchId, Executor& actor, time_t begin, time_t end ) ;

    Node* createNode( const string& className );

    Node* getNode( Definition& def, int id );

    void initAdaptable( Adaptable& node, const string& adapterClassName );

    void initRelationship( Definition& def );
    
    void initCheckers( Definition& def );

    void initExtAttribute( Element & e );

    string time2timestamp( time_t st );

    time_t timestamp2time( const string& timestamp );
    
    bool isLocalHost(const string& ip);
    
    const Host& getHostByHostId( int hostId );
    
    void loadHosts();
};
}
#endif //_OCCIFLOWLINEDAO_H_
