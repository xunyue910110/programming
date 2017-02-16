// ##########################################
// Source file : FieldValidators.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATORS_H_HEADER_INCLUDED_B7A679BB
#define FIELDVALIDATORS_H_HEADER_INCLUDED_B7A679BB


#include "config-all.h"

#include <string>
#include <vector>

#include "FieldInfo.h"
#include "FieldValidateRule.h"

#include "CdrErrorCode.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std)

//##ModelId=4850D4360186
class FieldValidators
{
  public:
    //##ModelId=4850D44801A5
    FieldValidators();

    //##ModelId=4850D44B00FA
    ~FieldValidators();

    //##ModelId=4850D45802BF
    bool initialize(const vector<string> &argname,const int iSubChannelNum);

    //##ModelId=4850D46D00DA
    bool validField();

    //##ModelId=4850D48400CB
    void setValidType(string left);

  private:
    //##ModelId=4850D49B02EE
    string getRuleName(string &theRuleText);

    //##ModelId=4850D4C0029F
    vector<FieldValidateRule *> m_ValidRules;

    //##ModelId=4850D4CF037A
    string m_ValidType;

    //##ModelId=4850D4E302DE
    CdrFlag *m_cdrFlag;

};



#endif /* FIELDVALIDATORS_H_HEADER_INCLUDED_B7A679BB */
