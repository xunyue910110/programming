// ############################################
// Source file : IpcDoubleLock.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef IPCDOUBLELOCK_H_HEADER_INCLUDED_B7E97A55
#define IPCDOUBLELOCK_H_HEADER_INCLUDED_B7E97A55
#include "DoubleLock.h"
#include "SysSemaphore.h"
#include <stdio.h>

//##ModelId=48167B8A01C5
class IpcDoubleLock : public DoubleLock
{
  public:
    IpcDoubleLock()
    {
      m_sysSemaphore=NULL;
    }
    //##ModelId=48167C0A0203
    IpcDoubleLock(const char * dbname)
    {
      strcpy(m_dbName,dbname);
      sprintf(m_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),dbname);
      m_sysSemaphore=NULL;
    }

    //##ModelId=48167C130280
    virtual ~IpcDoubleLock();

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
  private:

    //##ModelId=481D619A01F4
    SysSemaphore *m_sysSemaphore;


};

/*
假设N为无穷大：
a... N  读信号量
b... 1  写信号量

读：
  加锁：
  if(等a增加进程为0)
  { 无写情况
    a-1
  }else
  {
    等待成功：b-1  即等待所有写退出
    a-1
    b+1
  }

  解锁：
   a+1


写：
  加锁：
    等待b-1成功
    等待成功：a-N  加写锁
  
  解锁：
   a+N
   b+1

*/


#endif /* IPCDOUBLELOCK_H_HEADER_INCLUDED_B7E97A55 */
