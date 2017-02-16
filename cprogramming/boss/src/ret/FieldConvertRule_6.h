// ##########################################
// Source file : FieldConvertRule_6.h
// System      : Mobile Billing System
// Version     : 0.0.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020331
// Update      : 20020331
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_6_H_INCLUDED_C35A6EF2
#define CONVERTRULE_6_H_INCLUDED_C35A6EF2

#include "FieldConvertRule.h"

//##ModelId=3CA5E8D9003F
class FieldConvertRule_6 : public FieldConvertRule
{
  public:

	//##ModelId=3CA5F3D303E2
	virtual bool initialize(string ruleText);

	//##ModelId=3CA5F3D4007C
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3CA5F3D30161
	int m_replaceType;

	//逻辑条件的字段名称
	string m_conditionFieldName;

	//逻辑条件的字段值
	string m_conditionFieldValue;

	string m_conditionExp;

	//##ModelId=3CA5F700019C
	string m_srcField;

	//##ModelId=3CA5F7090311
	string m_desField;

};

#endif /* CONVERTRULE_6_H_INCLUDED_C35A6EF2 */
