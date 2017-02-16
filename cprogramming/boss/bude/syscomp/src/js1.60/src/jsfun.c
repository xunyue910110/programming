/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sw=4 et tw=78:
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
 * JS function support.
 */
#include "jsstddef.h"
#include <string.h>
#include "jstypes.h"
#include "jsbit.h"
#include "jsutil.h" /* Added by JSIFY */
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsdbgapi.h"
#include "jsfun.h"
#include "jsgc.h"
#include "jsinterp.h"
#include "jslock.h"
#include "jsnum.h"
#include "jsobj.h"
#include "jsopcode.h"
#include "jsparse.h"
#include "jsscan.h"
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"
#include "jsexn.h"

/* Generic function/call/arguments tinyids -- also reflected bit numbers. */
enum {
    CALL_ARGUMENTS  = -1,       /* predefined arguments local variable */
    CALL_CALLEE     = -2,       /* reference to active function's object */
    ARGS_LENGTH     = -3,       /* number of actual args, arity if inactive */
    ARGS_CALLEE     = -4,       /* reference from arguments to active funobj */
    FUN_ARITY       = -5,       /* number of formal parameters; desired argc */
    FUN_NAME        = -6,       /* function name, "" if anonymous */
    FUN_CALLER      = -7        /* Function.prototype.caller, backward compat */
};

#if JSFRAME_OVERRIDE_BITS < 8
# error "not enough override bits in JSStackFrame.flags!"
#endif

#define TEST_OVERRIDE_BIT(fp, tinyid) \
    ((fp)->flags & JS_BIT(JSFRAME_OVERRIDE_SHIFT - ((tinyid) + 1)))

#define SET_OVERRIDE_BIT(fp, tinyid) \
    ((fp)->flags |= JS_BIT(JSFRAME_OVERRIDE_SHIFT - ((tinyid) + 1)))

#if JS_HAS_ARGS_OBJECT

JSBool
js_GetArgsValue(JSContext *cx, JSStackFrame *fp, jsval *vp)
{
    JSObject *argsobj;

    if (TEST_OVERRIDE_BIT(fp, CALL_ARGUMENTS)) {
        JS_ASSERT(fp->callobj);
        return OBJ_GET_PROPERTY(cx, fp->callobj,
                                ATOM_TO_JSID(cx->runtime->atomState
                                             .argumentsAtom),
                                vp);
    }
    argsobj = js_GetArgsObject(cx, fp);
    if (!argsobj)
        return JS_FALSE;
    *vp = OBJECT_TO_JSVAL(argsobj);
    return JS_TRUE;
}

static JSBool
MarkArgDeleted(JSContext *cx, JSStackFrame *fp, uintN slot)
{
    JSObject *argsobj;
    jsval bmapval, bmapint;
    size_t nbits, nbytes;
    jsbitmap *bitmap;

    argsobj = fp->argsobj;
    (void) JS_GetReservedSlot(cx, argsobj, 0, &bmapval);
    nbits = fp->argc;
    JS_ASSERT(slot < nbits);
    if (JSVAL_IS_VOID(bmapval)) {
        if (nbits <= JSVAL_INT_BITS) {
            bmapint = 0;
            bitmap = (jsbitmap *) &bmapint;
        } else {
            nbytes = JS_HOWMANY(nbits, JS_BITS_PER_WORD) * sizeof(jsbitmap);
            bitmap = (jsbitmap *) JS_malloc(cx, nbytes);
            if (!bitmap)
                return JS_FALSE;
            memset(bitmap, 0, nbytes);
            bmapval = PRIVATE_TO_JSVAL(bitmap);
            JS_SetReservedSlot(cx, argsobj, 0, bmapval);
        }
    } else {
        if (nbits <= JSVAL_INT_BITS) {
            bmapint = JSVAL_TO_INT(bmapval);
            bitmap = (jsbitmap *) &bmapint;
        } else {
            bitmap = (jsbitmap *) JSVAL_TO_PRIVATE(bmapval);
        }
    }
    JS_SET_BIT(bitmap, slot);
    if (bitmap == (jsbitmap *) &bmapint) {
        bmapval = INT_TO_JSVAL(bmapint);
        JS_SetReservedSlot(cx, argsobj, 0, bmapval);
    }
    return JS_TRUE;
}

/* NB: Infallible predicate, false does not mean error/exception. */
static JSBool
ArgWasDeleted(JSContext *cx, JSStackFrame *fp, uintN slot)
{
    JSObject *argsobj;
    jsval bmapval, bmapint;
    jsbitmap *bitmap;

    argsobj = fp->argsobj;
    (void) JS_GetReservedSlot(cx, argsobj, 0, &bmapval);
    if (JSVAL_IS_VOID(bmapval))
        return JS_FALSE;
    if (fp->argc <= JSVAL_INT_BITS) {
        bmapint = JSVAL_TO_INT(bmapval);
        bitmap = (jsbitmap *) &bmapint;
    } else {
        bitmap = (jsbitmap *) JSVAL_TO_PRIVATE(bmapval);
    }
    return JS_TEST_BIT(bitmap, slot) != 0;
}

JSBool
js_GetArgsProperty(JSContext *cx, JSStackFrame *fp, jsid id,
                   JSObject **objp, jsval *vp)
{
    jsval val;
    JSObject *obj;
    uintN slot;

    if (TEST_OVERRIDE_BIT(fp, CALL_ARGUMENTS)) {
        JS_ASSERT(fp->callobj);
        if (!OBJ_GET_PROPERTY(cx, fp->callobj,
                              ATOM_TO_JSID(cx->runtime->atomState
                                           .argumentsAtom),
                              &val)) {
            return JS_FALSE;
        }
        if (JSVAL_IS_PRIMITIVE(val)) {
            obj = js_ValueToNonNullObject(cx, val);
            if (!obj)
                return JS_FALSE;
        } else {
            obj = JSVAL_TO_OBJECT(val);
        }
        *objp = obj;
        return OBJ_GET_PROPERTY(cx, obj, id, vp);
    }

    *objp = NULL;
    *vp = JSVAL_VOID;
    if (JSID_IS_INT(id)) {
        slot = (uintN) JSID_TO_INT(id);
        if (slot < fp->argc) {
            if (fp->argsobj && ArgWasDeleted(cx, fp, slot))
                return OBJ_GET_PROPERTY(cx, fp->argsobj, id, vp);
            *vp = fp->argv[slot];
        } else {
            /*
             * Per ECMA-262 Ed. 3, 10.1.8, last bulleted item, do not share
             * storage between the formal parameter and arguments[k] for all
             * k >= fp->argc && k < fp->fun->nargs.  For example, in
             *
             *   function f(x) { x = 42; return arguments[0]; }
             *   f();
             *
             * the call to f should return undefined, not 42.  If fp->argsobj
             * is null at this point, as it would be in the example, return
             * undefined in *vp.
             */
            if (fp->argsobj)
                return OBJ_GET_PROPERTY(cx, fp->argsobj, id, vp);
        }
    } else {
        if (id == ATOM_TO_JSID(cx->runtime->atomState.lengthAtom)) {
            if (fp->argsobj && TEST_OVERRIDE_BIT(fp, ARGS_LENGTH))
                return OBJ_GET_PROPERTY(cx, fp->argsobj, id, vp);
            *vp = INT_TO_JSVAL((jsint) fp->argc);
        }
    }
    return JS_TRUE;
}

JSObject *
js_GetArgsObject(JSContext *cx, JSStackFrame *fp)
{
    JSObject *argsobj;

    /* Skip eval and debugger frames. */
    while (fp->flags & JSFRAME_SPECIAL)
        fp = fp->down;

    /* Create an arguments object for fp only if it lacks one. */
    argsobj = fp->argsobj;
    if (argsobj)
        return argsobj;

    /* Link the new object to fp so it can get actual argument values. */
    argsobj = js_NewObject(cx, &js_ArgumentsClass, NULL, NULL);
    if (!argsobj || !JS_SetPrivate(cx, argsobj, fp)) {
        cx->newborn[GCX_OBJECT] = NULL;
        return NULL;
    }
    fp->argsobj = argsobj;
    return argsobj;
}

static JSBool
args_enumerate(JSContext *cx, JSObject *obj);

JSBool
js_PutArgsObject(JSContext *cx, JSStackFrame *fp)
{
    JSObject *argsobj;
    jsval bmapval, rval;
    JSBool ok;
    JSRuntime *rt;

    /*
     * Reuse args_enumerate here to reflect fp's actual arguments as indexed
     * elements of argsobj.  Do this first, before clearing and freeing the
     * deleted argument slot bitmap, because args_enumerate depends on that.
     */
    argsobj = fp->argsobj;
    ok = args_enumerate(cx, argsobj);

    /*
     * Now clear the deleted argument number bitmap slot and free the bitmap,
     * if one was actually created due to 'delete arguments[0]' or similar.
     */
    (void) JS_GetReservedSlot(cx, argsobj, 0, &bmapval);
    if (!JSVAL_IS_VOID(bmapval)) {
        JS_SetReservedSlot(cx, argsobj, 0, JSVAL_VOID);
        if (fp->argc > JSVAL_INT_BITS)
            JS_free(cx, JSVAL_TO_PRIVATE(bmapval));
    }

    /*
     * Now get the prototype properties so we snapshot fp->fun and fp->argc
     * before fp goes away.
     */
    rt = cx->runtime;
    ok &= js_GetProperty(cx, argsobj, ATOM_TO_JSID(rt->atomState.calleeAtom),
                         &rval);
    ok &= js_SetProperty(cx, argsobj, ATOM_TO_JSID(rt->atomState.calleeAtom),
                         &rval);
    ok &= js_GetProperty(cx, argsobj, ATOM_TO_JSID(rt->atomState.lengthAtom),
                         &rval);
    ok &= js_SetProperty(cx, argsobj, ATOM_TO_JSID(rt->atomState.lengthAtom),
                         &rval);

    /*
     * Clear the private pointer to fp, which is about to go away (js_Invoke).
     * Do this last because the args_enumerate and js_GetProperty calls above
     * need to follow the private slot to find fp.
     */
    ok &= JS_SetPrivate(cx, argsobj, NULL);
    fp->argsobj = NULL;
    return ok;
}

static JSBool
args_delProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    jsint slot;
    JSStackFrame *fp;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;
    fp = (JSStackFrame *)
         JS_GetInstancePrivate(cx, obj, &js_ArgumentsClass, NULL);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->argsobj);

    slot = JSVAL_TO_INT(id);
    switch (slot) {
      case ARGS_CALLEE:
      case ARGS_LENGTH:
        SET_OVERRIDE_BIT(fp, slot);
        break;

      default:
        if ((uintN)slot < fp->argc && !MarkArgDeleted(cx, fp, slot))
            return JS_FALSE;
        break;
    }
    return JS_TRUE;
}

static JSBool
args_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    jsint slot;
    JSStackFrame *fp;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;
    fp = (JSStackFrame *)
         JS_GetInstancePrivate(cx, obj, &js_ArgumentsClass, NULL);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->argsobj);

    slot = JSVAL_TO_INT(id);
    switch (slot) {
      case ARGS_CALLEE:
        if (!TEST_OVERRIDE_BIT(fp, slot))
            *vp = fp->argv ? fp->argv[-2] : OBJECT_TO_JSVAL(fp->fun->object);
        break;

      case ARGS_LENGTH:
        if (!TEST_OVERRIDE_BIT(fp, slot))
            *vp = INT_TO_JSVAL((jsint)fp->argc);
        break;

      default:
        if ((uintN)slot < fp->argc && !ArgWasDeleted(cx, fp, slot))
            *vp = fp->argv[slot];
        break;
    }
    return JS_TRUE;
}

static JSBool
args_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSStackFrame *fp;
    jsint slot;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;
    fp = (JSStackFrame *)
         JS_GetInstancePrivate(cx, obj, &js_ArgumentsClass, NULL);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->argsobj);

    slot = JSVAL_TO_INT(id);
    switch (slot) {
      case ARGS_CALLEE:
      case ARGS_LENGTH:
        SET_OVERRIDE_BIT(fp, slot);
        break;

      default:
        if (fp->fun->interpreted &&
            (uintN)slot < fp->argc &&
            !ArgWasDeleted(cx, fp, slot)) {
            fp->argv[slot] = *vp;
        }
        break;
    }
    return JS_TRUE;
}

static JSBool
args_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags,
             JSObject **objp)
{
    JSStackFrame *fp;
    uintN slot;
    JSString *str;
    JSAtom *atom;
    intN tinyid;
    jsval value;

    *objp = NULL;
    fp = (JSStackFrame *)
         JS_GetInstancePrivate(cx, obj, &js_ArgumentsClass, NULL);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->argsobj);

    if (JSVAL_IS_INT(id)) {
        slot = JSVAL_TO_INT(id);
        if (slot < fp->argc && !ArgWasDeleted(cx, fp, slot)) {
            /* XXX ECMA specs DontEnum, contrary to other array-like objects */
            if (!js_DefineProperty(cx, obj, INT_JSVAL_TO_JSID(id),
                                   fp->argv[slot],
                                   args_getProperty, args_setProperty,
                                   JS_VERSION_IS_ECMA(cx)
                                   ? 0
                                   : JSPROP_ENUMERATE,
                                   NULL)) {
                return JS_FALSE;
            }
            *objp = obj;
        }
    } else {
        str = JSVAL_TO_STRING(id);
        atom = cx->runtime->atomState.lengthAtom;
        if (str == ATOM_TO_STRING(atom)) {
            tinyid = ARGS_LENGTH;
            value = INT_TO_JSVAL(fp->argc);
        } else {
            atom = cx->runtime->atomState.calleeAtom;
            if (str == ATOM_TO_STRING(atom)) {
                tinyid = ARGS_CALLEE;
                value = fp->argv ? fp->argv[-2]
                                 : OBJECT_TO_JSVAL(fp->fun->object);
            } else {
                atom = NULL;

                /* Quell GCC overwarnings. */
                tinyid = 0;
                value = JSVAL_NULL;
            }
        }

        if (atom && !TEST_OVERRIDE_BIT(fp, tinyid)) {
            if (!js_DefineNativeProperty(cx, obj, ATOM_TO_JSID(atom), value,
                                         args_getProperty, args_setProperty, 0,
                                         SPROP_HAS_SHORTID, tinyid, NULL)) {
                return JS_FALSE;
            }
            *objp = obj;
        }
    }

    return JS_TRUE;
}

static JSBool
args_enumerate(JSContext *cx, JSObject *obj)
{
    JSStackFrame *fp;
    JSObject *pobj;
    JSProperty *prop;
    uintN slot, argc;

    fp = (JSStackFrame *)
         JS_GetInstancePrivate(cx, obj, &js_ArgumentsClass, NULL);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->argsobj);

    /*
     * Trigger reflection with value snapshot in args_resolve using a series
     * of js_LookupProperty calls.  We handle length, callee, and the indexed
     * argument properties.  We know that args_resolve covers all these cases
     * and creates direct properties of obj, but that it may fail to resolve
     * length or callee if overridden.
     */
    if (!js_LookupProperty(cx, obj,
                           ATOM_TO_JSID(cx->runtime->atomState.lengthAtom),
                           &pobj, &prop)) {
        return JS_FALSE;
    }
    if (prop)
        OBJ_DROP_PROPERTY(cx, pobj, prop);

    if (!js_LookupProperty(cx, obj,
                           ATOM_TO_JSID(cx->runtime->atomState.calleeAtom),
                           &pobj, &prop)) {
        return JS_FALSE;
    }
    if (prop)
        OBJ_DROP_PROPERTY(cx, pobj, prop);

    argc = fp->argc;
    for (slot = 0; slot < argc; slot++) {
        if (!js_LookupProperty(cx, obj, INT_TO_JSID((jsint)slot), &pobj, &prop))
            return JS_FALSE;
        if (prop)
            OBJ_DROP_PROPERTY(cx, pobj, prop);
    }
    return JS_TRUE;
}

/*
 * The Arguments class is not initialized via JS_InitClass, and must not be,
 * because its name is "Object".  Per ECMA, that causes instances of it to
 * delegate to the object named by Object.prototype.  It also ensures that
 * arguments.toString() returns "[object Object]".
 *
 * The JSClass functions below collaborate to lazily reflect and synchronize
 * actual argument values, argument count, and callee function object stored
 * in a JSStackFrame with their corresponding property values in the frame's
 * arguments object.
 */
JSClass js_ArgumentsClass = {
    js_Object_str,
    JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE | JSCLASS_HAS_RESERVED_SLOTS(1),
    JS_PropertyStub,  args_delProperty,
    args_getProperty, args_setProperty,
    args_enumerate,   (JSResolveOp) args_resolve,
    JS_ConvertStub,   JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

#endif /* JS_HAS_ARGS_OBJECT */

#if JS_HAS_CALL_OBJECT

JSObject *
js_GetCallObject(JSContext *cx, JSStackFrame *fp, JSObject *parent)
{
    JSObject *callobj, *funobj;

    /* Create a call object for fp only if it lacks one. */
    JS_ASSERT(fp->fun);
    callobj = fp->callobj;
    if (callobj)
        return callobj;
    JS_ASSERT(fp->fun);

    /* The default call parent is its function's parent (static link). */
    if (!parent) {
        funobj = fp->argv ? JSVAL_TO_OBJECT(fp->argv[-2]) : fp->fun->object;
        if (funobj)
            parent = OBJ_GET_PARENT(cx, funobj);
    }

    /* Create the call object and link it to its stack frame. */
    callobj = js_NewObject(cx, &js_CallClass, NULL, parent);
    if (!callobj || !JS_SetPrivate(cx, callobj, fp)) {
        cx->newborn[GCX_OBJECT] = NULL;
        return NULL;
    }
    fp->callobj = callobj;

    /* Make callobj be the scope chain and the variables object. */
    fp->scopeChain = callobj;
    fp->varobj = callobj;
    return callobj;
}

static JSBool
call_enumerate(JSContext *cx, JSObject *obj);

JSBool
js_PutCallObject(JSContext *cx, JSStackFrame *fp)
{
    JSObject *callobj;
    JSBool ok;
    jsid argsid;
    jsval aval;

    /*
     * Reuse call_enumerate here to reflect all actual args and vars into the
     * call object from fp.
     */
    callobj = fp->callobj;
    if (!callobj)
        return JS_TRUE;
    ok = call_enumerate(cx, callobj);

    /*
     * Get the arguments object to snapshot fp's actual argument values.
     */
    if (fp->argsobj) {
        argsid = ATOM_TO_JSID(cx->runtime->atomState.argumentsAtom);
        ok &= js_GetProperty(cx, callobj, argsid, &aval);
        ok &= js_SetProperty(cx, callobj, argsid, &aval);
        ok &= js_PutArgsObject(cx, fp);
    }

    /*
     * Clear the private pointer to fp, which is about to go away (js_Invoke).
     * Do this last because the call_enumerate and js_GetProperty calls above
     * need to follow the private slot to find fp.
     */
    ok &= JS_SetPrivate(cx, callobj, NULL);
    fp->callobj = NULL;
    return ok;
}

static JSPropertySpec call_props[] = {
    {js_arguments_str,  CALL_ARGUMENTS, JSPROP_PERMANENT,0,0},
    {"__callee__",      CALL_CALLEE,    0,0,0},
    {0,0,0,0,0}
};

static JSBool
call_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSStackFrame *fp;
    jsint slot;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;
    fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->fun);

    slot = JSVAL_TO_INT(id);
    switch (slot) {
      case CALL_ARGUMENTS:
        if (!TEST_OVERRIDE_BIT(fp, slot)) {
            JSObject *argsobj = js_GetArgsObject(cx, fp);
            if (!argsobj)
                return JS_FALSE;
            *vp = OBJECT_TO_JSVAL(argsobj);
        }
        break;

      case CALL_CALLEE:
        if (!TEST_OVERRIDE_BIT(fp, slot))
            *vp = fp->argv ? fp->argv[-2] : OBJECT_TO_JSVAL(fp->fun->object);
        break;

      default:
        if ((uintN)slot < JS_MAX(fp->argc, fp->fun->nargs))
            *vp = fp->argv[slot];
        break;
    }
    return JS_TRUE;
}

static JSBool
call_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSStackFrame *fp;
    jsint slot;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;
    fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->fun);

    slot = JSVAL_TO_INT(id);
    switch (slot) {
      case CALL_ARGUMENTS:
      case CALL_CALLEE:
        SET_OVERRIDE_BIT(fp, slot);
        break;

      default:
        if ((uintN)slot < JS_MAX(fp->argc, fp->fun->nargs))
            fp->argv[slot] = *vp;
        break;
    }
    return JS_TRUE;
}

JSBool
js_GetCallVariable(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSStackFrame *fp;

    JS_ASSERT(JSVAL_IS_INT(id));
    fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
    if (fp) {
        /* XXX no jsint slot commoning here to avoid MSVC1.52 crashes */
        if ((uintN)JSVAL_TO_INT(id) < fp->nvars)
            *vp = fp->vars[JSVAL_TO_INT(id)];
    }
    return JS_TRUE;
}

JSBool
js_SetCallVariable(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSStackFrame *fp;

    JS_ASSERT(JSVAL_IS_INT(id));
    fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
    if (fp) {
        /* XXX jsint slot is block-local here to avoid MSVC1.52 crashes */
        jsint slot = JSVAL_TO_INT(id);
        if ((uintN)slot < fp->nvars)
            fp->vars[slot] = *vp;
    }
    return JS_TRUE;
}

static JSBool
call_enumerate(JSContext *cx, JSObject *obj)
{
    JSStackFrame *fp;
    JSObject *funobj, *pobj;
    JSScope *scope;
    JSScopeProperty *sprop, *cprop;
    JSPropertyOp getter;
    jsval *vec;
    JSAtom *atom;
    JSProperty *prop;

    fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
    if (!fp)
        return JS_TRUE;

    /*
     * Do not enumerate a cloned function object at fp->argv[-2], it may have
     * gained its own (mutable) scope (e.g., a brutally-shared XUL script sets
     * the clone's prototype property).  We must enumerate the function object
     * that was decorated with parameter and local variable properties by the
     * compiler when the compiler created fp->fun, namely fp->fun->object.
     *
     * Contrast with call_resolve, where we prefer fp->argv[-2], because we'll
     * use js_LookupProperty to find any overridden properties in that object,
     * if it was a mutated clone; and if not, we will search its prototype,
     * fp->fun->object, to find compiler-created params and locals.
     */
    funobj = fp->fun->object;
    if (!funobj)
        return JS_TRUE;

    /*
     * Reflect actual args from fp->argv for formal parameters, and local vars
     * and functions in fp->vars for declared variables and nested-at-top-level
     * local functions.
     */
    scope = OBJ_SCOPE(funobj);
    for (sprop = SCOPE_LAST_PROP(scope); sprop; sprop = sprop->parent) {
        getter = sprop->getter;
        if (getter == js_GetArgument)
            vec = fp->argv;
        else if (getter == js_GetLocalVariable)
            vec = fp->vars;
        else
            continue;

        /* Trigger reflection by looking up the unhidden atom for sprop->id. */
        JS_ASSERT(JSID_IS_ATOM(sprop->id));
        atom = JSID_TO_ATOM(sprop->id);
        JS_ASSERT(atom->flags & ATOM_HIDDEN);
        atom = atom->entry.value;

        if (!js_LookupProperty(cx, obj, ATOM_TO_JSID(atom), &pobj, &prop))
            return JS_FALSE;

        /*
         * If we found the property in a different object, don't try sticking
         * it into wrong slots vector. This can occur because we have a mutable
         * __proto__ slot, and cloned function objects rely on their __proto__
         * to delegate to the object that contains the var and arg properties.
         */
        if (!prop || pobj != obj) {
            if (prop)
                OBJ_DROP_PROPERTY(cx, pobj, prop);
            continue;
        }
        cprop = (JSScopeProperty *)prop;
        LOCKED_OBJ_SET_SLOT(obj, cprop->slot, vec[(uint16) sprop->shortid]);
        OBJ_DROP_PROPERTY(cx, obj, prop);
    }

    return JS_TRUE;
}

static JSBool
call_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags,
             JSObject **objp)
{
    JSStackFrame *fp;
    JSObject *funobj;
    JSString *str;
    JSAtom *atom;
    JSObject *obj2;
    JSProperty *prop;
    JSScopeProperty *sprop;
    JSPropertyOp getter, setter;
    uintN attrs, slot, nslots, spflags;
    jsval *vp, value;
    intN shortid;

    fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
    if (!fp)
        return JS_TRUE;
    JS_ASSERT(fp->fun);

    if (!JSVAL_IS_STRING(id))
        return JS_TRUE;

    funobj = fp->argv ? JSVAL_TO_OBJECT(fp->argv[-2]) : fp->fun->object;
    if (!funobj)
        return JS_TRUE;
    JS_ASSERT((JSFunction *) JS_GetPrivate(cx, funobj) == fp->fun);

    str = JSVAL_TO_STRING(id);
    atom = js_AtomizeString(cx, str, 0);
    if (!atom)
        return JS_FALSE;
    if (!js_LookupHiddenProperty(cx, funobj, ATOM_TO_JSID(atom), &obj2, &prop))
        return JS_FALSE;

    if (prop) {
        if (!OBJ_IS_NATIVE(obj2)) {
            OBJ_DROP_PROPERTY(cx, obj2, prop);
            return JS_TRUE;
        }

        sprop = (JSScopeProperty *) prop;
        getter = sprop->getter;
        attrs = sprop->attrs & ~JSPROP_SHARED;
        slot = (uintN) sprop->shortid;
        OBJ_DROP_PROPERTY(cx, obj2, prop);

        /* Ensure we found an arg or var property for the same function. */
        if ((sprop->flags & SPROP_IS_HIDDEN) &&
            (obj2 == funobj ||
             (JSFunction *) JS_GetPrivate(cx, obj2) == fp->fun)) {
            if (getter == js_GetArgument) {
                vp = fp->argv;
                nslots = JS_MAX(fp->argc, fp->fun->nargs);
                getter = setter = NULL;
            } else {
                JS_ASSERT(getter == js_GetLocalVariable);
                vp = fp->vars;
                nslots = fp->nvars;
                getter = js_GetCallVariable;
                setter = js_SetCallVariable;
            }
            if (slot < nslots) {
                value = vp[slot];
                spflags = SPROP_HAS_SHORTID;
                shortid = (intN) slot;
            } else {
                value = JSVAL_VOID;
                spflags = 0;
                shortid = 0;
            }
            if (!js_DefineNativeProperty(cx, obj, ATOM_TO_JSID(atom), value,
                                         getter, setter, attrs,
                                         spflags, shortid, NULL)) {
                return JS_FALSE;
            }
            *objp = obj;
        }
    }

    return JS_TRUE;
}

static JSBool
call_convert(JSContext *cx, JSObject *obj, JSType type, jsval *vp)
{
    JSStackFrame *fp;

    if (type == JSTYPE_FUNCTION) {
        fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
        if (fp) {
            JS_ASSERT(fp->fun);
            *vp = fp->argv ? fp->argv[-2] : OBJECT_TO_JSVAL(fp->fun->object);
        }
    }
    return JS_TRUE;
}

JSClass js_CallClass = {
    js_Call_str,
    JSCLASS_HAS_PRIVATE | JSCLASS_NEW_RESOLVE,
    JS_PropertyStub,  JS_PropertyStub,
    call_getProperty, call_setProperty,
    call_enumerate,   (JSResolveOp)call_resolve,
    call_convert,     JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

#endif /* JS_HAS_CALL_OBJECT */

/*
 * ECMA-262 specifies that length is a property of function object instances,
 * but we can avoid that space cost by delegating to a prototype property that
 * is JSPROP_PERMANENT and JSPROP_SHARED.  Each fun_getProperty call computes
 * a fresh length value based on the arity of the individual function object's
 * private data.
 *
 * The extensions below other than length, i.e., the ones not in ECMA-262,
 * are neither JSPROP_READONLY nor JSPROP_SHARED, because for compatibility
 * with ECMA we must allow a delegating object to override them.
 */
#define LENGTH_PROP_ATTRS (JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED)

static JSPropertySpec function_props[] = {
    {js_arguments_str, CALL_ARGUMENTS, JSPROP_PERMANENT,  0,0},
    {js_arity_str,     FUN_ARITY,      JSPROP_PERMANENT,  0,0},
    {js_caller_str,    FUN_CALLER,     JSPROP_PERMANENT,  0,0},
    {js_length_str,    ARGS_LENGTH,    LENGTH_PROP_ATTRS, 0,0},
    {js_name_str,      FUN_NAME,       JSPROP_PERMANENT,  0,0},
    {0,0,0,0,0}
};

static JSBool
fun_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    jsint slot;
    JSFunction *fun;
    JSStackFrame *fp;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;
    slot = JSVAL_TO_INT(id);

    /*
     * Loop because getter and setter can be delegated from another class,
     * but loop only for ARGS_LENGTH because we must pretend that f.length
     * is in each function instance f, per ECMA-262, instead of only in the
     * Function.prototype object (we use JSPROP_PERMANENT with JSPROP_SHARED
     * to make it appear so).
     *
     * This code couples tightly to the attributes for the function_props[]
     * initializers above, and to js_SetProperty and js_HasOwnPropertyHelper.
     *
     * It's important to allow delegating objects, even though they inherit
     * this getter (fun_getProperty), to override arguments, arity, caller,
     * and name.  If we didn't return early for slot != ARGS_LENGTH, we would
     * clobber *vp with the native property value, instead of letting script
     * override that value in delegating objects.
     *
     * Note how that clobbering is what simulates JSPROP_READONLY for all of
     * the non-standard properties when the directly addressed object (obj)
     * is a function object (i.e., when this loop does not iterate).
     */
    while (!(fun = (JSFunction *)
                   JS_GetInstancePrivate(cx, obj, &js_FunctionClass, NULL))) {
        if (slot != ARGS_LENGTH)
            return JS_TRUE;
        obj = OBJ_GET_PROTO(cx, obj);
        if (!obj)
            return JS_TRUE;
    }

    /* Find fun's top-most activation record. */
    for (fp = cx->fp; fp && (fp->fun != fun || (fp->flags & JSFRAME_SPECIAL));
         fp = fp->down) {
        continue;
    }

    switch (slot) {
      case CALL_ARGUMENTS:
#if JS_HAS_ARGS_OBJECT
        /* Warn if strict about f.arguments or equivalent unqualified uses. */
        if (!JS_ReportErrorFlagsAndNumber(cx,
                                          JSREPORT_WARNING | JSREPORT_STRICT,
                                          js_GetErrorMessage, NULL,
                                          JSMSG_DEPRECATED_USAGE,
                                          js_arguments_str)) {
            return JS_FALSE;
        }
        if (fp) {
            if (!js_GetArgsValue(cx, fp, vp))
                return JS_FALSE;
        } else {
            *vp = JSVAL_NULL;
        }
        break;
#else  /* !JS_HAS_ARGS_OBJECT */
        *vp = OBJECT_TO_JSVAL(fp ? obj : NULL);
        break;
#endif /* !JS_HAS_ARGS_OBJECT */

      case ARGS_LENGTH:
        if (!JS_VERSION_IS_ECMA(cx))
            *vp = INT_TO_JSVAL((jsint)(fp && fp->fun ? fp->argc : fun->nargs));
        else
      case FUN_ARITY:
            *vp = INT_TO_JSVAL((jsint)fun->nargs);
        break;

      case FUN_NAME:
        *vp = fun->atom
              ? ATOM_KEY(fun->atom)
              : STRING_TO_JSVAL(cx->runtime->emptyString);
        break;

      case FUN_CALLER:
        while (fp && (fp->flags & JSFRAME_SKIP_CALLER) && fp->down)
            fp = fp->down;
        if (fp && fp->down && fp->down->fun && fp->down->argv)
            *vp = fp->down->argv[-2];
        else
            *vp = JSVAL_NULL;
        if (!JSVAL_IS_PRIMITIVE(*vp) && cx->runtime->checkObjectAccess) {
            id = ATOM_KEY(cx->runtime->atomState.callerAtom);
            if (!cx->runtime->checkObjectAccess(cx, obj, id, JSACC_READ, vp))
                return JS_FALSE;
        }
        break;

      default:
        /* XXX fun[0] and fun.arguments[0] are equivalent. */
        if (fp && fp->fun && (uintN)slot < fp->fun->nargs)
            *vp = fp->argv[slot];
        break;
    }

    return JS_TRUE;
}

static JSBool
fun_enumerate(JSContext *cx, JSObject *obj)
{
    jsid prototypeId;
    JSObject *pobj;
    JSProperty *prop;

    prototypeId = ATOM_TO_JSID(cx->runtime->atomState.classPrototypeAtom);
    if (!OBJ_LOOKUP_PROPERTY(cx, obj, prototypeId, &pobj, &prop))
        return JS_FALSE;
    if (prop)
        OBJ_DROP_PROPERTY(cx, pobj, prop);
    return JS_TRUE;
}

static JSBool
fun_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags,
            JSObject **objp)
{
    JSFunction *fun;
    JSString *str;
    JSAtom *prototypeAtom;

    if (!JSVAL_IS_STRING(id))
        return JS_TRUE;

    /* No valid function object should lack private data, but check anyway