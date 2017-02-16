#include "InoutputRule.h"
#include <algorithm>
#include <iostream>
#include "base/StringUtil.h"
#include "base/SysParam.h"
#include "RateConfig.h"

bool InoutputRuleMgr::setCdrFormat(const PString & r_formatName,
                                   const vector<CdrFormat> &r_cdrFormats,
                                   const CdrFormat  *      &r_cdrFmt
                                   )
{
  CdrFormat  t_cdrFormat;
  vector<CdrFormat>::const_iterator t_cdrFmtItr;
  t_cdrFormat.m_formatName = r_formatName;
  t_cdrFmtItr=::lower_bound(r_cdrFormats.begin(),r_cdrFormats.end(),t_cdrFormat);
  if(t_cdrFmtItr==r_cdrFormats.end() || !(*t_cdrFmtItr == t_cdrFormat))
  {
    #ifdef _DEBUG_
      cout<<"cdrformat ："<<r_formatName<<" no defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  r_cdrFmt = &(*t_cdrFmtItr);
  return true;
}
bool InoutputRuleMgr::isPath(const PString &r_pathName)
{
  struct stat r_buf;

  if(lstat(r_pathName.c_str(),&r_buf)<0)
  {
    return false;
  }
  if(S_ISDIR(r_buf.st_mode))
    return true;
  else
    return false;
}


bool InputRuleMgr::initialize(SysParam *r_sysparam,
                              const char* r_appName,
                              const int &r_channelNo,
                              const CdrAttribMacros &r_attribMacros)
{
  string t_rootpath,t_path,t_subPath,t_subNodeName,t_value;
  string t_appName,t_channelNo;
  vector<string>  t_subNodeList,t_initDefs;
  vector<string>::iterator t_nodeItr;
  InputRule       t_inputRule;
  InputPath       t_inputPath;
  map<string,string> t_nodes;
  PString            t_pstr;
  vector<PString>    t_pstrList;
  
  t_appName = r_appName;t_channelNo=StringUtil::toString(r_channelNo);
  //1.读取输入规则定义信息（文件名前缀和规则的定义关系）
  t_path = SECTDELIM + t_appName + SECTDELIM + "input_rule";
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
    if (!r_sysparam->getValue(t_subPath,t_nodes))
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath<<",t_nodes)"
            <<" false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_pstr=t_nodes["pattern"].c_str();
    t_pstrList.clear();
    t_pstr.split(';',t_pstrList);
    t_value = t_nodes["init_values"];
    t_initDefs.clear();
    t_inputRule.m_initValues.clear();
    if(t_value.length()>0)
    {
      if(CdrFormat::parseAttribMacro(t_value,r_attribMacros)==false)
      {
        #ifdef _DEBUG_
          cout<<"CdrFormat::parseAttribMacro("<<t_value<<",r_attribMacros)"
              <<" false!"<<__FILE__<<__LINE__<<endl;
        #endif
        return false;
      }
      t_initDefs = StringUtil::split(t_value,";");
      for(vector<string>::iterator t_strItr=t_initDefs.begin();
          t_strItr!=t_initDefs.end();t_strItr++)
      {
        vector<string> t_pair=StringUtil::split(*t_strItr,":");
        if(t_pair.size()!=2)
        {
          #ifdef _DEBUG_
            cout<<"init Def :"<<*t_strItr<<" error!"
                <<__FILE__<<__LINE__<<endl;
          #endif
          return false;
        }
        t_inputRule.m_initValues[atoi(t_pair[0].c_str())]=t_pair[1];
      }
    }
    t_inputRule.m_patternType=(PATTERNTYPE)(atoi(t_nodes["patternType"].c_str()));
    if(t_inputRule.m_patternType<=0) 
    {
      t_inputRule.m_patternType=PATTERN_TYPE_FILE;
    }
    t_inputRule.m_formatName=t_nodes["format"].c_str();
    for(vector<PString>::iterator t_pstrItr=t_pstrList.begin();
        t_pstrItr!=t_pstrList.end();++t_pstrItr)
    {
      t_inputRule.m_pattern=*t_pstrItr;
      m_inputRules.push_back(t_inputRule);  
    }
  }
  ::sort(m_inputRules.begin(),m_inputRules.end());
  //2.读取输入目录一些配置信息
  m_intputPaths.clear();
  t_path = SECTDELIM + t_appName + SECTDELIM + t_channelNo+ SECTDELIM +"input_path";
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
    t_nodes.clear();
    if (!r_sysparam->getValue(t_subPath,t_nodes))
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath<<",t_nodes)"
            <<" false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_inputPath.m_inputPath = t_nodes["input_path"];
    t_inputPath.m_inputBackPath= t_nodes["input_backup_path"];
    if(t_inputPath.m_inputBackPath.length()>0)
    {
      t_inputPath.m_bakFlag = true;
    }else
    {
      t_inputPath.m_bakFlag = false;
    }
    t_inputPath.m_invalidPath= t_nodes["invalid_path"];
    t_inputPath.m_delayedPath= t_nodes["delayed_path"];
    t_inputPath.m_sortMode= atoi(t_nodes["sort_method"].c_str());
    //一些校验
    if(t_inputPath.valid()==false)
    {
      #ifdef _DEBUG_
        cout<<"input path is invalid !"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_intputPaths.push_back(t_inputPath);
  }  
  return true;
}
//根据话单格式列表设置输入规则中的话单格式指针
bool InputRuleMgr::setCdrFormat(const vector<CdrFormat> &r_cdrFormats)
{
  for(vector<InputRule>::iterator t_itr=m_inputRules.begin();
      t_itr!=m_inputRules.end();++t_itr)
  {
    if(InoutputRuleMgr::setCdrFormat(t_itr->m_formatName,r_cdrFormats,t_itr->m_pFormat)==false)
    {
      #ifdef _DEBUG_
        cout<<"cdrformat ："<<t_itr->m_formatName<<" no defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  return true;
}
InputRule *InputRuleMgr::getInputRule(const char* r_pattern,const PATTERNTYPE &r_patternType)
{
  InputRule   t_inputRule;
  vector<InputRule>::iterator r_ruleItr;
  t_inputRule.m_patternType=r_patternType;
  t_inputRule.m_pattern    =r_pattern;
  r_ruleItr=::lower_bound(m_inputRules.begin(),m_inputRules.end(),t_inputRule);
  if(r_ruleItr==m_inputRules.end()) return NULL;
  if(r_patternType!=r_ruleItr->m_patternType ||
     strncmp(r_pattern,r_ruleItr->m_pattern.c_str(),r_ruleItr->m_pattern.length())!=0)
  {
    return NULL;
  }
  return &(*r_ruleItr);
}

void InputRuleMgr::dump() const
{
  cout<<"............Input Rule Mgr ........"<<endl;
  for(vector<InputRule>::const_iterator r_ruleItr=m_inputRules.begin();
      r_ruleItr!=m_inputRules.end();++r_ruleItr)
  {
    cout<<" rule: "<<endl;
    cout<<"\tm_pattern="<<r_ruleItr->m_pattern<<endl;
    cout<<"\tm_patternType="<<r_ruleItr->m_patternType<<endl;
    cout<<"\tm_formatName="<<r_ruleItr->m_formatName<<endl;
    for(map<int,PString>::const_iterator t_mapItr=r_ruleItr->m_initValues.begin();
        t_mapItr!=r_ruleItr->m_initValues.end();++t_mapItr)
    {
      cout<<"\t\t"<<t_mapItr->first<<":"<<t_mapItr->second<<endl;
    }
  }
  cout<<"............InputPath..............."<<endl;
  for(vector<InputPath>::const_iterator t_pathItr=m_intputPaths.begin();
      t_pathItr!=m_intputPaths.end();++t_pathItr)
  {
    cout<<"\t"<<"m_inputPath    ="<<t_pathItr->m_inputPath<<endl;
    cout<<"\t"<<"m_bakFlag      ="<<t_pathItr->m_bakFlag<<endl;
    cout<<"\t"<<"m_inputBackPath="<<t_pathItr->m_inputBackPath<<endl;
    cout<<"\t"<<"m_invalidPath  ="<<t_pathItr->m_invalidPath<<endl;
    cout<<"\t"<<"m_delayedPath  ="<<t_pathItr->m_delayedPath<<endl;
    cout<<"\t"<<"m_sortMode     ="<<t_pathItr->m_sortMode<<endl;
  }
  cout<<"-----------------------------------"<<endl;
}

bool OutputRuleMgr::initialize(SysParam *r_sysparam,const char* r_appName,
                               const int &r_channelNo,
                               const CdrAttribMacros &r_attribMacros,
                               RateConfig *r_config)
{
  string t_rootpath,t_path,t_subPath,t_subNodeName,t_name,t_value;
  string t_appName,t_channelNo,t_outpath;
  vector<string>  t_subNodeList,t_initDefs;
  vector<string>::iterator t_nodeItr;
  OutputRule      t_outputRule;
  map<string,string> t_nodes;
  PString            t_pstr;
  vector<PString>    t_pstrList;
  vector<string>     t_outputPathNames;
  
  t_appName = r_appName;t_channelNo=StringUtil::toString(r_channelNo);
  //1.读取输出规则定义信息（文件名前缀和规则的定义关系）
  t_path = SECTDELIM + t_appName + SECTDELIM + "output_rule";
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
    if (!r_sysparam->getValue(t_subPath,t_nodes))
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath<<",t_nodes)"
            <<" false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    t_pstr=t_nodes["pattern"].c_str();
    t_pstrList.clear();
    t_pstr.split(';',t_pstrList);
    
    if(atoi(t_nodes["patternType"].c_str())<=0)
    {
      t_outputRule.m_patternType=PATTERN_TYPE_FILE;
    }else
    {
      t_outputRule.m_patternType=(PATTERNTYPE)(atoi(t_nodes["patternType"].c_str()));
    }
    t_outputRule.m_formatName=t_nodes["format"].c_str();

    for(vector<PString>::iterator t_pstrItr=t_pstrList.begin();
        t_pstrItr!=t_pstrList.end();++t_pstrItr)
    {
      t_outputRule.m_pattern=*t_pstrItr;
      m_outputRules.push_back(t_outputRule);  
    }
  }
  ::sort(m_outputRules.begin(),m_outputRules.end());
  //2.读取一些输出目录配置信息
  //2.1.读取普通清单输出目录配置
  t_path = SECTDELIM + t_appName + SECTDELIM+t_channelNo+ SECTDELIM + "output_path";
  t_nodes.clear();
  if (!r_sysparam->getValue(t_path,t_nodes))
  {
    #ifdef _DEBUG_
      cout<<"r_sysparam->getValue("<<t_path<<",t_nodes)"
          <<" false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  t_value = t_nodes["output_paths"].c_str();
  m_outPaths.clear();
  t_outputPathNames.clear();
  t_outputPathNames = StringUtil::split(t_value,";");
  for(vector<string>::iterator outPthNameItr=t_outputPathNames.begin();
      outPthNameItr!=t_outputPathNames.end();++outPthNameItr)
  {
    m_outPaths.push_back(t_nodes[*outPthNameItr].c_str());
  }
  m_errpath = t_nodes["error_path"].c_str();
  //2.2 读取账单输出目录配置
  m_userBillPath     = t_nodes["user_bill_path"].c_str();
  m_userBillPathBak  = t_nodes["user_bill_path_bak"].c_str();
  m_acctBillPath     = t_nodes["acct_bill_path"].c_str();
  m_acctBillPathBak  = t_nodes["acct_bill_path_bak"].c_str();
  m_balInfoPath      = t_nodes["bal_path"].c_str();
  m_fixTrackPath     = t_nodes["fixfee_track_path"].c_str();
  m_acctTrackPath    = t_nodes["acct_track_path"].c_str();
  m_otherInfoPath    = t_nodes["other_info_path"].c_str();
  //Added by dingr 2009-09-16
  //帐前调账轨迹输出路径
  m_adjustBeforePath    = t_nodes["adjustBefore_track_path"].c_str();
  
  //Added By Sunam 2009/8/8 19:22:07
  //免费资源提醒输出目录
  m_remindInfoPath   = t_nodes["remind_info_path"].c_str();
  //add by cuiyw 20090915
  m_wirelessPath     = t_nodes["wireless_bill_path"].c_str();
  //end by cuiyw 20090915 
  //Added By Sunam 2009/9/29 10:06:48
  m_userSumBillPath   = t_nodes["user_sumbill_path"].c_str();
  
 //add by fug for monfee 
 m_monfeeMBillPath = t_nodes["monfee_mbill_path"].c_str();

  m_monfeeIBillPath =  t_nodes["monfee_ibill_path"].c_str();
   
  m_monfeePBillPath =  t_nodes["monfee_pbill_path"].c_str();

  m_monfeeBBillPath =  t_nodes["monfee_bbill_path"].c_str();
  //2.3 读取其它输出目录配置 coding later
  //如果支持容灾系统,则读取容灾对应的目录配置
  //如果支持OCS系统，则读取OCS系统对应的目录配置
  
  if(valid()==false)
  {
    #ifdef _DEBUG_
      cout<<"output rule defined false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

//根据话单格式列表设置输入规则中的话单格式指针
bool OutputRuleMgr::setCdrFormat(const vector<CdrFormat> &r_cdrFormats)
{
  for(vector<OutputRule>::iterator t_itr=m_outputRules.begin();
      t_itr!=m_outputRules.end();++t_itr)
  {
    if(InoutputRuleMgr::setCdrFormat(t_itr->m_formatName,r_cdrFormats,t_itr->m_pFormat)==false)
    {
      #ifdef _DEBUG_
        cout<<"cdrformat ："<<t_itr->m_formatName<<" no defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  return true;
}
OutputRule *OutputRuleMgr::getOutputRule(const char* r_pattern,const PATTERNTYPE &r_patternType)
{
  OutputRule   t_outputRule;
  vector<OutputRule>::iterator r_ruleItr;
  t_outputRule.m_patternType=r_patternType;
  t_outputRule.m_pattern    =r_pattern;
  r_ruleItr=::lower_bound(m_outputRules.begin(),m_outputRules.end(),t_outputRule);
  if(r_ruleItr==m_outputRules.end()) return NULL;
  if(r_patternType!=r_ruleItr->m_patternType ||
     strncmp(r_pattern,r_ruleItr->m_pattern.c_str(),r_ruleItr->m_pattern.length())!=0)
  {
    return NULL;
  }
  return &(*r_ruleItr);
}

void OutputRuleMgr::dump() const
{
  cout<<"............Output Rule Mgr ........"<<endl;
  for(vector<OutputRule>::const_iterator r_ruleItr=m_outputRules.begin();
      r_ruleItr!=m_outputRules.end();++r_ruleItr)
  {
    cout<<" rule: "<<endl;
    cout<<"\tm_pattern="<<r_ruleItr->m_pattern<<endl;
    cout<<"\tm_patternType="<<r_ruleItr->m_patternType<<endl;
    cout<<"\tm_formatName="<<r_ruleItr->m_formatName<<endl;
  }
  cout<<"............OutPath..............."<<endl;
  for(vector<PString>::const_iterator t_pathItr=m_outPaths.begin();
      t_pathItr!=m_outPaths.end();++t_pathItr)
  {
    cout<<"\t"<<*t_pathItr<<endl;
  }
  cout<<"m_errpath         = "<<m_errpath<<endl;
  cout<<"m_userBillPath    = "<<m_userBillPath<<endl;
  cout<<"m_userBillPathBak = "<<m_userBillPathBak<<endl;
  cout<<"m_acctBillPath    = "<<m_acctBillPath<<endl;
  cout<<"m_acctBillPathBak = "<<m_acctBillPathBak<<endl;
  cout<<"m_balInfoPath     = "<<m_balInfoPath<<endl;
  cout<<"m_fixTrackPath    = "<<m_fixTrackPath<<endl;
  cout<<"m_acctTrackPath   = "<<m_acctTrackPath<<endl;
  cout<<"m_otherInfoPath   = "<<m_otherInfoPath<<endl;
  //add by cuiyw  20090915
  cout<<"m_wirelessPath    = "<<m_wirelessPath<<endl;
  //end by cuiyw  20090915
  cout<<"m_monfeeMBillPath    = "<<m_monfeeMBillPath<<endl;

  cout<<"m_monfeeBBillPath    = "<<m_monfeeBBillPath<<endl;

  cout<<"m_monfeePBillPath    = "<<m_monfeePBillPath<<endl;

  cout<<"m_monfeeIBillPath    = "<<m_monfeeIBillPath<<endl;

  cout<<"-----------------------------------"<<endl;
}
