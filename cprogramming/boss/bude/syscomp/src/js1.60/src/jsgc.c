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
 * JS Mark-and-Sweep Garbage Collector.
 *
 * This GC allocates only fixed-sized things big enough to contain two words
 * (pointers) on any host architecture.  It allocates from an arena pool (see
 * jsarena.h).  It uses an ideally parallel array of flag bytes to hold the
 * mark bit, finalizer type index, etc.
 *
 * XXX swizzle page to freelist for better locality of reference
 */
#include "jsstddef.h"
#include <stdlib.h>     /* for free, called by JS_ARENA_DESTROY */
#include <string.h>     /* for memset, called by jsarena.h macros if DEBUG */
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jshash.h" /* Added by JSIFY */
#include "jsapi.h"
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
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"

#if JS_HAS_XML_SUPPORT
#include "jsxml.h"
#endif

/*
 * GC arena sizing depends on amortizing arena overhead using a large number
 * of things per arena, and on the thing/flags ratio of 8:1 on most platforms.
 *
 * On 64-bit platforms, we would have half as many things per arena because
 * pointers are twice as big, so we double the bytes for things per arena.
 * This preserves the 1024 byte flags sub-arena size, which relates to the
 * GC_PAGE_SIZE (see below for why).
 */
#if JS_BYTES_PER_WORD == 8
# define GC_THINGS_SHIFT 14     /* 16KB for things on Alpha, etc. */
#else
# define GC_THINGS_SHIFT 13     /* 8KB for things on most platforms */
#endif
#define GC_THINGS_SIZE  JS_BIT(GC_THINGS_SHIFT)
#define GC_FLAGS_SIZE   (GC_THINGS_SIZE / sizeof(JSGCThing))
#define GC_ARENA_SIZE   (GC_THINGS_SIZE + GC_FLAGS_SIZE)

/*
 * A GC arena contains one flag byte for each thing in its heap, and supports
 * O(1) lookup of a flag given its thing's address.
 *
 * To implement this, we take advantage of the thing/flags numerology: given
 * the 8K bytes worth of GC-things, there are 1K flag bytes.  We mask a thing's
 * address with ~1023 to find a JSGCPageInfo record at the front of a mythical
 * "GC page" within the larger 8K thing arena.  That JSGCPageInfo contains a
 * pointer to the 128 flag bytes corresponding to the things in the page, so we
 * index into this flags array using the thing's index within its page.
 *
 * To align thing pages on 1024-byte boundaries, we must allocate the 9KB of
 * flags+things arena payload, then find the first 0 mod 1024 boundary after
 * the first payload address.  That's where things start, with a JSGCPageInfo
 * taking up the first thing-slot, as usual for 0 mod 1024 byte boundaries.
 * The effect of this alignment trick is to split the flags into at most 2
 * discontiguous spans, one before the things and one after (if we're really
 * lucky, and the arena payload starts on a 0 mod 1024 byte boundary, no need
 * to split).
 *
 * The overhead of this scheme for most platforms is (16+8*(8+1))/(16+9K) or
 * .95% (assuming 16 byte JSArena header size, and 8 byte JSGCThing size).
 *
 * Here's some ASCII art showing an arena:
 *
 *   split
 *     |
 *     V
 *  +--+-------+-------+-------+-------+-------+-------+-------+-------+-----+
 *  |fB|  tp0  |  tp1  |  tp2  |  tp3  |  tp4  |  tp5  |  tp6  |  tp7  | fA  |
 *  +--+-------+-------+-------+-------+-------+-------+-------+-------+-----+
 *              ^                                 ^
 *  tI ---------+                                 |
 *  tJ -------------------------------------------+
 *
 *  - fB are the "before split" flags, fA are the "after split" flags
 *  - tp0-tp7 are the 8 thing pages
 *  - thing tI points into tp1, whose flags are below the split, in fB
 *  - thing tJ points into tp5, clearly above the split
 *
 * In general, one of the thing pages will have some of its things' flags on
 * the low side of the split, and the rest of its things' flags on the high
 * side.  All the other pages have flags only below or only above.  Therefore
 * we'll have to test something to decide whether the split divides flags in
 * a given thing's page.  So we store the split pointer (the pointer to tp0)
 * in each JSGCPageInfo, along with the flags pointer for the 128 flag bytes
 * ideally starting, for tp0 things, at the beginning of the arena's payload
 * (at the start of fB).
 *
 * That is, each JSGCPageInfo's flags pointer is 128 bytes from the previous,
 * or at the start of the arena if there is no previous page in this arena.
 * Thus these ideal 128-byte flag pages run contiguously from the start of the
 * arena (right over the split!), and the JSGCPageInfo flags pointers contain
 * no discontinuities over the split created by the thing pages.  So if, for a
 * given JSGCPageInfo *pi, we find that
 *
 *  pi->flags + ((jsuword)thing % 1024) / sizeof(JSGCThing) >= pi->split
 *
 * then we must add GC_THINGS_SIZE to the nominal flags pointer to jump over
 * all the thing pages that split the flags into two discontiguous spans.
 *
 * (If we need to implement card-marking for an incremental GC write barrier,
 * we can use the low byte of the pi->split pointer as the card-mark, for an
 * extremely efficient write barrier: when mutating an object obj, just store
 * a 1 byte at (uint8 *) ((jsuword)obj & ~1023) for little-endian platforms.
 * When finding flags, we'll of course have to mask split with ~255, but it is
 * guaranteed to be 1024-byte aligned, so no information is lost by overlaying
 * the card-mark byte on split's low byte.)
 */
#define GC_PAGE_SHIFT   10
#define GC_PAGE_MASK    ((jsuword) JS_BITMASK(GC_PAGE_SHIFT))
#define GC_PAGE_SIZE    JS_BIT(GC_PAGE_SHIFT)

typedef struct JSGCPageInfo {
    uint8       *split;
    uint8       *flags;
} JSGCPageInfo;

#define FIRST_THING_PAGE(a)     (((a)->base + GC_FLAGS_SIZE) & ~GC_PAGE_MASK)

/*
 * Given a jsuword page pointer p and a thing size n, return the address of
 * the first thing in p.  We know that any n not a power of two packs from
 * the end of the page leaving at least enough room for one JSGCPageInfo, but
 * not for another thing, at the front of the page (JS_ASSERTs below insist
 * on this).
 *
 * This works because all allocations are a multiple of sizeof(JSGCThing) ==
 * sizeof(JSGCPageInfo) in size.
 */
#define FIRST_THING(p,n)        (((n) & ((n) - 1))                            \
                                 ? (p) + (uint32)(GC_PAGE_SIZE % (n))         \
                                 : (p) + (n))

static JSGCThing *
gc_new_arena(JSArenaPool *pool, size_t nbytes)
{
    uint8 *flagp, *split, *pagep, *limit;
    JSArena *a;
    jsuword p;
    JSGCThing *thing;
    JSGCPageInfo *pi;

    /* Use JS_ArenaAllocate to grab another 9K-net-size hunk of space. */
    flagp = (uint8 *) JS_ArenaAllocate(pool, GC_ARENA_SIZE);
    if (!flagp)
        return NULL;
    a = pool->current;

    /* Reset a->avail to start at the flags split, aka the first thing page. */
    p = FIRST_THING_PAGE(a);
    split = pagep = (uint8 *) p;
    a->avail = FIRST_THING(p, nbytes);
    JS_ASSERT(a->avail >= p + sizeof(JSGCPageInfo));
    thing = (JSGCThing *) a->avail;
    JS_ArenaCountAllocation(pool, a->avail - p);
    a->avail += nbytes;

    /* Initialize the JSGCPageInfo records at the start of every thing page. */
    limit = pagep + GC_THINGS_SIZE;
    do {
        pi = (JSGCPageInfo *) pagep;
        pi->split = split;
        pi->flags = flagp;
        flagp += GC_PAGE_SIZE >> (GC_THINGS_SHIFT -  GC_PAGE_SHIFT);
        pagep += GC_PAGE_SIZE;
    } while (pagep < limit);
    return thing;
}

uint8 *
js_GetGCThingFlags(void *thing)
{
    JSGCPageInfo *pi;
    uint8 *flagp;

    pi = (JSGCPageInfo *) ((jsuword)thing & ~GC_PAGE_MASK);
    flagp = pi->flags + ((jsuword)thing & GC_PAGE_MASK) / sizeof(JSGCThing);
    if (flagp >= pi->split)
        flagp += GC_THINGS_SIZE;
    return flagp;
}

JSBool
js_IsAboutToBeFinalized(JSContext *cx, void *thing)
{
    uint8 flags = *js_GetGCThingFlags(thing);

    return !(flags & (GCF_MARK | GCF_LOCK | GCF_FINAL));
}

typedef void (*GCFinalizeOp)(JSContext *cx, JSGCThing *thing);

#ifndef DEBUG
# define js_FinalizeDouble       NULL
#endif

#if !JS_HAS_XML_SUPPORT
# define js_FinalizeXMLNamespace NULL
# define js_FinalizeXMLQName     NULL
# define js_FinalizeXML          NULL
#endif

static GCFinalizeOp gc_finalizers[GCX_NTYPES] = {
    (GCFinalizeOp) js_FinalizeObject,           /* GCX_OBJECT */
    (GCFinalizeOp) js_FinalizeString,           /* GCX_STRING */
    (GCFinalizeOp) js_FinalizeDouble,           /* GCX_DOUBLE */
    (GCFinalizeOp) js_FinalizeString,           /* GCX_MUTABLE_STRING */
    NULL,                                       /* GCX_PRIVATE */
    (GCFinalizeOp) js_FinalizeXMLNamespace,     /* GCX_NAMESPACE */
    (GCFinalizeOp) js_FinalizeXMLQName,         /* GCX_QNAME */
    (GCFinalizeOp) js_FinalizeXML,              /* GCX_XML */
    NULL,                                       /* GCX_EXTERNAL_STRING */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

#ifdef GC_MARK_DEBUG
static const char newborn_external_string[] = "newborn external string";

static const char *gc_typenames[GCX_NTYPES] = {
    "newborn object",
    "newborn string",
    "newborn double",
    "newborn mutable string",
    "newborn private",
    "newborn Namespace",
    "newborn QName",
    "newborn XML",
    newborn_external_string,
    newborn_external_string,
    newborn_external_string,
    newborn_external_string,
    newborn_external_string,
    newborn_external_string,
    newborn_external_string,
    newborn_external_string
};
#endif

intN
js_ChangeExternalStringFinalizer(JSStringFinalizeOp oldop,
                                 JSStringFinalizeOp newop)
{
    uintN i;

    for (i = GCX_EXTERNAL_STRING; i < GCX_NTYPES; i++) {
        if (gc_finalizers[i] == (GCFinalizeOp) oldop) {
            gc_finalizers[i] = (GCFinalizeOp) newop;
            return (intN) i;
        }
    }
    return -1;
}

#ifdef JS_GCMETER
#define METER(x) x
#else
#define METER(x) /* nothing */
#endif

/* Initial size of the gcRootsHash table (SWAG, small enough to amortize). */
#define GC_ROOTS_SIZE   256
#define GC_FINALIZE_LEN 1024

JSBool
js_InitGC(JSRuntime *rt, uint32 maxbytes)
{
    uintN i;

    JS_ASSERT(sizeof(JSGCThing) == sizeof(JSGCPageInfo));
    JS_ASSERT(sizeof(JSGCThing) >= sizeof(JSObject));
    JS_ASSERT(sizeof(JSGCThing) >= sizeof(JSString));
    JS_ASSERT(sizeof(JSGCThing) >= sizeof(jsdouble));
    JS_ASSERT(GC_FLAGS_SIZE >= GC_PAGE_SIZE);
    JS_ASSERT(sizeof(JSStackHeader) >= 2 * sizeof(jsval));

    for (i = 0; i < GC_NUM_FREELISTS; i++)
        JS_InitArenaPool(&rt->gcArenaPool[i], "gc-arena", GC_ARENA_SIZE, 1);
    if (!JS_DHashTableInit(&rt->gcRootsHash, JS_DHashGetStubOps(), NULL,
                           sizeof(JSGCRootHashEntry), GC_ROOTS_SIZE)) {
        rt->gcRootsHash.ops = NULL;
        return JS_FALSE;
    }
    rt->gcLocksHash = NULL;     /* create lazily */

    /*
     * Separate gcMaxMallocBytes from gcMaxBytes but initialize to maxbytes
     * for default backward API compatibility.
     */
    rt->gcMaxBytes = rt->gcMaxMallocBytes = maxbytes;
    return JS_TRUE;
}

#ifdef JS_GCMETER
JS_FRIEND_API(void)
js_DumpGCStats(JSRuntime *rt, FILE *fp)
{
    uintN i;

    fprintf(fp, "\nGC allocation statistics:\n");

#define UL(x)       ((unsigned long)(x))
#define ULSTAT(x)   UL(rt->gcStats.x)
    fprintf(fp, "     public bytes allocated: %lu\n", UL(rt->gcBytes));
    fprintf(fp, "    private bytes allocated: %lu\n", UL(rt->gcPrivateBytes));
    fprintf(fp, "             alloc attempts: %lu\n", ULSTAT(alloc));
    for (i = 0; i < GC_NUM_FREELISTS; i++) {
        fprintf(fp, "       GC freelist %u length: %lu\n",
                i, ULSTAT(freelen[i]));
        fprintf(fp, " recycles via GC freelist %u: %lu\n",
                i, ULSTAT(recycle[i]));
    }
    fprintf(fp, "allocation retries after GC: %lu\n", ULSTAT(retry));
    fprintf(fp, "        allocation failures: %lu\n", ULSTAT(fail));
    fprintf(fp, "         things born locked: %lu\n", ULSTAT(lockborn));
    fprintf(fp, "           valid lock calls: %lu\n", ULSTAT(lock));
    fprintf(fp, "         valid unlock calls: %lu\n", ULSTAT(unlock));
    fprintf(fp, "       mark recursion depth: %lu\n", ULSTAT(depth));
    fprintf(fp, "     maximum mark recursion: %lu\n", ULSTAT(maxdepth));
    fprintf(fp, "     mark C recursion depth: %lu\n", ULSTAT(cdepth));
    fprintf(fp, "   maximum mark C recursion: %lu\n", ULSTAT(maxcdepth));
    fprintf(fp, "     mark C stack overflows: %lu\n", ULSTAT(dswmark));
    fprintf(fp, "   mark DSW recursion depth: %lu\n", ULSTAT(dswdepth));
    fprintf(fp, " maximum mark DSW recursion: %lu\n", ULSTAT(maxdswdepth));
    fprintf(fp, "  mark DSW up-tree movement: %lu\n", ULSTAT(dswup));
    fprintf(fp, "DSW up-tree obj->slot steps: %lu\n", ULSTAT(dswupstep));
    fprintf(fp, "   maximum GC nesting level: %lu\n", ULSTAT(maxlevel));
    fprintf(fp, "potentially useful GC calls: %lu\n", ULSTAT(poke));
    fprintf(fp, "           useless GC calls: %lu\n", ULSTAT(nopoke));
    fprintf(fp, "  thing arenas freed so far: %lu\n", ULSTAT(afree));
    fprintf(fp, "     stack segments scanned: %lu\n", ULSTAT(stackseg));
    fprintf(fp, "stack segment slots scanned: %lu\n", ULSTAT(segslots));
#undef UL
#undef US

#ifdef JS_ARENAMETER
    JS_DumpArenaStats(fp);
#endif
}
#endif

#ifdef DEBUG
JS_STATIC_DLL_CALLBACK(JSDHashOperator)
js_root_printer(JSDHashTable *table, JSDHashEntryHdr *hdr, uint32 i, void *arg)
{
    uint32 *leakedroots = (uint32 *)arg;
    JSGCRootHashEntry *rhe = (JSGCRootHashEntry *)hdr;

    (*leakedroots)++;
    fprintf(stderr,
            "JS engine warning: leaking GC root \'%s\' at %p\n",
            rhe->name ? (char *)rhe->name : "", rhe->root);

    return JS_DHASH_NEXT;
}
#endif

void
js_FinishGC(JSRuntime *rt)
{
    uintN i;

#ifdef JS_ARENAMETER
    JS_DumpArenaStats(stdout);
#endif
#ifdef JS_GCMETER
    js_DumpGCStats(rt, stdout);
#endif
    for (i = 0; i < GC_NUM_FREELISTS; i++) {
        JS_FinishArenaPool(&rt->gcArenaPool[i]);
        rt->gcFreeList[i] = NULL;
    }
    JS_ArenaFinish();

    if (rt->gcRootsHash.ops) {
#ifdef DEBUG
        uint32 leakedroots = 0;

        /* Warn (but don't assert) debug builds of any remaining roots. */
        JS_DHashTableEnumerate(&rt->gcRootsHash, js_root_printer,
                               &leakedroots);
        if (leakedroots > 0) {
            if (leakedroots == 1) {
                fprintf(stderr,
"JS engine warning: 1 GC root remains after destroying the JSRuntime.\n"
"                   This root may point to freed memory. Objects reachable\n"
"                   through it have not been finalized.\n");
            } else {
                fprintf(stderr,
"JS engine warning: %lu GC roots remain after destroying the JSRuntime.\n"
"                   These roots may point to freed memory. Objects reachable\n"
"                   through them have not been finalized.\n",
                        (unsigned long) leakedroots);
            }
        }
#endif

        JS_DHashTableFinish(&rt->gcRootsHash);
        rt->gcRootsHash.ops = NULL;
    }
    if (rt->gcLocksHash) {
        JS_DHashTableDestroy(rt->gcLocksHash);
        rt->gcLocksHash = NULL;
    }
}

JSBool
js_AddRoot(JSContext *cx, void *rp, const char *name)
{
    JSBool ok = js_AddRootRT(cx->runtime, rp, name);
    if (!ok)
        JS_ReportOutOfMemory(cx);
    return ok;
}

JSBool
js_AddRootRT(JSRuntime *rt, void *rp, const char *name)
{
    JSBool ok;
    JSGCRootHashEntry *rhe;

    /*
     * Due to the long-standing, but now removed, use of rt->gcLock across the
     * bulk of js_GC, API users have come to depend on JS_AddRoot etc. locking
     * properly with a racing GC, without calling JS_AddRoot from a request.
     * We have to preserve API compatibility here, now that we avoid holding
     * rt->gcLock across the mark phase (including the root hashtable mark).
     *
     * If the GC is running and we're called on another thread, wait for this
     * GC activation to finish.  We can safely wait here (in the case where we
     * are called within a request on another thread's context) without fear
     * of deadlock because the GC doesn't set rt->gcRunning until after it has
     * waited for all active requests to end.
     */
    JS_LOCK_GC(rt);
#ifdef JS_THREADSAFE
    JS_ASSERT(!rt->gcRunning || rt->gcLevel > 0);
    if (rt->gcRunning && rt->gcThread != js_CurrentThreadId()) {
        do {
            JS_AWAIT_GC_DONE(rt);
        } while (rt->gcLevel > 0);
    }
#endif
    rhe = (JSGCRootHashEntry *) JS_DHashTableOperate(&rt->gcRootsHash, rp,
                                                     JS_DHASH_ADD);
    if (rhe) {
        rhe->root = rp;
        rhe->name = name;
        ok = JS_TRUE;
    } else {
        ok = JS_FALSE;
    }
    JS_UNLOCK_GC(rt);
    return ok;
}

JSBool
js_RemoveRoot(JSRuntime *rt, void *rp)
{
    /*
     * Due to the JS_RemoveRootRT API, we may be called outside of a request.
     * Same synchronization drill as above in js_AddRoot.
     */
    JS_LOCK_GC(rt);
#ifdef JS_THREADSAFE
    JS_ASSERT(!rt->gcRunning || rt->gcLevel > 0);
    if (rt->gcRunning && rt->gcThread != js_CurrentThreadId()) {
        do {
            JS_AWAIT_GC_DONE(rt);
        } while (rt->gcLevel > 0);
    }
#endif
    (void) JS_DHashTableOperate(&rt->gcRootsHash, rp, JS_DHASH_REMOVE);
    rt->gcPoke = JS_TRUE;
    JS_UNLOCK_GC(rt);
    return JS_TRUE;
}

#ifdef DEBUG_brendan
#define NGCHIST 64

static struct GCHist {
    JSBool      lastDitch;
    JSGCThing   *freeList;
} gchist[NGCHIST];

unsigned gchpos;
#endif

void *
js_NewGCThing(JSContext *cx, uintN flags, size_t nbytes)
{
    JSBool tried_gc;
    JSRuntime *rt;
    size_t nflags;
    uintN i;
    JSGCThing *thing, **flp;
    uint8 *flagp;
    JSLocalRootStack *lrs;
    uint32 *bytesptr;

    rt = cx->runtime;
    JS_LOCK_GC(rt);
    JS_ASSERT(!rt->gcRunning);
    if (rt->gcRunning) {
        METER(rt->gcStats.finalfail++);
        JS_UNLOCK_GC(rt);
        return NULL;
    }

#ifdef TOO_MUCH_GC
#ifdef WAY_TOO_MUCH_GC
    rt->gcPoke = JS_TRUE;
#endif
    js_GC(cx, GC_KEEP_ATOMS | GC_ALREADY_LOCKED);
    tried_gc = JS_TRUE;
#else
    tried_gc = JS_FALSE;
#endif

    METER(rt->gcStats.alloc++);
    nbytes = JS_ROUNDUP(nbytes, sizeof(JSGCThing));
    nflags = nbytes / sizeof(JSGCThing);
    i = GC_FREELIST_INDEX(nbytes);
    flp = &rt->gcFreeList[i];

retry:
    thing = *flp;
    if (thing) {
        *flp = thing->next;
        flagp = thing->flagp;
        METER(rt->gcStats.freelen[i]--);
        METER(rt->gcStats.recycle[i]++);
    } else {
        if (rt->gcBytes < rt->gcMaxBytes &&
            (tried_gc || rt->gcMallocBytes < rt->gcMaxMallocBytes))
        {
            /*
             * Inline form of JS_ARENA_ALLOCATE adapted to truncate the current
             * arena's limit to a GC_PAGE_SIZE boundary, and to skip over every
             * GC_PAGE_SIZE-byte-aligned thing (which is actually not a thing,
             * it's a JSGCPageInfo record).
             */
            JSArenaPool *pool = &rt->gcArenaPool[i];
            JSArena *a = pool->current;
            jsuword p = a->avail;
            jsuword q = p + nbytes;

            if (q > (a->limit & ~GC_PAGE_MASK)) {
                thing = gc_new_arena(pool, nbytes);
            } else {
                if ((p & GC_PAGE_MASK) == 0) {
                    /* Beware, p points to a JSGCPageInfo record! */
                    p = FIRST_THING(p, nbytes);
                    q = p + nbytes;
                    JS_ArenaCountAllocation(pool, p & GC_PAGE_MASK);
                }
                a->avail = q;
                thing = (JSGCThing *)p;
            }
            JS_ArenaCountAllocation(pool, nbytes);
        }

        /*
         * Consider doing a "last ditch" GC if thing couldn't be allocated.
         *
         * Keep rt->gcLock across the call into js_GC so we don't starve and
         * lose to racing threads who deplete the heap just after js_GC has
         * replenished it (or has synchronized with a racing GC that collected
         * a bunch of garbage).  This unfair scheduling can happen on certain
         * operating systems.  For the gory details, see Mozilla bug 162779
         * (http://bugzilla.mozilla.org/show_bug.cgi?id=162779).
         */
        if (!thing) {
            if (!tried_gc) {
                rt->gcPoke = JS_TRUE;
                js_GC(cx, GC_KEEP_ATOMS | GC_ALREADY_LOCKED);
                tried_gc = JS_TRUE;
                METER(rt->gcStats.retry++);
                goto retry;
            }
            goto fail;
        }

        /* Find the flags pointer given thing's address. */
        flagp = js_GetGCThingFlags(thing);
    }

    lrs = cx->localRootStack;
    if (lrs) {
        /*
         * If we're in a local root scope, don't set cx->newborn[type] at all,
         * to avoid entraining garbage from it for an unbounded amount of time
         * on this context.  A caller will leave the local root scope and pop
         * this reference, allowing thing to be GC'd if it has no other refs.
         * See JS_EnterLocalRootScope and related APIs.
         */
        if (js_PushLocalRoot(cx, lrs, (jsval) thing) < 0) {
            /*
             * When we fail for a thing allocated through the tail of
             * the last arena, thing's flag byte is not initialized. So
             * to prevent GC accessing the uninitialized flags during
             * the finalization, we always mark the thing as final. See
             * bug 337407.
             */
            *flagp = GCF_FINAL;
            goto fail;
        }
    } else {
        /*
         * No local root scope, so we're stuck with the old, fragile model of
         * depending on a pigeon-hole newborn per type per context.
         */
        cx->newborn[flags & GCF_TYPEMASK] = thing;
    }

    /* We can't fail now, so update flags and rt->gc{,Private}Bytes. */
    *flagp = (uint8)flags;
    bytesptr = ((flags & GCF_TYPEMASK) == GCX_PRIVATE)
               ? &rt->gcPrivateBytes
               : &rt->gcBytes;
    *bytesptr += nbytes + nflags;

    /*
     * Clear thing before unlocking in case a GC run is about to scan it,
     * finding it via cx->newborn[].
     */
    thing->next = NULL;
    thing->flagp = NULL;
#ifdef DEBUG_brendan
    gchist[gchpos].lastDitch = tried_gc;
    gchist[gchpos].freeList = *flp;
    if (++gchpos == NGCHIST)
        gchpos = 0;
#endif
    METER(if (flags & GCF_LOCK) rt->gcStats.lockborn++);
    JS_UNLOCK_GC(rt);
    return thing;

fail:
    METER(rt->gcStats.fail++);
    JS_UNLOCK_GC(rt);
    JS_ReportOutOfMemory(cx);
    return NULL;
}

JSBool
js_LockGCThing(JSContext *cx, void *thing)
{
    JSBool ok = js_LockGCThingRT(cx->runtime, thing);
    if (!ok)
        JS_ReportOutOfMemory(cx);
    return ok;
}

/*
 * Deep GC-things can't be locked just by setting the GCF_LOCK bit, because
 * their descendants must be marked by the GC.  To find them during the mark
 * phase, they are added to rt->gcLocksHash, which is created lazily.
 *
 * NB: we depend on the order of GC-thing type indexes here!
 */
#define GC_TYPE_IS_STRING(t)    ((t) == GCX_STRING ||                         \
                                 (t) >= GCX_EXTERNAL_STRING)
#define GC_TYPE_IS_XML(t)       ((unsigned)((t) - GCX_NAMESPACE) <=           \
                                 (unsigned)(GCX_XML - GCX_NAMESPACE))
#define GC_TYPE_IS_DEEP(t)      ((t) == GCX_OBJECT || GC_TYPE_IS_XML(t))

#define IS_DEEP_STRING(t,o)     (GC_TYPE_IS_STRING(t) &&                      \
                                 JSSTRING_IS_DEPENDENT((JSString *)(o)))

#define GC_THING_IS_DEEP(t,o)   (GC_TYPE_IS_DEEP(t) || IS_DEEP_STRING(t, o))

JSBool
js_LockGCThingRT(JSRuntime *rt, void *thing)
{
    JSBool ok, deep;
    uint8 *flagp, flags, lock, type;
    JSGCLockHashEntry *lhe;

    ok = JS_TRUE;
    if (!thing)
        return ok;

    flagp = js_GetGCThingFlags(thing);

    JS_LOCK_GC(rt);
    flags = *flagp;
    lock = (flags & GCF_LOCK);
    type = (flags & GCF_TYPEMASK);
    deep = GC_THING_IS_DEEP(type, thing);

    /*
     * Avoid adding a rt->gcLocksHash entry for shallow things until someone
     * nests a lock -- then start such an entry with a count of 2, not 1.
     */
    if (lock || deep) {
        if (!rt->gcLocksHash) {
            rt->gcLocksHash =
                JS_NewDHashTable(JS_DHashGetStubOps(), NULL,
                                 sizeof(JSGCLockHashEntry),
                                 GC_ROOTS_SIZE);
            if (!rt->gcLocksHash) {
                ok = JS_FALSE;
                goto done;
            }
        } else if (lock == 0) {
#ifdef DEBUG
            JSDHashEntryHdr *hdr =
                JS_DHashTableOperate(rt->gcLocksHash, thing,
                                     JS_DHASH_LOOKUP);
            JS_ASSERT(JS_DHASH_ENTRY_IS_FREE(hdr));
#endif
        }

        lhe = (JSGCLockHashEntry *)
            JS_DHashTableOperate(rt->gcLocksHash, thing, JS_DHASH_ADD);
        if (!lhe) {
            ok = JS_FALSE;
            goto done;
        }
        if (!lhe->thing) {
            lhe->thing = thing;
            lhe->count = deep ? 1 : 2;
        } else {
            JS_ASSERT(lhe->count >= 1);
            lhe->count++;
        }
    }

    *flagp = (uint8)(flags | GCF_LOCK);
    METER(rt->gcStats.lock++);
    ok = JS_TRUE;
done:
    JS_UNLOCK_GC(rt);
    return ok;
}

JSBool
js_UnlockGCThingRT(JSRuntime *rt, void *thing)
{
    uint8 *flagp, flags;
    JSGCLockHashEntry *lhe;

    if (!thing)
        return JS_TRUE;

    flagp = js_GetGCThingFlags(thing);
    JS_LOCK_GC(rt);
    flags = *flagp;

    if (flags & GCF_LOCK) {
        if (!rt->gcLocksHash ||
            (lhe = (JSGCLockHashEntry *)
                   JS_DHashTableOperate(rt->gcLocksHash, thing,
                                        JS_DHASH_LOOKUP),
             JS_DHASH_ENTRY_IS_FREE(&lhe->hdr))) {
            /* Shallow GC-thing with an implicit lock count of 1. */
            JS_ASSERT(!GC_THING_IS_DEEP(flags & GCF_TYPEMASK, thing));
        } else {
            /* Basis or nested unlock of a deep thing, or nested of shallow. */
            if (--lhe->count != 0)
                goto out;
            JS_DHashTableOperate(rt->gcLocksHash, thing, JS_DHASH_REMOVE);
        }
        *flagp = (uint8)(flags & ~GCF_LOCK);
    }

    rt->gcPoke = JS_TRUE;
out:
    METER(rt->gcStats.unlock++);
    JS_UNLOCK_GC(rt);
    return JS_TRUE;
}

#ifdef GC_MARK_DEBUG

#include <stdio.h>
#include "jsprf.h"

JS_FRIEND_DATA(FILE *) js_DumpGCHeap;
JS_EXPORT_DATA(void *) js_LiveThingToFind;

#ifdef HAVE_XPCONNECT
#include "dump_xpc.h"
#endif

static const char *
gc_object_class_name(void* thing)
{
    uint8 *flagp = js_GetGCThingFlags(thing);
    const char *className = "";
    static char depbuf[32];

    switch (*flagp & GCF_TYPEMASK) {
      case GCX_OBJECT: {
        JSObject  *obj = (JSObject *)thing;
        JSClass   *clasp = JSVAL_TO_PRIVATE(obj->slots[JSSLOT_CLASS]);
        className = clasp->name;
#ifdef HAVE_XPCONNECT
        if (clasp->flags & JSCLASS_PRIVATE_IS_NSISUPPORTS) {
            jsval privateValue = obj->slots[JSSLOT_PRIVATE];

            JS_ASSERT(clasp->flags & JSCLASS_HAS_PRIVATE);
            if (!JSVAL_IS_VOID(privateValue)) {
                void  *privateThing = JSVAL_TO_PRIVATE(privateValue);
                const char *xpcClassName = GetXPCObjectClassName(privateThing);

                if (xpcClassName)
                    className = xpcClassName;
            }
        }
#endif
        break;
      }

      case GCX_STRING:
      case GCX_MUTABLE_STRING: {
        JSString *str = (JSString *)thing;
        if (JSSTRING_IS_DEPENDENT(str)) {
            JS_snprintf(depbuf, sizeof depbuf, "start:%u, length:%u",
                        JSSTRDEP_START(str), JSSTRDEP_LENGTH(str));
            className = depbuf;
        } else {
            className = "string";
        }
        break;
      }

      case GCX_DOUBLE:
        className = "double";
        break;
    }

    return className;
}

static void
gc_dump_thing(JSGCThing *thing, uint8 flags, GCMarkNode *prev, FILE *fp)
{
    GCMarkNode *next = NULL;
    char *path = NULL;

    while (prev) {
        next = prev;
        prev = prev->prev;
    }
    while (next) {
        uint8 nextFlags = *js_GetGCThingFlags(next->thing);
        if ((nextFlags & GCF_TYPEMASK) == GCX_OBJECT) {
            path = JS_sprintf_append(path, "%s(%s @ 0x%08p).",
                                     next->name,
                                     gc_object_class_name(next->thing),
                                     (JSObject*)next->thing);
        } else {
            path = JS_sprintf_append(path, "%s(%s).",
                                     next->name,
                                     gc_object_class_name(next->thing));
        }
        next = next->next;
    }
    if (!path)
        return;

    fprintf(fp, "%08lx ", (long)thing);
    switch (flags & GCF_TYPEMASK) {
      case GCX_OBJECT:
      {
        JSObject  *obj = (JSObject *)thing;
        jsval     privateValue = obj->slots[JSSLOT_PRIVATE];
        void      *privateThing = JSVAL_IS_VOID(privateValue)
                                  ? NULL
                                  : JSVAL_TO_PRIVATE(privateValue);
        const char *className = gc_object_class_name(thing);
        fprintf(fp, "object %8p %s", privateThing, className);
        break;
      }
#if JS_HAS_XML_SUPPORT
      case GCX_NAMESPACE:
      {
        JSXMLNamespace *ns = (JSXMLNamespace *)thing;
        fprintf(fp, "namespace %s:%s",
                JS_GetStringBytes(ns->prefix), JS_GetStringBytes(ns->uri));
        break;
      }
      case GCX_QNAME:
      {
        JSXMLQName *qn = (JSXMLQName *)thing;
        fprintf(fp, "qname %s(%s):%s",
                JS_GetStringBytes(qn->prefix), JS_GetStringBytes(qn->uri),
                JS_GetStringBytes(qn->localName));
        break;
      }
      case GCX_XML:
      {
        extern const char *js_xml_class_str[];
        JSXML *xml = (JSXML *)thing;
        fprintf(fp, "xml %8p %s", xml, js_xml_class_str[xml->xml_class]);
        break;
      }
#endif
      case GCX_DOUBLE:
        fprintf(fp, "double %g", *(jsdouble *)thing);
        break;
      case GCX_PRIVATE:
        fprintf(fp, "private %8p", (void *)thing);
        break;
      default:
        fprintf(fp, "string %s", JS_GetStringBytes((JSString *)thing));
        break;
    }
    fprintf(fp, " via %s\n", path);
    free(path);
}

#endif /* !GC_MARK_DEBUG */

static void
gc_mark_atom_key_thing(void *thing, void *arg)
{
    JSContext *cx = (JSContext *) arg;

    GC_MARK(cx, thing, "atom", NULL);
}

void
js_MarkAtom(JSContext *cx, JSAtom *atom, void *arg)
{
    jsval key;

    if (atom->flags & A