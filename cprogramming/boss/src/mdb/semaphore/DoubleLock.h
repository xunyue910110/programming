// ############################################
// Source file : DoubleLock.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef DOUBLELOCK_H_HEADER_INCLUDED_B7E904AD
#define DOUBLELOCK_H_HEADER_INCLUDED_B7E904AD

#include "MdbConstDef.h"
#include <stdio.h>
//##ModelId=481027F403B9
class DoubleLock
{
  protected:
    DoubleLock();
  public:
    //##ModelId=481027F403BA
    DoubleLock(const char * dbname);

    //##ModelId=481027F403BD
    virtual ~DoubleLock();

    //##ModelId=481027F403BE
	// 获取信号量，如果信号量不存在，创建新的信号量；如果信号量存在，连接该信号量
	// resourceType 资源类型---0为数据库,1为表,2为tablespace；
	// resourceID   资源编号---作为对应，是数据库名或表名或tablespace名
    virtual bool open(int resourceType, char *resourceID) = 0;

    //##ModelId=481027F403C1
    // 销毁该信号量
    virtual bool destroy() = 0;

    //// 加上共享锁read，可以继续加共享锁read，阻止排他锁write
    //// 加上排他锁write的时候，阻止排他锁write和共享锁read

    //##ModelId=481027F403C2
    // 对信号量加读锁（共享锁）；如果不成功，等待
    virtual bool getReadLock() = 0;

    //##ModelId=481027F403C3
    // 对信号量加写锁（排他锁）；如果不成功，等待
    virtual bool getWriteLock() = 0;

    //##ModelId=481027F403C4
    // 对信号量加读锁（共享锁）；如果不成功，不等待，直接返回
    virtual bool getReadLockNoWait() = 0;

    //##ModelId=481027F403C5
    // 对信号量加写锁（排他锁）；如果不成功，不等待，直接返回
    virtual bool getWriteLockNoWait() = 0;

    //##ModelId=481027F403C6
    // 对信号量释放读锁（共享锁）
    virtual bool releaseReadLock() = 0;

    //##ModelId=481027F403C7
    // 对信号量释放写锁（排他锁）
    virtual bool releaseWriteLock() = 0;
  protected:
    struct InnerSrc
    {
      int         m_sourceType;
      T_NAMEDEF   m_sourceName;
      int         m_sourceId;
    };
    int getInnerSrcId(const char * fileName,int resourceType, char *resourceID);    
  public:
    enum
    {
      MAX_MUTEX_NUM = 100000, //支持互斥锁最大个数
      MAX_RWLOCK_NUM = 100000 //支持读写锁最大个数
    };
  protected:
    //##ModelId=481027F403C8
    int m_semid;

    //##ModelId=4811773500AB
    T_FILENAMEDEF m_fileName; 
    T_NAMEDEF     m_dbName;
    int m_innerSrcId;
};



#endif /* DOUBLELOCK_H_HEADER_INCLUDED_B7E904AD */
