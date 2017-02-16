// ##########################################
// Source file : FieldConvertRule_9.h
// System      : Mobile Billing System
// Version     : 0.0.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030429
// Update      : 20030429
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ##########################################


#ifndef FIELDCONVERTRULE_9_H_INCLUDED_C35A6EF2
#define FIELDCONVERTRULE_9_H_INCLUDED_C35A6EF2

#include "FieldConvertRule.h"

//##ModelId=3CA5E8D9003F
//此规则实现字段之间的 +,-,*,/ 操作
class FieldConvertRule_9 : public FieldConvertRule
{
  public:

	//##ModelId=3CA5F3D303E2
	virtual bool initialize(string ruleText);

	//##ModelId=3CA5F3D4007C
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:

    // rule_7;01;ALL;cfee=cfee1+cfee2

	//##ModelId=3CA5F45B0012
	string m_conditionFieldName;

	//##ModelId=3CA5F6E201D5
	string m_conditionFieldValue;

	//##ModelId=3CA5F6F900E7
	string m_conditionExp;   	

	//##ModelId=3CA5F700019C
	string m_desFieldName;  
	
	string m_desOperat;

	string m_operatLeftFieldName;     
	string m_operatRightFieldName;

};

#endif 
