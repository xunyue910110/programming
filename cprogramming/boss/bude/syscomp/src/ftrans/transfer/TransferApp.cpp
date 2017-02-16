

#include "TransferApp.h"
#include "TransferAppError.h"
#include "ftrans/FTController.h"

#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include "base/StringUtil.h"


#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>


#define TASK_COUNT "task_count"

const int SLEEP_TIME = 25 ;

TransferApp g_application;
Application * const theApp = &g_application;

int writeLog(const string & log) 
{
  return g_application.writeLog(log);
}

int writeAlert(const string & msg)
{
  return g_application.writeAlert(msg);
}

int writeState(const string & msg)
{
  g_application.updateStatus(msg.c_str());
  return 0;
}



bool TransferApp::initialization()
{
  setoptstr("c:");
  if ( !Application::initialization())
    return false;
  
  #ifndef _DEBUG_
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
  #endif

  m_objectName = "启动";
  updateStatus();


  // get sysparam
  char c[10];
  sprintf(c,"%d",m_channelNo);
  string section;
  map<string, string> paramMap;
  section = SECTDELIM;
  section = section + m_name + SECTDELIM + c;

  if ( !m_theSysParam->getValue(section, paramMap))  {
    m_errorCode = EAPPFRM_SYSVAL;
    writeAlert(section);
    return false;
  }

  if (paramMap[TASK_COUNT] == "")  {
    m_errorCode = EAPPFRM_SYSVAL;
    sprintf(m_errorMsg, "task_count param error!");
    writeAlert(m_errorMsg);
    return false;
  }

  m_TaskCount = atoi(paramMap[TASK_COUNT].c_str());

  string tempSection;
  char taskSection[256*4];
  
  m_FTControllers.clear();
  for (int i = 0; i < m_TaskCount; i++)
  {
    m_FTControllers.push_back(new FTController());
    tempSection = section + SECTDELIM + "task";
    sprintf(taskSection, "%s%d", tempSection.c_str(), i);

    (m_FTControllers[i])->writeLogHook = &(::writeLog);
    (m_FTControllers[i])->writeAlertHook =&(::writeAlert);
    (m_FTControllers[i])->writeStateHook =&(::writeState);
//    cout << "taskSection : " << taskSection << endl;

    if ((m_FTControllers[i])->initialize(m_theSysParam->getFileName(), taskSection) < 0)
    {
      m_errorCode = ERROR_FTCONTROLLER_INIT;
      sprintf(m_errorMsg, "task%d initialize failed!", i);
      writeAlert(m_errorMsg);
      return false;
    }

  }
  
  m_reserved = "";
  m_objectName = "启动";
  updateStatus();
  
  return true;
}


bool TransferApp::loopProcess()
{
  for (m_iTask = 0; m_iTask < m_TaskCount; m_iTask++)
  {
    theLoggerProcessID = theLogger->pBegin();

    m_rtn = m_FTControllers[m_iTask]->execute();
    if (m_rtn < 0)
    {
      m_errorCode = ERROR_FTCONTROLLER_EXEC;
      sprintf(m_errorMsg, " task%d execute failed!", m_iTask);
      writeAlert(m_errorMsg);   
    }

    theLogger->pEnd(theLoggerProcessID);
  }

  m_objectName = "--------";
  updateStatus();
  if (m_rtn <= 0)
    sleep(SLEEP_TIME);
  return Application::loopProcess();
}


bool TransferApp::end()
{
  int i;

  for( i = 0; i < m_TaskCount; i++) {
    (m_FTControllers[i])->end();
    delete m_FTControllers[i];
  }
   
  m_FTControllers.clear();
 
  Application::end();
  return  true;
}



int TransferApp::writeLog(const string & log)
{
  m_logstr = log;
  
  if (log.empty())
    return -1;

  string log_file_name,log_extend,log_remote,log_temp,source_name,target_name;
  int log_size;

  vector<string> lines,fields,props;
  vector<string>::iterator itr;
  int i;
     
  lines.clear(); StringUtil::split(string((log.c_str() + 4)),"\n" ,lines);
//  if (lines.size() != 2)        
//    return -1;
  for (i=0; i<lines.size(); ++i) { 
//    cout << "lines:" << lines[i] << endl;
    log_file_name = "";log_extend = "";log_size = -1;
    
    fields.clear(); StringUtil::split( lines[ i ], "," , fields);
    for (itr = fields.begin(); itr != fields.end(); ++itr) {
      props.clear(); StringUtil::split((*itr), "=" , props);
      if (props.size() != 2)
          return -1;
      if (props[0] == "file_name") log_file_name = props[1];
      else if (props[0] == "size") log_size = atoi(props[1].c_str());

      if (props[0] == "source_name") source_name = props[1];
      else if (props[0] == "target_name") target_name = props[1];

      log_remote = "";
      if (props[0] == "remote")
      {
        log_remote = props[1];
        if (log_remote.length() < 10)
        {
          sprintf(m_cTemp, "%d%s\0", m_iTask, log_remote.c_str());
          log_remote = m_cTemp;
        }
        log_extend += props[0] + "=" + log_remote + ",";
      }


      if ((log_remote != "") && (log_remote.length() > 6))
      {
        log_temp = "file:////";
        log_temp += log_remote;
        log_temp += log_file_name;
        log_file_name = log_temp;
      }
    }

    if (i==0) //第 1 行为输入信息
      theLogger->pInput(theLoggerProcessID,log_file_name,log_size,log_extend.c_str());
    if (i==1) //第 2 行为输入信息
      theLogger->pOutput(theLoggerProcessID,log_file_name,log_size,log_extend.c_str());
    if (i>1) //第 2 行以后为link信息 2006-6-14 15:13 by liuxiang
      theLogger->pLinkOut(theLoggerProcessID,source_name,target_name);
  }

  return 0;
}

int TransferApp::writeAlert(const string & msg)
{
  m_theErrorMessages->insert(m_errorCode, msg);
  Application::writeAlert();

  return 0;
}
