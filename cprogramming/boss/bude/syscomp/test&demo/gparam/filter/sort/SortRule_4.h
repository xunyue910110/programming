// ##########################################
// Source file : SortRule_4.h
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

#ifndef SORTRULE_4_H_INCLUDED_C3C904AA
#define SORTRULE_4_H_INCLUDED_C3C904AA

#include "SortCdr.h"

#include "SortLog.h"
extern SortLog * const theSortLog;

//##ModelId=3C36FAB103E5
//##Documentation
//## 只需根据IMSI绑定用户资料的分拣处理处理
class SortRule_4 : public SortCdr
{
 public:

   //##ModelId=3C36FAD500E3
   virtual string getChannel(char * & cdr);
};



#endif /* SORTRULE_4_H_INCLUDED_C3C904AA */
