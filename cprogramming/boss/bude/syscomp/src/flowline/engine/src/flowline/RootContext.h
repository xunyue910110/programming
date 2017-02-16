#ifndef _ROOTCONTEXT_H_
#define _ROOTCONTEXT_H_

#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"

namespace flowline
{

class RootContext : public flowline::Context {
public:
	RootContext();
	virtual ~RootContext();
	
	void setFlowLineDao(FlowLineDao* flowlineDao);
	virtual FlowLineDao& getFlowLineDao();
	
	virtual const Properties& getEnvironment() ;
	void setEnvironment(const Properties* prop);
	
	void setBatchLogId(int batchId);
    virtual int getBatchLogId();
//    virtual int nextId( const string& key );

//    virtual int log(Node& actor, time_t begin, time_t end ); 
    
    virtual Logger& getLogger();

	virtual const Cycle::Instance& getCycleInstance();		
	void setCycleInstance(const Cycle::Instance& cins);
	
	void setCycleLogId(int cycleLogId);
	virtual int getCycleLogId();
	
	void setDefinition(Definition* def);
	virtual Definition& getDefinition();
//    virtual int log( Node& actor, vector<pair<PResource,int> >& inputs, vector<PResource>& outputs, time_t begin, time_t end );

    virtual string getAttribute( const string& name ) const;
    
//  virtual const Host& getHost(int id);
	
//	virtual const Database& getDatabase(int id);
private:
	Cycle::Instance m_cycleInstance;    
	FlowLineDao* m_flowLineDao;
	int m_batchLogId;
	int m_cycleLogId;
	const Properties* m_env;
	Definition* m_definition;
};

};

#endif //_ROOTCONTEXT_H_
