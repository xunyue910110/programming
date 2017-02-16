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
 * JS object implementation.
 */
#include "jsstddef.h"
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jshash.h" /* Added by JSIFY */
#include "jsdhash.h"
#include "jsprf.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jsbool.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsfun.h"
#include "jsgc.h"
#include "jsinterp.h"
#include "jslock.h"
#include "jsnum.h"
#include "jsobj.h"
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"
#include "jsopcode.h"

#include "jsdbgapi.h"   /* whether or not JS_HAS_OBJ_WATCHPOINT */

#if JS_HAS_XML_SUPPORT
#include "jsxml.h"
#endif

#ifdef JS_THREADSAFE
#define NATIVE_DROP_PROPERTY js_DropProperty

extern void
js_DropProperty(JSContext *cx, JSObject *obj, JSProperty *prop);
#else
#define NATIVE_DROP_PROPERTY NULL
#endif

JS_FRIEND_DATA(JSObjectOps) js_ObjectOps = {
    js_NewObjectMap,        js_DestroyObjectMap,
    js_LookupProperty,      js_DefineProperty,
    js_GetProperty,         js_SetProperty,
    js_GetAttributes,       js_SetAttributes,
    js_DeleteProperty,      js_DefaultValue,
    js_Enumerate,           js_CheckAccess,
    NULL,                   NATIVE_DROP_PROPERTY,
    js_Call,                js_Construct,
    NULL,                   js_HasInstance,
    js_SetProtoOrParent,    js_SetProtoOrParent,
    js_Mark,                js_Clear,
    js_GetRequiredSlot,     js_SetRequiredSlot
};

JSClass js_ObjectClass = {
    js_Object_str,
    0,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

#if JS_HAS_OBJ_PROTO_PROP

static JSBool
obj_getSlot(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

static JSBool
obj_setSlot(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

static JSBool
obj_getCount(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

static JSPropertySpec object_props[] = {
    /* These two must come first; see object_props[slot].name usage below. */
    {js_proto_str, JSSLOT_PROTO, JSPROP_PERMANENT|JSPROP_SHARED,
                                                  obj_getSlot,  obj_setSlot},
    {js_parent_str,JSSLOT_PARENT,JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED,
                                                  obj_getSlot,  obj_setSlot},
    {js_count_str, 0,            JSPROP_PERMANENT,obj_getCount, obj_getCount},
    {0,0,0,0,0}
};

/* NB: JSSLOT_PROTO and JSSLOT_PARENT are already indexes into object_props. */
#define JSSLOT_COUNT 2

static JSBool
ReportStrictSlot(JSContext *cx, uint32 slot)
{
    if (slot == JSSLOT_PROTO)
        return JS_TRUE;
    return JS_ReportErrorFlagsAndNumber(cx,
                                        JSREPORT_WARNING | JSREPORT_STRICT,
                                        js_GetErrorMessage, NULL,
                                        JSMSG_DEPRECATED_USAGE,
                                        object_props[slot].name);
}

static JSBool
obj_getSlot(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    uint32 slot;
    jsid propid;
    JSAccessMode mode;
    uintN attrs;
    JSObject *pobj;
    JSClass *clasp;
    JSExtendedClass *xclasp;

    slot = (uint32) JSVAL_TO_INT(id);
    if (id == INT_TO_JSVAL(JSSLOT_PROTO)) {
        propid = ATOM_TO_JSID(cx->runtime->atomState.protoAtom);
        mode = JSACC_PROTO;
    } else {
        propid = ATOM_TO_JSID(cx->runtime->atomState.parentAtom);
        mode = JSACC_PARENT;
    }

    /* Let OBJ_CHECK_ACCESS get the slot's value, based on the access mode. */
    if (!OBJ_CHECK_ACCESS(cx, obj, propid, mode, vp, &attrs))
        return JS_FALSE;

    pobj = JSVAL_TO_OBJECT(*vp);
    if (pobj) {
        clasp = OBJ_GET_CLASS(cx, pobj);
        if (clasp->flags & JSCLASS_IS_EXTENDED) {
            xclasp = (JSExtendedClass *) clasp;
            if (xclasp->outerObject) {
                pobj = xclasp->outerObject(cx, pobj);
                if (!pobj)
                    return JS_FALSE;
                *vp = OBJECT_TO_JSVAL(pobj);
            }
        }
    }
    return JS_TRUE;
}

static JSBool
obj_setSlot(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSObject *pobj;
    uint32 slot;
    jsid propid;
    uintN attrs;

    if (!JSVAL_IS_OBJECT(*vp))
        return JS_TRUE;
    pobj = JSVAL_TO_OBJECT(*vp);
    slot = (uint32) JSVAL_TO_INT(id);
    if (JS_HAS_STRICT_OPTION(cx) && !ReportStrictSlot(cx, slot))
        return JS_FALSE;

    /* __parent__ is readonly and permanent, only __proto__ may be set. */
    propid = ATOM_TO_JSID(cx->runtime->atomState.protoAtom);
    if (!OBJ_CHECK_ACCESS(cx, obj, propid, JSACC_PROTO|JSACC_WRITE, vp, &attrs))
        return JS_FALSE;

    return js_SetProtoOrParent(cx, obj, slot, pobj);
}

static JSBool
obj_getCount(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    jsval iter_state;
    jsid num_properties;
    JSBool ok;

    if (JS_HAS_STRICT_OPTION(cx) && !ReportStrictSlot(cx, JSSLOT_COUNT))
        return JS_FALSE;

    /* Get the number of properties to enumerate. */
    iter_state = JSVAL_NULL;
    ok = OBJ_ENUMERATE(cx, obj, JSENUMERATE_INIT, &iter_state, &num_properties);
    if (!ok)
        goto out;

    if (!JSVAL_IS_INT(num_properties)) {
        JS_ASSERT(0);
        *vp = JSVAL_ZERO;
        goto out;
    }
    *vp = num_properties;

out:
    if (iter_state != JSVAL_NULL)
        ok = OBJ_ENUMERATE(cx, obj, JSENUMERATE_DESTROY, &iter_state, 0);
    return ok;
}

#else  /* !JS_HAS_OBJ_PROTO_PROP */

#define object_props NULL

#endif /* !JS_HAS_OBJ_PROTO_PROP */

JSBool
js_SetProtoOrParent(JSContext *cx, JSObject *obj, uint32 slot, JSObject *pobj)
{
    JSRuntime *rt;
    JSObject *obj2, *oldproto;
    JSScope *scope, *newscope;

    /*
     * Serialize all proto and parent setting in order to detect cycles.
     * We nest locks in this function, and only here, in the following orders:
     *
     * (1)  rt->setSlotLock < pobj's scope lock;
     *      rt->setSlotLock < pobj's proto-or-parent's scope lock;
     *      rt->setSlotLock < pobj's grand-proto-or-parent's scope lock;
     *      etc...
     * (2)  rt->setSlotLock < obj's scope lock < pobj's scope lock.
     *
     * We avoid AB-BA deadlock by restricting obj from being on pobj's parent
     * or proto chain (pobj may already be on obj's parent or proto chain; it
     * could be moving up or down).  We finally order obj with respect to pobj
     * at the bottom of this routine (just before releasing rt->setSlotLock),
     * by making pobj be obj's prototype or parent.
     *
     * After we have set the slot and released rt->setSlotLock, another call
     * to js_SetProtoOrParent could nest locks according to the first order
     * list above, but it cannot deadlock with any other thread.  For there
     * to be a deadlock, other parts of the engine would have to nest scope
     * locks in the opposite order.  XXXbe ensure they don't!
     */
    rt = cx->runtime;
#ifdef JS_THREADSAFE

    JS_ACQUIRE_LOCK(rt->setSlotLock);
    while (rt->setSlotBusy) {
        jsrefcount saveDepth;

        /* Take pains to avoid nesting rt->gcLock inside rt->setSlotLock! */
        JS_RELEASE_LOCK(rt->setSlotLock);
        saveDepth = JS_SuspendRequest(cx);
        JS_ACQUIRE_LOCK(rt->setSlotLock);
        if (rt->setSlotBusy)
            JS_WAIT_CONDVAR(rt->setSlotDone, JS_NO_TIMEOUT);
        JS_RELEASE_LOCK(rt->setSlotLock);
        JS_ResumeRequest(cx, saveDepth);
        JS_ACQUIRE_LOCK(rt->setSlotLock);
    }
    rt->setSlotBusy = JS_TRUE;
    JS_RELEASE_LOCK(rt->setSlotLock);

#define SET_SLOT_DONE(rt)                                                     \
    JS_BEGIN_MACRO                                                            \
        JS_ACQUIRE_LOCK((rt)->setSlotLock);                                   \
        (rt)->setSlotBusy = JS_FALSE;                                         \
        JS_NOTIFY_ALL_CONDVAR((rt)->setSlotDone);                             \
        JS_RELEASE_LOCK((rt)->setSlotLock);                                   \
    JS_END_MACRO

#else

#define SET_SLOT_DONE(rt)       /* nothing */

#endif

    obj2 = pobj;
    while (obj2) {
        if (obj2 == obj) {
            SET_SLOT_DONE(rt);
            JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                                 JSMSG_CYCLIC_VALUE,
#if JS_HAS_OBJ_PROTO_PROP
                                 object_props[slot].name
#else
                                 (slot == JSSLOT_PROTO) ? js_proto_str
                                                        : js_parent_str
#endif
                                 );
            return JS_FALSE;
        }
        obj2 = JSVAL_TO_OBJECT(OBJ_GET_SLOT(cx, obj2, slot));
    }

    if (slot == JSSLOT_PROTO && OBJ_IS_NATIVE(obj)) {
        /* Check to see whether obj shares its prototype's scope. */
        JS_LOCK_OBJ(cx, obj);
        scope = OBJ_SCOPE(obj);
        oldproto = JSVAL_TO_OBJECT(LOCKED_OBJ_GET_SLOT(obj, JSSLOT_PROTO));
        if (oldproto && OBJ_SCOPE(oldproto) == scope) {
            /* Either obj needs a new empty scope, or it should share pobj's. */
            if (!pobj ||
                !OBJ_IS_NATIVE(pobj) ||
                OBJ_GET_CLASS(cx, pobj) != LOCKED_OBJ_GET_CLASS(oldproto)) {
                /*
                 * With no proto and no scope of its own, obj is truly empty.
                 *
                 * If pobj is not native, obj needs its own empty scope -- it
                 * should not continue to share oldproto's scope once oldproto
                 * is not on obj's prototype chain.  That would put properties
                 * from oldproto's scope ahead of properties defined by pobj,
                 * in lookup order.
                 *
                 * If pobj's class differs from oldproto's, we may need a new
                 * scope to handle differences in private and reserved slots,
                 * so we suboptimally but safely make one.
                 */
                scope = js_GetMutableScope(cx, obj);
                if (!scope) {
                    JS_UNLOCK_OBJ(cx, obj);
                    SET_SLOT_DONE(rt);
                    return JS_FALSE;
                }
            } else if (OBJ_SCOPE(pobj) != scope) {
#ifdef JS_THREADSAFE
                /*
                 * We are about to nest scope locks.  Help jslock.c:ShareScope
                 * keep scope->u.count balanced for the JS_UNLOCK_SCOPE, while
                 * avoiding deadlock, by recording scope in rt->setSlotScope.
                 */
                if (scope->ownercx) {
                    JS_ASSERT(scope->ownercx == cx);
                    rt->setSlotScope = scope;
                }
#endif

                /* We can't deadlock because we checked for cycles above (2). */
                JS_LOCK_OBJ(cx, pobj);
                newscope = (JSScope *) js_HoldObjectMap(cx, pobj->map);
                obj->map = &newscope->map;
                js_DropObjectMap(cx, &scope->map, obj);
                JS_TRANSFER_SCOPE_LOCK(cx, scope, newscope);
                scope = newscope;
#ifdef JS_THREADSAFE
                rt->setSlotScope = NULL;
#endif
            }
        }
        LOCKED_OBJ_SET_SLOT(obj, JSSLOT_PROTO, OBJECT_TO_JSVAL(pobj));
        JS_UNLOCK_SCOPE(cx, scope);
    } else {
        OBJ_SET_SLOT(cx, obj, slot, OBJECT_TO_JSVAL(pobj));
    }

    SET_SLOT_DONE(rt);
    return JS_TRUE;

#undef SET_SLOT_DONE
}

JS_STATIC_DLL_CALLBACK(JSHashNumber)
js_hash_object(const void *key)
{
    return (JSHashNumber)JS_PTR_TO_UINT32(key) >> JSVAL_TAGBITS;
}

static JSHashEntry *
MarkSharpObjects(JSContext *cx, JSObject *obj, JSIdArray **idap)
{
    JSSharpObjectMap *map;
    JSHashTable *table;
    JSHashNumber hash;
    JSHashEntry **hep, *he;
    jsatomid sharpid;
    JSIdArray *ida;
    JSBool ok;
    jsint i, length;
    jsid id;
#if JS_HAS_GETTER_SETTER
    JSObject *obj2;
    JSProperty *prop;
    uintN attrs;
#endif
    jsval val;
    int stackDummy;

    if (!JS_CHECK_STACK_SIZE(cx, stackDummy)) {
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_OVER_RECURSED);
        return NULL;
    }

    map = &cx->sharpObjectMap;
    table = map->table;
    hash = js_hash_object(obj);
    hep = JS_HashTableRawLookup(table, hash, obj);
    he = *hep;
    if (!he) {
        sharpid = 0;
        he = JS_HashTableRawAdd(table, hep, hash, obj,
                                JS_UINT32_TO_PTR(sharpid));
        if (!he) {
            JS_ReportOutOfMemory(cx);
            return NULL;
        }

        /* 
         * Increment map->depth to protect js_EnterSharpObject from reentering
         * itself badly.  Without this fix, if we reenter the basis case where
         * map->depth == 0, when unwinding the inner call we will destroy the
         * newly-created hash table and crash.
         */
	++map->depth;
        ida = JS_Enumerate(cx, obj);
	--map->depth;
        if (!ida)
            return NULL;

        ok = JS_TRUE;
        for (i = 0, length = ida->length; i < length; i++) {
            id = ida->vector[i];
#if JS_HAS_GETTER_SETTER
            ok = OBJ_LOOKUP_PROPERTY(cx, obj, id, &obj2, &prop);
            if (!ok)
                break;
            if (!prop)
                continue;
            ok = OBJ_GET_ATTRIBUTES(cx, obj2, id, prop, &attrs);
            if (ok) {
                if (OBJ_IS_NATIVE(obj2) &&
                    (attrs & (JSPROP_GETTER | JSPROP_SETTER))) {
                    val = JSVAL_NULL;
                    if (attrs & JSPROP_GETTER)
                        val = (jsval) ((JSScopeProperty*)prop)->getter;
                    if (attrs & JSPROP_SETTER) {
                        if (val != JSVAL_NULL) {
                            /* Mark the getter, then set val to setter. */
                            ok = (MarkSharpObjects(cx, JSVAL_TO_OBJECT(val),
                                                   NULL)
                                  != NULL);
                        }
                        val = (jsval) ((JSScopeProperty*)prop)->setter;
                    }
                } else {
                    ok = OBJ_GET_PROPERTY(cx, obj, id, &val);
                }
            }
            OBJ_DROP_PROPERTY(cx, obj2, prop);
#else
            ok = OBJ_GET_PROPERTY(cx, obj, id, &val);
#endif
            if (!ok)
                break;
            if (!JSVAL_IS_PRIMITIVE(val) &&
                !MarkSharpObjects(cx, JSVAL_TO_OBJECT(val), NULL)) {
                ok = JS_FALSE;
                break;
            }
        }
        if (!ok || !idap)
            JS_DestroyIdArray(cx, ida);
        if (!ok)
            return NULL;
    } else {
        sharpid = JS_PTR_TO_UINT32(he->value);
        if (sharpid == 0) {
            sharpid = ++map->sharpgen << SHARP_ID_SHIFT;
            he->value = JS_UINT32_TO_PTR(sharpid);
        }
        ida = NULL;
    }
    if (idap)
        *idap = ida;
    return he;
}

JSHashEntry *
js_EnterSharpObject(JSContext *cx, JSObject *obj, JSIdArray **idap,
                    jschar **sp)
{
    JSSharpObjectMap *map;
    JSHashTable *table;
    JSIdArray *ida;
    JSHashNumber hash;
    JSHashEntry *he, **hep;
    jsatomid sharpid;
    char buf[20];
    size_t len;

    if (JS_HAS_NATIVE_BRANCH_CALLBACK_OPTION(cx) &&
        cx->branchCallback &&
        !cx->branchCallback(cx, NULL)) {
        return NULL;
    }

    /* Set to null in case we return an early error. */
    *sp = NULL;
    map = &cx->sharpObjectMap;
    table = map->table;
    if (!table) {
        table = JS_NewHashTable(8, js_hash_object, JS_CompareValues,
                                JS_CompareValues, NULL, NULL);
        if (!table) {
            JS_ReportOutOfMemory(cx);
            return NULL;
        }
        map->table = table;
        JS_KEEP_ATOMS(cx->runtime);
    }

    /* From this point the control must flow either through out: or bad:. */
    ida = NULL;
    if (map->depth == 0) {
        he = MarkSharpObjects(cx, obj, &ida);
        if (!he)
            goto bad;
        JS_ASSERT((JS_PTR_TO_UINT32(he->value) & SHARP_BIT) == 0);
        if (!idap) {
            JS_DestroyIdArray(cx, ida);
            ida = NULL;
        }
    } else {
        hash = js_hash_object(obj);
        hep = JS_HashTableRawLookup(table, hash, obj);
        he = *hep;

        /*
         * It's possible that the value of a property has changed from the
         * first time the object's properties are traversed (when the property
         * ids are entered into the hash table) to the second (when they are
         * converted to strings), i.e., the OBJ_GET_PROPERTY() call is not
         * idempotent.
         */
        if (!he) {
            he = JS_HashTableRawAdd(table, hep, hash, obj, NULL);
            if (!he) {
                JS_ReportOutOfMemory(cx);
                goto bad;
            }
            sharpid = 0;
            goto out;
        }
    }

    sharpid = JS_PTR_TO_UINT32(he->value);
    if (sharpid != 0) {
        len = JS_snprintf(buf, sizeof buf, "#%u%c",
                          sharpid >> SHARP_ID_SHIFT,
                          (sharpid & SHARP_BIT) ? '#' : '=');
        *sp = js_InflateString(cx, buf, &len);
        if (!*sp) {
            if (ida)
                JS_DestroyIdArray(cx, ida);
            goto bad;
        }
    }

out:
    JS_ASSERT(he);
    if ((sharpid & SHARP_BIT) == 0) {
        if (idap && !ida) {
            ida = JS_Enumerate(cx, obj);
            if (!ida) {
                if (*sp) {
                    JS_free(cx, *sp);
                    *sp = NULL;
                }
                goto bad;
            }
        }
        map->depth++;
    }

    if (idap)
        *idap = ida;
    return he;

bad:
    /* Clean up the sharpObjectMap table on outermost error. */
    if (map->depth == 0) {
        JS_UNKEEP_ATOMS(cx->runtime);
        map->sharpgen = 0;
        JS_HashTableDestroy(map->table);
        map->table = NULL;
    }
    return NULL;
}

void
js_LeaveSharpObject(JSContext *cx, JSIdArray **idap)
{
    JSSharpObjectMap *map;
    JSIdArray *ida;

    map = &cx->sharpObjectMap;
    JS_ASSERT(map->depth > 0);
    if (--map->depth == 0) {
        JS_UNKEEP_ATOMS(cx->runtime);
        map->sharpgen = 0;
        JS_HashTableDestroy(map->table);
        map->table = NULL;
    }
    if (idap) {
        ida = *idap;
        if (ida) {
            JS_DestroyIdArray(cx, ida);
            *idap = NULL;
        }
    }
}

JS_STATIC_DLL_CALLBACK(intN)
gc_sharp_table_entry_marker(JSHashEntry *he, intN i, void *arg)
{
    GC_MARK((JSContext *)arg, (JSObject *)he->key, "sharp table entry", NULL);
    return JS_DHASH_NEXT;
}

void
js_GCMarkSharpMap(JSContext *cx, JSSharpObjectMap *map)
{
    JS_ASSERT(map->depth > 0);
    JS_ASSERT(map->table);

    /*
     * During recursive calls to MarkSharpObjects a non-native object or
     * object with a custom getProperty method can potentially return an
     * unrooted value or even cut from the object graph an argument of one of
     * MarkSharpObjects recursive invocations. So we must protect map->table
     * entries against GC.
     *
     * We can not simply use JSTempValueRooter to mark the obj argument of
     * MarkSharpObjects during recursion as we have to protect *all* entries
     * in JSSharpObjectMap including those that contains otherwise unreachable
     * objects just allocated through custom getProperty. Otherwise newer
     * allocations can re-use the address of an object stored in the hashtable
     * confusing js_EnterSharpObject. So to address the problem we simply
     * mark all objects from map->table.
     *
     * An alternative "proper" solution is to use JSTempValueRooter in
     * MarkSharpObjects with code to remove during finalization entries
     * with otherwise unreachable objects. But this is way too complex
     * to justify spending efforts.
     */
    JS_HashTableEnumerateEntries(map->table, gc_sharp_table_entry_marker, cx);
}

#define OBJ_TOSTRING_EXTRA      4       /* for 4 local GC roots */

#if JS_HAS_INITIALIZERS || JS_HAS_TOSOURCE
JSBool
js_obj_toSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                jsval *rval)
{
    JSBool ok, outermost;
    JSHashEntry *he;
    JSIdArray *ida;
    jschar *chars, *ochars, *vsharp;
    const jschar *idstrchars, *vchars;
    size_t nchars, idstrlength, gsoplength, vlength, vsharplength, curlen;
    char *comma;
    jsint i, j, length, valcnt;
    jsid id;
#if JS_HAS_GETTER_SETTER
    JSObject *obj2;
    JSProperty *prop;
    uintN attrs;
#endif
    jsval *val;
    JSString *gsop[2];
    JSAtom *atom;
    JSString *idstr, *valstr, *str;
    int stackDummy;

    if (!JS_CHECK_STACK_SIZE(cx, stackDummy)) {
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_OVER_RECURSED);
        return JS_FALSE;
    }

    /*
     * obj_toString for 1.2 calls toSource, and doesn't want the extra parens
     * on the outside.
     */
    outermost = !JS_VERSION_IS_1_2(cx) && cx->sharpObjectMap.depth == 0;
    he = js_EnterSharpObject(cx, obj, &ida, &chars);
    if (!he)
        return JS_FALSE;
    if (IS_SHARP(he)) {
        /*
         * We didn't enter -- obj is already "sharp", meaning we've visited it
         * already in our depth first search, and therefore chars contains a
         * string of the form "#n#".
         */
        JS_ASSERT(!ida);
#if JS_HAS_SHARP_VARS
        nchars = js_strlen(chars);
#else
        chars[0] = '{';
        chars[1] = '}';
        chars[2] = 0;
        nchars = 2;
#endif
        goto make_string;
    }
    JS_ASSERT(ida);
    ok = JS_TRUE;

    if (!chars) {
        /* If outermost, allocate 4 + 1 for "({})" and the terminator. */
        chars = (jschar *) malloc(((outermost ? 4 : 2) + 1) * sizeof(jschar));
        nchars = 0;
        if (!chars)
            goto error;
        if (outermost)
            chars[nchars++] = '(';
    } else {
        /* js_EnterSharpObject returned a string of the form "#n=" in chars. */
        MAKE_SHARP(he);
        nchars = js_strlen(chars);
        chars = (jschar *)
            realloc((ochars = chars), (nchars + 2 + 1) * sizeof(jschar));
        if (!chars) {
            free(ochars);
            goto error;
        }
        if (outermost) {
            /*
             * No need for parentheses around the whole shebang, because #n=
             * unambiguously begins an object initializer, and never a block
             * statement.
             */
            outermost = JS_FALSE;
        }
    }

#ifdef DUMP_CALL_TABLE
    if (cx->options & JSOPTION_LOGCALL_TOSOURCE) {
        const char *classname = OBJ_GET_CLASS(cx, obj)->name;
        size_t classnchars = strlen(classname);
        static const char classpropid[] = "C";
        const char *cp;
        size_t onchars = nchars;

        /* 2 for ': ', 2 quotes around classname, 2 for ', ' after. */
        classnchars += sizeof classpropid - 1 + 2 + 2;
        if (ida->length)
            classnchars += 2;

        /* 2 for the braces, 1 for the terminator */
        chars = (jschar *)
            realloc((ochars = chars),
                    (nchars + classnchars + 2 + 1) * sizeof(jschar));
        if (!chars) {
            free(ochars);
            goto error;
        }

        chars[nchars++] = '{';          /* 1 from the 2 braces */
        for (cp = classpropid; *cp; cp++)
            chars[nchars++] = (jschar) *cp;
        chars[nchars++] = ':';
        chars[nchars++] = ' ';          /* 2 for ': ' */
        chars[nchars++] = '"';
        for (cp = classname; *cp; cp++)
            chars[nchars++] = (jschar) *cp;
        chars[nchars++] = '"';          /* 2 quotes */
        if (ida->length) {
            chars[nchars++] = ',';
            chars[nchars++] = ' ';      /* 2 for ', ' */
        }

        JS_ASSERT(nchars - onchars == 1 + classnchars);
    } else
#endif
    chars[nchars++] = '{';

    comma = NULL;

    /*
     * We have four local roots for cooked and raw value GC safety.  Hoist the
     * "argv + 2" out of the loop using the val local, which refers to the raw
     * (unconverted, "uncooked") values.
     */
    val = argv + 2;

    for (i = 0, length = ida->length; i < length; i++) {
        /* Get strings for id and value and GC-root them via argv. */
        id = ida->vector[i];

#if JS_HAS_GETTER_SETTER

        ok = OBJ_LOOKUP_PROPERTY(cx, obj, id, &obj2, &prop);
        if (!ok)
            goto error;
        valcnt = 0;
        if (prop) {
            ok = OBJ_GET_ATTRIBUTES(cx, obj2, id, prop, &attrs);
            if (!ok) {
                OBJ_DROP_PROPERTY(cx, obj2, prop);
                goto error;
            }
            if (OBJ_IS_NATIVE(obj2) &&
                (attrs & (JSPROP_GETTER | JSPROP_SETTER))) {
                if (attrs & JSPROP_GETTER) {
                    val[valcnt] = (jsval) ((JSScopeProperty *)prop)->getter;
#ifdef OLD_GETTER_SETTER
                    gsop[valcnt] =
                        ATOM_TO_STRING(cx->runtime->atomState.getterAtom);
#else
                    gsop[valcnt] =
                        ATOM_TO_STRING(cx->runtime->atomState.getAtom);
#endif
                    valcnt++;
                }
                if (attrs & JSPROP_SETTER) {
                    val[valcnt] = (jsval) ((JSScopeProperty *)prop)->setter;
#ifdef OLD_GETTER_SETTER
                    gsop[valcnt] =
                        ATOM_TO_STRING(cx->runtime->atomState.setterAtom);
#else
                    gsop[valcnt] =
                        ATOM_TO_STRING(cx->runtime->atomState.setAtom);
#endif
                    valcnt++;
                }
            } else {
                valcnt = 1;
                gsop[0] = NULL;
                ok = OBJ_GET_PROPERTY(cx, obj, id, &val[0]);
            }
            OBJ_DROP_PROPERTY(cx, obj2, prop);
        }

#else  /* !JS_HAS_GETTER_SETTER */

        valcnt = 1;
        gsop[0] = NULL;
        ok = OBJ_GET_PROPERTY(cx, obj, id, &val[0]);

#endif /* !JS_HAS_GETTER_SETTER */

        if (!ok)
            goto error;

        /* Convert id to a jsval and then to a string. */
        atom = JSID_IS_ATOM(id) ? JSID_TO_ATOM(id) : NULL;
        id = ID_TO_VALUE(id);
        idstr = js_ValueToString(cx, id);
        if (!idstr) {
            ok = JS_FALSE;
            goto error;
        }
        *rval = STRING_TO_JSVAL(idstr);         /* local root */

        /*
         * If id is a string that's a reserved identifier, or else id is not
         * an identifier at all, then it needs to be quoted.  Also, negative
         * integer ids must be quoted.
         */
        if (atom
            ? (ATOM_KEYWORD(atom) || !js_IsIdentifier(idstr))
            : (JSID_IS_OBJECT(id) || JSID_TO_INT(id) < 0)) {
            idstr = js_QuoteString(cx, idstr, (jschar)'\'');
            if (!idstr) {
                ok = JS_FALSE;
                goto error;
            }
            *rval = STRING_TO_JSVAL(idstr);     /* local root */
        }
        idstrchars = JSSTRING_CHARS(idstr);
        idstrlength = JSSTRING_LENGTH(idstr);

        for (j = 0; j < valcnt; j++) {
            /* Convert val[j] to its canonical source form. */
            valstr = js_ValueToSource(cx, val[j]);
            if (!valstr) {
                ok = JS_FALSE;
                goto error;
            }
            argv[j] = STRING_TO_JSVAL(valstr);  /* local root */
            vchars = JSSTRING_CHARS(valstr);
            vlength = JSSTRING_LENGTH(valstr);

#ifndef OLD_GETTER_SETTER
            /*
             * Remove '(function ' from the beginning of valstr and ')' from the
             * end so that we can put "get" in front of the function definition.
             */
            if (gsop[j]) {
                int n = strlen(js_function_str) + 2;
                vchars += n;
                vlength -= n + 1;
            }
#endif

            /* If val[j] is a non-sharp object, consider sharpening it. */
            vsharp = NULL;
            vsharplength = 0;
#if JS_HAS_SHARP_VARS
            if (!JSVAL_IS_PRIMITIVE(val[j]) && vchars[0] != '#') {
                he = js_EnterSharpObject(cx, JSVAL_TO_OBJECT(val[j]), NULL,
                                         &vsharp);
                if (!he) {
                    ok = JS_FALSE;
                    goto error;
                }
                if (IS_SHARP(he)) {
                    vchars = vsharp;
                    vlength = js_strlen(vchars);
                } else {
                    if (vsharp) {
                        vsharplength = js_strlen(vsharp);
                        MAKE_SHARP(he);
                    }
                    js_LeaveSharpObject(cx, NULL);
                }
            }
#endif

#define SAFE_ADD(n)                                                          \
    JS_BEGIN_MACRO                                                           \
        size_t n_ = (n);                                                     \
        curlen += n_;                                                        \
        if (curlen < n_)                                                     \
            goto overflow;                                                   \
    JS_END_MACRO

            curlen = nchars;
            if (comma)
                SAFE_ADD(2);
            SAFE_ADD(idstrlength + 1);
            if (gsop[j])
                SAFE_ADD(JSSTRING_LENGTH(gsop[j]) + 1);
            SAFE_ADD(vsharplength);
            SAFE_ADD(vlength);
            SAFE_ADD((outermost ? 2 : 1) + 1);
#undef SAFE_ADD

            if (curlen > (size_t)-1 / sizeof(jschar))
                goto overflow;

            /* Allocate 1 + 1 at end for closing brace and terminating 0. */
            chars = (jschar *)
                realloc((ochars = chars), curlen * sizeof(jschar));
            if (!chars) {
                /* Save code space on error: let JS_free ignore null vsharp. */
                JS_free(cx, vsharp);
                free(ochars);
                goto error;
            }

            if (comma) {
                chars[nchars++] = comma[0];
                chars[nchars++] = comma[1];
            }
            comma = ", ";

#ifdef OLD_GETTER_SETTER
            js_strncpy(&chars[nchars], idstrchars, idstrlength);
            nchars += idstrlength;
            if (gsop[j]) {
                chars[nchars++] = ' ';
                gsoplength = JSSTRING_LENGTH(gsop[j]);
                js_strncpy(&chars[nchars], JSSTRING_CHARS(gsop[j]), gsoplength);
                nchars += gsoplength;
            }
            chars[nchars++] = ':';
#else
            if (gsop[j]) {
                gsoplength = JSSTRING_LENGTH(gsop[j]);
                js_strncpy(&chars[nchars], JSSTRING_CHARS(gsop[j]), gsoplength);
                nchars += gsople