#ifndef _RWSRCINFO_H_INCLUDE_20080605_
#define _RWSRCINFO_H_INCLUDE_20080605_
#include <stdio.h>
#include <stdlib.h>
#include "MdbConstDef.h"
#include <pthread.h>

class RwlockNode
{
  public:
    pthread_rwlock_t    m_rwlock;
    pthread_cond_t      m_cond;
    int                 m_rdCount;
    int                 m_wtCount;
    int                 m_wtWait;
};
class RwlockShmInfo
{
  public:
    T_NAMEDEF  m_dbName;
    void*      m_shmAddr;
    int        m_maxNum;
  public:
    friend int operator<(const RwlockShmInfo &left,const RwlockShmInfo &right)
    {
      return (strcasecmp(left.m_dbName,right.m_dbName)<0);
    }
    friend int operator==(const RwlockShmInfo &left,const RwlockShmInfo &right)
    {
      return (strcasecmp(left.m_dbName,right.m_dbName)==0);
    }
};

class RwSrcInfo
{
  public:
    int     m_sid;
    int     m_rcount;
    int     m_wcount;
    int     m_wwait;
    time_t  m_time;
    RwSrcInfo(){}
    RwSrcInfo(const RwSrcInfo &right)
    {
      memcpy(this,&right,sizeof(RwSrcInfo));
    }
};
const int MAX_PSRC_NUM=10;
class ProcessMSrcInfo
{
  public:
    int         m_state; //0 未占用，1 已经占用
    pid_t       m_pid;
    pthread_t   m_thId;
    time_t      m_pTime;
    int         m_srcNum;
    RwSrcInfo   m_srcInfo[MAX_PSRC_NUM];
  public:
    ProcessMSrcInfo(){}
    ProcessMSrcInfo(const ProcessMSrcInfo &right)
    {
      memcpy(this,&right,sizeof(ProcessMSrcInfo));
    }
    friend int operator==(const ProcessMSrcInfo &left,const ProcessMSrcInfo &right)
    {
      return (left.m_pid==right.m_pid &&
              left.m_thId==right.m_thId);
    }
};

#endif //_RWSRCINFO_H_INCLUDE_20080605_
