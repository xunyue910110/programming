// ##########################################
// Source file : MyApplication..cpp
// System      : NEW GENERATION BILLING SYSTEM
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Zhao Liyang
// E_mail      : zhaoly@lianchuang.com
// Create      : 20081018
// Update      : 20081029
// Copyright(C): 2008 by Zhao Liyang, Linkage.
// ##########################################

#ifndef MYAPPLICATION_H_INCLUDED_C4353D06
#define MYAPPLICATION_H_INCLUDED_C4353D06

#include "SendLog.h"
#include "SendApp.h"

#include "appfrm/Application.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"
#include "appfrm/globalobj.h"

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <string>
USING_NAMESPACE(std)

#define SLEEP_TIME   3
#define MAX_FILES    100

class MyApplication : public FileApplication
{
 public:
  virtual ~MyApplication();
  void emergyExitAlert(){ writeAlert(); }

 protected:
  virtual bool processopt(int optopt, const char *optarg);
  virtual bool initialization();
  virtual bool recovery();
  virtual bool loopProcess();
  virtual bool end();

 private:
  SendApp *m_SendApp;
  SendLog *m_SendLog_;
  vector <string> m_fileList;
  
  string m_sourcePath;
  string m_desPath;
  int m_filecount;
  int m_proccount;

  //文件号相关成员变量：
  //文件号,文件号长度,开始文件号，间隔时间，beforetime
  int m_fileNo;
  int m_fileNoLen;
  int m_firstFileNo,m_intervalTime,m_beforeTime;

  int m_sendFlag;
  string m_fileType;
  string m_logPath;
  string m_rcdFileName; 
  int m_isHeadOrTail;
  int m_createOutName;
  
  string m_nowTime;

 private:

  bool getPath(string path,vector <string> &pathMessage);
  void writeLog();
  int checkFileNo(const int &sendNo);
  int dayadd(char *d,char *s,long n);
  int getFileNo();//获取文件号方法，recovery调用
  int getNowNo();//获取当前的文件号
  bool checkDate(int Year, int Month, int Day);
  void setNowTime(void);
  int writeRcdFile();
};

#endif /* MYAPPLICATION_H_INCLUDED_C4353D06 */
