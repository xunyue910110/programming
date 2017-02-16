/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sw=4 et tw=80:
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
 * JS string type implementation.
 *
 * In order to avoid unnecessary js_LockGCThing/js_UnlockGCThing calls, these
 * native methods store strings (possibly newborn) converted from their 'this'
 * parameter and arguments on the stack: 'this' conversions at argv[-1], arg
 * conversions at their index (argv[0], argv[1]).  This is a legitimate method
 * of rooting things that might lose their newborn root due to subsequent GC
 * allocations in the same native method.
 */
#include "jsstddef.h"
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsutil.h" /* Added by JSIFY */
#include "jshash.h" /* Added by JSIFY */
#include "jsprf.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jsbool.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsgc.h"
#include "jsinterp.h"
#include "jslock.h"
#include "jsnum.h"
#include "jsobj.h"
#include "jsopcode.h"
#include "jsregexp.h"
#include "jsstr.h"

#if JS_HAS_REPLACE_LAMBDA
#include "jsinterp.h"
#endif

#define JSSTRDEP_RECURSION_LIMIT        100

size_t
js_MinimizeDependentStrings(JSString *str, int level, JSString **basep)
{
    JSString *base;
    size_t start, length;

    JS_ASSERT(JSSTRING_IS_DEPENDENT(str));
    base = JSSTRDEP_BASE(str);
    start = JSSTRDEP_START(str);
    if (JSSTRING_IS_DEPENDENT(base)) {
        if (level < JSSTRDEP_RECURSION_LIMIT) {
            start += js_MinimizeDependentStrings(base, level + 1, &base);
        } else {
            do {
                start += JSSTRDEP_START(base);
                base = JSSTRDEP_BASE(base);
            } while (JSSTRING_IS_DEPENDENT(base));
        }
        if (start == 0) {
            JS_ASSERT(JSSTRING_IS_PREFIX(str));
            JSPREFIX_SET_BASE(str, base);
        } else if (start <= JSSTRDEP_START_MASK) {
            length = JSSTRDEP_LENGTH(str);
            JSSTRDEP_SET_START_AND_LENGTH(str, start, length);
            JSSTRDEP_SET_BASE(str, base);
        }
    }
    *basep = base;
    return start;
}

jschar *
js_GetDependentStringChars(JSString *str)
{
    size_t start;
    JSString *base;

    start = js_MinimizeDependentStrings(str, 0, &base);
    JS_ASSERT(!JSSTRING_IS_DEPENDENT(base));
    JS_ASSERT(start < base->length);
    return base->chars + start;
}

jschar *
js_GetStringChars(JSString *str)
{
    if (JSSTRING_IS_DEPENDENT(str) && !js_UndependString(NULL, str))
        return NULL;

    *js_GetGCThingFlags(str) &= ~GCF_MUTABLE;
    return str->chars;
}

JSString *
js_ConcatStrings(JSContext *cx, JSString *left, JSString *right)
{
    size_t rn, ln, lrdist, n;
    jschar *rs, *ls, *s;
    JSDependentString *ldep;    /* non-null if left should become dependent */
    JSString *str;

    if (JSSTRING_IS_DEPENDENT(right)) {
        rn = JSSTRDEP_LENGTH(right);
        rs = JSSTRDEP_CHARS(right);
    } else {
        rn = right->length;
        rs = right->chars;
    }
    if (rn == 0)
        return left;

    if (JSSTRING_IS_DEPENDENT(left) ||
        !(*js_GetGCThingFlags(left) & GCF_MUTABLE)) {
        /* We must copy if left does not own a buffer to realloc. */
        ln = JSSTRING_LENGTH(left);
        if (ln == 0)
            return right;
        ls = JSSTRING_CHARS(left);
        s = (jschar *) JS_malloc(cx, (ln + rn + 1) * sizeof(jschar));
        if (!s)
            return NULL;
        js_strncpy(s, ls, ln);
        ldep = NULL;
    } else {
        /* We can realloc left's space and make it depend on our result. */
        ln = left->length;
        if (ln == 0)
            return right;
        ls = left->chars;
        s = (jschar *) JS_realloc(cx, ls, (ln + rn + 1) * sizeof(jschar));
        if (!s)
            return NULL;

        /* Take care: right could depend on left! */
        lrdist = (size_t)(rs - ls);
        if (lrdist < ln)
            rs = s + lrdist;
        left->chars = ls = s;
        ldep = JSSTRDEP(left);
    }

    js_strncpy(s + ln, rs, rn);
    n = ln + rn;
    s[n] = 0;
    str = js_NewString(cx, s, n, GCF_MUTABLE);
    if (!str) {
        /* Out of memory: clean up any space we (re-)allocated. */
        if (!ldep) {
            JS_free(cx, s);
        } else {
            s = JS_realloc(cx, ls, (ln + 1) * sizeof(jschar));
            if (s)
                left->chars = s;
        }
    } else {
        /* Morph left into a dependent prefix if we realloc'd its buffer. */
        if (ldep) {
            JSPREFIX_SET_LENGTH(ldep, ln);
            JSPREFIX_SET_BASE(ldep, str);
#ifdef DEBUG
          {
            JSRuntime *rt = cx->runtime;
            JS_RUNTIME_METER(rt, liveDependentStrings);
            JS_RUNTIME_METER(rt, totalDependentStrings);
            JS_LOCK_RUNTIME_VOID(rt,
                (rt->strdepLengthSum += (double)ln,
                 rt->strdepLengthSquaredSum += (double)ln * (double)ln));
          }
#endif
        }
    }

    return str;
}

/*
 * May be called with null cx by js_GetStringChars, above; and by the jslock.c
 * MAKE_STRING_IMMUTABLE file-local macro.
 */
const jschar *
js_UndependString(JSContext *cx, JSString *str)
{
    size_t n, size;
    jschar *s;

    if (JSSTRING_IS_DEPENDENT(str)) {
        n = JSSTRDEP_LENGTH(str);
        size = (n + 1) * sizeof(jschar);
        s = (jschar *) (cx ? JS_malloc(cx, size) : malloc(size));
        if (!s)
            return NULL;

        js_strncpy(s, JSSTRDEP_CHARS(str), n);
        s[n] = 0;
        str->length = n;
        str->chars = s;

#ifdef DEBUG
        if (cx) {
            JSRuntime *rt = cx->runtime;
            JS_RUNTIME_UNMETER(rt, liveDependentStrings);
            JS_RUNTIME_UNMETER(rt, totalDependentStrings);
            JS_LOCK_RUNTIME_VOID(rt,
                (rt->strdepLengthSum -= (double)n,
                 rt->strdepLengthSquaredSum -= (double)n * (double)n));
        }
#endif
    }

    return str->chars;
}

/*
 * Forward declarations for URI encode/decode and helper routines
 */
static JSBool
str_decodeURI(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval);

static JSBool
str_decodeURI_Component(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                        jsval *rval);

static JSBool
str_encodeURI(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval);

static JSBool
str_encodeURI_Component(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                        jsval *rval);

static uint32
Utf8ToOneUcs4Char(const uint8 *utf8Buffer, int utf8Length);

/*
 * Contributions from the String class to the set of methods defined for the
 * global object.  escape and unescape used to be defined in the Mocha library,
 * but as ECMA decided to spec them, they've been moved to the core engine
 * and made ECMA-compliant.  (Incomplete escapes are interpreted as literal
 * characters by unescape.)
 */

/*
 * Stuff to emulate the old libmocha escape, which took a second argument
 * giving the type of escape to perform.  Retained for compatibility, and
 * copied here to avoid reliance on net.h, mkparse.c/NET_EscapeBytes.
 */

#define URL_XALPHAS     ((uint8) 1)
#define URL_XPALPHAS    ((uint8) 2)
#define URL_PATH        ((uint8) 4)

static const uint8 urlCharType[256] =
/*      Bit 0           xalpha          -- the alphas
 *      Bit 1           xpalpha         -- as xalpha but
 *                             converts spaces to plus and plus to %20
 *      Bit 2 ...       path            -- as xalphas but doesn't escape '/'
 */
    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* 0x */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* 1x */
         0,0,0,0,0,0,0,0,0,0,7,4,0,7,7,4,       /* 2x   !"#$%&'()*+,-./  */
         7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,       /* 3x  0123456789:;<=>?  */
         7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       /* 4x  @ABCDEFGHIJKLMNO  */
         7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,       /* 5X  PQRSTUVWXYZ[\]^_  */
         0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       /* 6x  `abcdefghijklmno  */
         7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,       /* 7X  pqrstuvwxyz{\}~  DEL */
         0, };

/* This matches the ECMA escape set when mask is 7 (default.) */

#define IS_OK(C, mask) (urlCharType[((uint8) (C))] & (mask))

/* See ECMA-262 15.1.2.4. */
JSBool
js_str_escape(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
    size_t i, ni, length, newlength;
    const jschar *chars;
    jschar *newchars;
    jschar ch;
    jsint mask;
    jsdouble d;
    const char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    mask = URL_XALPHAS | URL_XPALPHAS | URL_PATH;
    if (argc > 1) {
        if (!js_ValueToNumber(cx, argv[1], &d))
            return JS_FALSE;
        if (!JSDOUBLE_IS_FINITE(d) ||
            (mask = (jsint)d) != d ||
            mask & ~(URL_XALPHAS | URL_XPALPHAS | URL_PATH))
        {
            char numBuf[12];
            JS_snprintf(numBuf, sizeof numBuf, "%lx", (unsigned long) mask);
            JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                                 JSMSG_BAD_STRING_MASK, numBuf);
            return JS_FALSE;
        }
    }

    str = js_ValueToString(cx, argv[0]);
    if (!str)
        return JS_FALSE;
    argv[0] = STRING_TO_JSVAL(str);

    chars = JSSTRING_CHARS(str);
    length = newlength = JSSTRING_LENGTH(str);

    /* Take a first pass and see how big the result string will need to be. */
    for (i = 0; i < length; i++) {
        if ((ch = chars[i]) < 128 && IS_OK(ch, mask))
            continue;
        if (ch < 256) {
            if (mask == URL_XPALPHAS && ch == ' ')
                continue;   /* The character will be encoded as '+' */
            newlength += 2; /* The character will be encoded as %XX */
        } else {
            newlength += 5; /* The character will be encoded as %uXXXX */
        }

        /*
         * This overflow test works because newlength is incremented by at
         * most 5 on each iteration.
         */
        if (newlength < length) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
    }

    if (newlength >= ~(size_t)0 / sizeof(jschar)) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    newchars = (jschar *) JS_malloc(cx, (newlength + 1) * sizeof(jschar));
    if (!newchars)
        return JS_FALSE;
    for (i = 0, ni = 0; i < length; i++) {
        if ((ch = chars[i]) < 128 && IS_OK(ch, mask)) {
            newchars[ni++] = ch;
        } else if (ch < 256) {
            if (mask == URL_XPALPHAS && ch == ' ') {
                newchars[ni++] = '+'; /* convert spaces to pluses */
            } else {
                newchars[ni++] = '%';
                newchars[ni++] = digits[ch >> 4];
                newchars[ni++] = digits[ch & 0xF];
            }
        } else {
            newchars[ni++] = '%';
            newchars[ni++] = 'u';
            newchars[ni++] = digits[ch >> 12];
            newchars[ni++] = digits[(ch & 0xF00) >> 8];
            newchars[ni++] = digits[(ch & 0xF0) >> 4];
            newchars[ni++] = digits[ch & 0xF];
        }
    }
    JS_ASSERT(ni == newlength);
    newchars[newlength] = 0;

    str = js_NewString(cx, newchars, newlength, 0);
    if (!str) {
        JS_free(cx, newchars);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}
#undef IS_OK

/* See ECMA-262 15.1.2.5 */
static JSBool
str_unescape(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
    size_t i, ni, length;
    const jschar *chars;
    jschar *newchars;
    jschar ch;

    str = js_ValueToString(cx, argv[0]);
    if (!str)
        return JS_FALSE;
    argv[0] = STRING_TO_JSVAL(str);

    chars = JSSTRING_CHARS(str);
    length = JSSTRING_LENGTH(str);

    /* Don't bother allocating less space for the new string. */
    newchars = (jschar *) JS_malloc(cx, (length + 1) * sizeof(jschar));
    if (!newchars)
        return JS_FALSE;
    ni = i = 0;
    while (i < length) {
        ch = chars[i++];
        if (ch == '%') {
            if (i + 1 < length &&
                JS7_ISHEX(chars[i]) && JS7_ISHEX(chars[i + 1]))
            {
                ch = JS7_UNHEX(chars[i]) * 16 + JS7_UNHEX(chars[i + 1]);
                i += 2;
            } else if (i + 4 < length && chars[i] == 'u' &&
                       JS7_ISHEX(chars[i + 1]) && JS7_ISHEX(chars[i + 2]) &&
                       JS7_ISHEX(chars[i + 3]) && JS7_ISHEX(chars[i + 4]))
            {
                ch = (((((JS7_UNHEX(chars[i + 1]) << 4)
                        + JS7_UNHEX(chars[i + 2])) << 4)
                      + JS7_UNHEX(chars[i + 3])) << 4)
                    + JS7_UNHEX(chars[i + 4]);
                i += 5;
            }
        }
        newchars[ni++] = ch;
    }
    newchars[ni] = 0;

    str = js_NewString(cx, newchars, ni, 0);
    if (!str) {
        JS_free(cx, newchars);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

#if JS_HAS_UNEVAL
static JSBool
str_uneval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;

    str = js_ValueToSource(cx, argv[0]);
    if (!str)
        return JS_FALSE;
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}
#endif

const char js_escape_str[] = "escape";
const char js_unescape_str[] = "unescape";
#if JS_HAS_UNEVAL
const char js_uneval_str[] = "uneval";
#endif
const char js_decodeURI_str[] = "decodeURI";
const char js_encodeURI_str[] = "encodeURI";
const char js_decodeURIComponent_str[] = "decodeURIComponent";
const char js_encodeURIComponent_str[] = "encodeURIComponent";

static JSFunctionSpec string_functions[] = {
    {js_escape_str,             js_str_escape,              1,0,0},
    {js_unescape_str,           str_unescape,               1,0,0},
#if JS_HAS_UNEVAL
    {js_uneval_str,             str_uneval,                 1,0,0},
#endif
    {js_decodeURI_str,          str_decodeURI,              1,0,0},
    {js_encodeURI_str,          str_encodeURI,              1,0,0},
    {js_decodeURIComponent_str, str_decodeURI_Component,    1,0,0},
    {js_encodeURIComponent_str, str_encodeURI_Component,    1,0,0},

    {0,0,0,0,0}
};

jschar      js_empty_ucstr[]  = {0};
JSSubString js_EmptySubString = {0, js_empty_ucstr};

enum string_tinyid {
    STRING_LENGTH = -1
};

static JSPropertySpec string_props[] = {
    {js_length_str,     STRING_LENGTH,
                        JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED, 0,0},
    {0,0,0,0,0}
};

static JSBool
str_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSString *str;
    jsint slot;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;

    /*
     * Call js_ValueToString because getters and setters can be invoked on
     * objects of different class, unlike enumerate, resolve, and the other
     * class hooks.
     */
    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;

    slot = JSVAL_TO_INT(id);
    if (slot == STRING_LENGTH)
        *vp = INT_TO_JSVAL((jsint) JSSTRING_LENGTH(str));
    return JS_TRUE;
}

#define STRING_ELEMENT_ATTRS (JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT)

static JSBool
str_enumerate(JSContext *cx, JSObject *obj)
{
    JSString *str, *str1;
    size_t i, length;

    /* Avoid infinite recursion via js_obj_toSource (see bug 271477). */
    if (JS_VERSION_IS_1_2(cx))
        return JS_TRUE;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_TRUE;
    cx->newborn[GCX_STRING] = (JSGCThing *) str;

    length = JSSTRING_LENGTH(str);
    for (i = 0; i < length; i++) {
        str1 = js_NewDependentString(cx, str, i, 1, 0);
        if (!str1)
            return JS_FALSE;
        if (!OBJ_DEFINE_PROPERTY(cx, obj, INT_TO_JSID(i),
                                 STRING_TO_JSVAL(str1), NULL, NULL,
                                 STRING_ELEMENT_ATTRS, NULL)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

static JSBool
str_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags,
            JSObject **objp)
{
    JSString *str, *str1;
    jsint slot;

    if (!JSVAL_IS_INT(id) || (flags & JSRESOLVE_ASSIGNING))
        return JS_TRUE;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_TRUE;
    cx->newborn[GCX_STRING] = (JSGCThing *) str;

    slot = JSVAL_TO_INT(id);
    if ((size_t)slot < JSSTRING_LENGTH(str)) {
        str1 = js_NewDependentString(cx, str, (size_t)slot, 1, 0);
        if (!str1)
            return JS_FALSE;
        if (!OBJ_DEFINE_PROPERTY(cx, obj, INT_TO_JSID(slot),
                                 STRING_TO_JSVAL(str1), NULL, NULL,
                                 STRING_ELEMENT_ATTRS, NULL)) {
            return JS_FALSE;
        }
        *objp = obj;
    }
    return JS_TRUE;
}

JSClass js_StringClass = {
    js_String_str,
    JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE,
    JS_PropertyStub,   JS_PropertyStub,   str_getProperty,   JS_PropertyStub,
    str_enumerate, (JSResolveOp)str_resolve, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

#if JS_HAS_TOSOURCE

/*
 * String.prototype.quote is generic (as are most string methods), unlike
 * toSource, toString, and valueOf.
 */
static JSBool
str_quote(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    str = js_QuoteString(cx, str, '"');
    if (!str)
        return JS_FALSE;
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static JSBool
str_toSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval v;
    JSString *str;
    size_t i, j, k, n;
    char buf[16];
    jschar *s, *t;

    if (!JS_InstanceOf(cx, obj, &js_StringClass, argv))
        return JS_FALSE;
    v = OBJ_GET_SLOT(cx, obj, JSSLOT_PRIVATE);
    if (!JSVAL_IS_STRING(v))
        return js_obj_toSource(cx, obj, argc, argv, rval);
    str = js_QuoteString(cx, JSVAL_TO_STRING(v), '"');
    if (!str)
        return JS_FALSE;
    j = JS_snprintf(buf, sizeof buf, "(new %s(", js_StringClass.name);
    s = JSSTRING_CHARS(str);
    k = JSSTRING_LENGTH(str);
    n = j + k + 2;
    t = (jschar *) JS_malloc(cx, (n + 1) * sizeof(jschar));
    if (!t)
        return JS_FALSE;
    for (i = 0; i < j; i++)
        t[i] = buf[i];
    for (j = 0; j < k; i++, j++)
        t[i] = s[j];
    t[i++] = ')';
    t[i++] = ')';
    t[i] = 0;
    str = js_NewString(cx, t, n, 0);
    if (!str) {
        JS_free(cx, t);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

#endif /* JS_HAS_TOSOURCE */

static JSBool
str_toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval v;

    if (!JS_InstanceOf(cx, obj, &js_StringClass, argv))
        return JS_FALSE;
    v = OBJ_GET_SLOT(cx, obj, JSSLOT_PRIVATE);
    if (!JSVAL_IS_STRING(v))
        return js_obj_toString(cx, obj, argc, argv, rval);
    *rval = v;
    return JS_TRUE;
}

static JSBool
str_valueOf(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (!JS_InstanceOf(cx, obj, &js_StringClass, argv))
        return JS_FALSE;
    *rval = OBJ_GET_SLOT(cx, obj, JSSLOT_PRIVATE);
    return JS_TRUE;
}

/*
 * Java-like string native methods.
 */
static JSBool
str_substring(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    JSString *str;
    jsdouble d;
    jsdouble length, begin, end;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    if (argc != 0) {
        if (!js_ValueToNumber(cx, argv[0], &d))
            return JS_FALSE;
        length = JSSTRING_LENGTH(str);
        begin = js_DoubleToInteger(d);
        if (begin < 0)
            begin = 0;
        else if (begin > length)
            begin = length;

        if (argc == 1) {
            end = length;
        } else {
            if (!js_ValueToNumber(cx, argv[1], &d))
                return JS_FALSE;
            end = js_DoubleToInteger(d);
            if (end < 0)
                end = 0;
            else if (end > length)
                end = length;
            if (end < begin) {
                if (!JS_VERSION_IS_1_2(cx)) {
                    /* XXX emulate old JDK1.0 java.lang.String.substring. */
                    jsdouble tmp = begin;
                    begin = end;
                    end = tmp;
                } else {
                    end = begin;
                }
            }
        }

        str = js_NewDependentString(cx, str, (size_t)begin,
                                    (size_t)(end - begin), 0);
        if (!str)
            return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static JSBool
str_toLowerCase(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    JSString *str;
    size_t i, n;
    jschar *s, *news;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    n = JSSTRING_LENGTH(str);
    news = (jschar *) JS_malloc(cx, (n + 1) * sizeof(jschar));
    if (!news)
        return JS_FALSE;
    s = JSSTRING_CHARS(str);
    for (i = 0; i < n; i++)
        news[i] = JS_TOLOWER(s[i]);
    news[n] = 0;
    str = js_NewString(cx, news, n, 0);
    if (!str) {
        JS_free(cx, news);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static JSBool
str_toLocaleLowerCase(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    JSString *str;

    /*
     * Forcefully ignore the first (or any) argument and return toLowerCase(),
     * ECMA has reserved that argument, presumably for defining the locale.
     */
    if (cx->localeCallbacks && cx->localeCallbacks->localeToLowerCase) {
        str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
        if (!str)
            return JS_FALSE;
        argv[-1] = STRING_TO_JSVAL(str);
        return cx->localeCallbacks->localeToLowerCase(cx, str, rval);
    }
    return str_toLowerCase(cx, obj, 0, argv, rval);
}

static JSBool
str_toUpperCase(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    JSString *str;
    size_t i, n;
    jschar *s, *news;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    n = JSSTRING_LENGTH(str);
    news = (jschar *) JS_malloc(cx, (n + 1) * sizeof(jschar));
    if (!news)
        return JS_FALSE;
    s = JSSTRING_CHARS(str);
    for (i = 0; i < n; i++)
        news[i] = JS_TOUPPER(s[i]);
    news[n] = 0;
    str = js_NewString(cx, news, n, 0);
    if (!str) {
        JS_free(cx, news);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static JSBool
str_toLocaleUpperCase(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    JSString *str;

    /*
     * Forcefully ignore the first (or any) argument and return toUpperCase(),
     * ECMA has reserved that argument, presumbaly for defining the locale.
     */
    if (cx->localeCallbacks && cx->localeCallbacks->localeToUpperCase) {
        str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
        if (!str)
            return JS_FALSE;
        argv[-1] = STRING_TO_JSVAL(str);
        return cx->localeCallbacks->localeToUpperCase(cx, str, rval);
    }
    return str_toUpperCase(cx, obj, 0, argv, rval);
}

static JSBool
str_localeCompare(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                  jsval *rval)
{
    JSString *str, *thatStr;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    if (argc == 0) {
        *rval = JSVAL_ZERO;
    } else {
        thatStr = js_ValueToString(cx, argv[0]);
        if (!thatStr)
            return JS_FALSE;
        if (cx->localeCallbacks && cx->localeCallbacks->localeCompare) {
            argv[0] = STRING_TO_JSVAL(thatStr);
            return cx->localeCallbacks->localeCompare(cx, str, thatStr, rval);
        }
        *rval = INT_TO_JSVAL(js_CompareStrings(str, thatStr));
    }
    return JS_TRUE;
}

static JSBool
str_charAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
    jsdouble d;
    size_t index;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    if (argc == 0) {
        d = 0.0;
    } else {
        if (!js_ValueToNumber(cx, argv[0], &d))
            return JS_FALSE;
        d = js_DoubleToInteger(d);
    }

    if (d < 0 || JSSTRING_LENGTH(str) <= d) {
        *rval = JS_GetEmptyStringValue(cx);
    } else {
        index = (size_t)d;
        str = js_NewDependentString(cx, str, index, 1, 0);
        if (!str)
            return JS_FALSE;
        *rval = STRING_TO_JSVAL(str);
    }
    return JS_TRUE;
}

static JSBool
str_charCodeAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
               jsval *rval)
{
    JSString *str;
    jsdouble d;
    size_t index;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);

    if (argc == 0) {
        d = 0.0;
    } else {
        if (!js_ValueToNumber(cx, argv[0], &d))
            return JS_FALSE;
        d = js_DoubleToInteger(d);
    }

    if (d < 0 || JSSTRING_LENGTH(str) <= d) {
        *rval = JS_GetNaNValue(cx);
    } else {
        index = (size_t)d;
        *rval = INT_TO_JSVAL((jsint) JSSTRING_CHARS(str)[index]);
    }
    return JS_TRUE;
}

jsint
js_BoyerMooreHorspool(const jschar *text, jsint textlen,
                      const jschar *pat, jsint patlen,
                      jsint start)
{
    jsint i, j, k, m;
    uint8 skip[BMH_CHARSET_SIZE];
    jschar c;

    JS_ASSERT(0 < patlen && patlen <= BMH_PATLEN_MAX);
    for (i = 0; i < BMH_CHARSET_SIZE; i++)
        skip[i] = (uint8)patlen;
    m = patlen - 1;
    for (i = 0; i < m; i++) {
        c = pat[i];
        if (c >= BMH_CHARSET_SIZE)
            return BMH_BAD_PATTERN;
        skip[c] = (uint8)(m - i);
    }
    for (k = start + m;
         k < textlen;
         k += ((c = text[k]) >= BMH_CHARSET_SIZE) ? patlen : skip[c]) {
        for (i = k, j = m; ; i--, j--) {
            if (j < 0)
                return i + 1;
            if (text[i] != pat[j])
                break;
        }
    }
    return -1;
}

static JSBool
str_indexOf(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str, *str2;
    jsint i, j, index, textlen, patlen;
    const jschar *text, *pat;
    jsdouble d;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);
    text = JSSTRING_CHARS(str);
    textlen = (jsint) JSSTRING_LENGTH(str);

    str2 = js_ValueToString(cx, argv[0]);
    if (!str2)
        return JS_FALSE;
    argv[0] = STRING_TO_JSVAL(str2);
    pat = JSSTRING_CHARS(str2);
    patlen = (jsint) JSSTRING_LENGTH(str2);

    if (argc > 1) {
        if (!js_ValueToNumber(cx, argv[1], &d))
            return JS_FALSE;
        d = js_DoubleToInteger(d);
        if (d < 0)
            i = 0;
        else if (d > textlen)
            i = textlen;
        else
            i = (jsint)d;
    } else {
        i = 0;
    }
    if (patlen == 0) {
        *rval = INT_TO_JSVAL(i);
        return JS_TRUE;
    }

    /* XXX tune the BMH threshold (512) */
    if ((jsuint)(patlen - 2) <= BMH_PATLEN_MAX - 2 && textlen >= 512) {
        index = js_BoyerMooreHorspool(text, textlen, pat, patlen, i);
        if (index != BMH_BAD_PATTERN)
            goto out;
    }

    index = -1;
    j = 0;
    while (i + j < textlen) {
        if (text[i + j] == pat[j]) {
            if (++j == patlen) {
                index = i;
                break;
            }
        } else {
            i++;
            j = 0;
        }
    }

out:
    *rval = INT_TO_JSVAL(index);
    return JS_TRUE;
}

static JSBool
str_lastIndexOf(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                  jsval *rval)
{
    JSString *str, *str2;
    const jschar *text, *pat;
    jsint i, j, textlen, patlen;
    jsdouble d;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(obj));
    if (!str)
        return JS_FALSE;
    argv[-1] = STRING_TO_JSVAL(str);
    text = JSSTRING_CHARS(str);
    textlen = (jsint) JSSTRING_LENGTH(str);

    str2 = js_ValueToString(cx, argv[0]);
    if (!str2)
        return JS_FALSE;
    argv[0] = STRING_TO_JSVAL(str2);
    pat = JSSTRING_CHARS(str2);
    patlen = (jsint) JSSTRING_LENGTH(str2);

    if (argc > 1) {
        if (!js_ValueToNumber(cx, argv[1], &d))
            return JS_FALSE;
        if (JSDOUBLE_IS_NaN(d)) {
            i = textlen;
        } else {
            d = js_DoubleToInteger(d);
            if (d < 0)
                i = 0;
            else if (d > textlen)
                i = textlen;
            else
                i = (jsint)d;
        }
    } else {
        i = textlen;
    }

    if (patlen == 0) {
        *rval = INT_TO_JSVAL(i);
        return JS_TRUE;
    }

    j = 0;
    while (i >= 0) {
        /* Don't assume that text is NUL-terminated: it could be dependent. */
        if (i + j < textlen && text[i + j] == pat[j]) {
            if (++j == patlen)
                break;
        } else {
            i--;
            j = 0;
        }
    }
    *rval = INT_TO_JSVAL(i);
    return JS_TRUE;
}

/*
 * Perl-inspired string functions.
 */
#if JS_HAS_REGEXPS
typedef struct GlobData {
    uintN       flags;          /* inout: mode and flag bits, see below */
    uintN       optarg;         /* in: index of optional flags argument */
    JSString    *str;           /* out: 'this' parameter object as string */
    JSRegExp    *regexp;        /* out: regexp parameter object private data */
} GlobData;

/*
 * Mode and flag bit definitions for match_or_replace's GlobData.flags field.
 */
#define MODE_MATCH      0x00    /* in: return match array on success */
#define MODE_REPLACE    0x01    /* in: match and replace */
#define MODE_SEARCH     0x02    /* in: search only, return match index or -1 */
#define GET_MODE(f)     ((f) & 0x03)
#define FORCE_FLAT      0x04    /* in: force flat (non-regexp) string match */
#define KEEP_REGEXP     0x08    /* inout: keep GlobData.regexp alive for caller
                                          of match_or_replace; if set on input
                                          but clear on output, regexp ownership
                                          does not pass 