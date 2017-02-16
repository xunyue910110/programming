// ##########################################
// Source file : FieldConvertRule_1.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011211
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_1_H_INCLUDED_C4550A6B
#define CONVERTRULE_1_H_INCLUDED_C4550A6B

#include "config-all.h"
USING_NAMESPACE(std)

#include <vector>
#include "FieldConvertRule.h"

//##ModelId=3BAAC0200087
//##Documentation
//## 用于对字段值的代码统一化
class FieldConvertRule_1 : public FieldConvertRule
{
  public:
	//##ModelId=3BAAEAB90073
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAEAE90145
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
	
	virtual ~FieldConvertRule_1(){}

 private:

  private:
	//##ModelId=3BAEDEB00046
  	string m_srcFieldName;

	//##ModelId=3C19CAA3014C
	vector<string> m_srcFieldValues;

	//##ModelId=3C19CABD03CA
	vector<string> m_desFieldValues;

};

#endif /* FieldConvertRule_1_H_INCLUDED_C4550A6B */
