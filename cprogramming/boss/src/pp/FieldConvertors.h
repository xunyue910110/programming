// ##########################################
// Source file : FieldConvertors.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################



#ifndef FIELDCONVERTORS_H_HEADER_INCLUDED_B7A6250A
#define FIELDCONVERTORS_H_HEADER_INCLUDED_B7A6250A

#include "config-all.h"

#include <string>
#include <vector>
#include "FieldInfo.h"
#include "FieldConvertRule.h"
#include "FieldConvertRule_1.h"
#include "FieldConvertRule_2.h"
#include "FieldConvertRule_3.h"
#include "FieldConvertRule_4.h"
#include "FieldConvertRule_5.h"
#include "FieldConvertRule_6.h"
#include "FieldConvertRule_7.h"
#include "FieldConvertRule_8.h"
#include "FieldConvertRule_9.h"
#include "FieldConvertRule_10.h"
#include "FieldConvertRule_11.h"
#include "FieldConvertRule_12.h"
#include "FieldConvertRule_13.h"
#include "FieldConvertRule_14.h"
#include "FieldConvertRule_15.h"
#include "FieldConvertRule_16.h"
#include "FieldConvertRule_17.h"
#include "FieldConvertRule_18.h"
#include "FieldConvertRule_19.h"

USING_NAMESPACE(std)

//##ModelId=4851D3520290
class FieldConvertors
{
  public:
    //##ModelId=4851D36300BB
    FieldConvertors();

    //##ModelId=4851D366001F
    ~FieldConvertors();

    //##ModelId=4851D379030D
    bool initialize(const vector<string> &argname,const int iSubChannelNum);

    //##ModelId=4851D3920261
    bool convertField();

  private:
    //##ModelId=4851D3AE032C
    string getRuleName(string &theRuleText);

    //##ModelId=4851D3C9035B
    vector<FieldConvertRule *> m_ConvertRules;

};



#endif /* FIELDCONVERTORS_H_HEADER_INCLUDED_B7A6250A */
