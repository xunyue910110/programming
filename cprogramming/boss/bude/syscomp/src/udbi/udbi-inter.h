/** 
 * @file udbi-inter.h
 * UDBI组件内部公用内容定义
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __UDBI_INTER_H_
#define __UDBI_INTER_H_

#include "base/config-all.h"

//! 引用头文件
#define __UDBI_INTERNAL_USE_
#include "udbi.h"

#include "oci.h"


//! 错误处理判断常量定义
#define FIELD_NOT_EXISTS	-1
#define FIELD_TYPE_UNSUPPORTED	-2
#define FIELD_UNK_ERROR		-3
#define FIELD_OK		1
#define FIELD_NULL		2
#define IS_ERROR(exp)	((exp)!=FIELD_OK)


//! JSQL 日志屏蔽选项
const int JSQL_BUG_OPEN = 1;
const int JSQL_BUG_CLOSE = 0;
//int g_jsqlBug;


//! UDBI调试信息输出函数声明
namespace UDBI {
	void setJSqlBug(const int val);
	void DDEBUG_CONSOLE(UDBI_DLEV level, const char* file, int line, const char* msg, ...);
	void DDEBUG_FILE(UDBI_DLEV level,const char* file, int line, const char* fmt, ...);
	//void CHECKERR(OCIError *err, sword status);
	void version(void);
}

#define CHECKERR(XXX, YYY) \
{ \
	sword status = YYY; \
	if (status!=OCI_SUCCESS) { \
		sb4 m_errcode; \
		text *errText; \
		errText=(text *)calloc(512,sizeof(text)); \
		(void) OCIErrorGet((dvoid *)XXX, (ub4) 1, (text *) NULL, &m_errcode, errText, 512, OCI_HTYPE_ERROR); \
		errText[511]='\0'; \
		string tmpStr((char *)errText); \
		free(errText); \
		THROW_AC(EUdbiException, OracleException(m_errcode, tmpStr), 90009, "OCI 调用异常"); \
	} \
}

//! UDBI调试信息输出宏定义（简化代码编写）
#ifdef OFF_UDBI_DEBUG
#define UDBI_DEBUG(msg)
#define UDBI_DEBUG_P1(msg, p1)
#define UDBI_DEBUG_P2(msg, p1, p2)
#else
#define UDBI_DEBUG(msg)	DDEBUG_FILE(UDBI_DEBUG, __FILE__, __LINE__, msg);
#define UDBI_DEBUG_P1(msg, p1) DDEBUG_FILE(UDBI_DEBUG, __FILE__, __LINE__, msg, p1);
#define UDBI_DEBUG_P2(msg, p1, p2) DDEBUG_FILE(UDBI_DEBUG, __FILE__, __LINE__, msg, p1, p2);
#endif

#ifdef OFF_UDBI_TRACE
#define UDBI_TRACE(msg) 
#define UDBI_TRACE_P1(msg, p1)
#define UDBI_TRACE_P2(msg, p1, p2)
#define UDBI_INFO(msg)
#else
#define UDBI_TRACE(msg) DDEBUG_FILE(UDBI_TRACE, __FILE__, __LINE__, msg);
#define UDBI_TRACE_P1(msg, p1) DDEBUG_FILE(UDBI_TRACE, __FILE__, __LINE__, msg, p1);
#define UDBI_TRACE_P2(msg, p1, p2) DDEBUG_FILE(UDBI_TRACE, __FILE__, __LINE__, msg, p1, p2);
#define UDBI_INFO(msg)  DDEBUG_FILE(UDBI_INFO, __FILE__, __LINE__, msg);
#endif



#endif  /* __UDBI_INTER_H_ */

