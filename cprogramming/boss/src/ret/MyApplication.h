// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// Editors:     Yang Zhang  <zhangy@lianchuang.com>
// Update       20011025
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
//
// Comments:
//

#ifndef MYAPPLICATION_H_INCLUDED_C4353D06
#define MYAPPLICATION_H_INCLUDED_C4353D06

#include "config-all.h"

//#include "State.h"
#include "RetLog.h"
#include "appfrm/Application.h"
#include "base/Directory.h"
#include "RetApp.h"
#include "log4cpp/Logger.h"
#include "RetLogRecord.h"
#include "PpRetLogCdr.h"

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

//##ModelId=3BCAA26200AF
class MyApplication : public FileApplication
{
 public:
  virtual ~MyApplication();
  void emergyExitAlert(){ writeAlert(); }
  Directory m_dir;
  RetLogRecord *m_RetLogRecord;
  
 protected:
  virtual bool processopt(int optopt, const char *optarg);

  //##ModelId=3BCAA280003A
  virtual bool initialization();

  //##ModelId=3BCAA2800076
  virtual bool recovery();

  //##ModelId=3BCAA28000C6
  virtual bool loopProcess();

  //##ModelId=3BCAA28000B2
  virtual bool end();

 private:
  RetApp *m_RetApp;

  string m_sourcePath;
  
  //zaiww added 20060518
  string m_desPath;

  string m_retlogPath;
  
  int m_filecount;

  int m_proccount;
  
  int m_retFileType;

  RetLog *m_RetLog_;

  vector <string> m_fileList;

 private:

  bool getPath(string path,vector <string> &pathMessage);

  void writeLog();

};



#endif /* MYAPPLICATION_H_INCLUDED_C4353D06 */


