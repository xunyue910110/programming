// ##########################################
// Source file : FilterFp.h
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011031
// Update      : 20020105
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FILTERFP_H_INCLUDED_C420236D
#define FILTERFP_H_INCLUDED_C420236D

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <string>
#include <vector>
#include <stdio.h>

USING_NAMESPACE(std);

//##ModelId=3BDF987200E9
class FilterFp
{
 public:
   //##ModelId=3BDF989C02E8
   FILE *m_filterFp;

   //##ModelId=3BDF98AD0012
   string m_channelNo;

   //##ModelId=3BDF98D60007
   string m_filterPath;

   //##ModelId=3C35BE150308
   string m_prefix;

 private:


};



#endif /* FILTERFP_H_INCLUDED_C420236D */
