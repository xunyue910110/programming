// ##########################################
// Source file : FieldConvertRule_14.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20060509
// Update      : 20060509
// Copyright(C): 2006 by Yang Zhang, Linkage.
// ##########################################

#include "config-all.h"

#include <vector>
#include "FieldConvertRule.h"
USING_NAMESPACE(std)

//## gprs 内容计费表示形式:(均采用十机制)业务代码|上行流量|下行流量多个业务直接用;隔开.现支持4个业务一条话单
class FieldConvertRule_14 : public FieldConvertRule
{
	public:
		virtual bool initialize(string ruleText);
		virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
		virtual ~FieldConvertRule_14(){}
	
	private:
		string m_conditionExp;
		
  	    string m_conditionFieldName;

	    string m_conditionFieldValue;
	
		// 配置文件中record段中标识用户流量内容的字段名称
		string m_serviceIdName;
		
		// 配置文件中record段中 上下行流量字段名称和时长字段名称
		string m_DataUpName;
		string m_DataDnName;
		string m_DurationName;
		
		// 配置文件中outPut段中 标识:"业务代码|上行流量|下行流量"的字段名称
		string m_cbc;
		
		// 存放原始话单中 上下行流量,业务代码的"值"的vector	
		vector<string> m_serviceId;
		vector<string> m_dataFlowUps;
		vector<string> m_dataFlowDns;
		vector<string> m_durations;
		
		FieldInfo *m_cbcInfo;

};
