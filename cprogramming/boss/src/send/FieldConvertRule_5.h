// ##########################################
// Source file : FieldConvertRule_5.h
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020301
// Update      : 20020302
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_5_H_INCLUDED_C3809835
#define CONVERTRULE_5_H_INCLUDED_C3809835

#include "FieldConvertRule.h"
#include "FieldInfo.h"
class RecordFormat;

//##ModelId=3C7F4CED02A6

class FieldConvertRule_5 : public FieldConvertRule
{
  public:

	//##ModelId=3C7F4D3A0076
	virtual bool initialize(string ruleText);

	//##ModelId=3C7F578300D6
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:

	//##ModelId=3C7F983D0001
  	string m_conditionFieldName;

	//##ModelId=3C7F985F008C
  	string m_conditionFieldValue;

  	//##ModelId=3C7F86320153
	string m_srcField;

	//##ModelId=3C7F982B0217
  	string m_desField;

	//##ModelId=3C7F980F0249
  	string m_substart;

	//##ModelId=3C7F984A00A0
	string m_subLength;

	string m_conditionExp;
};

#endif /* CONVERTRULE_5_H_INCLUDED_C3809835 */
