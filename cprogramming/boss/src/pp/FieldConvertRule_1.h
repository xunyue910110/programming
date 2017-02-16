// ##########################################
// Source file : FieldConvertRule_1.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080925
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_1_H_HEADER_INCLUDED_B7A640A5
#define FIELDCONVERTRULE_1_H_HEADER_INCLUDED_B7A640A5

#include "config-all.h"
USING_NAMESPACE(std)

#include <vector>
#include "FieldConvertRule.h"



//##ModelId=4856149500EA
//##用于对字段值的代码统一化
class FieldConvertRule_1 : public FieldConvertRule
{
  public:
  	FieldConvertRule_1(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485614B302CE
    virtual bool initialize(string ruleText);

    //##ModelId=485614C00213

    virtual bool convertField();
    //##ModelId=485614D20157
    virtual ~FieldConvertRule_1();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485614F10232
    string m_srcFieldName;
	
	FieldInfo *m_pSrcFieldInfo;
    //##ModelId=485614FD0167
    vector<string> m_srcFieldValues;

    //##ModelId=4856150C036B
    vector<string> m_desFieldValues;

};



#endif /* FIELDCONVERTRULE_1_H_HEADER_INCLUDED_B7A640A5 */
