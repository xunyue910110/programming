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

#ifndef LOG_H_INCLUDED_C37B75A3
#define LOG_H_INCLUDED_C37B75A3

#include "config-all.h"

#include <vector>
#include <string>


USING_NAMESPACE(std)

// 以singleton模式，提供日志对象
class Log
{
	public:
		void reset();
		void addTotal()      {m_total++; }
		void addMerged()     {m_merged++;}      // 已经合并完成而且输出的
		void addPushed2pool(){m_pushed2pool++;} // 压入缓存等待合并的
		void addPool2file()  {m_pool2file++;}   // 超过时间阀值缓存大小,直接输出到文件中的
		void addOverTime()   {m_overTimeCdr++;}  //等待超时的话单数
		
		void setFileName(const string fileName) { m_fileName = fileName ;}
		
		int getTotal()       {return m_total;}
		int getMerged()      {return m_merged;} 
		int getPushed2pool() {return m_pushed2pool;}
		int getPool2file()   {return m_pool2file;}
		int getOverTime()    {return m_overTimeCdr;}
		string getFileName() {return m_fileName;}
		string getCurrentTimeDealCdr();
		
		static Log * getInstance()
		{
			if( m_log != NULL )
				return m_log;
			else
			{
				m_log = new Log();
				return m_log;	
			}	
		}
		time_t getStartDealTime_tm() const;
		void setCurrentDateTime();
	
	private:
		Log()
		{
			reset();
		}
		
	private:
		int m_total;          // 总话单数
		int m_merged;         // 被合并的话单数
		int m_pushed2pool;    // 被压入缓存的话单数
		int m_pool2file;      // 从缓存中提取出来的话单数
		int m_overTimeCdr;    // 超时的话单数
		
		string m_fileName;
		time_t m_tNowTime;
		string m_curDateTime; 
		
		
		static Log *m_log;     
};

#endif 
