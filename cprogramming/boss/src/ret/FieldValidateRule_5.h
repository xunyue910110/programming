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

#ifndef VALIDRULE_5_H_INCLUDED_C45527BB
#define VALIDRULE_5_H_INCLUDED_C45527BB
#include <map>
#include <time.h>
#include "FieldValidateRule.h"

//##########################################
//##此校验规则用于统计交换机所下话单的时效性
//##########################################

class FieldValidateRule_5 : public FieldValidateRule
{
  public:
	//##ModelId=3BAAA7DF0060
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAAA29017C
	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3BFC77C3007F
	map<string,int> m_FileNoMap;
	
	// 用于考核的时间量(语音是开始日期和时间,短信是提交日期和时间)
	string m_checkDate;
	string m_checkTime;

	int m_iRange1;
	int m_iRange2;
	int m_iRange3;
	int m_iRange4;
	int m_iRange5;
	int m_iRange6;
};



#endif /* VALIDRULE_5_H_INCLUDED_C45527BB */
