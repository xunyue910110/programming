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
 * JS date methods.
 */

/*
 * "For example, OS/360 devotes 26 bytes of the permanently
 *  resident date-turnover routine to the proper handling of
 *  December 31 on leap years (when it is Day 366).  That
 *  might have been left to the operator."
 *
 * Frederick Brooks, 'The Second-System Effect'.
 */

#include "jsstddef.h"
#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsprf.h"
#include "prmjtime.h"
#include "jsutil.h" /* Added by JSIFY */
#include "jsapi.h"
#include "jsconfig.h"
#include "jscntxt.h"
#include "jsdate.h"
#include "jsinterp.h"
#include "jsnum.h"
#include "jsobj.h"
#include "jsstr.h"

/*
 * The JS 'Date' object is patterned after the Java 'Date' object.
 * Here is an script:
 *
 *    today = new Date();
 *
 *    print(today.toLocaleString());
 *
 *    weekDay = today.getDay();
 *
 *
 * These Java (and ECMA-262) methods are supported:
 *
 *     UTC
 *     getDate (getUTCDate)
 *     getDay (getUTCDay)
 *     getHours (getUTCHours)
 *     getMinutes (getUTCMinutes)
 *     getMonth (getUTCMonth)
 *     getSeconds (getUTCSeconds)
 *     getMilliseconds (getUTCMilliseconds)
 *     getTime
 *     getTimezoneOffset
 *     getYear
 *     getFullYear (getUTCFullYear)
 *     parse
 *     setDate (setUTCDate)
 *     setHours (setUTCHours)
 *     setMinutes (setUTCMinutes)
 *     setMonth (setUTCMonth)
 *     setSeconds (setUTCSeconds)
 *     setMilliseconds (setUTCMilliseconds)
 *     setTime
 *     setYear (setFullYear, setUTCFullYear)
 *     toGMTString (toUTCString)
 *     toLocaleString
 *     toString
 *
 *
 * These Java methods are not supported
 *
 *     setDay
 *     before
 *     after
 *     equals
 *     hashCode
 */

/*
 * 11/97 - jsdate.c has been rewritten to conform to the ECMA-262 language
 * definition and reduce dependence on NSPR.  NSPR is used to get the current
 * time in milliseconds, the time zone offset, and the daylight savings time
 * offset for a given time.  NSPR is also used for Date.toLocaleString(), for
 * locale-specific formatting, and to get a string representing the timezone.
 * (Which turns out to be platform-dependent.)
 *
 * To do:
 * (I did some performance tests by timing how long it took to run what
 *  I had of the js ECMA conformance tests.)
 *
 * - look at saving results across multiple calls to supporting
 * functions; the toString functions compute some of the same values
 * multiple times.  Although - I took a quick stab at this, and I lost
 * rather than gained.  (Fractionally.)  Hard to tell what compilers/processors
 * are doing these days.
 *
 * - look at tweaking function return types to return double instead
 * of int; this seems to make things run slightly faster sometimes.
 * (though it could be architecture-dependent.)  It'd be good to see
 * how this does on win32.  (Tried it on irix.)  Types could use a
 * general going-over.
 */

/*
 * Supporting functions - ECMA 15.9.1.*
 */

#define HalfTimeDomain  8.64e15
#define HoursPerDay     24.0
#define MinutesPerDay   (HoursPerDay * MinutesPerHour)
#define MinutesPerHour  60.0
#define SecondsPerDay   (MinutesPerDay * SecondsPerMinute)
#define SecondsPerHour  (MinutesPerHour * SecondsPerMinute)
#define SecondsPerMinute 60.0

#if defined(XP_WIN) || defined(XP_OS2)
/* Work around msvc double optimization bug by making these runtime values; if
 * they're available at compile time, msvc optimizes division by them by
 * computing the reciprocal and multiplying instead of dividing - this loses
 * when the reciprocal isn't representable in a double.
 */
static jsdouble msPerSecond = 1000.0;
static jsdouble msPerDay = SecondsPerDay * 1000.0;
static jsdouble msPerHour = SecondsPerHour * 1000.0;
static jsdouble msPerMinute = SecondsPerMinute * 1000.0;
#else
#define msPerDay        (SecondsPerDay * msPerSecond)
#define msPerHour       (SecondsPerHour * msPerSecond)
#define msPerMinute     (SecondsPerMinute * msPerSecond)
#define msPerSecond     1000.0
#endif

#define Day(t)          floor((t) / msPerDay)

static jsdouble
TimeWithinDay(jsdouble t)
{
    jsdouble result;
    result = fmod(t, msPerDay);
    if (result < 0)
        result += msPerDay;
    return result;
}

#define DaysInYear(y)   ((y) % 4 == 0 && ((y) % 100 || ((y) % 400 == 0))  \
                         ? 366 : 365)

/* math here has to be f.p, because we need
 *  floor((1968 - 1969) / 4) == -1
 */
#define DayFromYear(y)  (365 * ((y)-1970) + floor(((y)-1969)/4.0)            \
                         - floor(((y)-1901)/100.0) + floor(((y)-1601)/400.0))
#define TimeFromYear(y) (DayFromYear(y) * msPerDay)

static jsint
YearFromTime(jsdouble t)
{
    jsint y = (jsint) floor(t /(msPerDay*365.2425)) + 1970;
    jsdouble t2 = (jsdouble) TimeFromYear(y);

    if (t2 > t) {
        y--;
    } else {
        if (t2 + msPerDay * DaysInYear(y) <= t)
            y++;
    }
    return y;
}

#define InLeapYear(t)   (JSBool) (DaysInYear(YearFromTime(t)) == 366)

#define DayWithinYear(t, year) ((intN) (Day(t) - DayFromYear(year)))

/*
 * The following array contains the day of year for the first day of
 * each month, where index 0 is January, and day 0 is January 1.
 */
static jsdouble firstDayOfMonth[2][12] = {
    {0.0, 31.0, 59.0, 90.0, 120.0, 151.0, 181.0, 212.0, 243.0, 273.0, 304.0, 334.0},
    {0.0, 31.0, 60.0, 91.0, 121.0, 152.0, 182.0, 213.0, 244.0, 274.0, 305.0, 335.0}
};

#define DayFromMonth(m, leap) firstDayOfMonth[leap][(intN)m];

static intN
MonthFromTime(jsdouble t)
{
    intN d, step;
    jsint year = YearFromTime(t);
    d = DayWithinYear(t, year);

    if (d < (step = 31))
        return 0;
    step += (InLeapYear(t) ? 29 : 28);
    if (d < step)
        return 1;
    if (d < (step += 31))
        return 2;
    if (d < (step += 30))
        return 3;
    if (d < (step += 31))
        return 4;
    if (d < (step += 30))
        return 5;
    if (d < (step += 31))
        return 6;
    if (d < (step += 31))
        return 7;
    if (d < (step += 30))
        return 8;
    if (d < (step += 31))
        return 9;
    if (d < (step += 30))
        return 10;
    return 11;
}

static intN
DateFromTime(jsdouble t)
{
    intN d, step, next;
    jsint year = YearFromTime(t);
    d = DayWithinYear(t, year);

    if (d <= (next = 30))
        return d + 1;
    step = next;
    next += (InLeapYear(t) ? 29 : 28);
    if (d <= next)
        return d - step;
    step = next;
    if (d <= (next += 31))
        return d - step;
    step = next;
    if (d <= (next += 30))
        return d - step;
    step = next;
    if (d <= (next += 31))
        return d - step;
    step = next;
    if (d <= (next += 30))
        return d - step;
    step = next;
    if (d <= (next += 31))
        return d - step;
    step = next;
    if (d <= (next += 31))
        return d - step;
    step = next;
    if (d <= (next += 30))
        return d - step;
    step = next;
    if (d <= (next += 31))
        return d - step;
    step = next;
    if (d <= (next += 30))
        return d - step;
    step = next;
    return d - step;
}

static intN
WeekDay(jsdouble t)
{
    jsint result;
    result = (jsint) Day(t) + 4;
    result = result % 7;
    if (result < 0)
        result += 7;
    return (intN) result;
}

#define MakeTime(hour, min, sec, ms) \
((((hour) * MinutesPerHour + (min)) * SecondsPerMinute + (sec)) * msPerSecond + (ms))

static jsdouble
MakeDay(jsdouble year, jsdouble month, jsdouble date)
{
    JSBool leap;
    jsdouble yearday;
    jsdouble monthday;

    year += floor(month / 12);

    month = fmod(month, 12.0);
    if (month < 0)
        month += 12;

    leap = (DaysInYear((jsint) year) == 366);

    yearday = floor(TimeFromYear(year) / msPerDay);
    monthday = DayFromMonth(month, leap);

    return yearday + monthday + date - 1;
}

#define MakeDate(day, time) ((day) * msPerDay + (time))

/* 
 * Years and leap years on which Jan 1 is a Sunday, Monday, etc. 
 *
 * yearStartingWith[0][i] is an example non-leap year where
 * Jan 1 appears on Sunday (i == 0), Monday (i == 1), etc.
 *
 * yearStartingWith[1][i] is an example leap year where
 * Jan 1 appears on Sunday (i == 0), Monday (i == 1), etc.
 */
static jsint yearStartingWith[2][7] = {
    {1978, 1973, 1974, 1975, 1981, 1971, 1977},
    {1984, 1996, 1980, 1992, 1976, 1988, 1972}
};

/*
 * Find a year for which any given date will fall on the same weekday.
 *
 * This function should be used with caution when used other than
 * for determining DST; it hasn't been proven not to produce an
 * incorrect year for times near year boundaries.
 */
static jsint
EquivalentYearForDST(jsint year) {
    jsint day;
    JSBool isLeapYear;

    day = (jsint) DayFromYear(year) + 4;
    day = day % 7;
    if (day < 0)
	day += 7;

    isLeapYear = (DaysInYear(year) == 366);

    return yearStartingWith[isLeapYear][day];
}

/* LocalTZA gets set by js_InitDateClass() */
static jsdouble LocalTZA;

static jsdouble
DaylightSavingTA(jsdouble t)
{
    volatile int64 PR_t;
    int64 ms2us;
    int64 offset;
    jsdouble result;

    /* abort if NaN */
    if (JSDOUBLE_IS_NaN(t))
        return t;

    /*
     * If earlier than 1970 or after 2038, potentially beyond the ken of
     * many OSes, map it to an equivalent year before asking.
     */
    if (t < 0.0 || t > 2145916800000.0) {
        jsint year;
        jsdouble day;

        year = EquivalentYearForDST(YearFromTime(t));
        day = MakeDay(year, MonthFromTime(t), DateFromTime(t));
        t = MakeDate(day, TimeWithinDay(t));
    }

    /* put our t in an LL, and map it to usec for prtime */
    JSLL_D2L(PR_t, t);
    JSLL_I2L(ms2us, PRMJ_USEC_PER_MSEC);
    JSLL_MUL(PR_t, PR_t, ms2us);

    offset = PRMJ_DSTOffset(PR_t);

    JSLL_DIV(offset, offset, ms2us);
    JSLL_L2D(result, offset);
    return result;
}


#define AdjustTime(t)   fmod(LocalTZA + DaylightSavingTA(t), msPerDay)

#define LocalTime(t)    ((t) + AdjustTime(t))

static jsdouble
UTC(jsdouble t)
{
    return t - AdjustTime(t - LocalTZA);
}

static intN
HourFromTime(jsdouble t)
{
    intN result = (intN) fmod(floor(t/msPerHour), HoursPerDay);
    if (result < 0)
        result += (intN)HoursPerDay;
    return result;
}

static intN
MinFromTime(jsdouble t)
{
    intN result = (intN) fmod(floor(t / msPerMinute), MinutesPerHour);
    if (result < 0)
        result += (intN)MinutesPerHour;
    return result;
}

static intN
SecFromTime(jsdouble t)
{
    intN result = (intN) fmod(floor(t / msPerSecond), SecondsPerMinute);
    if (result < 0)
        result += (intN)SecondsPerMinute;
    return result;
}

static intN
msFromTime(jsdouble t)
{
    intN result = (intN) fmod(t, msPerSecond);
    if (result < 0)
        result += (intN)msPerSecond;
    return result;
}

#define TIMECLIP(d) ((JSDOUBLE_IS_FINITE(d) \
                      && !((d < 0 ? -d : d) > HalfTimeDomain)) \
                     ? js_DoubleToInteger(d + (+0.)) : *cx->runtime->jsNaN)

/**
 * end of ECMA 'support' functions
 */

/*
 * Other Support routines and definitions
 */

static JSClass date_class = {
    js_Date_str,
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* for use by date_parse */

static const char* wtb[] = {
    "am", "pm",
    "monday", "tuesday", "wednesday", "thursday", "friday",
    "saturday", "sunday",
    "january", "february", "march", "april", "may", "june",
    "july", "august", "september", "october", "november", "december",
    "gmt", "ut", "utc",
    "est", "edt",
    "cst", "cdt",
    "mst", "mdt",
    "pst", "pdt"
    /* time zone table needs to be expanded */
};

static int ttb[] = {
    -1, -2, 0, 0, 0, 0, 0, 0, 0,       /* AM/PM */
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    10000 + 0, 10000 + 0, 10000 + 0,   /* GMT/UT/UTC */
    10000 + 5 * 60, 10000 + 4 * 60,    /* EST/EDT */
    10000 + 6 * 60, 10000 + 5 * 60,    /* CST/CDT */
    10000 + 7 * 60, 10000 + 6 * 60,    /* MST/MDT */
    10000 + 8 * 60, 10000 + 7 * 60     /* PST/PDT */
};

/* helper for date_parse */
static JSBool
date_regionMatches(const char* s1, int s1off, const jschar* s2, int s2off,
                   int count, int ignoreCase)
{
    JSBool result = JS_FALSE;
    /* return true if matches, otherwise, false */

    while (count > 0 && s1[s1off] && s2[s2off]) {
        if (ignoreCase) {
            if (JS_TOLOWER((jschar)s1[s1off]) != JS_TOLOWER(s2[s2off])) {
                break;
            }
        } else {
            if ((jschar)s1[s1off] != s2[s2off]) {
                break;
            }
        }
        s1off++;
        s2off++;
        count--;
    }

    if (count == 0) {
        result = JS_TRUE;
    }

    return result;
}

/* find UTC time from given date... no 1900 correction! */
static jsdouble
date_msecFromDate(jsdouble year, jsdouble mon, jsdouble mday, jsdouble hour,
                  jsdouble min, jsdouble sec, jsdouble msec)
{
    jsdouble day;
    jsdouble msec_time;
    jsdouble result;

    day = MakeDay(year, mon, mday);
    msec_time = MakeTime(hour, min, sec, msec);
    result = MakeDate(day, msec_time);
    return result;
}

/*
 * See ECMA 15.9.4.[3-10];
 */
/* XXX this function must be above date_parseString to avoid a
   horrid bug in the Win16 1.52 compiler */
#define MAXARGS        7
static JSBool
date_UTC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsdouble array[MAXARGS];
    uintN loop;
    jsdouble d;

    for (loop = 0; loop < MAXARGS; loop++) {
        if (loop < argc) {
            if (!js_ValueToNumber(cx, argv[loop], &d))
                return JS_FALSE;
            /* return NaN if any arg is NaN */
            if (!JSDOUBLE_IS_FINITE(d)) {
                return js_NewNumberValue(cx, d, rval);
            }
            array[loop] = floor(d);
        } else {
            array[loop] = 0;
        }
    }

    /* adjust 2-digit years into the 20th century */
    if (array[0] >= 0 && array[0] <= 99)
        array[0] += 1900;

    /* if we got a 0 for 'date' (which is out of range)
     * pretend it's a 1.  (So Date.UTC(1972, 5) works) */
    if (array[2] < 1)
        array[2] = 1;

    d = date_msecFromDate(array[0], array[1], array[2],
                              array[3], array[4], array[5], array[6]);
    d = TIMECLIP(d);

    return js_NewNumberValue(cx, d, rval);
}

static JSBool
date_parseString(JSString *str, jsdouble *result)
{
    jsdouble msec;

    const jschar *s = JSSTRING_CHARS(str);
    size_t limit = JSSTRING_LENGTH(str);
    size_t i = 0;
    int year = -1;
    int mon = -1;
    int mday = -1;
    int hour = -1;
    int min = -1;
    int sec = -1;
    int c = -1;
    int n = -1;
    jsdouble tzoffset = -1;  /* was an int, overflowed on win16!!! */
    int prevc = 0;
    JSBool seenplusminus = JS_FALSE;
    int temp;
    JSBool seenmonthname = JS_FALSE;

    if (limit == 0)
        goto syntax;
    while (i < limit) {
        c = s[i];
        i++;
        if (c <= ' ' || c == ',' || c == '-') {
            if (c == '-' && '0' <= s[i] && s[i] <= '9') {
              prevc = c;
            }
            continue;
        }
        if (c == '(') { /* comments) */
            int depth = 1;
            while (i < limit) {
                c = s[i];
                i++;
                if (c == '(') depth++;
                else if (c == ')')
                    if (--depth <= 0)
                        break;
            }
            continue;
        }
        if ('0' <= c && c <= '9') {
            n = c - '0';
            while (i < limit && '0' <= (c = s[i]) && c <= '9') {
                n = n * 10 + c - '0';
                i++;
            }

            /* allow TZA before the year, so
             * 'Wed Nov 05 21:49:11 GMT-0800 1997'
             * works */

            /* uses of seenplusminus allow : in TZA, so Java
             * no-timezone style of GMT+4:30 works
             */

            if ((prevc == '+' || prevc == '-')/*  && year>=0 */) {
                /* make ':' case below change tzoffset */
                seenplusminus = JS_TRUE;

                /* offset */
                if (n < 24)
                    n = n * 60; /* EG. "GMT-3" */
                else
                    n = n % 100 + n / 100 * 60; /* eg "GMT-0430" */
                if (prevc == '+')       /* plus means east of GMT */
                    n = -n;
                if (tzoffset != 0 && tzoffset != -1)
                    goto syntax;
                tzoffset = n;
            } else if (prevc == '/' && mon >= 0 && mday >= 0 && year < 0) {
                if (c <= ' ' || c == ',' || c == '/' || i >= limit)
                    year = n;
                else
                    goto syntax;
            } else if (c == ':') {
                if (hour < 0)
                    hour = /*byte*/ n;
                else if (min < 0)
                    min = /*byte*/ n;
                else
                    goto syntax;
            } else if (c == '/') {
                /* until it is determined that mon is the actual
                   month, keep it as 1-based rather than 0-based */
                if (mon < 0)
                    mon = /*byte*/ n;
                else if (mday < 0)
                    mday = /*byte*/ n;
                else
                    goto syntax;
            } else if (i < limit && c != ',' && c > ' ' && c != '-' && c != '(') {
                goto syntax;
            } else if (seenplusminus && n < 60) {  /* handle GMT-3:30 */
                if (tzoffset < 0)
                    tzoffset -= n;
                else
                    tzoffset += n;
            } else if (hour >= 0 && min < 0) {
                min = /*byte*/ n;
            } else if (prevc == ':' && min >= 0 && sec < 0) {
                sec = /*byte*/ n;
            } else if (mon < 0) {
                mon = /*byte*/n;
            } else if (mon >= 0 && mday < 0) {
                mday = /*byte*/ n;
            } else if (mon >= 0 && mday >= 0 && year < 0) {
                year = n;
            } else {
                goto syntax;
            }
            prevc = 0;
        } else if (c == '/' || c == ':' || c == '+' || c == '-') {
            prevc = c;
        } else {
            size_t st = i - 1;
            int k;
            while (i < limit) {
                c = s[i];
                if (!(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')))
                    break;
                i++;
            }
            if (i <= st + 1)
                goto syntax;
            for (k = (sizeof(wtb)/sizeof(char*)); --k >= 0;)
                if (date_regionMatches(wtb[k], 0, s, st, i-st, 1)) {
                    int action = ttb[k];
                    if (action != 0) {
                        if (action < 0) {
                            /*
                             * AM/PM. Count 12:30 AM as 00:30, 12:30 PM as
                             * 12:30, instead of blindly adding 12 if PM.
                             */
                            JS_ASSERT(action == -1 || action == -2);
                            if (hour > 12 || hour < 0) {
                                goto syntax;
                            } else {
                                if (action == -1 && hour == 12) { /* am */
                                    hour = 0;
                                } else if (action == -2 && hour != 12) { /* pm */
                                    hour += 12;
                                }
                            }
                        } else if (action <= 13) { /* month! */
                            /* Adjust mon to be 1-based until the final values
                               for mon, mday and year are adjusted below */
                            if (seenmonthname) {
                                goto syntax;
                            }
                            seenmonthname = JS_TRUE;
                            temp = /*byte*/ (action - 2) + 1;

                            if (mon < 0) {
                                mon = temp;
                            } else if (mday < 0) {
                                mday = mon;
                                mon = temp;
                            } else if (year < 0) {
                                year = mon;
                                mon = temp;
                            } else {
                                goto syntax;
                            }
                        } else {
                            tzoffset = action - 10000;
                        }
                    }
                    break;
                }
            if (k < 0)
                goto syntax;
            prevc = 0;
        }
    }
    if (year < 0 || mon < 0 || mday < 0)
        goto syntax;
    /*
      Case 1. The input string contains an English month name.
              The form of the string can be month f l, or f month l, or
              f l month which each evaluate to the same date. 
              If f and l are both greater than or equal to 70, or
              both less than 70, the date is invalid.
              The year is taken to be the greater of the values f, l.
              If the year is greater than or equal to 70 and less than 100,
              it is considered to be the number of years after 1900.
      Case 2. The input string is of the form "f/m/l" where f, m and l are
              integers, e.g. 7/16/45.
              Adjust the mon, mday and year values to achieve 100% MSIE 
              compatibility.
              a. If 0 <= f < 70, f/m/l is interpreted as month/day/year.
                 i.  If year < 100, it is the number of years after 1900
                 ii. If year >= 100, it is the number of years after 0.
              b. If 70 <= f < 100
                 i.  If m < 70, f/m/l is interpreted as
                     year/month/day where year is the number of years after
                     1900.
                 ii. If m >= 70, the date is invalid.
              c. If f >= 100
                 i.  If m < 70, f/m/l is interpreted as
                     year/month/day where year is the number of years after 0.
                 ii. If m >= 70, the date is invalid.
    */
    if (seenmonthname) {
        if ((mday >= 70 && year >= 70) || (mday < 70 && year < 70)) {
            goto syntax;
        }
        if (mday > year) {
            temp = year;
            year = mday;
            mday = temp;
        }
        if (year >= 70 && year < 100) {
            year += 1900;
        }
    } else if (mon < 70) { /* (a) month/day/year */
        if (year < 100) {
            year += 1900;
        }
    } else if (mon < 100) { /* (b) year/month/day */
        if (mday < 70) { 
            temp = year;
            year = mon + 1900;
            mon = mday;
            mday = temp;
        } else {
            goto syntax;
        }
    } else { /* (c) year/month/day */
        if (mday < 70) {
            temp = year;
            year = mon;
            mon = mday;
            mday = temp;
        } else {
            goto syntax;
        }
    }
    mon -= 1; /* convert month to 0-based */
    if (sec < 0)
        sec = 0;
    if (min < 0)
        min = 0;
    if (hour < 0)
        hour = 0;
    if (tzoffset == -1) { /* no time zone specified, have to use local */
        jsdouble msec_time;
        msec_time = date_msecFromDate(year, mon, mday, hour, min, sec, 0);

        *result = UTC(msec_time);
        return JS_TRUE;
    }

    msec = date_msecFromDate(year, mon, mday, hour, min, sec, 0);
    msec += tzoffset * msPerMinute;
    *result = msec;
    return JS_TRUE;

syntax:
    /* syntax error */
    *result = 0;
    return JS_FALSE;
}

static JSBool
date_parse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
    jsdouble result;

    str = js_ValueToString(cx, argv[0]);
    if (!str)
        return JS_FALSE;
    if (!date_parseString(str, &result)) {
        *rval = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
        return JS_TRUE;
    }

    result = TIMECLIP(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_now(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int64 us, ms, us2ms;
    jsdouble msec_time;

    us = PRMJ_Now();
    JSLL_UI2L(us2ms, PRMJ_USEC_PER_MSEC);
    JSLL_DIV(ms, us, us2ms);
    JSLL_L2D(msec_time, ms);

    return js_NewDoubleValue(cx, msec_time, rval);
}

/*
 * Check that obj is an object of class Date, and get the date value.
 * Return NULL on failure.
 */
static jsdouble *
date_getProlog(JSContext *cx, JSObject *obj, jsval *argv)
{
    if (!JS_InstanceOf(cx, obj, &date_class, argv))
        return NULL;
    return JSVAL_TO_DOUBLE(OBJ_GET_SLOT(cx, obj, JSSLOT_PRIVATE));
}

/*
 * See ECMA 15.9.5.4 thru 15.9.5.23
 */
static JSBool
date_getTime(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;

    return js_NewNumberValue(cx, *date, rval);
}

static JSBool
date_getYear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsdouble *date;
    jsdouble result;
    JSVersion version;

    date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;

    result = *date;
    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = YearFromTime(LocalTime(result));

    /*
     * During the great date rewrite of 1.3, we tried to track the evolving ECMA
     * standard, which then had a definition of getYear which always subtracted
     * 1900.  Which we implemented, not realizing that it was incompatible with
     * the old behavior...  now, rather than thrash the behavior yet again,
     * we've decided to leave it with the - 1900 behavior and point people to
     * the getFullYear method.  But we try to protect existing scripts that
     * have specified a version...
     */
    version = cx->version & JSVERSION_MASK;
    if (version == JSVERSION_1_0 ||
        version == JSVERSION_1_1 ||
        version == JSVERSION_1_2)
    {
        if (result >= 1900 && result < 2000)
            result -= 1900;
    } else {
        result -= 1900;
    }
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getFullYear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                 jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = YearFromTime(LocalTime(result));
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getUTCFullYear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                    jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = YearFromTime(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getMonth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = MonthFromTime(LocalTime(result));
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getUTCMonth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                 jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = MonthFromTime(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getDate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = LocalTime(result);
    result = DateFromTime(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getUTCDate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = DateFromTime(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getDay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = LocalTime(result);
    result = WeekDay(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getUTCDay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
               jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = WeekDay(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getHours(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = HourFromTime(LocalTime(result));
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getUTCHours(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                 jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    result = *date;

    if (!JSDOUBLE_IS_FINITE(result))
        return js_NewNumberValue(cx, result, rval);

    result = HourFromTime(result);
    return js_NewNumberValue(cx, result, rval);
}

static JSBool
date_getMinutes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    jsdouble result;
    jsdouble *date = date_getProlog(cx, obj, argv);
    if (!date)
        return JS_FALSE;
    re