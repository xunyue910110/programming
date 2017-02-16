/*
 * ===========================================================================
 * PRODUCTION $Log: pcre.c,v $
 * PRODUCTION Revision 1.1  2011/04/15 07:56:29  sunam
 * PRODUCTION *** empty log message ***
 * PRODUCTION
 * PRODUCTION Revision 1.1  2011/03/24 12:51:16  hanyu
 * PRODUCTION *** empty log message ***
 * PRODUCTION
 * PRODUCTION Revision 1.1  2009/09/08 15:59:14  shenglj
 * PRODUCTION *** empty log message ***
 * PRODUCTION
 * PRODUCTION Revision 1.1  2005/03/03 06:44:41  admin
 * PRODUCTION *** empty log message ***
 * PRODUCTION
 * PRODUCTION Revision 1.1  2005/03/02 11:44:34  admin
 * PRODUCTION add unix env contents.
 * PRODUCTION
 * PRODUCTION Revision 1000.0  2003/10/29 15:55:43  gouriano
 * PRODUCTION PRODUCTION: IMPORTED [ORIGINAL] Dev-tree R1.2
 * PRODUCTION
 * ===========================================================================
 */

/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/*
This is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language. See
the file Tech.Notes for some information on the internals.

Written by: Philip Hazel <ph10@cam.ac.uk>

           Copyright (c) 1997-2001 University of Cambridge

-----------------------------------------------------------------------------
Permission is granted to anyone to use this software for any purpose on any
computer system, and to redistribute it freely, subject to the following
restrictions:

1. This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

4. If PCRE is embedded in any software that is released under the GNU
   General Purpose Licence (GPL), then the terms of that licence shall
   supersede any condition above with which it is incompatible.
-----------------------------------------------------------------------------
*/

/* Use a macro for debugging printing */

#if defined(PCRE_DEBUG)
#  define DPRINTF(p) printf p
#else
#  define DPRINTF(p) /*nothing*/
#endif

/* Include the internals header, which itself includes Standard C headers plus
the external pcre header. */

#include "pcre_internal.h"

/* Allow compilation as C++ source code, should anybody want to do that. */

#ifdef __cplusplus
#define class pcre_class
#endif


/* Maximum number of items on the nested bracket stacks at compile time. This
applies to the nesting of all kinds of parentheses. It does not limit
un-nested, non-capturing parentheses. This number can be made bigger if
necessary - it is used to dimension one int and one unsigned char vector at
compile time. */

#define BRASTACK_SIZE 200


/* The number of bytes in a literal character string above which we can't add
any more is different when UTF-8 characters may be encountered. */

#ifdef SUPPORT_UTF8
#define MAXLIT 250
#else
#define MAXLIT 255
#endif


/* Min and max values for the common repeats; for the maxima, 0 => infinity */

static const char rep_min[] = { 0, 0, 1, 1, 0, 0 };
static const char rep_max[] = { 0, 0, 0, 0, 1, 1 };

/* Text forms of OP_ values and things, for debugging (not all used) */

#ifdef DEBUG
static const char *OP_names[] = {
  "End", "\\A", "\\B", "\\b", "\\D", "\\d",
  "\\S", "\\s", "\\W", "\\w", "\\Z", "\\z",
  "Opt", "^", "$", "Any", "chars", "not",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{", "{",
  "*", "*?", "+", "+?", "?", "??", "{", "{",
  "class", "Ref", "Recurse",
  "Alt", "Ket", "KetRmax", "KetRmin", "Assert", "Assert not",
  "AssertB", "AssertB not", "Reverse", "Once", "Cond", "Cref",
  "Brazero", "Braminzero", "Branumber", "Bra"
};
#endif

/* Table for handling escaped characters in the range '0'-'z'. Positive returns
are simple data values; negative values are for special things like \d and so
on. Zero means further processing is needed (for things like \x), or the escape
is invalid. */

static const short int escapes[] = {
    0,      0,      0,      0,      0,      0,      0,      0,   /* 0 - 7 */
    0,      0,    ':',    ';',    '<',    '=',    '>',    '?',   /* 8 - ? */
  '@', -ESC_A, -ESC_B,      0, -ESC_D,      0,      0,      0,   /* @ - G */
    0,      0,      0,      0,      0,      0,      0,      0,   /* H - O */
    0,      0,      0, -ESC_S,      0,      0,      0, -ESC_W,   /* P - W */
    0,      0, -ESC_Z,    '[',   '\\',    ']',    '^',    '_',   /* X - _ */
  '`',      7, -ESC_b,      0, -ESC_d,  ESC_E,  ESC_F,      0,   /* ` - g */
    0,      0,      0,      0,      0,      0,  ESC_N,      0,   /* h - o */
    0,      0,  ESC_R, -ESC_s,  ESC_T,      0,      0, -ESC_w,   /* p - w */
    0,      0, -ESC_z                                            /* x - z */
};

/* Tables of names of POSIX character classes and their lengths. The list is
terminated by a zero length entry. The first three must be alpha, upper, lower,
as this is assumed for handling case independence. */

static const char *posix_names[] = {
  "alpha", "lower", "upper",
  "alnum", "ascii", "cntrl", "digit", "graph",
  "print", "punct", "space", "word",  "xdigit" };

static const uschar posix_name_lengths[] = {
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 6, 0 };

/* Table of class bit maps for each POSIX class; up to three may be combined
to form the class. */

static const int posix_class_maps[] = {
  cbit_lower, cbit_upper, -1,             /* alpha */
  cbit_lower, -1,         -1,             /* lower */
  cbit_upper, -1,         -1,             /* upper */
  cbit_digit, cbit_lower, cbit_upper,     /* alnum */
  cbit_print, cbit_cntrl, -1,             /* ascii */
  cbit_cntrl, -1,         -1,             /* cntrl */
  cbit_digit, -1,         -1,             /* digit */
  cbit_graph, -1,         -1,             /* graph */
  cbit_print, -1,         -1,             /* print */
  cbit_punct, -1,         -1,             /* punct */
  cbit_space, -1,         -1,             /* space */
  cbit_word,  -1,         -1,             /* word */
  cbit_xdigit,-1,         -1              /* xdigit */
};


/* Definition to allow mutual recursion */

static BOOL
  compile_regex(int, int, int *, uschar **, const uschar **, const char **,
    BOOL, int, int *, int *, compile_data *);

/* Structure for building a chain of data that actually lives on the
stack, for holding the values of the subject pointer at the start of each
subpattern, so as to detect when an empty string has been matched by a
subpattern - to break infinite loops. */

typedef struct eptrblock {
  struct eptrblock *prev;
  const uschar *saved_eptr;
} eptrblock;

/* Flag bits for the match() function */

#define match_condassert   0x01    /* Called to check a condition assertion */
#define match_isgroup      0x02    /* Set if start of bracketed group */



/*************************************************
*               Global variables                 *
*************************************************/

/* PCRE is thread-clean and doesn't use any global variables in the normal
sense. However, it calls memory allocation and free functions via the two
indirections below, which are can be changed by the caller, but are shared
between all threads. */

void *(*pcre_malloc)(size_t) = malloc;
void  (*pcre_free)(void *) = free;



/*************************************************
*    Macros and tables for character handling    *
*************************************************/

/* When UTF-8 encoding is being used, a character is no longer just a single
byte. The macros for character handling generate simple sequences when used in
byte-mode, and more complicated ones for UTF-8 characters. */

#ifndef SUPPORT_UTF8
#define GETCHARINC(c, eptr) c = *eptr++;
#define GETCHARLEN(c, eptr, len) c = *eptr;
#define BACKCHAR(eptr)

#else   /* SUPPORT_UTF8 */

/* Get the next UTF-8 character, advancing the pointer */

#define GETCHARINC(c, eptr) \
  c = *eptr++; \
  if (md->utf8 && (c & 0xc0) == 0xc0) \
    { \
    int a = utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int s = 6*a; \
    c = (c & utf8_table3[a]) << s; \
    while (a-- > 0) \
      { \
      s -= 6; \
      c |= (*eptr++ & 0x3f) << s; \
      } \
    }

/* Get the next UTF-8 character, not advancing the pointer, setting length */

#define GETCHARLEN(c, eptr, len) \
  c = *eptr; \
  len = 1; \
  if (md->utf8 && (c & 0xc0) == 0xc0) \
    { \
    int i; \
    int a = utf8_table4[c & 0x3f];  /* Number of additional bytes */ \
    int s = 6*a; \
    c = (c & utf8_table3[a]) << s; \
    for (i = 1; i <= a; i++) \
      { \
      s -= 6; \
      c |= (eptr[i] & 0x3f) << s; \
      } \
    len += a; \
    }

/* If the pointer is not at the start of a character, move it back until
it is. */

#define BACKCHAR(eptr) while((*eptr & 0xc0) == 0x80) eptr--;

#endif



/*************************************************
*             Default character tables           *
*************************************************/

/* A default set of character tables is included in the PCRE binary. Its source
is built by the maketables auxiliary program, which uses the default C ctypes
functions, and put in the file chartables.c. These tables are used by PCRE
whenever the caller of pcre_compile() does not provide an alternate set of
tables. */

#include "chartables.c"



#ifdef SUPPORT_UTF8
/*************************************************
*           Tables for UTF-8 support             *
*************************************************/

/* These are the breakpoints for different numbers of bytes in a UTF-8
character. */

static int utf8_table1[] = { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};

/* These are the indicator bits and the mask for the data bits to set in the
first byte of a character, indexed by the number of additional bytes. */

static int utf8_table2[] = { 0,    0xc0, 0xe0, 0xf0, 0xf8, 0xfc};
static int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

/* Table of the number of extra characters, indexed by the first character
masked with 0x3f. The highest number for a valid UTF-8 character is in fact
0x3d. */

static uschar utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };


/*************************************************
*       Convert character value to UTF-8         *
*************************************************/

/* This function takes an integer value in the range 0 - 0x7fffffff
and encodes it as a UTF-8 character in 0 to 6 bytes.

Arguments:
  cvalue     the character value
  buffer     pointer to buffer for result - at least 6 bytes long

Returns:     number of characters placed in the buffer
*/

static int
ord2utf8(int cvalue, uschar *buffer)
{
register int i, j;
for (i = 0; i < sizeof(utf8_table1)/sizeof(int); i++)
  if (cvalue <= utf8_table1[i]) break;
buffer += i;
for (j = i; j > 0; j--)
 {
 *buffer-- = 0x80 | (cvalue & 0x3f);
 cvalue >>= 6;
 }
*buffer = utf8_table2[i] | cvalue;
return i + 1;
}
#endif



/*************************************************
*          Return version string                 *
*************************************************/

#define STRING(a)  # a
#define XSTRING(s) STRING(s)

const char *
pcre_version(void)
{
return XSTRING(PCRE_MAJOR) "." XSTRING(PCRE_MINOR) " " XSTRING(PCRE_DATE);
}




/*************************************************
* (Obsolete) Return info about compiled pattern  *
*************************************************/

/* This is the original "info" function. It picks potentially useful data out
of the private structure, but its interface was too rigid. It remains for
backwards compatibility. The public options are passed back in an int - though
the re->options field has been expanded to a long int, all the public options
at the low end of it, and so even on 16-bit systems this will still be OK.
Therefore, I haven't changed the API for pcre_info().

Arguments:
  external_re   points to compiled code
  optptr        where to pass back the options
  first_char    where to pass back the first character,
                or -1 if multiline and all branches start ^,
                or -2 otherwise

Returns:        number of capturing subpatterns
                or negative values on error
*/

int
pcre_info(const pcre *external_re, int *optptr, int *first_char)
{
const real_pcre *re = (const real_pcre *)external_re;
if (re == NULL) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;
if (optptr != NULL) *optptr = (int)(re->options & PUBLIC_OPTIONS);
if (first_char != NULL)
  *first_char = ((re->options & PCRE_FIRSTSET) != 0)? re->first_char :
     ((re->options & PCRE_STARTLINE) != 0)? -1 : -2;
return re->top_bracket;
}



/*************************************************
*        Return info about compiled pattern      *
*************************************************/

/* This is a newer "info" function which has an extensible interface so
that additional items can be added compatibly.

Arguments:
  external_re      points to compiled code
  external_study   points to study data, or NULL
  what             what information is required
  where            where to put the information

Returns:           0 if data returned, negative on error
*/

int
pcre_fullinfo(const pcre *external_re, const pcre_extra *study_data, int what,
  void *where)
{
const real_pcre *re = (const real_pcre *)external_re;
const real_pcre_extra *study = (const real_pcre_extra *)study_data;

if (re == NULL || where == NULL) return PCRE_ERROR_NULL;
if (re->magic_number != MAGIC_NUMBER) return PCRE_ERROR_BADMAGIC;

switch (what)
  {
  case PCRE_INFO_OPTIONS:
  *((unsigned long int *)where) = re->options & PUBLIC_OPTIONS;
  break;

  case PCRE_INFO_SIZE:
  *((size_t *)where) = re->size;
  break;

  case PCRE_INFO_CAPTURECOUNT:
  *((int *)where) = re->top_bracket;
  break;

  case PCRE_INFO_BACKREFMAX:
  *((int *)where) = re->top_backref;
  break;

  case PCRE_INFO_FIRSTCHAR:
  *((int *)where) =
    ((re->options & PCRE_FIRSTSET) != 0)? re->first_char :
    ((re->options & PCRE_STARTLINE) != 0)? -1 : -2;
  break;

  case PCRE_INFO_FIRSTTABLE:
  *((const uschar **)where) =
    (study != NULL && (study->options & PCRE_STUDY_MAPPED) != 0)?
      study->start_bits : NULL;
  break;

  case PCRE_INFO_LASTLITERAL:
  *((int *)where) =
    ((re->options & PCRE_REQCHSET) != 0)? re->req_char : -1;
  break;

  default: return PCRE_ERROR_BADOPTION;
  }

return 0;
}



#ifdef DEBUG
/*************************************************
*        Debugging function to print chars       *
*************************************************/

/* Print a sequence of chars in printable format, stopping at the end of the
subject if the requested.

Arguments:
  p           points to characters
  length      number to print
  is_subject  TRUE if printing from within md->start_subject
  md          pointer to matching data block, if is_subject is TRUE

Returns:     nothing
*/

static void
pchars(const uschar *p, int length, BOOL is_subject, match_data *md)
{
int c;
if (is_subject && length > md->end_subject - p) length = md->end_subject - p;
while (length-- > 0)
  if (isprint(c = *(p++))) printf("%c", c); else printf("\\x%02x", c);
}
#endif




/*************************************************
*            Handle escapes                      *
*************************************************/

/* This function is called when a \ has been encountered. It either returns a
positive value for a simple escape such as \n, or a negative value which
encodes one of the more complicated things such as \d. When UTF-8 is enabled,
a positive value greater than 255 may be returned. On entry, ptr is pointing at
the \. On exit, it is on the final character of the escape sequence.

Arguments:
  ptrptr     points to the pattern position pointer
  errorptr   points to the pointer to the error message
  bracount   number of previous extracting brackets
  options    the options bits
  isclass    TRUE if inside a character class
  cd         pointer to char tables block

Returns:     zero or positive => a data character
             negative => a special escape sequence
             on error, errorptr is set
*/

static int
check_escape(const uschar **ptrptr, const char **errorptr, int bracount,
  int options, BOOL isclass, compile_data *cd)
{
const uschar *ptr = *ptrptr;
int c, i;

/* If backslash is at the end of the pattern, it's an error. */

c = *(++ptr);
if (c == 0) *errorptr = ERR1;

/* Digits or letters may have special meaning; all others are literals. */

else if (c < '0' || c > 'z') {}

/* Do an initial lookup in a table. A non-zero result is something that can be
returned immediately. Otherwise further processing may be required. */

else if ((i = escapes[c - '0']) != 0) c = i;

/* Escapes that need further processing, or are illegal. */

else
  {
  const uschar *oldptr;
  switch (c)
    {
    /* The handling of escape sequences consisting of a string of digits
    starting with one that is not zero is not straightforward. By experiment,
    the way Perl works seems to be as follows:

    Outside a character class, the digits are read as a decimal number. If the
    number is less than 10, or if there are that many previous extracting
    left brackets, then it is a back reference. Otherwise, up to three octal
    digits are read to form an escaped byte. Thus \123 is likely to be octal
    123 (cf \0123, which is octal 012 followed by the literal 3). If the octal
    value is greater than 377, the least significant 8 bits are taken. Inside a
    character class, \ followed by a digit is always an octal number. */

    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':

    if (!isclass)
      {
      oldptr = ptr;
      c -= '0';
      while ((cd->ctypes[ptr[1]] & ctype_digit) != 0)
        c = c * 10 + *(++ptr) - '0';
      if (c < 10 || c <= bracount)
        {
        c = -(ESC_REF + c);
        break;
        }
      ptr = oldptr;      /* Put the pointer back and fall through */
      }

    /* Handle an octal number following \. If the first digit is 8 or 9, Perl
    generates a binary zero byte and treats the digit as a following literal.
    Thus we have to pull back the pointer by one. */

    if ((c = *ptr) >= '8')
      {
      ptr--;
      c = 0;
      break;
      }

    /* \0 always starts an octal number, but we may drop through to here with a
    larger first octal digit. */

    case '0':
    c -= '0';
    while(i++ < 2 && (cd->ctypes[ptr[1]] & ctype_digit) != 0 &&
      ptr[1] != '8' && ptr[1] != '9')
        c = c * 8 + *(++ptr) - '0';
    c &= 255;     /* Take least significant 8 bits */
    break;

    /* \x is complicated when UTF-8 is enabled. \x{ddd} is a character number
    which can be greater than 0xff, but only if the ddd are hex digits. */

    case 'x':
#ifdef SUPPORT_UTF8
    if (ptr[1] == '{' && (options & PCRE_UTF8) != 0)
      {
      const uschar *pt = ptr + 2;
      register int count = 0;
      c = 0;
      while ((cd->ctypes[*pt] & ctype_xdigit) != 0)
        {
        count++;
        c = c * 16 + cd->lcc[*pt] -
          (((cd->ctypes[*pt] & ctype_digit) != 0)? '0' : 'W');
        pt++;
        }
      if (*pt == '}')
        {
        if (c < 0 || count > 8) *errorptr = ERR34;
        ptr = pt;
        break;
        }
      /* If the sequence of hex digits does not end with '}', then we don't
      recognize this construct; fall through to the normal \x handling. */
      }
#endif

    /* Read just a single hex char */

    c = 0;
    while (i++ < 2 && (cd->ctypes[ptr[1]] & ctype_xdigit) != 0)
      {
      ptr++;
      c = c * 16 + cd->lcc[*ptr] -
        (((cd->ctypes[*ptr] & ctype_digit) != 0)? '0' : 'W');
      }
    break;

    /* Other special escapes not starting with a digit are straightforward */

    case 'c':
    c = *(++ptr);
    if (c == 0)
      {
      *errorptr = ERR2;
      return 0;
      }

    /* A letter is upper-cased; then the 0x40 bit is flipped */

    if (c >= 'a' && c <= 'z') c = cd->fcc[c];
    c ^= 0x40;
    break;

    /* PCRE_EXTRA enables extensions to Perl in the matter of escapes. Any
    other alphameric following \ is an error if PCRE_EXTRA was set; otherwise,
    for Perl compatibility, it is a literal. This code looks a bit odd, but
    there used to be some cases other than the default, and there may be again
    in future, so I haven't "optimized" it. */

    default:
    if ((options & PCRE_EXTRA) != 0) switch(c)
      {
      default:
      *errorptr = ERR3;
      break;
      }
    break;
    }
  }

*ptrptr = ptr;
return c;
}



/*************************************************
*            Check for counted repeat            *
*************************************************/

/* This function is called when a '{' is encountered in a place where it might
start a quantifier. It looks ahead to see if it really is a quantifier or not.
It is only a quantifier if it is one of the forms {ddd} {ddd,} or {ddd,ddd}
where the ddds are digits.

Arguments:
  p         pointer to the first char after '{'
  cd        pointer to char tables block

Returns:    TRUE or FALSE
*/

static BOOL
is_counted_repeat(const uschar *p, compile_data *cd)
{
if ((cd->ctypes[*p++] & ctype_digit) == 0) return FALSE;
while ((cd->ctypes[*p] & ctype_digit) != 0) p++;
if (*p == '}') return TRUE;

if (*p++ != ',') return FALSE;
if (*p == '}') return TRUE;

if ((cd->ctypes[*p++] & ctype_digit) == 0) return FALSE;
while ((cd->ctypes[*p] & ctype_digit) != 0) p++;
return (*p == '}');
}



/*************************************************
*         Read repeat counts                     *
*************************************************/

/* Read an item of the form {n,m} and return the values. This is called only
after is_counted_repeat() has confirmed that a repeat-count quantifier exists,
so the syntax is guaranteed to be correct, but we need to check the values.

Arguments:
  p          pointer to first char after '{'
  minp       pointer to int for min
  maxp       pointer to int for max
             returned as -1 if no max
  errorptr   points to pointer to error message
  cd         pointer to character tables clock

Returns:     pointer to '}' on success;
             current ptr on error, with errorptr set
*/

static const uschar *
read_repeat_counts(const uschar *p, int *minp, int *maxp,
  const char **errorptr, compile_data *cd)
{
int min = 0;
int max = -1;

while ((cd->ctypes[*p] & ctype_digit) != 0) min = min * 10 + *p++ - '0';

if (*p == '}') max = min; else
  {
  if (*(++p) != '}')
    {
    max = 0;
    while((cd->ctypes[*p] & ctype_digit) != 0) max = max * 10 + *p++ - '0';
    if (max < min)
      {
      *errorptr = ERR4;
      return p;
      }
    }
  }

/* Do paranoid checks, then fill in the required variables, and pass back the
pointer to the terminating '}'. */

if (min > 65535 || max > 65535)
  *errorptr = ERR5;
else
  {
  *minp = min;
  *maxp = max;
  }
return p;
}



/*************************************************
*        Find the fixed length of a pattern      *
*************************************************/

/* Scan a pattern and compute the fixed length of subject that will match it,
if the length is fixed. This is needed for dealing with backward assertions.

Arguments:
  code     points to the start of the pattern (the bracket)
  options  the compiling options

Returns:   the fixed length, or -1 if there is no fixed length
*/

static int
find_fixedlength(uschar *code, int options)
{
int length = -1;

register int branchlength = 0;
register uschar *cc = code + 3;

/* Scan along the opcodes for this branch. If we get to the end of the
branch, check the length against that of the other branches. */

for (;;)
  {
  int d;
  register int op = *cc;
  if (op >= OP_BRA) op = OP_BRA;

  switch (op)
    {
    case OP_BRA:
    case OP_ONCE:
    case OP_COND:
    d = find_fixedlength(cc, options);
    if (d < 0) return -1;
    branchlength += d;
    do cc += (cc[1] << 8) + cc[2]; while (*cc == OP_ALT);
    cc += 3;
    break;

    /* Reached end of a branch; if it's a ket it is the end of a nested
    call. If it's ALT it is an alternation in a nested call. If it is
    END it's the end of the outer call. All can be handled by the same code. */

    case OP_ALT:
    case OP_KET:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_END:
    if (length < 0) length = branchlength;
      else if (length != branchlength) return -1;
    if (*cc != OP_ALT) return length;
    cc += 3;
    branchlength = 0;
    break;

    /* Skip over assertive subpatterns */

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    do cc += (cc[1] << 8) + cc[2]; while (*cc == OP_ALT);
    cc += 3;
    break;

    /* Skip over things that don't match chars */

    case OP_REVERSE:
    case OP_BRANUMBER:
    case OP_CREF:
    cc++;
    /* Fall through */

    case OP_OPT:
    cc++;
    /* Fall through */

    case OP_SOD:
    case OP_EOD:
    case OP_EODN:
    case OP_CIRC:
    case OP_DOLL:
    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
    cc++;
    break;

    /* Handle char strings. In UTF-8 mode we must count characters, not bytes.
    This requires a scan of the string, unfortunately. We assume valid UTF-8
    strings, so all we do is reduce the length by one for byte whose bits are
    10xxxxxx. */

    case OP_CHARS:
    branchlength += *(++cc);
#ifdef SUPPORT_UTF8
    for (d = 1; d <= *cc; d++)
      if ((cc[d] & 0xc0) == 0x80) branchlength--;
#endif
    cc += *cc + 1;
    break;

    /* Handle exact repetitions */

    case OP_EXACT:
    case OP_TYPEEXACT:
    branchlength += (cc[1] << 8) + cc[2];
    cc += 4;
    break;

    /* Handle single-char matchers */

    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    branchlength++;
    cc++;
    break;


    /* Check a class for variable quantification */

    case OP_CLASS:
    cc += 33;

    switch (*cc)
      {
      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      return -1;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      if ((cc[1] << 8) + cc[2] != (cc[3] << 8) + cc[4]) return -1;
      branchlength += (cc[1] << 8) + cc[2];
      cc += 5;
      break;

      default:
      branchlength++;
      }
    break;

    /* Anything else is variable length */

    default:
    return -1;
    }
  }
/* Control never gets here */
}




/*************************************************
*           Check for POSIX class syntax         *
*************************************************/

/* This function is called when the sequence "[:" or "[." or "[=" is
encountered in a character class. It checks whether this is followed by an
optional ^ and then a sequence of letters, terminated by a matching ":]" or
".]" or "=]".

Argument:
  ptr      pointer to the initial [
  endptr   where to return the end pointer
  cd       pointer to compile data

Returns:   TRUE or FALSE
*/

static BOOL
check_posix_syntax(const uschar *ptr, const uschar **endptr, compile_data *cd)
{
int terminator;          /* Don't combine these lines; the Solaris cc */
terminator = *(++ptr);   /* compiler warns about "non-constant" initializer. */
if (*(++ptr) == '^') ptr++;
while ((cd->ctypes[*ptr] & ctype_letter) != 0) ptr++;
if (*ptr == terminator && ptr[1] == ']')
  {
  *endptr = ptr;
  return TRUE;
  }
return FALSE;
}




/*************************************************
*          Check POSIX class name                *
*************************************************/

/* This function is called to check the name given in a POSIX-style class entry
such as [:alnum:].

Arguments:
  ptr        points to the first letter
  len        the length of the name

Returns:     a value representing the name, or -1 if unknown
*/

static int
check_posix_name(const uschar *ptr, int len)
{
register int yield = 0;
while (posix_name_lengths[yield] != 0)
  {
  if (len == posix_name_lengths[yield] &&
    strncmp((const char *)ptr, posix_names[yield], len) == 0) return yield;
  yield++;
  }
return -1;
}




/*************************************************
*           Compile one branch                   *
*************************************************/

/* Scan the pattern, compiling it into the code vector.

Arguments:
  options      the option bits
  brackets     points to number of extracting brackets used
  code         points to the pointer to the current code point
  ptrptr       points to the current pattern pointer
  errorptr     points to pointer to error message
  optchanged   set to the value of the last OP_OPT item compiled
  reqchar      set to the last literal character required, else -1
  countlits    set to count of mandatory literal characters
  cd           contains pointers to tables

Returns:       TRUE on success
               FALSE, with *errorptr set on error
*/

static BOOL
compile_branch(int options, int *brackets, uschar **codeptr,
  const uschar **ptrptr, const char **errorptr, int *optchanged,
  int *reqchar, int *countlits, compile_data *cd)
{
int repeat_type, op_type;
int repeat_min, repeat_max;
int bravalue, length;
int greedy_default, greedy_non_default;
int prevreqchar;
int condcount = 0;
int subcountlits = 0;
register int c;
register uschar *code = *codeptr;
uschar *tempcode;
const uschar *ptr = *ptrptr;
const uschar *tempptr;
uschar *previous = NULL;
uschar class[32];

/* Set up the default and non-default settings for greediness */

greedy_default = ((options & PCRE_UNGREEDY) != 0);
greedy_non_default = greedy_default ^ 1;

/* Initialize no required char, and count of literals */

*reqchar = prevreqchar = -1;
*countlits = 0;

/* Switch on next character until the end of the branch */

for (;; ptr++)
  {
  BOOL negate_class;
  int class_charcount;
  int class_lastchar;
  int newoptions;
  int skipbytes;
  int subreqchar;

  c = *ptr;
  if ((options & PCRE_EXTENDED) != 0)
    {
    if ((cd->ctypes[c] & ctype_space) != 0) continue;
    if (c == '#')
      {
      /* The space before the ; is to avoid a warning on a silly compiler
      on the Macintosh. */
      while ((c = *(++ptr)) != 0 && c != NEWLINE) ;
      continue;
      }
    }

  switch(c)
    {
    /* The branch terminates at end of string, |, or ). */

    case 0:
    case '|':
    case ')':
    *codeptr = code;
    *ptrptr = ptr;
    return TRUE;

    /* Handle single-character metacharacters */

    case '^':
    previous = NULL;
    *code++ = OP_CIRC;
    break;

    case '$':
    previous = NULL;
    *code++ = OP_DOLL;
    break;

    case '.':
    previous = code;
    *code++ = OP_ANY;
    break;

    /* Character classes. These always build a 32-byte bitmap of the permitted
    characters, except in the special case where there is only one character.
    For negated classes, we build the map as usual, then invert it at the end.
    */

    case '[':
    previous = code;
    *code++ = OP_CLASS;

    /* If the first character is '^', set the negation flag and skip it. */

    if ((c = *(++ptr)) == '^')
      {
      negate_class = TRUE;
      c = *(++ptr);
      }
    else negate_class = FALSE;

    /* Keep a count of chars so that we can optimize the case of just a single
    character. */

    class_charcount = 0;
    class_lastchar = -1;

    /* Initialize the 32-char bit map to all zeros. We have to build the
    map in a temporary bit of store, in case the class contains only 1
    character, because in that case the compiled code doesn't use the
    bit map. */

    memset(class, 0, 32 * sizeof(uschar));

    /* Process characters until ] is reached. By writing this as a "do" it
    means that an initial ] is taken as a data character. */

    do
      {
      if (c == 0)
        {
        *errorptr = ERR6;
        goto FAILED;
        }

      /* Handle POSIX class names. Perl allows a negation extension of the
      form [:^name]. A square bracket that doesn't match the syntax is
      treated as a literal. We also recognize the POSIX constructions
      [.ch.] and [=ch=] ("collating elements") and fault them, as Perl
      5.6 does. */

      if (c == '[' &&
          (ptr[1] ==