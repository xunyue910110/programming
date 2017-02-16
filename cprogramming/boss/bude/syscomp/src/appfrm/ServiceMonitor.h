/* Copyright (c) 2003,Linkage All rights reserved.*/

#ifndef __SERVICEMONITOR_H__
#define __SERVICEMONITOR_H__

#include "base/config-all.h"

#include <vector>
#include <string>
#include <strstream>


USING_NAMESPACE(std)


enum MONITOR_TRACE_ELEVEL 
{
	MONITOR_TRACE_DEFAULT =0,
  MONITOR_TRACE_ALERT=0, 
  MONITOR_TRACE_INFO, 
  MONITOR_TRACE_DEBUG 
};



///进程状态类
class NameValue {
  public:
    string m_name;
    string m_value;
};

typedef vector < NameValue > NameValueList;


///进程服务类
class InternalServiceMonitor;


///为封装内容（FPush、UdpSender）外部不可见定义的接口类
class ServiceMonitor
{
public:
    ///处理进程运行参数、lock路径
    ServiceMonitor(int argc, char *argv[], const string& workdir);
    ServiceMonitor();
   ~ServiceMonitor();

    ///建进程运行所需系统目录，建文件锁
    bool initialize();
    ///清环境，删文件锁
    bool end();

    /// debug
    bool debug(int code , const char * message);

    /// info
    bool info(int code , const char * messgage);

    ///写告警信息
    bool alert(int code , const char * message);

    ///写日志信息
    bool log(int type , const char * data);

    ///写进程状态
    bool setStatus(const NameValueList & state);

    void setDump(ostream *o);

    void setAlertFile(const char * file);
    void setLogFile(const char * file);    

    void traceLevel(int level);
    bool isDebugEnabled();
    bool isInfoEnabled();    

private:
    InternalServiceMonitor * m_monitor;
};


/** 
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define MONITOR_DEBUG(x, message) { \
	if (x->isDebugEnabled()) { \
	ostrstream oss; \
	oss << "" << __FILE__ << ":" << __LINE__ << ": "; \
	oss << message; \
	x->debug(MONITOR_TRACE_DEBUG,oss.str()); }}

/** 
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define MONITOR_INFO(x, message) { \
	if (x->isInfoEnabled()) {\
	ostrstream oss; oss << message; \
	x->info(MONITOR_TRACE_INFO,oss.str()); }}

/** 
Logs a message to a specified logger with the alert level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define MONITOR_ALERT(x, message) { \
	{\
	ostrstream oss; oss << message; \
	x->alert(MONITOR_TRACE_ALERT,oss.str()); }}
	

#endif //__SERVICE_MONITOR_H__


