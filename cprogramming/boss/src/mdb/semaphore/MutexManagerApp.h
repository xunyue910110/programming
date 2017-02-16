// ############################################
// Source file : MutexManagerApp.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef _MUTEXMANAGERAPP_H_INCLUDE_20080530_
#define _MUTEXMANAGERAPP_H_INCLUDE_20080530_
#include "appfrm/Application.h"
#include "MutexSrcMgr.h"
#include "DoubleLock.h"
#include <list>


const int EWARN_BASE      = 1100;
const int EWARN_NORMAL    = EWARN_BASE+1;
const int EWARN_SERIOUS   = EWARN_BASE+2;
const int EWARN_FATAL     = EWARN_BASE+3;
const int EWARN_DELFORBID = EWARN_BASE+4;
class MutexWarnLog
{
  public:
    ProcessMSrcInfo  m_srcInfo;
    //告警级别：1告警、2严重告警、3致命、4进程已退出，强制释放
    int              m_warnLevel; 
    time_t           m_warnTime;
};
class MutexManagerApp:public Application
{
  public:
    MutexManagerApp();
    virtual ~MutexManagerApp();
   
  protected:
    virtual bool initialization();
    virtual bool beforeLoop();
    virtual bool recovery();
    virtual bool loopProcess();
    virtual bool end();
    
    bool getSysParam();
    void warning(const MutexWarnLog &r_wlog);
    void getWarnLevel(const ProcessMSrcInfo &r_srcInfo,int &r_wLevel);
    char* getLTime(const time_t &r_time);
  protected:
    MutexSrcMgr   m_mutexMgr;
    T_NAMEDEF     m_dbName;      //数据库名
    int           m_warnTimeOut; //告警时间
    int           m_sWarnTimeOut;//严重告警时间
    int           m_delTimeOut;  //强制释放时间
    T_FILENAMEDEF m_wlogFile ;   //告警日志文件
    char          m_chWarnInfo[10240+1];
  protected:
    list<MutexWarnLog>  m_warnLogInfo; //记录告警信息,用于对告警的排重
};

#endif //_MUTEXMANAGERAPP_H_INCLUDE_20080530_
