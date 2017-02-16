// ##########################################
// Source file : SortCdr.h
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020308
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef SORTLOG_H_INCLUDED_C37B75A3
#define SORTLOG_H_INCLUDED_C37B75A3

#include <map>
#include <string>
#include "base/PString.h"

#include "config-all.h"
#include "ConstDef.h"
#include "OutChanInfo.h"

#include "FilterGlobalInfo.h"

/*
#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif
*/

//USING_NAMESPACE(std)

struct LOGINFO
{
    int      batch_id;          //进程号
    int      m_total;           //记录数
    int      inout_tag ;        //输出输出标记
    //PString  m_fileName;         //处理文件名
    //PString  m_pathName;         //处理文件目录
    PString  m_fullPathName;       //全路径文件名
    PString  m_channelNo;        //通道号
    PString  m_outfiletype;      //输出话单类型 0:正常 1:预开卡用户多输出清单 2:其余多分出的清单
};

class SortLog
{
 public:

    SortLog();
    ~SortLog();

    void initialize(FilterGlobalInfo *r_gFilterInfo);

    //void addLogCdrs(const PString &r_channelNo,const PString &r_fileName);
    void addLogCdrs(const PString &r_channelNo,const PString &r_fileName,const PString &r_outfiletype);
    map<PString,LOGINFO> &getLogs();

    bool begin();  //日志处理初始化
    bool write();
    bool commit(); //日志处理结束
    void addFerrCdr(); //增加异常话单蔵

 public:
    time_t    m_beginDealTime;//开始处理时间
    time_t    m_endDealTime;  //结束处理时间
    int       m_ferrCdrs;  //异常话单数(通常是记录数不符合处理规范)

 private:
    FilterGlobalInfo    *m_gFilterInfo;
    map<PString,LOGINFO>  m_logInfo;
};

#endif /* SORTLOG_H_INCLUDED_C37B75A3 */
