// ############################################
// Source file : RwDoubleLock.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef RWDOUBLELOCK_H_HEADER_INCLUDED_B7E91197
#define RWDOUBLELOCK_H_HEADER_INCLUDED_B7E91197

#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "DoubleLock.h"
#include "RwlockShmMgr.h"

class RwDoubleLock;

//##ModelId=481027F403E3
class RwDoubleLock : public DoubleLock
{
  public:
    RwlockNode          *m_pRwlock;
    void                *m_shmAddr;
    ProcessMSrcInfo     *m_pMSrcInfo;

  public:
    //##ModelId=481027F403E4
    RwDoubleLock(const char * dbname);

    //##ModelId=481027F403E5
    virtual ~RwDoubleLock();

    //##ModelId=481027F403E6
    virtual bool open(int resourceType, char *resourceID);

    //##ModelId=481027F403E7
    virtual bool destroy();

    //##ModelId=481027F403E8
    virtual bool getReadLock();

    //##ModelId=481027F403E9
    virtual bool getWriteLock();

    //##ModelId=481027F403EA
    virtual bool getReadLockNoWait();

    //##ModelId=481027F403EB
    virtual bool getWriteLockNoWait();

    //##ModelId=48167E0E003E
    virtual bool releaseReadLock();

    //##ModelId=48167E19036B
    virtual bool releaseWriteLock();

  private:
    //##ModelId=481027F403EC
    //int semid;

    //##ModelId=48167CF6007D
    //char parthName[100];
    
    //pthread_rwlockattr_t    *m_rwattr;
    
    //pthread_rwlock_t        *m_rwlock;

};



#endif /* RWDOUBLELOCK_H_HEADER_INCLUDED_B7E91197 */
