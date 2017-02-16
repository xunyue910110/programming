// ##########################################
// Source file : FilterRule_1.h
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20011217
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FILTERRULE_1_H_INCLUDED_C422A086
#define FILTERRULE_1_H_INCLUDED_C422A086

#include "FilterCdr.h"

//##ModelId=3BDD1C3B0006
//##Documentation
//## 海南移动BOSS过滤处理专用规则（MSC话单）
class FilterRule_1 : public FilterCdr
{
 public:
   //##ModelId=3C1DC0C10098
   virtual bool isTrashCdr(int callType,char *imsi,char *otherParty,char *msisdn);

};



#endif /* FILTERRULE_1_H_INCLUDED_C422A086 */
