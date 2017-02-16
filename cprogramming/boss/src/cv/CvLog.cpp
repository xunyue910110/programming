// ##########################################
// Source file : CvLog.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : shijianchun
// E_mail      : shijc@lianchuang.com
// Create      : 20098018
// Update      : 20098018
// Copyright(C): 2009 by shijianchun, Linkage.
// ##########################################

#include <stdio.h>
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"

#include "CvLog.h"

CvLog::CvLog()
{
}

CvLog::~CvLog()
{
}


void CvLog::initialize()
{ 
   m_logInfo.m_total=0; 
}



bool CvLog::begin()
{
   theLoggerProcessID = theLogger->pBegin();
   return true;
}

//设置日志的通道信息，已经当前处理的文件名
void CvLog::setLogInfo(const int &i_channelNo,const string &r_fileName,const string &r_fileNameOut)
{
	m_logInfo.m_total = 0;
	m_logInfo.m_fullPathName = r_fileName;	
	m_logInfo.m_OutputPathName = r_fileNameOut;
	m_logInfo.m_channelNo = i_channelNo;
	m_logInfo.m_filetype = CConvertConfig::GetInstance()->GetSwitchName(i_channelNo);
		
}

//处理的话单累计
void CvLog::addLogCdrs()
{	
  m_logInfo.m_total++;  
}

int CvLog::getCdrCount()
{
	return m_logInfo.m_total;
}
//写话单
bool CvLog::write()
{
	  theLogger->pInput(theLoggerProcessID,m_logInfo.m_fullPathName.c_str(),m_logInfo.m_total);
    theLogger->pOutput(theLoggerProcessID,m_logInfo.m_OutputPathName,m_logInfo.m_total,
      "channel_Index=%d,file_type=%s,totalcdr_Processed = %d\n",m_logInfo.m_channelNo,m_logInfo.m_filetype,m_logInfo.m_total);  
    return true;
}

//日志提交
bool CvLog::commit()
{
   write();
   theLogger->pEnd(theLoggerProcessID);
   return true;
}
