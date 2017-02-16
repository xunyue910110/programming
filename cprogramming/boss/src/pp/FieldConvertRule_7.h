// ##########################################
// Source file : FieldConvertRule_7.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080903
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_7_H_HEADER_INCLUDED_B7A63DF8
#define FIELDCONVERTRULE_7_H_HEADER_INCLUDED_B7A63DF8

#include "FieldConvertRule.h"
#include "FieldInfo.h"
class FieldInfos;

//##ModelId=4856180103D8
//##此转换用于把以时分秒表示的时长转换为以秒为单位的时长
class FieldConvertRule_7 : public FieldConvertRule
{
  public:
  	FieldConvertRule_7(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485618140186
    virtual bool initialize(string ruleText);

    //##ModelId=4856181B02CE
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485618240119
    string m_hour;

    //##ModelId=4856182C033C
    string m_minute;

    //##ModelId=4856183501A5
    string m_second;

    FieldInfo *m_pFieldDu;
	FieldInfo *m_pFieldDU_hour;
	FieldInfo *m_pFieldDU_minute;
	FieldInfo *m_pFieldDU_second;
	
};



#endif /* FIELDCONVERTRULE_7_H_HEADER_INCLUDED_B7A63DF8 */
