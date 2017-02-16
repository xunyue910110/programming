#ifndef _TRADE_STATEAGENT_H_
#define _TRADE_STATEAGENT_H_

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "mwlcu/mwlcu.h"
#include <ctime>
#include <time.h>


using namespace std;

//采集项
struct  TradeStateItem{ 
	string   trade;      //交易编码
	struct   timeval   begin;      //开始时间
	struct   timeval   end;        //结束时间
	int      resultcode; //成功失败	
};

//采集数据
struct CollectedState{
	string   trade;  //交易编码		
	size_t   called;   //called 统计
	size_t   failed;   //failed 统计
	size_t   worktime;   //总时长
	size_t   min;        //最短时间
	size_t   max;        //最长时间
	//size_t   avg;           //平均时间 总时长/called
	//string   servername;    //服务名	是同一个服务没有必要记
};

class TradeStateAgent
{
private :      
	static TradeStateAgent agentInstance;  
	
	TradeStateAgent():m_tpsvrinfo(0),cycle_size(0) {
		//resetCycle();
	}	
	
	TradeStateAgent(const TradeStateAgent&){
	}      	
	
	TradeStateAgent& operator = (const TradeStateAgent&){
		return *this;
	}
	   
	size_t   cycle_size;    //周期长度(ms)	//周期计划结束时间
	struct   timeval   cycle_begin;	//周期开始时间
	struct   timeval   cycle_now;	//周期开始时间
	
	map<string,CollectedState>  m_tradelist;
	
	TPSVRINFO * m_tpsvrinfo;
	
public:
	
	virtual ~TradeStateAgent(){};    
	static TradeStateAgent& getInstance(){     
		return TradeStateAgent::agentInstance;
    }
    
    void initsvr(TPSVRINFO * tpsvrinfo){
    	m_tpsvrinfo = tpsvrinfo; 
    }
    //开始计时
    TradeStateItem beginTradeState();
    //提交具体某一次调用
    void submitTradeState(TradeStateItem& tradstate);
    //输出一次统计信息
    void writeState();
    
    void resetCycle();
    //设置采集时长
    void setCycleSize(size_t size){cycle_size = size;}
    bool addTradeStateItem(TradeStateItem& tradstate);
    bool checkCycleTime();
    
public:    	
	string  time2timestamp( time_t st ); 

	time_t  timestamp2time( const string& timestamp );
	    	
};

#endif //_TRADE_STATEAGENT_H_
