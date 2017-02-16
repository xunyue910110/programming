// ##########################################
// Source file : FieldValidateRule_3.h
// System      : Mobile Billing System
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080922
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_3_H_HEADER_INCLUDED_B7A62467
#define FIELDVALIDATERULE_3_H_HEADER_INCLUDED_B7A62467

#include "FieldValidateRule.h"

//##ModelId=485645A702DE
class FieldValidateRule_3 : public FieldValidateRule
{
  public:
    //##ModelId=485645B302EE
  	FieldValidateRule_3(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    virtual bool initialize(string ruleText);

    //##ModelId=485645BA0203
    virtual int validField();

  private:
    //##ModelId=485645C80157
    vector<string> m_fields;

};



#endif /* FIELDVALIDATERULE_3_H_HEADER_INCLUDED_B7A62467 */
