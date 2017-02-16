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
 * JS bytecode generation.
 */
#include "jsstddef.h"
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <string.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsbit.h"
#include "jsprf.h"
#include "jsapi.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsemit.h"
#include "jsfun.h"
#include "jsnum.h"
#include "jsopcode.h"
#include "jsparse.h"
#include "jsregexp.h"
#include "jsscan.h"
#include "jsscope.h"
#include "jsscript.h"

/* Allocation chunk counts, must be powers of two in general. */
#define BYTECODE_CHUNK  256     /* code allocation increment */
#define SRCNOTE_CHUNK   64      /* initial srcnote allocation increment */
#define TRYNOTE_CHUNK   64      /* trynote allocation increment */

/* Macros to compute byte sizes from typed element counts. */
#define BYTECODE_SIZE(n)        ((n) * sizeof(jsbytecode))
#define SRCNOTE_SIZE(n)         ((n) * sizeof(jssrcnote))
#define TRYNOTE_SIZE(n)         ((n) * sizeof(JSTryNote))

JS_FRIEND_API(JSBool)
js_InitCodeGenerator(JSContext *cx, JSCodeGenerator *cg,
                     JSArenaPool *codePool, JSArenaPool *notePool,
                     const char *filename, uintN lineno,
                     JSPrincipals *principals)
{
    memset(cg, 0, sizeof *cg);
    TREE_CONTEXT_INIT(&cg->treeContext);
    cg->treeContext.flags |= TCF_COMPILING;
    cg->codePool = codePool;
    cg->notePool = notePool;
    cg->codeMark = JS_ARENA_MARK(codePool);
    cg->noteMark = JS_ARENA_MARK(notePool);
    cg->tempMark = JS_ARENA_MARK(&cx->tempPool);
    cg->current = &cg->main;
    cg->filename = filename;
    cg->firstLine = cg->prolog.currentLine = cg->main.currentLine = lineno;
    cg->principals = principals;
    ATOM_LIST_INIT(&cg->atomList);
    cg->prolog.noteMask = cg->main.noteMask = SRCNOTE_CHUNK - 1;
    ATOM_LIST_INIT(&cg->constList);
    return JS_TRUE;
}

JS_FRIEND_API(void)
js_FinishCodeGenerator(JSContext *cx, JSCodeGenerator *cg)
{
    TREE_CONTEXT_FINISH(&cg->treeContext);
    JS_ARENA_RELEASE(cg->codePool, cg->codeMark);
    JS_ARENA_RELEASE(cg->notePool, cg->noteMark);
    JS_ARENA_RELEASE(&cx->tempPool, cg->tempMark);
}

static ptrdiff_t
EmitCheck(JSContext *cx, JSCodeGenerator *cg, JSOp op, ptrdiff_t delta)
{
    jsbytecode *base, *limit, *next;
    ptrdiff_t offset, length;
    size_t incr, size;

    base = CG_BASE(cg);
    next = CG_NEXT(cg);
    limit = CG_LIMIT(cg);
    offset = PTRDIFF(next, base, jsbytecode);
    if (next + delta > limit) {
        length = offset + delta;
        length = (length <= BYTECODE_CHUNK)
                 ? BYTECODE_CHUNK
                 : JS_BIT(JS_CeilingLog2(length));
        incr = BYTECODE_SIZE(length);
        if (!base) {
            JS_ARENA_ALLOCATE_CAST(base, jsbytecode *, cg->codePool, incr);
        } else {
            size = BYTECODE_SIZE(PTRDIFF(limit, base, jsbytecode));
            incr -= size;
            JS_ARENA_GROW_CAST(base, jsbytecode *, cg->codePool, size, incr);
        }
        if (!base) {
            JS_ReportOutOfMemory(cx);
            return -1;
        }
        CG_BASE(cg) = base;
        CG_LIMIT(cg) = base + length;
        CG_NEXT(cg) = base + offset;
    }
    return offset;
}

static void
UpdateDepth(JSContext *cx, JSCodeGenerator *cg, ptrdiff_t target)
{
    jsbytecode *pc;
    const JSCodeSpec *cs;
    intN nuses;

    pc = CG_CODE(cg, target);
    cs = &js_CodeSpec[pc[0]];
    nuses = cs->nuses;
    if (nuses < 0)
        nuses = 2 + GET_ARGC(pc);       /* stack: fun, this, [argc arguments] */
    cg->stackDepth -= nuses;
    JS_ASSERT(cg->stackDepth >= 0);
    if (cg->stackDepth < 0) {
        char numBuf[12];
        JS_snprintf(numBuf, sizeof numBuf, "%d", target);
        JS_ReportErrorFlagsAndNumber(cx, JSREPORT_WARNING,
                                     js_GetErrorMessage, NULL,
                                     JSMSG_STACK_UNDERFLOW,
                                     cg->filename ? cg->filename : "stdin",
                                     numBuf);
    }
    cg->stackDepth += cs->ndefs;
    if ((uintN)cg->stackDepth > cg->maxStackDepth)
        cg->maxStackDepth = cg->stackDepth;
}

ptrdiff_t
js_Emit1(JSContext *cx, JSCodeGenerator *cg, JSOp op)
{
    ptrdiff_t offset = EmitCheck(cx, cg, op, 1);

    if (offset >= 0) {
        *CG_NEXT(cg)++ = (jsbytecode)op;
        UpdateDepth(cx, cg, offset);
    }
    return offset;
}

ptrdiff_t
js_Emit2(JSContext *cx, JSCodeGenerator *cg, JSOp op, jsbytecode op1)
{
    ptrdiff_t offset = EmitCheck(cx, cg, op, 2);

    if (offset >= 0) {
        jsbytecode *next = CG_NEXT(cg);
        next[0] = (jsbytecode)op;
        next[1] = op1;
        CG_NEXT(cg) = next + 2;
        UpdateDepth(cx, cg, offset);
    }
    return offset;
}

ptrdiff_t
js_Emit3(JSContext *cx, JSCodeGenerator *cg, JSOp op, jsbytecode op1,
         jsbytecode op2)
{
    ptrdiff_t offset = EmitCheck(cx, cg, op, 3);

    if (offset >= 0) {
        jsbytecode *next = CG_NEXT(cg);
        next[0] = (jsbytecode)op;
        next[1] = op1;
        next[2] = op2;
        CG_NEXT(cg) = next + 3;
        UpdateDepth(cx, cg, offset);
    }
    return offset;
}

ptrdiff_t
js_EmitN(JSContext *cx, JSCodeGenerator *cg, JSOp op, size_t extra)
{
    ptrdiff_t length = 1 + (ptrdiff_t)extra;
    ptrdiff_t offset = EmitCheck(cx, cg, op, length);

    if (offset >= 0) {
        jsbytecode *next = CG_NEXT(cg);
        *next = (jsbytecode)op;
        memset(next + 1, 0, BYTECODE_SIZE(extra));
        CG_NEXT(cg) = next + length;
        UpdateDepth(cx, cg, offset);
    }
    return offset;
}

/* XXX too many "... statement" L10N gaffes below -- fix via js.msg! */
const char js_with_statement_str[] = "with statement";
const char js_script_str[]         = "script";

static const char *statementName[] = {
    "block",                 /* BLOCK */
    "label statement",       /* LABEL */
    "if statement",          /* IF */
    "else statement",        /* ELSE */
    "switch statement",      /* SWITCH */
    js_with_statement_str,   /* WITH */
    "try statement",         /* TRY */
    "catch block",           /* CATCH */
    "finally statement",     /* FINALLY */
    "do loop",               /* DO_LOOP */
    "for loop",              /* FOR_LOOP */
    "for/in loop",           /* FOR_IN_LOOP */
    "while loop",            /* WHILE_LOOP */
};

static const char *
StatementName(JSCodeGenerator *cg)
{
    if (!cg->treeContext.topStmt)
        return js_script_str;
    return statementName[cg->treeContext.topStmt->type];
}

static void
ReportStatementTooLarge(JSContext *cx, JSCodeGenerator *cg)
{
    JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_NEED_DIET,
                         StatementName(cg));
}

/**
  Span-dependent instructions in JS bytecode consist of the jump (JOF_JUMP)
  and switch (JOF_LOOKUPSWITCH, JOF_TABLESWITCH) format opcodes, subdivided
  into unconditional (gotos and gosubs), and conditional jumps or branches
  (which pop a value, test it, and jump depending on its value).  Most jumps
  have just one immediate operand, a signed offset from the jump opcode's pc
  to the target bytecode.  The lookup and table switch opcodes may contain
  many jump offsets.

  Mozilla bug #80981 (http://bugzilla.mozilla.org/show_bug.cgi?id=80981) was
  fixed by adding extended "X" counterparts to the opcodes/formats (NB: X is
  suffixed to prefer JSOP_ORX thereby avoiding a JSOP_XOR name collision for
  the extended form of the JSOP_OR branch opcode).  The unextended or short
  formats have 16-bit signed immediate offset operands, the extended or long
  formats have 32-bit signed immediates.  The span-dependency problem consists
  of selecting as few long instructions as possible, or about as few -- since
  jumps can span other jumps, extending one jump may cause another to need to
  be extended.

  Most JS scripts are short, so need no extended jumps.  We optimize for this
  case by generating short jumps until we know a long jump is needed.  After
  that point, we keep generating short jumps, but each jump's 16-bit immediate
  offset operand is actually an unsigned index into cg->spanDeps, an array of
  JSSpanDep structs.  Each struct tells the top offset in the script of the
  opcode, the "before" offset of the jump (which will be the same as top for
  simplex jumps, but which will index further into the bytecode array for a
  non-initial jump offset in a lookup or table switch), the after "offset"
  adjusted during span-dependent instruction selection (initially the same
  value as the "before" offset), and the jump target (more below).

  Since we generate cg->spanDeps lazily, from within js_SetJumpOffset, we must
  ensure that all bytecode generated so far can be inspected to discover where
  the jump offset immediate operands lie within CG_CODE(cg).  But the bonus is
  that we generate span-dependency records sorted by their offsets, so we can
  binary-search when trying to find a JSSpanDep for a given bytecode offset,
  or the nearest JSSpanDep at or above a given pc.

  To avoid limiting scripts to 64K jumps, if the cg->spanDeps index overflows
  65534, we store SPANDEP_INDEX_HUGE in the jump's immediate operand.  This
  tells us that we need to binary-search for the cg->spanDeps entry by the
  jump opcode's bytecode offset (sd->before).

  Jump targets need to be maintained in a data structure that lets us look
  up an already-known target by its address (jumps may have a common target),
  and that also lets us update the addresses (script-relative, a.k.a. absolute
  offsets) of targets that come after a jump target (for when a jump below
  that target needs to be extended).  We use an AVL tree, implemented using
  recursion, but with some tricky optimizations to its height-balancing code
  (see http://www.cmcrossroads.com/bradapp/ftp/src/libs/C++/AvlTrees.html).

  A final wrinkle: backpatch chains are linked by jump-to-jump offsets with
  positive sign, even though they link "backward" (i.e., toward lower bytecode
  address).  We don't want to waste space and search time in the AVL tree for
  such temporary backpatch deltas, so we use a single-bit wildcard scheme to
  tag true JSJumpTarget pointers and encode untagged, signed (positive) deltas
  in JSSpanDep.target pointers, depending on whether the JSSpanDep has a known
  target, or is still awaiting backpatching.

  Note that backpatch chains would present a problem for BuildSpanDepTable,
  which inspects bytecode to build cg->spanDeps on demand, when the first
  short jump offset overflows.  To solve this temporary problem, we emit a
  proxy bytecode (JSOP_BACKPATCH; JSOP_BACKPATCH_POP for branch ops) whose
  nuses/ndefs counts help keep the stack balanced, but whose opcode format
  distinguishes its backpatch delta immediate operand from a normal jump
  offset.
 */
static int
BalanceJumpTargets(JSJumpTarget **jtp)
{
    JSJumpTarget *jt, *jt2, *root;
    int dir, otherDir, heightChanged;
    JSBool doubleRotate;

    jt = *jtp;
    JS_ASSERT(jt->balance != 0);

    if (jt->balance < -1) {
        dir = JT_RIGHT;
        doubleRotate = (jt->kids[JT_LEFT]->balance > 0);
    } else if (jt->balance > 1) {
        dir = JT_LEFT;
        doubleRotate = (jt->kids[JT_RIGHT]->balance < 0);
    } else {
        return 0;
    }

    otherDir = JT_OTHER_DIR(dir);
    if (doubleRotate) {
        jt2 = jt->kids[otherDir];
        *jtp = root = jt2->kids[dir];

        jt->kids[otherDir] = root->kids[dir];
        root->kids[dir] = jt;

        jt2->kids[dir] = root->kids[otherDir];
        root->kids[otherDir] = jt2;

        heightChanged = 1;
        root->kids[JT_LEFT]->balance = -JS_MAX(root->balance, 0);
        root->kids[JT_RIGHT]->balance = -JS_MIN(root->balance, 0);
        root->balance = 0;
    } else {
        *jtp = root = jt->kids[otherDir];
        jt->kids[otherDir] = root->kids[dir];
        root->kids[dir] = jt;

        heightChanged = (root->balance != 0);
        jt->balance = -((dir == JT_LEFT) ? --root->balance : ++root->balance);
    }

    return heightChanged;
}

typedef struct AddJumpTargetArgs {
    JSContext           *cx;
    JSCodeGenerator     *cg;
    ptrdiff_t           offset;
    JSJumpTarget        *node;
} AddJumpTargetArgs;

static int
AddJumpTarget(AddJumpTargetArgs *args, JSJumpTarget **jtp)
{
    JSJumpTarget *jt;
    int balanceDelta;

    jt = *jtp;
    if (!jt) {
        JSCodeGenerator *cg = args->cg;

        jt = cg->jtFreeList;
        if (jt) {
            cg->jtFreeList = jt->kids[JT_LEFT];
        } else {
            JS_ARENA_ALLOCATE_CAST(jt, JSJumpTarget *, &args->cx->tempPool,
                                   sizeof *jt);
            if (!jt) {
                JS_ReportOutOfMemory(args->cx);
                return 0;
            }
        }
        jt->offset = args->offset;
        jt->balance = 0;
        jt->kids[JT_LEFT] = jt->kids[JT_RIGHT] = NULL;
        cg->numJumpTargets++;
        args->node = jt;
        *jtp = jt;
        return 1;
    }

    if (jt->offset == args->offset) {
        args->node = jt;
        return 0;
    }

    if (args->offset < jt->offset)
        balanceDelta = -AddJumpTarget(args, &jt->kids[JT_LEFT]);
    else
        balanceDelta = AddJumpTarget(args, &jt->kids[JT_RIGHT]);
    if (!args->node)
        return 0;

    jt->balance += balanceDelta;
    return (balanceDelta && jt->balance)
           ? 1 - BalanceJumpTargets(jtp)
           : 0;
}

#ifdef DEBUG_brendan
static int AVLCheck(JSJumpTarget *jt)
{
    int lh, rh;

    if (!jt) return 0;
    JS_ASSERT(-1 <= jt->balance && jt->balance <= 1);
    lh = AVLCheck(jt->kids[JT_LEFT]);
    rh = AVLCheck(jt->kids[JT_RIGHT]);
    JS_ASSERT(jt->balance == rh - lh);
    return 1 + JS_MAX(lh, rh);
}
#endif

static JSBool
SetSpanDepTarget(JSContext *cx, JSCodeGenerator *cg, JSSpanDep *sd,
                 ptrdiff_t off)
{
    AddJumpTargetArgs args;

    if (off < JUMPX_OFFSET_MIN || JUMPX_OFFSET_MAX < off) {
        ReportStatementTooLarge(cx, cg);
        return JS_FALSE;
    }

    args.cx = cx;
    args.cg = cg;
    args.offset = sd->top + off;
    args.node = NULL;
    AddJumpTarget(&args, &cg->jumpTargets);
    if (!args.node)
        return JS_FALSE;

#ifdef DEBUG_brendan
    AVLCheck(cg->jumpTargets);
#endif

    SD_SET_TARGET(sd, args.node);
    return JS_TRUE;
}

#define SPANDEPS_MIN            256
#define SPANDEPS_SIZE(n)        ((n) * sizeof(JSSpanDep))
#define SPANDEPS_SIZE_MIN       SPANDEPS_SIZE(SPANDEPS_MIN)

static JSBool
AddSpanDep(JSContext *cx, JSCodeGenerator *cg, jsbytecode *pc, jsbytecode *pc2,
           ptrdiff_t off)
{
    uintN index;
    JSSpanDep *sdbase, *sd;
    size_t size;

    index = cg->numSpanDeps;
    if (index + 1 == 0) {
        ReportStatementTooLarge(cx, cg);
        return JS_FALSE;
    }

    if ((index & (index - 1)) == 0 &&
        (!(sdbase = cg->spanDeps) || index >= SPANDEPS_MIN)) {
        if (!sdbase) {
            size = SPANDEPS_SIZE_MIN;
            JS_ARENA_ALLOCATE_CAST(sdbase, JSSpanDep *, &cx->tempPool, size);
        } else {
            size = SPANDEPS_SIZE(index);
            JS_ARENA_GROW_CAST(sdbase, JSSpanDep *, &cx->tempPool, size, size);
        }
        if (!sdbase)
            return JS_FALSE;
        cg->spanDeps = sdbase;
    }

    cg->numSpanDeps = index + 1;
    sd = cg->spanDeps + index;
    sd->top = PTRDIFF(pc, CG_BASE(cg), jsbytecode);
    sd->offset = sd->before = PTRDIFF(pc2, CG_BASE(cg), jsbytecode);

    if (js_CodeSpec[*pc].format & JOF_BACKPATCH) {
        /* Jump offset will be backpatched if off is a non-zero "bpdelta". */
        if (off != 0) {
            JS_ASSERT(off >= 1 + JUMP_OFFSET_LEN);
            if (off > BPDELTA_MAX) {
                ReportStatementTooLarge(cx, cg);
                return JS_FALSE;
            }
        }
        SD_SET_BPDELTA(sd, off);
    } else if (off == 0) {
        /* Jump offset will be patched directly, without backpatch chaining. */
        SD_SET_TARGET(sd, NULL);
    } else {
        /* The jump offset in off is non-zero, therefore it's already known. */
        if (!SetSpanDepTarget(cx, cg, sd, off))
            return JS_FALSE;
    }

    if (index > SPANDEP_INDEX_MAX)
        index = SPANDEP_INDEX_HUGE;
    SET_SPANDEP_INDEX(pc2, index);
    return JS_TRUE;
}

static JSBool
BuildSpanDepTable(JSContext *cx, JSCodeGenerator *cg)
{
    jsbytecode *pc, *end;
    JSOp op;
    const JSCodeSpec *cs;
    ptrdiff_t len, off;

    pc = CG_BASE(cg) + cg->spanDepTodo;
    end = CG_NEXT(cg);
    while (pc < end) {
        op = (JSOp)*pc;
        cs = &js_CodeSpec[op];
        len = (ptrdiff_t)cs->length;

        switch (cs->format & JOF_TYPEMASK) {
          case JOF_JUMP:
            off = GET_JUMP_OFFSET(pc);
            if (!AddSpanDep(cx, cg, pc, pc, off))
                return JS_FALSE;
            break;

#if JS_HAS_SWITCH_STATEMENT
          case JOF_TABLESWITCH:
          {
            jsbytecode *pc2;
            jsint i, low, high;

            pc2 = pc;
            off = GET_JUMP_OFFSET(pc2);
            if (!AddSpanDep(cx, cg, pc, pc2, off))
                return JS_FALSE;
            pc2 += JUMP_OFFSET_LEN;
            low = GET_JUMP_OFFSET(pc2);
            pc2 += JUMP_OFFSET_LEN;
            high = GET_JUMP_OFFSET(pc2);
            pc2 += JUMP_OFFSET_LEN;
            for (i = low; i <= high; i++) {
                off = GET_JUMP_OFFSET(pc2);
                if (!AddSpanDep(cx, cg, pc, pc2, off))
                    return JS_FALSE;
                pc2 += JUMP_OFFSET_LEN;
            }
            len = 1 + pc2 - pc;
            break;
          }

          case JOF_LOOKUPSWITCH:
          {
            jsbytecode *pc2;
            jsint npairs;

            pc2 = pc;
            off = GET_JUMP_OFFSET(pc2);
            if (!AddSpanDep(cx, cg, pc, pc2, off))
                return JS_FALSE;
            pc2 += JUMP_OFFSET_LEN;
            npairs = (jsint) GET_ATOM_INDEX(pc2);
            pc2 += ATOM_INDEX_LEN;
            while (npairs) {
                pc2 += ATOM_INDEX_LEN;
                off = GET_JUMP_OFFSET(pc2);
                if (!AddSpanDep(cx, cg, pc, pc2, off))
                    return JS_FALSE;
                pc2 += JUMP_OFFSET_LEN;
                npairs--;
            }
            len = 1 + pc2 - pc;
            break;
          }
#endif /* JS_HAS_SWITCH_STATEMENT */
        }

        JS_ASSERT(len > 0);
        pc += len;
    }

    return JS_TRUE;
}

static JSSpanDep *
GetSpanDep(JSCodeGenerator *cg, jsbytecode *pc)
{
    uintN index;
    ptrdiff_t offset;
    int lo, hi, mid;
    JSSpanDep *sd;

    index = GET_SPANDEP_INDEX(pc);
    if (index != SPANDEP_INDEX_HUGE)
        return cg->spanDeps + index;

    offset = PTRDIFF(pc, CG_BASE(cg), jsbytecode);
    lo = 0;
    hi = cg->numSpanDeps - 1;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        sd = cg->spanDeps + mid;
        if (sd->before == offset)
            return sd;
        if (sd->before < offset)
            lo = mid + 1;
        else
            hi = mid - 1;
    }

    JS_ASSERT(0);
    return NULL;
}

static JSBool
SetBackPatchDelta(JSContext *cx, JSCodeGenerator *cg, jsbytecode *pc,
                  ptrdiff_t delta)
{
    JSSpanDep *sd;

    JS_ASSERT(delta >= 1 + JUMP_OFFSET_LEN);
    if (!cg->spanDeps && delta < JUMP_OFFSET_MAX) {
        SET_JUMP_OFFSET(pc, delta);
        return JS_TRUE;
    }

    if (delta > BPDELTA_MAX) {
        ReportStatementTooLarge(cx, cg);
        return JS_FALSE;
    }

    if (!cg->spanDeps && !BuildSpanDepTable(cx, cg))
        return JS_FALSE;

    sd = GetSpanDep(cg, pc);
    JS_ASSERT(SD_GET_BPDELTA(sd) == 0);
    SD_SET_BPDELTA(sd, delta);
    return JS_TRUE;
}

static void
UpdateJumpTargets(JSJumpTarget *jt, ptrdiff_t pivot, ptrdiff_t delta)
{
    if (jt->offset > pivot) {
        jt->offset += delta;
        if (jt->kids[JT_LEFT])
            UpdateJumpTargets(jt->kids[JT_LEFT], pivot, delta);
    }
    if (jt->kids[JT_RIGHT])
        UpdateJumpTargets(jt->kids[JT_RIGHT], pivot, delta);
}

static JSSpanDep *
FindNearestSpanDep(JSCodeGenerator *cg, ptrdiff_t offset, int lo,
                   JSSpanDep *guard)
{
    int num, hi, mid;
    JSSpanDep *sdbase, *sd;

    num = cg->numSpanDeps;
    JS_ASSERT(num > 0);
    hi = num - 1;
    sdbase = cg->spanDeps;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        sd = sdbase + mid;
        if (sd->before == offset)
            return sd;
        if (sd->before < offset)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    if (lo == num)
        return guard;
    sd = sdbase + lo;
    JS_ASSERT(sd->before >= offset && (lo == 0 || sd[-1].before < offset));
    return sd;
}

static void
FreeJumpTargets(JSCodeGenerator *cg, JSJumpTarget *jt)
{
    if (jt->kids[JT_LEFT])
        FreeJumpTargets(cg, jt->kids[JT_LEFT]);
    if (jt->kids[JT_RIGHT])
        FreeJumpTargets(cg, jt->kids[JT_RIGHT]);
    jt->kids[JT_LEFT] = cg->jtFreeList;
    cg->jtFreeList = jt;
}

static JSBool
OptimizeSpanDeps(JSContext *cx, JSCodeGenerator *cg)
{
    jsbytecode *pc, *oldpc, *base, *limit, *next;
    JSSpanDep *sd, *sd2, *sdbase, *sdlimit, *sdtop, guard;
    ptrdiff_t offset, growth, delta, top, pivot, span, length, target;
    JSBool done;
    JSOp op;
    uint32 type;
    size_t size, incr;
    jssrcnote *sn, *snlimit;
    JSSrcNoteSpec *spec;
    uintN i, n, noteIndex;
    JSTryNote *tn, *tnlimit;
#ifdef DEBUG_brendan
    int passes = 0;
#endif

    base = CG_BASE(cg);
    sdbase = cg->spanDeps;
    sdlimit = sdbase + cg->numSpanDeps;
    offset = CG_OFFSET(cg);
    growth = 0;

    do {
        done = JS_TRUE;
        delta = 0;
        top = pivot = -1;
        sdtop = NULL;
        pc = NULL;
        op = JSOP_NOP;
        type = 0;
#ifdef DEBUG_brendan
        passes++;
#endif

        for (sd = sdbase; sd < sdlimit; sd++) {
            JS_ASSERT(JT_HAS_TAG(sd->target));
            sd->offset += delta;

            if (sd->top != top) {
                sdtop = sd;
                top = sd->top;
                JS_ASSERT(top == sd->before);
                pivot = sd->offset;
                pc = base + top;
                op = (JSOp) *pc;
                type = (js_CodeSpec[op].format & JOF_TYPEMASK);
                if (JOF_TYPE_IS_EXTENDED_JUMP(type)) {
                    /*
                     * We already extended all the jump offset operands for
                     * the opcode at sd->top.  Jumps and branches have only
                     * one jump offset operand, but switches have many, all
                     * of which are adjacent in cg->spanDeps.
                     */
                    continue;
                }

                JS_ASSERT(type == JOF_JUMP ||
                          type == JOF_TABLESWITCH ||
                          type == JOF_LOOKUPSWITCH);
            }

            if (!JOF_TYPE_IS_EXTENDED_JUMP(type)) {
                span = SD_SPAN(sd, pivot);
                if (span < JUMP_OFFSET_MIN || JUMP_OFFSET_MAX < span) {
                    ptrdiff_t deltaFromTop = 0;

                    done = JS_FALSE;

                    switch (op) {
                      case JSOP_GOTO:         op = JSOP_GOTOX; break;
                      case JSOP_IFEQ:         op = JSOP_IFEQX; break;
                      case JSOP_IFNE:         op = JSOP_IFNEX; break;
                      case JSOP_OR:           op = JSOP_ORX; break;
                      case JSOP_AND:          op = JSOP_ANDX; break;
                      case JSOP_GOSUB:        op = JSOP_GOSUBX; break;
                      case JSOP_CASE:         op = JSOP_CASEX; break;
                      case JSOP_DEFAULT:      op = JSOP_DEFAULTX; break;
                      case JSOP_TABLESWITCH:  op = JSOP_TABLESWITCHX; break;
                      case JSOP_LOOKUPSWITCH: op = JSOP_LOOKUPSWITCHX; break;
                      default:                JS_ASSERT(0);
                    }
                    *pc = (jsbytecode) op;

                    for (sd2 = sdtop; sd2 < sdlimit && sd2->top == top; sd2++) {
                        if (sd2 <= sd) {
                            /*
                             * sd2->offset already includes delta as it stood
                             * before we entered this loop, but it must also
                             * include the delta relative to top due to all the
                             * extended jump offset immediates for the opcode
                             * starting at top, which we extend in this loop.
                             *
                             * If there is only one extended jump offset, then
                             * sd2->offset won't change and this for loop will
                             * iterate once only.
                             */
                            sd2->offset += deltaFromTop;
                            deltaFromTop += JUMPX_OFFSET_LEN - JUMP_OFFSET_LEN;
                        } else {
                            /*
                             * sd2 comes after sd, and won't be revisited by
                             * the outer for loop, so we have to increase its
                             * offset by delta, not merely by deltaFromTop.
                             */
                            sd2->offset += delta;
                        }

                        delta += JUMPX_OFFSET_LEN - JUMP_OFFSET_LEN;
                        UpdateJumpTargets(cg->jumpTargets, sd2->offset,
                                          JUMPX_OFFSET_LEN - JUMP_OFFSET_LEN);
                    }
                    sd = sd2 - 1;
                }
            }
        }

        growth += delta;
    } while (!done);

    if (growth) {
#ifdef DEBUG_brendan
        printf("%s:%u: %u/%u jumps extended in %d passes (%d=%d+%d)\n",
               cg->filename ? cg->filename : "stdin", cg->firstLine,
               growth / (JUMPX_OFFSET_LEN - JUMP_OFFSET_LEN), cg->numSpanDeps,
               passes, offset + growth, offset, growth);
#endif

        /*
         * Ensure that we have room for the extended jumps, but don't round up
         * to a power of two -- we're done generating code, so we cut to fit.
         */
        limit = CG_LIMIT(cg);
        length = offset + growth;
        next = base + length;
        if (next > limit) {
            JS_ASSERT(length > BYTECODE_CHUNK);
            size = BYTECODE_SIZE(PTRDIFF(limit, base, jsbytecode));
            incr = BYTECODE_SIZE(length) - size;
            JS_ARENA_GROW_CAST(base, jsbytecode *, cg->codePool, size, incr);
            if (!base) {
                JS_ReportOutOfMemory(cx);
                return JS_FALSE;
            }
            CG_BASE(cg) = base;
            CG_LIMIT(cg) = next = base + length;
        }
        CG_NEXT(cg) = next;

        /*
         * Set up a fake span dependency record to guard the end of the code
         * being generated.  This guard record is returned as a fencepost by
         * FindNearestSpanDep if there is no real spandep at or above a given
         * unextended code offset.
         */
        guard.top = -1;
        guard.offset = offset + growth;
        guard.before = offset;
        guard.target = NULL;
    }

    /*
     * Now work backwards through the span dependencies, copying chunks of
     * bytecode between each extended jump toward the end of the grown code
     * space, and restoring immediate offset operands for all jump bytecodes.
     * The first chunk of bytecodes, starting at base and ending at the first
     * extended jump offset (NB: this chunk includes the operation bytecode
     * just before that immediate jump offset), doesn't need to be copied.
     */
    JS_ASSERT(sd == sdlimit);
    top = -1;
    while (--sd >= sdbase) {
        if (sd->top != top) {
            top = sd->top;
            op = (JSOp) base[top];
            type = (js_CodeSpec[op].format & JOF_TYPEMASK);

            for (sd2 = sd - 1; sd2 >= sdbase && sd2->top == top; sd2--)
                continue;
            sd2++;
            pivot = sd2->offset;
            JS_ASSERT(top == sd2->before);
        }

        oldpc = base + sd->before;
        span = SD_SPAN(sd, pivot);

        /*
         * If this jump didn't need to be extended, restore its span immediate
         * offset operand now, overwriting the index of sd within cg->spanDeps
         * that was stored temporarily after *pc when BuildSpanDepTable ran.
         *
         * Note that span might fit in 16 bits even for an extended jump op,
         * if the op has multiple span operands, not all of which overflowed
         * (e.g. JSOP_LOOKUPSWITCH or JSOP_TABLESWITCH where some cases are in
         * range for a short jump, but others are not).
         */
        if (!JOF_TYPE_IS_EXTENDED_JUMP(type)) {
            JS_ASSERT(JUMP_OFFSET_MIN <= span && span <= JUMP_OFFSET_MAX);
            SET_JUMP_OFFSET(oldpc, span);
            continue;
        }

        /*
         * Set up parameters needed to copy the next run of bytecode starting
         * at offset (which is a cursor into the unextended, original bytecode
         * vector), down to sd->before (a cursor of the same scale as offset,
         * it's the index of the original jump pc).  Reuse delta to count the
         * nominal number of bytes to copy.
         */
        pc = base + sd->offset;
        delta = offset - sd->before;
        JS_ASSERT(delta >= 1 + JUMP_OFFSET_LEN);

        /*
         * Don't bother copying the jump offset we're about to reset, but do
         * copy the bytecode at oldpc (which comes just before its immediate
         * jump offset operand), on the next iteration through the loop, by
         * including it in offset's new value.
         */
        offset = sd->before + 1;
        size = BYTECODE_SIZE(delta - (1 + JUMP_OFFSET_LEN));
        if (size) {
            memmove(pc + 1 + JUMPX_OFFSET_LEN,
                    oldpc + 1 + JUMP_OFFSET_LEN,
                    size);
        }

        SET_JUMPX_OFFSET(pc, span);
    }

    if (growth) {
        /*
         * Fix source note deltas.  Don't hardwire the delta fixup adjustment,
         * even though currently it must be JUMPX_OFFSET_LEN - JUMP_OFFSET_LEN
         * at each sd that moved.  The future may bring different offset sizes
         * for span-dependent instruction operands.  However, we fix only main
         * notes here, not prolog notes -- we know that prolog opcodes are not
         * span-dependent, and aren't likely ever to be.
         */
        offset = growth = 0;
        sd = sdbase;
        for (sn = cg->main.notes, snlimit = sn + cg->main.noteCount;
             sn < snlimit;
             sn = SN_NEXT(sn)) {
            /*
             * Recall that the offset of a given note includes its delta, and
             * tells the offset of the annotated bytecode fro