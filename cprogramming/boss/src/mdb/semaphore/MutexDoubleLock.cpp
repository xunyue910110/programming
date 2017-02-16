// ############################################
// Source file : MutexDoubleLock.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################
#include <sys/errno.h>
#include "MutexDoubleLock.h"
#include "Mdb_Exception.h"
#include "CtlElementTmpt.h"

//##ModelId=482183620213
MutexDoubleLock::MutexDoubleLock(const char * dbname)
{
  strcpy(m_dbName,dbname);
  sprintf(m_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),dbname);
  if(MutexShmMgr::getMutexShmAddr(dbname,DoubleLock::MAX_MUTEX_NUM,m_shmAddr,m_pMSrcInfo)==false)
  {
    #ifdef _DEBUG_
      cout<<"getMutexShmAddr("<<dbname<<") error!"<<__FILE__<<__LINE__<<endl;
    #endif
    throw Mdb_Exception(__FILE__, __LINE__,"getMutexShmAddr %s error!",dbname);
  }
}

//##ModelId=4821838C01D4
MutexDoubleLock::~MutexDoubleLock()
{
  m_pMutex=NULL;
  m_shmAddr=NULL;
}

//##ModelId=481027F403E6
bool MutexDoubleLock::open(int resourceType, char *resourceID)
{
  if((m_innerSrcId = getInnerSrcId(m_fileName, resourceType, resourceID)) < 0)
  {
    #ifdef _DEBUG_
      cout<<"getInnerSrcId("<<m_fileName<<","<<resourceType
          <<","<<resourceID<<") failed!"<<__FILE__<<__LINE__<<endl;
    #endif
    throw Mdb_Exception(__FILE__, __LINE__,"getInnerSrcId %s %s error!",resourceType,resourceID);
  }
  if(m_innerSrcId>=DoubleLock::MAX_MUTEX_NUM)
  {
    #ifdef _DEBUG_
      cout<<"m_innerSrcId ="<<m_innerSrcId<<" 过大，最大可为"<<DoubleLock::MAX_MUTEX_NUM-1
          <<" !"<<__FILE__<<__LINE__<<endl;
    #endif
    throw Mdb_Exception(__FILE__, __LINE__,"SourceId %d 过大!",m_innerSrcId);
  }
  MutexNode * t_pMutex=(MutexNode*)((char*)m_shmAddr+MutexShmMgr::STATE_BIT_SIZE);
  m_pMutex=t_pMutex+m_innerSrcId;
  return true;
}

//##ModelId=481027F403E7
bool MutexDoubleLock::destroy()
{
  return true;
}

//##ModelId=481027F403E8
bool MutexDoubleLock::getReadLock()
{
  // 先注册,如果下面加锁后,解锁前,该进程被-9了,那么mutex_manager也能解mutex锁
  MutexShmMgr::lockRdSrc(m_pMSrcInfo,m_innerSrcId);
  if (pthread_mutex_lock(&(m_pMutex->m_mutex)) != 0)
  {
    #ifdef _DEBUG_
  	  printf("%s", strerror(errno));
    #endif
  	throw Mdb_Exception(__FILE__, __LINE__, strerror(errno));
  }
  
  while(m_pMutex->m_wtCount + m_pMutex->m_wtWait > 0)
  {
    pthread_cond_wait(&(m_pMutex->m_cond),&(m_pMutex->m_mutex));
  }
  ++(m_pMutex->m_rdCount);
  pthread_mutex_unlock(&(m_pMutex->m_mutex));
  return true;
}

//##ModelId=481027F403E9
bool MutexDoubleLock::getWriteLock()
{
  if (pthread_mutex_lock(&(m_pMutex->m_mutex)) != 0)
  {
    #ifdef _DEBUG_
  	  printf("%s", strerror(errno));
    #endif
  	throw Mdb_Exception(__FILE__, __LINE__, strerror(errno));
  }
  ++(m_pMutex->m_wtWait);
  MutexShmMgr::lockWWaitSrc(m_pMSrcInfo,m_innerSrcId);
  while((m_pMutex->m_rdCount + m_pMutex->m_wtCount) > 0)
  {
      pthread_cond_wait(&(m_pMutex->m_cond),&(m_pMutex->m_mutex));
  }
  ++(m_pMutex->m_wtCount);
  --(m_pMutex->m_wtWait);
  MutexShmMgr::unlockWWaitSrc(m_pMSrcInfo,m_innerSrcId);
  MutexShmMgr::lockWdSrc(m_pMSrcInfo,m_innerSrcId);
  pthread_mutex_unlock(&(m_pMutex->m_mutex));
  return true;
}

//##ModelId=481027F403EA
bool MutexDoubleLock::getReadLockNoWait()
{
  if (pthread_mutex_trylock(&(m_pMutex->m_mutex)) != 0)
  {
	  return false;
  }
  while(m_pMutex->m_wtCount + m_pMutex->m_wtWait > 0)
  {
    pthread_mutex_unlock(&(m_pMutex->m_mutex));
    return false;
  }
  ++(m_pMutex->m_rdCount);
  MutexShmMgr::lockRdSrc(m_pMSrcInfo,m_innerSrcId);
  pthread_mutex_unlock(&(m_pMutex->m_mutex));
  return true;
}

//##ModelId=481027F403EB
bool MutexDoubleLock::getWriteLockNoWait()
{
  if (pthread_mutex_trylock(&(m_pMutex->m_mutex)) != 0)
  {
  	return false;
  }
  while((m_pMutex->m_rdCount + m_pMutex->m_wtCount) > 0)
  {
    pthread_mutex_unlock(&(m_pMutex->m_mutex));
    return false;
  }
  ++(m_pMutex->m_wtCount);
  MutexShmMgr::lockWdSrc(m_pMSrcInfo,m_innerSrcId);
  pthread_mutex_unlock(&(m_pMutex->m_mutex));
  return true;
}

//##ModelId=48167E0E003E
bool MutexDoubleLock::releaseReadLock()
{
  if (pthread_mutex_lock(&(m_pMutex->m_mutex)) != 0)
  {
    #ifdef _DEBUG_
  	  printf("%s", strerror(errno));
    #endif
  	throw Mdb_Exception(__FILE__, __LINE__, strerror(errno));
  }
  --(m_pMutex->m_rdCount);
  // add by chenm 2009-5-11 
  if(m_pMutex->m_rdCount<0)
  {
  	int irdCount = m_pMutex->m_rdCount;
  	
  	m_pMutex->m_rdCount = 0;
	MutexShmMgr::unlockRdSrc(m_pMSrcInfo,m_innerSrcId);
	pthread_mutex_unlock(&(m_pMutex->m_mutex));
	
  	throw Mdb_Exception(__FILE__, __LINE__, "资源 %d 的ReadLock出现负值:%d!",m_innerSrcId,irdCount);
  }
  // over2009-5-11 10:35:45
  if (0 == m_pMutex->m_rdCount)
  {
    pthread_cond_broadcast(&(m_pMutex->m_cond));
  }
  // by chenm 2009-10-5 19:29:43 后注册,如果下面加锁后,解锁前,该进程被-9了,那么mutex_manager也能解mutex锁
  //MutexShmMgr::unlockRdSrc(m_pMSrcInfo,m_innerSrcId);
  //pthread_mutex_unlock(&(m_pMutex->m_mutex));
	pthread_mutex_unlock(&(m_pMutex->m_mutex));
	MutexShmMgr::unlockRdSrc(m_pMSrcInfo,m_innerSrcId);
	// over 2009-10-5 19:30:22
  return true;
}

//##ModelId=48167E19036B
bool MutexDoubleLock::releaseWriteLock()
{
  if (pthread_mutex_lock(&(m_pMutex->m_mutex)) != 0)
  {
    #ifdef _DEBUG_
  	  printf("%s", strerror(errno));
    #endif
  	throw Mdb_Exception(__FILE__, __LINE__, strerror(errno));
  }
  --(m_pMutex->m_wtCount);
  // add by chenm 2009-5-11 
  if(m_pMutex->m_wtCount<0)
  {
  	int iwtCount = m_pMutex->m_wtCount;
  	
  	m_pMutex->m_wtCount = 0;
	MutexShmMgr::unlockWdSrc(m_pMSrcInfo,m_innerSrcId);
	pthread_mutex_unlock(&(m_pMutex->m_mutex));

  	throw Mdb_Exception(__FILE__, __LINE__, "资源 %d 的WriteLock出现负值:%d!",m_innerSrcId,iwtCount);
  }
  // over2009-5-11 10:35:45
  if (0 == m_pMutex->m_wtCount)
  {
    pthread_cond_broadcast(&(m_pMutex->m_cond));
  }
  MutexShmMgr::unlockWdSrc(m_pMSrcInfo,m_innerSrcId);
  pthread_mutex_unlock(&(m_pMutex->m_mutex));
  return true;
}

