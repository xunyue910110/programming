// ############################################
// Source file : InstanceFactory.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-5-18
// Update      : 2008-5-18
// Copyright(C): chen min, Linkage.
// ############################################

#include "TransDataMgr.h"
#include "ExpressionMgr.h"
#include "MdbSession.h"
#include "MutexDoubleLock.h"
#include "IndexHash.h"
#include "DataBase.h"
#include "RecordConvert.h"
#include "InstanceFactory.h"
#include "RemoteMdbSession.h"
#include "RemoteDB.h"
#include "ODBC/OdbcDB.h"
#include "IndexBTree.h"

//##ModelId=482CE0220119
Index* InstanceFactory::getIndexInstance(T_INDEXTYPE indexType, 
												const char *cTableName, 
												const char *cIndexName, 
												MemManager* memMgr, 
												RecordConvert* recordConvert)
{
	Index *pIndex;
	
	switch( indexType )
	{
		case HASH_INDEX:
			pIndex = new IndexHash(cTableName,cIndexName,memMgr,recordConvert);
			break;
		case TREE_INDEX:
			pIndex = new IndexBTree(cTableName,cIndexName,memMgr,recordConvert);
			break;
		default	:
			throw Mdb_Exception (__FILE__, __LINE__, " 未知的索引类型!");
	}
	
	return pIndex;
}

//##ModelId=482CE85A0128
DataBase *InstanceFactory::getDBInstance(T_DATABASETYPE dbType
											,const char * cDataBaseName
											,const char *hostName
											,const int port
											,const char *cUserName
    										,const char *cPwd
    										,const int iConnType)
{
	DataBase *pDB;
	
	switch( dbType)
	{
		case MDB:
			pDB = new DataBase(cDataBaseName);	
			break;
		case RMDB:
			pDB = new RemoteDB(cDataBaseName,hostName,port);	
			break;
	#ifdef ALTIBASE
		case ODBCMDB:
			pDB = new OdbcDB(cDataBaseName,hostName,port,cUserName,cPwd,iConnType);	
			break;
	#endif
	#ifdef TIMESTEN
		case ODBCMDB:
			pDB = new OdbcDB(cDataBaseName,hostName,port,cUserName,cPwd,iConnType);	
			break;
	#endif
	
		default:
			throw Mdb_Exception (__FILE__, __LINE__, " 未知的数据库类型!");
	}
	
	return pDB;
}

//##ModelId=482CE87702AF
DoubleLock *InstanceFactory::getDoubLockInstance(T_SEMAPHORETYPE semaphoreType,const char * cDataBaseName)
{
	DoubleLock *pDoubleLock;
	
	switch( semaphoreType )
	{
		case SYS_SEMAPORE:
			pDoubleLock = new MutexDoubleLock(cDataBaseName);
			break;
		default:
			throw Mdb_Exception (__FILE__, __LINE__, " 未知的信号管理类型!");
	}
	
	return pDoubleLock;
}

Session *InstanceFactory::getSessionInstance(T_SESSIONTYPE sessionType,void *point)
{
	Session *pSession;
	
	switch( sessionType )
	{
		case LOCAL_SESSION:
			pSession = new MdbSession((DataBase *)point);
			break;
		case REMOTE_SESSION:
			pSession = new RemoteMdbSession((Socket *)point); 
			break;
		default:
			throw Mdb_Exception (__FILE__, __LINE__, " 未知的session类型!");
	}	
	
	return pSession;
}
