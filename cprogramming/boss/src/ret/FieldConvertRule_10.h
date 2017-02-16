// ##########################################
// Source file : FieldConvertRule_10.h
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

#ifndef CONVERTRULE_10_H_INCLUDED_C31CF69D
#define CONVERTRULE_10_H_INCLUDED_C31CF69D

#include "FieldConvertRule.h"
#include "FieldInfo.h"
#include "DataFormat.h"
#include "CdrFlag.h"

//## 此转换用于把以以秒为单位的时间转换为时分秒表示的时间值。
class FieldConvertRule_10 : public FieldConvertRule
{
  public:
	//##ModelId=3CE364420011
	bool initialize(string ruleText);

	//##ModelId=3CE3645003C8
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
	virtual ~FieldConvertRule_10(){}	

  private:
    void addField(multimap<MyString,FieldInfo *> &theFieldInfoMaps,const string &fieldName,
					const string &fieldValue,const int filenLength);
  
  private:
	//##ModelId=3CE3647D0124
	DateTime m_baseDateTime;
	
	//以秒为单位的开始时间和结束时间字段名
	string m_startFieldName;
	string m_endFieldName;
	
};

#endif /* CONVERTRULE_7_H_INCLUDED_C31CF69D */
