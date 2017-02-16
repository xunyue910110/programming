// ##########################################
// Source file : CvLog.h
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

#ifndef _CVLOG_H
#define _CVLOG_H

#include <map>
#include <string>
#include "base/PString.h"

#include "CConvertConfig.h"


//USING_NAMESPACE(std)



class CvLog
{
		
 public:
		typedef struct _ST_LOGINFO
		{
		    int      batch_id;          //进程号
		    int      m_total;           //记录数		    
		    string  m_fullPathName;       //全路径文件名
		    int  m_channelNo;        //子通道号
		    string  m_filetype;         //  话单类型  ：对应的交换机
		    string  m_OutputPathName;   //输出文件全路径名
		}LOGINFO;
    CvLog();
    ~CvLog();

    void initialize();

    void setLogInfo(const int &i_channelNo,const string &r_fileName,const string &r_fileNameOut);
    void addLogCdrs();
    //map<PString,LOGINFO> &getLogs();

    bool begin();  //日志处理初始化
    bool write();
    bool commit(); //日志处理结束
    int  getCdrCount();
 public:
    time_t    m_beginDealTime;//开始处理时间
    time_t    m_endDealTime;  //结束处理时间
		   
 private:
    LOGINFO  m_logInfo;
  
};

#endif 
