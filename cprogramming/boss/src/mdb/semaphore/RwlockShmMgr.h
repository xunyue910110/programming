
#ifndef _RWLOCKSHMMGR_H_INCLUDE_20080610_
#define _RWLOCKSHMMGR_H_INCLUDE_20080610_

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "MdbConstDef.h"
#include <pthread.h>
#include <RwSrcInfo.h>

class RwlockShmMgr
{
  public:
    RwlockShmMgr();
    virtual ~RwlockShmMgr();
  public:
    enum 
    {
      RWLOCK_FTOK_ID    = 0x7F ,
      STATE_BIT_SIZE    = 8    ,
      MAX_PROCESS_NUM=  10000 //,
      //MAX_PSRC_NUM    = 10
    };
  public:
    static bool getRwlockShmAddr(const char * r_dbName,const int &r_RwlockNum,void * &r_shmAddr,ProcessMSrcInfo* &r_pMsrcInfo);
    static bool createRwlockShm(RwlockShmInfo &r_RwlockShmInfo);
    static bool deleteRwlock(const  char * r_dbName,const int &r_RwlockNum);
    static bool delThreadSrcInfo(const char *r_dbName);
    static bool lockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool lockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool unlockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool unlockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool lockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
    static bool unlockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId);
  protected:
    static bool setThreadSrcPos(RwlockShmInfo &r_RwlockShmInfo,ProcessMSrcInfo* &r_pMsrcInfo);
    static bool getThreadSrcPos(const RwlockShmInfo &r_RwlockShmInfo,ProcessMSrcInfo* &r_pMsrcInfo);
  protected:
    static vector<RwlockShmInfo> m_RwlockShmList;
    static pthread_rwlock_t      m_innerRwlock;
};




#endif //_RWLOCKSHMMGR_H_INCLUDE_20080610_
