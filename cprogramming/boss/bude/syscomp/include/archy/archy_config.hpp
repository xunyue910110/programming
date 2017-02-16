#if !defined(CONFIG_Q_GENEL__)
#define CONFIG_Q_GENEL__

#define BEGIN_NAMESPACE_ARCHY namespace archy {
#define END_NAMESPACE_ARCHY }

#if defined(WIN32) || defined(OS_WIN) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)

	#define ARCHY_HAS_WINTHREADS
	#include <windows.h>
#else
	// define sysV semaphores
	#define ARCHY_SYSV_SEMAPHORES
	#define ARCHY_HAS_NANOSLEEP
	#define ARCHY_HAS_PTHREADS
	#define ARCHY_BYTE char;
	#define ARCHY_UBYTE unsigned char;
#endif

#define SIZEOF_CHAR 1
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4

/*
#define SIZEOF_LONG
#define SIZEOF_LONG_LONG
#define SIZEOF___INT64 
*/
#if defined(__sun__) || defined(OS_SUN)
	#ifndef OS_SUN
	#define OS_SUN
	#endif
	

// 定义字节序
	#define ARCHY_BIG_ENDIAN

	#define SIZEOF_LONG_LONG 8
	
	#define ARCHY_WORD	short
	#define ARCHY_UWORD	unsigned short
	#define ARCHY_DWORD	int
	#define ARCHY_UDWORD	unsigned int
	
#elif defined(__hpux) || defined(OS_HP)
	#ifndef OS_HP
	#define OS_HP
	#endif

	#define SIZEOF_LONG_LONG 8

	#define ARCHY_BIG_ENDIAN

	#define ARCHY_WORD	short
	#define ARCHY_UWORD	unsigned short
	#define ARCHY_DWORD	int
	#define ARCHY_UDWORD	unsigned int
	
		
#elif defined(_AIX)	|| defined(OS_AIX)

	#ifndef OS_AIX
	#define OS_AIX
	#endif

	#define SIZEOF_LONG_LONG 8
	#define ARCHY_BIG_ENDIAN
	#define ARCHY_WORD	short
	#define ARCHY_UWORD	unsigned short
	#define ARCHY_DWORD	int
	#define ARCHY_UDWORD	unsigned int

#elif defined(__digital__) || defined(OS_DEC)

	#ifndef OS_DEC
	#define OS_DEC
	#endif

	#define SIZEOF_LONG 8

	  #define ARCHY_LITTLE_ENDIAN

	#define ARCHY_WORD	short
	#define ARCHY_UWORD	unsigned short
	#define ARCHY_DWORD	int
	#define ARCHY_UDWORD	unsigned int

#elif defined(__linux__) || defined(OS_LINUX)
	#ifndef OS_LINUX
	#define OS_LINUX
	#endif

	  #define ARCHY_LITTLE_ENDIAN
	#define SIZEOF_LONG_LONG 8
	#define ARCHY_WORD	short
	#define ARCHY_UWORD	unsigned short
	#define ARCHY_DWORD	int
	#define ARCHY_UDWORD	unsigned int


#elif defined(WIN32) || defined(OS_WIN) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
	#ifndef OS_WIN
	#define OS_WIN
	#endif
	#define SIZEOF___INT64 8

	  #define ARCHY_LITTLE_ENDIAN

	#define ARCHY_WORD	WORD
	#define ARCHY_UWORD	UWORD
	#define ARCHY_DWORD	DWORD
	#define ARCHY_UDWORD	UDWORD

//	#ifndef DWORD
//	#define DWORD	int
//	#endif
	
//	#define UWORD	unsigned short
//	#define DWORD	int
//	#define UDWORD	unsigned int
//     	typedef long long    int64_t;
//     	typedef unsigned long long    uint64_t;

#endif

#include <stdio.h>

#if !defined(__hpux) && !defined(__digital__)
#define USING_STD_NAMESPACE using namespace std
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <list>
#include <utility>
#else
#include <exception>
#include <string>
#include <iterator>
#include <iostream.h>
#include <utility>
#include <vector>
#include <stddef.h>
namespace std
{
	using ::ptrdiff_t;
	using ::vector;
	using ::basic_string;
	using ::pair;
	using ::cout;
	using ::cerr;
	using ::endl;
	using ::exception;
	using ::string;
	using ::runtime_error;
	using ::input_iterator_tag;
}
#define USING_STD_NAMESPACE
#endif

#if defined(WIN32) || defined(OS_WIN)
	#define ARCHY_HAS_FTIME
#else
	#define ARCHY_HAS_GETTIMEOFDAY
#endif
#endif // CONFIG_Q_GENEL__

