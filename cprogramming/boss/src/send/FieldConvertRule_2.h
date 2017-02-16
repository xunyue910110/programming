// ##########################################
// Source file : FieldConvertRule_2.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020516
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_2_H_INCLUDED_C4555C9F
#define CONVERTRULE_2_H_INCLUDED_C4555C9F

#include "FieldConvertRule.h"

//##ModelId=3BAAC0440237
//##Documentation
//## 对以下情况进行转换：
//## 已知module的值，对各种字段进行赋值，这些字段不存在，则创建字段。
class FieldConvertRule_2 : public FieldConvertRule
{
  public:
	//##ModelId=3BAAEABE023D
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAEAFF02AF
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
	
	virtual ~FieldConvertRule_2(){}

  private:
	//##ModelId=3C1A016D001E
    bool getField(const string value);


  private:
	//##ModelId=3BAFF1ED006F
  	string m_conditionFieldName;

	//##ModelId=3C19E10F02FE
	string m_conditionFieldValue;
	
	//##ModelId=3C39462000C8
	string m_conditionExp;

	//##ModelId=3C19F03E0253
	vector<string> m_dstFieldNames;

	//##ModelId=3C19F0500123
	vector<string> m_dstFieldValues;

	//##ModelId=3C19F0A803DC
	vector<string> m_dstExps;

};



#endif /* CONVERTRULE_2_H_INCLUDED_C4555C9F */
