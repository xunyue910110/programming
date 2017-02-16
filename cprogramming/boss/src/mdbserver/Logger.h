//  ############################################
//  Source file : logger.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2009-5-21
//  Update      : 2009-5-21
//  Copyright(C): chen min, Linkage.
//  ############################################

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "config-all.h"
#include <string>
USING_NAMESPACE(std)

class Logger
{
	public:
		static Logger * getInstance();
		void setChannelNo(int iChanNo);

		void setMsg(const char *cDateTime
						,const string & sClientName
						,const int iThreadID
						,const int iLogType);
				
	private:
		Logger();
		virtual ~Logger();
				
	private:
		static Logger *pLogger;
		ofstream m_ofstream;
		int  m_iChanNo;
};

#endif
