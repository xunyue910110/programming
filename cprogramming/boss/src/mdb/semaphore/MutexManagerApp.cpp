// ############################################
// Source file : MutexManagerApp.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "MutexManagerApp.h"

#include "appfrm/ErrorHandle.h"
#include "appfrm/VersionInfo.h"
#include "log4cpp/Logger.h"
#include "base/Directory.h"
#include "base/SysParam.h"
#include <fstream>

MutexManagerApp g_application;
Application * const theApp = &g_application;

MutexManagerApp::MutexManagerApp()
{
}

MutexManagerApp::~MutexManagerApp()
{
}

bool MutexManagerApp::initialization()
{
  setoptstr("c:");
  
  if (!Application::initialization())
      return false;
      
  #ifndef _DEBUG_ 
      set_asdaemon(true);
      set_onlyone(true);
      set_runinbkg(true);
  #endif
  
  m_objectName = "程序初始化";
  m_status = PROC_READY;
  updateStatus();
  if(getSysParam()==false)
  {
    #ifdef _DEBUG_
      cout<<"getSysParam() false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

bool MutexManagerApp::beforeLoop()
{
  if(m_mutexMgr.attachMutexShm(m_dbName,DoubleLock::MAX_MUTEX_NUM)==false)
  {
    #ifdef _DEBUG_
      cout<<" m_mutexMgr.attachMutexShm("<<m_dbName<<") false"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}


bool MutexManagerApp::recovery()
{
  return true;
}

bool MutexManagerApp::loopProcess()
{
  vector<ProcessMSrcInfo> t_PMsrcInfo;
  vector<ProcessMSrcInfo>::iterator t_itr;
  list<MutexWarnLog>::iterator t_wlogItr;
  bool                         t_flag;
  MutexWarnLog                 t_wlog;
  int                          t_wLevel;
  time_t                       t_nowTime;
  while(m_stop==false)
  {
    time(&t_nowTime);
    //1.将进程已退出且进程信息未注销的信息。
    t_PMsrcInfo.clear();
    if(m_mutexMgr.getExitProcessInfo(t_PMsrcInfo)==false)
    { //重新attach
      if(m_mutexMgr.attachMutexShm(m_dbName,DoubleLock::MAX_MUTEX_NUM)==false)
      {
        #ifdef _DEBUG_
          cout<<" m_mutexMgr.attachMutexShm("<<m_dbName<<") false"
              <<__FILE__<<__LINE__<<endl;
        #endif
        m_stop=true;
        return false;
      }else
        continue;
    }
    for(t_itr=t_PMsrcInfo.begin();t_itr!=t_PMsrcInfo.end();t_itr++)
    {
      //将该线程对应的资源释放回滚
      if(m_mutexMgr.rollbackSrc(*t_itr)==false)
      {
        #ifdef _DEBUG_
          cout<<" m_mutexMgr.rollbackSrc() false"
              <<__FILE__<<__LINE__<<endl;
        #endif
        m_stop=true;
        return false;
      }
      //1.2告警并记录日志信息  
      t_wlog.m_srcInfo   = *t_itr;
      t_wlog.m_warnLevel = 4;
      time(&t_wlog.m_warnTime);
      warning(t_wlog);
    }
    //1.3将删除的资源信息，历史告警记录清除
    for(t_wlogItr=m_warnLogInfo.begin();t_wlogItr!=m_warnLogInfo.end();)
    {
      t_flag=false;
      for(t_itr=t_PMsrcInfo.begin();t_itr!=t_PMsrcInfo.end();t_itr++)
      {
        if(*t_itr == t_wlogItr->m_srcInfo)
        {
          t_flag=true;break;
        }
      }
      if(t_flag==true)
      {
        t_wlogItr=m_warnLogInfo.erase(t_wlogItr);
      }else
      {
        t_wlogItr++;
      }
    }
    //2. 获取超时m_warnTimeOut的信息
    t_PMsrcInfo.clear();
    //if(m_mutexMgr.getEProcessInfo(m_warnTimeOut,t_PMsrcInfo)==false)
    if(m_mutexMgr.getEProcessInfo(0,t_PMsrcInfo)==false)
    { //重新attach
      if(m_mutexMgr.attachMutexShm(m_dbName,DoubleLock::MAX_MUTEX_NUM)==false)
      {
        #ifdef _DEBUG_
          cout<<" m_mutexMgr.attachMutexShm("<<m_dbName<<") false"
              <<__FILE__<<__LINE__<<endl;
        #endif
        m_stop=true;
        return false;
      }else continue;
    }
    for(t_itr=t_PMsrcInfo.begin();t_itr!=t_PMsrcInfo.end();t_itr++)
    {
      //设置t_wLevel
      getWarnLevel(*t_itr,t_wLevel);
      //2.1 将该线程对应资源告警出来并记录日志信息  
      //    告警级别未升级的不告警，原来没有的增加新记录
      //    告警级别升级的则将告警级别调高
      t_flag=false;
      for(t_wlogItr=m_warnLogInfo.begin();t_wlogItr!=m_warnLogInfo.end();t_wlogItr++)
      {
        if(*t_itr == t_wlogItr->m_srcInfo)
        {
          t_flag=true;
          if(t_wLevel>t_wlogItr->m_warnLevel)
          {
            t_wlogItr->m_warnLevel=t_wLevel;
            time(&(t_wlogItr->m_warnTime));
            warning(*t_wlogItr);
          }
          break;
        }
      }
      if(t_flag==false)
      { //增加告警，并记录日志信息
        t_wlog.m_srcInfo   = *t_itr;
        t_wlog.m_warnLevel = t_wLevel;
        time(&t_wlog.m_warnTime);
        warning(t_wlog);
        m_warnLogInfo.push_back(t_wlog);
      }
    }
    
    //3. 将不在t_PMsrcInfo的告警信息清除（告警解除）
    for(t_wlogItr=m_warnLogInfo.begin();t_wlogItr!=m_warnLogInfo.end();)
    {
      t_flag=false;
      for(t_itr=t_PMsrcInfo.begin();t_itr!=t_PMsrcInfo.end();t_itr++)
      {
        if(*t_itr == t_wlogItr->m_srcInfo )
        {
          t_flag=true;break;
        }
      }
      //告警已经解除一段事件后，清除该告警
      if(t_flag==false &&
         difftime(t_nowTime,t_wlogItr->m_warnTime)>m_warnTimeOut)
      { //告警解除
        t_wlogItr=m_warnLogInfo.erase(t_wlogItr);
      }else
      {
        t_wlogItr++;
      }
    }    
    sleep(1); //休眠1秒
  }
  return true;
}

bool MutexManagerApp::end()
{
  m_mutexMgr.detachMutexShm();
  return Application::end();
}

bool MutexManagerApp::getSysParam()
{
  string r_path,r_name,r_value;
  char   r_chPath[256+1];
  sprintf(r_chPath,"%c%s%c%d",SECTDELIM,m_name,SECTDELIM,m_channelNo);
  r_path=r_chPath;
  
  //数据库名
  r_name="DBNAME";
  if (!m_theSysParam->getValue(r_path, r_name, r_value))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else if(strlen(r_value.c_str())<=0)
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else
  {
    strcpy(m_dbName,r_value.c_str());
  }
  //告警时间
  r_name="WARNTIMEOUT";
  if (!m_theSysParam->getValue(r_path, r_name, r_value))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else if(atoi(r_value.c_str())<=0)
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else
  {
    m_warnTimeOut=atoi(r_value.c_str());
  }
  //严重告警时间
  r_name="SWARNTIMEOUT";
  if (!m_theSysParam->getValue(r_path, r_name, r_value))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else if(atoi(r_value.c_str())<=0)
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else
  {
    m_sWarnTimeOut=atoi(r_value.c_str());
  }  
  //强制释放时间
  r_name="DELTIMEOUT";
  if (!m_theSysParam->getValue(r_path, r_name, r_value))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else if(atoi(r_value.c_str())<=0)
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else
  {
    m_delTimeOut=atoi(r_value.c_str());
  }

  r_name="LOGFILE";
  if (!m_theSysParam->getValue(r_path, r_name, r_value))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL, r_path +
                        SECTDELIM + r_name);
    return false;
  }else 
  {
    strcpy(m_wlogFile,r_value.c_str());
  }
  
  return true;
}

void MutexManagerApp::warning(const MutexWarnLog &r_wlog)
{
  int  t_warnCode;
  const ProcessMSrcInfo& t_srcInfo=r_wlog.m_srcInfo;
  t_warnCode=EWARN_BASE+r_wlog.m_warnLevel;
  sprintf(m_chWarnInfo,"%d,%ld,%ld,%s,",t_warnCode,
           t_srcInfo.m_pid,t_srcInfo.m_thId,
           getLTime(t_srcInfo.m_pTime));
  for(int i=0;i<t_srcInfo.m_srcNum;i++)
  {
    const MsrcInfo& t_msrc=t_srcInfo.m_srcInfo[i];
    if(i>0)
    {
      sprintf(m_chWarnInfo+strlen(m_chWarnInfo),";%d|%d|%d|%d|%s",
              t_msrc.m_sid,t_msrc.m_rcount,t_msrc.m_wcount,t_msrc.m_wwait,
              getLTime(t_msrc.m_time));
    }else
    {
      sprintf(m_chWarnInfo+strlen(m_chWarnInfo),"%d|%d|%d|%d|%s",
              t_msrc.m_sid,t_msrc.m_rcount,t_msrc.m_wcount,t_msrc.m_wwait,
              getLTime(t_msrc.m_time));
    }
  }
  #ifdef _DEBUG_
    cout<<m_chWarnInfo<<endl;
  #endif
  //0.发出告警信息
  m_theErrorMessages->insert(t_warnCode,string(m_chWarnInfo));
  ofstream   t_outfile;
  //1.写方式打开日志文件
  t_outfile.open(m_wlogFile,ios::out|ios::app);
  if(!t_outfile)
  {
    return;
  }
  //2.输出告警日志信息
  t_outfile<<m_chWarnInfo<<endl;
  //3.关闭文件
  t_outfile.close();
  t_outfile.clear();
  return;
}

void MutexManagerApp::getWarnLevel(const ProcessMSrcInfo &r_srcInfo,int &r_wLevel)
{
  time_t  t_nowTime;
  time_t  t_minTime=0;
  int     t_outTime;
  time(&t_nowTime);
  for(int i=0;i<r_srcInfo.m_srcNum;i++)
  {
    if(i==0 || t_minTime>r_srcInfo.m_srcInfo[i].m_time)
    {
      t_minTime=r_srcInfo.m_srcInfo[i].m_time;
    }
  }
  t_outTime=difftime(t_nowTime,t_minTime);
  if(t_outTime>=m_delTimeOut)
  {
    r_wLevel = 3;
  }else if(t_outTime>=m_sWarnTimeOut)
  {
    r_wLevel = 2;
  }else 
  {
    r_wLevel = 1;
  }
  return;
}
char* MutexManagerApp::getLTime(const time_t &r_time)
{
  static char s_time[14+1];
  struct tm t_tm;
  localtime_r(&r_time,&t_tm);
  sprintf(s_time,"%04d%02d%02d%02d%02d%02d",
             t_tm.tm_year+1900,
             t_tm.tm_mon+1,
             t_tm.tm_mday,
             t_tm.tm_hour,
             t_tm.tm_min,
             t_tm.tm_sec);
  return s_time;
}


