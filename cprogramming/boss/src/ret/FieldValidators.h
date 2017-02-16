// ##########################################
// Source file : FieldValidators.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011122
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef PPVALID_H_INCLUDED_C45540CB
#define PPVALID_H_INCLUDED_C45540CB

#include "config-all.h"

#include <string>
#include <vector>

#include "FieldInfo.h"
#include "FieldValidateRule.h"
#include "FieldValidateRule_1.h"
#include "FieldValidateRule_2.h"
#include "FieldValidateRule_3.h"
#include "FieldValidateRule_7.h"
#include "FieldValidateRule_8.h"
#include "CdrErrorCode.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std)

//##ModelId=3B4EA8900119
class FieldValidators
{
  public:
	//##ModelId=3B621699009D
	FieldValidators();

	//##ModelId=3B62169901AB
	~FieldValidators();

	//##ModelId=3BAAAA660101
	bool initialize(const vector<string> &argname);

	//##ModelId=3BAAAA7601C2
	bool validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

	//##ModelId=3BCCEE2602B1
	void setValidType(string left);

  private:
	//##ModelId=3BCC0183008C
	string getRuleName(string &theRuleText);

  private:
	//##ModelId=3BAAA465036E
	vector<FieldValidateRule *> m_ValidRules;

	//##ModelId=3BCCEDAD0375
	string m_ValidType;

	//##ModelId=3BCE5D91032F
	CdrFlag *m_cdrFlag ;

};

#endif /* PPVALID_H_INCLUDED_C45540CB */
