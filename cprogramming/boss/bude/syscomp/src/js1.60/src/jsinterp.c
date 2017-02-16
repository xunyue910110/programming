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
 * JavaScript bytecode interpreter.
 */
#include "jsstddef.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsprf.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jsbool.h"
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
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"

#if JS_HAS_JIT
#include "jsjit.h"
#endif

#if JS_HAS_XML_SUPPORT
#include "jsxml.h"
#endif

#ifdef DEBUG
#define ASSERT_CACHE_IS_EMPTY(cache)                                          \
    JS_BEGIN_MACRO                                                            \
        JSPropertyCacheEntry *end_, *pce_, entry_;                            \
        JSPropertyCache *cache_ = (cache);                                    \
        JS_ASSERT(cache_->empty);                                             \
        end_ = &cache_->table[PROPERTY_CACHE_SIZE];                           \
        for (pce_ = &cache_->table[0]; pce_ < end_; pce_++) {                 \
            PCE_LOAD(cache_, pce_, entry_);                                   \
            JS_ASSERT(!PCE_OBJECT(entry_));                                   \
            JS_ASSERT(!PCE_PROPERTY(entry_));                                 \
        }                                                                     \
    JS_END_MACRO
#else
#define ASSERT_CACHE_IS_EMPTY(cache) ((void)0)
#endif

void
js_FlushPropertyCache(JSContext *cx)
{
    JSPropertyCache *cache;

    cache = &cx->runtime->propertyCache;
    if (cache->empty) {
        ASSERT_CACHE_IS_EMPTY(cache);
        return;
    }
    memset(cache->table, 0, sizeof cache->table);
    cache->empty = JS_TRUE;
#ifdef JS_PROPERTY_CACHE_METERING
    cache->flushes++;
#endif
}

void
js_DisablePropertyCache(JSContext *cx)
{
    JS_ASSERT(!cx->runtime->propertyCache.disabled);
    cx->runtime->propertyCache.disabled = JS_TRUE;
}

void
js_EnablePropertyCache(JSContext *cx)
{
    JS_ASSERT(cx->runtime->propertyCache.disabled);
    ASSERT_CACHE_IS_EMPTY(&cx->runtime->propertyCache);
    cx->runtime->propertyCache.disabled = JS_FALSE;
}

/*
 * Class for for/in loop property iterator objects.
 */
#define JSSLOT_ITER_STATE   JSSLOT_PRIVATE

static void
prop_iterator_finalize(JSContext *cx, JSObject *obj)
{
    jsval iter_state;
    jsval iteratee;

    /* Protect against stillborn iterators. */
    iter_state = obj->slots[JSSLOT_ITER_STATE];
    iteratee = obj->slots[JSSLOT_PARENT];
    if (!JSVAL_IS_NULL(iter_state) && !JSVAL_IS_PRIMITIVE(iteratee)) {
        OBJ_ENUMERATE(cx, JSVAL_TO_OBJECT(iteratee), JSENUMERATE_DESTROY,
                      &iter_state, NULL);
    }
    js_RemoveRoot(cx->runtime, &obj->slots[JSSLOT_PARENT]);
}

static JSClass prop_iterator_class = {
    "PropertyIterator",
    0,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   prop_iterator_finalize,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/*
 * Stack macros and functions.  These all use a local variable, jsval *sp, to
 * point to the next free stack slot.  SAVE_SP must be called before any call
 * to a function that may invoke the interpreter.  RESTORE_SP must be called
 * only after return from js_Invoke, because only js_Invoke changes fp->sp.
 */
#define PUSH(v)         (*sp++ = (v))
#define POP()           (*--sp)
#ifdef DEBUG
#define SAVE_SP(fp)                                                           \
    (JS_ASSERT((fp)->script || !(fp)->spbase || (sp) == (fp)->spbase),        \
     (fp)->sp = sp)
#else
#define SAVE_SP(fp)     ((fp)->sp = sp)
#endif
#define RESTORE_SP(fp)  (sp = (fp)->sp)

/*
 * Push the generating bytecode's pc onto the parallel pc stack that runs
 * depth slots below the operands.
 *
 * NB: PUSH_OPND uses sp, depth, and pc from its lexical environment.  See
 * js_Interpret for these local variables' declarations and uses.
 */
#define PUSH_OPND(v)    (sp[-depth] = (jsval)pc, PUSH(v))
#define STORE_OPND(n,v) (sp[(n)-depth] = (jsval)pc, sp[n] = (v))
#define POP_OPND()      POP()
#define FETCH_OPND(n)   (sp[n])

/*
 * Push the jsdouble d using sp, depth, and pc from the lexical environment.
 * Try to convert d to a jsint that fits in a jsval, otherwise GC-alloc space
 * for it and push a reference.
 */
#define STORE_NUMBER(cx, n, d)                                                \
    JS_BEGIN_MACRO                                                            \
        jsint i_;                                                             \
        jsval v_;                                                             \
                                                                              \
        if (JSDOUBLE_IS_INT(d, i_) && INT_FITS_IN_JSVAL(i_)) {                \
            v_ = INT_TO_JSVAL(i_);                                            \
        } else {                                                              \
            ok = js_NewDoubleValue(cx, d, &v_);                               \
            if (!ok)                                                          \
                goto out;                                                     \
        }                                                                     \
        STORE_OPND(n, v_);                                                    \
    JS_END_MACRO

#define FETCH_NUMBER(cx, n, d)                                                \
    JS_BEGIN_MACRO                                                            \
        jsval v_;                                                             \
                                                                              \
        v_ = FETCH_OPND(n);                                                   \
        VALUE_TO_NUMBER(cx, v_, d);                                           \
    JS_END_MACRO

#define FETCH_INT(cx, n, i)                                                   \
    JS_BEGIN_MACRO                                                            \
        jsval v_ = FETCH_OPND(n);                                             \
        if (JSVAL_IS_INT(v_)) {                                               \
            i = JSVAL_TO_INT(v_);                                             \
        } else {                                                              \
            SAVE_SP(fp);                                                      \
            ok = js_ValueToECMAInt32(cx, v_, &i);                             \
            if (!ok)                                                          \
                goto out;                                                     \
        }                                                                     \
    JS_END_MACRO

#define FETCH_UINT(cx, n, ui)                                                 \
    JS_BEGIN_MACRO                                                            \
        jsval v_ = FETCH_OPND(n);                                             \
        jsint i_;                                                             \
        if (JSVAL_IS_INT(v_) && (i_ = JSVAL_TO_INT(v_)) >= 0) {               \
            ui = (uint32) i_;                                                 \
        } else {                                                              \
            SAVE_SP(fp);                                                      \
            ok = js_ValueToECMAUint32(cx, v_, &ui);                           \
            if (!ok)                                                          \
                goto out;                                                     \
        }                                                                     \
    JS_END_MACRO

/*
 * Optimized conversion macros that test for the desired type in v before
 * homing sp and calling a conversion function.
 */
#define VALUE_TO_NUMBER(cx, v, d)                                             \
    JS_BEGIN_MACRO                                                            \
        if (JSVAL_IS_INT(v)) {                                                \
            d = (jsdouble)JSVAL_TO_INT(v);                                    \
        } else if (JSVAL_IS_DOUBLE(v)) {                                      \
            d = *JSVAL_TO_DOUBLE(v);                                          \
        } else {                                                              \
            SAVE_SP(fp);                                                      \
            ok = js_ValueToNumber(cx, v, &d);                                 \
            if (!ok)                                                          \
                goto out;                                                     \
        }                                                                     \
    JS_END_MACRO

#define POP_BOOLEAN(cx, v, b)                                                 \
    JS_BEGIN_MACRO                                                            \
        v = FETCH_OPND(-1);                                                   \
        if (v == JSVAL_NULL) {                                                \
            b = JS_FALSE;                                                     \
        } else if (JSVAL_IS_BOOLEAN(v)) {                                     \
            b = JSVAL_TO_BOOLEAN(v);                                          \
        } else {                                                              \
            SAVE_SP(fp);                                                      \
            ok = js_ValueToBoolean(cx, v, &b);                                \
            if (!ok)                                                          \
                goto out;                                                     \
        }                                                                     \
        sp--;                                                                 \
    JS_END_MACRO

#define VALUE_TO_OBJECT(cx, v, obj)                                           \
    JS_BEGIN_MACRO                                                            \
        if (!JSVAL_IS_PRIMITIVE(v)) {                                         \
            obj = JSVAL_TO_OBJECT(v);                                         \
        } else {                                                              \
            SAVE_SP(fp);                                                      \
            obj = js_ValueToNonNullObject(cx, v);                             \
            if (!obj) {                                                       \
                ok = JS_FALSE;                                                \
                goto out;                                                     \
            }                                                                 \
        }                                                                     \
    JS_END_MACRO

#define FETCH_OBJECT(cx, n, v, obj)                                           \
    JS_BEGIN_MACRO                                                            \
        v = FETCH_OPND(n);                                                    \
        VALUE_TO_OBJECT(cx, v, obj);                                          \
        STORE_OPND(n, OBJECT_TO_JSVAL(obj));                                  \
    JS_END_MACRO

#if JS_BUG_VOID_TOSTRING
#define CHECK_VOID_TOSTRING(cx, v)                                            \
    if (JSVAL_IS_VOID(v)) {                                                   \
        JSString *str_;                                                       \
        str_ = ATOM_TO_STRING(cx->runtime->atomState.typeAtoms[JSTYPE_VOID]); \
        v = STRING_TO_JSVAL(str_);                                            \
    }
#else
#define CHECK_VOID_TOSTRING(cx, v)  ((void)0)
#endif

#if JS_BUG_EAGER_TOSTRING
#define CHECK_EAGER_TOSTRING(hint)  (hint = JSTYPE_STRING)
#else
#define CHECK_EAGER_TOSTRING(hint)  ((void)0)
#endif

#define VALUE_TO_PRIMITIVE(cx, v, hint, vp)                                   \
    JS_BEGIN_MACRO                                                            \
        if (JSVAL_IS_PRIMITIVE(v)) {                                          \
            CHECK_VOID_TOSTRING(cx, v);                                       \
            *vp = v;                                                          \
        } else {                                                              \
            SAVE_SP(fp);                                                      \
            CHECK_EAGER_TOSTRING(hint);                                       \
            ok = OBJ_DEFAULT_VALUE(cx, JSVAL_TO_OBJECT(v), hint, vp);         \
            if (!ok)                                                          \
                goto out;                                                     \
        }                                                                     \
    JS_END_MACRO

JS_FRIEND_API(jsval *)
js_AllocRawStack(JSContext *cx, uintN nslots, void **markp)
{
    jsval *sp;

    if (markp)
        *markp = JS_ARENA_MARK(&cx->stackPool);
    JS_ARENA_ALLOCATE_CAST(sp, jsval *, &cx->stackPool, nslots * sizeof(jsval));
    if (!sp) {
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_STACK_OVERFLOW,
                             (cx->fp && cx->fp->fun)
                             ? JS_GetFunctionName(cx->fp->fun)
                             : "script");
    }
    return sp;
}

JS_FRIEND_API(void)
js_FreeRawStack(JSContext *cx, void *mark)
{
    JS_ARENA_RELEASE(&cx->stackPool, mark);
}

JS_FRIEND_API(jsval *)
js_AllocStack(JSContext *cx, uintN nslots, void **markp)
{
    jsval *sp, *vp, *end;
    JSArena *a;
    JSStackHeader *sh;
    JSStackFrame *fp;

    /* Callers don't check for zero nslots: we do to avoid empty segments. */
    if (nslots == 0) {
        *markp = NULL;
        return JS_ARENA_MARK(&cx->stackPool);
    }

    /* Allocate 2 extra slots for the stack segment header we'll likely need. */
    sp = js_AllocRawStack(cx, 2 + nslots, markp);
    if (!sp)
        return NULL;

    /* Try to avoid another header if we can piggyback on the last segment. */
    a = cx->stackPool.current;
    sh = cx->stackHeaders;
    if (sh && JS_STACK_SEGMENT(sh) + sh->nslots == sp) {
        /* Extend the last stack segment, give back the 2 header slots. */
        sh->nslots += nslots;
        a->avail -= 2 * sizeof(jsval);
    } else {
        /*
         * Need a new stack segment, so we must initialize unused slots in the
         * current frame.  See js_GC, just before marking the "operand" jsvals,
         * where we scan from fp->spbase to fp->sp or through fp->script->depth
         * (whichever covers fewer slots).
         */
        fp = cx->fp;
        if (fp && fp->script && fp->spbase) {
#ifdef DEBUG
            jsuword depthdiff = fp->script->depth * sizeof(jsval);
            JS_ASSERT(JS_UPTRDIFF(fp->sp, fp->spbase) <= depthdiff);
            JS_ASSERT(JS_UPTRDIFF(*markp, fp->spbase) >= depthdiff);
#endif
            end = fp->spbase + fp->script->depth;
            for (vp = fp->sp; vp < end; vp++)
                *vp = JSVAL_VOID;
        }

        /* Allocate and push a stack segment header from the 2 extra slots. */
        sh = (JSStackHeader *)sp;
        sh->nslots = nslots;
        sh->down = cx->stackHeaders;
        cx->stackHeaders = sh;
        sp += 2;
    }

    /*
     * Store JSVAL_NULL using memset, to let compilers optimize as they see
     * fit, in case a caller allocates and pushes GC-things one by one, which
     * could nest a last-ditch GC that will scan this segment.
     */
    memset(sp, 0, nslots * sizeof(jsval));
    return sp;
}

JS_FRIEND_API(void)
js_FreeStack(JSContext *cx, void *mark)
{
    JSStackHeader *sh;
    jsuword slotdiff;

    /* Check for zero nslots allocation special case. */
    if (!mark)
        return;

    /* We can assert because js_FreeStack always balances js_AllocStack. */
    sh = cx->stackHeaders;
    JS_ASSERT(sh);

    /* If mark is in the current segment, reduce sh->nslots, else pop sh. */
    slotdiff = JS_UPTRDIFF(mark, JS_STACK_SEGMENT(sh)) / sizeof(jsval);
    if (slotdiff < (jsuword)sh->nslots)
        sh->nslots = slotdiff;
    else
        cx->stackHeaders = sh->down;

    /* Release the stackPool space allocated since mark was set. */
    JS_ARENA_RELEASE(&cx->stackPool, mark);
}

JSBool
js_GetArgument(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

JSBool
js_SetArgument(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

JSBool
js_GetLocalVariable(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

JSBool
js_SetLocalVariable(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

JSBool
js_ComputeThis(JSContext *cx, JSObject *thisp, JSStackFrame *fp)
{
    if (thisp && OBJ_GET_CLASS(cx, thisp) != &js_CallClass) {
        /* Some objects (e.g., With) delegate 'this' to another object. */
        thisp = OBJ_THIS_OBJECT(cx, thisp);
        if (!thisp)
            return JS_FALSE;

        /* Default return value for a constructor is the new object. */
        if (fp->flags & JSFRAME_CONSTRUCTING)
            fp->rval = OBJECT_TO_JSVAL(thisp);
    } else {
        /*
         * ECMA requires "the global object", but in the presence of multiple
         * top-level objects (windows, frames, or certain layers in the client
         * object model), we prefer fun's parent.  An example that causes this
         * code to run:
         *
         *   // in window w1
         *   function f() { return this }
         *   function g() { return f }
         *
         *   // in window w2
         *   var h = w1.g()
         *   alert(h() == w1)
         *
         * The alert should display "true".
         */
        JS_ASSERT(!(fp->flags & JSFRAME_CONSTRUCTING));
        if (JSVAL_IS_PRIMITIVE(fp->argv[-2]) ||
            !OBJ_GET_PARENT(cx, JSVAL_TO_OBJECT(fp->argv[-2]))) {
            thisp = cx->globalObject;
        } else {
            jsid id;
            jsval v;
            uintN attrs;

            /* Walk up the parent chain. */
            thisp = JSVAL_TO_OBJECT(fp->argv[-2]);
            id = ATOM_TO_JSID(cx->runtime->atomState.parentAtom);
            for (;;) {
                if (!OBJ_CHECK_ACCESS(cx, thisp, id, JSACC_PARENT, &v, &attrs))
                    return JS_FALSE;
                if (JSVAL_IS_VOID(v))
                    v = OBJ_GET_SLOT(cx, thisp, JSSLOT_PARENT);
                if (JSVAL_IS_NULL(v))
                    break;
                thisp = JSVAL_TO_OBJECT(v);
            }
        }
    }
    fp->thisp = thisp;
    fp->argv[-1] = OBJECT_TO_JSVAL(thisp);
    return JS_TRUE;
}

#ifdef DUMP_CALL_TABLE

#include "jsclist.h"
#include "jshash.h"
#include "jsdtoa.h"

typedef struct CallKey {
    jsval               callee;                 /* callee value */
    const char          *filename;              /* function filename or null */
    uintN               lineno;                 /* function lineno or 0 */
} CallKey;

/* Compensate for typeof null == "object" brain damage. */
#define JSTYPE_NULL     JSTYPE_LIMIT
#define TYPEOF(cx,v)    (JSVAL_IS_NULL(v) ? JSTYPE_NULL : JS_TypeOfValue(cx,v))
#define TYPENAME(t)     (((t) == JSTYPE_NULL) ? js_null_str : js_type_str[t])
#define NTYPEHIST       (JSTYPE_LIMIT + 1)

typedef struct CallValue {
    uint32              total;                  /* total call count */
    uint32              recycled;               /* LRU-recycled calls lost */
    uint16              minargc;                /* minimum argument count */
    uint16              maxargc;                /* maximum argument count */
    struct ArgInfo {
        uint32          typeHist[NTYPEHIST];    /* histogram by type */
        JSCList         lruList;                /* top 10 values LRU list */
        struct ArgValCount {
            JSCList     lruLink;                /* LRU list linkage */
            jsval       value;                  /* recently passed value */
            uint32      count;                  /* number of times passed */
            char        strbuf[112];            /* string conversion buffer */
        } topValCounts[10];                     /* top 10 value storage */
    } argInfo[8];
} CallValue;

typedef struct CallEntry {
    JSHashEntry         entry;
    CallKey             key;
    CallValue           value;
    char                name[32];               /* function name copy */
} CallEntry;

static void *
AllocCallTable(void *pool, size_t size)
{
    return malloc(size);
}

static void
FreeCallTable(void *pool, void *item)
{
    free(item);
}

static JSHashEntry *
AllocCallEntry(void *pool, const void *key)
{
    return (JSHashEntry*) calloc(1, sizeof(CallEntry));
}

static void
FreeCallEntry(void *pool, JSHashEntry *he, uintN flag)
{
    JS_ASSERT(flag == HT_FREE_ENTRY);
    free(he);
}

static JSHashAllocOps callTableAllocOps = {
    AllocCallTable, FreeCallTable,
    AllocCallEntry, FreeCallEntry
};

JS_STATIC_DLL_CALLBACK(JSHashNumber)
js_hash_call_key(const void *key)
{
    CallKey *ck = (CallKey *) key;
    JSHashNumber hash = (jsuword)ck->callee >> 3;

    if (ck->filename) {
        hash = (hash << 4) ^ JS_HashString(ck->filename);
        hash = (hash << 4) ^ ck->lineno;
    }
    return hash;
}

JS_STATIC_DLL_CALLBACK(intN)
js_compare_call_keys(const void *k1, const void *k2)
{
    CallKey *ck1 = (CallKey *)k1, *ck2 = (CallKey *)k2;

    return ck1->callee == ck2->callee &&
           ((ck1->filename && ck2->filename)
            ? strcmp(ck1->filename, ck2->filename) == 0
            : ck1->filename == ck2->filename) &&
           ck1->lineno == ck2->lineno;
}

JSHashTable *js_CallTable;
size_t      js_LogCallToSourceLimit;

JS_STATIC_DLL_CALLBACK(intN)
CallTableDumper(JSHashEntry *he, intN k, void *arg)
{
    CallEntry *ce = (CallEntry *)he;
    FILE *fp = (FILE *)arg;
    uintN argc, i, n;
    struct ArgInfo *ai;
    JSType save, type;
    JSCList *cl;
    struct ArgValCount *avc;
    jsval argval;

    if (ce->key.filename) {
        /* We're called at the end of the mark phase, so mark our filenames. */
        js_MarkScriptFilename(ce->key.filename);
        fprintf(fp, "%s:%u ", ce->key.filename, ce->key.lineno);
    } else {
        fprintf(fp, "@%p ", (void *) ce->key.callee);
    }

    if (ce->name[0])
        fprintf(fp, "name %s ", ce->name);
    fprintf(fp, "calls %lu (%lu) argc %u/%u\n",
            (unsigned long) ce->value.total,
            (unsigned long) ce->value.recycled,
            ce->value.minargc, ce->value.maxargc);

    argc = JS_MIN(ce->value.maxargc, 8);
    for (i = 0; i < argc; i++) {
        ai = &ce->value.argInfo[i];

        n = 0;
        save = -1;
        for (type = JSTYPE_VOID; type <= JSTYPE_LIMIT; type++) {
            if (ai->typeHist[type]) {
                save = type;
                ++n;
            }
        }
        if (n == 1) {
            fprintf(fp, "  arg %u type %s: %lu\n",
                    i, TYPENAME(save), (unsigned long) ai->typeHist[save]);
        } else {
            fprintf(fp, "  arg %u type histogram:\n", i);
            for (type = JSTYPE_VOID; type <= JSTYPE_LIMIT; type++) {
                fprintf(fp, "  %9s: %8lu ",
                       TYPENAME(type), (unsigned long) ai->typeHist[type]);
                for (n = (uintN) JS_HOWMANY(ai->typeHist[type], 10); n > 0; --n)
                    fputc('*', fp);
                fputc('\n', fp);
            }
        }

        fprintf(fp, "  arg %u top 10 values:\n", i);
        n = 1;
        for (cl = ai->lruList.prev; cl != &ai->lruList; cl = cl->prev) {
            avc = (struct ArgValCount *)cl;
            if (!avc->count)
                break;
            argval = avc->value;
            fprintf(fp, "  %9u: %8lu %.*s (%#lx)\n",
                    n, (unsigned long) avc->count,
                    sizeof avc->strbuf, avc->strbuf, argval);
            ++n;
        }
    }

    return HT_ENUMERATE_NEXT;
}

void
js_DumpCallTable(JSContext *cx)
{
    char name[24];
    FILE *fp;
    static uintN dumpCount;

    if (!js_CallTable)
        return;

    JS_snprintf(name, sizeof name, "/tmp/calltable.dump.%u", dumpCount & 7);
    dumpCount++;
    fp = fopen(name, "w");
    if (!fp)
        return;

    JS_HashTableEnumerateEntries(js_CallTable, CallTableDumper, fp);
    fclose(fp);
}

static void
LogCall(JSContext *cx, jsval callee, uintN argc, jsval *argv)
{
    CallKey key;
    const char *name, *cstr;
    JSFunction *fun;
    JSHashNumber keyHash;
    JSHashEntry **hep, *he;
    CallEntry *ce;
    uintN i, j;
    jsval argval;
    JSType type;
    struct ArgInfo *ai;
    struct ArgValCount *avc;
    JSString *str;

    if (!js_CallTable) {
        js_CallTable = JS_NewHashTable(1024, js_hash_call_key,
                                       js_compare_call_keys, NULL,
                                       &callTableAllocOps, NULL);
        if (!js_CallTable)
            return;
    }

    key.callee = callee;
    key.filename = NULL;
    key.lineno = 0;
    name = "";
    if (JSVAL_IS_FUNCTION(cx, callee)) {
        fun = (JSFunction *) JS_GetPrivate(cx, JSVAL_TO_OBJECT(callee));
        if (fun->atom)
            name = js_AtomToPrintableString(cx, fun->atom);
        if (fun->interpreted) {
            key.filename = fun->u.script->filename;
            key.lineno = fun->u.script->lineno;
        }
    }
    keyHash = js_hash_call_key(&key);

    hep = JS_HashTableRawLookup(js_CallTable, keyHash, &key);
    he = *hep;
    if (he) {
        ce = (CallEntry *) he;
        JS_ASSERT(strncmp(ce->name, name, sizeof ce->name) == 0);
    } else {
        he = JS_HashTableRawAdd(js_CallTable, hep, keyHash, &key, NULL);
        if (!he)
            return;
        ce = (CallEntry *) he;
        ce->entry.key = &ce->key;
        ce->entry.value = &ce->value;
        ce->key = key;
        for (i = 0; i < 8; i++) {
            ai = &ce->value.argInfo[i];
            JS_INIT_CLIST(&ai->lruList);
            for (j = 0; j < 10; j++)
                JS_APPEND_LINK(&ai->topValCounts[j].lruLink, &ai->lruList);
        }
        strncpy(ce->name, name, sizeof ce->name);
    }

    ++ce->value.total;
    if (ce->value.minargc < argc)
        ce->value.minargc = argc;
    if (ce->value.maxargc < argc)
        ce->value.maxargc = argc;
    if (argc > 8)
        argc = 8;
    for (i = 0; i < argc; i++) {
        ai = &ce->value.argInfo[i];
        argval = argv[i];
        type = TYPEOF(cx, argval);
        ++ai->typeHist[type];

        for (j = 0; ; j++) {
            if (j == 10) {
                avc = (struct ArgValCount *) ai->lruList.next;
                ce->value.recycled += avc->count;
                avc->value = argval;
                avc->count = 1;
                break;
            }
            avc = &ai->topValCounts[j];
            if (avc->value == argval) {
                ++avc->count;
                break;
            }
        }

        /* Move avc to the back of the LRU list. */
        JS_REMOVE_LINK(&avc->lruLink);
        JS_APPEND_LINK(&avc->lruLink, &ai->lruList);

        str = NULL;
        cstr = "";
        switch (TYPEOF(cx, argval)) {
          case JSTYPE_VOID:
            cstr = js_type_str[JSTYPE_VOID];
            break;
          case JSTYPE_NULL:
            cstr = js_null_str;
            break;
          case JSTYPE_BOOLEAN:
            cstr = js_boolean_str[JSVAL_TO_BOOLEAN(argval)];
            break;
          case JSTYPE_NUMBER:
            if (JSVAL_IS_INT(argval)) {
                JS_snprintf(avc->strbuf, sizeof avc->strbuf, "%ld",
                            JSVAL_TO_INT(argval));
            } else {
                JS_dtostr(avc->strbuf, sizeof avc->strbuf, DTOSTR_STANDARD, 0,
                          *JSVAL_TO_DOUBLE(argval));
            }
            continue;
          case JSTYPE_STRING:
            str = js_QuoteString(cx, JSVAL_TO_STRING(argval), (jschar)'"');
            break;
          case JSTYPE_FUNCTION:
            if (JSVAL_IS_FUNCTION(cx, argval)) {
                fun = (JSFunction *)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argval));
                if (fun && fun->atom) {
                    str = ATOM_TO_STRING(fun->atom);
                    break;
                }
            }
            /* FALL THROUGH */
          case JSTYPE_OBJECT:
            js_LogCallToSourceLimit = sizeof avc->strbuf;
            cx->options |= JSOPTION_LOGCALL_TOSOURCE;
            str = js_ValueToSource(cx, argval);
            cx->options &= ~JSOPTION_LOGCALL_TOSOURCE;
            break;
        }
        if (str)
            cstr = JS_GetStringBytes(str);
        strncpy(avc->strbuf, cstr, sizeof avc->strbuf);
    }
}

#endif /* DUMP_CALL_TABLE */

/*
 * Find a function reference and its 'this' object implicit first parameter
 * under argc arguments on cx's stack, and call the function.  Push missing
 * required arguments, allocate declared local variables, and pop everything
 * when done.  Then push the return value.
 */
JS_FRIEND_API(JSBool)
js_Invoke(JSContext *cx, uintN argc, uintN flags)
{
    void *mark;
    JSStackFrame *fp, frame;
    jsval *sp, *newsp, *limit;
    jsval *vp, v;
    JSObject *funobj, *parent, *thisp;
    JSBool ok;
    JSClass *clasp;
    JSObjectOps *ops;
    JSNative native;
    JSFunction *fun;
    JSScript *script;
    uintN nslots, nvars, nalloc, surplus;
    JSInterpreterHook hook;
    void *hookData;

    /* Mark the top of stack and load frequently-used registers. */
    mark = JS_ARENA_MARK(&cx->stackPool);
    fp = cx->fp;
    sp = fp->sp;

    /*
     * Set vp to the callee value's stack slot (it's where rval goes).
     * Once vp is set, control should flow through label out2: to return.
     * Set frame.rval early so native class and object ops can throw and
     * return false, causing a goto out2 with ok set to false.  Also set
     * frame.flags to flags so that ComputeThis can test bits in it.
     */
    vp = sp - (2 + argc);
    v = *vp;
    frame.rval = JSVAL_VOID;
    frame.flags = flags;
    thisp = JSVAL_TO_OBJECT(vp[1]);

    /*
     * A callee must be an object reference, unless its |this| parameter
     * implements the __noSuchMethod__ method, in which case that method will
     * be called like so:
     *
     *   thisp.__noSuchMethod__(id, args)
     *
     * where id is the name of the method that this invocation attempted to
     * call by name, and args is an Array containing this invocation's actual
     * parameters.
     */
    if (JSVAL_