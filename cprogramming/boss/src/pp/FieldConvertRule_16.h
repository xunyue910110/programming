// ##########################################
// Source file : FieldConvertRule_16.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_16_H_HEADER_INCLUDED_B7A6158F
#define FIELDCONVERTRULE_16_H_HEADER_INCLUDED_B7A6158F


#include "FieldConvertRule.h"
#include "appfrm/ErrorHandle.h"
#include "RepeatableFieldInfo.h"
class FieldInfos;

extern ErrorMessages * const theErrorMessages;
class RecordFormat;

//##ModelId=4856268A0271
//查找列表类型数据中的第N个，如果N＝-1,则查找最后一个
class FieldConvertRule_16 : public FieldConvertRule
{
  public:
  	FieldConvertRule_16(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485626B500FA
    virtual bool initialize(string ruleText);

    //##ModelId=485626BC030D
    virtual bool convertField();

    //##ModelId=485626EA0000
    virtual ~FieldConvertRule_16();

  protected:
  	virtual void getFieldObj();

  private:
  	//##ModelId=3C7F86320153
	string m_srcFieldName;

	//##ModelId=3C7F982B0217
  	string m_desFieldName;

	//##ModelId=3C7F980F0249
  	int m_iPosition;

    RepeatableFieldInfo *m_pFieldSrc;
    FieldInfo *m_pFieldDes;
};



#endif /* FIELDCONVERTRULE_16_H_HEADER_INCLUDED_B7A6158F */
