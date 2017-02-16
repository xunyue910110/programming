// ##########################################
// Source file : FieldValidateRule_8.h
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

#ifndef FIELDVALIDATERULE_8_H_HEADER_INCLUDED_B7A67133
#define FIELDVALIDATERULE_8_H_HEADER_INCLUDED_B7A67133
#include <map>
#include "FieldValidateRule.h"

//##ModelId=48564748033C
class FieldValidateRule_8 : public FieldValidateRule
{
  public:
  	FieldValidateRule_8(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=4856475901A5
    virtual bool initialize(string ruleText);

    //##ModelId=48564760001F
    virtual int validField();

  private:
    //##ModelId=4856477B033C
    string m_fieldName;

};



#endif /* FIELDVALIDATERULE_8_H_HEADER_INCLUDED_B7A67133 */
