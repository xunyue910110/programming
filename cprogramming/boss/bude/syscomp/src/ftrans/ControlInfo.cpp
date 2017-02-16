#include "ControlInfo.h"
#include "base/SysParam.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define PROTOCAL_TYPE     "protocal_type"
#define ACTION            "action"
#define LINK_TYPE          "link_type"
#define COMPRESS          "compress"
#define UNCOMPRESS        "uncompress"
#define CONNECT_REMOTE    "connect_remote"
#define CONNECT_PORT      "connect_port"
#define CONNECT_USER      "connect_user"
#define CONNECT_PASSWD    "connect_passwd"
#define SWAP_PATH         "swap_path"
#define SOURCE_COUNT      "source_count"

#define SOURCE_PATH       "source_path"
#define BACKUP_PATH       "backup_path"
#define LIST_IN_SOURCE    "list_in_source"
#define PATTERN_COUNT     "pattern_count"
#define PATTERN           "pattern"
#define CHECK_INTERVAL    "check_interval"
#define TARGET_PATH       "target_path"
#define DISTRIBUTE_PATH   "distribute_path"

#define CHANGE_NAME          "change_name"

#define SOURCE_PATH       "source_path"
#define FINISH_TAG_FILE   "finish_tag_file"   //新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30

const char DIR_SEPARATER_CHAR = '/';


ControlInfo_get_source_pattern::ControlInfo_get_source_pattern() 
{
  pattern = "*";
  check_interval = 1200;
  target_path = "";
  distribute_path ="";
  m_queue.clear();
}

//==============================================================================
// ControlInfo_get_source_pattern::checkTimeOut
//
/**
   超时判断
*/
//==============================================================================
bool ControlInfo_get_source_pattern::checkTimeOut()
{
    
  time_t currentTime = time(NULL);

  if(m_lastTime >= currentTime - check_interval)
    return false;
  else
//    m_lastTime += check_interval;
    m_lastTime = currentTime;


  if (check_interval <=0)
    return false;
    
  return true;
}


void ControlInfo_get_source_pattern::resetTime()
{
  m_lastTime = time(NULL);
}


char* ControlInfo_get_source_pattern::dump(char* buf) const
{
  buf += sprintf(buf, "pattern : %s\n", pattern.c_str());
  buf += sprintf(buf, "check_interval : %d\n", check_interval);
  buf += sprintf(buf, "target_path : %s\n", target_path.c_str());
  buf += sprintf(buf, "distribute_path : %s\n", distribute_path.c_str());

  return buf;
}


ControlInfo_get_source::ControlInfo_get_source() 
{
  source_path = "";
  backup_path = "";
  list_in_source = "no";
  finish_tag_file = "";//新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
  pattern_count = 0;
  patterns.clear();
  m_queue.clear();
}


ControlInfo_get_source::~ControlInfo_get_source() 
{
  vector<ControlInfo_get_source_pattern *>::iterator patternIter;
  
  for(patternIter = patterns.begin(); 
      patternIter != patterns.end(); ++patternIter)
  {
    delete (*patternIter);
  }
  patterns.clear();
  
}


char* ControlInfo_get_source::dump(char* buf) const
{

  vector<ControlInfo_get_source_pattern *>::const_iterator patternIter;
  
  buf += sprintf(buf, "source_path : %s\n", source_path.c_str());
  buf += sprintf(buf, "backup_path : %s\n", backup_path.c_str());
  buf += sprintf(buf, "list_in_source : %s\n", list_in_source.c_str());
  buf += sprintf(buf, "pattern_count : %d\n", pattern_count);
    
  buf += sprintf(buf, "<<<-------- Pattern --------->>>\n");
  for (patternIter = patterns.begin(); 
       patternIter != patterns.end(); ++patternIter) 
  { 
    buf = (*patternIter)->dump(buf);
  }

  return buf;
}


ControlInfo_put_source::ControlInfo_put_source() 
{
  check_interval =1200 ;
  source_path = "";
  backup_path = "";
  target_path = "";
}


char* ControlInfo_put_source::dump(char* buf) const
{ 
  buf += sprintf(buf, "source_path : %s\n", source_path.c_str());
  buf += sprintf(buf, "backup_path : %s\n", backup_path.c_str());
  buf += sprintf(buf, "target_path : %s\n", target_path.c_str()); 
  buf += sprintf(buf, "check_interval : %d\n", check_interval);

  return buf;
}


//==============================================================================
// ControlInfo_put_source::checkTimeOut
//
/**
   超时检查
*/
//==============================================================================
bool ControlInfo_put_source::checkTimeOut()
{
   
  time_t currentTime = time(NULL);

  if(m_lastTime >= currentTime - check_interval)
    return false;
  else
//    m_lastTime += check_interval;
    m_lastTime = currentTime;

  if (check_interval <=0)
    return false;

  return true;
}


void ControlInfo_put_source::resetTime()
{
  m_lastTime = time(NULL);
}


ControlInfo::ControlInfo() {
  protocal_type = "FTP";
  action = "get";
  compress = "";
  uncompress = "";
  connect_remote = "";
  connect_port = "21";
  connect_user = "";
  connect_passwd = "";
  swap_path = "";
  source_count = 0;
  get_sources.clear();
  put_sources.clear();

  m_controlSysParam = NULL;
  m_param.clear();
}


ControlInfo::~ControlInfo() 
{
  vector<ControlInfo_get_source *>::iterator getIter;
  vector<ControlInfo_put_source *>::iterator putIter;
 
  for(getIter = get_sources.begin(); getIter != get_sources.end(); ++getIter)
  {
    delete (*getIter);
  }
  get_sources.clear();

  for(putIter = put_sources.begin(); putIter != put_sources.end(); ++putIter)
  {
    delete (*putIter);
  }
  get_sources.clear();
  
  if (m_controlSysParam != NULL) {
    m_controlSysParam->closeSection();
    delete m_controlSysParam;
    m_controlSysParam = NULL;
  }
  
  m_param.clear();
}
  

//==============================================================================
// ControlInfo::initialize
//
/**
   初始化
*/
//==============================================================================
int ControlInfo::initialize(const string& config_file,
                            const string& config_section ) 
{

  //TODO:read configure infomation from config file
  if ((m_controlSysParam = new SysParam()) == 0) {
    return -1;
  } 
________________________________________________________________________________
  m_controlSysParam->initialize((char *)config_file.c_str());
________________________________________________________________________________
  if ( !getSysParam(config_section)) {
    return -1;
  }
________________________________________________________________________________
  return 0;
}


//==============================================================================
// ControlInfo::getSysParam
//
/**
   取得配置信息
   
*/
//==============================================================================
bool ControlInfo::getSysParam(const string& section_path)
{
 

    vector<string> paramVector;
    m_controlSysParam->getValue("\\SYSTEM", "SYS_LOCKPATH", paramVector);
________________________________________________________________________________
  if (!m_controlSysParam->getValue(section_path , m_param)) {
    return false;
  }
________________________________________________________________________________
  if (  (m_param[CONNECT_REMOTE].empty())
      || (m_param[CONNECT_USER].empty())
      || (m_param[CONNECT_PASSWD].empty())
      || (m_param[SOURCE_COUNT].empty()) )
  {
      return false;
  }
________________________________________________________________________________
  if (   (m_param[PROTOCAL_TYPE] != "FTP") 
    && (m_param[PROTOCAL_TYPE] != "DTA")
    && ((m_param[PROTOCAL_TYPE] != "FTAM") 
    && (m_param[PROTOCAL_TYPE] != "FILE")) )
  {
      return false;
  }
________________________________________________________________________________
  if (   (m_param[ACTION] != "get") 
      && (m_param[ACTION] != "put")
      && (! m_param[ACTION].empty()) )
  {
      return false;
  }
________________________________________________________________________________
  protocal_type = m_param[PROTOCAL_TYPE];

  if (! m_param[ACTION].empty())
    action = m_param[ACTION];

  link_type = m_param[LINK_TYPE];

  if (! m_param[COMPRESS].empty())
    compress = m_param[COMPRESS];    
  
  if (! m_param[UNCOMPRESS].empty())
    uncompress = m_param[UNCOMPRESS];        
  
  connect_remote = m_param[CONNECT_REMOTE];

  if (! m_param[CONNECT_PORT].empty())    
    connect_port = m_param[CONNECT_PORT];
  
  connect_user = m_param[CONNECT_USER];

  connect_passwd = m_param[CONNECT_PASSWD];

  if (! m_param[SWAP_PATH].empty()) {
    swap_path = m_param[SWAP_PATH];
    if (swap_path[swap_path.length() - 1] != DIR_SEPARATER_CHAR) {
      swap_path += DIR_SEPARATER_CHAR;
    }
  }
    
  if ( (source_count = atoi(m_param[SOURCE_COUNT].c_str())) == 0 ) {
    return false;
  }
  
  if ( action == "get" ) {
    if (! getGetSubSysParam(section_path)) {
        return false;
    }   
  }
  // action == "put"
  else { 
    if (! getPutSubSysParam(section_path)) {
        return false;
    }   
  }
      
  return true;
}


//==============================================================================
// ControlInfo::getGetSubSysParam
//
/**
   取得下载子系统配置信息
*/
//==============================================================================
bool ControlInfo::getGetSubSysParam(const string& section_path)
{
  int length;
  string tempSection;
  char subGetSection[1000];
  map<string, string> getParamMap;
  
  get_sources.clear();
  for(int i = 0; i < source_count; i++)
  {
    get_sources.push_back(new ControlInfo_get_source());
    tempSection = section_path + SECTDELIM + "source";
    sprintf(subGetSection, "%s%d", tempSection.c_str(), i);

    getParamMap.clear();
    if (! m_controlSysParam->getValue(subGetSection, getParamMap)) {
      return false;
    }
    
    if (  (getParamMap[SOURCE_PATH].empty())
        || (getParamMap[PATTERN_COUNT].empty()) )
    {
      return false;
    }

    if (   (m_param[LIST_IN_SOURCE] != "yes") 
        && (m_param[LIST_IN_SOURCE] != "no")
        && (! m_param[LIST_IN_SOURCE].empty()) )
    {
      return false;
    }
    //新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    if (    (protocal_type != "FILE")
          && (!getParamMap[FINISH_TAG_FILE].empty()) )
    {
        //仅支持FILE方式
        return false;             
    }
    get_sources[i]->source_path = getParamMap[SOURCE_PATH];
    length = get_sources[i]->source_path.length();
    if (get_sources[i]->source_path[length - 1] != DIR_SEPARATER_CHAR) {
      get_sources[i]->source_path += DIR_SEPARATER_CHAR;
    }

    if (! getParamMap[BACKUP_PATH].empty()) {
      get_sources[i]->backup_path = getParamMap[BACKUP_PATH];
      length = get_sources[i]->backup_path.length();
      if (get_sources[i]->backup_path[length - 1] != DIR_SEPARATER_CHAR) {
        get_sources[i]->backup_path += DIR_SEPARATER_CHAR;
      }     
    }

    if (! getParamMap[CHANGE_NAME].empty()) {
      get_sources[i]->change_name = getParamMap[CHANGE_NAME];
      if (!NAMEJScript::Initialize(get_sources[0]->m_name)) 
        return false;

      if(!(get_sources[i]->m_js = new NAMEJScript(getParamMap[CHANGE_NAME].c_str())) ||
         !get_sources[i]->m_js->good()) 
        return false;
    }

    get_sources[i]->list_in_source = getParamMap[LIST_IN_SOURCE];
    get_sources[i]->pattern_count=atoi(getParamMap[PATTERN_COUNT].c_str());

    //新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    if (! getParamMap[FINISH_TAG_FILE].empty()) {
        get_sources[i]->finish_tag_file =  getParamMap[FINISH_TAG_FILE];
        get_sources[i]->list_in_source = "yes"; // 文件判断方式list_in_source置yes    
    }
       
    if (get_sources[i]->pattern_count == 0) {
      return false;
    }

    if (!getGetPatternSubSysParam(subGetSection, get_sources[i])) {
        return false;
    }       
  }
      
  return true;
}


//==============================================================================
// ControlInfo::getPutSubSysParam
//
/**
   取得上传子系统配置信息
*/
//==============================================================================
bool ControlInfo::getPutSubSysParam(const string& section_path)
{
  int length;
  string tempSection;
  char subPutSection[1000];
  map<string, string> putParamMap;    
  
  put_sources.clear();
  for(int i = 0; i < source_count; i++)
  {
    put_sources.push_back(new ControlInfo_put_source());

    tempSection = section_path + SECTDELIM + "source";
    sprintf(subPutSection, "%s%d", tempSection.c_str(), i);
    
    putParamMap.clear();
    if (!m_controlSysParam->getValue(subPutSection, putParamMap)) {
      return false;
    }

    if (  (putParamMap[SOURCE_PATH].empty())
        || (putParamMap[TARGET_PATH].empty()) )
    {
      return false;
    }
    
    put_sources[i]->source_path = putParamMap[SOURCE_PATH];
    length = put_sources[i]->source_path.length();
    if (put_sources[i]->source_path[length - 1] != DIR_SEPARATER_CHAR) {
      put_sources[i]->source_path += DIR_SEPARATER_CHAR;
    }
    
    if (! putParamMap[BACKUP_PATH].empty()) {
      put_sources[i]->backup_path = putParamMap[BACKUP_PATH];
      length = put_sources[i]->backup_path.length();
      if (put_sources[i]->backup_path[length - 1] != DIR_SEPARATER_CHAR) {
        put_sources[i]->backup_path += DIR_SEPARATER_CHAR;
      }     
    }

    if (! putParamMap[CHECK_INTERVAL].empty()) {
      put_sources[i]->check_interval = 
        atoi(putParamMap[CHECK_INTERVAL].c_str());  
    }
        
    put_sources[i]->target_path = putParamMap[TARGET_PATH];
    length = put_sources[i]->target_path.length();
    if (put_sources[i]->target_path[length - 1] != DIR_SEPARATER_CHAR) {
      put_sources[i]->target_path += DIR_SEPARATER_CHAR;
    }

    put_sources[i]->resetTime();      
  } 
      
  return true;
}


//==============================================================================
// ControlInfo::getGetPatternSubSysParam
//
/**
   取得下载子系统内模式子系统配置信息
*/
//==============================================================================
bool ControlInfo::getGetPatternSubSysParam(const string& section_path,
       ControlInfo_get_source * get_source)
{
  int length;
  string tempSection;
  char subPatternSection[1000]; 
  map<string, string> patternParamMap;  
  ControlInfo_get_source_pattern* get_source_pattern = 0;   
  
  get_source->patterns.clear();
  for (int i = 0; i < get_source->pattern_count; i++)
  {
    get_source->patterns.push_back(new ControlInfo_get_source_pattern());
    get_source_pattern = get_source->patterns[i];   

    tempSection = section_path + SECTDELIM + "pattern";
    sprintf(subPatternSection, "%s%d", tempSection.c_str(), i);
        
    patternParamMap.clear();
    if (!m_controlSysParam->getValue(subPatternSection, patternParamMap)) {
      return false;
    }

    if (patternParamMap[TARGET_PATH].empty()) {
      return false;
    }   
    
    if (! patternParamMap[PATTERN].empty()) {
      get_source_pattern->pattern = patternParamMap[PATTERN];
    } 

    if (! patternParamMap[CHECK_INTERVAL].empty()) {
      get_source_pattern->check_interval = 
        atoi(patternParamMap[CHECK_INTERVAL].c_str());
    } 

    get_source_pattern->target_path = patternParamMap[TARGET_PATH];
    length = get_source_pattern->target_path.length();
    if (get_source_pattern->target_path[length - 1] != DIR_SEPARATER_CHAR) {
      get_source_pattern->target_path += DIR_SEPARATER_CHAR;
    }

    if (! patternParamMap[DISTRIBUTE_PATH].empty()) {
      get_source_pattern->distribute_path = patternParamMap[DISTRIBUTE_PATH];
      length = get_source_pattern->distribute_path.length();
      if (get_source_pattern->distribute_path[length - 1] 
        != DIR_SEPARATER_CHAR) 
      {
        get_source_pattern->distribute_path += DIR_SEPARATER_CHAR;
      }
    } 
    
    get_source_pattern->resetTime();                        
  }
  
  return true;
} 


char* ControlInfo::dump(char* buf) const
{
  vector<ControlInfo_get_source *>::const_iterator getIter;
  vector<ControlInfo_put_source *>::const_iterator putIter;
  
  buf += sprintf(buf, "<<<-------- ControlInfo --------->>>\n");  
  
  buf += sprintf(buf, "protocal_type : %s\n", protocal_type.c_str());
  buf += sprintf(buf, "action : %s\n", action.c_str());
  buf += sprintf(buf, "compress : %s\n", compress.c_str());
  buf += sprintf(buf, "uncompress : %s\n", uncompress.c_str());
  buf += sprintf(buf, "connect_remote : %s\n", connect_remote.c_str());
  buf += sprintf(buf, "connect_port : %s\n", connect_port.c_str());
  buf += sprintf(buf, "connect_user : %s\n", connect_user.c_str());
  buf += sprintf(buf, "connect_passwd : %s\n", connect_passwd.c_str());
  buf += sprintf(buf, "swap_path : %s\n", swap_path.c_str());
    
  buf += sprintf(buf, "source_count : %d\n", source_count);
    
  for (getIter = get_sources.begin(); 
       getIter != get_sources.end(); ++getIter) 
  { 
    buf += sprintf(buf, "<<<-------- Source --------->>>\n");
    buf = (*getIter)->dump(buf);
  }

  for (putIter = put_sources.begin(); 
       putIter != put_sources.end(); ++putIter) 
  { 
    buf += sprintf(buf, "<<<-------- Source --------->>>\n");
    buf = (*putIter)->dump(buf);
  }
  
  return buf;
}
