#ifndef SYS_HEADER_INCLUDED
#define SYS_HEADER_INCLUDED

//#define __64BIT__
#define __32BIT__

// HP UNIX                __HPUX__              (hpux, __hpux__, __hpux)
// IBM AIX                __IBM_AIX__           (_AIX)
// Linux                  __LINUX               (__linux__)
// DEC UNIX               __DEC_UNIX__          (DGUX)
// Sun Solaris            __SUN_SOLARIS__       (SUNOS)
// Microsoft Windows 32   __WIN32__             (WIN32, __WIN32__)


#if defined(hpux) || defined(__hpux) || defined(__hpux)
#define __HPUX__
#endif

#if defined(_AIX)
#define __IBM_AIX__
#endif

#if defined(__linux__)
#define __LINUX__
#endif

#if defined(DGUX)
#define __DEC_UNIX__
#endif

#if defined(SUNOS)
#define __SUN_SOLARIS__
#endif

#if defined(__BORLANDC__)
#define __WIN_BCB__
#endif 

#if defined(WIN32) || defined (__WIN32__) || defined (__MS_NET__)
#define __OPERSYS_WIN__
#ifndef __OS_WIN32__
    #define __OS_WIN32__
#endif
#endif

//操作系统识别
#if defined(__HPUX__)||defined(__IBM_AIX__)||defined(__LINUX__)||defined(__DEC_UNIX__)||defined(__SUN_SOLARIS__)
#define __OPERSYS_UNIX__
#define __OS_UNIX__
#endif

//路径分隔符号
#if defined(__OPERSYS_WIN__)
#define PATH_DELIMITATED_CHAR '\\'
#else
#define PATH_DELIMITATED_CHAR '/'
#endif

//不同系统的编译器的特性
#if defined(__WIN_DEVCPP__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif

#if defined(__WIN_MSVC__)
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#define STD_NAMESPACE_ENABLE
#endif

#if defined(__WIN_BCB__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif

#if defined(__HPUX__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif

#if defined(__IBM_AIX__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif

#if defined(__SUN_SOLARIS__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif

#if defined(__LINUX__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif

#if defined(__MS_NET__)
#define STD_NAMESPACE_ENABLE
#define MEMBER_TEMPLATE_FUNCTION_ENABLE
#endif


#if defined(STD_NAMESPACE_ENABLE)               //如果有std名字空间的话，则声明UNS
#define UNS(x) using namespace x
#else
#define UNS(x)
//#define std
#endif

#ifdef __AP__  /* for using first version std lib */
#include <iostream.h>
//using namespace std;
#else
#include <iostream>
UNS(std);
#endif

#define LOG

#endif  /* SYS_HEADER_INCLUDED */



