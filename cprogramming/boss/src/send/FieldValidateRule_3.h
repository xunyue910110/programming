// ##########################################
// Source file : FieldValidateRule_3.h
// System      : Mobile Billing System
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011017
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef VALIDRULE_3_H_INCLUDED_C4550CB7
#define VALIDRULE_3_H_INCLUDED_C4550CB7

#include "FieldValidateRule.h"

//##ModelId=3BAAA42400CB
class FieldValidateRule_3 : public FieldValidateRule
{
  public:
	//##ModelId=3BAAA7E20029
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAAA2C0053
	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  public:
	//##ModelId=3BFCC4890187
   vector<string> m_fields;
};



#endif /* VALIDRULE_3_H_INCLUDED_C4550CB7 */
