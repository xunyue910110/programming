// ##########################################
// Source file : SortRule_2.h
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

#ifndef SORTRULE_2_H_INCLUDED
#define SORTRULE_2_H_INCLUDED

#include "SortCdr.h"
#include "SortLog.h"
extern SortLog * const theSortLog;


//## 天津移动BOSS专用分拣处理（MSC/SSP话单）
class SortRule_2 : public SortCdr
{
 public:

    virtual string getChannel(char * & cdr);

 private:

    string pocSort();

    string otherSort();

    string nationalSort();

    string PSort();   //专网分检 add by liux 2004.09.09 
};

#endif /* SORTRULE_2_H_INCLUDED */
