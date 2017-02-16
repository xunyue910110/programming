
#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "config-all.h"
#include "RateGlobalInfo.h"

class Processor 
{
  public:
    bool initialize(RateGlobalInfo *r_pRateInfo);
    int process();
    //设置参数代理指针 added by zhanglong for ocs 2006-7-16 22:35
    //bool setPmManager(PMManager *pmManager,int indicator);

  protected:  //modifid by zhanglong for ocs 2006-6-19 15:12  private -> protected 
    RateGlobalInfo  *m_pRateInfo;
    RateCdr         *m_pCdr;
    RateConfig      *m_pConfig;
    ProcessRule     *m_processRule;  
    
};

#endif

