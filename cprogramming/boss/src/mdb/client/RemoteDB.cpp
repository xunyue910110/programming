//  ############################################
//  Source file : RemoteDB.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-6-5 15:34:08
//  Update      : 2008-6-5 15:34:09
//  Copyright(C): chen min, Linkage.
//  ############################################
#include "PublicInfo.h" //add by chenzm for hp-ux

#include "RemoteDB.h"
#include "Socket.h"
#include "InstanceFactory.h"
#include "Session.h"
#include "RemoteMdbSession.h"
#include "Mdb_Exception.h"

RemoteDB::~RemoteDB()
{
	if(m_pSocket!=NULL)
		delete m_pSocket;	
}


RemoteDB::RemoteDB(const char *dbName,const char *hostName,const int port)
{
	strcpy(m_dbName, dbName);
	
	memset(m_hostname,0,sizeof(m_hostname));
	strcpy(m_hostname,hostName);
	
	m_port = port;
	m_pSocket = NULL;
}

Session* RemoteDB::connect()
{
	if( m_pSocket!=NULL)
		throw Mdb_Exception(__FILE__, __LINE__, "已有conncet尚未断开!");

	try
	{
		m_pSocket = new Socket();
		m_pSocket->connect(m_hostname,m_port);
		
		Session *t_session = InstanceFactory::getSessionInstance(REMOTE_SESSION,m_pSocket);
		return t_session;
	}
	catch(Mdb_Exception &e)
	{
		if(m_pSocket!=NULL)
			delete m_pSocket;	
		throw e;
	}
}

bool RemoteDB::disconnect(Session *&session)
{	
	delete session;
	delete m_pSocket;
	m_pSocket = NULL;
	return true;
}
