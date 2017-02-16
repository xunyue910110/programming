
// ##########################################
// Source file : FieldConvertRule_64.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language        : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080925
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_64_H_HEADER_INCLUDED_B7A640A5
#define FIELDCONVERTRULE_64_H_HEADER_INCLUDED_B7A640A5

#include "config-all.h"
USING_NAMESPACE(std)

#include <vector>
#include "FieldConvertRule.h"
class FieldInfos;


//##ModelId=4856149500EA
//##用于对字段值的代码统一化
class FieldConvertRule_64 : public FieldConvertRule
{
  public:
    //##ModelId=485614B302CE
    virtual bool initialize(string ruleText);
     FieldConvertRule_64(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485614C00213

    virtual bool convertField();
    //##ModelId=485614D20157
    virtual ~FieldConvertRule_64();

  private:
    //##ModelId=485614F10232
        //##ModelId=3C7F983D0001
        string m_conditionFieldName;

        //##ModelId=3C7F985F008C
        string m_conditionFieldValue;

        string m_conditionExp;
    
    string m_acctField;
    string m_useField;
    bool ismon;

};



#endif /* FIELDCONVERTRULE_64_H_HEADER_INCLUDED_B7A640A5 */
