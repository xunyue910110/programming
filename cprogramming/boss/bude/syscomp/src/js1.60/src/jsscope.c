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
 * JS symbol tables.
 */
#include "jsstddef.h"
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h"
#include "jsbit.h"
#include "jsclist.h"
#include "jsdhash.h"
#include "jsutil.h" /* Added by JSIFY */
#include "jsapi.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsdbgapi.h"
#include "jslock.h"
#include "jsnum.h"
#include "jsscope.h"
#include "jsstr.h"

JSScope *
js_GetMutableScope(JSContext *cx, JSObject *obj)
{
    JSScope *scope, *newscope;

    scope = OBJ_SCOPE(obj);
    JS_ASSERT(JS_IS_SCOPE_LOCKED(cx, scope));
    if (scope->object == obj)
        return scope;
    newscope = js_NewScope(cx, 0, scope->map.ops, LOCKED_OBJ_GET_CLASS(obj),
                           obj);
    if (!newscope)
        return NULL;
    JS_LOCK_SCOPE(cx, newscope);
    obj->map = js_HoldObjectMap(cx, &newscope->map);
    scope = (JSScope *) js_DropObjectMap(cx, &scope->map, obj);
    JS_TRANSFER_SCOPE_LOCK(cx, scope, newscope);
    return newscope;
}

/*
 * JSScope uses multiplicative hashing, _a la_ jsdhash.[ch], but specialized
 * to minimize footprint.  But if a scope has fewer than SCOPE_HASH_THRESHOLD
 * entries, we use linear search and avoid allocating scope->table.
 */
#define SCOPE_HASH_THRESHOLD    6
#define MIN_SCOPE_SIZE_LOG2     4
#define MIN_SCOPE_SIZE          JS_BIT(MIN_SCOPE_SIZE_LOG2)
#define SCOPE_TABLE_NBYTES(n)   ((n) * sizeof(JSScopeProperty *))

static void
InitMinimalScope(JSScope *scope)
{
    scope->hashShift = JS_DHASH_BITS - MIN_SCOPE_SIZE_LOG2;
    scope->entryCount = scope->removedCount = 0;
    scope->table = NULL;
    scope->lastProp = NULL;
}

static JSBool
CreateScopeTable(JSContext *cx, JSScope *scope, JSBool report)
{
    int sizeLog2;
    JSScopeProperty *sprop, **spp;

    JS_ASSERT(!scope->table);
    JS_ASSERT(scope->lastProp);

    if (scope->entryCount > SCOPE_HASH_THRESHOLD) {
        /*
         * Ouch: calloc failed at least once already -- let's try again,
         * overallocating to hold at least twice the current population.
         */
        sizeLog2 = JS_CeilingLog2(2 * scope->entryCount);
        scope->hashShift = JS_DHASH_BITS - sizeLog2;
    } else {
        JS_ASSERT(scope->hashShift == JS_DHASH_BITS - MIN_SCOPE_SIZE_LOG2);
        sizeLog2 = MIN_SCOPE_SIZE_LOG2;
    }

    scope->table = (JSScopeProperty **)
        calloc(JS_BIT(sizeLog2), sizeof(JSScopeProperty *));
    if (!scope->table) {
        if (report)
            JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    /* Racy update after calloc, to help keep the GC self-scheduled well. */
    cx->runtime->gcMallocBytes += JS_BIT(sizeLog2) * sizeof(JSScopeProperty *);

    scope->hashShift = JS_DHASH_BITS - sizeLog2;
    for (sprop = scope->lastProp; sprop; sprop = sprop->parent) {
        spp = js_SearchScope(scope, sprop->id, JS_TRUE);
        SPROP_STORE_PRESERVING_COLLISION(spp, sprop);
    }
    return JS_TRUE;
}

JSScope *
js_NewScope(JSContext *cx, jsrefcount nrefs, JSObjectOps *ops, JSClass *clasp,
            JSObject *obj)
{
    JSScope *scope;

    scope = (JSScope *) JS_malloc(cx, sizeof(JSScope));
    if (!scope)
        return NULL;

    js_InitObjectMap(&scope->map, nrefs, ops, clasp);
    scope->object = obj;
    scope->flags = 0;
    scope->dswIndex = 0;
    InitMinimalScope(scope);

#ifdef JS_THREADSAFE
    scope->ownercx = cx;
    memset(&scope->lock, 0, sizeof scope->lock);

    /*
     * Set u.link = NULL, not u.count = 0, in case the target architecture's
     * null pointer has a non-zero integer representation.
     */
    scope->u.link = NULL;

#ifdef DEBUG
    scope->file[0] = scope->file[1] = scope->file[2] = scope->file[3] = NULL;
    scope->line[0] = scope->line[1] = scope->line[2] = scope->line[3] = 0;
#endif
#endif

    JS_RUNTIME_METER(cx->runtime, liveScopes);
    JS_RUNTIME_METER(cx->runtime, totalScopes);
    return scope;
}

#ifdef DEBUG_SCOPE_COUNT
extern void
js_unlog_scope(JSScope *scope);
#endif

void
js_DestroyScope(JSContext *cx, JSScope *scope)
{
#ifdef DEBUG_SCOPE_COUNT
    js_unlog_scope(scope);
#endif

#ifdef JS_THREADSAFE
    /* Scope must be single-threaded at this point, so set scope->ownercx. */
    JS_ASSERT(scope->u.count == 0);
    scope->ownercx = cx;
    js_FinishLock(&scope->lock);
#endif
    if (scope->table)
        JS_free(cx, scope->table);

#ifdef DEBUG
    JS_LOCK_RUNTIME_VOID(cx->runtime,
                         cx->runtime->liveScopeProps -= scope->entryCount);
#endif
    JS_RUNTIME_UNMETER(cx->runtime, liveScopes);
    JS_free(cx, scope);
}

#ifdef DUMP_SCOPE_STATS
typedef struct JSScopeStats {
    jsrefcount          searches;
    jsrefcount          steps;
    jsrefcount          hits;
    jsrefcount          misses;
    jsrefcount          stepHits;
    jsrefcount          stepMisses;
    jsrefcount          adds;
    jsrefcount          redundantAdds;
    jsrefcount          addFailures;
    jsrefcount          changeFailures;
    jsrefcount          compresses;
    jsrefcount          grows;
    jsrefcount          removes;
    jsrefcount          removeFrees;
    jsrefcount          uselessRemoves;
    jsrefcount          shrinks;
} JSScopeStats;

JS_FRIEND_DATA(JSScopeStats) js_scope_stats;

# define METER(x)       JS_ATOMIC_INCREMENT(&js_scope_stats.x)
#else
# define METER(x)       /* nothing */
#endif

/*
 * Double hashing needs the second hash code to be relatively prime to table
 * size, so we simply make hash2 odd.  The inputs to multiplicative hash are
 * the golden ratio, expressed as a fixed-point 32 bit fraction, and the int
 * property index or named property's atom number (observe that most objects
 * have either no indexed properties, or almost all indexed and a few names,
 * so collisions between index and atom number are unlikely).
 */
#define SCOPE_HASH0(id)                 (HASH_ID(id) * JS_GOLDEN_RATIO)
#define SCOPE_HASH1(hash0,shift)        ((hash0) >> (shift))
#define SCOPE_HASH2(hash0,log2,shift)   ((((hash0) << (log2)) >> (shift)) | 1)

JS_FRIEND_API(JSScopeProperty **)
js_SearchScope(JSScope *scope, jsid id, JSBool adding)
{
    JSHashNumber hash0, hash1, hash2;
    int hashShift, sizeLog2;
    JSScopeProperty *stored, *sprop, **spp, **firstRemoved;
    uint32 sizeMask;

    METER(searches);
    if (!scope->table) {
        /* Not enough properties to justify hashing: search from lastProp. */
        JS_ASSERT(!SCOPE_HAD_MIDDLE_DELETE(scope));
        for (spp = &scope->lastProp; (sprop = *spp); spp = &sprop->parent) {
            if (sprop->id == id) {
                METER(hits);
                return spp;
            }
        }
        METER(misses);
        return spp;
    }

    /* Compute the primary hash address. */
    hash0 = SCOPE_HASH0(id);
    hashShift = scope->hashShift;
    hash1 = SCOPE_HASH1(hash0, hashShift);
    spp = scope->table + hash1;

    /* Miss: return space for a new entry. */
    stored = *spp;
    if (SPROP_IS_FREE(stored)) {
        METER(misses);
        return spp;
    }

    /* Hit: return entry. */
    sprop = SPROP_CLEAR_COLLISION(stored);
    if (sprop && sprop->id == id) {
        METER(hits);
        return spp;
    }

    /* Collision: double hash. */
    sizeLog2 = JS_DHASH_BITS - hashShift;
    hash2 = SCOPE_HASH2(hash0, sizeLog2, hashShift);
    sizeMask = JS_BITMASK(sizeLog2);

    /* Save the first removed entry pointer so we can recycle it if adding. */
    if (SPROP_IS_REMOVED(stored)) {
        firstRemoved = spp;
    } else {
        firstRemoved = NULL;
        if (adding && !SPROP_HAD_COLLISION(stored))
            SPROP_FLAG_COLLISION(spp, sprop);
    }

    for (;;) {
        METER(steps);
        hash1 -= hash2;
        hash1 &= sizeMask;
        spp = scope->table + hash1;

        stored = *spp;
        if (SPROP_IS_FREE(stored)) {
            METER(stepMisses);
            return (adding && firstRemoved) ? firstRemoved : spp;
        }

        sprop = SPROP_CLEAR_COLLISION(stored);
        if (sprop && sprop->id == id) {
            METER(stepHits);
            return spp;
        }

        if (SPROP_IS_REMOVED(stored)) {
            if (!firstRemoved)
                firstRemoved = spp;
        } else {
            if (adding && !SPROP_HAD_COLLISION(stored))
                SPROP_FLAG_COLLISION(spp, sprop);
        }
    }

    /* NOTREACHED */
    return NULL;
}

static JSBool
ChangeScope(JSContext *cx, JSScope *scope, int change)
{
    int oldlog2, newlog2;
    uint32 oldsize, newsize, nbytes;
    JSScopeProperty **table, **oldtable, **spp, **oldspp, *sprop;

    /* Grow, shrink, or compress by changing scope->table. */
    oldlog2 = JS_DHASH_BITS - scope->hashShift;
    newlog2 = oldlog2 + change;
    oldsize = JS_BIT(oldlog2);
    newsize = JS_BIT(newlog2);
    nbytes = SCOPE_TABLE_NBYTES(newsize);
    table = (JSScopeProperty **) calloc(nbytes, 1);
    if (!table) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    /* Now that we have a new table allocated, update scope members. */
    scope->hashShift = JS_DHASH_BITS - newlog2;
    scope->removedCount = 0;
    oldtable = scope->table;
    scope->table = table;

    /* Treat the above calloc as a JS_malloc, to match CreateScopeTable. */
    cx->runtime->gcMallocBytes += nbytes;

    /* Copy only live entries, leaving removed and free ones behind. */
    for (oldspp = oldtable; oldsize != 0; oldspp++) {
        sprop = SPROP_FETCH(oldspp);
        if (sprop) {
            spp = js_SearchScope(scope, sprop->id, JS_TRUE);
            JS_ASSERT(SPROP_IS_FREE(*spp));
            *spp = sprop;
        }
        oldsize--;
    }

    /* Finally, free the old table storage. */
    JS_free(cx, oldtable);
    return JS_TRUE;
}

/*
 * Take care to exclude the mark and duplicate bits, in case we're called from
 * the GC, or we are searching for a property that has not yet been flagged as
 * a duplicate when making a duplicate formal parameter.
 */
#define SPROP_FLAGS_NOT_MATCHED (SPROP_MARK | SPROP_IS_DUPLICATE)

JS_STATIC_DLL_CALLBACK(JSDHashNumber)
js_HashScopeProperty(JSDHashTable *table, const void *key)
{
    const JSScopeProperty *sprop = (const JSScopeProperty *)key;
    JSDHashNumber hash;
    JSPropertyOp gsop;

    /* Accumulate from least to most random so the low bits are most random. */
    hash = 0;
    gsop = sprop->getter;
    if (gsop)
        hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4) ^ (jsword)gsop;
    gsop = sprop->setter;
    if (gsop)
        hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4) ^ (jsword)gsop;

    hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4)
           ^ (sprop->flags & ~SPROP_FLAGS_NOT_MATCHED);

    hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4) ^ sprop->attrs;
    hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4) ^ sprop->shortid;
    hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4) ^ sprop->slot;
    hash = (hash >> (JS_DHASH_BITS - 4)) ^ (hash << 4) ^ sprop->id;
    return hash;
}

#define SPROP_MATCH(sprop, child)                                             \
    SPROP_MATCH_PARAMS(sprop, (child)->id, (child)->getter, (child)->setter,  \
                       (child)->slot, (child)->attrs, (child)->flags,         \
                       (child)->shortid)

#define SPROP_MATCH_PARAMS(sprop, aid, agetter, asetter, aslot, aattrs,       \
                           aflags, ashortid)                                  \
    ((sprop)->id == (aid) &&                                                  \
     SPROP_MATCH_PARAMS_AFTER_ID(sprop, agetter, asetter, aslot, aattrs,      \
                                 aflags, ashortid))

#define SPROP_MATCH_PARAMS_AFTER_ID(sprop, agetter, asetter, aslot, aattrs,   \
                                    aflags, ashortid)                         \
    ((sprop)->getter == (agetter) &&                                          \
     (sprop)->setter == (asetter) &&                                          \
     (sprop)->slot == (aslot) &&                                              \
     (sprop)->attrs == (aattrs) &&                                            \
     (((sprop)->flags ^ (aflags)) & ~SPROP_FLAGS_NOT_MATCHED) == 0 &&         \
     (sprop)->shortid == (ashortid))

JS_STATIC_DLL_CALLBACK(JSBool)
js_MatchScopeProperty(JSDHashTable *table,
                      const JSDHashEntryHdr *hdr,
                      const void *key)
{
    const JSPropertyTreeEntry *entry = (const JSPropertyTreeEntry *)hdr;
    const JSScopeProperty *sprop = entry->child;
    const JSScopeProperty *kprop = (const JSScopeProperty *)key;

    return SPROP_MATCH(sprop, kprop);
}

static const JSDHashTableOps PropertyTreeHashOps = {
    JS_DHashAllocTable,
    JS_DHashFreeTable,
    JS_DHashGetKeyStub,
    js_HashScopeProperty,
    js_MatchScopeProperty,
    JS_DHashMoveEntryStub,
    JS_DHashClearEntryStub,
    JS_DHashFinalizeStub,
    NULL
};

/*
 * A property tree node on rt->propertyFreeList overlays the following prefix
 * struct on JSScopeProperty.
 */
typedef struct FreeNode {
    jsid                id;
    JSScopeProperty     *next;
    JSScopeProperty     **prevp;
} FreeNode;

#define FREENODE(sprop) ((FreeNode *) (sprop))

#define FREENODE_INSERT(list, sprop)                                          \
    JS_BEGIN_MACRO                                                            \
        FREENODE(sprop)->next = (list);                                       \
        FREENODE(sprop)->prevp = &(list);                                     \
        if (list)                                                             \
            FREENODE(list)->prevp = &FREENODE(sprop)->next;                   \
        (list) = (sprop);                                                     \
    JS_END_MACRO

#define FREENODE_REMOVE(sprop)                                                \
    JS_BEGIN_MACRO                                                            \
        *FREENODE(sprop)->prevp = FREENODE(sprop)->next;                      \
        if (FREENODE(sprop)->next)                                            \
            FREENODE(FREENODE(sprop)->next)->prevp = FREENODE(sprop)->prevp;  \
    JS_END_MACRO

/* NB: Called with the runtime lock held. */
static JSScopeProperty *
NewScopeProperty(JSRuntime *rt)
{
    JSScopeProperty *sprop;

    sprop = rt->propertyFreeList;
    if (sprop) {
        FREENODE_REMOVE(sprop);
    } else {
        JS_ARENA_ALLOCATE_CAST(sprop, JSScopeProperty *,
                               &rt->propertyArenaPool,
                               sizeof(JSScopeProperty));
        if (!sprop)
            return NULL;
    }

    JS_RUNTIME_METER(rt, livePropTreeNodes);
    JS_RUNTIME_METER(rt, totalPropTreeNodes);
    return sprop;
}

#define CHUNKY_KIDS_TAG         ((jsuword)1)
#define KIDS_IS_CHUNKY(kids)    ((jsuword)(kids) & CHUNKY_KIDS_TAG)
#define KIDS_TO_CHUNK(kids)     ((PropTreeKidsChunk *)                        \
                                 ((jsuword)(kids) & ~CHUNKY_KIDS_TAG))
#define CHUNK_TO_KIDS(chunk)    ((JSScopeProperty *)                          \
                                 ((jsuword)(chunk) | CHUNKY_KIDS_TAG))
#define MAX_KIDS_PER_CHUNK      10

typedef struct PropTreeKidsChunk PropTreeKidsChunk;

struct PropTreeKidsChunk {
    JSScopeProperty     *kids[MAX_KIDS_PER_CHUNK];
    PropTreeKidsChunk   *next;
};

static PropTreeKidsChunk *
NewPropTreeKidsChunk(JSRuntime *rt)
{
    PropTreeKidsChunk *chunk;

    chunk = calloc(1, sizeof *chunk);
    if (!chunk)
        return NULL;
    JS_ASSERT(((jsuword)chunk & CHUNKY_KIDS_TAG) == 0);
    JS_RUNTIME_METER(rt, propTreeKidsChunks);
    return chunk;
}

static void
DestroyPropTreeKidsChunk(JSRuntime *rt, PropTreeKidsChunk *chunk)
{
    JS_RUNTIME_UNMETER(rt, propTreeKidsChunks);
    free(chunk);
}

/* NB: Called with the runtime lock held. */
static JSBool
InsertPropertyTreeChild(JSRuntime *rt, JSScopeProperty *parent,
                        JSScopeProperty *child, PropTreeKidsChunk *sweptChunk)
{
    JSPropertyTreeEntry *entry;
    JSScopeProperty **childp, *kids, *sprop;
    PropTreeKidsChunk *chunk, **chunkp;
    uintN i;

    JS_ASSERT(!parent || child->parent != parent);

    if (!parent) {
        entry = (JSPropertyTreeEntry *)
            JS_DHashTableOperate(&rt->propertyTreeHash, child, JS_DHASH_ADD);
        if (!entry)
            return JS_FALSE;
        childp = &entry->child;
        sprop = *childp;
        if (!sprop) {
            *childp = child;
        } else {
            /*
             * A "Duplicate child" case.
             *
             * We can't do away with child, as at least one live scope entry
             * still points at it.  What's more, that scope's lastProp chains
             * through an ancestor line to reach child, and js_Enumerate and
             * others count on this linkage.  We must leave child out of the
             * hash table, and not require it to be there when we eventually
             * GC it (see RemovePropertyTreeChild, below).
             *
             * It is necessary to leave the duplicate child out of the hash
             * table to preserve entry uniqueness.  It is safe to leave the
             * child out of the hash table (unlike the duplicate child cases
             * below), because the child's parent link will be null, which
             * can't dangle.
             */
            JS_ASSERT(sprop != child && SPROP_MATCH(sprop, child));
            JS_RUNTIME_METER(rt, duplicatePropTreeNodes);
        }
    } else {
        childp = &parent->kids;
        kids = *childp;
        if (kids) {
            if (KIDS_IS_CHUNKY(kids)) {
                chunk = KIDS_TO_CHUNK(kids);
                do {
                    for (i = 0; i < MAX_KIDS_PER_CHUNK; i++) {
                        childp = &chunk->kids[i];
                        sprop = *childp;
                        if (!sprop)
                            goto insert;

                        JS_ASSERT(sprop != child);
                        if (SPROP_MATCH(sprop, child)) {
                            /*
                             * Duplicate child, see comment above.  In this
                             * case, we must let the duplicate be inserted at
                             * this level in the tree, so we keep iterating,
                             * looking for an empty slot in which to insert.
                             */
                            JS_ASSERT(sprop != child);
                            JS_RUNTIME_METER(rt, duplicatePropTreeNodes);
                        }
                    }
                    chunkp = &chunk->next;
                } while ((chunk = *chunkp) != NULL);

                if (sweptChunk) {
                    chunk = sweptChunk;
                } else {
                    chunk = NewPropTreeKidsChunk(rt);
                    if (!chunk)
                        return JS_FALSE;
                }
                *chunkp = chunk;
                childp = &chunk->kids[0];
            } else {
                sprop = kids;
                JS_ASSERT(sprop != child);
                if (SPROP_MATCH(sprop, child)) {
                    /*
                     * Duplicate child, see comment above.  Once again, we
                     * must let duplicates created by deletion pile up in a
                     * kids-chunk-list, in order to find them when sweeping
                     * and thereby avoid dangling parent pointers.
                     */
                    JS_RUNTIME_METER(rt, duplicatePropTreeNodes);
                }

                chunk = NewPropTreeKidsChunk(rt);
                if (!chunk)
                    return JS_FALSE;
                parent->kids = CHUNK_TO_KIDS(chunk);
                chunk->kids[0] = sprop;
                childp = &chunk->kids[1];
            }
        }
    insert:
        *childp = child;
    }

    child->parent = parent;
    return JS_TRUE;
}

/* NB: Called with the runtime lock held. */
static void
RemovePropertyTreeChild(JSRuntime *rt, JSScopeProperty *child)
{
    JSPropertyTreeEntry *entry;
    JSScopeProperty *parent, *kids, *kid;
    PropTreeKidsChunk *list, *chunk, **chunkp, *lastChunk;
    uintN i, j;

    parent = child->parent;
    if (!parent) {
        /*
         * Don't remove child if it is not in rt->propertyTreeHash, but only
         * matches a root child in the table that has compatible members. See
         * the "Duplicate child" comments in InsertPropertyTreeChild, above.
         */
        entry = (JSPropertyTreeEntry *)
            JS_DHashTableOperate(&rt->propertyTreeHash, child, JS_DHASH_LOOKUP);

        if (entry->child == child)
            JS_DHashTableRawRemove(&rt->propertyTreeHash, &entry->hdr);
    } else {
        kids = parent->kids;
        if (KIDS_IS_CHUNKY(kids)) {
            list = chunk = KIDS_TO_CHUNK(kids);
            chunkp = &list;

            do {
                for (i = 0; i < MAX_KIDS_PER_CHUNK; i++) {
                    if (chunk->kids[i] == child) {
                        lastChunk = chunk;
                        if (!lastChunk->next) {
                            j = i + 1;
                        } else {
                            j = 0;
                            do {
                                chunkp = &lastChunk->next;
                                lastChunk = *chunkp;
                            } while (lastChunk->next);
                        }
                        for (; j < MAX_KIDS_PER_CHUNK; j++) {
                            if (!lastChunk->kids[j])
                                break;
                        }
                        --j;
                        if (chunk != lastChunk || j > i)
                            chunk->kids[i] = lastChunk->kids[j];
                        lastChunk->kids[j] = NULL;
                        if (j == 0) {
                            *chunkp = NULL;
                            if (!list)
                                parent->kids = NULL;
                            DestroyPropTreeKidsChunk(rt, lastChunk);
                        }
                        return;
                    }
                }

                chunkp = &chunk->next;
            } while ((chunk = *chunkp) != NULL);
        } else {
            kid = kids;
            if (kid == child)
                parent->kids = NULL;
        }
    }
}

/*
 * Called *without* the runtime lock held, this function acquires that lock
 * only when inserting a new child.  Thus there may be races to find or add
 * a node that result in duplicates.  We expect such races to be rare!
 */
static JSScopeProperty *
GetPropertyTreeChild(JSContext *cx, JSScopeProperty *parent,
                     JSScopeProperty *child)
{
    JSRuntime *rt;
    JSPropertyTreeEntry *entry;
    JSScopeProperty *sprop;
    PropTreeKidsChunk *chunk;
    uintN i;

    rt = cx->runtime;
    if (!parent) {
        JS_LOCK_RUNTIME(rt);

        entry = (JSPropertyTreeEntry *)
            JS_DHashTableOperate(&rt->propertyTreeHash, child, JS_DHASH_ADD);
        if (!entry)
            goto out_of_memory;

        sprop = entry->child;
        if (sprop)
            goto out;
    } else {
        /*
         * Because chunks are appended at the end and never deleted except by
         * the GC, we can search without taking the runtime lock.  We may miss
         * a matching sprop added by another thread, and make a duplicate one,
         * but that is an unlikely, therefore small, cost.  The property tree
         * has extremely low fan-out below its root in popular embeddings with
         * real-world workloads.
         *
         * If workload changes so as to increase fan-out significantly below
         * the property tree root, we'll want to add another tag bit stored in
         * parent->kids that indicates a JSDHashTable pointer.
         */
        entry = NULL;
        sprop = parent->kids;
        if (sprop) {
            if (KIDS_IS_CHUNKY(sprop)) {
                chunk = KIDS_TO_CHUNK(sprop);
                do {
                    for (i = 0; i < MAX_KIDS_PER_CHUNK; i++) {
                        sprop = chunk->kids[i];
                        if (!sprop)
                            goto not_found;

                        if (SPROP_MATCH(sprop, child))
                            return sprop;
                    }
                } while ((chunk = chunk->next) != NULL);
            } else {
                if (SPROP_MATCH(sprop, child))
                    return sprop;
            }
        }

    not_found:
        JS_LOCK_RUNTIME(rt);
    }

    sprop = NewScopeProperty(rt);
    if (!sprop)
        goto out_of_memory;

    sprop->id = child->id;
    sprop->getter = child->getter;
    sprop->setter = child->setter;
    sprop->slot = child->slot;
    sprop->attrs = child->attrs;
    sprop->flags = child->flags;
    sprop->shortid = child->shortid;
    sprop->parent = sprop->kids = NULL;
    if (!parent) {
        entry->child = sprop;
    } else {
        if (!InsertPropertyTreeChild(rt, parent, sprop, NULL))
            goto out_of_memory;
    }

out:
    JS_UNLOCK_RUNTIME(rt);
    return sprop;

out_of_memory:
    JS_UNLOCK_RUNTIME(rt);
    JS_ReportOutOfMemory(cx);
    return NULL;
}

#ifdef DEBUG_notbrendan
#define CHECK_ANCESTOR_LINE(scope, sparse)                                    \
    JS_BEGIN_MACRO                                                            \
        if ((scope)->table) CheckAncestorLine(scope, sparse);                 \
    JS_END_MACRO

static void
CheckAncestorLine(JSScope *scope, JSBool sparse)
{
    uint32 size;
    JSScopeProperty **spp, **start, **end, *ancestorLine, *sprop, *aprop;
    uint32 entryCount, ancestorCount;

    ancestorLine = SCOPE_LAST_PROP(scope);
    if (ancestorLine)
        JS_ASSERT(SCOPE_HAS_PROPERTY(scope, ancestorLine));

    entryCount = 0;
    size = SCOPE_CAPACITY(scope);
    start = scope->table;
    for (spp = start, end = start + size; spp < end; spp++) {
        sprop = SPROP_FETCH(spp);
        if (sprop) {
            entryCount++;
            for (aprop = ancestorLine; aprop; aprop = aprop->parent) {
                if (aprop == sprop)
                    break;
            }
            JS_ASSERT(aprop);
        }
    }
    JS_ASSERT(entryCount == scope->entryCount);

    ancestorCount = 0;
    for (sprop = ancestorLine; sprop; sprop = sprop->parent) {
        if (SCOPE_HAD_MIDDLE_DELETE(scope) &&
            !SCOPE_HAS_PROPERTY(scope, sprop)) {
            JS_ASSERT(sparse || (sprop->flags & SPROP_IS_DUPLICATE));
            continue;
        }
        ancestorCount++;
    }
    JS_ASSERT(ancestorCount == scope->entryCount);
}
#else
#define CHECK_ANCESTOR_LINE(scope, sparse) /* nothing */
#endif

static void
ReportReadOnlyScope(JSContext *cx, JSScope *scope)
{
    JSString *str;

    str = js_ValueToString(cx, OBJECT_TO_JSVAL(scope->object));
    JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_READ_ONLY,
                         str
                         ? JS_GetStringBytes(str)
                         : LOCKED_OBJ_GET_CLASS(scope->object)->name);
}

JSScopeProperty *
js_AddScopeProperty(JSContext *cx, JSScope *scope, jsid id,
                    JSPropertyOp getter, JSPropertyOp setter, uint32 slot,
                    uintN attrs, uintN flags, intN shortid)
{
    JSScopeProperty **spp, *sprop, *overwriting, **spvec, **spp2, child;
    uint32 size, splen, i;
    int change;

    JS_ASSERT(JS_IS_SCOPE_LOCKED(cx, scope));
    CHECK_ANCESTOR_LINE(scope, JS_TRUE);

    /*
     * You can't add properties to a sealed scope.  But note well that you can
     * change property attributes in a sealed scope, even though that replaces
     * a JSScopeProperty * in the scope's hash table -- but no id is added, so
     * the scope remains sealed.
     */
    if (SCOPE_IS_SEALED(scope)) {
        ReportReadOnlyScope(cx, scope);
        return NULL;
    }

    /*
     * Normalize stub getter and setter values for faster is-stub testing in
     * the SPROP_CALL_[GS]ETTER macros.
     */
    if (getter == JS_PropertyStub)
        getter = NULL;
    if (setter == JS_PropertyStub)
        setter = NULL;

    /*
     * Search for id in order to claim its entry, allocating a property tree
     * node if one doesn't already exist for our parameters.
     */
    spp = js_SearchScope(scope, id, JS_TRUE);
    sprop = overwriting = SPROP_FETCH(spp);
    if (!sprop) {
        /* Check whether we need to grow, if the load factor is >= .75. */
        size = SCOPE_CAPACITY(scope);
        if (scope->entryCount + scope->removedCount >= size - (size >> 2)) {
            if (scope->removedCount >= size >> 2) {
                METER(compresses);
                change = 0;
            } else {
                METER(grows);
                change = 1;
            }
            if (!ChangeScope(cx, scope, change) &&
                scope->entryCount + scope->removedCount == size - 1) {
                METER(addFailures);
                return NULL;
            }
            spp = js_SearchScope(scope, id, JS_TRUE);
            JS_ASSERT(!SPROP_FETCH(spp));
        }
    } else {
        /* Property exists: js_SearchScope must have returned a valid entry. */
        JS_ASSERT(!SPROP_IS_REMOVED(*spp));

        /*
         * If all property members match, this is a redundant add and we can
         * return early.  If the caller wants to allocate a slot, but doesn't
         * care which slot, copy sprop->slot into slot so we can match sprop,
         * if all other members match.
         */
        if (!(attrs & JSPROP_SHARED) &&
            slot == SPROP_INVALID_SLOT &&
            SPROP_HAS_VALID_SLOT(sprop, scope)) {
            slot = sprop->slot;
        }
        if (SPROP_MATCH_PARAMS_AFTER_ID(sprop, getter, setter, slot, attrs,
                                        flags, shortid)) {
            METER(redundantAdds);
            return sprop;
        }

        /*
         * Duplicate formal parameters require us to leave the old property
         * on the ancestor line, so the decompiler can find it, even though
         * its entry in scope->table is overwritten to point at a new property
         * descending from the old one.  The SPROP_IS_DUPLICATE flag helps us
         * cope with the consequent disparity between ancestor line height and
         * scope->entryCount.
         */
        if (flags & SPROP_IS_DUPLICATE) {
            sprop->flags |= SPROP_IS_DUPLICATE;
        } else {
            /*
             * If we are clearing sprop to force an existing property to be
             * overwritten (apart from a duplicate formal parameter), we must
             * unlink it from the ancestor line at scope->lastProp, lazily if
             * sprop is not lastProp.  And we must remove the entry at *spp,
             * precisely so the lazy "middle delete" fixup code further below
             * won't find sprop in scope->table, in spite