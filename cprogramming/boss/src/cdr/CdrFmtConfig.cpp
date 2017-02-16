#include "CdrFmtConfig.h"
#include <iostream>
#include <algorithm>
#include <map>
#include "base/SysParam.h"

bool CdrFmtConfig::getSysParam(SysParam *r_sysParam,const char *r_confPath)
{
  string t_rootpath,t_path,t_subNodeName,t_value;
  string t_strChannel;
  vector<string>  t_subNodeList;
  map<string,string> t_nodes;
  
  t_rootpath = r_confPath;
  //1.读取宏定义
  t_path = t_rootpath+SECTDELIM + "cdr_attrib_define";
  if(m_attribMacros.getSysParam(r_sysParam,t_path.c_str())==false)
  {
    #ifdef _DEBUG_
      cout<<"m_attribMacros.getSysParam(r_sysParam,"<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  
  //2.获取话单格式定义
  CdrFormat  t_cdrFormat;
  m_cdrFormatList.clear();
  t_rootpath = t_rootpath + SECTDELIM + "cdr_format"; 
  if(r_sysParam->setSectionPath(t_rootpath)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->setSectionPath("<<t_rootpath<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  t_subNodeList.clear();
  while (r_sysParam->getSubSection(t_subNodeName) == true)
  {
    t_subNodeList.push_back(t_subNodeName);
  } 
  for(vector<string>::iterator t_nodeItr=t_subNodeList.begin();
      t_nodeItr!=t_subNodeList.end();++t_nodeItr)
  {
    t_path=t_rootpath+SECTDELIM+*t_nodeItr;
    if(t_cdrFormat.getSysParam(r_sysParam,t_path.c_str(),
                               t_nodeItr->c_str(),m_attribMacros)==false)
    {
      #ifdef _DEBUG_
        cout<<"t_cdrFormat.getSysParam(r_sysParam,"<<t_path<<") false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_cdrFormatList.push_back(t_cdrFormat);
  }
  ::sort(m_cdrFormatList.begin(),m_cdrFormatList.end());
  
  return true;
}

const CdrFormat* CdrFmtConfig::getCdrFormat(const char * r_formatName) const
{
  CdrFormat  t_cdrFormat;
  vector<CdrFormat>::const_iterator t_cdrFmtItr;
  t_cdrFormat.m_formatName = r_formatName;
  t_cdrFmtItr=::lower_bound(m_cdrFormatList.begin(),m_cdrFormatList.end(),t_cdrFormat);
  if(t_cdrFmtItr==m_cdrFormatList.end() || !(*t_cdrFmtItr == t_cdrFormat))
  {
    return NULL;
  }
  return &(*t_cdrFmtItr);
}

void CdrFmtConfig::dump() const
{
  m_attribMacros.dump();
  for(vector<CdrFormat>::const_iterator t_itr=m_cdrFormatList.begin();
      t_itr!=m_cdrFormatList.end();++t_itr)
  {
    t_itr->dump();
  }
}


