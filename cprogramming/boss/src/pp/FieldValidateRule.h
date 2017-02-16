// ##########################################
// Source file : FieldValidateRule.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_H_HEADER_INCLUDED_B7A67FE3
#define FIELDVALIDATERULE_H_HEADER_INCLUDED_B7A67FE3

#include "config-all.h"
#include <string>
#include <map>
#include <vector>

#include "PpLog.h"
#include "CdrFlag.h"
#include "FieldInfo.h"
#include "CdrErrorCode.h"
#include "AlertCode.h"
#include "base/StringUtil.h"

USING_NAMESPACE(std)

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

class FieldInfos;
//##ModelId=48564347031C
class FieldValidateRule
{
  public:
    //##ModelId=4856436500FA
    FieldValidateRule(const int iSubChannelNum);

    //##ModelId=4856436E035B
    virtual ~FieldValidateRule() {;};

    //##ModelId=485643860242
    virtual bool initialize(string ruleText)=0;

    //##ModelId=485643A001C5
    virtual int validField()=0;

    //##ModelId=48564407006D
    void substr(string &srcString, string &desString);

    //##ModelId=48564425034B
    void substr(char * src, char *des);

    //##ModelId=4856443A033C
    bool setSubstr(string &src);

  protected:
 
    bool generalCondJudge();								
    virtual void getFieldObj();
    string  lrTrim(string theString);

  protected:
    //##ModelId=4856449E030D
    string m_fieldName;

    //##ModelId=485644AA008C
    int m_substart;

    //##ModelId=485644B20290
    int m_subLength;

    //##ModelId=485644BB02EE
    PpLog *m_PpLog_;

    //##ModelId=485644C80290
    CdrFlag *m_cdrFlag;

    //##ModelId=485644DE0167
    string m_callType;

    //##ModelId=485644E803B9
    string m_conditionFieldName;

    //##ModelId=485644F0036B
    string m_conditionFieldValue;

    FieldInfos *m_pFieldInfos;

    FieldInfo *m_pFieldCallType;
    FieldInfo *m_pFieldCondition;

};



#endif /* FIELDVALIDATERULE_H_HEADER_INCLUDED_B7A67FE3 */
