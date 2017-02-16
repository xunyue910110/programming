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
 * JavaScript API.
 */
#include "jsstddef.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsclist.h"
#include "jsdhash.h"
#include "jsprf.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jsbool.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsdate.h"
#include "jsdtoa.h"
#include "jsemit.h"
#include "jsexn.h"
#include "jsfun.h"
#include "jsgc.h"
#include "jsinterp.h"
#include "jslock.h"
#include "jsmath.h"
#include "jsnum.h"
#include "jsobj.h"
#include "jsopcode.h"
#include "jsparse.h"
#include "jsregexp.h"
#include "jsscan.h"
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"
#include "prmjtime.h"

#if JS_HAS_FILE_OBJECT
#include "jsfile.h"
#endif

#if JS_HAS_XML_SUPPORT
#include "jsxml.h"
#endif

#ifdef HAVE_VA_LIST_AS_ARRAY
#define JS_ADDRESSOF_VA_LIST(ap) ((va_list *)(ap))
#else
#define JS_ADDRESSOF_VA_LIST(ap) (&(ap))
#endif

#if defined(JS_PARANOID_REQUEST) && defined(JS_THREADSAFE)
#define CHECK_REQUEST(cx)       JS_ASSERT(cx->requestDepth)
#else
#define CHECK_REQUEST(cx)       ((void)0)
#endif

JS_PUBLIC_API(int64)
JS_Now()
{
    return PRMJ_Now();
}

JS_PUBLIC_API(jsval)
JS_GetNaNValue(JSContext *cx)
{
    return DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
}

JS_PUBLIC_API(jsval)
JS_GetNegativeInfinityValue(JSContext *cx)
{
    return DOUBLE_TO_JSVAL(cx->runtime->jsNegativeInfinity);
}

JS_PUBLIC_API(jsval)
JS_GetPositiveInfinityValue(JSContext *cx)
{
    return DOUBLE_TO_JSVAL(cx->runtime->jsPositiveInfinity);
}

JS_PUBLIC_API(jsval)
JS_GetEmptyStringValue(JSContext *cx)
{
    return STRING_TO_JSVAL(cx->runtime->emptyString);
}

static JSBool
TryArgumentFormatter(JSContext *cx, const char **formatp, JSBool fromJS,
                     jsval **vpp, va_list *app)
{
    const char *format;
    JSArgumentFormatMap *map;

    format = *formatp;
    for (map = cx->argumentFormatMap; map; map = map->next) {
        if (!strncmp(format, map->format, map->length)) {
            *formatp = format + map->length;
            return map->formatter(cx, format, fromJS, vpp, app);
        }
    }
    JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_BAD_CHAR, format);
    return JS_FALSE;
}

JS_PUBLIC_API(JSBool)
JS_ConvertArguments(JSContext *cx, uintN argc, jsval *argv, const char *format,
                    ...)
{
    va_list ap;
    JSBool ok;

    va_start(ap, format);
    ok = JS_ConvertArgumentsVA(cx, argc, argv, format, ap);
    va_end(ap);
    return ok;
}

JS_PUBLIC_API(JSBool)
JS_ConvertArgumentsVA(JSContext *cx, uintN argc, jsval *argv,
                      const char *format, va_list ap)
{
    jsval *sp;
    JSBool required;
    char c;
    JSFunction *fun;
    jsdouble d;
    JSString *str;
    JSObject *obj;

    CHECK_REQUEST(cx);
    sp = argv;
    required = JS_TRUE;
    while ((c = *format++) != '\0') {
        if (isspace(c))
            continue;
        if (c == '/') {
            required = JS_FALSE;
            continue;
        }
        if (sp == argv + argc) {
            if (required) {
                fun = js_ValueToFunction(cx, &argv[-2], 0);
                if (fun) {
                    char numBuf[12];
                    JS_snprintf(numBuf, sizeof numBuf, "%u", argc);
                    JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                                         JSMSG_MORE_ARGS_NEEDED,
                                         JS_GetFunctionName(fun), numBuf,
                                         (argc == 1) ? "" : "s");
                }
                return JS_FALSE;
            }
            break;
        }
        switch (c) {
          case 'b':
            if (!js_ValueToBoolean(cx, *sp, va_arg(ap, JSBool *)))
                return JS_FALSE;
            break;
          case 'c':
            if (!js_ValueToUint16(cx, *sp, va_arg(ap, uint16 *)))
                return JS_FALSE;
            break;
          case 'i':
            if (!js_ValueToECMAInt32(cx, *sp, va_arg(ap, int32 *)))
                return JS_FALSE;
            break;
          case 'u':
            if (!js_ValueToECMAUint32(cx, *sp, va_arg(ap, uint32 *)))
                return JS_FALSE;
            break;
          case 'j':
            if (!js_ValueToInt32(cx, *sp, va_arg(ap, int32 *)))
                return JS_FALSE;
            break;
          case 'd':
            if (!js_ValueToNumber(cx, *sp, va_arg(ap, jsdouble *)))
                return JS_FALSE;
            break;
          case 'I':
            if (!js_ValueToNumber(cx, *sp, &d))
                return JS_FALSE;
            *va_arg(ap, jsdouble *) = js_DoubleToInteger(d);
            break;
          case 's':
          case 'S':
          case 'W':
            str = js_ValueToString(cx, *sp);
            if (!str)
                return JS_FALSE;
            *sp = STRING_TO_JSVAL(str);
            if (c == 's')
                *va_arg(ap, char **) = JS_GetStringBytes(str);
            else if (c == 'W')
                *va_arg(ap, jschar **) = JS_GetStringChars(str);
            else
                *va_arg(ap, JSString **) = str;
            break;
          case 'o':
            if (!js_ValueToObject(cx, *sp, &obj))
                return JS_FALSE;
            *sp = OBJECT_TO_JSVAL(obj);
            *va_arg(ap, JSObject **) = obj;
            break;
          case 'f':
            obj = js_ValueToFunctionObject(cx, sp, 0);
            if (!obj)
                return JS_FALSE;
            *va_arg(ap, JSFunction **) = (JSFunction *) JS_GetPrivate(cx, obj);
            break;
          case 'v':
            *va_arg(ap, jsval *) = *sp;
            break;
          case '*':
            break;
          default:
            format--;
            if (!TryArgumentFormatter(cx, &format, JS_TRUE, &sp,
                                      JS_ADDRESSOF_VA_LIST(ap))) {
                return JS_FALSE;
            }
            /* NB: the formatter already updated sp, so we continue here. */
            continue;
        }
        sp++;
    }
    return JS_TRUE;
}

JS_PUBLIC_API(jsval *)
JS_PushArguments(JSContext *cx, void **markp, const char *format, ...)
{
    va_list ap;
    jsval *argv;

    va_start(ap, format);
    argv = JS_PushArgumentsVA(cx, markp, format, ap);
    va_end(ap);
    return argv;
}

JS_PUBLIC_API(jsval *)
JS_PushArgumentsVA(JSContext *cx, void **markp, const char *format, va_list ap)
{
    uintN argc;
    jsval *argv, *sp;
    char c;
    const char *cp;
    JSString *str;
    JSFunction *fun;
    JSStackHeader *sh;

    CHECK_REQUEST(cx);
    *markp = NULL;
    argc = 0;
    for (cp = format; (c = *cp) != '\0'; cp++) {
        /*
         * Count non-space non-star characters as individual jsval arguments.
         * This may over-allocate stack, but we'll fix below.
         */
        if (isspace(c) || c == '*')
            continue;
        argc++;
    }
    sp = js_AllocStack(cx, argc, markp);
    if (!sp)
        return NULL;
    argv = sp;
    while ((c = *format++) != '\0') {
        if (isspace(c) || c == '*')
            continue;
        switch (c) {
          case 'b':
            *sp = BOOLEAN_TO_JSVAL((JSBool) va_arg(ap, int));
            break;
          case 'c':
            *sp = INT_TO_JSVAL((uint16) va_arg(ap, unsigned int));
            break;
          case 'i':
          case 'j':
            if (!js_NewNumberValue(cx, (jsdouble) va_arg(ap, int32), sp))
                goto bad;
            break;
          case 'u':
            if (!js_NewNumberValue(cx, (jsdouble) va_arg(ap, uint32), sp))
                goto bad;
            break;
          case 'd':
          case 'I':
            if (!js_NewDoubleValue(cx, va_arg(ap, jsdouble), sp))
                goto bad;
            break;
          case 's':
            str = JS_NewStringCopyZ(cx, va_arg(ap, char *));
            if (!str)
                goto bad;
            *sp = STRING_TO_JSVAL(str);
            break;
          case 'W':
            str = JS_NewUCStringCopyZ(cx, va_arg(ap, jschar *));
            if (!str)
                goto bad;
            *sp = STRING_TO_JSVAL(str);
            break;
          case 'S':
            str = va_arg(ap, JSString *);
            *sp = STRING_TO_JSVAL(str);
            break;
          case 'o':
            *sp = OBJECT_TO_JSVAL(va_arg(ap, JSObject *));
            break;
          case 'f':
            fun = va_arg(ap, JSFunction *);
            *sp = fun ? OBJECT_TO_JSVAL(fun->object) : JSVAL_NULL;
            break;
          case 'v':
            *sp = va_arg(ap, jsval);
            break;
          default:
            format--;
            if (!TryArgumentFormatter(cx, &format, JS_FALSE, &sp,
                                      JS_ADDRESSOF_VA_LIST(ap))) {
                goto bad;
            }
            /* NB: the formatter already updated sp, so we continue here. */
            continue;
        }
        sp++;
    }

    /*
     * We may have overallocated stack due to a multi-character format code
     * handled by a JSArgumentFormatter.  Give back that stack space!
     */
    JS_ASSERT(sp <= argv + argc);
    if (sp < argv + argc) {
        /* Return slots not pushed to the current stack arena. */
        cx->stackPool.current->avail = (jsuword)sp;

        /* Reduce the count of slots the GC will scan in this stack segment. */
        sh = cx->stackHeaders;
        JS_ASSERT(JS_STACK_SEGMENT(sh) + sh->nslots == argv + argc);
        sh->nslots -= argc - (sp - argv);
    }
    return argv;

bad:
    js_FreeStack(cx, *markp);
    return NULL;
}

JS_PUBLIC_API(void)
JS_PopArguments(JSContext *cx, void *mark)
{
    CHECK_REQUEST(cx);
    js_FreeStack(cx, mark);
}

JS_PUBLIC_API(JSBool)
JS_AddArgumentFormatter(JSContext *cx, const char *format,
                        JSArgumentFormatter formatter)
{
    size_t length;
    JSArgumentFormatMap **mpp, *map;

    length = strlen(format);
    mpp = &cx->argumentFormatMap;
    while ((map = *mpp) != NULL) {
        /* Insert before any shorter string to match before prefixes. */
        if (map->length < length)
            break;
        if (map->length == length && !strcmp(map->format, format))
            goto out;
        mpp = &map->next;
    }
    map = (JSArgumentFormatMap *) JS_malloc(cx, sizeof *map);
    if (!map)
        return JS_FALSE;
    map->format = format;
    map->length = length;
    map->next = *mpp;
    *mpp = map;
out:
    map->formatter = formatter;
    return JS_TRUE;
}

JS_PUBLIC_API(void)
JS_RemoveArgumentFormatter(JSContext *cx, const char *format)
{
    size_t length;
    JSArgumentFormatMap **mpp, *map;

    length = strlen(format);
    mpp = &cx->argumentFormatMap;
    while ((map = *mpp) != NULL) {
        if (map->length == length && !strcmp(map->format, format)) {
            *mpp = map->next;
            JS_free(cx, map);
            return;
        }
        mpp = &map->next;
    }
}

JS_PUBLIC_API(JSBool)
JS_ConvertValue(JSContext *cx, jsval v, JSType type, jsval *vp)
{
    JSBool ok, b;
    JSObject *obj;
    JSString *str;
    jsdouble d, *dp;

    CHECK_REQUEST(cx);
    switch (type) {
      case JSTYPE_VOID:
        *vp = JSVAL_VOID;
        ok = JS_TRUE;
        break;
      case JSTYPE_OBJECT:
        ok = js_ValueToObject(cx, v, &obj);
        if (ok)
            *vp = OBJECT_TO_JSVAL(obj);
        break;
      case JSTYPE_FUNCTION:
        *vp = v;
        obj = js_ValueToFunctionObject(cx, vp, JSV2F_SEARCH_STACK);
        ok = (obj != NULL);
        break;
      case JSTYPE_STRING:
        str = js_ValueToString(cx, v);
        ok = (str != NULL);
        if (ok)
            *vp = STRING_TO_JSVAL(str);
        break;
      case JSTYPE_NUMBER:
        ok = js_ValueToNumber(cx, v, &d);
        if (ok) {
            dp = js_NewDouble(cx, d, 0);
            ok = (dp != NULL);
            if (ok)
                *vp = DOUBLE_TO_JSVAL(dp);
        }
        break;
      case JSTYPE_BOOLEAN:
        ok = js_ValueToBoolean(cx, v, &b);
        if (ok)
            *vp = BOOLEAN_TO_JSVAL(b);
        break;
      default: {
        char numBuf[12];
        JS_snprintf(numBuf, sizeof numBuf, "%d", (int)type);
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_BAD_TYPE,
                             numBuf);
        ok = JS_FALSE;
        break;
      }
    }
    return ok;
}

JS_PUBLIC_API(JSBool)
JS_ValueToObject(JSContext *cx, jsval v, JSObject **objp)
{
    CHECK_REQUEST(cx);
    return js_ValueToObject(cx, v, objp);
}

JS_PUBLIC_API(JSFunction *)
JS_ValueToFunction(JSContext *cx, jsval v)
{
    CHECK_REQUEST(cx);
    return js_ValueToFunction(cx, &v, JSV2F_SEARCH_STACK);
}

JS_PUBLIC_API(JSFunction *)
JS_ValueToConstructor(JSContext *cx, jsval v)
{
    CHECK_REQUEST(cx);
    return js_ValueToFunction(cx, &v, JSV2F_SEARCH_STACK);
}

JS_PUBLIC_API(JSString *)
JS_ValueToString(JSContext *cx, jsval v)
{
    CHECK_REQUEST(cx);
    return js_ValueToString(cx, v);
}

JS_PUBLIC_API(JSBool)
JS_ValueToNumber(JSContext *cx, jsval v, jsdouble *dp)
{
    CHECK_REQUEST(cx);
    return js_ValueToNumber(cx, v, dp);
}

JS_PUBLIC_API(JSBool)
JS_ValueToECMAInt32(JSContext *cx, jsval v, int32 *ip)
{
    CHECK_REQUEST(cx);
    return js_ValueToECMAInt32(cx, v, ip);
}

JS_PUBLIC_API(JSBool)
JS_ValueToECMAUint32(JSContext *cx, jsval v, uint32 *ip)
{
    CHECK_REQUEST(cx);
    return js_ValueToECMAUint32(cx, v, ip);
}

JS_PUBLIC_API(JSBool)
JS_ValueToInt32(JSContext *cx, jsval v, int32 *ip)
{
    CHECK_REQUEST(cx);
    return js_ValueToInt32(cx, v, ip);
}

JS_PUBLIC_API(JSBool)
JS_ValueToUint16(JSContext *cx, jsval v, uint16 *ip)
{
    CHECK_REQUEST(cx);
    return js_ValueToUint16(cx, v, ip);
}

JS_PUBLIC_API(JSBool)
JS_ValueToBoolean(JSContext *cx, jsval v, JSBool *bp)
{
    CHECK_REQUEST(cx);
    return js_ValueToBoolean(cx, v, bp);
}

JS_PUBLIC_API(JSType)
JS_TypeOfValue(JSContext *cx, jsval v)
{
    JSType type;
    JSObject *obj;
    JSObjectOps *ops;
    JSClass *clasp;

    CHECK_REQUEST(cx);
    if (JSVAL_IS_OBJECT(v)) {
        type = JSTYPE_OBJECT;           /* XXXbe JSTYPE_NULL for JS2 */
        obj = JSVAL_TO_OBJECT(v);
        if (obj) {
            ops = obj->map->ops;
#if JS_HAS_XML_SUPPORT
            if (ops == &js_XMLObjectOps.base) {
                type = JSTYPE_XML;
            } else
#endif
            {
                /*
                 * ECMA 262, 11.4.3 says that any native object that implements
                 * [[Call]] should be of type "function". Note that RegExp and
                 * Script are both of type "function" for compatibility with
                 * older SpiderMonkeys.
                 */
                clasp = OBJ_GET_CLASS(cx, obj);
                if ((ops == &js_ObjectOps)
                    ? (clasp->call
                       ? (clasp == &js_RegExpClass || clasp == &js_ScriptClass)
                       : clasp == &js_FunctionClass)
                    : ops->call != NULL) {
                    type = JSTYPE_FUNCTION;
                } else {
#ifdef NARCISSUS
                    if (!OBJ_GET_PROPERTY(cx, obj,
                                          ATOM_TO_JSID(cx->runtime->atomState
                                                       .callAtom),
                                          &v)) {
                        JS_ClearPendingException(cx);
                    } else if (JSVAL_IS_FUNCTION(cx, v)) {
                        type = JSTYPE_FUNCTION;
                    }
#endif
                }
            }
        }
    } else if (JSVAL_IS_NUMBER(v)) {
        type = JSTYPE_NUMBER;
    } else if (JSVAL_IS_STRING(v)) {
        type = JSTYPE_STRING;
    } else if (JSVAL_IS_BOOLEAN(v)) {
        type = JSTYPE_BOOLEAN;
    } else {
        type = JSTYPE_VOID;
    }
    return type;
}

JS_PUBLIC_API(const char *)
JS_GetTypeName(JSContext *cx, JSType type)
{
    if ((uintN)type >= (uintN)JSTYPE_LIMIT)
        return NULL;
    return js_type_str[type];
}

/************************************************************************/

JS_PUBLIC_API(JSRuntime *)
JS_NewRuntime(uint32 maxbytes)
{
    JSRuntime *rt;

#ifdef DEBUG
    JS_BEGIN_MACRO
    /*
     * This code asserts that the numbers associated with the error names in
     * jsmsg.def are monotonically increasing.  It uses values for the error
     * names enumerated in jscntxt.c.  It's not a compiletime check, but it's
     * better than nothing.
     */
    int errorNumber = 0;
#define MSG_DEF(name, number, count, exception, format) \
    JS_ASSERT(name == errorNumber++);
#include "js.msg"
#undef MSG_DEF
    JS_END_MACRO;
#endif /* DEBUG */

    if (!js_InitStringGlobals())
        return NULL;
    rt = (JSRuntime *) malloc(sizeof(JSRuntime));
    if (!rt)
        return NULL;

    /* Initialize infallibly first, so we can goto bad and JS_DestroyRuntime. */
    memset(rt, 0, sizeof(JSRuntime));
    JS_INIT_CLIST(&rt->contextList);
    JS_INIT_CLIST(&rt->trapList);
    JS_INIT_CLIST(&rt->watchPointList);

    if (!js_InitGC(rt, maxbytes))
        goto bad;
#ifdef JS_THREADSAFE
    rt->gcLock = JS_NEW_LOCK();
    if (!rt->gcLock)
        goto bad;
    rt->gcDone = JS_NEW_CONDVAR(rt->gcLock);
    if (!rt->gcDone)
        goto bad;
    rt->requestDone = JS_NEW_CONDVAR(rt->gcLock);
    if (!rt->requestDone)
        goto bad;
    /* this is asymmetric with JS_ShutDown: */
    if (!js_SetupLocks(8, 16))
        goto bad;
    rt->rtLock = JS_NEW_LOCK();
    if (!rt->rtLock)
        goto bad;
    rt->stateChange = JS_NEW_CONDVAR(rt->gcLock);
    if (!rt->stateChange)
        goto bad;
    rt->setSlotLock = JS_NEW_LOCK();
    if (!rt->setSlotLock)
        goto bad;
    rt->setSlotDone = JS_NEW_CONDVAR(rt->setSlotLock);
    if (!rt->setSlotDone)
        goto bad;
    rt->scopeSharingDone = JS_NEW_CONDVAR(rt->gcLock);
    if (!rt->scopeSharingDone)
        goto bad;
    rt->scopeSharingTodo = NO_SCOPE_SHARING_TODO;
#endif
    rt->propertyCache.empty = JS_TRUE;
    if (!js_InitPropertyTree(rt))
        goto bad;
    return rt;

bad:
    JS_DestroyRuntime(rt);
    return NULL;
}

JS_PUBLIC_API(void)
JS_DestroyRuntime(JSRuntime *rt)
{
#ifdef DEBUG
    /* Don't hurt everyone in leaky ol' Mozilla with a fatal JS_ASSERT! */
    if (!JS_CLIST_IS_EMPTY(&rt->contextList)) {
        JSContext *cx, *iter = NULL;
        uintN cxcount = 0;
        while ((cx = js_ContextIterator(rt, JS_TRUE, &iter)) != NULL)
            cxcount++;
        fprintf(stderr,
"JS API usage error: %u contexts left in runtime upon JS_DestroyRuntime.\n",
                cxcount);
    }
#endif

    js_FreeRuntimeScriptState(rt);
    js_FinishAtomState(&rt->atomState);
    js_FinishGC(rt);
#ifdef JS_THREADSAFE
    if (rt->gcLock)
        JS_DESTROY_LOCK(rt->gcLock);
    if (rt->gcDone)
        JS_DESTROY_CONDVAR(rt->gcDone);
    if (rt->requestDone)
        JS_DESTROY_CONDVAR(rt->requestDone);
    if (rt->rtLock)
        JS_DESTROY_LOCK(rt->rtLock);
    if (rt->stateChange)
        JS_DESTROY_CONDVAR(rt->stateChange);
    if (rt->setSlotLock)
        JS_DESTROY_LOCK(rt->setSlotLock);
    if (rt->setSlotDone)
        JS_DESTROY_CONDVAR(rt->setSlotDone);
    if (rt->scopeSharingDone)
        JS_DESTROY_CONDVAR(rt->scopeSharingDone);
#endif
    js_FinishPropertyTree(rt);
    free(rt);
}

JS_PUBLIC_API(void)
JS_ShutDown(void)
{
    JS_ArenaShutDown();
    js_FinishDtoa();
    js_FreeStringGlobals();
#ifdef JS_THREADSAFE
    js_CleanupLocks();
#endif
}

JS_PUBLIC_API(void *)
JS_GetRuntimePrivate(JSRuntime *rt)
{
    return rt->data;
}

JS_PUBLIC_API(void)
JS_SetRuntimePrivate(JSRuntime *rt, void *data)
{
    rt->data = data;
}

#ifdef JS_THREADSAFE

JS_PUBLIC_API(void)
JS_BeginRequest(JSContext *cx)
{
    JSRuntime *rt;

    JS_ASSERT(cx->thread);
    if (!cx->requestDepth) {
        /* Wait until the GC is finished. */
        rt = cx->runtime;
        JS_LOCK_GC(rt);

        /* NB: we use cx->thread here, not js_CurrentThreadId(). */
        if (rt->gcThread != cx->thread) {
            while (rt->gcLevel > 0)
                JS_AWAIT_GC_DONE(rt);
        }

        /* Indicate that a request is running. */
        rt->requestCount++;
        cx->requestDepth = 1;
        JS_UNLOCK_GC(rt);
        return;
    }
    cx->requestDepth++;
}

JS_PUBLIC_API(void)
JS_EndRequest(JSContext *cx)
{
    JSRuntime *rt;
    JSScope *scope, **todop;
    uintN nshares;

    CHECK_REQUEST(cx);
    JS_ASSERT(cx->requestDepth > 0);
    if (cx->requestDepth == 1) {
        /* Lock before clearing to interlock with ClaimScope, in jslock.c. */
        rt = cx->runtime;
        JS_LOCK_GC(rt);
        cx->requestDepth = 0;

        /* See whether cx has any single-threaded scopes to start sharing. */
        todop = &rt->scopeSharingTodo;
        nshares = 0;
        while ((scope = *todop) != NO_SCOPE_SHARING_TODO) {
            if (scope->ownercx != cx) {
                todop = &scope->u.link;
                continue;
            }
            *todop = scope->u.link;
            scope->u.link = NULL;       /* null u.link for sanity ASAP */

            /*
             * If js_DropObjectMap returns null, we held the last ref to scope.
             * The waiting thread(s) must have been killed, after which the GC
             * collected the object that held this scope.  Unlikely, because it
             * requires that the GC ran (e.g., from a branch callback) during
             * this request, but possible.
             */
            if (js_DropObjectMap(cx, &scope->map, NULL)) {
                js_InitLock(&scope->lock);
                scope->u.count = 0;                 /* NULL may not pun as 0 */
                js_FinishSharingScope(rt, scope);   /* set ownercx = NULL */
                nshares++;
            }
        }
        if (nshares)
            JS_NOTIFY_ALL_CONDVAR(rt->scopeSharingDone);

        /* Give the GC a chance to run if this was the last request running. */
        JS_ASSERT(rt->requestCount > 0);
        rt->requestCount--;
        if (rt->requestCount == 0)
            JS_NOTIFY_REQUEST_DONE(rt);

        JS_UNLOCK_GC(rt);
        return;
    }

    cx->requestDepth--;
}

/* Yield to pending GC operations, regardless of request depth */
JS_PUBLIC_API(void)
JS_YieldRequest(JSContext *cx)
{
    JSRuntime *rt;

    JS_ASSERT(cx->thread);
    CHECK_REQUEST(cx);

    rt = cx->runtime;
    JS_LOCK_GC(rt);
    JS_ASSERT(rt->requestCount > 0);
    rt->requestCount--;
    if (rt->requestCount == 0)
        JS_NOTIFY_REQUEST_DONE(rt);
    JS_UNLOCK_GC(rt);
    /* XXXbe give the GC or another request calling it a chance to run here?
             Assumes FIFO scheduling */
    JS_LOCK_GC(rt);
    rt->requestCount++;
    JS_UNLOCK_GC(rt);
}

JS_PUBLIC_API(jsrefcount)
JS_SuspendRequest(JSContext *cx)
{
    jsrefcount saveDepth = cx->requestDepth;

    while (cx->requestDepth)
        JS_EndRequest(cx);
    return saveDepth;
}

JS_PUBLIC_API(void)
JS_ResumeRequest(JSContext *cx, jsrefcount saveDepth)
{
    JS_ASSERT(!cx->requestDepth);
    while (--saveDepth >= 0)
        JS_BeginRequest(cx);
}

#endif /* JS_THREADSAFE */

JS_PUBLIC_API(void)
JS_Lock(JSRuntime *rt)
{
    JS_LOCK_RUNTIME(rt);
}

JS_PUBLIC_API(void)
JS_Unlock(JSRuntime *rt)
{
    JS_UNLOCK_RUNTIME(rt);
}

JS_PUBLIC_API(JSContext *)
JS_NewContext(JSRuntime *rt, size_t stackChunkSize)
{
    return js_NewContext(rt, stackChunkSize);
}

JS_PUBLIC_API(void)
JS_DestroyContext(JSContext *cx)
{
    js_DestroyContext(cx, JS_FORCE_GC);
}

JS_PUBLIC_API(void)
JS_DestroyContextNoGC(JSContext *cx)
{
    js_DestroyContext(cx, JS_NO_GC);
}

JS_PUBLIC_API(void)
JS_DestroyContextMaybeGC(JSContext *cx)
{
    js_DestroyContext(cx, JS_MAYBE_GC);
}

JS_PUBLIC_API(void *)
JS_GetContextPrivate(JSContext *cx)
{
    return cx->data;
}

JS_PUBLIC_API(void)
JS_SetContextPrivate(JSContext *cx, void *data)
{
    cx->data = data;
}

JS_PUBLIC_API(JSRuntime *)
JS_GetRuntime(JSContext *cx)
{
    return cx->runtime;
}

JS_PUBLIC_API(JSContext *)
JS_ContextIterator(JSRuntime *rt, JSContext **iterp)
{
    return js_ContextIterator(rt, JS_TRUE, iterp);
}

JS_PUBLIC_API(JSVersion)
JS_GetVersion(JSContext *cx)
{
    return cx->version & JSVERSION_MASK;
}

JS_PUBLIC_API(JSVersion)
JS_SetVersion(JSContext *cx, JSVersion version)
{
    JSVersion oldVersion;

    JS_ASSERT(version != JSVERSION_UNKNOWN);
    JS_ASSERT((version & ~JSVERSION_MASK) == 0);

    oldVersion = cx->version & JSVERSION_MASK;
    if (version == oldVersion)
        return oldVersion;

    cx->version = (cx->version & ~JSVERSION_MASK) | version;
    js_OnVersionChange(cx);
    return oldVersion;
}

static struct v2smap {
    JSVersion   version;
    const char  *string;
} v2smap[] = {
    {JSVERSION_1_0,     "1.0"},
    {JSVERSION_1_1,     "1.1"},
    {JSVERSION_1_2,     "1.2"},
    {JSVERSION_1_3,     "1.3"},
    {JSVERSION_1_4,     "1.4"},
    {JSVERSION_ECMA_3,  "ECMAv3"},
    {JSVERSION_1_5,     "1.5"},
    {JSVERSION_1_6,     "1.6"},
    {JSVERSION_DEFAULT, js_default_str},
    {JSVERSION_UNKNOWN, NULL},          /* must be last, NULL is sentinel */
};

JS_PUBLIC_API(const char *)
JS_VersionToString(JSVersion version)
{
    int i;

    for (i = 0; v2smap[i].string; i++)
        if (v2smap[i].version == version)
            return v2smap[i].string;
    return "unknown";
}

JS_PUBLIC_API(JSVersion)
JS_StringToVersion(const char *string)
{
    int i;

    for (i = 0; v2smap[i].string; i++)
        if (strcmp(v2smap[i].string, string) == 0)
            return v2smap[i].version;
    return JSVERSION_UNKNOWN;
}

JS_PUBLIC_API(uint32)
JS_GetOptions(JSContext *cx)
{
    return cx->options;
}

#define SYNC_OPTIONS_TO_VERSION(cx)                                           \
    JS_BEGIN_MACRO                                                            \
        if ((cx)->options & JSOPTION_XML)                                     \
            (cx)->version |= JSVERSION_HAS_XML;                               \
        else                                                                  \
            (cx)->version &= ~JSVERSION_HAS_XML;                              \
    JS_END_MACRO

JS_PUBLIC_API(uint32)
JS_SetOptions(JSContext *cx, uint32 options)
{
    uint32 oldopts = cx->options;
    cx->options = options;
    SYNC_OPTIONS_TO_VERSION(cx);
    return oldopts;
}

JS_PUBLIC_API(uint32)
JS_ToggleOptions(JSContext *cx, uint32 options)
{
    uint32 oldopts = cx->options;
    cx->options ^= options;
    SYNC_OPTIONS_TO_VERSION(cx);
    return oldopts;
}

JS_PUBLIC_API(const char *)
JS_GetImplementationVersion(void)
{
    return "JavaScript-C 1.6 2006-11-19";
}


JS_PUBLIC_API(JSObject *)
JS_GetGlobalObject(JSContext *cx)
{
    return cx->globalObject;
}

JS_PUBLIC_API(void)
JS_SetGlobalObject(JSContext *cx, JSObject *obj)
{
    cx->globalObject = obj;
#if JS_HAS_XML_SUPPORT
    cx->xmlSettingFlags = 0;
#endif
}

static JSObject *
InitFunctionAndObjectClasses(JSContext *cx, JSObject *obj)
{
    JSDHashTable *table;
    JSBool resolving;
    JSRuntime *rt;
    JSResolvingKey key;
    JSResolvingEntry *entry;
    JSObject *fun_proto, *obj_proto;

    /* If cx has no global object, use obj so prototypes can be found. */
    if (!cx->globalObject)
        JS_SetGlobalObject(cx, obj);

    /* Record Function and Object in cx->resolvingTable, if we are resolving. */
    table = cx->resolvingTable;
    resolving = (table && table->entryCount);
    if (resolving) {
        rt = cx->runtime;
        key.obj = obj;
        key.id = ATOM_TO_JSID(rt->atomState.FunctionAtom);
        entry = (JSResolvingEntry *)
                JS_DHashTableOperate(table, &key, JS_DHASH_ADD);
        if (entry && entry->key.obj && (entry->flags & JSRESFLAG_LOOKUP)) {
            /* Already resolving Function, record Object too. */
            JS_ASSERT(entry->key.obj == obj);
            key.id = ATOM_TO_JSID(rt->atomState.ObjectAtom);
            entry = (JSResolvingEntry *)
                    JS_DHashTableOperate(table, &key, JS_DHASH_ADD);
        }
        if (!entry) {
            JS_ReportOutOfMemory(cx);
            return NULL;
        }
        JS_ASSERT(!entry->key.obj && entry->flags == 0);
        entry->key = key;
        entry->flags = JSRESFLAG_LOOKUP;
    }

    /* Initialize the function class first so constructors can be made. */
    fun_proto = js_InitFunctionClass(cx, obj);
    if (!fun_proto)
        goto out;

    /* Initialize the object class next so Object.prototype works. */
    obj_proto = js_InitObjectClass(cx, obj);
    if (!obj_proto) {
        fun_proto = NULL;
        goto out;
    }

    /* Function.prototype and the global object delegate to Object.prototype. */
    OBJ_SET_PROTO(cx, fun_proto, obj_proto);
    if (!OBJ_GET_PROTO(cx, obj))
        OBJ_SET_PROTO(cx, obj, obj_proto);

out:
    /* If resolving, remove the other entry (Object or Function) from table. */
    if (resolving)
        JS_DHashTableOperate(table, &key, JS_DHASH_REMOVE);
    return fun_proto;
}

JS_PUBLIC_API(JSBool)
JS_InitStandardClasses(JSContext *cx, JSObject *obj)
{
    CHECK_REQUEST(cx);

#if JS_HAS_UNDEFINED
{
    /* Define a top-level property 'undefined' with the undefined value. */
    JSAtom *atom = cx->runtime->atomState.typeAtoms[JSTYPE_VOID];
    if (!OBJ_DEFINE_PROPERTY(cx, obj, ATOM_TO_JSID(atom), JSVAL_VOID,
                             NULL, NULL, JSPROP_PERMANENT, NULL)) {
        return JS_FALSE;
    }
}
#endif

    /* Function and Object require cooperative bootstrapping magic. */
    if (!InitFunctionAndObjectClasses(cx, obj))
        return JS_FALSE;

    /* Initialize the rest of the standard objects and functions. */
    return js_InitArrayClass(cx, obj) &&
           js_InitBooleanClass(cx, obj) &&
           js_InitMathClass(cx, obj) &&
           js_InitNumberClass(cx, obj) &&
           js_InitStringClass(cx, obj) &&
#if JS_HAS_CALL_OBJECT
           js_InitCallClass(cx, obj) &&
#endif
#if JS_HAS_REGEXPS
           js_InitRegExpClass(cx, obj) &&
#endif
#if JS_HAS_SCRIPT_OBJECT
           js_InitScriptClass(cx, obj) &&
#endif
#if JS_HAS_ERROR_EXCEPTIONS
           js_InitExceptionClasses(cx, obj) &&
#endif
#if JS_HAS_XML_SUPPORT
           js_InitXMLClasses(cx, obj) &&
#endif
#if JS_HAS_FILE_OBJECT
           js_InitFileClass(cx, obj) &&
#endif
           js_InitDateClass(cx, obj);
}

#define ATOM_OFFSET(name)       offsetof(JSAtomState, name##Atom)
#define OFFSET_TO_ATOM(rt,off)  (*(JSAtom **)((char*)&(rt)->atomState + (off)))

/*
 * Table of class initializers and their atom offsets in rt->atomState.
 * If you add a "standard" class, remember to update this table.
 */
static struct {
    JSObjectOp  init;
    size_t      atomOffset;
} standard_class_atoms[] = {
    {InitFunctionAndObjectClasses,  ATOM_OFFSET(Function)},
    {InitFunctionAndObjectClasses,  ATOM_OFFSET(Object)},
    {js_InitArrayClass,             ATOM_OFFSET(Array)},
    {js_InitBooleanClass,   