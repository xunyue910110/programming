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

#ifndef jsapi_h___
#define jsapi_h___
/*
 * JavaScript API.
 */
#include <stddef.h>
#include <stdio.h>
#include "jspubtd.h"

JS_BEGIN_EXTERN_C

/*
 * Type tags stored in the low bits of a jsval.
 */
#define JSVAL_OBJECT            0x0     /* untagged reference to object */
#define JSVAL_INT               0x1     /* tagged 31-bit integer value */
#define JSVAL_DOUBLE            0x2     /* tagged reference to double */
#define JSVAL_STRING            0x4     /* tagged reference to string */
#define JSVAL_BOOLEAN           0x6     /* tagged boolean value */

/* Type tag bitfield length and derived macros. */
#define JSVAL_TAGBITS           3
#define JSVAL_TAGMASK           JS_BITMASK(JSVAL_TAGBITS)
#define JSVAL_TAG(v)            ((v) & JSVAL_TAGMASK)
#define JSVAL_SETTAG(v,t)       ((v) | (t))
#define JSVAL_CLRTAG(v)         ((v) & ~(jsval)JSVAL_TAGMASK)
#define JSVAL_ALIGN             JS_BIT(JSVAL_TAGBITS)

/* Predicates for type testing. */
#define JSVAL_IS_OBJECT(v)      (JSVAL_TAG(v) == JSVAL_OBJECT)
#define JSVAL_IS_NUMBER(v)      (JSVAL_IS_INT(v) || JSVAL_IS_DOUBLE(v))
#define JSVAL_IS_INT(v)         (((v) & JSVAL_INT) && (v) != JSVAL_VOID)
#define JSVAL_IS_DOUBLE(v)      (JSVAL_TAG(v) == JSVAL_DOUBLE)
#define JSVAL_IS_STRING(v)      (JSVAL_TAG(v) == JSVAL_STRING)
#define JSVAL_IS_BOOLEAN(v)     (JSVAL_TAG(v) == JSVAL_BOOLEAN)
#define JSVAL_IS_NULL(v)        ((v) == JSVAL_NULL)
#define JSVAL_IS_VOID(v)        ((v) == JSVAL_VOID)
#define JSVAL_IS_PRIMITIVE(v)   (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v))

/* Objects, strings, and doubles are GC'ed. */
#define JSVAL_IS_GCTHING(v)     (!((v) & JSVAL_INT) && !JSVAL_IS_BOOLEAN(v))
#define JSVAL_TO_GCTHING(v)     ((void *)JSVAL_CLRTAG(v))
#define JSVAL_TO_OBJECT(v)      ((JSObject *)JSVAL_TO_GCTHING(v))
#define JSVAL_TO_DOUBLE(v)      ((jsdouble *)JSVAL_TO_GCTHING(v))
#define JSVAL_TO_STRING(v)      ((JSString *)JSVAL_TO_GCTHING(v))
#define OBJECT_TO_JSVAL(obj)    ((jsval)(obj))
#define DOUBLE_TO_JSVAL(dp)     JSVAL_SETTAG((jsval)(dp), JSVAL_DOUBLE)
#define STRING_TO_JSVAL(str)    JSVAL_SETTAG((jsval)(str), JSVAL_STRING)

/* Lock and unlock the GC thing held by a jsval. */
#define JSVAL_LOCK(cx,v)        (JSVAL_IS_GCTHING(v)                          \
                                 ? JS_LockGCThing(cx, JSVAL_TO_GCTHING(v))    \
                                 : JS_TRUE)
#define JSVAL_UNLOCK(cx,v)      (JSVAL_IS_GCTHING(v)                          \
                                 ? JS_UnlockGCThing(cx, JSVAL_TO_GCTHING(v))  \
                                 : JS_TRUE)

/* Domain limits for the jsval int type. */
#define JSVAL_INT_BITS          31
#define JSVAL_INT_POW2(n)       ((jsval)1 << (n))
#define JSVAL_INT_MIN           ((jsval)1 - JSVAL_INT_POW2(30))
#define JSVAL_INT_MAX           (JSVAL_INT_POW2(30) - 1)
#define INT_FITS_IN_JSVAL(i)    ((jsuint)((i)+JSVAL_INT_MAX) <= 2*JSVAL_INT_MAX)
#define JSVAL_TO_INT(v)         ((jsint)(v) >> 1)
#define INT_TO_JSVAL(i)         (((jsval)(i) << 1) | JSVAL_INT)

/* Convert between boolean and jsval. */
#define JSVAL_TO_BOOLEAN(v)     ((JSBool)((v) >> JSVAL_TAGBITS))
#define BOOLEAN_TO_JSVAL(b)     JSVAL_SETTAG((jsval)(b) << JSVAL_TAGBITS,     \
                                             JSVAL_BOOLEAN)

/* A private data pointer (2-byte-aligned) can be stored as an int jsval. */
#define JSVAL_TO_PRIVATE(v)     ((void *)((v) & ~JSVAL_INT))
#define PRIVATE_TO_JSVAL(p)     ((jsval)(p) | JSVAL_INT)

/* Property attributes, set in JSPropertySpec and passed to API functions. */
#define JSPROP_ENUMERATE        0x01    /* property is visible to for/in loop */
#define JSPROP_READONLY         0x02    /* not settable: assignment is no-op */
#define JSPROP_PERMANENT        0x04    /* property cannot be deleted */
#define JSPROP_EXPORTED         0x08    /* property is exported from object */
#define JSPROP_GETTER           0x10    /* property holds getter function */
#define JSPROP_SETTER           0x20    /* property holds setter function */
#define JSPROP_SHARED           0x40    /* don't allocate a value slot for this
                                           property; don't copy the property on
                                           set of the same-named property in an
                                           object that delegates to a prototype
                                           containing this property */
#define JSPROP_INDEX            0x80    /* name is actually (jsint) index */

/* Function flags, set in JSFunctionSpec and passed to JS_NewFunction etc. */
#define JSFUN_LAMBDA            0x08    /* expressed, not declared, function */
#define JSFUN_GETTER            JSPROP_GETTER
#define JSFUN_SETTER            JSPROP_SETTER
#define JSFUN_BOUND_METHOD      0x40    /* bind this to fun->object's parent */
#define JSFUN_HEAVYWEIGHT       0x80    /* activation requires a Call object */
#define JSFUN_FLAGS_MASK        0xf8    /* overlay JSFUN_* attributes */

/*
 * Well-known JS values.  The extern'd variables are initialized when the
 * first JSContext is created by JS_NewContext (see below).
 */
#define JSVAL_VOID              INT_TO_JSVAL(0 - JSVAL_INT_POW2(30))
#define JSVAL_NULL              OBJECT_TO_JSVAL(0)
#define JSVAL_ZERO              INT_TO_JSVAL(0)
#define JSVAL_ONE               INT_TO_JSVAL(1)
#define JSVAL_FALSE             BOOLEAN_TO_JSVAL(JS_FALSE)
#define JSVAL_TRUE              BOOLEAN_TO_JSVAL(JS_TRUE)

/*
 * Microseconds since the epoch, midnight, January 1, 1970 UTC.  See the
 * comment in jstypes.h regarding safe int64 usage.
 */
extern JS_PUBLIC_API(int64)
JS_Now();

/* Don't want to export data, so provide accessors for non-inline jsvals. */
extern JS_PUBLIC_API(jsval)
JS_GetNaNValue(JSContext *cx);

extern JS_PUBLIC_API(jsval)
JS_GetNegativeInfinityValue(JSContext *cx);

extern JS_PUBLIC_API(jsval)
JS_GetPositiveInfinityValue(JSContext *cx);

extern JS_PUBLIC_API(jsval)
JS_GetEmptyStringValue(JSContext *cx);

/*
 * Format is a string of the following characters (spaces are insignificant),
 * specifying the tabulated type conversions:
 *
 *   b      JSBool          Boolean
 *   c      uint16/jschar   ECMA uint16, Unicode char
 *   i      int32           ECMA int32
 *   u      uint32          ECMA uint32
 *   j      int32           Rounded int32 (coordinate)
 *   d      jsdouble        IEEE double
 *   I      jsdouble        Integral IEEE double
 *   s      char *          C string
 *   S      JSString *      Unicode string, accessed by a JSString pointer
 *   W      jschar *        Unicode character vector, 0-terminated (W for wide)
 *   o      JSObject *      Object reference
 *   f      JSFunction *    Function private
 *   v      jsval           Argument value (no conversion)
 *   *      N/A             Skip this argument (no vararg)
 *   /      N/A             End of required arguments
 *
 * The variable argument list after format must consist of &b, &c, &s, e.g.,
 * where those variables have the types given above.  For the pointer types
 * char *, JSString *, and JSObject *, the pointed-at memory returned belongs
 * to the JS runtime, not to the calling native code.  The runtime promises
 * to keep this memory valid so long as argv refers to allocated stack space
 * (so long as the native function is active).
 *
 * Fewer arguments than format specifies may be passed only if there is a /
 * in format after the last required argument specifier and argc is at least
 * the number of required arguments.  More arguments than format specifies
 * may be passed without error; it is up to the caller to deal with trailing
 * unconverted arguments.
 */
extern JS_PUBLIC_API(JSBool)
JS_ConvertArguments(JSContext *cx, uintN argc, jsval *argv, const char *format,
                    ...);

#ifdef va_start
extern JS_PUBLIC_API(JSBool)
JS_ConvertArgumentsVA(JSContext *cx, uintN argc, jsval *argv,
                      const char *format, va_list ap);
#endif

/*
 * Inverse of JS_ConvertArguments: scan format and convert trailing arguments
 * into jsvals, GC-rooted if necessary by the JS stack.  Return null on error,
 * and a pointer to the new argument vector on success.  Also return a stack
 * mark on success via *markp, in which case the caller must eventually clean
 * up by calling JS_PopArguments.
 *
 * Note that the number of actual arguments supplied is specified exclusively
 * by format, so there is no argc parameter.
 */
extern JS_PUBLIC_API(jsval *)
JS_PushArguments(JSContext *cx, void **markp, const char *format, ...);

#ifdef va_start
extern JS_PUBLIC_API(jsval *)
JS_PushArgumentsVA(JSContext *cx, void **markp, const char *format, va_list ap);
#endif

extern JS_PUBLIC_API(void)
JS_PopArguments(JSContext *cx, void *mark);

#ifdef JS_ARGUMENT_FORMATTER_DEFINED

/*
 * Add and remove a format string handler for JS_{Convert,Push}Arguments{,VA}.
 * The handler function has this signature (see jspubtd.h):
 *
 *   JSBool MyArgumentFormatter(JSContext *cx, const char *format,
 *                              JSBool fromJS, jsval **vpp, va_list *app);
 *
 * It should return true on success, and return false after reporting an error
 * or detecting an already-reported error.
 *
 * For a given format string, for example "AA", the formatter is called from
 * JS_ConvertArgumentsVA like so:
 *
 *   formatter(cx, "AA...", JS_TRUE, &sp, &ap);
 *
 * sp points into the arguments array on the JS stack, while ap points into
 * the stdarg.h va_list on the C stack.  The JS_TRUE passed for fromJS tells
 * the formatter to convert zero or more jsvals at sp to zero or more C values
 * accessed via pointers-to-values at ap, updating both sp (via *vpp) and ap
 * (via *app) to point past the converted arguments and their result pointers
 * on the C stack.
 *
 * When called from JS_PushArgumentsVA, the formatter is invoked thus:
 *
 *   formatter(cx, "AA...", JS_FALSE, &sp, &ap);
 *
 * where JS_FALSE for fromJS means to wrap the C values at ap according to the
 * format specifier and store them at sp, updating ap and sp appropriately.
 *
 * The "..." after "AA" is the rest of the format string that was passed into
 * JS_{Convert,Push}Arguments{,VA}.  The actual format trailing substring used
 * in each Convert or PushArguments call is passed to the formatter, so that
 * one such function may implement several formats, in order to share code.
 *
 * Remove just forgets about any handler associated with format.  Add does not
 * copy format, it points at the string storage allocated by the caller, which
 * is typically a string constant.  If format is in dynamic storage, it is up
 * to the caller to keep the string alive until Remove is called.
 */
extern JS_PUBLIC_API(JSBool)
JS_AddArgumentFormatter(JSContext *cx, const char *format,
                        JSArgumentFormatter formatter);

extern JS_PUBLIC_API(void)
JS_RemoveArgumentFormatter(JSContext *cx, const char *format);

#endif /* JS_ARGUMENT_FORMATTER_DEFINED */

extern JS_PUBLIC_API(JSBool)
JS_ConvertValue(JSContext *cx, jsval v, JSType type, jsval *vp);

extern JS_PUBLIC_API(JSBool)
JS_ValueToObject(JSContext *cx, jsval v, JSObject **objp);

extern JS_PUBLIC_API(JSFunction *)
JS_ValueToFunction(JSContext *cx, jsval v);

extern JS_PUBLIC_API(JSFunction *)
JS_ValueToConstructor(JSContext *cx, jsval v);

extern JS_PUBLIC_API(JSString *)
JS_ValueToString(JSContext *cx, jsval v);

extern JS_PUBLIC_API(JSBool)
JS_ValueToNumber(JSContext *cx, jsval v, jsdouble *dp);

/*
 * Convert a value to a number, then to an int32, according to the ECMA rules
 * for ToInt32.
 */
extern JS_PUBLIC_API(JSBool)
JS_ValueToECMAInt32(JSContext *cx, jsval v, int32 *ip);

/*
 * Convert a value to a number, then to a uint32, according to the ECMA rules
 * for ToUint32.
 */
extern JS_PUBLIC_API(JSBool)
JS_ValueToECMAUint32(JSContext *cx, jsval v, uint32 *ip);

/*
 * Convert a value to a number, then to an int32 if it fits by rounding to
 * nearest; but failing with an error report if the double is out of range
 * or unordered.
 */
extern JS_PUBLIC_API(JSBool)
JS_ValueToInt32(JSContext *cx, jsval v, int32 *ip);

/*
 * ECMA ToUint16, for mapping a jsval to a Unicode point.
 */
extern JS_PUBLIC_API(JSBool)
JS_ValueToUint16(JSContext *cx, jsval v, uint16 *ip);

extern JS_PUBLIC_API(JSBool)
JS_ValueToBoolean(JSContext *cx, jsval v, JSBool *bp);

extern JS_PUBLIC_API(JSType)
JS_TypeOfValue(JSContext *cx, jsval v);

extern JS_PUBLIC_API(const char *)
JS_GetTypeName(JSContext *cx, JSType type);

/************************************************************************/

/*
 * Initialization, locking, contexts, and memory allocation.
 */
#define JS_NewRuntime       JS_Init
#define JS_DestroyRuntime   JS_Finish
#define JS_LockRuntime      JS_Lock
#define JS_UnlockRuntime    JS_Unlock

extern JS_PUBLIC_API(JSRuntime *)
JS_NewRuntime(uint32 maxbytes);

extern JS_PUBLIC_API(void)
JS_DestroyRuntime(JSRuntime *rt);

extern JS_PUBLIC_API(void)
JS_ShutDown(void);

JS_PUBLIC_API(void *)
JS_GetRuntimePrivate(JSRuntime *rt);

JS_PUBLIC_API(void)
JS_SetRuntimePrivate(JSRuntime *rt, void *data);

#ifdef JS_THREADSAFE

extern JS_PUBLIC_API(void)
JS_BeginRequest(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_EndRequest(JSContext *cx);

/* Yield to pending GC operations, regardless of request depth */
extern JS_PUBLIC_API(void)
JS_YieldRequest(JSContext *cx);

extern JS_PUBLIC_API(jsrefcount)
JS_SuspendRequest(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_ResumeRequest(JSContext *cx, jsrefcount saveDepth);

#endif /* JS_THREADSAFE */

extern JS_PUBLIC_API(void)
JS_Lock(JSRuntime *rt);

extern JS_PUBLIC_API(void)
JS_Unlock(JSRuntime *rt);

extern JS_PUBLIC_API(JSContext *)
JS_NewContext(JSRuntime *rt, size_t stackChunkSize);

extern JS_PUBLIC_API(void)
JS_DestroyContext(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_DestroyContextNoGC(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_DestroyContextMaybeGC(JSContext *cx);

extern JS_PUBLIC_API(void *)
JS_GetContextPrivate(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_SetContextPrivate(JSContext *cx, void *data);

extern JS_PUBLIC_API(JSRuntime *)
JS_GetRuntime(JSContext *cx);

extern JS_PUBLIC_API(JSContext *)
JS_ContextIterator(JSRuntime *rt, JSContext **iterp);

extern JS_PUBLIC_API(JSVersion)
JS_GetVersion(JSContext *cx);

extern JS_PUBLIC_API(JSVersion)
JS_SetVersion(JSContext *cx, JSVersion version);

extern JS_PUBLIC_API(const char *)
JS_VersionToString(JSVersion version);

extern JS_PUBLIC_API(JSVersion)
JS_StringToVersion(const char *string);

/*
 * JS options are orthogonal to version, and may be freely composed with one
 * another as well as with version.
 *
 * JSOPTION_VAROBJFIX is recommended -- see the comments associated with the
 * prototypes for JS_ExecuteScript, JS_EvaluateScript, etc.
 */
#define JSOPTION_STRICT         JS_BIT(0)       /* warn on dubious practice */
#define JSOPTION_WERROR         JS_BIT(1)       /* convert warning to error */
#define JSOPTION_VAROBJFIX      JS_BIT(2)       /* make JS_EvaluateScript use
                                                   the last object on its 'obj'
                                                   param's scope chain as the
                                                   ECMA 'variables object' */
#define JSOPTION_PRIVATE_IS_NSISUPPORTS \
                                JS_BIT(3)       /* context private data points
                                                   to an nsISupports subclass */
#define JSOPTION_COMPILE_N_GO   JS_BIT(4)       /* caller of JS_Compile*Script
                                                   promises to execute compiled
                                                   script once only; enables
                                                   compile-time scope chain
                                                   resolution of consts. */
#define JSOPTION_ATLINE         JS_BIT(5)       /* //@line number ["filename"]
                                                   option supported for the
                                                   XUL preprocessor and kindred
                                                   beasts. */

extern JS_PUBLIC_API(uint32)
JS_GetOptions(JSContext *cx);

extern JS_PUBLIC_API(uint32)
JS_SetOptions(JSContext *cx, uint32 options);

extern JS_PUBLIC_API(uint32)
JS_ToggleOptions(JSContext *cx, uint32 options);

extern JS_PUBLIC_API(const char *)
JS_GetImplementationVersion(void);

extern JS_PUBLIC_API(JSObject *)
JS_GetGlobalObject(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_SetGlobalObject(JSContext *cx, JSObject *obj);

/*
 * Initialize standard JS class constructors, prototypes, and any top-level
 * functions and constants associated with the standard classes (e.g. isNaN
 * for Number).
 *
 * NB: This sets cx's global object to obj if it was null.
 */
extern JS_PUBLIC_API(JSBool)
JS_InitStandardClasses(JSContext *cx, JSObject *obj);

/*
 * Resolve id, which must contain either a string or an int, to a standard
 * class name in obj if possible, defining the class's constructor and/or
 * prototype and storing true in *resolved.  If id does not name a standard
 * class or a top-level property induced by initializing a standard class,
 * store false in *resolved and just return true.  Return false on error,
 * as usual for JSBool result-typed API entry points.
 *
 * This API can be called directly from a global object class's resolve op,
 * to define standard classes lazily.  The class's enumerate op should call
 * JS_EnumerateStandardClasses(cx, obj), to define eagerly during for..in
 * loops any classes not yet resolved lazily.
 */
extern JS_PUBLIC_API(JSBool)
JS_ResolveStandardClass(JSContext *cx, JSObject *obj, jsval id,
                        JSBool *resolved);

extern JS_PUBLIC_API(JSBool)
JS_EnumerateStandardClasses(JSContext *cx, JSObject *obj);

extern JS_PUBLIC_API(JSObject *)
JS_GetScopeChain(JSContext *cx);

extern JS_PUBLIC_API(void *)
JS_malloc(JSContext *cx, size_t nbytes);

extern JS_PUBLIC_API(void *)
JS_realloc(JSContext *cx, void *p, size_t nbytes);

extern JS_PUBLIC_API(void)
JS_free(JSContext *cx, void *p);

extern JS_PUBLIC_API(char *)
JS_strdup(JSContext *cx, const char *s);

extern JS_PUBLIC_API(jsdouble *)
JS_NewDouble(JSContext *cx, jsdouble d);

extern JS_PUBLIC_API(JSBool)
JS_NewDoubleValue(JSContext *cx, jsdouble d, jsval *rval);

extern JS_PUBLIC_API(JSBool)
JS_NewNumberValue(JSContext *cx, jsdouble d, jsval *rval);

/*
 * A JS GC root is a pointer to a JSObject *, JSString *, or jsdouble * that
 * itself points into the GC heap (more recently, we support this extension:
 * a root may be a pointer to a jsval v for which JSVAL_IS_GCTHING(v) is true).
 *
 * Therefore, you never pass JSObject *obj to JS_AddRoot(cx, obj).  You always
 * call JS_AddRoot(cx, &obj), passing obj by reference.  And later, before obj
 * or the structure it is embedded within goes out of scope or is freed, you
 * must call JS_RemoveRoot(cx, &obj).
 *
 * Also, use JS_AddNamedRoot(cx, &structPtr->memberObj, "structPtr->memberObj")
 * in preference to JS_AddRoot(cx, &structPtr->memberObj), in order to identify
 * roots by their source callsites.  This way, you can find the callsite while
 * debugging if you should fail to do JS_RemoveRoot(cx, &structPtr->memberObj)
 * before freeing structPtr's memory.
 */
extern JS_PUBLIC_API(JSBool)
JS_AddRoot(JSContext *cx, void *rp);

#ifdef NAME_ALL_GC_ROOTS
#define JS_DEFINE_TO_TOKEN(def) #def
#define JS_DEFINE_TO_STRING(def) JS_DEFINE_TO_TOKEN(def)
#define JS_AddRoot(cx,rp) JS_AddNamedRoot((cx), (rp), (__FILE__ ":" JS_TOKEN_TO_STRING(__LINE__))
#endif

extern JS_PUBLIC_API(JSBool)
JS_AddNamedRoot(JSContext *cx, void *rp, const char *name);

extern JS_PUBLIC_API(JSBool)
JS_AddNamedRootRT(JSRuntime *rt, void *rp, const char *name);

extern JS_PUBLIC_API(JSBool)
JS_RemoveRoot(JSContext *cx, void *rp);

extern JS_PUBLIC_API(JSBool)
JS_RemoveRootRT(JSRuntime *rt, void *rp);

/*
 * The last GC thing of each type (object, string, double, external string
 * types) created on a given context is kept alive until another thing of the
 * same type is created, using a newborn root in the context.  These newborn
 * roots help native code protect newly-created GC-things from GC invocations
 * activated before those things can be rooted using local or global roots.
 *
 * However, the newborn roots can also entrain great gobs of garbage, so the
 * JS_GC entry point clears them for the context on which GC is being forced.
 * Embeddings may need to do likewise for all contexts.
 *
 * See the scoped local root API immediately below for a better way to manage
 * newborns in cases where native hooks (functions, getters, setters, etc.)
 * create many GC-things, potentially without connecting them to predefined
 * local roots such as *rval or argv[i] in an active native function.  Using
 * JS_EnterLocalRootScope disables updating of the context's per-gc-thing-type
 * newborn roots, until control flow unwinds and leaves the outermost nesting
 * local root scope.
 */
extern JS_PUBLIC_API(void)
JS_ClearNewbornRoots(JSContext *cx);

/*
 * Scoped local root management allows native functions, getter/setters, etc.
 * to avoid worrying about the newborn root pigeon-holes, overloading local
 * roots allocated in argv and *rval, or ending up having to call JS_Add*Root
 * and JS_RemoveRoot to manage global roots temporarily.
 *
 * Instead, calling JS_EnterLocalRootScope and JS_LeaveLocalRootScope around
 * the body of the native hook causes the engine to allocate a local root for
 * each newborn created in between the two API calls, using a local root stack
 * associated with cx.  For example:
 *
 *    JSBool
 *    my_GetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
 *    {
 *        JSBool ok;
 *
 *        if (!JS_EnterLocalRootScope(cx))
 *            return JS_FALSE;
 *        ok = my_GetPropertyBody(cx, obj, id, vp);
 *        JS_LeaveLocalRootScope(cx);
 *        return ok;
 *    }
 *
 * NB: JS_LeaveLocalRootScope must be called once for every prior successful
 * call to JS_EnterLocalRootScope.  If JS_EnterLocalRootScope fails, you must
 * not make the matching JS_LeaveLocalRootScope call.
 *
 * In case a native hook allocates many objects or other GC-things, but the
 * native protects some of those GC-things by storing them as property values
 * in an object that is itself protected, the hook can call JS_ForgetLocalRoot
 * to free the local root automatically pushed for the now-protected GC-thing.
 *
 * JS_ForgetLocalRoot works on any GC-thing allocated in the current local
 * root scope, but it's more time-efficient when called on references to more
 * recently created GC-things.  Calling it successively on other than the most
 * recently allocated GC-thing will tend to average the time inefficiency, and
 * may risk O(n^2) growth rate, but in any event, you shouldn't allocate too
 * many local roots if you can root as you go (build a tree of objects from
 * the top down, forgetting each latest-allocated GC-thing immediately upon
 * linking it to its parent).
 */
extern JS_PUBLIC_API(JSBool)
JS_EnterLocalRootScope(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_LeaveLocalRootScope(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_ForgetLocalRoot(JSContext *cx, void *thing);

#ifdef DEBUG
extern JS_PUBLIC_API(void)
JS_DumpNamedRoots(JSRuntime *rt,
                  void (*dump)(const char *name, void *rp, void *data),
                  void *data);
#endif

/*
 * Call JS_MapGCRoots to map the GC's roots table using map(rp, name, data).
 * The root is pointed at by rp; if the root is unnamed, name is null; data is
 * supplied from the third parameter to JS_MapGCRoots.
 *
 * The map function should return JS_MAP_GCROOT_REMOVE to cause the currently
 * enumerated root to be removed.  To stop enumeration, set JS_MAP_GCROOT_STOP
 * in the return value.  To keep on mapping, return JS_MAP_GCROOT_NEXT.  These
 * constants are flags; you can OR them together.
 *
 * This function acquires and releases rt's GC lock around the mapping of the
 * roots table, so the map function should run to completion in as few cycles
 * as possible.  Of course, map cannot call JS_GC, JS_MaybeGC, JS_BeginRequest,
 * or any JS API entry point that acquires locks, without double-tripping or
 * deadlocking on the GC lock.
 *
 * JS_MapGCRoots returns the count of roots that were successfully mapped.
 */
#define JS_MAP_GCROOT_NEXT      0       /* continue mapping entries */
#define JS_MAP_GCROOT_STOP      1       /* stop mapping entries */
#define JS_MAP_GCROOT_REMOVE    2       /* remove and free the current entry */

typedef intN
(* JS_DLL_CALLBACK JSGCRootMapFun)(void *rp, const char *name, void *data);

extern JS_PUBLIC_API(uint32)
JS_MapGCRoots(JSRuntime *rt, JSGCRootMapFun map, void *data);

extern JS_PUBLIC_API(JSBool)
JS_LockGCThing(JSContext *cx, void *thing);

extern JS_PUBLIC_API(JSBool)
JS_LockGCThingRT(JSRuntime *rt, void *thing);

extern JS_PUBLIC_API(JSBool)
JS_UnlockGCThing(JSContext *cx, void *thing);

extern JS_PUBLIC_API(JSBool)
JS_UnlockGCThingRT(JSRuntime *rt, void *thing);

/*
 * For implementors of JSObjectOps.mark, to mark a GC-thing reachable via a
 * property or other strong ref identified for debugging purposes by name.
 * The name argument's storage needs to live only as long as the call to
 * this routine.
 *
 * The final arg is used by GC_MARK_DEBUG code to build a ref path through
 * the GC's live thing graph.  Implementors of JSObjectOps.mark should pass
 * its final arg through to this function when marking all GC-things that are
 * directly reachable from the object being marked.
 *
 * See the JSMarkOp typedef in jspubtd.h, and the JSObjectOps struct below.
 */
extern JS_PUBLIC_API(void)
JS_MarkGCThing(JSContext *cx, void *thing, const char *name, void *arg);

extern JS_PUBLIC_API(void)
JS_GC(JSContext *cx);

extern JS_PUBLIC_API(void)
JS_MaybeGC(JSContext *cx);

extern JS_PUBLIC_API(JSGCCallback)
JS_SetGCCallback(JSContext *cx, JSGCCallback cb);

extern JS_PUBLIC_API(JSGCCallback)
JS_SetGCCallbackRT(JSRuntime *rt, JSGCCallback cb);

extern JS_PUBLIC_API(JSBool)
JS_IsAboutToBeFinalized(JSContext *cx, void *thing);

/*
 * Add an external string finalizer, one created by JS_NewExternalString (see
 * below) using a type-code returned from this function, and that understands
 * how to free or release the memory pointed at by JS_GetStringChars(str).
 *
 * Return a nonnegative type index if there is room for finalizer in the
 * global GC finalizers table, else return -1.  If the engine is compiled
 * JS_THREADSAFE and used in a multi-threaded environment, this function must
 * be invoked on the primordial thread only, at startup -- or else the entire
 * program must single-thread itself while loading a module that calls this
 * function.
 */
extern JS_PUBLIC_API(intN)
JS_AddExternalStringFinalizer(JSStringFinalizeOp finalizer);

/*
 * Remove finalizer from the global GC finalizers table, returning its type
 * code if found, -1 if not found.
 *
 * As with JS_AddExternalStringFinalizer, there is a threading restriction
 * if you compile the engine JS_THREADSAFE: this function may be called for a
 * given finalizer pointer on only one thread; different threads may call to
 * remove distinct finalizers safely.
 *
 * You must ensure that all strings with finalizer's type have been collected
 * before calling this function.  Otherwise, string data will be leaked by the
 * GC, for want of a finalizer to call.
 */
extern JS_PUBLIC_API(intN)
JS_RemoveExternalStringFinalizer(JSStringFinalizeOp finalizer);

/*
 * Create a new JSString whose chars member refers to external memory, i.e.,
 * memory requiring special, type-specific finalization.  The type code must
 * be a nonnegative return value from JS_AddExternalStringFinalizer.
 */
extern JS_PUBLIC_API(JSString *)
JS_NewExternalString(JSContext *cx, jschar *chars, size_t length, intN type);

/*
 * Returns the external-string finalizer index for this string, or -1 if it is
 * an "internal" (native to JS engine) string.
 */
extern JS_PUBLIC_API(intN)
JS_GetExternalStringGCType(JSRuntime *rt, JSString *str);

/*
 * Sets maximum (if stack grows upward) or minimum (downward) legal stack byte
 * address in limitAddr for the thread or process stack used by cx.  To disable
 * stack size checking, pass 0 for limitAddr.
 */
extern JS_PUBLIC_API(void)
JS_SetThreadStackLimit(JSContext *cx, jsuword limitAddr);

/************************************************************************/

/*
 * Classes, objects, and properties.
 */

/* For detailed comments on the function pointer types, see jspubtd.h. */
struct JSClass {
    const char          *name;
    uint32              flags;

    /* Mandatory non-null function pointer members. */
    JSPropertyOp        addProperty;
    JSPropertyOp        delProperty;
    JSPropertyOp        getProperty;
    JSPropertyOp        setProperty;
    JSEnumerateOp       enumerate;
    JSResolveOp         resolve;
    JSConvertOp         convert;
    JSFinalizeOp        finalize;

    /* Optionally non-null members start here. */
    JSGetObjectOps      getObjectOps;
    JSCheckAccessOp     checkAccess;
    JSNative            call;
    JSNative            construct;
    JSXDRObjectOp       xdrObject;
    JSHasInstanceOp     hasInstance;
    JSMarkOp            mark;
    JSReserveSlotsOp    reserveSlots;
};

#define JSCLASS_HAS_PRIVATE             (1<<0)  /* objects have private slot */
#define JSCLASS_NEW_ENUMERATE           (1<<1)  /* has JSNewEnumerateOp hook */
#define JSCLASS_NEW_RESOLVE             (1<<2)  /* has JSNewResolveOp hook */
#define JSCLASS_PRIVATE_IS_NSISUPPORTS  (1<<3)  /* private is (nsISupports *) */
#define JSCLASS_SHARE_ALL_PROPERTIES    (1<<4)  /* all properties are SHARED */
#define JSCLASS_NEW_RESOLVE_GETS_START  (1<<5)  /* JSNewResolveOp gets starting
                                                   object in prototype chain
                                                   passed in via *objp in/out
                                                   parameter */

/*
 * To reserve slots fetched and stored via JS_Get/SetReservedSlot, bitwise-or
 * JSCLASS_HAS_RESERVED_SLOTS(n) into the initializer for JSClass.flags, where
 * n is a constant in [1, 255].  Reserved slots are indexed from 0 to n-1.
 */
#define JSCLASS_RESERVED_SLOTS_SHIFT    8       /* room for 8 flags below */
#define JSCLASS_RESERVED_SLOTS_WIDTH    8       /* and 16 above this field */
#define JSCLASS_RESERVED_SLOTS_MASK     JS_BITMASK(JSCLASS_RESERVED_SLOTS_WIDTH)
#define JSCLASS_HAS_RESERVED_SLOTS(n)   (((n) & JSCLASS_RESERVED_SLOTS_MASK)  \
                                         << JSCLASS_RESERVED_SLOTS_SHIFT)
#define JSCLASS_RESERVED_SLOTS(clasp)   (((clasp)->flags                      \
                                          >> JSCLASS_RESERVED_SLOTS_SHIFT)    \
                                         & JSCLASS_RESERVED_SLOTS_MASK)

/* Initializer for unused members of stati