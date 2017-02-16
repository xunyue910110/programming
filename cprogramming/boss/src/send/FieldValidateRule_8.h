// ##########################################
// Source file : FieldValidateRule_8.h
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

#ifndef FIELDVALIDATERULE_8_H_INCLUDED_C45527BB
#define FIELDVALIDATERULE_8_H_INCLUDED_C45527BB
#include <map>
#include "FieldValidateRule.h"

// 此校验方法专用于对给定的字段进行非法字符校验
class FieldValidateRule_8 : public FieldValidateRule
{
  public:

	virtual bool initialize(string ruleText);

	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	string m_fieldName;

};



#endif 
