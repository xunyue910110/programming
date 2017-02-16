// ##########################################
// Source file : SortCdr.h
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020402
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTCDR_H_INCLUDED_C42284E8
#define SORTCDR_H_INCLUDED_C42284E8

#include "supdebug.h"
#include "config-all.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <algorithm>

#if defined(STDIOSTREAM_DISABLE)
    #include <iostream.h>
    #include <fstream.h>
#else
    #include <iostream>
    #include <fstream>
#endif

#include "Channel.h"
#include "FilterCdr.h"
#include "PM_Filter.h"

#include "InputField.h"
#include "CdrErrorCode.h"
#include "FilterRule_1.h"
#include "FilterRule_2.h"


#include "ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

#include "SortLog.h"
extern SortLog * const theSortLog;

#define CDR_BUFFER_SIZE 1024

USING_NAMESPACE(std);

//##ModelId=3BDCF54501AE
//##Documentation
//## 分拣话单记录类(抽象类)
class SortCdr
{
 public:
   //##ModelId=3BDFD1E5010F
   int getFields(const char *cdrBuffer);

   int getError();

   //##ModelId=3BE21A3800A8
   virtual string getChannel(char * &cdr) = 0;

   //##ModelId=3BE286F10295
   bool initialize(string theFilterRule,PM_Filter *thePM);

   //##ModelId=3BE360D90005
   virtual ~SortCdr();

   //##ModelId=3BDD07E502EF
   void setInputField(const string fieldName,const string position);

   //##ModelId=3BDF727A00E1
   void setOutputField(const string fieldName,const string position);

   //##ModelId=3BDFD69000D1
   int getField(const char *cdrBuffer, int position, char *fieldValue);

   //##ModelId=3C1DE24C0154
   bool isTrashCdr(int &callType);

  //##ModelId=3C02EC6B00DD
   char *addFields(int errorNo);

 public:

   //##ModelId=3BDEB4200126
   vector<InputField> m_inputField;

   //##ModelId=3BDEBF2300C5
   vector<InputField> m_outputField;

 protected:
   //##ModelId=3BE89AA50016
   // flag = true,无条件;flag = false, 当值为空时
   bool setFieldValue(int position,string value,bool flag);

   //##ModelId=3C1F30E40310
   string countSum(int channel);


   //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码信息
   void bindInfo(ServInfo &servInfo,ServInfo &servInfo_other);

   //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码信息
   void bindInfo(ServInfo2 &servInfo2,ServInfo &servInfo_other);
   
   
 protected:

   //##ModelId=3BE270170225
   PM_Filter *m_PM;

   //##ModelId=3C1DBAC801F8
   int m_callType;

   //##ModelId=3C33F72B0296
   char m_homeAreaCode[10];

   //##ModelId=3BE7B06F0086
   char m_cdrBuffer[CDR_BUFFER_SIZE];

   //##ModelId=3BDEB86001F3
   char m_IMSI[30];

   //##ModelId=3BDEBA22025E
   char m_startDate[30];

   //##ModelId=3C1DBAC80053
   char m_startTime[30];

   //##ModelId=3BE1F4FA006D
   char m_errorCode[30];

   //##ModelId=3C1DBAC802F2
   char m_otherParty[30];

   //##ModelId=3C1DBAC90023
   char m_outTrunkId[30];

   //##ModelId=3C1DBAC90131
   char m_inTrunkId[30];

   //##ModelId=3C1DBAC9022B
   char m_msisdn[30];

   //##ModelId=3C1EEEA70141
   char m_dateTime[30];

   //##ModelId=3C33F7530171
   char m_rateInd[10];

   //##ModelId=3C3707210216
   char m_Msc[30];  
   
   char m_roamType[80];     	 
   
   char m_sourceType[80];   	 


 private:
   //##ModelId=3BE20AB002F0
   int getNextToken(const char *cdrBuffer,int offset,int len);

   //##ModelId=3BDD07430238
   FilterCdr *m_FilterCdr;

};

#endif /* SORTCDR_H_INCLUDED_C42284E8 */
