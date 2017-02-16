// ##########################################
// Source file : FieldValidateRule_9.h
// System      : Mobile Billing System
// Version     : 1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : chengm@lianchuang.com
// Create      : 20021114
// Update      : 20021114
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_9_H_INCLUDED_C45527BB
#define FIELDVALIDATERULE_9_H_INCLUDED_C45527BB
#include "FieldValidateRule.h"

// 此校验方法专用于对给定字段进行连续性校验
//  rule_9;fields;ALL;CDR_NO
class FieldValidateRule_9 : public FieldValidateRule
{
  public:

	virtual bool initialize(string ruleText);

	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	string m_fieldName;

};

#endif
