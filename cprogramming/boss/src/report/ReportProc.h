// #############################################
// Source file : ReportProc.h
// System    : Mobile Billing System
// Version   : 1.00
// Language     : ANSI C++
// OS Platform : HPUX
// Authors   : Wang Xiaoxin
// E_mail    : wangxx@lianchuang.com
// Create    : 20010827
// Update    : 20010921 
// Copyright(C): 2001 by Wang Xiaoxin, Linkage.
// #############################################


#ifndef REPORTPROC_H
#define REPORTPROC_H
#include <string.h>
#include "ReadParam.h"
#include "dbi/dbi.h"
#include "Report.h"
#include "appfrm/ErrorHandle.h"
//#include "globalobj.h"
#include <iostream>

extern char dbServer[64];
extern char dbUser[64];
extern char dbPassword[64];
extern char dbName[64];
extern char reportTableName[64];

#define MAXFILEID "ZZ"

class ReportProc//## 汇总处理类。根据输入参数接收、发送汇总文件
{
 public:
  ReportProc(TypdayVector *,TypotherVector *);
  ~ReportProc();

  TypdayVector *m_dayVector;
  TypotherVector *m_otherVector;
  int callProcedure();//## 调存储过程
  int getDataFromDB();//## 从数据库中取数据到文件
  int getFileName(int,int,char *);//## 取得汇总文件名
  int getDataFromFile();//## 将汇总记录从文件读出到容器
  int connectDb();
  int mainProcess();//## 汇总处理主过程
  int FileIdAdd(char *chFileId);
  
  int m_flag;
  char m_fileName[30];//## 汇总文件名
  char m_fileDate[8+1];

 protected:
  ErrorMessages *m_theErrorMessages;
  DbInterface dbi;
  int m_reportFlag;
  int m_reportNo;
  char m_procedureName[64];//## 汇总过程名
  char m_tableName[64];//## 汇总处理表名
  char m_fullBackupName[80];//## 汇总备份文件名（含路径）
  char m_fullFileName[80];//## 汇总文件名（含路径）
};
#endif
