// ##########################################
// Source file : log.h
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020308
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "Log.h"

Log* Log::m_log = NULL;

void Log::reset()
{
	m_total       = 0;
	m_merged      = 0;
	m_pushed2pool = 0;
	m_pool2file   = 0;
	m_overTimeCdr = 0;
	m_fileName    = "";
	setCurrentDateTime();
}

void Log::setCurrentDateTime()
{
    struct tm* stNowTime;
	char tmp[15];
	
	time(&m_tNowTime);       //获得日历时间，以秒为单位
	stNowTime = localtime(&m_tNowTime); //将日历时间转换成年月日时分秒的样式
    memset(tmp,0,sizeof(tmp));
	sprintf(tmp,"%04d%02d%02d%02d%02d%02d",stNowTime->tm_year + 1900,stNowTime->tm_mon+1,stNowTime->tm_mday,
									stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
	
	m_curDateTime = tmp;
}

time_t Log::getStartDealTime_tm() const
{
	return m_tNowTime;	
}

string Log::getCurrentTimeDealCdr()
{
    time_t tmp;
    
    char str[50];
    
    time(&tmp);
    
    sprintf(str,"%ld",tmp);
    
    return string(str);
}
