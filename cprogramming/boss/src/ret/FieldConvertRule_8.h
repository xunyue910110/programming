// ##########################################
// Source file : FieldConvertRule_8.h
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030109
// Update      : 20030109
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_8_H_INCLUDED_C3809835
#define FIELDCONVERTRULE_8_H_INCLUDED_C3809835

#include "FieldConvertRule.h"
#include "FieldInfo.h"
class RecordFormat;

//##ModelId=3C7F4CED02A6
// filed2的字段值是"200#0#02#02"，此规则用来得到其中的子字段值
class FieldConvertRule_8 : public FieldConvertRule
{
  public:

	//##ModelId=3C7F4D3A0076
	virtual bool initialize(string ruleText);

	//##ModelId=3C7F578300D6
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:

	//##ModelId=3C7F983D0001
  	string m_conditionFieldName;
  	
  	string m_conditionExp;

	//##ModelId=3C7F985F008C
  	string m_conditionFieldValue;

	char m_myToken[10];		//分割符

	string m_compositeFieldName;	//复合字段名

	int m_postion;			//子字段顺序号

	string m_dstFieldNames;		//子字段名

};

#endif 
