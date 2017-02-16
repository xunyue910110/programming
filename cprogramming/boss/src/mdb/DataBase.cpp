// ############################################
// Source file : DataBase.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "DoubleLock.h"
#include "MemManager.h"
#include "Table.h"
#include "Mdb_Exception.h"
#include "ExpressionMgr.h"
#include "MdbConstDef.h"
#include "InstanceFactory.h"
#include "MdbSession.h"
#include "PublicInfo.h" //add by chenzm for hp-ux
#include "DataBase.h"

DataBase::DataBase()
{
	m_dbLock = NULL;
	m_memManager = NULL;
    m_tables.clear();
}

//##ModelId=481027F403F0
DataBase::DataBase(const char *dbName)
{
    strcpy(m_dbName, dbName);
    m_dbLock = InstanceFactory::getDoubLockInstance(SYS_SEMAPORE, m_dbName);
    
    // 只是给connect用,其它库相关操作,都不使用该实例
    m_memManager = NULL;
    m_tables.clear();
}

//##ModelId=481027F403F2
DataBase::~DataBase()
{
	for (TABLES_POOL_ITR iter_table = m_tables.begin(); iter_table != m_tables.end(); ++iter_table)
	{
		delete iter_table->second;
	}
	m_tables.clear();
    
    if (m_dbLock != NULL)
    {
        delete m_dbLock;
    }
    if (m_memManager!= NULL)
    {
        delete m_memManager;
    }
}

//##ModelId=481027F403F3
bool DataBase::create()
{
    MemManager *pMemMgr;
    try
    {
	    m_dbLock->open(1, m_dbName);
	    m_dbLock->getWriteLock();
        
        pMemMgr = new MemManager(m_dbName);
        pMemMgr->creatMdb();
        delete pMemMgr;
    }
    catch(Mdb_Exception &e)
    {
    	e.toString();
        if( pMemMgr!=NULL )
        {
        	delete 	pMemMgr;
        }
        m_dbLock->releaseWriteLock();
        m_dbLock->destroy();
        throw e;
    }
    m_dbLock->releaseWriteLock();
    return true;
}

//##ModelId=481027F403F4
bool DataBase::drop()
{
    MemManager *pMemMgr;
    try
    {
 	    m_dbLock->open(1, m_dbName);
	    m_dbLock->getWriteLock();

        pMemMgr = new MemManager(m_dbName);
        pMemMgr->dropMdb();
        delete pMemMgr;
    }
    catch(Mdb_Exception &e)
    {
        if( pMemMgr!=NULL )
        {
        	delete 	pMemMgr;
        }
        m_dbLock->releaseWriteLock();
        throw e;
    }
    m_dbLock->releaseWriteLock();
    m_dbLock->destroy();
    return true;
}

//##ModelId=481027F403F5
bool DataBase::start()
{
    MemManager *pMemMgr;
    try
    {
	    m_dbLock->open(1, m_dbName);
	    m_dbLock->getWriteLock();
        
        pMemMgr = new MemManager(m_dbName);
        pMemMgr->startMdb();
        delete pMemMgr;
    }
    catch(Mdb_Exception &e)
    {
        if( pMemMgr!=NULL )
        {
        	delete 	pMemMgr;
        }
        m_dbLock->releaseWriteLock();
        throw e;
    }
    m_dbLock->releaseWriteLock();
    return true;
}

//##ModelId=481027F403F6
bool DataBase::stop()
{
    MemManager *pMemMgr;
    try
    {
	    m_dbLock->open(1, m_dbName);
	    m_dbLock->getWriteLock();
	    
        pMemMgr = new MemManager(m_dbName);
        pMemMgr->stopMdb();
        delete pMemMgr;
    }
    catch(Mdb_Exception &e)
    {
        if( pMemMgr!=NULL )
        {
        	delete 	pMemMgr;
        }
        m_dbLock->releaseWriteLock();
        throw e;
    }
    m_dbLock->releaseWriteLock();
    return true;
}

//##ModelId=481027F403F7
Session* DataBase::connect()
{
    
    // 同一个mdb不能嵌套session
    if( m_memManager!= NULL)
    {
    	throw Mdb_Exception(__FILE__, __LINE__, "已有conncet尚未断开!");
    }
    try
    {
    	// APP获取session的时候，同时获取m_memManager,并连接共享内存（相当于连接数据库）
	    m_memManager = new MemManager(m_dbName);
	    m_memManager->openMdb();
	    
	    SessionInfo t_sessioninfo;
		
		t_sessioninfo.m_pId = getpid();
	    t_sessioninfo.m_time = time(NULL);
	    memset(t_sessioninfo.m_ipAddr,0,sizeof(T_IPADDR));
	    m_memManager->registerSession(t_sessioninfo);
	    
	    Session *t_session = InstanceFactory::getSessionInstance(LOCAL_SESSION,this);
	    t_session->setSessionID(t_sessioninfo.m_sessionId);
	    return t_session;
	}
	catch(Mdb_Exception &e)
	{
		if( m_memManager!= NULL )
		{
			delete m_memManager;
			m_memManager = NULL;
		}
		throw e;	
	}
}

//##ModelId=481027F403F8
bool DataBase::disconnect(Session *&session)
{
    SessionInfo t_sessioninfo;
    if (session != NULL)
    {
        t_sessioninfo.m_sessionId = session->getSessionID();
        m_memManager->unRegisterSession(t_sessioninfo);
        m_memManager->closeMdb();
        
        delete session;
	    delete m_memManager;
	    m_memManager = NULL;
    }
    return true;
}

//##ModelId=481027F403FA
bool DataBase::backup()
{
    MemManager *pMemMgr;
    try
    {
	    m_dbLock->open(1, m_dbName);
	    m_dbLock->getReadLock();
    	
    	pMemMgr = new MemManager(m_dbName);
    	pMemMgr->openMdb();
    	pMemMgr->backupMdb();
    	pMemMgr->closeMdb();
    	delete 	pMemMgr;
    }
    catch(Mdb_Exception &e)
    {
        if( pMemMgr!=NULL )
        {
        	delete 	pMemMgr;
        }
        m_dbLock->releaseReadLock();
        throw e;
    }
    m_dbLock->releaseReadLock();
    return true;
}

//##ModelId=481027F403FB
bool DataBase::createTable(const TableDef& tableDef)
{
    Table *t_table;
    t_table = new Table(m_dbName, tableDef.m_tableName, m_memManager);
    t_table->create(tableDef);
    
    MyString s;
    strcpy(s.m_pStr,tableDef.m_tableName);
    m_tables.insert(TABLES_POOL::value_type(s, t_table));
        
    return true;
}

//##ModelId=481027F403FD
bool DataBase::dropTable(const char *tableName)
{
    Table *t_table;
    t_table = findTableByName(tableName);
    t_table->drop();
    
	// 删除对应的缓存的table对象
    MyString s;
    TABLES_POOL_ITR iter_table;
	strcpy(s.m_pStr,tableName);
	iter_table = m_tables.find(s);
	m_tables.erase(iter_table);
    delete t_table;
    
    return true;
}

//##ModelId=481027F403FF
Table* DataBase::findTableByName(const char *tableName)
{

    Table *t_table;
    MyString s;
    TABLES_POOL_ITR iter_table;

    strcpy(s.m_pStr,tableName);
	iter_table = m_tables.find(s);
   		
    if ( iter_table != m_tables.end())
    {
        t_table = iter_table->second;
    }
    else
    {
        t_table = new Table(m_dbName, tableName, m_memManager); 
        t_table->initialize(); 
        m_tables.insert(TABLES_POOL::value_type(s, t_table));
    }
 
    return t_table;
}


MemManager* DataBase::getMemMgrInstance()
{
    return m_memManager;
}

