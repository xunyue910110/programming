//  ############################################
//  Source file : logger.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2009-5-21
//  Update      : 2009-5-21
//  Copyright(C): chen min, Linkage.
//  ############################################

#include "Mdb_Exception.h"
#include "SocketSession.h"
#include "Logger.h"

Logger *Logger::pLogger=NULL;
static  pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;  

Logger::Logger()
{
}

Logger::~Logger()
{
	if(m_ofstream.is_open())
	{
		m_ofstream.close();	
	}	
}

Logger *Logger::getInstance()
{
	if(pLogger==NULL)
	{
		pLogger=new Logger();
	}
	return pLogger;
}

void Logger::setChannelNo(int iChanNo)
{
	char logFileName[100];
	
	memset(logFileName,0,sizeof(logFileName));
	sprintf(logFileName,"%s/log/MdbServer.%d.log",getenv(MDB_HOMEPATH.c_str()),iChanNo);
	
	m_ofstream.open(logFileName,ios_base::out|ios_base::app);
        	
	if(m_ofstream.rdstate() != ios::goodbit)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "open log file error! %s",logFileName);		
	}
	
	return;
}

void Logger::setMsg(const char *cDateTime
						,const string &sClientName
						,const int iThreadID
						,const int iLogType)
{
	pthread_mutex_lock(&mutex);
	if( m_ofstream.rdstate()==ios::goodbit )
	{
		if(iLogType==CONNECT_LOG_TYPE)
		{
			m_ofstream<<"Begin ;"<<cDateTime<<";"<<iThreadID<<";"<<sClientName<<endl;
		}
		else if(iLogType==DISCONNECT_LOG_TYPE)
		{
			m_ofstream<<"End   ;"<<cDateTime<<";"<<iThreadID<<";"<<sClientName<<endl;	
		}
		//m_ofstream.fflush();
	}
	pthread_mutex_unlock(&mutex);	
	return;	
}
