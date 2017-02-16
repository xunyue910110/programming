// ##########################################
// Source file : FieldConvertRule_12.h
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

#ifndef FIELDCONVERTRULE_12_H_HEADER_INCLUDED_B7A6109A
#define FIELDCONVERTRULE_12_H_HEADER_INCLUDED_B7A6109A
#include "FieldConvertRule.h"

class FieldInfos;


//##ModelId=48561EE8038A
//用于浮点型字段之间的 +,-,*,/ 操作
class FieldConvertRule_12 : public FieldConvertRule
{
  public:
  	FieldConvertRule_12(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485629B101A5
    virtual bool initialize(string ruleText);

    //##ModelId=485629BB007D
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=48562A05007D
    vector<string> m_desFieldName;

    //##ModelId=48562A0D0196
    vector<char *> m_desOperat;

    //##ModelId=48562A16003E
    vector<string> m_operatLeftFieldName;

    //##ModelId=48562A1F003E
    vector<string> m_operatRightFieldName;

    vector<int>         m_iSrcLeftSeqs;
    vector<int>         m_iSrcRightSeqs;  
	vector<FieldInfo *> m_pDesFields;

};



#endif /* FIELDCONVERTRULE_12_H_HEADER_INCLUDED_B7A6109A */
