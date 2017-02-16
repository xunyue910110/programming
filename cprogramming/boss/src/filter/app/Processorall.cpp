#include "Processorall.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

#include "ProcessBase.h"
#include "FilterConfig.h"

bool Processorall::initialize(FilterGlobalInfo *r_pFilterInfo)
{

  m_pFilterInfo    = r_pFilterInfo;
  m_pcdr           = &(r_pFilterInfo->m_cdr);
  m_pConfig        = r_pFilterInfo->m_Config;
  //通道切换 重新获取处理类
  //m_pProcessorRule = r_pFilterInfo->m_processorRule;

  map<string,ProcessBase*>::iterator t_itr;
  map<string,ProcessBase*> &t_procedures = *ProcessBaseHelper::GetProcedures();
  for (t_itr=t_procedures.begin();
       t_itr!=t_procedures.end(); ++t_itr)
  {
    t_itr->second->setGlobalInfo(r_pFilterInfo);
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


int Processorall::process() 
{
  m_pProcessorRule = m_pConfig->m_processorRuleMgr.getProcessorRule(m_pFilterInfo->m_subChannelName.c_str());
  m_pFilterInfo->m_processorRule = m_pProcessorRule;

  //顺序执行各个处理过程
  for(vector<ProcessBase*>::iterator t_procItr=m_pProcessorRule->m_pProcessbase.begin();
      t_procItr!=m_pProcessorRule->m_pProcessbase.end();++t_procItr)
  {
    int t_ret = ProcessBaseHelper::ExecuteProcedure(*t_procItr);
    if(t_ret == -1)
    {
      return -1;
    }
  }
  m_pcdr->finalize();

  return 0;
}
