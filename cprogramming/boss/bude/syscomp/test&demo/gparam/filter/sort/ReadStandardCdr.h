// ##########################################
// Source file : ReadStandardCdr.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020318
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef READSTANDARDCDR_H_INCLUDED_C422F616
#define READSTANDARDCDR_H_INCLUDED_C422F616

#include "supdebug.h"
#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <string>
#include <stdio.h>

USING_NAMESPACE(std);

#include "ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//##ModelId=3BD934F603C8
//##Documentation
//## 得到标准话单记录(抽象类)
class ReadStandardCdr
{
 public:
   //##ModelId=3BD944980123
   bool initialize(string errorPath);

   //##ModelId=3BDEA6330382
   bool reset(const string fileName);

   //##ModelId=3BDD6168000A
   bool getCdr(char * cdrBuffer,const int n);

   //##ModelId=3BDEA63303BF
   bool close(int flag);   // flag =0 ,not error; =1, is error

 private:

   //##ModelId=3BDD64050175
   string m_fileName;

   //##ModelId=3BDD6413017F
   FILE *m_fileFp;

   //##ModelId=3C98A7F10099
   string m_errorPath;
};



#endif /* READSTANDARDCDR_H_INCLUDED_C422F616 */
