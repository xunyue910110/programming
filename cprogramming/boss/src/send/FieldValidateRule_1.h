// ##########################################
// Source file : FieldValidateRule_1.h
// System      : Mobile Billing System
// Version     : 0.1.2
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011017
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef VALIDRULE_1_H_INCLUDED_C45540C2
#define VALIDRULE_1_H_INCLUDED_C45540C2

#include "FieldValidateRule.h"

//##ModelId=3BAAA40C00EF
class FieldValidateRule_1 : public FieldValidateRule
{
  public:
	//##ModelId=3BAAA7DB0386
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAAA25008F
	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3BCC1A3702F2
	//##Documentation
	//## 校验类型。类型如下：
	//## 1、字段长度校验
	//## 2、字段值校验
	string m_valudType;


	//##ModelId=3BCC1A6603A4
	//##Documentation
	//## 允许最大值
	int m_max;

	//##ModelId=3BCC1A7A029E
	//##Documentation
	//## 允许最小值
	int m_min;

};

#endif /* VALIDRULE_1_H_INCLUDED_C45540C2 */
