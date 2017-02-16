#include "Engine.h"
#include "flowline/FlowLineException.h"
#include "RootContext.h"
#include "flowline/ResourceRepository.h"
#include "flowline/flbase.h"
#include <sys/time.h>

namespace flowline {

Engine::Engine(const Properties* env, FlowLineDao* dao, AdapterFactory* factory, const string& main )
        : m_env(env), m_dao( dao ), m_factory( factory ), m_main( main ) {}

Engine::~Engine() {}
void Engine::run() {
    //执行
    if ( !m_main.empty() ) {
        Definition& def = m_dao->getDefinition( m_main );
        time_t current = time(0);
		if((def.getValidFrom() != 0 && def.getValidFrom() > current) || 
		   (def.getValidTo() != 0 && def.getValidTo() <= current)) {
			THROW(DefinitionException, "当前已经超出流程的有效期");
		}
        
        run( def );
    }
}
void Engine::run( Definition& def ) {
	Cycle::Instance ci;
	
	if(def.getDeliveryCycle().getCycle() == 0) {
		//周期无限大
		doCycle(def, 0, ci);
		return;
	}
	
    //查找未完工的周期
    vector<pair<int, Cycle::Instance> > logid_cycleinstance;
    m_dao->getUnfinishedCycleInstances( def, logid_cycleinstance );
    vector<pair<int, Cycle::Instance> >::iterator it = logid_cycleinstance.begin();
    for ( ;it != logid_cycleinstance.end(); it++ ) {
        if ( !doCycle( def, it->first, it->second ) )
            return ;
    }
    
    if ( logid_cycleinstance.size() == 0 ) {
        //没有一条历史记录, 我们计算上个周期
        def.getDeliveryCycle().compute( time( 0 ), ci );
        def.getDeliveryCycle().compute( ci.getBegin() - 1, ci );
       
        FL_TRACE_P1( "没有一条历史记录, 计算出上个周期为(%s)", ci.toString().c_str() );;
    } else {
        ci = logid_cycleinstance[ logid_cycleinstance.size() - 1 ].second;
    }

    //从ci开始往下执行所有的周期, 直到目前时间<ci.getEnd()
    //cout<<"time(0)"<<time(0)<<endl;
    //cout<<"ci.getEnd()"<<ci.getEnd()<<endl;
    while ( time( 0 ) > ci.getEnd() ) {
     	//while(1){
        //计算紧接的下个周期
        def.getDeliveryCycle().compute( ci.getEnd(), ci );

        if ( !doCycle( def, -1, ci ) )
            return ;
    }
}


/**
 *@return true, 可以执行下个周期, false 不能执行下个周期
 */
bool Engine::doCycle( Definition& def, int cycleLogId, const Cycle::Instance& ci ) {
    if ( cycleLogId == -1 ) {
        cycleLogId = m_dao->logOpenCycle( def, ci );
        FL_TRACE_P2( "创建新周期(cyclelogid=%d, %s), 并执行", cycleLogId, ci.toString().c_str() );
    } 

    //新建批处理日志
    int batchLogId = m_dao->logOpenBatch( cycleLogId );

    FL_TRACE_P3( "cyclelogid=%d, %s: 开始执行批处理(batchLogId=%d)", cycleLogId, ci.toString().c_str(), batchLogId );

    RootContext ctx;
    ctx.setEnvironment(m_env);
    
    ctx.setDefinition(&def);
  
    ctx.setFlowLineDao( m_dao );
  
    ctx.setBatchLogId( batchLogId );
  
    ctx.setCycleLogId(cycleLogId);
  
	ctx.setCycleInstance(ci);
	
    int code = def.execute( ctx );
    
    //记录批处理日志
    m_dao->logCloseBatchs( cycleLogId );

    switch ( code ) {
    case 1:
        FL_TRACE_P1( "cycleid=%d: 正常周期结束", ci.getId().c_str() );
        m_dao->logCloseCycle( cycleLogId );
        return true;
    case -2:
        FL_TRACE_P1( "cycleid=%d: 强制进入下个周期", ci.getId().c_str() );
        return true;
    case -1:
        FL_TRACE_P1( "cycleid=%d: 周期条件不满足, 又不能进入下个周期", ci.getId().c_str() );//zzc
       //THROW_P1( ExecutionException, "周期条件不满足, 又不能进入下个周期%d", code );
        return false;//zzc
    case 0:
        FL_TRACE_P1( "cycleid=%d: 正常一个批处理周期", ci.getId().c_str() );
        return false;
    default:
        THROW_P1( ExecutionException, "未知的返回值%d", code );
    }
}


};
