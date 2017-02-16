// ##########################################
// Source file : FilterRule_2.h
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

#ifndef FILTERRULE_2_H_INCLUDED_C422E4EC
#define FILTERRULE_2_H_INCLUDED_C422E4EC

#include "FilterCdr.h"

//##ModelId=3BDD1C6D00B2
//##Documentation
//## 过滤处理规则2(不过滤)
class FilterRule_2 : public FilterCdr
{
 public:
   //##ModelId=3C1DE4470384
   virtual bool isTrashCdr(int callType,char *imsi,char *otherParty,char *msisdn);
};


#endif /* FILTERRULE_2_H_INCLUDED_C422E4EC */
