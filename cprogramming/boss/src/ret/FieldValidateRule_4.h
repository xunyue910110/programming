// ##########################################
// Source file : FieldValidateRule_4.h
// System      : Mobile Billing System
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : min chen
// E_mail      : chenm@lianchuang.com
// Create      : 20020602
// Update      : 20020602
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef VALIDRULE_4_H_INCLUDED_C45527BB
#define VALIDRULE_4_H_INCLUDED_C45527BB
#include <map>
#include "FieldValidateRule.h"

//##ModelId=3BAAA41B008C
class FieldValidateRule_4 : public FieldValidateRule
{
  public:
	//##ModelId=3BAAA7DF0060
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAAA29017C
	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3BFC77C3007F
	map<string,int> m_FileNoMap;

	int m_iSubSwitchStart;
	int m_iSubSwitchLenth;
	int m_iSubFileStart;
	int m_iSubFileLenth;

};



#endif /* VALIDRULE_2_H_INCLUDED_C45527BB */
