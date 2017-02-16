//  ############################################
//  Source file : OdbcDB.h
//  Version     : 0.1.1
//  Language	: ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-9-19 
//  Update      : 2008-9-19 
//  Copyright(C): chen min, Linkage.
//  ############################################
#include "PublicInfo.h" //add by chenzm for hp-ux
#include "OdbcDB.h"
#include "InstanceFactory.h"
#include "Session.h"
#include "OdbcSession.h"
#include "Mdb_Exception.h"


OdbcDB::~OdbcDB()
{
}

OdbcDB::OdbcDB(const char *dbName
						,const char *hostName
						,const int port
						,const char *dbUserName
						,const char *dbUserPwd
						,const int iConnType)
{
	strcpy(m_dbName, dbName);
	
	memset(m_hostname,0,sizeof(m_hostname));
	strcpy(m_hostname,hostName);

	memset(m_dbUserName,0,sizeof(m_dbUserName));
	strcpy(m_dbUserName,dbUserName);

	memset(m_dbUserPwd,0,sizeof(m_dbUserPwd));
	strcpy(m_dbUserPwd,dbUserPwd);
	
	m_port = port;
	m_memManager = NULL;
	m_dbLock=NULL;
	m_iConnType=iConnType;
}

Session* OdbcDB::connect()
{
	Session *pSession = new OdbcSession(m_hostname,m_dbUserName,m_dbUserPwd,m_port,m_iConnType);
	return pSession;
}

bool OdbcDB::disconnect(Session *&pSession)
{	
	delete pSession;
	pSession = NULL;
	return true;
}
