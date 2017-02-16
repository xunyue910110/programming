// ##########################################
// Source file : FieldValidateRule_2.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011122
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef VALIDRULE_2_H_INCLUDED_C45527BB
#define VALIDRULE_2_H_INCLUDED_C45527BB

#include "FieldValidateRule.h"

//##ModelId=3BAAA41B008C
class FieldValidateRule_2 : public FieldValidateRule
{
  public:
	//##ModelId=3BAAA7DF0060
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAAA29017C
	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3BFC77C3007F
	string m_validValue;

};



#endif /* VALIDRULE_2_H_INCLUDED_C45527BB */
