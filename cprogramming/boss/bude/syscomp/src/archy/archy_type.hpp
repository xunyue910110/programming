/* Char, Uchar, Int[1,2,4], Uint[1,2,4]
 */

#if (SIZEOF_CHAR != 1)
#  error "Unsupported size of char(must be 1 byte)"
#endif
#if (SIZEOF_SHORT != 2)
#  error "Unsupported size of short int(must be 2 bytes)"
#endif
#if (SIZEOF_INT != 4)
#  error "Unsupported size of int(must be 4 bytes)"
#endif


typedef          char  ARCHY_Char;
typedef signed   char  ARCHY_Schar;
typedef unsigned char  ARCHY_Uchar;
typedef signed   char  ARCHY_Int1;
typedef unsigned char  ARCHY_Uint1;
typedef signed   short ARCHY_Int2;
typedef unsigned short ARCHY_Uint2;
typedef signed   int   ARCHY_Int4;
typedef unsigned int   ARCHY_Uint4;


/* Int8, Uint8
 */

#if   (SIZEOF_LONG == 8)
#  define ARCHY_INT8_TYPE long
#  define ARCHY_MIN_I8  LONG_MIN
#  define ARCHY_MAX_I8  LONG_MAX
#  define ARCHY_MAX_UI8 ULONG_MAX

#elif (SIZEOF_LONG_LONG == 8)
#  define ARCHY_INT8_TYPE long long
#  define ARCHY_MIN_I8  0x8000000000000000LL
#  define ARCHY_MAX_I8  0x7FFFFFFFFFFFFFFFLL
#  define ARCHY_MAX_UI8 0xFFFFFFFFFFFFFFFFULL
#elif (SIZEOF___INT64 == 8)
#  define USE_INT64 1
#  define ARCHY_INT8_TYPE __int64
#  define ARCHY_MIN_I8  0x8000000000000000i64
#  define ARCHY_MAX_I8  0x7FFFFFFFFFFFFFFFi64
#  define ARCHY_MAX_UI8 0xFFFFFFFFFFFFFFFFui64
#else
#  error "This platform does not support 8-byte integer"
#endif

typedef signed   ARCHY_INT8_TYPE ARCHY_Int8;
typedef unsigned ARCHY_INT8_TYPE ARCHY_Uint8;

