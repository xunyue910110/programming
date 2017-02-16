//  ############################################
//  Source file : MdbServer.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-4-23
//  Update      : 2008-4-23
//  Copyright(C): chen min, Linkage.
//  ############################################

#include "MdbServer.h"
#include "Socket.h"
#include "SocketSession.h"
#include "Thread.h"
#include "Mdb_Exception.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std)

MdbServer::~MdbServer()
{
	this->end();
}

//##ModelId=483BCDBF038A
void MdbServer::run()
{
	Socket *pSocket;
	SocketSession *pSockSession;

	m_pListenSocket->startUpListen(NULL,m_port);
	while(1)
	{
		try
		{
			pSocket = m_pListenSocket->accept();
			
			// 如果socket accept收到进程终止信号,则会返回NULL,那么进程退出
			if( pSocket == NULL )
			{
				//printf("XXXXXX!");
				break;
			}
			pSockSession = this->getSession(pSocket);
			// add by chenm 2009-5-21 防止上次client端异常退出,导致的夸表事务异常出现
			pSockSession->rollBack();
			// modified by chenm 2009-5-13 21:58:45
			if(pSockSession!=NULL)
			{
			// over 2009-5-13 21:58:52
				pSockSession->setSocket(pSocket);  
		
				m_pThread->start( &(runSocketSession),pSockSession );		
				m_pThread->join();
			}
		}
		catch(Mdb_Exception &e)
		{
			if( pSocket != NULL)
				delete pSocket;
			theErrorMessages->insert(20000,e.GetString());
		   #ifdef _DEBUG_
			e.toString();	
		   #endif
			break;
		}
	}	
}

// 静态方法.启动一个线程,对应处理一个远程连接的请求
void *MdbServer::runSocketSession(void* arg)
{
	( (SocketSession *)arg )->run();	
	return NULL;
}

//##ModelId=483BCDCC0148
bool MdbServer::initialize(const char *cDBName,int port,int iMaxConnect)
{
	m_pListenSocket = new Socket();
	m_pThread       = new Thread();
	
	m_port       = port;
	m_dbName     = cDBName;
	m_maxConnect = iMaxConnect;
	
	m_sessionPool.clear();
	
	return true;
}

//##ModelId=483BC28F0213
SocketSession *MdbServer::getSession(Socket * pSocket)
{
	int i;
	for(i=0;i<m_sessionPool.size();++i)
	{
		if( !m_sessionPool[i]->isActive() )
		{
			m_sessionPool[i]->setActive();
			pSocket->write(SUCCESS,sizeof(SUCCESS));
			return 	m_sessionPool[i];
		}	
	}
	// 如果session池未满,则创建一个
	if( i < m_maxConnect )
	{
		SocketSession *pSocketSession = new SocketSession();
		// socket会话连接本地内存数据库
		pSocketSession->connect(m_dbName.c_str());
		m_sessionPool.push_back(pSocketSession);
		pSocket->write(SUCCESS,sizeof(SUCCESS));
		return pSocketSession;			
	}
	else
	{
		//modified by chenm 2009-5-13 22:00:04
		//throw Mdb_Exception(__FILE__, __LINE__, "连接数达到上限!");	
		pSocket->write("Refused!MDB reached MAX connection!",35);
		return NULL;
		//over 2009-5-13 22:00:12	
	}
}

// 进程退出时,需要做的工作
bool MdbServer::end()
{
	for(int i=0;i<m_sessionPool.size();++i)
	{
		if( m_sessionPool[i]!= NULL )
		{
			m_sessionPool[i]->end();
			delete m_sessionPool[i];
		}
	}
	m_sessionPool.clear();
	
	if( m_pListenSocket!= NULL)
	{
		m_pListenSocket->shutdown();
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
	if( m_pThread!= NULL)
	{
		delete m_pThread;
		m_pThread = NULL;
	}
	
	return true;	
}

