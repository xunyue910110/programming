
#include "Processor.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

#include "RateCdr.h"
#include "RateError.h"
#include "Procedure.h"
//#include "PMHelper.h"//added by zhangong 2006-7-19 16:14

bool Processor::initialize(RateGlobalInfo *r_pRateInfo) 
{
  m_pRateInfo = r_pRateInfo;
  m_pCdr      = &(m_pRateInfo->m_cdr);
  m_pConfig   = m_pRateInfo->m_pConfig;
  map<string,Procedure*>::iterator t_itr;
  map<string,Procedure*> &t_procedures = *ProcedureHelper::GetProcedures();
  for (t_itr=t_procedures.begin(); 
       t_itr!=t_procedures.end(); ++t_itr)
  {
    t_itr->second->setRateInfoObj(r_pRateInfo);
    if (!t_itr->second->initialize()) 
    {
      #ifdef _DEBUG_
        cout<<"Procedure Init error:"<<t_itr->first
            <<" "<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  return true;
}


int Processor::process() 
{
  m_processRule = m_pConfig->m_processRuleMgr.getProcessRule(m_pCdr);
  m_pRateInfo->m_processRule = m_processRule;
  //顺序执行各个处理过程
  for(vector<Procedure*>::iterator t_procItr=m_processRule->m_pProcedures.begin();
      t_procItr!=m_processRule->m_pProcedures.end();++t_procItr)
  {
    int t_ret = ProcedureHelper::ExecuteProcedure(*t_procItr);
  }
  return 0;
}

/*
//设置参数代理指针 added by zhanglong for ocs 2006-7-16 22:35
bool Processor::setPmManager(PMManager *pmManager,int indicator)
{
    map<string,Procedure*>::iterator itr;
    map<string,Procedure*> &procedures = *ProcedureHelper::GetProcedures();
    for (itr=procedures.begin(); itr!=procedures.end(); ++itr) 
    {
        itr->second->m_pmManager    =pmManager;
    }
    PMHelper::m_indicator = indicator;
    return true;
}
*/

