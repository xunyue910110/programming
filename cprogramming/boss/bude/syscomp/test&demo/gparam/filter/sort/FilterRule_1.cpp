// ##########################################
// Source file : FilterRule_1.cpp
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20011217
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "FilterRule_1.h"

//##ModelId=3C1DC0C10098
//##Documentation
//## 无效话单判断模块
bool FilterRule_1::isTrashCdr(int callType,char *imsi,char *otherParty,char *msisdn)
{
    bool flag;
    int cType;
    
    // 01:MOC; 02:MTC; 03:FORW; 04:ROAM; 07:SMMO; 08:SMMT; 11:POC
    
    flag  = false;
    switch (callType)
    {
       case 1:        // MOC
          break;
       case 2:        // MTC
          break;
       case 3:        // FORW
          break;
       case 7:        // SMMO
          if( strncmp(imsi,"46000",5) == 0 || strncmp(imsi,"46001",5) == 0 || strncmp(imsi,"46002",5) == 0) 
          {
             flag = true;   // 国内短信话单为无效话单
          }
          break;
       case 8:        // SMMT
          if( strncmp(imsi,"46000",5) == 0 || strncmp(imsi,"46001",5) == 0 || strncmp(imsi,"46002",5) == 0) 
          {
             flag = true;   // 国内短信话单为无效话单
          }
          break;
       case 11:       // POC专网话单
          if( strncmp(msisdn,"13",2)==0) 
          {
             flag = true;
          }
          break;
       default :      // is trash cdr
          flag = true;
          break;
    }
    
    return flag;
}


