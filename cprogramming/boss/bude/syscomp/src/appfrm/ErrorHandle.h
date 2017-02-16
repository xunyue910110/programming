/* Copyright (c) 2003,Linkage All rights reserved.*/

#ifndef __ERRORMESSAGES_H__
#define __ERRORMESSAGES_H__

#include "base/config-all.h"

#include <deque>
#include <string>


USING_NAMESPACE(std)


const int ERRORMSG_LEN = 255;

// 错误信息
class ERRORINFO {
  public:
    ERRORINFO() {;}
    ERRORINFO(const ERRORINFO& right);
    const ERRORINFO& operator=(const ERRORINFO& right);
    int no;
    char message[ERRORMSG_LEN+1];
};



///错误消息容器，可以将错误信息与错误编码加入容器内，框架统一的处理错误信息。
class ErrorMessages
{
  public:
    /// 设置应用相关信息。对一个具体的应用，初始设置一次，运行过程不发生改变。
    void setApplicationInfo(const string& applicationName, const int channelNo);
  
    /// 获取应用相关信息。
    void getApplicationInfo(string& applicationName, int channelNo);
  
    /// 加入错误信息。ERRORINFO包括：错误编码与错误信息。
    void insert(ERRORINFO &errInfo);
  
    /// 得到一个错误信息，并将错误信息从容器中删除。
    /// 成功返回true，无信息或失败时返回false。
    bool get(ERRORINFO& errInfo);
  
    /// 清空所有的错误信息。
    void clean();
  
    /// 容器是否为空。true为空，false非空。
    bool isEmpty();
  
    /// 加入错误信息。ERRORINFO包括：错误编码与错误信息。
    void insert(const int errorCode, const string& errorMessage);
  
    /// 加入错误信息。ERRORINFO包括：错误编码与错误信息。
    void insert(const int errorCode, const char *errorMessage);

  private:
    string m_applicationName; /// 应用程序名
    int m_channelNo;  /// 通道号
    deque<ERRORINFO> m_errInfoQueue; ///错误信息队列

};

#endif /* ERRORMESSAGES_H_INCLUDED*/


