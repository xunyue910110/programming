#ifndef __TYPES_H
#define __TYPES_H

#if defined(_WIN64)
		typedef long               Int64;
#elif defined(_WIN32)
		typedef __int64            Int64;
#elif defined(__GNUC__)
	#if defined(__LP64__)
		typedef long               Int64;
	#else
		typedef long long          Int64;
	#endif
#elif defined(__HP_aCC)
	#if defined(__LP64__)
		typedef __int64            Int64;
	#else
		typedef __int64            Int64;
	#endif
#elif defined(__SUNPRO_CC)
	#if defined(__sparcv9)
		typedef long               Int64;
	#else
		typedef long long          Int64;
	#endif
#elif defined(__IBMCPP__)
	#if defined(__64BIT__)
		typedef long               Int64;
	#else
		typedef long long          Int64;
	#endif

#endif

typedef unsigned int           UInt32;

#endif //__TYPES_H
