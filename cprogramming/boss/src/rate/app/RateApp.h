
#ifndef __RATEAPP_H__20080613_
#define __RATEAPP_H__20080613_

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "RateGlobalInfo.h"
#include "Inputer.h"
#include "Outputer.h"
#include "PMManager.h"
#include "RateLog.h"
#include "Processor.h"
#include "BillManager.h"

class RateApp: public Application 
{
  public:
    RateApp();
   virtual ~RateApp();
  protected:
    virtual bool initialization();
    virtual bool beforeLoop();
    virtual bool loopProcess();
    virtual bool end();
    bool installProc();
    bool cycInfoCtl(); //帐期流程控制
  private:
    bool RateRun();
  
    bool begin();
    bool rollback();
    bool commit();
    void writeLog();
    
    //获取处理日期、时间
    //r_curDate: YYYYMMDD  系统日期
    //r_cycle: YYMM  系统日期对应的帐期
    bool getDealTime(int &r_curDate,int &r_cycle) const;
    //处理固定费文件
    void dealRentFeeFile(const char *t_fileName);
    bool realUserGrpDiscntProcess();
    
  private:
     Inputer        m_inputer;    //输入管理对象
     RateConfig     m_config;    //配置文件 
     //RateGlobalInfo m_gRateInfo; //处理间的信息
     RateLog        m_rateLog;   //处理日志信息

     Outputer       m_outputer;   //输出管理对象

     BillManager    m_billManager; //帐单管理对象
     PMManager      m_pmManager;   //参数管理
     Processor      m_processor;   //PROC处理器

     RateRunInfo    m_runInfo;       //运行信息
     CycParam       m_cycParam;      //当前帐期信息
     int            m_sysCycle;      //系统时间帐期
     //add by gaojf 控制查询更新时间戳间隔
     time_t         m_pmUpdateTime; //参数更新时间
};

#endif //__RATEAPP_H__20080613_

