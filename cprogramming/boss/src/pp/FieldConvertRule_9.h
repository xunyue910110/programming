// ##########################################
// Source file : FieldConvertRule_9.h
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

#ifndef FIELDCONVERTRULE_9_H_HEADER_INCLUDED_B7A6574B
#define FIELDCONVERTRULE_9_H_HEADER_INCLUDED_B7A6574B

#include "FieldConvertRule.h"


//##ModelId=485618E3029F
//##此规则整型实现字段之间+,-,*,/操作
class FieldConvertRule_9 : public FieldConvertRule
{
  public:
  	FieldConvertRule_9(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=48561903004E
    virtual bool initialize(string ruleText);

    //##ModelId=4856190E0157
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=4856192F02EE
    vector<string> m_desFieldName;

    //##ModelId=485619380000
    vector<char *> m_desOperat;

    //##ModelId=485619420213
    vector<string> m_operatLeftFieldName;

    //##ModelId=4856194A01E4
    vector<string> m_operatRightFieldName;
    
    vector<int>         m_iSrcLeftSeqs;
    vector<int>         m_iSrcRightSeqs;  
	vector<FieldInfo *> m_pDesFields;


};



#endif /* FIELDCONVERTRULE_9_H_HEADER_INCLUDED_B7A6574B */
