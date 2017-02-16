#include "FilterConfig.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "FilterError.h"

#include <iostream>
#include <algorithm>


FilterConfig::FilterConfig()
{
//  m_drFlag    = false;
//  m_ocsFlag   = false;
//  m_ocsOutPutCdrRule=NULL;
  m_smsProcessMethod.clear();
  m_resetDelayTimeMap.clear();
  m_specialOutField.clear();
}
FilterConfig::~FilterConfig()
{
  m_cdrFmtConf.m_cdrFormatList.clear();
}
bool FilterConfig::getSysParam(SysParam *r_sysParam,const char * r_appName,
                             const int &r_channelNo)
{
  string t_rootpath,t_path,t_subpath,t_value;
  vector<string>  t_subNodeList;
  map<string,string> t_nodes,t_smsprocessMethod,t_resetDelayTimeMap,t_specialOutField;
  const CdrFormat *t_Format;
  map<string,string>::iterator r_itr;

  m_appName  =r_appName;
  m_channelNo=r_channelNo;
  
  //1. 读取一些全局定义信息
  t_path = SECTDELIM + m_appName + SECTDELIM + "common";
  t_nodes.clear();
  if (!r_sysParam->getValue(t_path, t_nodes))
  {
    #ifdef _DEBUG_
      cout<<"get common config param false!"<<t_path
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"get common config param false! " + t_path);
    return false;
  }

	 m_getAHomeMethod         = atoi(t_nodes["get_a_home_method"          ].c_str());
	 m_getAVisitMethod        = atoi(t_nodes["get_a_visit_method"         ].c_str());
	 m_getGprsAVisitMethod    = atoi(t_nodes["get_gprs_a_visit_method"    ].c_str());
	 m_getBHomeMethod         = atoi(t_nodes["get_b_home_method"          ].c_str());
	 m_getBVisitMethod        = atoi(t_nodes["get_b_visit_method"        ].c_str());
	 m_getRoAHomeMethod       = atoi(t_nodes["get_ro_a_home_method"      ].c_str());
	 m_getRoAVisitMethod      = atoi(t_nodes["get_ro_a_visit_method"      ].c_str());
	 m_getRoBHomeMethod       = atoi(t_nodes["get_ro_b_home_method"      ].c_str());
	 m_getRoBVisitMethod      = atoi(t_nodes["get_ro_b_visit_method"      ].c_str());
	 m_getProvEdgeRoamMethod      = atoi(t_nodes["get_prov_edgeroam_method"    ].c_str());
	 m_getRoamOutEdgeRoamMethod   = atoi(t_nodes["get_roamout_edgeroam_method" ].c_str());

   m_default_bindMethod         = atoi(t_nodes["default_bind_method"         ].c_str());
   m_getLocalMmsMethod          = atoi(t_nodes["get_local_mms_b_method"      ].c_str());
   m_statusAuditMethod          = atoi(t_nodes["get_status_audit_method"     ].c_str());
   m_svcAllowance               = atoi(t_nodes["get_svc_allowance_method"    ].c_str());
   m_delayTime                  = atoi(t_nodes["delay_time"    ].c_str());

   //add by shenglj 2009-08-12 20:37:01
   m_resetTimeSourceVector.clear();
   StringUtil::split(t_nodes["reset_begin_time_source"].c_str(),",",m_resetTimeSourceVector,true);
   m_resetTimeMethod            = atoi(t_nodes["reset_begin_time_method"     ].c_str());

   m_errorPath                  = t_nodes["file_error_path"];
   m_default_separate           = t_nodes["default_separate"][0];
	 m_file_sort_method           = atoi(t_nodes["sort_method"   ].c_str());
	 m_isVerifyFee                = t_nodes["isVerifyFee"][0];

  m_provinceCode              = t_nodes["province_code"];
  m_areaCode                  = t_nodes["area_code"];
  m_operatorType              = atoi(t_nodes["operator_type"   ].c_str());
  m_dbServName                = t_nodes["server_name"];
  m_dbUser                    = t_nodes["user_name"];
  m_dbPassword                = t_nodes["password"];
	m_interval                  = atoi(t_nodes["db_interval"].c_str());

  m_mdbInfo.m_dbType          = T_DATABASETYPE(atoi(t_nodes["mdb_type"].c_str()));
  m_mdbInfo.m_dbServer        = t_nodes["mdb_servername"];
  m_mdbInfo.m_hostName        = t_nodes["mdb_hostname"];
	m_mdbInfo.m_port            = atoi(t_nodes["mdb_port"].c_str());
	m_mdbInfo.m_user            = t_nodes["mdb_user"];
	m_mdbInfo.m_passwd          = t_nodes["mdb_password"];

  //获取短信处理配置
  t_smsprocessMethod.clear();
  t_subpath = t_path + SECTDELIM + "sm_process_method"; 
  if (!r_sysParam->getValue(t_subpath, t_smsprocessMethod)) 
  {
    #ifdef _DEBUG_
      cout<<"get sm_process_method config param false!"<<t_subpath
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"get sm_process_method config param false! " + t_subpath);
    return false;
  }
  for(r_itr =t_smsprocessMethod.begin();r_itr!=t_smsprocessMethod.end();r_itr++)
  {
     m_smsProcessMethod[r_itr->first]=atoi((r_itr->second).c_str());
  }

  //获取各类数据源延时时间
  t_resetDelayTimeMap.clear();
  t_subpath = t_path + SECTDELIM + "reset_delay_time"; 
  if (!r_sysParam->getValue(t_subpath, t_resetDelayTimeMap)) 
  {
    #ifdef _DEBUG_
      cout<<"get resetDelayTime config param false!"<<t_subpath
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"get resetDelayTime config param false! " + t_subpath);
    return false;
  }
  for(r_itr =t_resetDelayTimeMap.begin();r_itr!=t_resetDelayTimeMap.end();r_itr++)
  {
     m_resetDelayTimeMap[r_itr->first]=atoi((r_itr->second).c_str());
  }

 //2. 读取宏定义及话单格式配置信息
  t_path = SECTDELIM + m_appName;
  if(m_cdrFmtConf.getSysParam(r_sysParam,t_path.c_str())==false)
  {
    #ifdef _DEBUG_
      cout<<"m_cdrFmtConf.getSysParam(r_sysParam,"<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_cdrFmtConf.getSysParam(r_sysParam,"+t_path+") false!");
    return false;
  }

  //获取通道指定话单格式
  t_specialOutField.clear();
  t_path = SECTDELIM + m_appName + SECTDELIM + "common";
  t_subpath = t_path + SECTDELIM + "special_output_field"; 
  if (!r_sysParam->getValue(t_subpath, t_specialOutField))
  {
    #ifdef _DEBUG_
      cout<<"get special out field config param false!"<<t_subpath
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"get special out field config param false! " + t_subpath);
    return false;
  }
  for(r_itr =t_specialOutField.begin();r_itr!=t_specialOutField.end();r_itr++)
  {
    if(FilterRuleMgr::setCdrFormat(r_itr->second,m_cdrFmtConf.m_cdrFormatList,t_Format)==false)
    {
      #ifdef _DEBUG_
        cout<<"special_output_field ："<<r_itr->second<<" no defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"special_output_field ：" + r_itr->second + " no defined!");
      return false;
    }
    m_specialOutField[r_itr->first]=t_Format;
    //m_specialOutField.insert(map<string,const CdrFormat*>::value_type(r_itr->first,t_Format));
  }

  //3. 读取输入配置信息
  if(m_inputRuleMgr.initialize(r_sysParam,r_appName,r_channelNo,m_cdrFmtConf.m_cdrFormatList,
                               m_cdrFmtConf.m_attribMacros,this)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_inputRuleMgr.initialize(r_sysParam,"<<r_appName<<","
          <<r_channelNo<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_inputRuleMgr.initialize(r_sysParam,"+m_appName+","+StringUtil::toString(m_channelNo)+") false!");
    return false;
  }
 
  //4. 读取输出配置信息
  if(m_outputRuleMgr.initialize(r_sysParam,r_appName,r_channelNo,m_cdrFmtConf.m_cdrFormatList,
                                m_cdrFmtConf.m_attribMacros,this)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_outputRuleMgr.initialize(r_sysParam,"<<r_appName<<" -c"
          <<r_channelNo<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_outputRuleMgr.initialize(r_sysParam,"+m_appName+" -c"+StringUtil::toString(m_channelNo)+") false!");
    return false;
  }
  //5. 读取排重字段规则配置信息
  if(m_dividRuleMgr.initialize(r_sysParam,r_appName,m_cdrFmtConf.m_cdrFormatList,m_cdrFmtConf.m_attribMacros,this)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_dividRuleMgr.initialize(r_sysParam,"<<r_appName<<",) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_dividRuleMgr.initialize(r_sysParam,"+m_appName+",) false!");
    return false;
  }
  //6. 读取通道提取规则
  if(m_channelRuleMgr.initialize(r_sysParam,r_appName,m_cdrFmtConf.m_attribMacros)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_channelRuleMgr.initialize(r_sysParam,"<<r_appName<<",) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_channelRuleMgr.initialize(r_sysParam,"+m_appName+",) false!");
    return false;
  }
  //7. 读取处理规则配置信息
  if(m_processorRuleMgr.initialize(&m_inputRuleMgr)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_processorRuleMgr.initialize(r_sysParam,"<<r_appName<<",) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_processorRuleMgr.initialize(r_sysParam,"+m_appName+",) false!");
    return false;
  }

  if(valid()==false)
  {
    return false;
  }
  return true;
}


CDRJScript *  FilterConfig::setScriptApplet(const string &r_script) 
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

CDRJScript * FilterConfig::getScriptApplet(const string &r_script) 
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

bool FilterConfig::valid()
{
  if(FilterRuleMgr::isPath(m_errorPath)==false)
  {
    #ifdef _DEBUG_
      cout<<"validpath:"<<m_errorPath<<" is not exists!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
if(m_inputRuleMgr.valid()==false) return false;
if(m_outputRuleMgr.valid()==false) return false;

  return true;
}

void FilterConfig::dump()
{
  cout<<"-----------一些全局信息----------------"<<endl;
  cout<<"m_appName                  = "<<m_appName<<endl;
  cout<<"m_channelNo                = "<<m_channelNo<<endl;

  cout<<"m_getAHomeMethod           = "<<m_getAHomeMethod<<endl;
  cout<<"m_getAVisitMethod          = "<<m_getAVisitMethod<<endl;
  cout<<"m_getGprsAVisitMethod      = "<<m_getGprsAVisitMethod<<endl;
  cout<<"m_getBHomeMethod           = "<<m_getBHomeMethod<<endl;
  cout<<"m_getBVisitMethod          = "<<m_getBVisitMethod<<endl;
  cout<<"m_getRoAHomeMethod         = "<<m_getRoAHomeMethod<<endl;
  cout<<"m_getRoAVisitMethod        = "<<m_getRoAVisitMethod<<endl;
  cout<<"m_getRoBHomeMethod         = "<<m_getRoBHomeMethod<<endl;
  cout<<"m_getRoBVisitMethod        = "<<m_getRoBVisitMethod<<endl;
  cout<<"m_getProvEdgeRoamMethod    = "<<m_getProvEdgeRoamMethod<<endl;
  cout<<"m_getRoamOutEdgeRoamMethod = "<<m_getRoamOutEdgeRoamMethod<<endl;

  cout<<"m_default_bindMethod       = "<<m_default_bindMethod<<endl;
  cout<<"m_getLocalMmsMethod        = "<<m_getLocalMmsMethod<<endl;
  cout<<"m_statusAuditMethod        = "<<m_statusAuditMethod<<endl;
  cout<<"m_svcAllowance             = "<<m_svcAllowance<<endl;
  cout<<"m_delayTime                = "<<m_delayTime<<endl;
  cout<<"m_errorPath                = "<<m_errorPath<<endl;
  cout<<"m_default_separate         = "<<m_default_separate<<endl;
  cout<<"m_file_sort_method         = "<<m_file_sort_method<<endl;
  cout<<"m_isVerifyFee              = "<<m_isVerifyFee<<endl<<endl;

  for(vector<string>::iterator r_itrstr=m_resetTimeSourceVector.begin();
                               r_itrstr!=m_resetTimeSourceVector.end();++r_itrstr)
  {
      cout<<"开始时间需要变更的数据源: "<<*r_itrstr<<endl;
  }
  cout<<"m_resetTimeMethod          = "<<m_resetTimeMethod<<endl;
  for(map<string,int>::iterator r_itr1 =m_resetDelayTimeMap.begin();r_itr1!=m_resetDelayTimeMap.end();r_itr1++)
  {
     cout<<"处理数据源: "<<r_itr1->first<<"    秒: "<<r_itr1->second<<endl;
  }

  cout<<"m_dbUser      = "<<m_dbUser<<endl;
  cout<<"m_dbPassword  = "<<m_dbPassword<<endl;
  cout<<"m_dbServName  = "<<m_dbServName<<endl;
  cout<<"m_provinceCode= "<<m_provinceCode<<endl;
  cout<<"m_areaCode    = "<<m_areaCode<<endl;
  cout<<"m_operatorType= "<<m_operatorType<<endl;
  
  cout<<"m_mdbInfo  = "<<m_mdbInfo<<endl;


  for(map<string,int>::iterator r_itr =m_smsProcessMethod.begin();r_itr!=m_smsProcessMethod.end();r_itr++)
  {
     cout<<"短信数据源: "<<r_itr->first<<"    处理方法: "<<r_itr->second<<endl;
  }

  cout<<"--------宏定义------------------"<<endl;
  //m_cdrFmtConf.dump();
  cout<<"---------输入规则定义-------------------"<<endl;
  m_inputRuleMgr.dump();
  cout<<"---------输出规则定义-------------------"<<endl;
  m_outputRuleMgr.dump();
  cout<<"---------排重字段提取规则定义-------------------"<<endl;
  m_dividRuleMgr.dump();
  cout<<"---------通道字段提取规则定义-------------------"<<endl;
  m_channelRuleMgr.dump();
  cout<<"---------处理规则定义-------------------"<<endl;
  m_processorRuleMgr.dump();
  cout<<"----------------------------------------"<<endl;
}
