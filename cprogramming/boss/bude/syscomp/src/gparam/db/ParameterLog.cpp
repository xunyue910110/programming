// ############################################
// Source file : ParameterLog.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030927
// Update      : 20030927
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <string>

#include "util/Util.h"  
#include "util/PFileProcess.h" 
#include "ParameterLog.h" 

#include <pthread.h>
ParameterLog * ParameterLog::m_parameterLog;
static  pthread_mutex_t g_ParameterLog_mutex; 

//##ModelId=3F752B6C01F3
ParameterLog::ParameterLog()
{   
	m_debug 	= 2;
	m_logFile 	= new PFileProcess;	
	pthread_mutex_init(&g_ParameterLog_mutex,NULL);
}

//##ModelId=3F752B6C0207
ParameterLog::~ParameterLog()
{  
	delete m_logFile;
	pthread_mutex_destroy(&g_ParameterLog_mutex);
}

//##ModelId=3F752B6C0257 
void ParameterLog::setMessage(string message, int flag) 
{   
	ofstream df;
	char month[3]; 
	//static string datetime,logFilename;  
	string datetime,logFilename;  
				
	if(m_debug>0) { 
		pthread_mutex_lock (&g_ParameterLog_mutex);//锁定互斥信号量
		//防止同时有多个线程打开同个文件
		datetime = getDateTime(2);  	
		memcpy(month,datetime.c_str()+4,2);
		month[2]=0;
	
		logFilename = m_logPath + "ParameterLog_" + month;
		m_logFile->open(logFilename,"a"); 
		if(flag == 1) {
			message = datetime + " " + message;  
		}
		
		message = logFilename + ":" + message; //跟踪文件名变化情况	2007-4-13 11:30		
		m_logFile->writeCdr( message.c_str() );		 
		m_logFile->close();
		pthread_mutex_unlock(&g_ParameterLog_mutex);//解锁互斥信号量
	}
	//m_message = message;
}		

//##ModelId=3F752D7E01A5
void ParameterLog::setDebug(int flag)
{  
	m_debug = flag;
}

string ParameterLog::getMessage()
{
	return m_message;
}

//##ModelId=3F752B6C0225
ParameterLog *ParameterLog::getInstance()
{
	if ( m_parameterLog == 0 )
		m_parameterLog= new ParameterLog;

	return m_parameterLog;
} 
//##ModelId=4026FD9E00FF
void ParameterLog::setLogPath(const string &logPath)
{  
	m_logPath = logPath;
}   
