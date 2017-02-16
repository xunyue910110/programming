// ############################################
// Source file : IpcDoubleLock.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "IpcDoubleLock.h"

const int MAX_SEMS = 10000;

//##ModelId=48167C130280
IpcDoubleLock::~IpcDoubleLock()
{
  if(m_sysSemaphore!=NULL)
  {
    delete m_sysSemaphore;
    m_sysSemaphore=NULL;
  }
}

//##ModelId=48167C1B01F4
bool IpcDoubleLock::open(int resourceType, char *resourceID)
{
    int value[2], n=2;
    value[0]=1;
    value[1]=MAX_SEMS;
    if(m_sysSemaphore==NULL)
    {
      //根据resourceType和resourceID取m_innerSrcId
      if(resourceType!=T_DBLK_LOCK)
      {
        if((m_innerSrcId=getInnerSrcId(m_fileName,resourceType,resourceID))<0)
        {
          #ifdef _DEBUG_
            cout<<"getInnerSrcId("<<m_fileName<<","<<resourceType
                <<","<<resourceID<<") failed!"<<__FILE__<<__LINE__<<endl;
          #endif
          return false;
        }
      }else
      { //LOCK文件信号灯
        m_innerSrcId=1;
      }
      m_sysSemaphore = new SysSemaphore(m_fileName,m_innerSrcId,n);
    }
    m_semid = m_sysSemaphore->sem_open();
    if(m_semid < 0)
    {
        // 创建 '0','1' 两个信号量,用于控制读写,
        // '0':二值信号量,控制写操作,
        //      '0' = 0: 有一进程正在进行写操作,此时别的进程就不能读
        //      '0' = 1: 目前没有进程在进行写操作
        // '1':多值信号量,控制读操作,
        //      '1' = MAX_SEMS: 没有进程在进行读操作
        //      '1' < MAX_SEMS: 有进程在进行读操作,但最多不能超过MAX_SEMS
        m_semid = m_sysSemaphore->sem_create(value,n);
        //初始化'1'信号量为0,表示没有进程在读共享IPC
        
        m_semid = m_sysSemaphore->sem_open();
    }
    return true;
}

//##ModelId=48167C49005D
bool IpcDoubleLock::destroy()
{
    return(m_sysSemaphore->sem_destroy());
}

//##ModelId=48167C5D0177
bool IpcDoubleLock::getReadLock()
{
  try
  {
    if(m_sysSemaphore->sem_getwcnt(1)!=0)
    { //存在写等待情况
      //获取写信号灯
      m_sysSemaphore->sem_wait(0,1);
      try
      {
        //获取读信号灯
        m_sysSemaphore->sem_wait(1,1);
      }catch(Mdb_Exception &e)
      {
        m_sysSemaphore->sem_post(0,1);
        throw e;
      }
      //释放写信号等
      m_sysSemaphore->sem_post(0,1);
    }else 
    {  //无写等待，则直接获取读信号灯
        m_sysSemaphore->sem_wait(1,1);
    }
  }catch(Mdb_Exception &e)
  {
    throw e;
    return false;
  }
  return true;
}

//##ModelId=48167C6D036B
bool IpcDoubleLock::getWriteLock()
{
  try
  { //获取写信号灯 
    m_sysSemaphore->sem_wait(0,1);
    try
    {
      m_sysSemaphore->sem_wait(1,MAX_SEMS);
    }catch(Mdb_Exception &e)
    {
      m_sysSemaphore->sem_post(0,1);
      throw e;
    }
  }catch(Mdb_Exception &e)
  {
    throw e;
    return false;
  }  
  return true;
}

//##ModelId=48167C790242
bool IpcDoubleLock::getReadLockNoWait()
{
  if (m_sysSemaphore->sem_trywait(1) != true)
  {
      return false;
  }
  else if (m_sysSemaphore->sem_getvalue(0) != 1)
  {
      m_sysSemaphore->sem_post(1);
      return false;
  }
  return true;
}

//##ModelId=48167C8C0157
bool IpcDoubleLock::getWriteLockNoWait()
{
    if (m_sysSemaphore->sem_trywait(0) != true)
    {
        return false;
    }
    else if (m_sysSemaphore->sem_getvalue(1) != MAX_SEMS)
    {
        m_sysSemaphore->sem_post(0);
        return false;
    }
    return true;
}

//##ModelId=48167C99035B
bool IpcDoubleLock::releaseReadLock()
{
    if (m_sysSemaphore->sem_post(1,1) != true)
    {
        return false;
    }
    else
    {
        return false;
    }
}

//##ModelId=48167CA60203
bool IpcDoubleLock::releaseWriteLock()
{
  try
  {
    m_sysSemaphore->sem_post(0,1); 
    m_sysSemaphore->sem_post(1,MAX_SEMS); 
  }catch (Mdb_Exception e)
  {
    throw e;
  }
  return true;
}

