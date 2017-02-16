// ##########################################
// Source file : FieldConvertRule.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080903
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_H_HEADER_INCLUDED_B7A62E4C
#define FIELDCONVERTRULE_H_HEADER_INCLUDED_B7A62E4C

#include "config-all.h"

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#include "CdrFlag.h"
#include "FieldInfo.h"
#include "PpLog.h"
#include "DateTime.h"
#include "CdrErrorCode.h"
#include "AlertCode.h"
#include "appfrm/ErrorHandle.h"
#include "base/StringUtil.h"
#include "PpConstDef.h"


extern ErrorMessages * const theErrorMessages;

class FieldInfos;
//##ModelId=48560F86008C
class FieldConvertRule
{
  public:
    //##ModelId=48561297034B
    FieldConvertRule(const int iSubChannelNum);

    //##ModelId=4856129A032C
    virtual ~FieldConvertRule();

    //##ModelId=485612B3038A
    virtual bool initialize(string ruleText) = 0;

    //##ModelId=485612D301C5
    virtual bool convertField() = 0;
    
  protected:
    //##ModelId=485613D9006D
    int strnrps(const char *src, const char *token, const char *rps, char *des, int max);

    //##ModelId=485614260261
    void editTime(int duration);
    
    virtual void getFieldObj();
    
    // 如果已经在fieldInfos中定义的字段 返回下标 否则 返回-1
    void getFieldSequnceByName(const vector<string> &fieldNames
    								,vector<int> &fieldSequnces);
    								
    bool generalCondJudge();								

	string lrTrim(string theString);
  protected:
    //##ModelId=485614440242
    CdrFlag *m_CdrFlag;

    //##ModelId=4856146303C8
    PpLog *m_PpLog;

    //##ModelId=485614720186
    string m_callType;

    FieldInfos *m_pFieldInfos;
    
    FieldInfo *m_pFieldCallType;
	FieldInfo *m_pFieldStartDate    ;
    FieldInfo *m_pFieldStartTime    ;
	FieldInfo *m_pFieldEndDate      ;
	FieldInfo *m_pFieldEndTime      ;
    FieldInfo *m_pFieldCondition;

    string m_conditionFieldName;
    string m_conditionFieldValue;
    string m_conditionExp;
	
};



#endif /* FIELDCONVERTRULE_H_HEADER_INCLUDED_B7A62E4C */
