// ##########################################
// Source file : StandardFile.cpp
// System      : Mobile Billing System
// Version     : 0.1.17
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010925
// Update      : 20020325
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <unistd.h>
#include <sys/stat.h>

#include "SendLog.h"
#include "StandardFile.h"
#include "MyApplication.h"
#include "base/StringUtil.h"
extern MyApplication g_application;

//##ModelId=3BB1954002BF
StandardFile::StandardFile()
{
    m_fDes    =0;
    m_fInvalid    =0;
    m_cdrFlag = CdrFlag::getInstance();
    m_SendLog   = SendLog::getInstance();

}

//##ModelId=3BB18C7F01EA
//重置输出标准文件句柄
bool StandardFile::reset(const string outputFileName)
{
    char tmp[30];
	  string theDesFileName;
	  //string errFileName,errPath;
	  string theErrFileName;
	  string theIndbFileName;
	  
	  m_FileName     = outputFileName;
	  theDesFileName = m_desBakPath+m_FileName;

    //错误记录输出    
    m_invalidFileName  = "E"+m_FileName;
    theErrFileName = m_desBakPath+m_invalidFileName;
    
    //入库记录输出
    if(m_isIndb==true)
    {
      m_indbFileName = "I"+m_FileName;
      theIndbFileName = m_desBakPath+m_indbFileName;
    }
    
	  setNowTime();
    
	  if( m_fDes != 0 ) 
	  {
	     if( fclose(m_fDes)!=0 )
	     {
	          theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                       "致命错误：关闭上一个输出文件"+m_FileName+"失败，程序退出 !");
            #ifndef PPTEST
                 g_application.emergyExitAlert();
            #endif 
            exit(1);
       }
	     m_fDes=0;
	  }

    if ( (m_fDes=fopen(theDesFileName.c_str(),(char *)"w")) == NULL )   //打开输出文件错误
    {
        theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE,"open des file("+m_FileName+")  error.