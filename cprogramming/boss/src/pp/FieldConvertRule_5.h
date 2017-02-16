// ##########################################
// Source file : FieldConvertRule_5.h
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

#ifndef FIELDCONVERTRULE_5_H_HEADER_INCLUDED_B7A62A7E
#define FIELDCONVERTRULE_5_H_HEADER_INCLUDED_B7A62A7E

#include "FieldConvertRule.h"
#include "FieldInfo.h"
class FieldInfos;

class RecordFormat;

//##ModelId=485617190261

/*规则5定义了对已有字段的substr功能、去非数字字符的功能、把BCD号码中的‘A’转换成‘0’。*/
class FieldConvertRule_5 : public FieldConvertRule
{
  public:
  	FieldConvertRule_5(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=4856172803D8
    virtual bool initialize(string ruleText);

    //##ModelId=4856172F0290
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485617500242
    vector<string> m_srcField;

    //##ModelId=485617580119
    vector<string> m_desField;

    //##ModelId=4856176102EE
    vector<string> m_substart;

    //##ModelId=485617690251
    vector<string> m_subLength;


    FieldInfo *m_pFieldTarget;
    vector<FieldInfo *> m_pSrcFields;
    vector<FieldInfo *> m_pDesFields;
    vector<int>         m_iSubStartSeqs;
    vector<int>         m_iSubLengthSeqs;    

};



#endif /* FIELDCONVERTRULE_5_H_HEADER_INCLUDED_B7A62A7E */
