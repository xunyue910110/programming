// ##########################################
// Source file : FieldConvertRule_15.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011211
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_15_H_INCLUDED_C4550A6B
#define CONVERTRULE_15_H_INCLUDED_C4550A6B

#include "config-all.h"

#include <vector>
#include "FieldConvertRule.h"

//cbc流量结构
class CbcListOfFlow
{
	public:
		CbcListOfFlow()
		{
			m_serviceId = "";
			m_dataUp    = 0;
			m_dataDn    = 0;
			m_duration  = 0;
		}
	public:
		string m_serviceId;
		int    m_dataUp;
		int    m_dataDn;
		long   m_duration;
};


//## 提供gprs流量链表中的上下行流量和时长,根据时段累计的功能
class FieldConvertRule_15 : public FieldConvertRule
{
	public:
		virtual bool initialize(string ruleText);
		virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
		virtual ~FieldConvertRule_15(){}
	
	private:
		bool getFields(const string &ruleStr,string &totalStr,string &srcStr);
		long diffSecond(const string & dt1,const string & dt2);
	
	private:
		// 配置文件中record段中标识用户流量发生时间的字段名称
		string m_serviceID;
		
		// 配置文件中record段中 上下行流量字段名称和时长字段名称
		string m_srcDtName;
		string m_srcDtDnName;
		
		// 配置文件中outPut段中 上下行流量字段名称和时长字段名称
		string m_totalDtUpName;
		string m_totalDtDnName;
		string m_strDuratName;
		string m_totalDuratName;
		
		// 存放原始话单中 上下行流量、时长的 “值” 的vector	
		vector<string> m_serviceIDs;
		vector<string> m_dataFlowUps;
		vector<string> m_dataFlowDns;
		vector<string> m_durations;
		          
		vector<CbcListOfFlow *> m_pCbcLists; 

};

#endif 
