// ############################################
// Source file : MutexShmMgr.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef _MUTEXSHMMGR_H_INCLUDE_20080529_
#define _MUTEXSHMMGR_H_INCLUDE_20080529_

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "MdbConstDef.h"
#include <pthread.h>
#include <MSrcInfo.h>

class MutexShmMgr
{
  public:
    MutexShmMgr();
    virtual ~MutexShmMgr();
  public:
    enum 
    {
      MUTEX_FTOK_ID  = 0x7F ,
      STATE_BIT_SIZE = 8    ,
      MAX_PROCESS_NUM= 10000 //,
      //MAX_PSRC_NUM   = 10
    };
  public:
    static bool getMutexShmAddr(const char * r_dbName,const int &r_mutexNum,void * &r_shmAddr,ProcessMSrcInfo* &r_pMsrcInfo);
    static bool createMutexShm(MutexShmInfo &r_mutexShmInfo);
    static bool deleteMutex(const  char * r_dbName,const int &r_mutexNum);
    static bool delThreadSrcInfo(const char *r_dbName);
    static bool lockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool lockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool unlockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool unlockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool lockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool unlockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
  protected:
    static bool setThreadSrcPos(MutexShmInfo &r_mutexShmInfo,ProcessMSrcInfo* &r_pMsrcInfo);
    static bool getThreadSrcPos(const MutexShmInfo &r_mutexShmInfo,ProcessMSrcInfo* &r_pMsrcInfo);
  protected:
    static vector<MutexShmInfo> m_mutexShmList;
    static pthread_mutex_t      m_innerMutex;
};




#endif //_MUTEXSHMMGR_H_INCLUDE_20080529_
