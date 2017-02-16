// ##########################################
// Source file : MyApplication.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef MYAPPLICATION_H_HEADER_INCLUDED_B795212C
#define MYAPPLICATION_H_HEADER_INCLUDED_B795212C

#include "config-all.h"
#include "PpLog.h"
#include "appfrm/Application.h"
#include "base/Directory.h"
#include "PreProcApplication.h"
#include "log4cpp/Logger.h"

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <string>
USING_NAMESPACE(std);

#define SLEEP_TIME   20
#define MAX_FILES    100

//##ModelId=486AF2BD034B
// 支持处理多输入通道
struct channelInfo
{
  public:
    //##ModelId=486AF2C80222
    channelInfo(const char *pChanNo, int iIsShow)
    {
        m_pInPutPathStr = new string;
        m_pOutPutPathStr= new string;
        m_pProcessor    = new PreProcApplication(pChanNo,iIsShow);
    }

    //##ModelId=486AF2F70138
    ~channelInfo()
    {
        if(m_pProcessor != 0)
        {
            delete m_pProcessor;
        }
        if(m_pInPutPathStr != 0 )
        {
            delete m_pInPutPathStr;
        }
        if(m_pOutPutPathStr != 0)
        {
            delete m_pOutPutPathStr;
        }
        
    }

  public:
    //##ModelId=486AF306006D
    string *m_pInPutPathStr;

    //##ModelId=486AF31200CB
    string *m_pOutPutPathStr;

    //##ModelId=486AF31E0213
    PreProcApplication *m_pProcessor;

};


//##ModelId=48571CE10271
class MyApplication : public FileApplication
{
  public:
    //##ModelId=48571D190271
    virtual ~MyApplication();

    //##ModelId=48571D2F0232
    void emergyExitAlert()
    {
        writeAlert();
    }

  protected:
    //##ModelId=48571D5900FA
    virtual bool processopt(int optopt, const char *optarg);

    //##ModelId=48571D77035B
    virtual bool initialization();

    //##ModelId=48571D860232
    virtual bool recovery();

    //##ModelId=48571D95034B
    virtual bool loopProcess();

    //##ModelId=48571DA40203
    virtual bool end();

  private:
    //##ModelId=48571DFA009C
    bool getPath(string path, vector <string> &pathMessage, channelInfo *pChanInfo);

    //##ModelId=48571E18033C
    void writeLog();

    //##ModelId=48571E24958F
    // 获取下一个预处理输入通道路径名和相应的处理类指针
    bool getNextChanInfo();

    //##ModelId=486B1B940177
    // 获取pp.cfg中的配置信息,并初始化处理器
    bool getSysParam();

    //##ModelId=486B10987AF4
    void setPpVersion();

  private:
    //##ModelId=48571DB5034B
    PreProcApplication *m_PreProcApplication;

    //##ModelId=48571DC100DA
    string* m_sourcePath;

    //##ModelId=48571DCE036B
    string* m_desPath;

    //##ModelId=48571DD903D8
    PpLog *m_PpLog_;

    //##ModelId=486AF41F00EA
    int m_filecount;

    //##ModelId=486AF4570128
    int m_proccount;

    //##ModelId=486AF47E03C8
    int m_fileSum;

    //##ModelId=486AF48A02EE
    vector<channelInfo *> m_channelInfos;

    //##ModelId=486AF498005D
    int m_channelSequnce;
    
    string m_roamPath;
    
    string m_longPath;
    
    bool m_isSplitLongCdr;

};



#endif /* MYAPPLICATION_H_HEADER_INCLUDED_B795212C */
