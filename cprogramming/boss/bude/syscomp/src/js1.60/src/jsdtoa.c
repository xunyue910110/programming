/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * Portable double to alphanumeric string and back converters.
 */
#include "jsstddef.h"
#include "jslibmath.h"
#include "jstypes.h"
#include "jsdtoa.h"
#include "jsprf.h"
#include "jsutil.h" /* Added by JSIFY */
#include "jspubtd.h"
#include "jsnum.h"

#ifdef JS_THREADSAFE
#include "prlock.h"
#endif

/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991 by Lucent Technologies.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/

/* Please send bug reports to
    David M. Gay
    Bell Laboratories, Room 2C-463
    600 Mountain Avenue
    Murray Hill, NJ 07974-0636
    U.S.A.
    dmg@bell-labs.com
 */

/* On a machine with IEEE extended-precision registers, it is
 * necessary to specify double-precision (53-bit) rounding precision
 * before invoking strtod or dtoa.  If the machine uses (the equivalent
 * of) Intel 80x87 arithmetic, the call
 *  _control87(PC_53, MCW_PC);
 * does this with many compilers.  Whether this or another call is
 * appropriate depends on the compiler; for this to work, it may be
 * necessary to #include "float.h" or another system-dependent header
 * file.
 */

/* strtod for IEEE-arithmetic machines.
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets err to JS_DTOA_ERANGE or JS_DTOA_ENOMEM).  With IEEE
 * arithmetic, ties are broken by the IEEE round-even rule.  Otherwise
 * ties are broken by biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 92-101].
 *
 * Modifications:
 *
 *  1. We only require IEEE double-precision
 *      arithmetic (not IEEE double-extended).
 *  2. We get by with floating-point arithmetic in a case that
 *      Clinger missed -- when we're computing d * 10^n
 *      for a small integer d and the integer n is not too
 *      much larger than 22 (the maximum integer k for which
 *      we can represent 10^k exactly), we may be able to
 *      compute (d*10^k) * 10^(e-k) with just one roundoff.
 *  3. Rather than a bit-at-a-time adjustment of the binary
 *      result in the hard case, we use floating-point
 *      arithmetic to determine the adjustment to within
 *      one bit; only in really hard cases do we need to
 *      compute a second residual.
 *  4. Because of 3., we don't need a large table of powers of 10
 *      for ten-to-e (just some small tables, e.g. of 10^k
 *      for 0 <= k <= 22).
 */

/*
 * #define IEEE_8087 for IEEE-arithmetic machines where the least
 *  significant byte has the lowest address.
 * #define IEEE_MC68k for IEEE-arithmetic machines where the most
 *  significant byte has the lowest address.
 * #define Long int on machines with 32-bit ints and 64-bit longs.
 * #define Sudden_Underflow for IEEE-format machines without gradual
 *  underflow (i.e., that flush to zero on underflow).
 * #define No_leftright to omit left-right logic in fast floating-point
 *  computation of js_dtoa.
 * #define Check_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3.
 * #define RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
 *  that use extended-precision instructions to compute rounded
 *  products and quotients) with IBM.
 * #define ROUND_BIASED for IEEE-format with biased rounding.
 * #define Inaccurate_Divide for IEEE-format with correctly rounded
 *  products but inaccurate quotients, e.g., for Intel i860.
 * #define JS_HAVE_LONG_LONG on machines that have a "long long"
 *  integer type (of >= 64 bits).  If long long is available and the name is
 *  something other than "long long", #define Llong to be the name,
 *  and if "unsigned Llong" does not work as an unsigned version of
 *  Llong, #define #ULLong to be the corresponding unsigned type.
 * #define Bad_float_h if your system lacks a float.h or if it does not
 *  define some or all of DBL_DIG, DBL_MAX_10_EXP, DBL_MAX_EXP,
 *  FLT_RADIX, FLT_ROUNDS, and DBL_MAX.
 * #define MALLOC your_malloc, where your_malloc(n) acts like malloc(n)
 *  if memory is available and otherwise does something you deem
 *  appropriate.  If MALLOC is undefined, malloc will be invoked
 *  directly -- and assumed always to succeed.
 * #define Omit_Private_Memory to omit logic (added Jan. 1998) for making
 *  memory allocations from a private pool of memory when possible.
 *  When used, the private pool is PRIVATE_MEM bytes long: 2000 bytes,
 *  unless #defined to be a different length.  This default length
 *  suffices to get rid of MALLOC calls except for unusual cases,
 *  such as decimal-to-binary conversion of a very long string of
 *  digits.
 * #define INFNAN_CHECK on IEEE systems to cause strtod to check for
 *  Infinity and NaN (case insensitively).  On some systems (e.g.,
 *  some HP systems), it may be necessary to #define NAN_WORD0
 *  appropriately -- to the most significant word of a quiet NaN.
 *  (On HP Series 700/800 machines, -DNAN_WORD0=0x7ff40000 works.)
 * #define MULTIPLE_THREADS if the system offers preemptively scheduled
 *  multiple threads.  In this case, you must provide (or suitably
 *  #define) two locks, acquired by ACQUIRE_DTOA_LOCK() and released
 *  by RELEASE_DTOA_LOCK().  (The second lock, accessed
 *  in pow5mult, ensures lazy evaluation of only one copy of high
 *  powers of 5; omitting this lock would introduce a small
 *  probability of wasting memory, but would otherwise be harmless.)
 *  You must also invoke freedtoa(s) to free the value s returned by
 *  dtoa.  You may do so whether or not MULTIPLE_THREADS is #defined.
 * #define NO_IEEE_Scale to disable new (Feb. 1997) logic in strtod that
 *  avoids underflows on inputs whose result does not underflow.
 */
#ifdef IS_LITTLE_ENDIAN
#define IEEE_8087
#else
#define IEEE_MC68k
#endif

#ifndef Long
#define Long int32
#endif

#ifndef ULong
#define ULong uint32
#endif

#define Bug(errorMessageString) JS_ASSERT(!errorMessageString)

#include "stdlib.h"
#include "string.h"

#ifdef MALLOC
extern void *MALLOC(size_t);
#else
#define MALLOC malloc
#endif

#define Omit_Private_Memory
/* Private memory currently doesn't work with JS_THREADSAFE */
#ifndef Omit_Private_Memory
#ifndef PRIVATE_MEM
#define PRIVATE_MEM 2000
#endif
#define PRIVATE_mem ((PRIVATE_MEM+sizeof(double)-1)/sizeof(double))
static double private_mem[PRIVATE_mem], *pmem_next = private_mem;
#endif

#ifdef Bad_float_h
#undef __STDC__

#define DBL_DIG 15
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP 1024
#define FLT_RADIX 2
#define FLT_ROUNDS 1
#define DBL_MAX 1.7976931348623157e+308



#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif

#else /* ifndef Bad_float_h */
#include "float.h"
#endif /* Bad_float_h */

#ifndef __MATH_H__
#include "math.h"
#endif

#ifndef CONST
#define CONST const
#endif

#if defined(IEEE_8087) + defined(IEEE_MC68k) != 1
Exactly one of IEEE_8087 or IEEE_MC68k should be defined.
#endif

#define word0(x)        JSDOUBLE_HI32(x)
#define set_word0(x, y) JSDOUBLE_SET_HI32(x, y)
#define word1(x)        JSDOUBLE_LO32(x)
#define set_word1(x, y) JSDOUBLE_SET_LO32(x, y)

#define Storeinc(a,b,c) (*(a)++ = (b) << 16 | (c) & 0xffff)

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#define Exp_shift  20
#define Exp_shift1 20
#define Exp_msk1    0x100000
#define Exp_msk11   0x100000
#define Exp_mask  0x7ff00000
#define P 53
#define Bias 1023
#define Emin (-1022)
#define Exp_1  0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask  0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask  0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14
#define Infinite(x) (word0(x) == 0x7ff00000) /* sufficient test for here */
#ifndef NO_IEEE_Scale
#define Avoid_Underflow
#endif



#ifdef RND_PRODQUOT
#define rounded_product(a,b) a = rnd_prod(a, b)
#define rounded_quotient(a,b) a = rnd_quot(a, b)
extern double rnd_prod(double, double), rnd_quot(double, double);
#else
#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1*(DBL_MAX_EXP+Bias-1))
#define Big1 0xffffffff

#ifndef JS_HAVE_LONG_LONG
#undef ULLong
#else   /* long long available */
#ifndef Llong
#define Llong JSInt64
#endif
#ifndef ULLong
#define ULLong JSUint64
#endif
#endif /* JS_HAVE_LONG_LONG */

#ifdef JS_THREADSAFE
#define MULTIPLE_THREADS
static PRLock *freelist_lock;
#define ACQUIRE_DTOA_LOCK()                                                   \
    JS_BEGIN_MACRO                                                            \
        if (!initialized)                                                     \
            InitDtoa();                                                       \
        PR_Lock(freelist_lock);                                               \
    JS_END_MACRO
#define RELEASE_DTOA_LOCK() PR_Unlock(freelist_lock)
#else
#undef MULTIPLE_THREADS
#define ACQUIRE_DTOA_LOCK()   /*nothing*/
#define RELEASE_DTOA_LOCK()   /*nothing*/
#endif

#define Kmax 15

struct Bigint {
    struct Bigint *next;  /* Free list link */
    int32 k;              /* lg2(maxwds) */
    int32 maxwds;         /* Number of words allocated for x */
    int32 sign;           /* Zero if positive, 1 if negative.  Ignored by most Bigint routines! */
    int32 wds;            /* Actual number of words.  If value is nonzero, the most significant word must be nonzero. */
    ULong x[1];           /* wds words of number in little endian order */
};

#ifdef ENABLE_OOM_TESTING
/* Out-of-memory testing.  Use a good testcase (over and over) and then use
 * these routines to cause a memory failure on every possible Balloc allocation,
 * to make sure that all out-of-memory paths can be followed.  See bug 14044.
 */

static int allocationNum;               /* which allocation is next? */
static int desiredFailure;              /* which allocation should fail? */

/**
 * js_BigintTestingReset
 *
 * Call at the beginning of a test run to set the allocation failure position.
 * (Set to 0 to just have the engine count allocations without failing.)
 */
JS_PUBLIC_API(void)
js_BigintTestingReset(int newFailure)
{
    allocationNum = 0;
    desiredFailure = newFailure;
}

/**
 * js_BigintTestingWhere
 *
 * Report the current allocation position.  This is really only useful when you
 * want to learn how many allocations a test run has.
 */
JS_PUBLIC_API(int)
js_BigintTestingWhere()
{
    return allocationNum;
}


/*
 * So here's what you do: Set up a fantastic test case that exercises the
 * elements of the code you wish.  Set the failure point at 0 and run the test,
 * then get the allocation position.  This number is the number of allocations
 * your test makes.  Now loop from 1 to that number, setting the failure point
 * at each loop count, and run the test over and over, causing failures at each
 * step.  Any memory failure *should* cause a Out-Of-Memory exception; if it
 * doesn't, then there's still an error here.
 */
#endif

typedef struct Bigint Bigint;

static Bigint *freelist[Kmax+1];

/*
 * Allocate a Bigint with 2^k words.
 * This is not threadsafe. The caller must use thread locks
 */
static Bigint *Balloc(int32 k)
{
    int32 x;
    Bigint *rv;
#ifndef Omit_Private_Memory
    uint32 len;
#endif

#ifdef ENABLE_OOM_TESTING
    if (++allocationNum == desiredFailure) {
        printf("Forced Failing Allocation number %d\n", allocationNum);
        return NULL;
    }
#endif

    if ((rv = freelist[k]) != NULL)
        freelist[k] = rv->next;
    if (rv == NULL) {
        x = 1 << k;
#ifdef Omit_Private_Memory
        rv = (Bigint *)MALLOC(sizeof(Bigint) + (x-1)*sizeof(ULong));
#else
        len = (sizeof(Bigint) + (x-1)*sizeof(ULong) + sizeof(double) - 1)
            /sizeof(double);
        if (pmem_next - private_mem + len <= PRIVATE_mem) {
            rv = (Bigint*)pmem_next;
            pmem_next += len;
            }
        else
            rv = (Bigint*)MALLOC(len*sizeof(double));
#endif
        if (!rv)
            return NULL;
        rv->k = k;
        rv->maxwds = x;
    }
    rv->sign = rv->wds = 0;
    return rv;
}

static void Bfree(Bigint *v)
{
    if (v) {
        v->next = freelist[v->k];
        freelist[v->k] = v;
    }
}

#define Bcopy(x,y) memcpy((char *)&x->sign, (char *)&y->sign, \
                          y->wds*sizeof(Long) + 2*sizeof(int32))

/* Return b*m + a.  Deallocate the old b.  Both a and m must be between 0 and
 * 65535 inclusive.  NOTE: old b is deallocated on memory failure.
 */
static Bigint *multadd(Bigint *b, int32 m, int32 a)
{
    int32 i, wds;
#ifdef ULLong
    ULong *x;
    ULLong carry, y;
#else
    ULong carry, *x, y;
    ULong xi, z;
#endif
    Bigint *b1;

#ifdef ENABLE_OOM_TESTING
    if (++allocationNum == desiredFailure) {
        /* Faux allocation, because I'm not getting all of the failure paths
         * without it.
         */
        printf("Forced Failing Allocation number %d\n", allocationNum);
        Bfree(b);
        return NULL;
    }
#endif

    wds = b->wds;
    x = b->x;
    i = 0;
    carry = a;
    do {
#ifdef ULLong
        y = *x * (ULLong)m + carry;
        carry = y >> 32;
        *x++ = (ULong)(y & 0xffffffffUL);
#else
        xi = *x;
        y = (xi & 0xffff) * m + carry;
        z = (xi >> 16) * m + (y >> 16);
        carry = z >> 16;
        *x++ = (z << 16) + (y & 0xffff);
#endif
    }
    while(++i < wds);
    if (carry) {
        if (wds >= b->maxwds) {
            b1 = Balloc(b->k+1);
            if (!b1) {
                Bfree(b);
                return NULL;
            }
            Bcopy(b1, b);
            Bfree(b);
            b = b1;
        }
        b->x[wds++] = (ULong)carry;
        b->wds = wds;
    }
    return b;
}

static Bigint *s2b(CONST char *s, int32 nd0, int32 nd, ULong y9)
{
    Bigint *b;
    int32 i, k;
    Long x, y;

    x = (nd + 8) / 9;
    for(k = 0, y = 1; x > y; y <<= 1, k++) ;
    b = Balloc(k);
    if (!b)
        return NULL;
    b->x[0] = y9;
    b->wds = 1;

    i = 9;
    if (9 < nd0) {
        s += 9;
        do {
            b = multadd(b, 10, *s++ - '0');
            if (!b)
                return NULL;
        } while(++i < nd0);
        s++;
    }
    else
        s += 10;
    for(; i < nd; i++) {
        b = multadd(b, 10, *s++ - '0');
        if (!b)
            return NULL;
    }
    return b;
}


/* Return the number (0 through 32) of most significant zero bits in x. */
static int32 hi0bits(register ULong x)
{
    register int32 k = 0;

    if (!(x & 0xffff0000)) {
        k = 16;
        x <<= 16;
    }
    if (!(x & 0xff000000)) {
        k += 8;
        x <<= 8;
    }
    if (!(x & 0xf0000000)) {
        k += 4;
        x <<= 4;
    }
    if (!(x & 0xc0000000)) {
        k += 2;
        x <<= 2;
    }
    if (!(x & 0x80000000)) {
        k++;
        if (!(x & 0x40000000))
            return 32;
    }
    return k;
}


/* Return the number (0 through 32) of least significant zero bits in y.
 * Also shift y to the right past these 0 through 32 zeros so that y's
 * least significant bit will be set unless y was originally zero. */
static int32 lo0bits(ULong *y)
{
    register int32 k;
    register ULong x = *y;

    if (x & 7) {
        if (x & 1)
            return 0;
        if (x & 2) {
            *y = x >> 1;
            return 1;
        }
        *y = x >> 2;
        return 2;
    }
    k = 0;
    if (!(x & 0xffff)) {
        k = 16;
        x >>= 16;
    }
    if (!(x & 0xff)) {
        k += 8;
        x >>= 8;
    }
    if (!(x & 0xf)) {
        k += 4;
        x >>= 4;
    }
    if (!(x & 0x3)) {
        k += 2;
        x >>= 2;
    }
    if (!(x & 1)) {
        k++;
        x >>= 1;
        if (!x & 1)
            return 32;
    }
    *y = x;
    return k;
}

/* Return a new Bigint with the given integer value, which must be nonnegative. */
static Bigint *i2b(int32 i)
{
    Bigint *b;

    b = Balloc(1);
    if (!b)
        return NULL;
    b->x[0] = i;
    b->wds = 1;
    return b;
}

/* Return a newly allocated product of a and b. */
static Bigint *mult(CONST Bigint *a, CONST Bigint *b)
{
    CONST Bigint *t;
    Bigint *c;
    int32 k, wa, wb, wc;
    ULong y;
    ULong *xc, *xc0, *xce;
    CONST ULong *x, *xa, *xae, *xb, *xbe;
#ifdef ULLong
    ULLong carry, z;
#else
    ULong carry, z;
    ULong z2;
#endif

    if (a->wds < b->wds) {
        t = a;
        a = b;
        b = t;
    }
    k = a->k;
    wa = a->wds;
    wb = b->wds;
    wc = wa + wb;
    if (wc > a->maxwds)
        k++;
    c = Balloc(k);
    if (!c)
        return NULL;
    for(xc = c->x, xce = xc + wc; xc < xce; xc++)
        *xc = 0;
    xa = a->x;
    xae = xa + wa;
    xb = b->x;
    xbe = xb + wb;
    xc0 = c->x;
#ifdef ULLong
    for(; xb < xbe; xc0++) {
        if ((y = *xb++) != 0) {
            x = xa;
            xc = xc0;
            carry = 0;
            do {
                z = *x++ * (ULLong)y + *xc + carry;
                carry = z >> 32;
                *xc++ = (ULong)(z & 0xffffffffUL);
                }
                while(x < xae);
            *xc = (ULong)carry;
            }
        }
#else
    for(; xb < xbe; xb++, xc0++) {
        if ((y = *xb & 0xffff) != 0) {
            x = xa;
            xc = xc0;
            carry = 0;
            do {
                z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
                carry = z >> 16;
                z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
                carry = z2 >> 16;
                Storeinc(xc, z2, z);
            }
            while(x < xae);
            *xc = carry;
        }
        if ((y = *xb >> 16) != 0) {
            x = xa;
            xc = xc0;
            carry = 0;
            z2 = *xc;
            do {
                z = (*x & 0xffff) * y + (*xc >> 16) + carry;
                carry = z >> 16;
                Storeinc(xc, z, z2);
                z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
                carry = z2 >> 16;
            }
            while(x < xae);
            *xc = z2;
        }
    }
#endif
    for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
    c->wds = wc;
    return c;
}

/*
 * 'p5s' points to a linked list of Bigints that are powers of 5.
 * This list grows on demand, and it can only grow: it won't change
 * in any other way.  So if we read 'p5s' or the 'next' field of
 * some Bigint on the list, and it is not NULL, we know it won't
 * change to NULL or some other value.  Only when the value of
 * 'p5s' or 'next' is NULL do we need to acquire the lock and add
 * a new Bigint to the list.
 */

static Bigint *p5s;

#ifdef JS_THREADSAFE
static PRLock *p5s_lock;
#endif

/* Return b * 5^k.  Deallocate the old b.  k must be nonnegative. */
/* NOTE: old b is deallocated on memory failure. */
static Bigint *pow5mult(Bigint *b, int32 k)
{
    Bigint *b1, *p5, *p51;
    int32 i;
    static CONST int32 p05[3] = { 5, 25, 125 };

    if ((i = k & 3) != 0) {
        b = multadd(b, p05[i-1], 0);
        if (!b)
            return NULL;
    }

    if (!(k >>= 2))
        return b;
    if (!(p5 = p5s)) {
#ifdef JS_THREADSAFE
        /*
         * We take great care to not call i2b() and Bfree()
         * while holding the lock.
         */
        Bigint *wasted_effort = NULL;
        p5 = i2b(625);
        if (!p5) {
            Bfree(b);
            return NULL;
        }
        /* lock and check again */
        PR_Lock(p5s_lock);
        if (!p5s) {
            /* first time */
            p5s = p5;
            p5->next = 0;
        } else {
            /* some other thread just beat us */
            wasted_effort = p5;
            p5 = p5s;
        }
        PR_Unlock(p5s_lock);
        if (wasted_effort) {
            Bfree(wasted_effort);
        }
#else
        /* first time */
        p5 = p5s = i2b(625);
        if (!p5) {
            Bfree(b);
            return NULL;
        }
        p5->next = 0;
#endif
    }
    for(;;) {
        if (k & 1) {
            b1 = mult(b, p5);
            Bfree(b);
            if (!b1)
                return NULL;
            b = b1;
        }
        if (!(k >>= 1))
            break;
        if (!(p51 = p5->next)) {
#ifdef JS_THREADSAFE
            Bigint *wasted_effort = NULL;
            p51 = mult(p5, p5);
            if (!p51) {
                Bfree(b);
                return NULL;
            }
            PR_Lock(p5s_lock);
            if (!p5->next) {
                p5->next = p51;
                p51->next = 0;
            } else {
                wasted_effort = p51;
                p51 = p5->next;
            }
            PR_Unlock(p5s_lock);
            if (wasted_effort) {
                Bfree(wasted_effort);
            }
#else
            p51 = mult(p5,p5);
            if (!p51) {
                Bfree(b);
                return NULL;
            }
            p51->next = 0;
            p5->next = p51;
#endif
        }
        p5 = p51;
    }
    return b;
}

/* Return b * 2^k.  Deallocate the old b.  k must be nonnegative.
 * NOTE: on memory failure, old b is deallocated. */
static Bigint *lshift(Bigint *b, int32 k)
{
    int32 i, k1, n, n1;
    Bigint *b1;
    ULong *x, *x1, *xe, z;

    n = k >> 5;
    k1 = b->k;
    n1 = n + b->wds + 1;
    for(i = b->maxwds; n1 > i; i <<= 1)
        k1++;
    b1 = Balloc(k1);
    if (!b1)
        goto done;
    x1 = b1->x;
    for(i = 0; i < n; i++)
        *x1++ = 0;
    x = b->x;
    xe = x + b->wds;
    if (k &= 0x1f) {
        k1 = 32 - k;
        z = 0;
        do {
            *x1++ = *x << k | z;
            z = *x++ >> k1;
        }
        while(x < xe);
        if ((*x1 = z) != 0)
            ++n1;
    }
    else do
        *x1++ = *x++;
         while(x < xe);
    b1->wds = n1 - 1;
done:
    Bfree(b);
    return b1;
}

/* Return -1, 0, or 1 depending on whether a<b, a==b, or a>b, respectively. */
static int32 cmp(Bigint *a, Bigint *b)
{
    ULong *xa, *xa0, *xb, *xb0;
    int32 i, j;

    i = a->wds;
    j = b->wds;
#ifdef DEBUG
    if (i > 1 && !a->x[i-1])
        Bug("cmp called with a->x[a->wds-1] == 0");
    if (j > 1 && !b->x[j-1])
        Bug("cmp called with b->x[b->wds-1] == 0");
#endif
    if (i -= j)
        return i;
    xa0 = a->x;
    xa = xa0 + j;
    xb0 = b->x;
    xb = xb0 + j;
    for(;;) {
        if (*--xa != *--xb)
            return *xa < *xb ? -1 : 1;
        if (xa <= xa0)
            break;
    }
    return 0;
}

static Bigint *diff(Bigint *a, Bigint *b)
{
    Bigint *c;
    int32 i, wa, wb;
    ULong *xa, *xae, *xb, *xbe, *xc;
#ifdef ULLong
    ULLong borrow, y;
#else
    ULong borrow, y;
    ULong z;
#endif

    i = cmp(a,b);
    if (!i) {
        c = Balloc(0);
        if (!c)
            return NULL;
        c->wds = 1;
        c->x[0] = 0;
        return c;
    }
    if (i < 0) {
        c = a;
        a = b;
        b = c;
        i = 1;
    }
    else
        i = 0;
    c = Balloc(a->k);
    if (!c)
        return NULL;
    c->sign = i;
    wa = a->wds;
    xa = a->x;
    xae = xa + wa;
    wb = b->wds;
    xb = b->x;
    xbe = xb + wb;
    xc = c->x;
    borrow = 0;
#ifdef ULLong
    do {
        y = (ULLong)*xa++ - *xb++ - borrow;
        borrow = y >> 32 & 1UL;
        *xc++ = (ULong)(y & 0xffffffffUL);
        }
        while(xb < xbe);
    while(xa < xae) {
        y = *xa++ - borrow;
        borrow = y >> 32 & 1UL;
        *xc++ = (ULong)(y & 0xffffffffUL);
        }
#else
    do {
        y = (*xa & 0xffff) - (*xb & 0xffff) - borrow;
        borrow = (y & 0x10000) >> 16;
        z = (*xa++ >> 16) - (*xb++ >> 16) - borrow;
        borrow = (z & 0x10000) >> 16;
        Storeinc(xc, z, y);
        }
        while(xb < xbe);
    while(xa < xae) {
        y = (*xa & 0xffff) - borrow;
        borrow = (y & 0x10000) >> 16;
        z = (*xa++ >> 16) - borrow;
        borrow = (z & 0x10000) >> 16;
        Storeinc(xc, z, y);
        }
#endif
    while(!*--xc)
        wa--;
    c->wds = wa;
    return c;
}

/* Return the absolute difference between x and the adjacent greater-magnitude double number (ignoring exponent overflows). */
static double ulp(double x)
{
    register Long L;
    double a = 0;

    L = (word0(x) & Exp_mask) - (P-1)*Exp_msk1;
#ifndef Sudden_Underflow
    if (L > 0) {
#endif
        set_word0(a, L);
        set_word1(a, 0);
#ifndef Sudden_Underflow
    }
    else {
        L = -L >> Exp_shift;
        if (L < Exp_shift) {
            set_word0(a, 0x80000 >> L);
            set_word1(a, 0);
        }
        else {
            set_word0(a, 0);
            L -= Exp_shift;
            set_word1(a, L >= 31 ? 1 : 1 << (31 - L));
        }
    }
#endif
    return a;
}


static double b2d(Bigint *a, int32 *e)
{
    ULong *xa, *xa0, w, y, z;
    int32 k;
    double d = 0;
#define d0 word0(d)
#define d1 word1(d)
#define set_d0(x) set_word0(d, x)
#define set_d1(x) set_word1(d, x)

    xa0 = a->x;
    xa = xa0 + a->wds;
    y = *--xa;
#ifdef DEBUG
    if (!y) Bug("zero y in b2d");
#endif
    k = hi0bits(y);
    *e = 32 - k;
    if (k < Ebits) {
        set_d0(Exp_1 | y >> (Ebits - k));
        w = xa > xa0 ? *--xa : 0;
        set_d1(y << (32-Ebits + k) | w >> (Ebits - k));
        goto ret_d;
    }
    z = xa > xa0 ? *--xa : 0;
    if (k -= Ebits) {
        set_d0(Exp_1 | y << k | z >> (32 - k));
        y = xa > xa0 ? *--xa : 0;
        set_d1(z << k | y >> (32 - k));
    }
    else {
        set_d0(Exp_1 | y);
        set_d1(z);
    }
  ret_d:
#undef d0
#undef d1
#undef set_d0
#undef set_d1
    return d;
}


/* Convert d into the form b*2^e, where b is an odd integer.  b is the returned
 * Bigint and e is the returned binary exponent.  Return the number of significant
 * bits in b in bits.  d must be finite and nonzero. */
static Bigint *d2b(double d, int32 *e, int32 *bits)
{
    Bigint *b;
    int32 de, i, k;
    ULong *x, y, z;
#define d0 word0(d)
#define d1 word1(d)
#define set_d0(x) set_word0(d, x)
#define set_d1(x) set_word1(d, x)

    b = Balloc(1);
    if (!b)
        return NULL;
    x = b->x;

    z = d0 & Frac_mask;
    set_d0(d0 & 0x7fffffff);  /* clear sign bit, which we ignore */
#ifdef Sudden_Underflow
    de = (int32)(d0 >> Exp_shift);
    z |= Exp_msk11;
#else
    if ((de = (int32)(d0 >> Exp_shift)) != 0)
        z |= Exp_msk1;
#endif
    if ((y = d1) != 0) {
        if ((k = lo0bits(&y)) != 0) {
            x[0] = y | z << (32 - k);
            z >>= k;
        }
        else
            x[0] = y;
        i = b->wds = (x[1] = z) ? 2 : 1;
    }
    else {
        JS_ASSERT(z);
        k = lo0bits(&z);
        x[0] = z;
        i = b->wds = 1;
        k += 32;
    }
#ifndef Sudden_Underflow
    if (de) {
#endif
        *e = de - Bias - (P-1) + k;
        *bits = P - k;
#ifndef Sudden_Underflow
    }
    else {
        *e = de - Bias - (P-1) + 1 + k;
        *bits = 32*i - hi0bits(x[i-1]);
    }
#endif
    return b;
}
#undef d0
#undef d1
#undef set_d0
#undef set_d1


static double ratio(Bigint *a, Bigint *b)
{
    double da, db;
    int32 k, ka, kb;

    da = b2d(a, &ka);
    db = b2d(b, &kb);
    k = ka - kb + 32*(a->wds - b->wds);
    if (k > 0)
        set_word0(da, word0(da) + k*Exp_msk1);
    else {
        k = -k;
        set_word0(db, word0(db) + k*Exp_msk1);
    }
    return da / db;
}

static CONST double
tens[] = {
    1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
    1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
    1e20, 1e21, 1e22
};

static CONST double bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };
static CONST double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128,
#ifdef Avoid_Underflow
        9007199254740992.e-256
#else
        1e-256
#endif
        };
/* The factor of 2^53 in tinytens[4] helps us avoid setting the underflow */
/* flag unnecessarily.  It leads to a song and dance at the end of strtod. */
#define Scale_Bit 0x10
#define n_bigtens 5


#ifdef INFNAN_CHECK

#ifndef NAN_WORD0
#define NAN_WORD0 0x7ff80000
#endif

#ifndef NAN_WORD1
#define NAN_WORD1 0
#endif

static int match(CONST char **sp, char *t)
{
    int c, d;
    CONST char *s = *sp;

    while(d = *t++) {
        if ((c = *++s) >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        if (c != d)
            return 0;
        }
    *sp = s + 1;
    return 1;
    }
#endif /* INFNAN_CHECK */


#ifdef JS_THREADSAFE
static JSBool initialized = JS_FALSE;

/* hacked replica of nspr _PR_InitDtoa */
static void InitDtoa(void)
{
    freelist_lock = PR_NewLock();
        p5s_lock = PR_NewLock();
    initialized = JS_TRUE;
}
#endif

void js_FinishDtoa(void)
{
    int count;
    Bigint *temp;

#ifdef JS_THREADSAFE
    if (initialized == JS_TRUE) {
        PR_DestroyLock(freelist_lock);
        PR_DestroyLock(p5s_lock);
        initialized = JS_FALSE;
    }
#endif

    /* clear down the freelist array and p5s */

    /* static Bigint *freelist[Kmax+1]; */
    for (count = 0; count <= Kmax; count++) {
        Bigint **listp = &freelist[count];
        while ((temp = *listp) != NULL) {
            *listp = temp->next;
            free(temp);
        }
        freelist[count] = NULL;
    }

    /* static Bigint *p5s; */
    while (p5s) {
        temp = p5s;
        p5s = p5s->next;
        free(temp);
    }
}

/* nspr2 watcom bug ifdef omitted */

JS_FRIEND_API(double)
JS_strtod(CONST char *s00, char **se, int *err)
{
    int32 scale;
    int32 bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign,
        e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
    CONST char *s, *s0, *s1;
    double aadj, aadj1, adj, rv, rv0;
    Long L;
    ULong y, z;
    Bigint *bb, *bb1, *bd, *bd0, *bs, *delta;

    *err = 0;

    bb = bd = bs = delta = NULL;
    sign = nz0 = nz = 0;
    rv = 0.;

    /* Locking for Balloc's shared buffers that will be used in this block */
    ACQUIRE_DTOA_LOCK();

    for(s = s00;;s++) switch(*s) {
    case '-':
        sign = 1;
        /* no break */
    case '+':
        if (*++s)
            goto break2;
        /* no break */
    case 0:
        s = s00;
        goto ret;
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
    case ' ':
        continue;
    de