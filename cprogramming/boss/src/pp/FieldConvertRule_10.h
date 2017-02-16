// ##########################################
// Source file : FieldConvertRule_10.h
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

#ifndef FIELDCONVERTRULE_10_H_HEADER_INCLUDED_B7A64B08
#define FIELDCONVERTRULE_10_H_HEADER_INCLUDED_B7A64B08

#include "DateTime.h"
#include "FieldConvertRule.h"
#include "DataFormat.h"
#include "CdrFlag.h"

class FieldInfos;

//##ModelId=4856196A00BB
//##此转换用于把以秒为单位的时间转换为时分秒表示的时间
class FieldConvertRule_10 : public FieldConvertRule
{
  public:
  	FieldConvertRule_10(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=4856197A0177
    virtual bool initialize(string ruleText);

    //##ModelId=4856198000AB
    virtual bool convertField();

    //##ModelId=4856198902FD
    virtual ~FieldConvertRule_10();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485619EB030D
    DateTime m_baseDateTime;

    //##ModelId=485619F90119
    string m_startFieldName;

    //##ModelId=48561A0202AF
    string m_endFieldName;

	FieldInfo *m_pFieldDu;
	FieldInfo *m_pFieldStartDu;
	FieldInfo *m_pFieldEndDu;

};



#endif /* FIELDCONVERTRULE_10_H_HEADER_INCLUDED_B7A64B08 */
