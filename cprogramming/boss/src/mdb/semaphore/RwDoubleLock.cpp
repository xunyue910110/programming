// ############################################
// Source file : RwDoubleLock.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "RwDoubleLock.h"
#include "Mdb_Exception.h"
#include "/billing/src/mdb/memMgr/CtlElementTmpt.h"

//##ModelId=481027F403E4
RwDoubleLock::RwDoubleLock(const char * dbname)
{
    strcpy(m_dbName,dbname);
    sprintf(m_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),dbname);
    if(RwlockShmMgr::getRwlockShmAddr(dbname,DoubleLock::MAX_RWLOCK_NUM,m_shmAddr,m_pMSrcInfo)==false)
    {
    #ifdef _DEBUG_
        cout<<"getRwlockShmAddr("<<dbname<<") error!"<<__FILE__<<__LINE__<<endl;
    #endif
    throw Mdb_Exception(__FILE__, __LINE__,"getRwlockShmAddr %s error!",dbname);
    }
}

//##ModelId=481027F403E5
RwDoubleLock::~RwDoubleLock()
{
    if (m_pRwlock != NULL)
    {
        delete m_pRwlock;
    }
    if (m_shmAddr != NULL)
    {
        delete m_shmAddr;
    }
}

//##ModelId=481027F403E6
bool RwDoubleLock::open(int resourceType, char *resourceID)
{
    if((m_innerSrcId = getInnerSrcId(m_fileName, resourceType, resourceID)) < 0)
    {
    #ifdef _DEBUG_
        cout<<"getInnerSrcId("<<m_fileName<<","<<resourceType
            <<","<<resourceID<<") failed!"<<__FILE__<<__LINE__<<endl;
    #endif
        throw Mdb_Exception(__FILE__, __LINE__,"getInnerSrcId %s %s error!",resourceType,resourceID);
    }
    if(m_innerSrcId>=DoubleLock::MAX_RWLOCK_NUM)
    {
        #ifdef _DEBUG_
        cout<<"m_innerSrcId ="<<m_innerSrcId<<" 过大，最大可为"<<DoubleLock::MAX_RWLOCK_NUM-1
            <<" !"<<__FILE__<<__LINE__<<endl;
        #endif
        throw Mdb_Exception(__FILE__, __LINE__,"SourceId %d 过大!",m_innerSrcId);
    }
    RwlockNode * t_pRwlock=(RwlockNode*)((char*)m_shmAddr+RwlockShmMgr::STATE_BIT_SIZE);
    m_pRwlock=t_pRwlock+m_innerSrcId;
    return true;
}

//##ModelId=481027F403E7
bool RwDoubleLock::destroy()
{
    return true;
}

//##ModelId=481027F403E8
bool RwDoubleLock::getReadLock()
{
    if (pthread_rwlock_rdlock(&(m_pRwlock->m_rwlock)) != 0)
    {
        throw Mdb_Exception(__FILE__, __LINE__,"pthread_rwlock_rdlock Error!");
    }
    ++(m_pRwlock->m_rdCount);
    RwlockShmMgr::lockRdSrc(m_pMSrcInfo, m_innerSrcId);
    return true;
}

//##ModelId=481027F403E9
bool RwDoubleLock::getWriteLock()
{
    if (pthread_rwlock_wrlock(&(m_pRwlock->m_rwlock)) != 0)
    {
        throw Mdb_Exception(__FILE__, __LINE__,"pthread_rwlock_wrlock Error!");
    }
    ++(m_pRwlock->m_wtWait);
    RwlockShmMgr::lockWWaitSrc(m_pMSrcInfo,m_innerSrcId);
    ++(m_pRwlock->m_wtCount);
    --(m_pRwlock->m_wtWait);
    RwlockShmMgr::unlockWWaitSrc(m_pMSrcInfo,m_innerSrcId);
    RwlockShmMgr::lockWdSrc(m_pMSrcInfo,m_innerSrcId);
    return true;
}

//##ModelId=481027F403EA
bool RwDoubleLock::getReadLockNoWait()
{
    if (pthread_rwlock_tryrdlock(&(m_pRwlock->m_rwlock)) != 0)
    {
        return false;
    }
    ++(m_pRwlock->m_rdCount);
    RwlockShmMgr::lockRdSrc(m_pMSrcInfo,m_innerSrcId);
    return true;
}

//##ModelId=481027F403EB
bool RwDoubleLock::getWriteLockNoWait()
{
    if (pthread_rwlock_trywrlock(&(m_pRwlock->m_rwlock)) != 0)
    {
        return false;
    }
    ++(m_pRwlock->m_wtCount);
    RwlockShmMgr::lockWdSrc(m_pMSrcInfo,m_innerSrcId);
    return true;
}

//##ModelId=48167E0E003E
bool RwDoubleLock::releaseReadLock()
{
    if (pthread_rwlock_unlock(&(m_pRwlock->m_rwlock)) != 0)
    {
        throw Mdb_Exception(__FILE__, __LINE__,"pthread_rwlock_unlock Error!");
    }
    --(m_pRwlock->m_rdCount);
    RwlockShmMgr::unlockRdSrc(m_pMSrcInfo,m_innerSrcId);
    return true;
}

//##ModelId=48167E19036B
bool RwDoubleLock::releaseWriteLock()
{
    if (pthread_rwlock_unlock(&(m_pRwlock->m_rwlock)) != 0)
    {
        throw Mdb_Exception(__FILE__, __LINE__,"pthread_rwlock_unlock Error!");
    }
    --(m_pRwlock->m_wtCount);
    RwlockShmMgr::unlockWdSrc(m_pMSrcInfo,m_innerSrcId);
    return true;
}

