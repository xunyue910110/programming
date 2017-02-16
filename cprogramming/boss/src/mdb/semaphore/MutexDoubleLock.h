// ############################################
// Source file : MutexDoubleLock.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef MUTEXDOUBLELOCK_H_HEADER_INCLUDED_B7DEB1E5
#define MUTEXDOUBLELOCK_H_HEADER_INCLUDED_B7DEB1E5

#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "DoubleLock.h"
#include "MutexShmMgr.h"

class MutexDoubleLock;

//##ModelId=48210F060119
class MutexDoubleLock : public DoubleLock
{
  public:
    MutexNode *m_pMutex;
    void      *m_shmAddr;
    ProcessMSrcInfo* m_pMSrcInfo;
  public:
    //##ModelId=482183620213
    MutexDoubleLock(const char * dbname);

    //##ModelId=4821838C01D4
    virtual ~MutexDoubleLock();

    //##ModelId=48167C1B01F4
    // 获取信号量，如果信号量不存在，创建新的信号量；如果信号量存在，连接该信号量
    virtual bool open(int resourceType, char *resourceID);

    //##ModelId=48167C49005D
    // 销毁该信号量
    virtual bool destroy();

    //##ModelId=48167C5D0177
    // 对信号量加读锁（共享锁）；如果不成功，等待
    virtual bool getReadLock();

    //##ModelId=48167C6D036B
    // 对信号量加写锁（排他锁）；如果不成功，等待
    virtual bool getWriteLock();

    //##ModelId=48167C790242
    // 对信号量加读锁（共享锁）；如果不成功，不等待，直接返回
    virtual bool getReadLockNoWait();

    //##ModelId=48167C8C0157
    // 对信号量加写锁（排他锁）；如果不成功，不等待，直接返回
    virtual bool getWriteLockNoWait();

    //##ModelId=48167C99035B
    // 对信号量释放读锁（共享锁）
    virtual bool releaseReadLock();

    //##ModelId=48167CA60203
    // 对信号量释放写锁（排他锁）
    virtual bool releaseWriteLock();

};

/*
假设N为无穷大：
r ... 0  读个数
w1... 0  写个数
w2... 0  写等待个数

读：
  加锁：
    获取信号量
    while(写等待个数＋写个数>0)
    { 
      条件等待
    }
    读个数加1
    释放信号量
  
  解锁：
    获取信号量
    读个数减1
    唤醒条件等待线程
    释放信号量

写：
  加锁：
    获取信号量
    写等待＋1
    while(读个数＋写个数>0)
    { 
      条件等待
    }
    写个数加1
    写等待－1
    释放信号量
  
  解锁：
    获取信号量
    写个数减1
    唤醒条件等待线程
    释放信号量
*/


#endif /* MUTEXDOUBLELOCK_H_HEADER_INCLUDED_B7DEB1E5 */
