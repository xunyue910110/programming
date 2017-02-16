#include "RateConfig.h"
#include "base/SysParam.h"
#include <iostream>
#include <algorithm>
#include "base/StringUtil.h"

RateConfig::RateConfig()
{
  m_drFlag    = false;
  m_ocsFlag   = false;
  m_ocsOutPutCdrRule=NULL;
  m_mdbInfo.m_dbType=MDB;
  m_mdbInfo.m_hostName = "";
  m_mdbInfo.m_port = 0;
  m_cycleDbNo = 1;
}
RateConfig::~RateConfig()
{
  
}
bool RateConfig::getSysParam(SysParam *r_sysParam,const char * r_appName,
                             const int &r_channelNo)
{
  string t_rootpath,t_path,t_subNodeName,t_value;
  string t_strChannel;
  vector<string>     t_subNodeList;
  map<string,string> t_nodes;
  
  m_appName  =r_appName;
  m_channelNo=r_channelNo;
  
  //-1. 读取宏定义及话单格式配置信息
  t_path = SECTDELIM + m_appName ;
  if(m_cdrFmtConf.getSysParam(r_sysParam,t_path.c_str())==false)
  {
    #ifdef _DEBUG_
      cout<<"m_cdrFmtConf.getSysParam(r_sysParam,"<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  
  t_strChannel = StringUtil::toString(r_channelNo);
  //0. 读取一些全局定义信息
  t_path = SECTDELIM + m_appName + SECTDELIM + "common"; 
  t_nodes.clear();
  if (!r_sysParam->getValue(t_path, t_nodes)) 
  {
    #ifdef _DEBUG_
      cout<<"get common config param false!"<<t_path
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_provinceCode              = t_nodes["province_code"]; 
  m_dbServName                = t_nodes["db_servname"];
  m_dbUser                    = t_nodes["db_username"];
  m_dbPassword                = t_nodes["db_password"];
  m_interval                  = atoi(t_nodes["interval"].c_str());
  m_doubleCycMode             = atoi(t_nodes["double_cycle_mode"].c_str());
  m_operatorType              = (OPERATOR_TYPE)(atoi(t_nodes["operator_type"].c_str()));

  if(strcasecmp(t_nodes["mdb_dbtype"].c_str(),"RMDB")==0)
  {
    m_mdbInfo.m_dbType   = RMDB;
    m_mdbInfo.m_hostName = t_nodes["mdb_host"];
    m_mdbInfo.m_dbServer = t_nodes["mdb_servername"];
    m_mdbInfo.m_user     = "";
    m_mdbInfo.m_passwd   = "";
    m_mdbInfo.m_port     =  0;    
  }else if(strcasecmp(t_nodes["mdb_dbtype"].c_str(),"ODBCMDB")==0)
  {
    m_mdbInfo.m_dbType = ODBCMDB;
    m_mdbInfo.m_hostName = t_nodes["mdb_host"];
    m_mdbInfo.m_dbServer = t_nodes["mdb_servername"];
    m_mdbInfo.m_user     = t_nodes["mdb_user"];
    m_mdbInfo.m_passwd   = t_nodes["mdb_pass"];
    m_mdbInfo.m_port     =  atoi(t_nodes["mdb_port"].c_str());
  }else
  {//(strcasecmp(t_nodes["mdb_dbtype"].c_str(),"MDB")==0)
    m_mdbInfo.m_dbType = MDB;
    m_mdbInfo.m_hostName = "";
    m_mdbInfo.m_dbServer = t_nodes["mdb_servername"];
    m_mdbInfo.m_user     = "";
    m_mdbInfo.m_passwd   = "";
    m_mdbInfo.m_port     =  0;    
  }
  m_mdbInfo_billInfo = m_mdbInfo; //账单MDB
  if(strcasecmp(t_nodes["mdb2_dbtype"].c_str(),"RMDB")==0)
  {
    m_mdbInfo_userInfo.m_dbType   = RMDB;
    m_mdbInfo_userInfo.m_hostName = t_nodes["mdb2_host"];
    m_mdbInfo_userInfo.m_dbServer = t_nodes["mdb2_servername"];
    m_mdbInfo_userInfo.m_user     = "";
    m_mdbInfo_userInfo.m_passwd   = "";
    m_mdbInfo_userInfo.m_port     =  0;    
  }else if(strcasecmp(t_nodes["mdb2_dbtype"].c_str(),"ODBCMDB")==0)
  {
    m_mdbInfo_userInfo.m_dbType = ODBCMDB;
    m_mdbInfo_userInfo.m_hostName = t_nodes["mdb2_host"];
    m_mdbInfo_userInfo.m_dbServer = t_nodes["mdb2_servername"];
    m_mdbInfo_userInfo.m_user     = t_nodes["mdb2_user"];
    m_mdbInfo_userInfo.m_passwd   = t_nodes["mdb2_pass"];
    m_mdbInfo_userInfo.m_port     =  atoi(t_nodes["mdb2_port"].c_str());
  }else if(strcasecmp(t_nodes["mdb2_dbtype"].c_str(),"MDB")==0)
  {
    m_mdbInfo_userInfo.m_dbType = MDB;
    m_mdbInfo_userInfo.m_hostName = "";
    m_mdbInfo_userInfo.m_dbServer = t_nodes["mdb2_servername"];
    m_mdbInfo_userInfo.m_user     = "";
    m_mdbInfo_userInfo.m_passwd   = "";
    m_mdbInfo_userInfo.m_port     =  0;    
  }else //没有配置MDB2
  {
    m_mdbInfo_userInfo = m_mdbInfo; //用户资料MDB
  }
  
  
  //通道内全局参数
  t_path = SECTDELIM + m_appName + SECTDELIM+t_strChannel;
  //add by gaojf 2009-2-13 19:06 增加帐期通道
  if(!r_sysParam->getValue(t_path,"cycle_dbno",t_value))
  {
    #ifdef _DEBUG_
      cout<<t_path<<"\\cycle_dbno not defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }else
  {
    m_cycleDbNo = atoi(t_value.c_str());
  }
  
  //Added by Sunam 2009/9/19 11:17:47  按地市出账
  if(!r_sysParam->getValue(t_path,"eparchy_code",t_value))
  {
    #ifdef _DEBUG_
      cout<<t_path<<"\\eparchy_code not defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }else
  {
    m_eparchyCode = t_value;
  }
  
  if(!r_sysParam->getValue(t_path,"work_path",t_value))
  {
    #ifdef _DEBUG_
      cout<<t_path<<"\\work_path not defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }else
  {
    m_workPath = t_value.c_str();
  }
  if(!r_sysParam->getValue(t_path,"channel_type",m_channelType))
  {
    #ifdef _DEBUG_
      cout<<t_path<<"\\channel_type not defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  ///////////支持多个Altibase库而增加2009-1-17 23:39 gaojf
  t_nodes.clear();
  if (!r_sysParam->getValue(t_path, t_nodes)) 
  {
    #ifdef _DEBUG_
      cout<<"get common config param false!"<<t_path
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(strcasecmp(t_nodes["mdb_dbtype"].c_str(),"RMDB")==0)
  {
    m_mdbInfo_billInfo.m_dbType   = RMDB;
    m_mdbInfo_billInfo.m_hostName = t_nodes["mdb_host"];
    m_mdbInfo_billInfo.m_dbServer = t_nodes["mdb_servername"];
    m_mdbInfo_billInfo.m_user     = "";
    m_mdbInfo_billInfo.m_passwd   = "";
    m_mdbInfo_billInfo.m_port     =  0;    
  }else if(strcasecmp(t_nodes["mdb_dbtype"].c_str(),"ODBCMDB")==0)
  {
    m_mdbInfo_billInfo.m_dbType = ODBCMDB;
    m_mdbInfo_billInfo.m_hostName = t_nodes["mdb_host"];
    m_mdbInfo_billInfo.m_dbServer = t_nodes["mdb_servername"];
    m_mdbInfo_billInfo.m_user     = t_nodes["mdb_user"];
    m_mdbInfo_billInfo.m_passwd   = t_nodes["mdb_pass"];
    m_mdbInfo_billInfo.m_port     =  atoi(t_nodes["mdb_port"].c_str());
  }else if(strcasecmp(t_nodes["mdb_dbtype"].c_str(),"MDB")==0)
  {
    m_mdbInfo_billInfo.m_dbType = MDB;
    m_mdbInfo_billInfo.m_hostName = "";
    m_mdbInfo_billInfo.m_dbServer = t_nodes["mdb_servername"];
    m_mdbInfo_billInfo.m_user     = "";
    m_mdbInfo_billInfo.m_passwd   = "";
    m_mdbInfo_billInfo.m_port     =  0;    
  }
  if(strcasecmp(t_nodes["mdb2_dbtype"].c_str(),"RMDB")==0)
  {
    m_mdbInfo_userInfo.m_dbType   = RMDB;
    m_mdbInfo_userInfo.m_hostName = t_nodes["mdb2_host"];
    m_mdbInfo_userInfo.m_dbServer = t_nodes["mdb2_servername"];
    m_mdbInfo_userInfo.m_user     = "";
    m_mdbInfo_userInfo.m_passwd   = "";
    m_mdbInfo_userInfo.m_port     =  0;    
  }else if(strcasecmp(t_nodes["mdb2_dbtype"].c_str(),"ODBCMDB")==0)
  {
    m_mdbInfo_userInfo.m_dbType = ODBCMDB;
    m_mdbInfo_userInfo.m_hostName = t_nodes["mdb2_host"];
    m_mdbInfo_userInfo.m_dbServer = t_nodes["mdb2_servername"];
    m_mdbInfo_userInfo.m_user     = t_nodes["mdb2_user"];
    m_mdbInfo_userInfo.m_passwd   = t_nodes["mdb2_pass"];
    m_mdbInfo_userInfo.m_port     =  atoi(t_nodes["mdb2_port"].c_str());
  }else if(strcasecmp(t_nodes["mdb2_dbtype"].c_str(),"MDB")==0)
  {
    m_mdbInfo_userInfo.m_dbType = MDB;
    m_mdbInfo_userInfo.m_hostName = "";
    m_mdbInfo_userInfo.m_dbServer = t_nodes["mdb2_servername"];
    m_mdbInfo_userInfo.m_user     = "";
    m_mdbInfo_userInfo.m_passwd   = "";
    m_mdbInfo_userInfo.m_port     =  0;    
  }else //没有配置MDB2
  {
    //m_mdbInfo_userInfo = m_mdbInfo; //用户资料MDB
  }
  ///////////支持多个Altibase库而增加2009-1-17 23:39 gaojf
  
  //m_drFlag
  //m_ocsFlag
  //m_ocsOutPutCdrRule
  
  //3. 读取输入配置信息
  if(m_inputRuleMgr.initialize(r_sysParam,r_appName,r_channelNo,
                               m_cdrFmtConf.m_attribMacros)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_inputRuleMgr.initialize(r_sysParam,"<<r_appName<<","
          <<r_channelNo<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(m_inputRuleMgr.setCdrFormat(m_cdrFmtConf.m_cdrFormatList)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_inputRuleMgr.setCdrFormat(m_cdrFmtConf.m_cdrFormatList) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  
  //4. 读取输出配置信息
  if(m_outputRuleMgr.initialize(r_sysParam,r_appName,r_channelNo,
                                m_cdrFmtConf.m_attribMacros,this)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_outputRuleMgr.initialize(r_sysParam,"<<r_appName<<","
          <<r_channelNo<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(m_outputRuleMgr.setCdrFormat(m_cdrFmtConf.m_cdrFormatList)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_outputRuleMgr.setCdrFormat(m_cdrFmtConf.m_cdrFormatList) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  //5. 读取处理规则配置信息
  if(m_processRuleMgr.initialize(r_sysParam,r_appName,r_channelNo,
                                 m_cdrFmtConf.m_attribMacros,this)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_processRuleMgr.initialize(r_sysParam,"<<r_appName<<","
          <<r_channelNo<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }  
  //6. 读取其它配置信息
  
  
  if(valid()==false)
  {
    return false;
  }
  return true;
}
CDRJScript *  RateConfig::setScriptApplet(const string &r_script) 
{
  map<string,CDRJScript *>::iterator t_itr;
  
  t_itr=m_scriptApplets.find(r_script);
  if (t_itr == m_scriptApplets.end()) 
  {
    CDRJScript * t_js =  new CDRJScript(); 
    if ((!t_js->compileScript(r_script.c_str())) 
         ||(!t_js->good())) 
    {
      string errorMsg = CDRJScript::GetContainer()->getErrorInfo();
      #ifdef _DEBUG_
        cout<<"jsp:"<<r_script<<" is not good!"
            <<"errinfo:"<<errorMsg<<" "
            <<__FILE__<<__LINE__<<endl;
      #endif
      delete t_js;
      return NULL;
    }
    m_scriptApplets[r_script] = t_js;
  }
  return (m_scriptApplets[r_script]);
}

CDRJScript * RateConfig::getScriptApplet(const string &r_script) 
{
  map<string,CDRJScript *>::iterator t_itr;
  t_itr=m_scriptApplets.find(r_script);
  if (t_itr == m_scriptApplets.end()) 
  {
    #ifdef _DEBUG_
    cout<<"jsp:"<<r_script<<" not defined!"
        <<__FILE__<<__LINE__<<endl;
    #endif
    return NULL;
  }
  return (m_scriptApplets[r_script]);
}

bool RateConfig::valid()
{
  if(InoutputRuleMgr::isPath(m_workPath)==false)
  {
    #ifdef _DEBUG_
      cout<<"workpath:"<<m_workPath<<" is not exists!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

void RateConfig::dump() const
{
  cout<<"-----------一些全局信息----------------"<<endl;
  cout<<"m_appName    = "<<m_appName<<endl;
  cout<<"m_channelNo  = "<<m_channelNo<<endl;
  cout<<"m_dbUser     = "<<m_dbUser<<endl;
  cout<<"m_dbPassword = "<<m_dbPassword<<endl;
  cout<<"m_dbServName = "<<m_dbServName<<endl;
  cout<<"m_interval   = "<<m_interval<<endl;
  cout<<"m_provinceCode= "<<m_provinceCode<<endl;
  
  cout<<"m_doubleCycMode= "<<m_doubleCycMode<<endl;
  //cout<<"m_cycleDelayTime= "<<m_cycleDelayTime<<endl;
  cout<<"m_operatorType= "<<m_operatorType<<endl;
  cout<<"--------MDBINFO-----------------"<<endl;
  cout<<"   mdb_dbtype = "<<m_mdbInfo.m_dbType<<endl;
  cout<<"   m_dbServer = "<<m_mdbInfo.m_dbServer<<endl;
  cout<<"   m_hostName = "<<m_mdbInfo.m_hostName<<endl;
  cout<<"   m_user     = "<<m_mdbInfo.m_user<<endl;
  cout<<"   m_passwd   = "<<m_mdbInfo.m_passwd<<endl;
  cout<<"   m_port     = "<<m_mdbInfo.m_port<<endl;
  cout<<"--------------------------------"<<endl;
  cout<<"  1mdb_dbtype = "<<m_mdbInfo_billInfo.m_dbType<<endl;
  cout<<"  1m_dbServer = "<<m_mdbInfo_billInfo.m_dbServer<<endl;
  cout<<"  1m_hostName = "<<m_mdbInfo_billInfo.m_hostName<<endl;
  cout<<"  1m_user     = "<<m_mdbInfo_billInfo.m_user<<endl;
  cout<<"  1m_passwd   = "<<m_mdbInfo_billInfo.m_passwd<<endl;
  cout<<"  1m_port     = "<<m_mdbInfo_billInfo.m_port<<endl;

  cout<<"  2mdb_dbtype = "<<m_mdbInfo_userInfo.m_dbType<<endl;
  cout<<"  2m_dbServer = "<<m_mdbInfo_userInfo.m_dbServer<<endl;
  cout<<"  2m_hostName = "<<m_mdbInfo_userInfo.m_hostName<<endl;
  cout<<"  2m_user     = "<<m_mdbInfo_userInfo.m_user<<endl;
  cout<<"  2m_passwd   = "<<m_mdbInfo_userInfo.m_passwd<<endl;
  cout<<"  2m_port     = "<<m_mdbInfo_userInfo.m_port<<endl;
  
  cout<<"--------------------------------"<<endl;
  cout<<"--------宏定义------------------"<<endl;
  m_cdrFmtConf.m_attribMacros.dump();
  cout<<"---------话单格式定义-------------------"<<endl;
  for(vector<CdrFormat>::const_iterator t_cdrFmtItr=m_cdrFmtConf.m_cdrFormatList.begin();
      t_cdrFmtItr!=m_cdrFmtConf.m_cdrFormatList.end();t_cdrFmtItr++)
  {
    t_cdrFmtItr->dump();
  }
  cout<<"---------输入规则定义-------------------"<<endl;
  m_inputRuleMgr.dump();
  cout<<"---------输出规则定义-------------------"<<endl;
  m_outputRuleMgr.dump();
  cout<<"---------处理规则定义-------------------"<<endl;
  m_processRuleMgr.dump();
  cout<<"----------------------------------------"<<endl;
}
