#ifdef MWGLOBAL_H
//do nothing
#else
#define MWGLOBAL_H

#include "base/config-all.h"
//如HP是DISABLE,IBM是ENABLE
#ifdef STDIOSTREAM_DISABLE
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>
#define STD
#else
#include <iostream>
#include <fstream>
#include <strstream>
#include <iomanip>
#define STD std::
#endif

//选择名空间
USING_NAMESPACE(std)
#include<vector>
#include<list>

/*
#ifdef NAMESPACE_DISABLE
#else
USING_NAMESPACE(std)
#endif

#ifdef MSVCPP_CONFIG
#include<stdio.h>
#include<string>
#include<list>
#include<vector>
#else
#include <unistd.h>
#include <string>
#include <list>
#include <vector>
#endif
*/

// Tuxedo头文件包含
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#define HAD_POSIX_SOURCE
#endif
#define _TMFML32
#include <atmi.h>
#include "userlog.h"
#ifdef _TMFML32
#include <fml32.h>
#include <fml1632.h>
#else
#include <fml.h>
#endif
#ifdef HAD_POSIX_SOURCE
#undef HAD_POSIX_SOURCE
#undef _POSIX_SOURCE
#endif

//常量名字定义
//tuxedo 缓冲区内型
const int TUX_BUF_TYPE_INT = 0;
const int TUX_BUF_TYPE_LONG = 1;
const int TUX_BUF_TYPE_CHAR = 2;
const int TUX_BUF_TYPE_FLOAT = 3;
const int TUX_BUF_TYPE_DOUBLE = 4;
const int TUX_BUF_TYPE_STRING = 5; 	/** null terminated char * */
const int TUX_BUF_TYPE_CARRAY  =6;	/** character array  */
const int TUX_BUF_TYPE_PTR = 9;       /** pointer to a buffer */
const int TUX_BUF_TYPE_FML32 = 10;       /** embedded FML32 buffer */
const int TUX_BUF_TYPE_VIEW32 = 11;       /** embedded VIEW32 buffer */
const int TUX_BUF_TYPE_MBSTRING = 12;       /** multibyte character array */
const int TUX_BUF_TYPE_FML = 13; /** not use int tuxedo system call*/

//Caller
const int LLSVC_LOOKUP=0;
const int LLSVC_SAVE=1;

///打开次编译选项可以提高写日志的性能,但是在多进程并发的情况下,只有一个进程可以正常工作
// #define _TUXCALLER_STATIC_LOG;


//size
const long SIZE_1K = 1024L;
const long SIZE_4K = 4096L;
const long SIZE_32K = 32768L;
const long SIZE_64K = 65536L;
const long SIZE_256K = 262144L;
const long SIZE_1M = 1048576L;
const long SIZE_2M = 2097152L;
const long SIZE_4M = 4194304L;
const long SIZE_200M = 209715200L;
const long DEFAULT_EXTAND_SIZE = SIZE_4M;
const long DEFAULT_SIZE = SIZE_2M;

//自定义操作异常代码
const int TUX_FML_INVALID_LENGTH = 1000;
const int TUX_FML_CONV_FAIL = 1001;
const int TUX_FML_UNSUPPORT = 1002;
const int TUX_FML_INVALID_OPER = 1003;
const int TUX_CALL_FAIL = 1004;
const int TUX_UNSUPPORT = 1005;
const int TUX_WSNADDR_TOO_LONG = 1006;
const int TUX_INVALID_OPER = 1007;
const int TUX_UNKNOW_ERROR = 1008;
const int LNS_OPERFAIL=1009;
const int LLSVC_OPER_FAIL=1010;
const int LNS_CONNECT_FAIL=1011;
const int LIMW_TUX_ERRCODE_OFFSET = 1100;
const int LIMW_BUF_ERRCODE_OFFSET = 1200;
const int LIMW_FLD_NOT_EXIST = 1300;
const int TUX_FML_SIZEOVER = 1400;
//自定义错误类型(Local & Remote Naming Svc)

const int LNS_OK = 0;
const int LNS_NOT_READY = -1;
const int LNS_NO_DATA_FOUND = -2;
const int LNS_BAD_CONTEXT = -3;
const int LNS_BAD_NAME = -4;
const int LNS_NO_FILE_NAME = -5;
const int LNS_NO_INDEX_FOUND = -6;
const int LNS_OPEN_FAIL = -7;
const int LNS_WRITE_FAIL = -8;


const int LNS_CREATING =9;
const int LNS_CREATED =10;
const int LNS_READY=11;
const int LNS_BUSY =12;

const char LNS_ENV_STR[]="LocalNameFile\0";

const int LNS_ID_LENGTH = 16;
const int LNS_CODE_LENGTH = 8 ;
const int LNS_NAME_LENGTH = 132;
const int LNS_IP_LENGTH = 16 ;
const int LNS_PORT_LENGTH = 6;


//自定义操作类型
const int TUX_INBUF = 2000;
const int TUX_OUTBUF = 2001;


//自定义??
#define _LKENTER _TMDLLENTRY
#define LK_SENDONLY TPSENDONLY
#define LK_RECVONLY	TPRECVONLY
#endif
