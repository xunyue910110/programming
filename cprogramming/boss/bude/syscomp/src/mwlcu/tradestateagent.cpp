#include "tradestateagent.h"
#include "log4cpp/Logger.h"
#include <ctime>
#include <time.h>
#include <algorithm>
#include <vector>
#include <functional>
#include "base/StringUtil.h"
#include "mwlcu/mwlcu.h"
#include "lcu/lcu.h"
USING_NAMESPACE(std);
using namespace log4cpp;
using namespace ebpm4c;

TradeStateAgent TradeStateAgent::agentInstance;

   
void TradeStateAgent::resetCycle(){
	
     gettimeofday(&cycle_begin,NULL);
     cycle_size =  300 ;//* 1000000;
     char* cycle_str = getenv("LCU_TRADE_AGENT_CYCLE");
     if(cycle_str){
     	cycle_size = atoi(cycle_str);
     }
     
     m_tradelist.clear();
     //cout << "[agent]性能指标计时周期开始:" << time2timestamp(cycle_begin.tv_sec) <<endl;
}


TradeStateItem TradeStateAgent::beginTradeState(){
	this->checkCycleTime();
	TradeStateItem ts;
	gettimeofday(&ts.begin, NULL);	
	return ts;
}
    
void TradeStateAgent::submitTradeState(TradeStateItem& ts){
	
	gettimeofday(&ts.end, NULL);	
    //std::tm* tmb = std::localtime(&ts.begin.tv_sec);
	//std::tm* tme = std::localtime(&ts.end.tv_sec);
	//std::asctime(tmb);	
	ostringstream os;
	
	int dif = difftime(ts.end.tv_sec,ts.begin.tv_sec);	
	size_t elapsed = dif * 1000000 + ts.end.tv_usec - ts.begin.tv_usec;
	ostringstream outs; 	
	outs << "[agent][Time:" << time2timestamp(ts.end.tv_sec);
	outs << "][" << ts.trade << "][elapsed: " << elapsed * 0.000001 << "s]---------------" ;
			
	//cout << outs.str() << endl;
	
	Logger& tradelog = Logger::getLogger(LCU_LOG_TRADE + ts.trade);
	LOG_TRACE(tradelog,outs.str().c_str());	
	
	addTradeStateItem(ts);
}

bool TradeStateAgent::addTradeStateItem(TradeStateItem& ts){
     
    int dif = difftime(ts.end.tv_sec,ts.begin.tv_sec);	
	size_t elapsed = dif * 1000000 + ts.end.tv_usec - ts.begin.tv_usec;
	
    map<string,CollectedState>::iterator it =  m_tradelist.find(ts.trade);
    
    if( it != m_tradelist.end()){
	    	CollectedState& col = it->second;
	    	col.called++; 
	    	col.worktime += elapsed;
	    	if(ts.resultcode != 0) col.failed++;    	
	    	if(col.min > elapsed ) col.min = elapsed;
	    	if(col.max < elapsed ) col.max = elapsed;    	
    } else {
			CollectedState col;
			col.trade = ts.trade;
			col.called = 1;
			col.failed = 0;
			col.worktime = elapsed;
			if(ts.resultcode != 0 ) col.failed++;
			col.min = elapsed;
			col.max = elapsed;			
			m_tradelist.insert(pair <string,CollectedState>(ts.trade,col));       
    }
    return true;
}

bool TradeStateAgent::checkCycleTime(){
	gettimeofday(&cycle_now,NULL);
	if(cycle_now.tv_sec - cycle_begin.tv_sec <= cycle_size){ 
		//cout << "[agent]还没有到状态输出周期:" << cycle_now.tv_sec - cycle_begin.tv_sec << endl;
		return false;//没有到周期
	}	
	try {
		writeState();
    }catch(exception & e){
        //cout << "[agent]交易性能采集输出失败:" << e << endl;
        Logger& tradelog = Logger::getLogger(LCU_LOG_TRACE);
        tradelog.alert(Grade::MAJOR, -1,e.what() );       
    }catch(...){
        //cout << "[agent]交易性能采集输出失败，非标准异常." << endl;
    }
	return true;
}


//输出一次统计信息
void TradeStateAgent::writeState(){
	//cout << "[agent]开始输出状态统计信息....." <<endl;
	string  servername = "lcu";
	if ( m_tpsvrinfo != 0 ){
		servername = m_tpsvrinfo->SRVNAME;		
	} else {
	 	//cout << "[agent]无法获得tuxedo服务信息....." <<endl;
	}
		
	//string  pref = string("gboss.lcu.state.") + servername + ".";

	map<string,CollectedState>::iterator it =  m_tradelist.begin();		
	ostringstream  oss;
	
	string   starttime = time2timestamp(cycle_begin.tv_sec);
	string   cyclestr  = StringUtil::toString(cycle_size);	
	int      average;
	CollectedState*	col;
	NameValueList   nvl;	
	
	string trade_path;
	
	MWLCU& mwlcu = MWLCU::getInstance();
    PDefinitionLoaderDao loaderDao = mwlcu.getDefinitionsLoader();
    PUDBIDBDefinitionsLoader  loader = dynamic_pointer_cast< UDBIDBDefinitionsLoader> (loaderDao);
	
	try{
		for( ;it != m_tradelist.end(); it++){		
			col = &(it->second);
						
			average = col->worktime / col->called;
			//oss << (int)average;	    oss >> avgstr;   oss.str(string());		
			//oss << (int)col->max;	oss >> maxstr;   oss.str(string());
			//oss << (int)col->min;	oss >> minstr;   oss.str(string());
			//oss << (int)col->called;	oss >> callstr;  oss.str(string());
			//oss << (int)col->failed;	oss >> failstr;  oss.str(string());
			string proc = col->trade;
			trade_path = loader->getDefinitionByName(proc)->getExtendedAttribute("path");
			nvl.push_back(NameValue("SVRNAME",   servername));
			nvl.push_back(NameValue("PATH",      trade_path));		
			nvl.push_back(NameValue("TRANSCODE", col->trade));
			
			nvl.push_back(NameValue("STARTTIME", starttime));	
			nvl.push_back(NameValue("CYCLESIZE", cyclestr));		
			nvl.push_back(NameValue("CALLED",    StringUtil::toString(col->called)));
			nvl.push_back(NameValue("FAILED",    StringUtil::toString(col->failed)));
			nvl.push_back(NameValue("MAX",       StringUtil::toString(col->max)));
			nvl.push_back(NameValue("MIN",       StringUtil::toString(col->min)));
			nvl.push_back(NameValue("AVERAGE",   StringUtil::toString(average)));

		    //Logger& tradestate = Logger::getLogger(pref + col->trade);
			//tradestate.state(nvl);
			
			Logger& tradelog = Logger::getLogger(LCU_LOG_TRADE + col->trade);
			tradelog.state(nvl);
			
			nvl.clear();
		
		}	
	}catch(exception & e){
		cout << "[agent]输出状态发生异常:" << e <<endl;
	}catch(...){
		cout << "[agent]输出状态发生未知异常:"<<__FILE__ << " : " << __LINE__ << endl;
	}
	//cout << "[agent]输出状态信息完毕." <<endl;
	resetCycle();
}


string  TradeStateAgent::time2timestamp( time_t st ) {
	    tm ltm = *localtime( &st);
	    char buffer[ 32 ];
	    sprintf( buffer, "%04d-%02d-%02d %02d:%02d:%02d", ltm.tm_year + 1900, ltm.tm_mon + 1,
	             ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec );
	    return buffer;
	    
}

time_t  TradeStateAgent::timestamp2time( const string& timestamp ) {
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
