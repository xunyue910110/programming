// ############################################
// Source file : MutexSrcMgr.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef _MUTEXSRCMGR_H_INCLUDE_20080530_
#define _MUTEXSRCMGR_H_INCLUDE_20080530_
#include "MutexShmMgr.h"

class MutexSrcMgr
{
  public:
    MutexSrcMgr();
    virtual ~MutexSrcMgr();
  public:
    bool createMutexShm(const char * r_dbName,const int &r_srcNum);
    bool deleteMutexShm(const char * r_dbName,const int &r_srcNum);
    bool attachMutexShm(const char * r_dbName,const int &r_srcNum);
    bool detachMutexShm();    
    bool getEProcessInfo(const int &r_timeOut,vector<ProcessMSrcInfo> &r_eMSrcInfo);
    //获取进程已退出且进程信息未注销的信息。
    bool getExitProcessInfo(vector<ProcessMSrcInfo> &r_eMSrcInfo);
    bool rollbackSrc(const ProcessMSrcInfo &r_mSrcInfo);
    bool isProcessExist(const pid_t &r_pid)  const;
  private:
    T_NAMEDEF        m_dbName;
    int              m_srcNum;
    void*            m_shmAddr;
    ProcessMSrcInfo *m_psMsrcInfo;
    bool             m_attachFlag;
};

#endif //_MUTEXSRCMGR_H_INCLUDE_20080530_
