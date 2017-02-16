// ##########################################
// Source file : FieldValidateRule_10.h
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

#ifndef FIELDVALIDATERULE_10_H_HEADER_INCLUDED_B7A62924
#define FIELDVALIDATERULE_10_H_HEADER_INCLUDED_B7A62924

#include <map>
#include "FieldValidateRule.h"

//##ModelId=485647BF01A5
// 此校验方法专用于对给定的字段进行全字符校验
class FieldValidateRule_10 : public FieldValidateRule
{
  public:
   	FieldValidateRule_10(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=485647D400DA
    virtual bool initialize(string ruleText);

    //##ModelId=485647DB009C
    virtual int validField();

 
  private:
    //##ModelId=4856480A035B
    string m_fieldName;

};



#endif /* FIELDVALIDATERULE_10_H_HEADER_INCLUDED_B7A62924 */
