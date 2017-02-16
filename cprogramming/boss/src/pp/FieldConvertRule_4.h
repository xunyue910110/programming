// ##########################################
// Source file : FieldConvertRule_4.h
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

#ifndef FIELDCONVERTRULE_4_H_HEADER_INCLUDED_B7A66A67
#define FIELDCONVERTRULE_4_H_HEADER_INCLUDED_B7A66A67

#include "FieldConvertRule.h"

class FieldConvertRule;
class FieldInfo;

//##ModelId=4856169B005D
//##此转换程序用于对字段值的替换
//##类型0用于对首位开始的串进行替换
//##类型1用于对整个字段内的串进行替换
class FieldConvertRule_4 : public FieldConvertRule
{
  public:
  	FieldConvertRule_4(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485616A9003E
    virtual bool initialize(string ruleText);

    //##ModelId=485616B0003E
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485616BA02BF
    int m_replaceType;

    //##ModelId=485616C5000F
    //##待转换的字段名称
    string m_fieldName;

    //##ModelId=485616CF0213
    //##待替换字段值中的"被替换值"
    string m_srcValue;

    //##ModelId=485616D80290
    //##待替换字段值中的" 字段值"
    string m_desValue;
    
    FieldInfo *m_pFieldTarget;

};



#endif /* FIELDCONVERTRULE_4_H_HEADER_INCLUDED_B7A66A67 */
