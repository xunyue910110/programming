/* Copyright (c) 2003,Linkage  All rights reserved.*/
#ifndef __FPUSH_H__
#define __FPUSH_H__


#include "base/config-all.h"

#include <fstream>
#include <stdexcept>

USING_NAMESPACE(std)


///抛出异常
class FPushException : public runtime_error {
  public:
    FPushException(const string& msg) : runtime_error(msg) { ; }
};


///
/// 及时写出文件。
///
/// 用于框架应用将日志、告警等信息实时的写出到指定的文件中。
class FPush
{
public:
    FPush(size_t maxLength = 1024);
    virtual ~FPush();

    /// 记录文件名
    void open(const string& fileName);
    /// 将信息写入文件
    void push(const char* data);
    /// 关闭
    void close();
private:
    ///将换行转换为"\n"
    void escape(const char *msg, char *buffer, size_t size);
    string m_fileName;
    size_t m_maxLength;
    char *m_lineBuffer;
    ofstream m_os;
};

#endif
