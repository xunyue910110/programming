// ##########################################
// Source file : SortRule_5.h
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20020105
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTRULE_5_H_INCLUDED_C3C943C4
#define SORTRULE_5_H_INCLUDED_C3C943C4

#include "SortCdr.h"

#include "SortLog.h"
extern SortLog * const theSortLog;

//##ModelId=3C36FAE901FA
//##Documentation
//## 只需根据msisdn和imsi绑定用户资料处理的分拣处理处理
class SortRule_5 : public SortCdr
{
 public:

   //##ModelId=3C36FC5D0282
   virtual string getChannel(char * & cdr);
};



#endif /* SORTRULE_5_H_INCLUDED_C3C943C4 */
