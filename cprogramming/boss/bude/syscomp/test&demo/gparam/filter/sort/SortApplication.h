// ##########################################
// Source file : SortApplication.h
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020524
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef FILTERAPPLICATION_H_INCLUDED_C426B96C
#define FILTERAPPLICATION_H_INCLUDED_C426B96C

#include "config-all.h"
#include "supdebug.h"

#include "SysParam.h"

#include "SortCdr.h"
#include "Channel.h"            // edit by zhangy in 2002.4.24
#include "FilterCdr.h"
#include "ReadStandardCdr.h"
#include "WriteStandardCdr.h"
#include "SortRule_1.h"
#include "SortRule_2.h"
#include "SortRule_3.h"
#include "SortRule_4.h"
#include "SortRule_5.h"
#include "SortRule_6.h"
#include "SortRule_Gprs.h"

#include "SortLog.h"

#include "PM_Filter.h"
#include "ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

//##ModelId=3BD933FC0075
//##Documentation
//## 分拣处理类
class SortApplication
{
 public:
   //##ModelId=3BD9342C03D1
   bool initialize(const int channelNo,SysParam *theSysParam, string root);

   //##ModelId=3BD9346B0255
   bool run(const string fileName,const int datasource);

   //##ModelId=3BDE8EC60389
   SortApplication();

   //##ModelId=3BDE8EE502DA
   ~SortApplication();

   //##ModelId=3BDFB2340055
   void close(int flag);   // flag =0 ,not error; =1, is error


 private:
   //##ModelId=3BDFAE3D0182
   int reset(const string fileName,const int datasource); // edit by zhangy in 2002.5.24

   //##ModelId=3BDE9A5C03B6
   SortCdr * createRuleObject(const string theSortRule);

   //##ModelId=3C1CA2E40056
   bool updata();

 private:

   //##ModelId=3BDD073C0093
   ReadStandardCdr m_ReadStandardCdr;

   //##ModelId=3BDD079902DC
   WriteStandardCdr m_WriteStandardCdr;

   //##ModelId=3BDD07400175
   vector<SortCdr *> m_SortCdr;

   //##ModelId=3C3AAE6E0337
   bool checkTokenCount(char token,const int datasource);

   //##ModelId=3BDEA71401D7
   char m_cdrBuffer[CDR_BUFFER_SIZE];

   //##ModelId=3C1CA1D30221
   PM_Filter m_PM;

   //##ModelId=3C3AACDD0055
   vector<int> m_fieldCount;

   //##ModelId=3C887866006F
   double m_runTimes;

   //##ModelId=3C887884007C
   int m_runCDRs;


};



#endif /* FILTERAPPLICATION_H_INCLUDED_C426B96C */
