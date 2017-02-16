// ############################################
// Source file : FilterApp.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : shenglj
// E_mail      : shenglj@lianchuang.com
// Create      : 2008-7-7
// Update      : 2008-7-7
// Copyright(C): shenglj, Linkage.
// ############################################

#ifndef FILTERAPP_H_INCLUDED_20080707
#define FILTERAPP_H_INCLUDED_20080707

#include "config-all.h"
#include "base/Directory.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "base/SysParam.h"
#include "base/supdebug.h"
#include "base/StringUtil.h"

#include "SortLog.h"
#include "InputerCdr.h"
#include "OutputerCdr.h"
#include "Processorall.h"
#include "PMGlobalManager.h"
#include "FilterGlobalInfo.h"


#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <string>

USING_NAMESPACE(std)

//#define SLEEP_TIME   20
//#define MAX_FILES    100
const int SLEEP_TIME = 3;

class FilterApp : public Application
{
public:
    FilterApp();

    ~FilterApp();

protected:
    virtual bool processopt(int optopt, const char *optarg);
    virtual bool initialization();
    virtual bool recovery();
    virtual bool beforeLoop();
    virtual bool loopProcess();
    virtual bool end();
private:
    void    writeLog();
    bool    getSysParam();
   
   //事务开始
    bool begin();
   //事务提交
    bool commit();
   //事务回退
    bool rollback();

    int filterRun();

    bool writeFerrCdr(); //异常话单输出

    FilterConfig      m_config;
    SortLog           m_sortLog;
    FilterGlobalInfo  m_globalInfo;
    InputerCdr        m_inputercdr;
    OutputerCdr       m_outputercdr;
    Processorall      m_processor;
    PMGlobalManager   m_pmGlobalManager;
    Inoutputer        m_ferroutcdr; //异常话单输出

    string            m_mdbErrorMsg;
    
};



#endif /* FILTERAPP_H_INCLUDED_20080707 */
