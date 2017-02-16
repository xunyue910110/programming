#include "ProcessRule.h"
#include <algorithm>
#include <iostream>
#include "base/StringUtil.h"
#include "base/SysParam.h"
#include "RateConfig.h"
#include "cdr/CDRJScript.h"
#include "Procedure.h"

ProcessRule::ProcessRule()
{
  clear();
}
ProcessRule::~ProcessRule()
{
  clear();
}
ProcessRule::ProcessRule(const ProcessRule &r_obj)
{
  m_rule              = r_obj.m_rule;
  m_procedures        = r_obj.m_procedures;
  m_baseTpFactors     = r_obj.m_baseTpFactors;
  m_rateFeeIdFactors  = r_obj.m_rateFeeIdFactors;
  m_acctFeeIdFactors  = r_obj.m_acctFeeIdFactors;
  m_pProcedures       = r_obj.m_pProcedures;
  m_baseTpFactorIds   = r_obj.m_baseTpFactorIds;
  m_rateFeeIdFactorIds= r_obj.m_rateFeeIdFactorIds;
  m_acctFeeIdFactorIds= r_obj.m_acctFeeIdFactorIds;  
}
void ProcessRule::clear()
{
  m_rule=NULL;
  m_procedures.clear();
  m_pProcedures.clear();
  m_baseTpFactorIds.clear();
  m_rateFeeIdFactorIds.clear();
  m_acctFeeIdFactorIds.clear();
}


void ProcessRule::parseIdStr()
{
  parseIdStr(m_baseTpFactors,m_baseTpFactorIds);
  parseIdStr(m_rateFeeIdFactors,m_rateFeeIdFactorIds);
  parseIdStr(m_acctFeeIdFactors,m_acctFeeIdFactorIds);
}
void ProcessRule::parseIdStr(const string &r_IdStr,vector<int> &r_IdList)
{
  r_IdList.clear();
  vector<string> t_values;
  t_values = StringUtil::split(r_IdStr,";");
  for(vector<string>::iterator t_Itr=t_values.begin();
      t_Itr!=t_values.end();++t_Itr)
  {
    r_IdList.push_back(atoi(t_Itr->c_str()));
  }
}
bool ProcessRule::setProcedures()
{
  Procedure * t_pProc;
  m_pProcedures.clear();
  for(vector<string>::iterator t_strItr=m_procedures.begin();
      t_strItr!=m_procedures.end();++t_strItr)
  {
    t_pProc = ProcedureHelper::GetProcedure(*t_strItr);
    if(t_pProc == NULL)
    {
      #ifdef _DEBUG_ 
        cout<<"Proc:"<<*t_strItr<<" not defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_pProcedures.push_back(t_pProc);
  }
  return true;
}
void ProcessRule::dump() const
{
  vector<int>::const_iterator t_iItr;
  for(vector<string>::const_iterator t_strItr=m_procedures.begin();
      t_strItr!=m_procedures.end();++t_strItr)
  {
    cout<<*t_strItr<<";";
  }
  cout<<endl;
  //cout<<"m_baseTpFactors = "<<m_baseTpFactors<<endl;
  //cout<<"m_rateFeeIdFactors = "<<m_rateFeeIdFactors<<endl;
  //cout<<"m_acctFeeIdFactors = "<<m_acctFeeIdFactors<<endl;
  cout<<"m_baseTpFactorIds = ";
  for(t_iItr=m_baseTpFactorIds.begin();
      t_iItr!=m_baseTpFactorIds.end();
      ++t_iItr)
  {
    cout<<*t_iItr<<";";
  }
  cout<<endl;
  cout<<"m_rateFeeIdFactorIds = ";
  for(t_iItr=m_rateFeeIdFactorIds.begin();
      t_iItr!=m_rateFeeIdFactorIds.end();
      ++t_iItr)
  {
    cout<<*t_iItr<<";";
  }
  cout<<endl;
  cout<<"m_acctFeeIdFactorIds = ";
  for(t_iItr=m_acctFeeIdFactorIds.begin();
      t_iItr!=m_acctFeeIdFactorIds.end();
      ++t_iItr)
  {
    cout<<*t_iItr<<";";
  }
  cout<<endl;
}


ProcessRule* ProcessRuleMgr::getProcessRule(RateCdr *r_cdr)
{
  for(vector<ProcessRule>::iterator t_itr=m_rules.begin();
      t_itr!=m_rules.end(); ++t_itr) 
  {
    if (t_itr->m_rule->matches())
    {
      return &(*t_itr);
    }
  }
  return &m_defaultRule;    
}

bool ProcessRuleMgr::initialize(SysParam *r_sysparam,const char* r_appName,
                                const int &r_channelNo,
                                const CdrAttribMacros &r_attribMacros,
                                RateConfig *r_config)
{
  string t_rootpath,t_path,t_subPath,t_subNodeName,t_value;
  string t_appName,t_channelNo;
  vector<string>  t_subNodeList;
  vector<string>::iterator t_nodeItr;
  ProcessRule     t_processRule;

  t_appName = r_appName;t_channelNo=StringUtil::toString(r_channelNo);
  //1.读取缺省过程定义
  t_path = SECTDELIM + t_appName + SECTDELIM + "process";
  if (!r_sysparam->getValue(t_path, "default_procedures", t_value)) 
  {
    return false;
  }else
  {
    m_defaultRule.clear();
    m_defaultRule.m_procedures = StringUtil::split(t_value,";");
    if(m_defaultRule.setProcedures()==false)
    {
      #ifdef _DEBUG_
        cout<<"m_defaultRule.setProcedures() false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  
  //2.读取各个过程规则定义
  m_rules.clear();
  t_path = t_path+SECTDELIM+"rule";
  if(r_sysparam->setSectionPath(t_path)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysparam->setSectionPath("<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  t_subNodeList.clear();
  while (r_sysparam->getSubSection(t_subNodeName) == true)
  {
    t_subNodeList.push_back(t_subNodeName);
  }   
  for(t_nodeItr=t_subNodeList.begin();
      t_nodeItr!=t_subNodeList.end();++t_nodeItr)
  {
    t_subPath=t_path+SECTDELIM+*t_nodeItr;
    
    t_processRule.clear();
    //读取条件规则定义
    if (!r_sysparam->getValue(t_subPath, "rule", t_value))
    {
      #ifdef _DEBUG_
        cout<<"get rule false!"<<t_subPath<<"/rule"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    if(CdrFormat::parseAttribMacro(t_value,r_attribMacros)==false)
    {
      #ifdef _DEBUG_
        cout<<"rule:"<<t_value<<" macros define error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_processRule.m_rule = r_config->setScriptApplet(t_value);
    if(t_processRule.m_rule==NULL)
    {
      #ifdef _DEBUG_
        cout<<"rule defined error!"<<t_value<<" "
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    //读取过程定义
    if (!r_sysparam->getValue(t_subPath, "procedure", t_value))
      return false;
    t_processRule.m_procedures.clear();
    t_processRule.m_procedures=StringUtil::split(t_value,";",true);

    if (!r_sysparam->getValue(t_subPath, "basetp_factors", t_value))
    {
      #ifdef _DEBUG_
        cout<<"getValue:"<<t_subPath<<"/basetp_factors error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    if (!CdrFormat::parseAttribMacro(t_value,r_attribMacros))
    { 
      #ifdef _DEBUG_
        cout<<"basetp_factors:"<<t_value<<" macros define error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_processRule.m_baseTpFactors = t_value;
    
    if (!r_sysparam->getValue(t_subPath, "rate_feeid_factors", t_value))
    {
      #ifdef _DEBUG_
        cout<<"getValue:"<<t_subPath<<"/rate_feeid_factors error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    if (!CdrFormat::parseAttribMacro(t_value,r_attribMacros)) 
    {
      #ifdef _DEBUG_
        cout<<"rate_feeid_factors:"<<t_value<<" macros define error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_processRule.m_rateFeeIdFactors = t_value;

    if (!r_sysparam->getValue(t_subPath, "acct_feeid_factors", t_value))
    {
      #ifdef _DEBUG_
        cout<<"getValue:"<<t_subPath<<"/acct_feeid_factors error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    if (!CdrFormat::parseAttribMacro(t_value,r_attribMacros)) 
    {
      #ifdef _DEBUG_
        cout<<"acct_feeid_factors:"<<t_value<<" macros define error!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_processRule.m_acctFeeIdFactors = t_value;
    
    t_processRule.parseIdStr();
    if(t_processRule.setProcedures()==false)
    {
      #ifdef _DEBUG_
        cout<<"t_processRule.setProcedures() false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_rules.push_back(t_processRule);
  }
  
  return true;
}

void ProcessRuleMgr::dump() const
{
  cout<<"------------m_defaultRule-------------"<<endl;
  m_defaultRule.dump();
  cout<<"-----------各个RULE-------------------"<<endl;
  for(vector<ProcessRule>::const_iterator t_itr=m_rules.begin();
      t_itr!=m_rules.end();++t_itr)
  {
    t_itr->dump();
    cout<<"------------------------------------"<<endl;
  }
}
