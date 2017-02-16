// ##########################################
// Source file : FieldConvertRule_6.h
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

#ifndef FIELDCONVERTRULE_6_H_HEADER_INCLUDED_B7A66EF4
#define FIELDCONVERTRULE_6_H_HEADER_INCLUDED_B7A66EF4

#include "FieldConvertRule.h"
class FieldInfos;


//##ModelId=4856178400EA
class FieldConvertRule_6 : public FieldConvertRule
{
  public:
  	FieldConvertRule_6(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=48561791005D
    virtual bool initialize(string ruleText);

    //##ModelId=485617980109
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485617A0035B
    int m_replaceType;

    //##ModelId=485617C4003E
    vector<string> m_srcField;

    //##ModelId=485617CC001F
    vector<string> m_desField;

	vector<FieldInfo *> m_pSrcFields;
	vector<FieldInfo *> m_pDesFields;
};



#endif /* FIELDCONVERTRULE_6_H_HEADER_INCLUDED_B7A66EF4 */
