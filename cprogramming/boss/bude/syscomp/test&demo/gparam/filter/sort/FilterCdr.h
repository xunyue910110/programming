// ##########################################
// Source file : FilterCdr.h
// System      : Mobile Billing System
// Version     : 0.1.2
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20011217
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FILTERCDR_H_INCLUDED_C422AB14
#define FILTERCDR_H_INCLUDED_C422AB14

#include "config-all.h"


#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif


//##ModelId=3BD9451F0376
//##Documentation
//## 过滤话单记录类(抽象类)
class FilterCdr
{
 public:

    virtual ~FilterCdr(){;}; // add by zhangy in 2002.11.13

    //##ModelId=3C1DE447004E
    virtual bool isTrashCdr(int callType,char *imsi,char *otherParty,char *msisdn)=0;

};



#endif /* FILTERCDR_H_INCLUDED_C422AB14 */
