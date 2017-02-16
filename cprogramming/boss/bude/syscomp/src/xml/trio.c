/*************************************************************************
 *
 * $Id: trio.c,v 1.1 2011/04/15 07:56:38 sunam Exp $
 *
 * Copyright (C) 1998 Bjorn Reese and Daniel Stenberg.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
 *************************************************************************
 *
 * A note to trio contributors:
 *
 * Avoid heap allocation at all costs to ensure that the trio functions
 * are async-safe. The exceptions are the printf/fprintf functions, which
 * uses fputc, and the asprintf functions and the <alloc> modifier, which
 * by design are required to allocate form the heap.
 *
 ************************************************************************/

/*
 * TODO:
 *  - Scan is probably too permissive about its modifiers.
 *  - C escapes in %#[] ?
 *  - Multibyte characters (done for format parsing, except scan groups)
 *  - Complex numbers? (C99 _Complex)
 *  - Boolean values? (C99 _Bool)
 *  - C99 NaN(n-char-sequence) missing. The n-char-sequence can be used
 *    to print the mantissa, e.g. NaN(0xc000000000000000)
 *  - Should we support the GNU %a alloc modifier? GNU has an ugly hack
 *    for %a, because C99 used %a for other purposes. If specified as
 *    %as or %a[ it is interpreted as the alloc modifier, otherwise as
 *    the C99 hex-float. This means that you cannot scan %as as a hex-float
 *    immediately followed by an 's'.
 *  - Scanning of collating symbols.
 */

/*************************************************************************
 * Trio include files
 */
#include "triodef.h"
#include "trio.h"
#include "triop.h"
#include "trionan.h"
#if !defined(TRIO_MINIMAL)
# include "triostr.h"
#endif

/**************************************************************************
 *
 * Definitions
 *
 *************************************************************************/

#include <math.h>
#include <limits.h>
#include <float.h>

#if defined(__STDC_ISO_10646__) || defined(MB_LEN_MAX) || defined(USE_MULTIBYTE) || TRIO_WIDECHAR
# define TRIO_COMPILER_SUPPORTS_MULTIBYTE
# if !defined(MB_LEN_MAX)
#  define MB_LEN_MAX 6
# endif
#endif

#if (defined(TRIO_COMPILER_MSVC) && (_MSC_VER >= 1100)) || defined(TRIO_COMPILER_BCB)
# define TRIO_COMPILER_SUPPORTS_MSVC_INT
#endif

/*************************************************************************
 * Generic definitions
 */

#if !(defined(DEBUG) || defined(NDEBUG))
# define NDEBUG
#endif

#include <assert.h>
#include <ctype.h>
#if !defined(TRIO_COMPILER_SUPPORTS_C99)
# define isblank(x) (((x)==32) || ((x)==9))
#endif
#if defined(TRIO_COMPILER_ANCIENT)
# include <varargs.h>
#else
# include <stdarg.h>
#endif
#include <stddef.h>
#include <errno.h>

#ifndef NULL
# define NULL 0
#endif
#define NIL ((char)0)
#ifndef FALSE
# define FALSE (1 == 0)
# define TRUE (! FALSE)
#endif
#define BOOLEAN_T int

/* mincore() can be used for debugging purposes */
#define VALID(x) (NULL != (x))

#if TRIO_ERRORS
  /*
   * Encode the error code and the position. This is decoded
   * with TRIO_ERROR_CODE and TRIO_ERROR_POSITION.
   */
# define TRIO_ERROR_RETURN(x,y) (- ((x) + ((y) << 8)))
#else
# define TRIO_ERROR_RETURN(x,y) (-1)
#endif

typedef unsigned long trio_flags_t;


/*************************************************************************
 * Platform specific definitions
 */
#if defined(TRIO_PLATFORM_UNIX)
# include <unistd.h>
# include <signal.h>
# include <locale.h>
# define USE_LOCALE
#endif /* TRIO_PLATFORM_UNIX */
#if defined(TRIO_PLATFORM_VMS)
# include <unistd.h>
#endif
#if defined(TRIO_PLATFORM_WIN32)
# include <io.h>
# define read _read
# define write _write
#endif /* TRIO_PLATFORM_WIN32 */

#if TRIO_WIDECHAR
# if defined(TRIO_COMPILER_SUPPORTS_ISO94)
#  include <wchar.h>
#  include <wctype.h>
typedef wchar_t trio_wchar_t;
typedef wint_t trio_wint_t;
# else
typedef char trio_wchar_t;
typedef int trio_wint_t;
#  define WCONST(x) L ## x
#  define WEOF EOF
#  define iswalnum(x) isalnum(x)
#  define iswalpha(x) isalpha(x)
#  define iswblank(x) isblank(x)
#  define iswcntrl(x) iscntrl(x)
#  define iswdigit(x) isdigit(x)
#  define iswgraph(x) isgraph(x)
#  define iswlower(x) islower(x)
#  define iswprint(x) isprint(x)
#  define iswpunct(x) ispunct(x)
#  define iswspace(x) isspace(x)
#  define iswupper(x) isupper(x)
#  define iswxdigit(x) isxdigit(x)
# endif
#endif


/*************************************************************************
 * Compiler dependent definitions
 */

/* Support for long long */
#ifndef __cplusplus
# if !defined(USE_LONGLONG)
#  if defined(TRIO_COMPILER_GCC) && !defined(__STRICT_ANSI__)
#   define USE_LONGLONG
#  elif defined(TRIO_COMPILER_SUNPRO)
#   define USE_LONGLONG
#  elif defined(_LONG_LONG) || defined(_LONGLONG)
#   define USE_LONGLONG
#  endif
# endif
#endif

/* The extra long numbers */
#if defined(USE_LONGLONG)
typedef signed long long int trio_longlong_t;
typedef unsigned long long int trio_ulonglong_t;
#elif defined(TRIO_COMPILER_SUPPORTS_MSVC_INT)
typedef signed __int64 trio_longlong_t;
typedef unsigned __int64 trio_ulonglong_t;
#else
typedef TRIO_SIGNED long int trio_longlong_t;
typedef unsigned long int trio_ulonglong_t;
#endif

/* Maximal and fixed integer types */
#if defined(TRIO_COMPILER_SUPPORTS_C99)
# include <stdint.h>
typedef intmax_t trio_intmax_t;
typedef uintmax_t trio_uintmax_t;
typedef int8_t trio_int8_t;
typedef int16_t trio_int16_t;
typedef int32_t trio_int32_t;
typedef int64_t trio_int64_t;
#elif defined(TRIO_COMPILER_SUPPORTS_UNIX98)
# include <inttypes.h>
typedef intmax_t trio_intmax_t;
typedef uintmax_t trio_uintmax_t;
typedef int8_t trio_int8_t;
typedef int16_t trio_int16_t;
typedef int32_t trio_int32_t;
typedef int64_t trio_int64_t;
#elif defined(TRIO_COMPILER_SUPPORTS_MSVC_INT)
typedef trio_longlong_t trio_intmax_t;
typedef trio_ulonglong_t trio_uintmax_t;
typedef __int8 trio_int8_t;
typedef __int16 trio_int16_t;
typedef __int32 trio_int32_t;
typedef __int64 trio_int64_t;
#else
typedef trio_longlong_t trio_intmax_t;
typedef trio_ulonglong_t trio_uintmax_t;
# if defined(TRIO_INT8_T)
typedef TRIO_INT8_T trio_int8_t;
# else
typedef TRIO_SIGNED char trio_int8_t;
# endif
# if defined(TRIO_INT16_T)
typedef TRIO_INT16_T trio_int16_t;
# else
typedef TRIO_SIGNED short trio_int16_t;
# endif
# if defined(TRIO_INT32_T)
typedef TRIO_INT32_T trio_int32_t;
# else
typedef TRIO_SIGNED int trio_int32_t;
# endif
# if defined(TRIO_INT64_T)
typedef TRIO_INT64_T trio_int64_t;
# else
typedef trio_longlong_t trio_int64_t;
# endif
#endif

#if !(defined(TRIO_COMPILER_SUPPORTS_C99) \
 || defined(TRIO_COMPILER_SUPPORTS_UNIX01))
# define floorl(x) floor((double)(x))
# define fmodl(x,y) fmod((double)(x),(double)(y))
# define powl(x,y) pow((double)(x),(double)(y))
#endif

#define TRIO_FABS(x) (((x) < 0.0) ? -(x) : (x))

/*************************************************************************
 * Internal Definitions
 */

#ifndef DECIMAL_DIG
# define DECIMAL_DIG DBL_DIG
#endif

/* Long double sizes */
#ifdef LDBL_DIG
# define MAX_MANTISSA_DIGITS LDBL_DIG
# define MAX_EXPONENT_DIGITS 4
# define MAX_DOUBLE_DIGITS LDBL_MAX_10_EXP
#else
# define MAX_MANTISSA_DIGITS DECIMAL_DIG
# define MAX_EXPONENT_DIGITS 3
# define MAX_DOUBLE_DIGITS DBL_MAX_10_EXP
#endif

#if defined(TRIO_COMPILER_ANCIENT) || !defined(LDBL_DIG)
# undef LDBL_DIG
# undef LDBL_MANT_DIG
# undef LDBL_EPSILON
# define LDBL_DIG DBL_DIG
# define LDBL_MANT_DIG DBL_MANT_DIG
# define LDBL_EPSILON DBL_EPSILON
#endif

/* The maximal number of digits is for base 2 */
#define MAX_CHARS_IN(x) (sizeof(x) * CHAR_BIT)
/* The width of a pointer. The number of bits in a hex digit is 4 */
#define POINTER_WIDTH ((sizeof("0x") - 1) + sizeof(trio_pointer_t) * CHAR_BIT / 4)

/* Infinite and Not-A-Number for floating-point */
#define INFINITE_LOWER "inf"
#define INFINITE_UPPER "INF"
#define LONG_INFINITE_LOWER "infinite"
#define LONG_INFINITE_UPPER "INFINITE"
#define NAN_LOWER "nan"
#define NAN_UPPER "NAN"

/* Various constants */
enum {
  TYPE_PRINT = 1,
  TYPE_SCAN  = 2,

  /* Flags. FLAGS_LAST must be less than ULONG_MAX */
  FLAGS_NEW                 = 0,
  FLAGS_STICKY              = 1,
  FLAGS_SPACE               = 2 * FLAGS_STICKY,
  FLAGS_SHOWSIGN            = 2 * FLAGS_SPACE,
  FLAGS_LEFTADJUST          = 2 * FLAGS_SHOWSIGN,
  FLAGS_ALTERNATIVE         = 2 * FLAGS_LEFTADJUST,
  FLAGS_SHORT               = 2 * FLAGS_ALTERNATIVE,
  FLAGS_SHORTSHORT          = 2 * FLAGS_SHORT,
  FLAGS_LONG                = 2 * FLAGS_SHORTSHORT,
  FLAGS_QUAD                = 2 * FLAGS_LONG,
  FLAGS_LONGDOUBLE          = 2 * FLAGS_QUAD,
  FLAGS_SIZE_T              = 2 * FLAGS_LONGDOUBLE,
  FLAGS_PTRDIFF_T           = 2 * FLAGS_SIZE_T,
  FLAGS_INTMAX_T            = 2 * FLAGS_PTRDIFF_T,
  FLAGS_NILPADDING          = 2 * FLAGS_INTMAX_T,
  FLAGS_UNSIGNED            = 2 * FLAGS_NILPADDING,
  FLAGS_UPPER               = 2 * FLAGS_UNSIGNED,
  FLAGS_WIDTH               = 2 * FLAGS_UPPER,
  FLAGS_WIDTH_PARAMETER     = 2 * FLAGS_WIDTH,
  FLAGS_PRECISION           = 2 * FLAGS_WIDTH_PARAMETER,
  FLAGS_PRECISION_PARAMETER = 2 * FLAGS_PRECISION,
  FLAGS_BASE                = 2 * FLAGS_PRECISION_PARAMETER,
  FLAGS_BASE_PARAMETER      = 2 * FLAGS_BASE,
  FLAGS_FLOAT_E             = 2 * FLAGS_BASE_PARAMETER,
  FLAGS_FLOAT_G             = 2 * FLAGS_FLOAT_E,
  FLAGS_QUOTE               = 2 * FLAGS_FLOAT_G,
  FLAGS_WIDECHAR            = 2 * FLAGS_QUOTE,
  FLAGS_ALLOC               = 2 * FLAGS_WIDECHAR,
  FLAGS_IGNORE              = 2 * FLAGS_ALLOC,
  FLAGS_IGNORE_PARAMETER    = 2 * FLAGS_IGNORE,
  FLAGS_VARSIZE_PARAMETER   = 2 * FLAGS_IGNORE_PARAMETER,
  FLAGS_FIXED_SIZE          = 2 * FLAGS_VARSIZE_PARAMETER,
  FLAGS_LAST                = FLAGS_FIXED_SIZE,
  /* Reused flags */
  FLAGS_EXCLUDE             = FLAGS_SHORT,
  FLAGS_USER_DEFINED        = FLAGS_IGNORE,
  FLAGS_ROUNDING            = FLAGS_INTMAX_T,
  /* Compounded flags */
  FLAGS_ALL_VARSIZES        = FLAGS_LONG | FLAGS_QUAD | FLAGS_INTMAX_T | FLAGS_PTRDIFF_T | FLAGS_SIZE_T,
  FLAGS_ALL_SIZES           = FLAGS_ALL_VARSIZES | FLAGS_SHORTSHORT | FLAGS_SHORT,

  NO_POSITION  = -1,
  NO_WIDTH     =  0,
  NO_PRECISION = -1,
  NO_SIZE      = -1,

  /* Do not change these */
  NO_BASE      = -1,
  MIN_BASE     =  2,
  MAX_BASE     = 36,
  BASE_BINARY  =  2,
  BASE_OCTAL   =  8,
  BASE_DECIMAL = 10,
  BASE_HEX     = 16,

  /* Maximal number of allowed parameters */
  MAX_PARAMETERS = 64,
  /* Maximal number of characters in class */
  MAX_CHARACTER_CLASS = UCHAR_MAX + 1,

  /* Maximal string lengths for user-defined specifiers */
  MAX_USER_NAME = 64,
  MAX_USER_DATA = 256,
  
  /* Maximal length of locale separator strings */
  MAX_LOCALE_SEPARATOR_LENGTH = MB_LEN_MAX,
  /* Maximal number of integers in grouping */
  MAX_LOCALE_GROUPS = 64,

  /* Initial size of asprintf buffer */
  DYNAMIC_START_SIZE = 32
};

#define NO_GROUPING ((int)CHAR_MAX)

/* Fundamental formatting parameter types */
#define FORMAT_UNKNOWN   0
#define FORMAT_INT       1
#define FORMAT_DOUBLE    2
#define FORMAT_CHAR      3
#define FORMAT_STRING    4
#define FORMAT_POINTER   5
#define FORMAT_COUNT     6
#define FORMAT_PARAMETER 7
#define FORMAT_GROUP     8
#if TRIO_GNU
# define FORMAT_ERRNO    9
#endif
#if TRIO_EXTENSION
# define FORMAT_USER_DEFINED 10
#endif

/* Character constants */
#define CHAR_IDENTIFIER '%'
#define CHAR_BACKSLASH '\\'
#define CHAR_QUOTE '\"'
#define CHAR_ADJUST ' '

/* Character class expressions */
#define CLASS_ALNUM "[:alnum:]"
#define CLASS_ALPHA "[:alpha:]"
#define CLASS_BLANK "[:blank:]"
#define CLASS_CNTRL "[:cntrl:]"
#define CLASS_DIGIT "[:digit:]"
#define CLASS_GRAPH "[:graph:]"
#define CLASS_LOWER "[:lower:]"
#define CLASS_PRINT "[:print:]"
#define CLASS_PUNCT "[:punct:]"
#define CLASS_SPACE "[:space:]"
#define CLASS_UPPER "[:upper:]"
#define CLASS_XDIGIT "[:xdigit:]"

/*
 * SPECIFIERS:
 *
 *
 * a  Hex-float
 * A  Hex-float
 * c  Character
 * C  Widechar character (wint_t)
 * d  Decimal
 * e  Float
 * E  Float
 * F  Float
 * F  Float
 * g  Float
 * G  Float
 * i  Integer
 * m  Error message
 * n  Count
 * o  Octal
 * p  Pointer
 * s  String
 * S  Widechar string (wchar_t *)
 * u  Unsigned
 * x  Hex
 * X  Hex
 * [] Group
 * <> User-defined
 *
 * Reserved:
 *
 * D  Binary Coded Decimal %D(length,precision) (OS/390)
 */
#define SPECIFIER_CHAR 'c'
#define SPECIFIER_STRING 's'
#define SPECIFIER_DECIMAL 'd'
#define SPECIFIER_INTEGER 'i'
#define SPECIFIER_UNSIGNED 'u'
#define SPECIFIER_OCTAL 'o'
#define SPECIFIER_HEX 'x'
#define SPECIFIER_HEX_UPPER 'X'
#define SPECIFIER_FLOAT_E 'e'
#define SPECIFIER_FLOAT_E_UPPER 'E'
#define SPECIFIER_FLOAT_F 'f'
#define SPECIFIER_FLOAT_F_UPPER 'F'
#define SPECIFIER_FLOAT_G 'g'
#define SPECIFIER_FLOAT_G_UPPER 'G'
#define SPECIFIER_POINTER 'p'
#define SPECIFIER_GROUP '['
#define SPECIFIER_UNGROUP ']'
#define SPECIFIER_COUNT 'n'
#if TRIO_UNIX98
# define SPECIFIER_CHAR_UPPER 'C'
# define SPECIFIER_STRING_UPPER 'S'
#endif
#if TRIO_C99
# define SPECIFIER_HEXFLOAT 'a'
# define SPECIFIER_HEXFLOAT_UPPER 'A'
#endif
#if TRIO_GNU
# define SPECIFIER_ERRNO 'm'
#endif
#if TRIO_EXTENSION
# define SPECIFIER_BINARY 'b'
# define SPECIFIER_BINARY_UPPER 'B'
# define SPECIFIER_USER_DEFINED_BEGIN '<'
# define SPECIFIER_USER_DEFINED_END '>'
# define SPECIFIER_USER_DEFINED_SEPARATOR ':'
#endif

/*
 * QUALIFIERS:
 *
 *
 * Numbers = d,i,o,u,x,X
 * Float = a,A,e,E,f,F,g,G
 * String = s
 * Char = c
 *
 *
 * 9$ Position
 *      Use the 9th parameter. 9 can be any number between 1 and
 *      the maximal argument
 *
 * 9 Width
 *      Set width to 9. 9 can be any number, but must not be postfixed
 *      by '$'
 *
 * h  Short
 *    Numbers:
 *      (unsigned) short int
 *
 * hh Short short
 *    Numbers:
 *      (unsigned) char
 *
 * l  Long
 *    Numbers:
 *      (unsigned) long int
 *    String:
 *      as the S specifier
 *    Char:
 *      as the C specifier
 *
 * ll Long Long
 *    Numbers:
 *      (unsigned) long long int
 *
 * L  Long Double
 *    Float
 *      long double
 *
 * #  Alternative
 *    Float:
 *      Decimal-point is always present
 *    String:
 *      non-printable characters are handled as \number
 *
 *    Spacing
 *
 * +  Sign
 *
 * -  Alignment
 *
 * .  Precision
 *
 * *  Parameter
 *    print: use parameter
 *    scan: no parameter (ignore)
 *
 * q  Quad
 *
 * Z  size_t
 *
 * w  Widechar
 *
 * '  Thousands/quote
 *    Numbers:
 *      Integer part grouped in thousands
 *    Binary numbers:
 *      Number grouped in nibbles (4 bits)
 *    String:
 *      Quoted string
 *
 * j  intmax_t
 * t  prtdiff_t
 * z  size_t
 *
 * !  Sticky
 * @  Parameter (for both print and scan)
 *
 * I  n-bit Integer
 *    Numbers:
 *      The following options exists
 *        I8  = 8-bit integer
 *        I16 = 16-bit integer
 *        I32 = 32-bit integer
 *        I64 = 64-bit integer
 */
#define QUALIFIER_POSITION '$'
#define QUALIFIER_SHORT 'h'
#define QUALIFIER_LONG 'l'
#define QUALIFIER_LONG_UPPER 'L'
#define QUALIFIER_ALTERNATIVE '#'
#define QUALIFIER_SPACE ' '
#define QUALIFIER_PLUS '+'
#define QUALIFIER_MINUS '-'
#define QUALIFIER_DOT '.'
#define QUALIFIER_STAR '*'
#define QUALIFIER_CIRCUMFLEX '^' /* For scanlists */
#if TRIO_C99
# define QUALIFIER_SIZE_T 'z'
# define QUALIFIER_PTRDIFF_T 't'
# define QUALIFIER_INTMAX_T 'j'
#endif
#if TRIO_BSD || TRIO_GNU
# define QUALIFIER_QUAD 'q'
#endif
#if TRIO_GNU
# define QUALIFIER_SIZE_T_UPPER 'Z'
#endif
#if TRIO_MISC
# define QUALIFIER_WIDECHAR 'w'
#endif
#if TRIO_MICROSOFT
# define QUALIFIER_FIXED_SIZE 'I'
#endif
#if TRIO_EXTENSION
# define QUALIFIER_QUOTE '\''
# define QUALIFIER_STICKY '!'
# define QUALIFIER_VARSIZE '&' /* This should remain undocumented */
# define QUALIFIER_PARAM '@' /* Experimental */
# define QUALIFIER_COLON ':' /* For scanlists */
# define QUALIFIER_EQUAL '=' /* For scanlists */
# define QUALIFIER_ROUNDING_UPPER 'R'
#endif


/*************************************************************************
 *
 * Internal Structures
 *
 *************************************************************************/

/* Parameters */
typedef struct {
  /* An indication of which entry in the data union is used */
  int type;
  /* The flags */
  trio_flags_t flags;
  /* The width qualifier */
  int width;
  /* The precision qualifier */
  int precision;
  /* The base qualifier */
  int base;
  /* The size for the variable size qualifier */
  int varsize;
  /* The marker of the end of the specifier */
  int indexAfterSpecifier;
  /* The data from the argument list */
  union {
    char *string;
#if TRIO_WIDECHAR
    trio_wchar_t *wstring;
#endif
    trio_pointer_t pointer;
    union {
      trio_intmax_t as_signed;
      trio_uintmax_t as_unsigned;
    } number;
    double doubleNumber;
    double *doublePointer;
    trio_long_double_t longdoubleNumber;
    trio_long_double_t *longdoublePointer;
    int errorNumber;
  } data;
  /* For the user-defined specifier */
  char user_name[MAX_USER_NAME];
  char user_data[MAX_USER_DATA];
} trio_parameter_t;

/* Container for customized functions */
typedef struct {
  union {
    trio_outstream_t out;
    trio_instream_t in;
  } stream;
  trio_pointer_t closure;
} trio_custom_t;

/* General trio "class" */
typedef struct _trio_class_t {
  /*
   * The function to write characters to a stream.
   */
  void (*OutStream) TRIO_PROTO((struct _trio_class_t *, int));
  /*
   * The function to read characters from a stream.
   */
  void (*InStream) TRIO_PROTO((struct _trio_class_t *, int *));
  /*
   * The current location in the stream.
   */
  trio_pointer_t location;
  /*
   * The character currently being processed.
   */
  int current;
  /*
   * The number of characters that would have been written/read
   * if there had been sufficient space.
   */
  int processed;
  /*
   * The number of characters that are actually written/read.
   * Processed and committed will only differ for the *nprintf
   * and *nscanf functions.
   */
  int committed;
  /*
   * The upper limit of characters that may be written/read.
   */
  int max;
  /*
   * The last output error that was detected.
   */
  int error;
} trio_class_t;

/* References (for user-defined callbacks) */
typedef struct _trio_reference_t {
  trio_class_t *data;
  trio_parameter_t *parameter;
} trio_reference_t;

/* Registered entries (for user-defined callbacks) */
typedef struct _trio_userdef_t {
  struct _trio_userdef_t *next;
  trio_callback_t callback;
  char *name;
} trio_userdef_t;

/*************************************************************************
 *
 * Internal Variables
 *
 *************************************************************************/

static TRIO_CONST char rcsid[] = "@(#)$Id: trio.c,v 1.1 2011/04/15 07:56:38 sunam Exp $";

/*
 * Need this to workaround a parser bug in HP C/iX compiler that fails
 * to resolves macro definitions that includes type 'long double',
 * e.g: va_arg(arg_ptr, long double)
 */
#if defined(TRIO_PLATFORM_MPEIX)
static TRIO_CONST trio_long_double_t ___dummy_long_double = 0;
#endif

static TRIO_CONST char internalNullString[] = "(nil)";

#if defined(USE_LOCALE)
static struct lconv *internalLocaleValues = NULL;
#endif

/*
 * UNIX98 says "in a locale where the radix character is not defined,
 * the radix character defaults to a period (.)"
 */
static int internalDecimalPointLength = 1;
static int internalThousandSeparatorLength = 1;
static char internalDecimalPoint = '.';
static char internalDecimalPointString[MAX_LOCALE_SEPARATOR_LENGTH + 1] = ".";
static char internalThousandSeparator[MAX_LOCALE_SEPARATOR_LENGTH + 1] = ",";
static char internalGrouping[MAX_LOCALE_GROUPS] = { (char)NO_GROUPING };

static TRIO_CONST char internalDigitsLower[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static TRIO_CONST char internalDigitsUpper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static BOOLEAN_T internalDigitsUnconverted = TRUE;
static int internalDigitArray[128];
#if TRIO_EXTENSION
static BOOLEAN_T internalCollationUnconverted = TRUE;
static char internalCollationArray[MAX_CHARACTER_CLASS][MAX_CHARACTER_CLASS];
#endif

#if TRIO_EXTENSION
static TRIO_VOLATILE trio_callback_t internalEnterCriticalRegion = NULL;
static TRIO_VOLATILE trio_callback_t internalLeaveCriticalRegion = NULL;
static trio_userdef_t *internalUserDef = NULL;
#endif


/*************************************************************************
 *
 * Internal Functions
 *
 ************************************************************************/

#if defined(TRIO_MINIMAL)
# define TRIO_STRING_PUBLIC static
# include "triostr.c"
#endif /* defined(TRIO_MINIMAL) */

/*************************************************************************
 * TrioIsQualifier
 *
 * Description:
 *  Remember to add all new qualifiers to this function.
 *  QUALIFIER_POSITION must not be added.
 */
TRIO_PRIVATE BOOLEAN_T
TrioIsQualifier
TRIO_ARGS1((character),
	   TRIO_CONST char character)
{
  /* QUALIFIER_POSITION is not included */
  switch (character)
    {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case QUALIFIER_PLUS:
    case QUALIFIER_MINUS:
    case QUALIFIER_SPACE:
    case QUALIFIER_DOT:
    case QUALIFIER_STAR:
    case QUALIFIER_ALTERNATIVE:
    case QUALIFIER_SHORT:
    case QUALIFIER_LONG:
    case QUALIFIER_LONG_UPPER:
    case QUALIFIER_CIRCUMFLEX:
#if defined(QUALIFIER_SIZE_T)
    case QUALIFIER_SIZE_T:
#endif
#if defined(QUALIFIER_PTRDIFF_T)
    case QUALIFIER_PTRDIFF_T:
#endif
#if defined(QUALIFIER_INTMAX_T)
    case QUALIFIER_INTMAX_T:
#endif
#if defined(QUALIFIER_QUAD)
    case QUALIFIER_QUAD:
#endif
#if defined(QUALIFIER_SIZE_T_UPPER)
    case QUALIFIER_SIZE_T_UPPER:
#endif
#if defined(QUALIFIER_WIDECHAR)
    case QUALIFIER_WIDECHAR:
#endif
#if defined(QUALIFIER_QUOTE)
    case QUALIFIER_QUOTE:
#endif
#if defined(QUALIFIER_STICKY)
    case QUALIFIER_STICKY:
#endif
#if defined(QUALIFIER_VARSIZE)
    case QUALIFIER_VARSIZE:
#endif
#if defined(QUALIFIER_PARAM)
    case QUALIFIER_PARAM:
#endif
#if defined(QUALIFIER_FIXED_SIZE)
    case QUALIFIER_FIXED_SIZE:
#endif
#if defined(QUALIFIER_ROUNDING_UPPER)
    case QUALIFIER_ROUNDING_UPPER:
#endif
      return TRUE;
    default:
      return FALSE;
    }
}

/*************************************************************************
 * TrioSetLocale
 */
#if defined(USE_LOCALE)
TRIO_PRIVATE void
TrioSetLocale(TRIO_NOARGS)
{
  internalLocaleValues = (struct lconv *)localeconv();
  if (internalLocaleValues)
    {
      if ((internalLocaleValues->decimal_point) &&
	  (internalLocaleValues->decimal_point[0] != NIL))
	{
	  internalDecimalPointLength = trio_length(internalLocaleValues->decimal_point);
	  if (internalDecimalPointLength == 1)
	    {
	      internalDecimalPoint = internalLocaleValues->decimal_point[0];
	    }
	  else
	    {
	      internalDecimalPoint = NIL;
	      trio_copy_max(internalDecimalPointString,
			    sizeof(internalDecimalPointString),
			    internalLocaleValues->decimal_point);
	    }
	}
      if ((internalLocaleValues->thousands_sep) &&
	  (internalLocaleValues->thousands_sep[0] != NIL))
	{
	  trio_copy_max(internalThousandSeparator,
			sizeof(internalThousandSeparator),
			internalLocaleValues->thousands_sep);
	  internalThousandSeparatorLength = trio_length(internalThousandSeparator);
	}
      if ((internalLocaleValues->grouping) &&
	  (internalLocaleValues->grouping[0] != NIL))
	{
	  trio_copy_max(internalGrouping,
			sizeof(internalGrouping),
			internalLocaleValues->grouping);
	}
    }
}
#endif /* defined(USE_LOCALE) */

TRIO_PRIVATE int
TrioCalcThousandSeparatorLength
TRIO_ARGS1((digits),
	   int digits)
{
#if TRIO_EXTENSION
  int count = 0;
  int step = NO_GROUPING;
  char *groupingPointer = internalGrouping;

  while (digits > 0)
    {
      if (*groupingPointer == CHAR_MAX)
	{
	  /* Disable grouping */
	  break; /* while */
	}
      else if (*groupingPointer == 0)
	{
	  /* Repeat last group */
	  if (step == NO_GROUPING)
	    {
	      /* Error in locale */
	      break; /* while */
	    }
	}
      else
	{
	  step = *groupingPointer++;
	}
      if (digits > step)
	count += internalThousandSeparatorLength;
      digits -= step;
    }
  return count;
#else
  return 0;
#endif
}

TRIO_PRIVATE BOOLEAN_T
TrioFollowedBySeparator
TRIO_ARGS1((position),
	   int position)
{
#if TRIO_EXTENSION
  int step = 0;
  char *groupingPointer = internalGrouping;

  position--;
  if (position == 0)
    return FALSE;
  while (position > 0)
    {
      if (*groupingPointer == CHAR_MAX)
	{
	  /* Disable grouping */
	  break; /* while */
	}
      else if (*groupingPointer != 0)
	{
	  step = *groupingPointer++;
	}
      if (step == 0)
	break;
      position -= step;
    }
  return (position == 0);
#else
  return FALSE;
#endif
}

/*************************************************************************
 * TrioGetPosition
 *
 * Get the %n$ position.
 */
TRIO_PRIVATE int
TrioGetPosition
TRIO_ARGS2((format, indexPointer),
	   TRIO_CONST char *format,
	   int *indexPointer)
{
#if TRIO_UNIX98
  char *tmpformat;
  int number = 0;
  int index = *indexPointer;

  number = (int)trio_to_long(&format[index], &tmpformat, BASE_DECIMAL);
  index = (int)(tmpformat - format);
  if ((number != 0) && (QUALIFIER_POSITION == format[index++]))
    {
      *indexPointer = index;
      /*
       * number is decreased by 1, because n$ starts from 1, whereas
       * the array it is indexing starts from 0.
       */
      return number - 1;
    }
#endif
  return NO_POSITION;
}

#if TRIO_EXTENSION
/*************************************************************************
 * TrioFindNamespace
 *
 * Find registered user-defined specifier.
 * The prev argument is used for optimization only.
 */
TRIO_PRIVATE trio_userdef_t *
TrioFindNamespace
TRIO_ARGS2((name, prev),
	   TRIO_CONST char *name,
	   trio_userdef_t **prev)
{
  trio_userdef_t *def;
  
  if (internalEnterCriticalRegion)
    (void)internalEnterCriticalRegion(NULL);
  
  for (def = internalUserDef; def; def = def->next)
    {
      /* Case-sensitive string comparison */
      if (trio_equal_case(def->name, name))
	break;
      
      if (prev)
	*prev = def;
    }
  
  if (internalLeaveCriticalRegion)
    (void)internalLeaveCriticalRegion(NULL);
  
  return def;
}
#endif

/*************************************************************************
 * TrioPower
 *
 * Description:
 *  Calculate pow(base, exponent), where number and exponent are integers.
 */
TRIO_PRIVATE trio_long_double_t
TrioPower
TRIO_ARGS2((number, exponent),
	   int number,
	   int exponent)
{
  trio_long_double_t result;

  if (number == 10)
    {
      switch (exponent)
	{
	  /* Speed up calculation of common cases */
	case 0:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E-1);
	  break;
	case 1:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+0);
	  break;
	case 2:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+1);
	  break;
	case 3:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+2);
	  break;
	case 4:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+3);
	  break;
	case 5:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+4);
	  break;
	case 6:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+5);
	  break;
	case 7:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+6);
	  break;
	case 8:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+7);
	  break;
	case 9:
	  result = (trio_long_double_t)number * TRIO_SUFFIX_LONG(1E+8);
	  break;
	default:
	  result = powl((trio_long_double_t)number,
			(trio_long_double_t)exponent);
	  break;
	}
    }
  else
    {
      return powl((trio_long_double_t)number, (trio_long_double_t)exponent);
    }
  return result;
}

/*************************************************************************
 * TrioLogarithm
 */
TRIO_PRIVATE double
TrioLogarithm
TRIO_ARGS2((number, base),
	   double number,
	   int base)
{
  double result;

  if (number <= 0.0)
    {
      /* xlC crashes on log(0) */
      result = (number == 0.0) ? trio_ninf() : trio_nan();
    }
  else
    {
      if (base == 10)
	{
	  result = log10(number);
	}
      else
	{
	  result = log10(number) / log10((double)base);
	}
    }
  return result;
}

/*************************************************************************
 * TrioLogarithmBase
 */
TRIO_PRIVATE double
TrioLogarithmBase
TRIO_ARGS1((base),
	   int base)
{
  switch (base)
    {
    case BASE_BINARY : return 1.0;
    case BASE_OCTAL  : return 3.0;
    case BASE_DECIMAL: return 3.321928094887362345;
    case BASE_HEX    : return 4.0;
    default          : return TrioLogarithm((double)base, 2);
    }
}

/*************************************************************************
 * TrioParse
 *
 * Description:
 *  Parse the format string
 */
TRIO_PRIVATE int
TrioParse
TRIO_ARGS5((type, format, parameters, arglist, argarray),
	   int type,
	   TRIO_CONST char *format,
	   trio_parameter_t *parameters,
	   va_list *arglist,
	   trio_pointer_t *argarray)
{
  /* Count the number of times a parameter is referenced */
  unsigned short usedEntries[MAX_PARAMETERS];
  /* Parameter counters */
  int parameterPosition;
  int currentParam;
  int maxParam = -1;
  /* Utility variables */
  trio_flags_t flags;
  int width;
  int precision;
  int varsize;
  int base;
  int index;  /* Index into formatting string */
  int dots;  /* Count number of dots in modifier part */
  BOOLEAN_T positional;  /* Does the specifier have a positional? */
  BOOLEAN_T gotSticky = FALSE;  /* Are there any sticky modifiers at all? */
  /*
   * indices specifies the order in which the parameters must be
   * read from the va_args (this is necessary to handle positionals)
   */
  int indices[MAX_PARAMETERS];
  int pos = 0;
  /* Various variables */
  char ch;
#if defined(TRIO_COMPILER_SUPPORTS_MULTIBYTE)
  int charlen;
#endif
  int save_errno;
  int i = -1;
  int num;
  char *tmpformat;

  /* One and only one of arglist and argarray must be used */
  assert((arglist != NULL) ^ (argarray != NULL));
  
  /*
   * The 'parameters' array is not initialized, but we need to
   * know which entries we have used.
   */
  memset(usedEntries, 0, sizeof(usedEntries));

  save_errno = errno;
  index = 0;
  parameterPosition = 0;
#if defined(TRIO_COMPILER_SUPPORTS_MULTIBYTE)
  (void)mblen(NULL, 0);
#endif
  
  while (format[index])
    {
#if defined(TRIO_COMPILER_SUPPORTS_MULTIBYTE)
      if (! isascii(format[index]))
	{
	  /*
	   * Multibyte characters cannot be legal specifiers or
	   * modifiers, so we skip over them.
	   */
	  charlen = mblen(&format[index], MB_LEN_MAX);
	  index += (charlen > 0) ? charlen : 1;
	  continue; /* while */
	}
#endif /* TRIO_COMPILER_SUPPORTS_MULTIBYTE */
      if (CHAR_IDENTIFIER == format[index++])
	{
	  if (CHAR_IDENTIFIER == format[index])
	    {
	      index++;
	      continue; /* while */
	    }

	  flags = FLAGS_NEW;
	  dots = 0;
	  currentParam = TrioGetPosition(format, &index);
	  positional = (NO_POSITION != currentParam);
	  if (!positional)
	    {
	      /* We have no positional, get the next counter */
	      currentParam = parameterPosition;
	    }
          if(currentParam >= MAX_PARAMETERS)
	    {
	      /* Bail out completely to make the error more obvious */
	      return TRIO_ERROR_RETURN(TRIO_ETOOMANY, index);
	    }

	  if (currentParam > maxParam)
	    maxParam = currentParam;

	  /* Default values */
	  width = NO_WIDTH;
	  precision = NO_PRECISION;
	  base = NO_BASE;
	  varsize = NO_SIZE;

	  while (TrioIsQualifier(format[index]))
	    {
	      ch = format[index++];

	      switch (ch)
		{
		case QUALIFIER_SPACE:
		  flags |= FLAGS_SPACE;
		  break;

		case QUALIFIER_PLUS:
		  flags |= FLAGS_SHOWSIGN;
		  break;

		case QUALIFIER_MINUS:
		  flags |= FLAGS_LEFTADJUST;
		  flags &= ~FLAGS_NILPADDING;
		  break;

		case QUALIFIER_ALTERNATIVE:
		  flags |= FLAGS_ALTERNATIVE;
		  break;

		case QUALIFIER_DOT:
		  if (dots == 0) /* Precision */
		    {
		      dots++;

		      /* Skip if no precision */
		      if (QUALIFIER_DOT == format[index])
			break;
		      
		      /* After the first dot we have the precision */
		      flags |= FLAGS_PRECISION;
		      if ((QUALIFIER_STAR == format[index])
#if defined(QUALIFIER_PARAM)
			  || (QUALIFIER_PARAM == format[index])
#endif
			  )
			{
			  index++;
			  flags |= FLAGS_PRECISION_PARAMETER;

			  precision = TrioGetPosition(format, &inde