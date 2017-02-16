// ##########################################
// Source file : FieldConvertRule_7.h
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020516
// Update      : 20020516
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_7_H_INCLUDED_C31CF69D
#define CONVERTRULE_7_H_INCLUDED_C31CF69D

#include "FieldConvertRule.h"
#include "FieldInfo.h"

//##ModelId=3CE3641B0228
//##Documentation
//## 此转换用于把以时分秒表示的时长转换为以秒为单位的时长。
class FieldConvertRule_7 : public FieldConvertRule
{
  public:
	//##ModelId=3CE364420011
	bool initialize(string ruleText);

	//##ModelId=3CE3645003C8
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3CE3647D0124
	string m_hour;

	//##ModelId=3CE364890185
	string m_minute;

	//##ModelId=3CE36490026C
	string m_second;

	//##ModelId=3CE369A3035A
	string m_conditionFieldName;

	//##ModelId=3CE369AB0117
	string m_conditionFieldValue;
	
	string m_conditionExp;
};

#endif /* CONVERTRULE_7_H_INCLUDED_C31CF69D */
