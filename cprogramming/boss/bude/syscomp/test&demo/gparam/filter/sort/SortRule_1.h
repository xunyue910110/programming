// ##########################################
// Source file : ReadRule_1.h
// System      : Mobile Billing System
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20011217
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTRULE_1_H_INCLUDED_C422A5EC
#define SORTRULE_1_H_INCLUDED_C422A5EC

#include "SortCdr.h"
#include "SortLog.h"
extern SortLog * const theSortLog;

//##ModelId=3BDD1E9901A2
//##Documentation
//## 测试用分拣的分拣处理处理
class SortRule_1 : public SortCdr
{
 public:

   //##ModelId=3BE21ACB0140
   virtual string getChannel(char * &cdr);

};



#endif /* SORTRULE_1_H_INCLUDED_C422A5EC */
