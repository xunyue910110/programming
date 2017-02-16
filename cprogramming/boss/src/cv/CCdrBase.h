// ##########################################
// Source file : CCdrBase.h
// System      : Mobile Billing System
// Version     : bss1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : jianchun Shi
// E_mail      : shijc@lianchuang.com
// Create      : 20090807
// Update      : 20090807
// Copyright(C): 2009 by ShiJianChun, Linkage.
// ##########################################

#ifndef  _CDRBASE_H
#define _CDRBASE_H

#include "base/supdebug.h"
#include "config-all.h"

#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "appfrm/ErrorHandle.h"
#include "CvLog.h"
USING_NAMESPACE(std);

extern ErrorMessages * const theErrorMessages;
double gettime();


//## 话单处理基类
class CCdrBase
{
  public:
    //##ModelId=48508FB1030D
    CCdrBase();   

    //##ModelId=4850C2310148
    ~CCdrBase();

    //##ModelId=4850C3D401F4
    virtual bool Init();
    virtual bool process();  
    virtual bool process(string sCdrFileName,string sCdrFileNameOut,int iChannelIndex); 
	  void   setCdrFileDate(string sCdrFileDate) {m_CdrFilesDate = sCdrFileDate;};
		CvLog  m_cvLog;
		string m_CdrFilesDate;
  private:
   
   void WriteLog(); 
   
};

#endif 