// ##########################################
// Source file : WriteStandardCdr.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020106
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef WRITESTANDARDCDR_H_INCLUDED_C422EF75
#define WRITESTANDARDCDR_H_INCLUDED_C422EF75

#include "config-all.h"

#include <vector>
#include <string>
#include <stdio.h>

#if defined(STDIOSTREAM_DISABLE)
    #include <iostream.h>
    #include <fstream.h>
#else
    #include <iostream>
    #include <fstream>
#endif

#include "FilterFp.h"

#include "ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

#include "SortLog.h"
extern SortLog * const theSortLog;

//##ModelId=3BD9455F01B6
//##Documentation
//## 写标准话单(抽象类)
class WriteStandardCdr
{
 public:

   //##ModelId=3BDD07F302EF
   bool setPath(const string channelNo, const string channelPath,const int chNo);

   //##ModelId=3BDD5FF50350
   bool write(string & channel, char* cdr);

   //##ModelId=3BDF9F090262
   void close();

   //##ModelId=3BDFADD802BD
   bool reset(const string fileName,const int chNo);

   //##ModelId=3C366A1D0147
   bool setPrefix(string p,string n,string i,string u);

   //##ModelId=3C200277019A
   WriteStandardCdr();

   //##ModelId=3C37DAD902DA
   vector< vector <FilterFp> > m_fp;

  //##ModelId=3BDF9900018E
   vector<FilterFp> m_FilterFp;

 private:
   //##ModelId=3BDF9E95021F
   FILE *getFilterFp(string & channelNo,const int chNo);

 private:

   //##ModelId=3BDD5F8403C6
   string m_fileName;

   //##ModelId=3BDF5848016A
   string m_invalidPath;

   //##ModelId=3BDF58990289
   FILE *m_invalidFp;

   //##ModelId=3C20018D003F
   string m_tmpFileHead;

   //##ModelId=3C366A5301A9
   vector<string> m_prefixP;

   //##ModelId=3C366A5D02BC
   vector<string> m_prefixN;

   //##ModelId=3C366A6501D7
   vector<string> m_prefixI;

   //##ModelId=3C36F51000D5
   vector<string> m_prefixU;

   //##ModelId=3C37E877008E
   int m_no;



};



#endif /* WRITESTANDARDCDR_H_INCLUDED_C422EF75 */
