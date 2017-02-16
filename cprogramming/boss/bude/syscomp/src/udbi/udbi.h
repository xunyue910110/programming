/** 
 * @file udbi.h
 * UDBI 组件接口头文件，使用UDBI组件的Client仅包含本头文件即可
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __UDBI_H_
#define __UDBI_H_

#define UDBILIBVER	0x000506
#define UDBILIBMAJOR	0X00
#define UDBILIBMINOR	0X05
#define UDBILIBMICRO	0x06

#define OCILIBVER		8.1
#define OCILIBVERMAJOR	8
#define OCILIBVERMINOR	1


//! 定义UDBI组件抛出的异常类
#include "base/exceptions.h"
DECLARE_EXCEPTION(EUdbiException, Exception);


#define OFF_UDBI_DEBUG
#define OFF_UDBI_TRACE


//! 定义UDBI组件的调试信息输出控制选项 
enum UDBI_DLEV {
	UDBI_FATAL=0, 
	UDBI_ERROR=1,
	UDBI_DEBUG=2,
	UDBI_TRACE=3,
	UDBI_INFO =4
};

const int UDBI_DEBUG_LEVEL=3;


//! 查询SQL执行后每次从数据库中获取记录数常量
const int UDBI_PREFETCH_RECS = 15;


//! 指定接口需包含的头文件 
#ifndef __UDBI_INTERNAL_USE_
#include "jdb.h"
#include "JConnect.h"
#include "JTable.h"
#include "MTable.h"
#include "JSQL.h"
#endif /* __UDBI_INTERNAL_USE_ */



#endif /*  __UDBI_H_ */
