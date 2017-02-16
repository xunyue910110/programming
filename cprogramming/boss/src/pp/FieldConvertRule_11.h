// ##########################################
// Source file : FieldConvertRule_11.h
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

#ifndef FIELDCONVERTRULE_11_H_HEADER_INCLUDED_B7A6327C
#define FIELDCONVERTRULE_11_H_HEADER_INCLUDED_B7A6327C

#include "config-all.h"

#include  <vector>
#include "FieldConvertRule.h"

USING_NAMESPACE(std)

class RepeatableFieldInfo;
class FieldInfos;
//##ModelId=48561A4102EE
//##提供gprs流量链表中的上下行流量和时长，根据时段累计的功能
class FieldConvertRule_11 : public FieldConvertRule
{
  public:
  	FieldConvertRule_11(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=48561A510251
    virtual bool initialize(string ruleText);

    //##ModelId=48561A570261
    virtual bool convertField();

    //##ModelId=48561A620222
    virtual ~FieldConvertRule_11();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=48561A72003E
    bool getFields(const string &ruleStr, string &totalStr, string &srcStr);

    //##ModelId=48561A9D03B9
    long diffSecond(const string &dt1, const string &dt2);

    //##ModelId=48561ACB03C8
    //配置文件中record段中标识用户流量发生时间的字段名称
    string m_sTimeFieldName;

    //##ModelId=48561AD60000
    //配置文件中record段中上下行流量字段名称
    string m_srcDtUpName;
    string m_srcDtDnName;

    //##ModelId=48561ADE0203
    //配置文件中output段中上下行流量字段名称和时长字段名称
    string m_totalDtUpName;
    string m_totalDtDnName;
    string m_strDuratName;

    //##ModelId=48561B5002CE
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



#endif /* FIELDCONVERTRULE_11_H_HEADER_INCLUDED_B7A6327C */
