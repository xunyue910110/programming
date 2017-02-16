/*
* Copyright (c) 2003,Linkage
* All rights reserved.
*
*@文件说明:Oracle数据库接口头文件
*@文件标识:暂无
*@摘    要:
*
*@当前版本:1.1
*@作    者:Cheng HaiDong <chenghd@lianchuang.com>
*@完成日期:2003年
*@修改说明:按编程规范修改
*
*@取代版本:1.0
*@原作者  :Qian YiXin <qianyx@lianchuang.com>
*@原日期  :2002年
*/

#ifndef OUTDB_H_INCLUDED_C4353D06
#define OUTDB_H_INCLUDED_C4353D06

#include "config-all.h"
#include "appfrm/Application.h"
#include "dbi/dbi.h"
#include "sys/param.h"
#include <string>
#include <map>
#include <fstream>

USING_NAMESPACE(std)

#define MAXNAMELEN          100
#define MAXBUFFLEN          200
#define MAXLONGBUFFLEN      2000
#define MAXRECORDBUFFLEN    5000

///基本功能:数据库记录出库
///必配参数:USER,PASSWD,SERVER,SQLSTATEMENT,FIELDNUM,TARGETPATH
///选配参数:DATABASE,SWAPPATH,FIELDLENGTH,ARRAYLINE,OUTPUTFILE
///PROCEDURE,TRUNCATETABLE,FIELDSEP,PROCSTOPCODE

class OutDb : public Application
{
public:
    ///参数容器定义
    typedef map<string, string> SysParamMap;
    SysParamMap m_paramMap;
    ///日志所需的时间
    char m_begin[15];
    char m_end[15];
    ///数据库记录的容器
    StringVector m_errVector;
    StringVector m_outVector;

protected:
    ///重载基类中的方法
    virtual bool initialization();
    virtual bool loopProcess();
    virtual bool end();

private:
    char m_errorMsg[200];///错误信息
    int  m_fieldNum;///当前文件字段数
    int  m_nTotal;///数据库接口用
    int  m_nSuccess;
    int  m_nError;
    int  m_nReturn;
    int  m_nStatis;
    int  m_fieldIndex;

    string m_fileName;///当前的文件名
    DbInterface m_dbi;///数据库接口
    PROCPARAM m_procParam[2];///调过程所需参数
    StringVectorIterator m_recordIter;

    bool GetSysParam();///取入库系统参数
    bool ProcessFile();///入库文件处理
    bool GetFileName();///取待处理的文件名
    bool RecordOut();///文件出库处理
    bool ConnectDb();///建立数据库连接
    void WriteAlarm(const int ecode,const char *msg);///写告警信息
    void WriteStat(const char *msg,const char stat);///写状态信息
};
#endif
