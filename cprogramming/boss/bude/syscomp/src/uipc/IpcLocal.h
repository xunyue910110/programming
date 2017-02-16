#ifndef __IPC_LOCAL_H_
#define __IPC_LOCAL_H_

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "budeipc.h"
		  
namespace budeipc {
	void DDEBUG_CONSOLE(IPC_DLEV level, const char* file, int line, const char* msg, ...);
	void DDEBUG_FILE(IPC_DLEV level,const char* file, int line, const char* fmt, ...);

} // namespace budeipc

//! IPC调试信息输出宏定义（简化代码编写）
#ifdef OFF_IPC_DEBUG
#define IPC_DEBUG(msg)
#define IPC_DEBUG_P1(msg, p1)
#define IPC_DEBUG_P2(msg, p1, p2)
#else
#define IPC_DEBUG(msg)	DDEBUG_FILE(IPC_DEBUG, __FILE__, __LINE__, msg);
#define IPC_DEBUG_P1(msg, p1) DDEBUG_FILE(IPC_DEBUG, __FILE__, __LINE__, msg, p1);
#define IPC_DEBUG_P2(msg, p1, p2) DDEBUG_FILE(IPC_DEBUG, __FILE__, __LINE__, msg, p1, p2);
#endif

#ifdef OFF_IPC_TRACE
#define IPC_TRACE(msg) 
#define IPC_TRACE_P1(msg, p1)
#define IPC_TRACE_P2(msg, p1, p2)
#define IPC_INFO(msg)
#else
#define IPC_TRACE(msg) DDEBUG_FILE(IPC_TRACE, __FILE__, __LINE__, msg);
#define IPC_TRACE_P1(msg, p1) DDEBUG_FILE(IPC_TRACE, __FILE__, __LINE__, msg, p1);
#define IPC_TRACE_P2(msg, p1, p2) DDEBUG_FILE(IPC_TRACE, __FILE__, __LINE__, msg, p1, p2);
#define IPC_INFO(msg)  DDEBUG_FILE(IPC_INFO, __FILE__, __LINE__, msg);
#endif

#endif

