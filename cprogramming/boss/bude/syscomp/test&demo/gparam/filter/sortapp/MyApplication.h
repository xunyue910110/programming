// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// Editors:     Yang Zhang  <zhangy@lianchuang.com>
// Update       20011030
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
//
// Comments:
//

#ifndef MYAPPLICATION_H_INCLUDED_C4353D06
#define MYAPPLICATION_H_INCLUDED_C4353D06

#include "config-all.h"

//#include "dbi/oradbi.h"  

#include "State.h"
#include "Directory.h"
#include "Application.h"
#include "SortApplication.h"

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <string>
USING_NAMESPACE(std)

#include "SortLog.h"
extern SortLog * const theSortLog;

#define SLEEP_TIME   20
#define MAX_FILES    100

//##ModelId=3BCAA26200AF
class MyApplication : public Application
{
 public:
  virtual ~MyApplication();

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

  void setPathIn(const char *path);

  int getFile(char *filename, const int length);

 private:
  SortApplication *m_SortApplication;

  string m_sourcePath;

  int m_pathNo;

  vector <string> m_srcPath;

  void writeLog();

  string getNextPath();

  int m_fileSum;//一个目录下已经处理的文件数

  Directory m_dir;

  Directory::SortFlags m_fsortmethod;

};



#endif /* MYAPPLICATION_H_INCLUDED_C4353D06 */
