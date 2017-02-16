// ##########################################
// Source file : FieldConvertRule_4.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011212
// Update      : 20011222
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_4_H_INCLUDED_C3E96E98
#define CONVERTRULE_4_H_INCLUDED_C3E96E98

#include "FieldConvertRule.h"

//##ModelId=3C16C7830297
//##Documentation
//## 此转换程序用于对字段值的替换。
//## 类型0用于对首位开始的串进行替换；
//## 类型1用于对整个字段内的串进行替换
class FieldConvertRule_4 : public FieldConvertRule
{
  public:
	//##ModelId=3C16C83D0227
	virtual bool initialize(string ruleText);

	//##ModelId=3C16C8470036
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3C16C7AB0082
	int m_replaceType;

	//待转换的字段名称
	string m_fieldName;

	//待替换字段值中的"被替换值"
	string m_srcValue;

	//待替换字段值中的"替换值"
	string m_desValue;

	string m_conditionExp;

	//逻辑条件字段名称
	string m_conditionFieldName;

	//逻辑条件字段值
	string m_conditionFieldValue;

};

#endif /* CONVERTRULE_4_H_INCLUDED_C3E96E98 */
