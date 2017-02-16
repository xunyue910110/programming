// ##########################################
// Source file : SortRule_6.h
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020321
// Update      : 20020321
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTRULE_6_H_INCLUDED
#define SORTRULE_6_H_INCLUDED

#include "SortCdr.h"
#include "SortLog.h"
extern SortLog * const theSortLog;


//## 天津移动BOSS一次分拣（MSC/SSP话单）即分拣漫游话单和省内话单
class SortRule_6 : public SortCdr
{
 public:

    virtual string getChannel(char * & cdr);

 private:

    string pocSort();

    string otherSort();

    string nationalSort();

};

#endif /* SORTRULE_6_H_INCLUDED */
