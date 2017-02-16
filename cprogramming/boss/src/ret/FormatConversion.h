// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     cheng haidong
//
// History:
// Copyright (C) Linkage. All Rights Reserved.
//
// Comments:
//
#ifndef FORMATCONVERSION_H_INCLUDED_C48750DB
#define FORMATCONVERSION_H_INCLUDED_C48750DB
#include "config-all.h"
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "math.h"
#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

USING_NAMESPACE(std);

class FormatConversion
{
 public:
  int static rtrim(char *);
  int static ltrim(char *);
  int static alltrim(char *);
  int static changeFormatLs(char *,int);
  int static changeFormatLz(char *,int);
  int static changeFormatRz(char *,int);
  int static changeFormatRs(char *,int);
  int static digitCheck(const char *);
  int static xdigitCheck(const char *);
  int static toNumber(char *);
  int static numToChar1 (char *, double , int , int );
  int static copyFile(const char *,const char *);
  long static timeDiff(const char *);
  int static addSecond(const char *,const long ,char *);
  int static dayadd(char *,char *,long );
  void static intToHex(char *);
  int  static tt(char);
  void static hexToInt(char *,char *);
};
#endif
