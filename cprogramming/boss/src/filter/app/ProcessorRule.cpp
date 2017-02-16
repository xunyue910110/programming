#include "ProcessorRule.h"
#include <algorithm>
#include <iostream>
#include "base/StringUtil.h"
#include "base/SysParam.h"
#include "FilterConfig.h"
#include "FilterRule.h"
//#include "CDRJScript.h"
#include "ProcessBase.h"

ProcessorRule::ProcessorRule()
{
  m_process_name.clear();
  m_pProcessbase.clear();
}
ProcessorRule::~ProcessorRule()
{
  m_process_name.clear();
  m_pProcessbase.clear();
}

ProcessorRule::ProcessorRule(const ProcessorRule &r_obj)
{
  m_subChanName      = r_obj.m_subChanName;
  m_process_name     = r_obj.m_process_name;
  m_pProcessbase     = r_obj.m_pProcessbase;
}

bool ProcessorRule::setProcedures()
{
  ProcessBase * t_pProc;
  m_pProcessbase.clear();
  for(vector<string>::iterator t_strItr=m_process_name.begin();
      t_strItr!=m_process_name.end();++t_strItr)
  {
    t_pProc = ProcessBaseHelper::GetProcedure(*t_strItr);
    if(t_pProc == NULL)
    {
      #ifdef _DEBUG_ 
        cout<<"Proc:"<<*t_strItr<<" not defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_pProcessbase.push_back(t_pProc);
  }
  return true;
}
void ProcessorRule::dump() const
{
   cout<<"m_subChanName   = "<<m_subChanName<<endl;

   cout<<"m_process_name:  ";
   for(vector<string>::const_iterator r_Itr=m_process_name.begin();
       r_Itr!=m_process_name.end();++r_Itr)
   {
       cout<<*r_Itr<<"\t";
   }
   cout<<endl;
}


ProcessorRule* ProcessorRuleMgr::getProcessorRule(const string & subname)
{

  for(vector<ProcessorRule>::iterator t_itr=m_processorRule.begin();
      t_itr!=m_processorRule.end(); ++t_itr)
  {
       if (t_itr->m_subChanName.compare(subname)==0)
       {
         return &(*t_itr);
       }
  }
  return &m_defaultRule;

}

bool ProcessorRuleMgr::initialize(InputRuleMgr *r_inputRuleMgr)
{
  ProcessorRule t_processorRule;
  string t_value;
  for(vector<InputRuleMgr::InputParam>::iterator t_itr=r_inputRuleMgr->m_inputParam.begin();
      t_itr!=r_inputRuleMgr->m_inputParam.end();++t_itr)
  {
      //子通道名正确
      //t_value = StringUtil::toString(t_itr->m_subChanName);
      t_value = t_itr->m_subChanName.c_str();
      if(!t_value.compare(0,strlen("channel_"),"channel_")==0)
      {
         #ifdef _DEBUG_
           cout<<"t_processorRule.m_subChanName ："<<t_value<<"  define error!"
               <<__FILE__<<__LINE__<<endl;
         #endif
         return false;
      }
      t_processorRule.m_subChanName = t_value;
      
      //处理过程名不为空
      t_processorRule.m_process_name.clear();
      if(t_itr->m_procName.size()<=0)
      {
         #ifdef _DEBUG_
           cout<<"m_process_name no define!"
               <<__FILE__<<__LINE__<<endl;
         #endif
         return false;
      }
      for(vector<PString>::iterator t_itrs=t_itr->m_procName.begin();
          t_itrs!=t_itr->m_procName.end();++t_itrs)
      {
         //t_processorRule.m_process_name.push_back(StringUtil::toString(*t_itrs) );
         t_processorRule.m_process_name.push_back(t_itrs->c_str() );
      }
      //根据处理名设定处理过程
      if(t_processorRule.setProcedures()==false)
      {
        #ifdef _DEBUG_
          cout<<"t_processorRule.setProcedures() false!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        return false;
      }
//
      m_processorRule.push_back(t_processorRule);
  }

  //缺省处理过程
  //t_value = "ProcessorStat";
  //m_defaultRule.m_process_name = StringUtil::split(t_value,";");

  return true;
}

void ProcessorRuleMgr::dump() const
{
  //cout<<"------------m_defaultRule-------------"<<endl;
  //m_defaultRule.dump();
  cout<<"............Processor Rule Mgr ........"<<endl;

  for(vector<ProcessorRule>::const_iterator r_Itr=m_processorRule.begin();
      r_Itr!=m_processorRule.end();++r_Itr)
  {
      r_Itr->dump();
  }

  cout<<"-----------------------------------\n"<<endl;

}
