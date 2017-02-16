// ##########################################
// Source file : FieldValidateRule_7.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080924
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_7_H_HEADER_INCLUDED_B7A66E73
#define FIELDVALIDATERULE_7_H_HEADER_INCLUDED_B7A66E73

#include <map>
#include <time.h>
#include "FieldValidateRule.h"
//#include "FieldInfos.h"

//##########################################
//##此校验规则用于统计交换机所下话单的时效性
//##########################################

//##ModelId=485646F50148
class FieldValidateRule_7 : public FieldValidateRule
{
  public:
   	FieldValidateRule_7(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=485647040232
    virtual bool initialize(string ruleText);

    //##ModelId=4856470B0196
    virtual int validField();

  private:
    //##ModelId=48564714031C
    map<string,int> m_FileNoMap;

    //##ModelId=485647200000
    // 用于考核的时间量(语音是开始日期和时间,短信是提交日期和时间)
    string m_checkDate;

    //##ModelId=4856472803C8
    string m_checkTime;

    //##ModelId=485647320271
    int m_iMaxTime;

    //##ModelId=4856473A00CB
    int m_iMinTime;
    
    string m_Exp;

};



#endif /* FIELDVALIDATERULE_7_H_HEADER_INCLUDED_B7A66E73 */
