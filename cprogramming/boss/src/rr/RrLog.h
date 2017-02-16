/**
*   排重模块日志类:包括合并日志和合并后的排重日志.
*   @author 高将飞 <gaojf@lianchuang.com>
*           
*   @version 1.0
*   @date 2008-08-12
*/

#ifndef _RRLOG_H_INCLUDED_20080812_
#define _RRLOG_H_INCLUDED_20080812_

#include "base/PString.h"

//合并后的日志
class RrLog
{
  public:
    void getLogStr();
    void clear();
  public:
    //PString m_fileName;
    size_t  m_totalCdr;
    size_t  m_errCdr;
    size_t  m_uniqCdr;
    size_t  m_repeatInFile;
    size_t  m_repeatHis;
};

//合并明细文件的日志
class RrFileLog
{
  public:
    PString m_fileName;
    size_t  m_totalCdr;
  public:
    RrFileLog();
    virtual ~RrFileLog();
    RrFileLog(const RrFileLog &r_right);
    RrFileLog& operator=(const RrFileLog &r_right);
};

//合并日志
class RrMergLog
{
  public:
    //PString m_fileName_merg;
    vector<RrFileLog> m_srcFileList;
  public:
    void  addFileLog(const char *r_fileName,size_t r_cdrNum); 
    void  clear();
};



#endif //_RRLOG_H_INCLUDED_20080812_
