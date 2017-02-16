// ##########################################
// Source file : FieldConvertRule_15.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_15_H_HEADER_INCLUDED_B7A64BAD
#define FIELDCONVERTRULE_15_H_HEADER_INCLUDED_B7A64BAD

#include "config-all.h"

#include <vector>
#include "FieldConvertRule.h"
class FieldInfos;
class RepeatableFieldInfo;
USING_NAMESPACE(std)


//##ModelId=4856238C032C
//## 提供gprs流量链表中的上下行流量和时长,根据"PDP的开始时间"累计忙闲时流量、时长的功能
class FieldConvertRule_15 : public FieldConvertRule
{
  public:
  	FieldConvertRule_15(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=4856239F0399
    virtual bool initialize(string ruleText);

    //##ModelId=485623A8005D
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485623BA0261
    bool getFields(const string&ruleStr, string &totalStr, string &srcStr);

    //##ModelId=4856242901D4
    long diffSecond(const string &dt1, const string &dt2);

    //##ModelId=4856244E01B5
    // 配置文件中record段中标识用户流量发生时间的字段名称
    string m_sTimeFieldName;

    //##ModelId=485624A403A9
    // 配置文件中record段中上下行流量字段名称
	string m_srcDtName;
	string m_srcDtDnName;

    //##ModelId=485624B7035B
    // 配置文件中outPut段中上下行流量字段名称和时长字段名称
 	string m_totalDtUpName;
	string m_totalDtDnName;
	string m_strDuratName;

    //##ModelId=48562564009C
    // 存放原始话单中上下行流量、时长的 "值"的vector	
	vector<string> m_sTimes;
	vector<string> m_dataFlowUps;
	vector<string> m_dataFlowDns;

    //##ModelId=4856258E03B9
    FieldInfo *m_pFieldDateUp1;
    FieldInfo *m_pFieldDateUp2;
    FieldInfo *m_pFieldDateDn1;
    FieldInfo *m_pFieldDateDn2;
    FieldInfo *m_pFieldDurat1;
    FieldInfo *m_pFieldDurat2;
    
    RepeatableFieldInfo *m_pStartTimes;
    RepeatableFieldInfo *m_pDataFlowUps;
    RepeatableFieldInfo *m_pDataFlowDns;

};



#endif /* FIELDCONVERTRULE_15_H_HEADER_INCLUDED_B7A64BAD */
