// ##########################################
// Source file : FieldConvertRule_12.h
// System      : Mobile Billing System
// Authors     : Chen Min
// E_mail      : chenm@lianchuang.com
// Create      : 20020704
// Update      : 
// Copyright(C): 2002 by Chen Min, Linkage.
// ##########################################

#ifndef CONVERTRULE_12_H_INCLUDED_C4555C9F
#define CONVERTRULE_12_H_INCLUDED_C4555C9F

#include <map>
#include <vector>
#include "FieldConvertRule.h"

//对于nortel的呼转话单,其lac要从相应的mtc话单中提取,本规则实现这一提取

class FieldConvertRule_12 : public FieldConvertRule
{
  public:
	virtual bool initialize(string ruleText);
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldMap);

  private:
	map< string,vector<FieldInfo> >       m_Map03;
	map<string,string>                    m_MapFW;
	string                                m_StrKey; 

};

#endif /* CONVERTRULE_7_H_INCLUDED_C4555C9F */
