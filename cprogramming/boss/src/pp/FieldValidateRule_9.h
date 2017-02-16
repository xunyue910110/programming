// ##########################################
// Source file : FieldValidateRule_9.h
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


#ifndef FIELDVALIDATERULE_9_H_HEADER_INCLUDED_B7A66643
#define FIELDVALIDATERULE_9_H_HEADER_INCLUDED_B7A66643

#include "FieldValidateRule.h"


//##ModelId=485647910119
// 此校验方法专用于对给定字段进行连续性校验
//  rule_9;fields;ALL;CDR_NO
class FieldValidateRule_9 : public FieldValidateRule
{
  public:
   	FieldValidateRule_9(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=4856479C006D
    virtual bool initialize(string ruleText);

    //##ModelId=485647A20196
    virtual int validField();

  private:
    //##ModelId=485647AA003E
    string m_fieldName;

};



#endif /* FIELDVALIDATERULE_9_H_HEADER_INCLUDED_B7A66643 */
