// ##########################################
// Source file : CDRConvertApp.h
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

#ifndef MYAPPLICATION_H_HEADER_INCLUDED_B795212C
#define MYAPPLICATION_H_HEADER_INCLUDED_B795212C

#include "config-all.h"
#include "appfrm/Application.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <iostream>
#endif

#include <string>
#include "CCdrBase.h"
#include "CCdrConverstion.h"
#include "CvLog.h"




USING_NAMESPACE(std);

#define SLEEP_TIME   20
#define MAX_FILES    100

extern ErrorMessages * const theErrorMessages;

//话单转换主应用基类
class CDRConvertApp : public FileApplication
{
  public:
    //##ModelId=48571D190271
    virtual ~CDRConvertApp();

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
		bool getNextChanInfo();
		
		//获得话单文件，话单文件的产生日期，以及输出文件名
		bool GetCdrFileName(string &sFileName,string &sSrcFileDate,string &sFileNameOut);		
		
		bool BackupFile(string sSrcFileName,string sDestFileName);
		
		//获取当前扫描通道目录下所有文件存入到容器vecFiles中
		bool GetAllFiles(vector<string> & vecFiles);
		
		bool ProcessFile(const string & sSrcFileName,const string& sDestFileName,const string&sSrcFileDate);
		
		void test();
  private:
  
  	//读cv.cfg配置文件，获得配置信息
    bool  GetConfigInfo();

    //写日志
    void writeLog();   
    
    

  private:
    //##ModelId=48571DB5034B
    
    class VecSort
    {
    	public:
    	  bool operator()(const string &left,const string &right); 
    };
    CCdrBase *m_pCdrBase;
    //##ModelId=48571DC100DA
    string  m_sourcePath;

    //##ModelId=48571DCE036B
    string  m_desPath;

   
    //##ModelId=486AF41F00EA
    int m_filecount;

    //##ModelId=486AF4570128
    int m_proccount;

    //##ModelId=486AF47E03C8
    int m_fileSum;

    //##ModelId=486AF498005D
    int m_channelSequnce;
    
    string m_roamPath;
    
    string m_longPath;
    
    string m_sFileDate;
    
    bool m_isSplitLongCdr;
    
    

};



#endif /* MYAPPLICATION_H_HEADER_INCLUDED_B795212C */
