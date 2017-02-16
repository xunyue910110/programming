// ##########################################
// Source file : FieldConvertors.h
// System      : Mobile Billing System
// Version     : 1.03
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020331
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef PPCONVERT_H_INCLUDED_C4836372
#define PPCONVERT_H_INCLUDED_C4836372

#include "config-all.h"

#include <string>
#include <vector>
#include "FieldInfo.h"
#include "FieldConvertRule.h"
#include "FieldConvertRule_1.h"
#include "FieldConvertRule_2.h"
#include "FieldConvertRule_6.h"
#include "FieldConvertRule_13.h"

USING_NAMESPACE(std)

//##ModelId=3B564F4D0008
class FieldConvertors
{
  public:
	//##ModelId=3B62167703A1
	FieldConvertors();

	//##ModelId=3B62167800BD
	~FieldConvertors();

	//##ModelId=3B5799C20283
	bool initialize(const vector<string> &argname);

	//##ModelId=3BAAE9EC00A1
	bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3BAED5CB0291
	string getRuleName(string &theRuleText);

  private:
	//##ModelId=3BAAC09D009A
	vector<FieldConvertRule *> m_ConvertRules;
};

#endif /* PPCONVERT_H_INCLUDED_C4836372 */
