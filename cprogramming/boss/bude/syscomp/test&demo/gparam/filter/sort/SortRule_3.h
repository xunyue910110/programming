// ##########################################
// Source file : SortRule_3.h
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20011030
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTRULE_3_H_INCLUDED_C422FC59
#define SORTRULE_3_H_INCLUDED_C422FC59

#include "SortCdr.h"

//##ModelId=3BDD1EB50329
//##Documentation
//##  漫游出访分拣处理的分拣处理处理
class SortRule_3 : public SortCdr
{
 public:
   //##ModelId=3BE21AE100FB
   virtual string getChannel(char * & cdr);

};


#endif /* SORTRULE_3_H_INCLUDED_C422FC59 */
