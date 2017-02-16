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
 * JS parser.
 *
 * This is a recursive-descent parser for the JavaScript language specified by
 * "The JavaScript 1.5 Language Specification".  It uses lexical and semantic
 * feedback to disambiguate non-LL(1) structures.  It generates trees of nodes
 * induced by the recursive parsing (not precise syntax trees, see jsparse.h).
 * After tree construction, it rewrites trees to fold constants and evaluate
 * compile-time expressions.  Finally, it calls js_EmitTree (see jsemit.h) to
 * generate bytecode.
 *
 * This parser attempts no error recovery.
 */
#include "jsstddef.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsapi.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsemit.h"
#include "jsfun.h"
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

#if JS_HAS_XML_SUPPORT
#include "jsxml.h"
#endif

/*
 * JS parsers, from lowest to highest precedence.
 *
 * Each parser takes a context, a token stream, and a tree context struct.
 * Each returns a parse node tree or null on error.
 */

typedef JSParseNode *
JSParser(JSContext *cx, JSTokenStream *ts, JSTreeContext *tc);

typedef JSParseNode *
JSMemberParser(JSContext *cx, JSTokenStream *ts, JSTreeContext *tc,
               JSBool allowCallSyntax);

static JSParser FunctionStmt;
#if JS_HAS_LEXICAL_CLOSURE
static JSParser FunctionExpr;
#endif
static JSParser Statements;
static JSParser Statement;
static JSParser Variables;
static JSParser Expr;
static JSParser AssignExpr;
static JSParser CondExpr;
static JSParser OrExpr;
static JSParser AndExpr;
static JSParser BitOrExpr;
static JSParser BitXorExpr;
static JSParser BitAndExpr;
static JSParser EqExpr;
static JSParser RelExpr;
static JSParser ShiftExpr;
static JSParser AddExpr;
static JSParser MulExpr;
static JSParser UnaryExpr;
static JSMemberParser MemberExpr;
static JSParser PrimaryExpr;

/*
 * Insist that the next token be of type tt, or report errno and return null.
 * NB: this macro uses cx and ts from its lexical environment.
 */
#define MUST_MATCH_TOKEN(tt, errno)                                           \
    JS_BEGIN_MACRO                                                            \
        if (js_GetToken(cx, ts) != tt) {                                      \
            js_ReportCompileErrorNumber(cx, ts, JSREPORT_TS | JSREPORT_ERROR, \
                            errno);                                           \
            return NULL;                                                      \
        }                                                                     \
    JS_END_MACRO

#define CHECK_RECURSION()                                                     \
    JS_BEGIN_MACRO                                                            \
        int stackDummy;                                                       \
        if (!JS_CHECK_STACK_SIZE(cx, stackDummy)) {                           \
            js_ReportCompileErrorNumber(cx, ts, JSREPORT_TS | JSREPORT_ERROR, \
                                        JSMSG_OVER_RECURSED);                 \
            return NULL;                                                      \
        }                                                                     \
    JS_END_MACRO

#ifdef METER_PARSENODES
static uint32 parsenodes = 0;
static uint32 maxparsenodes = 0;
static uint32 recyclednodes = 0;
#endif

static JSParseNode *
RecycleTree(JSParseNode *pn, JSTreeContext *tc)
{
    JSParseNode *next;

    if (!pn)
        return NULL;
    JS_ASSERT(pn != tc->nodeList);      /* catch back-to-back dup recycles */
    next = pn->pn_next;
    pn->pn_next = tc->nodeList;
    tc->nodeList = pn;
#ifdef METER_PARSENODES
    recyclednodes++;
#endif
    return next;
}

static JSParseNode *
NewOrRecycledNode(JSContext *cx, JSTreeContext *tc)
{
    JSParseNode *pn;

    pn = tc->nodeList;
    if (!pn) {
        JS_ARENA_ALLOCATE_TYPE(pn, JSParseNode, &cx->tempPool);
        if (!pn)
            JS_ReportOutOfMemory(cx);
    } else {
        tc->nodeList = pn->pn_next;

        /* Recycle immediate descendents only, to save work and working set. */
        switch (pn->pn_arity) {
          case PN_FUNC:
            RecycleTree(pn->pn_body, tc);
            break;
          case PN_LIST:
            if (pn->pn_head) {
                /* XXX check for dup recycles in the list */
                *pn->pn_tail = tc->nodeList;
                tc->nodeList = pn->pn_head;
#ifdef METER_PARSENODES
                recyclednodes += pn->pn_count;
#endif
            }
            break;
          case PN_TERNARY:
            RecycleTree(pn->pn_kid1, tc);
            RecycleTree(pn->pn_kid2, tc);
            RecycleTree(pn->pn_kid3, tc);
            break;
          case PN_BINARY:
            RecycleTree(pn->pn_left, tc);
            RecycleTree(pn->pn_right, tc);
            break;
          case PN_UNARY:
            RecycleTree(pn->pn_kid, tc);
            break;
          case PN_NAME:
            RecycleTree(pn->pn_expr, tc);
            break;
          case PN_NULLARY:
            break;
        }
    }
#ifdef METER_PARSENODES
    if (pn) {
        parsenodes++;
        if (parsenodes - recyclednodes > maxparsenodes)
            maxparsenodes = parsenodes - recyclednodes;
    }
#endif
    return pn;
}

/*
 * Allocate a JSParseNode from cx's temporary arena.
 */
static JSParseNode *
NewParseNode(JSContext *cx, JSTokenStream *ts, JSParseNodeArity arity,
             JSTreeContext *tc)
{
    JSParseNode *pn;
    JSToken *tp;

    pn = NewOrRecycledNode(cx, tc);
    if (!pn)
        return NULL;
    tp = &CURRENT_TOKEN(ts);
    pn->pn_type = tp->type;
    pn->pn_pos = tp->pos;
    pn->pn_op = JSOP_NOP;
    pn->pn_arity = arity;
    pn->pn_next = NULL;
#if JS_HAS_XML_SUPPORT
    pn->pn_ts = ts;
#endif
    return pn;
}

static JSParseNode *
NewBinary(JSContext *cx, JSTokenType tt,
          JSOp op, JSParseNode *left, JSParseNode *right,
          JSTreeContext *tc)
{
    JSParseNode *pn, *pn1, *pn2;

    if (!left || !right)
        return NULL;

    /*
     * Flatten a left-associative (left-heavy) tree of a given operator into
     * a list, to reduce js_FoldConstants and js_EmitTree recursion.
     */
    if (left->pn_type == tt &&
        left->pn_op == op &&
        (js_CodeSpec[op].format & JOF_LEFTASSOC)) {
        if (left->pn_arity != PN_LIST) {
            pn1 = left->pn_left, pn2 = left->pn_right;
            left->pn_arity = PN_LIST;
            PN_INIT_LIST_1(left, pn1);
            PN_APPEND(left, pn2);
            if (tt == TOK_PLUS) {
                if (pn1->pn_type == TOK_STRING)
                    left->pn_extra |= PNX_STRCAT;
                else if (pn1->pn_type != TOK_NUMBER)
                    left->pn_extra |= PNX_CANTFOLD;
                if (pn2->pn_type == TOK_STRING)
                    left->pn_extra |= PNX_STRCAT;
                else if (pn2->pn_type != TOK_NUMBER)
                    left->pn_extra |= PNX_CANTFOLD;
            }
        }
        PN_APPEND(left, right);
        left->pn_pos.end = right->pn_pos.end;
        if (tt == TOK_PLUS) {
            if (right->pn_type == TOK_STRING)
                left->pn_extra |= PNX_STRCAT;
            else if (right->pn_type != TOK_NUMBER)
                left->pn_extra |= PNX_CANTFOLD;
        }
        return left;
    }

    /*
     * Fold constant addition immediately, to conserve node space and, what's
     * more, so js_FoldConstants never sees mixed addition and concatenation
     * operations with more than one leading non-string operand in a PN_LIST
     * generated for expressions such as 1 + 2 + "pt" (which should evaluate
     * to "3pt", not "12pt").
     */
    if (tt == TOK_PLUS &&
        left->pn_type == TOK_NUMBER &&
        right->pn_type == TOK_NUMBER) {
        left->pn_dval += right->pn_dval;
        left->pn_pos.end = right->pn_pos.end;
        RecycleTree(right, tc);
        return left;
    }

    pn = NewOrRecycledNode(cx, tc);
    if (!pn)
        return NULL;
    pn->pn_type = tt;
    pn->pn_pos.begin = left->pn_pos.begin;
    pn->pn_pos.end = right->pn_pos.end;
    pn->pn_op = op;
    pn->pn_arity = PN_BINARY;
    pn->pn_left = left;
    pn->pn_right = right;
    pn->pn_next = NULL;
#if JS_HAS_XML_SUPPORT
    pn->pn_ts = NULL;
#endif
    return pn;
}

#if JS_HAS_GETTER_SETTER
static JSTokenType
CheckGetterOrSetter(JSContext *cx, JSTokenStream *ts, JSTokenType tt)
{
    JSAtom *atom;
    JSRuntime *rt;
    JSOp op;
    const char *name;

    JS_ASSERT(CURRENT_TOKEN(ts).type == TOK_NAME);
    atom = CURRENT_TOKEN(ts).t_atom;
    rt = cx->runtime;
    if (atom == rt->atomState.getterAtom)
        op = JSOP_GETTER;
    else if (atom == rt->atomState.setterAtom)
        op = JSOP_SETTER;
    else
        return TOK_NAME;
    if (js_PeekTokenSameLine(cx, ts) != tt)
        return TOK_NAME;
    (void) js_GetToken(cx, ts);
    if (CURRENT_TOKEN(ts).t_op != JSOP_NOP) {
        js_ReportCompileErrorNumber(cx, ts, JSREPORT_TS | JSREPORT_ERROR,
                                    JSMSG_BAD_GETTER_OR_SETTER,
                                    (op == JSOP_GETTER)
                                    ? js_getter_str
                                    : js_setter_str);
        return TOK_ERROR;
    }
    CURRENT_TOKEN(ts).t_op = op;
    name = js_AtomToPrintableString(cx, atom);
    if (!name ||
        !js_ReportCompileErrorNumber(cx, ts,
                                     JSREPORT_TS |
                                     JSREPORT_WARNING |
                                     JSREPORT_STRICT,
                                     JSMSG_DEPRECATED_USAGE,
                                     name)) {
        return TOK_ERROR;
    }
    return tt;
}
#endif

/*
 * Parse a top-level JS script.
 */
JS_FRIEND_API(JSParseNode *)
js_ParseTokenStream(JSContext *cx, JSObject *chain, JSTokenStream *ts)
{
    JSStackFrame *fp, frame;
    JSTreeContext tc;
    JSParseNode *pn;

    /*
     * Push a compiler frame if we have no frames, or if the top frame is a
     * lightweight function activation, or if its scope chain doesn't match
     * the one passed to us.
     */
    fp = cx->fp;
    if (!fp || !fp->varobj || fp->scopeChain != chain) {
        memset(&frame, 0, sizeof frame);
        frame.varobj = frame.scopeChain = chain;
        if (cx->options & JSOPTION_VAROBJFIX) {
            while ((chain = JS_GetParent(cx, chain)) != NULL)
                frame.varobj = chain;
        }
        frame.down = fp;
        if (fp) {
            frame.flags = fp->flags & (JSFRAME_SPECIAL | JSFRAME_COMPILE_N_GO |
                                       JSFRAME_SCRIPT_OBJECT);
        }
        cx->fp = &frame;
    }

    /*
     * Protect atoms from being collected by a GC activation, which might
     * - nest on this thread due to out of memory (the so-called "last ditch"
     *   GC attempted within js_NewGCThing), or
     * - run for any reason on another thread if this thread is suspended on
     *   an object lock before it finishes generating bytecode into a script
     *   protected from the GC by a root or a stack frame reference.
     */
    JS_KEEP_ATOMS(cx->runtime);
    TREE_CONTEXT_INIT(&tc);
    pn = Statements(cx, ts, &tc);
    if (pn) {
        if (!js_MatchToken(cx, ts, TOK_EOF)) {
            js_ReportCompileErrorNumber(cx, ts, JSREPORT_TS | JSREPORT_ERROR,
                                        JSMSG_SYNTAX_ERROR);
            pn = NULL;
        } else {
            pn->pn_type = TOK_LC;
            if (!js_FoldConstants(cx, pn, &tc))
                pn = NULL;
        }
    }

    TREE_CONTEXT_FINISH(&tc);
    JS_UNKEEP_ATOMS(cx->runtime);
    cx->fp = fp;
    return pn;
}

/*
 * Compile a top-level script.
 */
JS_FRIEND_API(JSBool)
js_CompileTokenStream(JSContext *cx, JSObject *chain, JSTokenStream *ts,
                      JSCodeGenerator *cg)
{
    JSStackFrame *fp, frame;
    uint32 flags;
    JSParseNode *pn;
    JSBool ok;
#ifdef METER_PARSENODES
    void *sbrk(ptrdiff_t), *before = sbrk(0);
#endif

    /*
     * Push a compiler frame if we have no frames, or if the top frame is a
     * lightweight function activation, or if its scope chain doesn't match
     * the one passed to us.
     */
    fp = cx->fp;
    if (!fp || !fp->varobj || fp->scopeChain != chain) {
        memset(&frame, 0, sizeof frame);
        frame.varobj = frame.scopeChain = chain;
        if (cx->options & JSOPTION_VAROBJFIX) {
            while ((chain = JS_GetParent(cx, chain)) != NULL)
                frame.varobj = chain;
        }
        frame.down = fp;
        if (fp) {
            frame.flags = fp->flags & (JSFRAME_SPECIAL | JSFRAME_COMPILE_N_GO |
                                       JSFRAME_SCRIPT_OBJECT);
        }
        cx->fp = &frame;
    }
    flags = cx->fp->flags;
    cx->fp->flags = flags |
                    (JS_HAS_COMPILE_N_GO_OPTION(cx)
                     ? JSFRAME_COMPILING | JSFRAME_COMPILE_N_GO
                     : JSFRAME_COMPILING);

    /* Prevent GC activation while compiling. */
    JS_KEEP_ATOMS(cx->runtime);

    pn = Statements(cx, ts, &cg->treeContext);
    if (!pn) {
        ok = JS_FALSE;
    } else if (!js_MatchToken(cx, ts, TOK_EOF)) {
        js_ReportCompileErrorNumber(cx, ts, JSREPORT_TS | JSREPORT_ERROR,
                                    JSMSG_SYNTAX_ERROR);
        ok = JS_FALSE;
    } else {
#ifdef METER_PARSENODES
        printf("Parser growth: %d (%u nodes, %u max, %u unrecycled)\n",
               (char *)sbrk(0) - (char *)before,
               parsenodes,
               maxparsenodes,
               parsenodes - recyclednodes);
        before = sbrk(0);
#endif

        /*
         * No need to emit code here -- Statements already has, for each
         * statement in turn.  Search for TCF_COMPILING in Statements, below.
         * That flag is set for every tc == &cg->treeContext, and it implies
         * that the tc can be downcast to a cg and used to emit code during
         * parsing, rather than at the end of the parse phase.
         */
        JS_ASSERT(cg->treeContext.flags & TCF_COMPILING);
        ok = JS_TRUE;
    }

#ifdef METER_PARSENODES
    printf("Code-gen growth: %d (%u bytecodes, %u srcnotes)\n",
           (char *)sbrk(0) - (char *)before, CG_OFFSET(cg), cg->noteCount);
#endif
#ifdef JS_ARENAMETER
    JS_DumpArenaStats(stdout);
#endif
    JS_UNKEEP_ATOMS(cx->runtime);
    cx->fp->flags = flags;
    cx->fp = fp;
    return ok;
}

/*
 * Insist on a final return before control flows out of pn, but don't be too
 * smart about loops (do {...; return e2;} while(0) at the end of a function
 * that contains an early return e1 will get a strict-option-only warning).
 */
#define ENDS_IN_OTHER   0
#define ENDS_IN_RETURN  1
#define ENDS_IN_BREAK   2

static int
HasFinalReturn(JSParseNode *pn)
{
    uintN rv, rv2, hasDefault;
    JSParseNode *pn2, *pn3;

    switch (pn->pn_type) {
      case TOK_LC:
        if (!pn->pn_head)
            return ENDS_IN_OTHER;
        return HasFinalReturn(PN_LAST(pn));

      case TOK_IF:
        rv = HasFinalReturn(pn->pn_kid2);
        if (pn->pn_kid3)
            rv &= HasFinalReturn(pn->pn_kid3);
        return rv;

#if JS_HAS_SWITCH_STATEMENT
      case TOK_SWITCH:
        rv = ENDS_IN_RETURN;
        hasDefault = ENDS_IN_OTHER;
        for (pn2 = pn->pn_kid2->pn_head; rv && pn2; pn2 = pn2->pn_next) {
            if (pn2->pn_type == TOK_DEFAULT)
                hasDefault = ENDS_IN_RETURN;
            pn3 = pn2->pn_right;
            JS_ASSERT(pn3->pn_type == TOK_LC);
            if (pn3->pn_head) {
                rv2 = HasFinalReturn(PN_LAST(pn3));
                if (rv2 == ENDS_IN_OTHER && pn2->pn_next)
                    /* Falling through to next case or default. */;
                else
                    rv &= rv2;
            }
        }
        /* If a final switch has no default case, we judge it harshly. */
        rv &= hasDefault;
        return rv;
#endif /* JS_HAS_SWITCH_STATEMENT */

      case TOK_BREAK:
        return ENDS_IN_BREAK;

      case TOK_WITH:
        return HasFinalReturn(pn->pn_right);

      case TOK_RETURN:
        return ENDS_IN_RETURN;

      case TOK_COLON:
        return HasFinalReturn(pn->pn_expr);

#if JS_HAS_EXCEPTIONS
      case TOK_THROW:
        return ENDS_IN_RETURN;

      case TOK_TRY:
        /* If we have a finally block that returns, we are done. */
        if (pn->pn_kid3) {
            rv = HasFinalReturn(pn->pn_kid3);
            if (rv == ENDS_IN_RETURN)
                return rv;
        }

        /* Else check the try block and any and all catch statements. */
        rv = HasFinalReturn(pn->pn_kid1);
        if (pn->pn_kid2)
            rv &= HasFinalReturn(pn->pn_kid2);
        return rv;

      case TOK_CATCH:
        /* Check this block's code and iterate over further catch blocks. */
        rv = HasFinalReturn(pn->pn_kid3);
        for (pn2 = pn->pn_kid2; pn2; pn2 = pn2->pn_kid2)
            rv &= HasFinalReturn(pn2->pn_kid3);
        return rv;
#endif

      default:
        return ENDS_IN_OTHER;
    }
}

static JSBool
ReportNoReturnValue(JSContext *cx, JSTokenStream *ts)
{
    JSFunction *fun;
    JSBool ok;

    fun = cx->fp->fun;
    if (fun->atom) {
        char *name = js_GetStringBytes(ATOM_TO_STRING(fun->atom));
        ok = js_ReportCompileErrorNumber(cx, ts,
                                         JSREPORT_TS |
                                         JSREPORT_WARNING |
                                         JSREPORT_STRICT,
                                         JSMSG_NO_RETURN_VALUE, name);
    } else {
        ok = js_ReportCompileErrorNumber(cx, ts,
                                         JSREPORT_TS |
                                         JSREPORT_WARNING |
                                         JSREPORT_STRICT,
                                         JSMSG_ANON_NO_RETURN_VALUE);
    }
    return ok;
}

static JSBool
CheckFinalReturn(JSContext *cx, JSTokenStream *ts, JSParseNode *pn)
{
    return HasFinalReturn(pn) == ENDS_IN_RETURN || ReportNoReturnValue(cx, ts);
}

static JSParseNode *
FunctionBody(JSContext *cx, JSTokenStream *ts, JSFunction *fun,
             JSTreeContext *tc)
{
    JSStackFrame *fp, frame;
    JSObject *funobj;
    uintN oldflags;
    JSParseNode *pn;

    fp = cx->fp;
    funobj = fun->object;
    if (!fp || fp->fun != fun || fp->varobj != funobj ||
        fp->scopeChain != funobj) {
        memset(&frame, 0, sizeof frame);
        frame.fun = fun;
        frame.varobj = frame.scopeChain = funobj;
        frame.down = fp;
        if (fp)
            frame.flags = fp->flags & JSFRAME_COMPILE_N_GO;
        cx->fp = &frame;
    }

    oldflags = tc->flags;
    tc->flags &= ~(TCF_RETURN_EXPR | TCF_RETURN_VOID);
    tc->flags |= TCF_IN_FUNCTION;
    pn = Statements(cx, ts, tc);

    /* Check for falling off the end of a function that returns a value. */
    if (pn && JS_HAS_STRICT_OPTION(cx) && (tc->flags & TCF_RETURN_EXPR)) {
        if (!CheckFinalReturn(cx, ts, pn))
            pn = NULL;
    }

    cx->fp = fp;
    tc->flags = oldflags | (tc->flags & (TCF_FUN_FLAGS | TCF_HAS_DEFXMLNS));
    return pn;
}

/*
 * Compile a JS function body, which might appear as the value of an event
 * handler attribute in an HTML <INPUT> tag.
 */
JSBool
js_CompileFunctionBody(JSContext *cx, JSTokenStream *ts, JSFunction *fun)
{
    JSArenaPool codePool, notePool;
    JSCodeGenerator funcg;
    JSStackFrame *fp, frame;
    JSObject *funobj;
    JSParseNode *pn;
    JSBool ok;

    JS_InitArenaPool(&codePool, "code", 1024, sizeof(jsbytecode));
    JS_InitArenaPool(&notePool, "note", 1024, sizeof(jssrcnote));
    if (!js_InitCodeGenerator(cx, &funcg, &codePool, &notePool,
                              ts->filename, ts->lineno,
                              ts->principals)) {
        return JS_FALSE;
    }

    /* Prevent GC activation while compiling. */
    JS_KEEP_ATOMS(cx->runtime);

    /* Push a JSStackFrame for use by FunctionBody. */
    fp = cx->fp;
    funobj = fun->object;
    JS_ASSERT(!fp || (fp->fun != fun && fp->varobj != funobj &&
                      fp->scopeChain != funobj));
    memset(&frame, 0, sizeof frame);
    frame.fun = fun;
    frame.varobj = frame.scopeChain = funobj;
    frame.down = fp;
    frame.flags = JS_HAS_COMPILE_N_GO_OPTION(cx)
                  ? JSFRAME_COMPILING | JSFRAME_COMPILE_N_GO
                  : JSFRAME_COMPILING;
    cx->fp = &frame;

    /* Ensure that the body looks like a block statement to js_EmitTree. */
    CURRENT_TOKEN(ts).type = TOK_LC;
    pn = FunctionBody(cx, ts, fun, &funcg.treeContext);
    if (!pn) {
        ok = JS_FALSE;
    } else {
        /*
         * No need to emit code here -- Statements (via FunctionBody) already
         * has.  See similar comment in js_CompileTokenStream, and bug 108257.
         */
        fun->u.script = js_NewScriptFromCG(cx, &funcg, fun);
        if (!fun->u.script) {
            ok = JS_FALSE;
        } else {
            fun->interpreted = JS_TRUE;
            if (funcg.treeContext.flags & TCF_FUN_HEAVYWEIGHT)
                fun->flags |= JSFUN_HEAVYWEIGHT;
            ok = JS_TRUE;
        }
    }

    /* Restore saved state and release code generation arenas. */
    cx->fp = fp;
    JS_UNKEEP_ATOMS(cx->runtime);
    js_FinishCodeGenerator(cx, &funcg);
    JS_FinishArenaPool(&codePool);
    JS_FinishArenaPool(&notePool);
    return ok;
}

static JSParseNode *
FunctionDef(JSContext *cx, JSTokenStream *ts, JSTreeContext *tc,
            JSBool lambda)
{
    JSOp op, prevop;
    JSParseNode *pn, *body, *result;
    JSAtom *funAtom, *objAtom, *argAtom;
    JSStackFrame *fp;
    JSObject *varobj, *pobj;
    JSAtomListElement *ale;
    JSProperty *prop;
    JSFunction *fun;
    uintN dupflag;
    JSBool ok;
    JSTreeContext funtc;

    /* Make a TOK_FUNCTION node. */
#if JS_HAS_GETTER_SETTER
    op = CURRENT_TOKEN(ts).t_op;
#endif
    pn = NewParseNode(cx, ts, PN_FUNC, tc);
    if (!pn)
        return NULL;

    /* Scan the optional function name into funAtom. */
    funAtom = js_MatchToken(cx, ts, TOK_NAME) ? CURRENT_TOKEN(ts).t_atom : NULL;
#if !JS_HAS_LEXICAL_CLOSURE
    if (!funAtom && !lambda) {
        js_ReportCompileErrorNumber(cx, ts, JSREPORT_TS | JSREPORT_ERROR,
                                    JSMSG_SYNTAX_ERROR);
        return NULL;
    }
#endif

    /* Find the nearest variable-declaring scope and use it as our parent. */
    fp = cx->fp;
    varobj = fp->varobj;

    /*
     * Record names for function statements in tc->decls so we know when to
     * avoid optimizing variable references that might name a function.
     */
    if (!lambda && funAtom) {
        ATOM_LIST_SEARCH(ale, &tc->decls, funAtom);
        if (ale) {
            prevop = ALE_JSOP(ale);
            if (JS_HAS_STRICT_OPTION(cx) || prevop == JSOP_DEFCONST) {
                const char *name = js_AtomToPrintableString(cx, funAtom);
                if (!name ||
                    !js_ReportCompileErrorNumber(cx, ts,
                                                 (prevop != JSOP_DEFCONST)
                                                 ? JSREPORT_TS |
                                                   JSREPORT_WARNING |
                                                   JSREPORT_STRICT
                                                 : JSREPORT_TS | JSREPORT_ERROR,
                                                 JSMSG_REDECLARED_VAR,
                                                 (prevop == JSOP_DEFFUN ||
                                                  prevop == JSOP_CLOSURE)
                                                 ? js_function_str
                                                 : (prevop == JSOP_DEFCONST)
                                                 ? js_const_str
                                                 : js_var_str,
                                                 name)) {
                    return NULL;
                }
            }
            if (tc->topStmt && prevop == JSOP_DEFVAR)
                tc->flags |= TCF_FUN_CLOSURE_VS_VAR;
        } else {
            ale = js_IndexAtom(cx, funAtom, &tc->decls);
            if (!ale)
                return NULL;
        }
        ALE_SET_JSOP(ale, tc->topStmt ? JSOP_CLOSURE : JSOP_DEFFUN);

#if JS_HAS_LEXICAL_CLOSURE
        /*
         * A function nested at top level inside another's body needs only a
         * local variable to bind its name to its value, and not an activation
         * object property (it might also need the activation property, if the
         * outer function contains with statements, e.g., but the stack slot
         * wins when jsemit.c's BindNameToSlot can optimize a JSOP_NAME into a
         * JSOP_GETVAR bytecode).
         */
        if (!tc->topStmt && (tc->flags & TCF_IN_FUNCTION)) {
            JSScopeProperty *sprop;

            /*
             * Define a property on the outer function so that BindNameToSlot
             * can properly optimize accesses.
             */
            JS_ASSERT(OBJ_GET_CLASS(cx, varobj) == &js_FunctionClass);
            JS_ASSERT(fp->fun == (JSFunction *) JS_GetPrivate(cx, varobj));
            if (!js_LookupHiddenProperty(cx, varobj, ATOM_TO_JSID(funAtom),
                                         &pobj, &prop)) {
                return NULL;
            }
            if (prop)
                OBJ_DROP_PROPERTY(cx, pobj, prop);
            sprop = NULL;
            if (!prop ||
                pobj != varobj ||
                (sprop = (JSScopeProperty *)prop,
                 sprop->getter != js_GetLocalVariable)) {
                uintN sflags;

                /*
                 * Use SPROP_IS_DUPLICATE if there is a formal argument of the
                 * same name, so the decompiler can find the parameter name.
                 */
                sflags = (sprop && sprop->getter == js_GetArgument)
                         ? SPROP_IS_DUPLICATE | SPROP_HAS_SHORTID
                         : SPROP_HAS_SHORTID;
                if (!js_AddHiddenProperty(cx, varobj, ATOM_TO_JSID(funAtom),
                                          js_GetLocalVariable,
                                          js_SetLocalVariable,
                                          SPROP_INVALID_SLOT,
                                          JSPROP_PERMANENT | JSPROP_SHARED,
                                          sflags, fp->fun->nvars)) {
                    return NULL;
                }
                if (fp->fun->nvars == JS_BITMASK(16)) {
                    JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                                         JSMSG_TOO_MANY_FUN_VARS);
                    return NULL;
                }
                fp->fun->nvars++;
            }
        }
#endif
    }

    fun = js_NewFunction(cx, NULL, NULL, 0, lambda ? JSFUN_LAMBDA : 0, varobj,
                         funAtom);
    if (!fun)
        return NULL;
#if JS_HAS_GETTER_SETTER
    if (op != JSOP_NOP)
        fun->flags |= (op == JSOP_GETTER) ? JSPROP_GETTER : JSPROP_SETTER;
#endif


    /*
     * Set interpreted early so js_EmitTree can test it to decide whether to
     * eliminate useless expressions.
     */
    fun->interpreted = JS_TRUE;

    /*
     * Atomize fun->object early to protect against a last-ditch GC under
     * js_LookupHiddenProperty.
     *
     * Absent use of the new scoped local GC roots API around compiler calls,
     * we need to atomize here to protect against a GC activation.  Atoms are
     * protected from GC during compilation by the JS_FRIEND_API entry points
     * in this file.  There doesn't seem to be any gain in switching from the
     * atom-keeping method to the bulkier, slower scoped local roots method.
     */
    objAtom = js_AtomizeObject(cx, fun->object, 0);
    if (!objAtom)
        return NULL;

    /* Now parse formal argument list and compute fun->nargs. */
    MUST_MATCH_TOKEN(TOK_LP, JSMSG_PAREN_BEFORE_FORMAL);
    if (!js_MatchToken(cx, ts, TOK_RP)) {
        do {
            MUST_MATCH_TOKEN(TOK_NAME, JSMSG_MISSING_FORMAL);
            argAtom = CURRENT_TOKEN(ts).t_atom;
            pobj = NULL;
            if (!js_LookupHiddenProperty(cx, fun->object, ATOM_TO_JSID(argAtom),
                                         &pobj, &prop)) {
                return NULL;
            }
            dupflag = 0;
            if (prop) {
                ok = JS_TRUE;
                if (pobj == fun->object &&
                    ((JSScopeProperty *) prop)->getter == js_GetArgument) {
                    const char *name = js_AtomToPrintableString(cx, argAtom);

                    /*
                     * A duplicate parameter name. We force a duplicate node
                     * on the SCOPE_LAST_PROP(scope) list with the same id,
                     * distinguished by the SPROP_IS_DUPLICATE flag, and not
                     * mapped by an entry in scope.
                     */
                    ok = name &&
                         js_ReportCompileErrorNumber(cx, ts,
                                                     JSREPORT_TS |
                                                     JSREPORT_WARNING |
                                                     JSREPORT_STRICT,
                                                     JSMSG_DUPLICATE_FORMAL,
                                                     name);

                    dupflag = SPROP_IS_DUPLICATE;
                }
                OBJ_DROP_PROPERTY(cx, pobj, prop);
                if (!ok)
                    return NULL;
                prop = NULL;
            }
            if (!js_AddHiddenProperty(cx, fun->object, ATOM_TO_JSID(argAtom),
                                      js_GetArgument, js_SetArgument,
                                      SPROP_INVALID_SLOT,
                                      JSPROP_PERMANENT | JSPROP_SHARED,
                                      dupflag | SPROP_HAS_SHORTID,
                                      fun->nargs)) {
                return NULL;
            }
            if (fun->nargs == JS_BITMASK(16)) {
                JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                                     JSMSG_TOO_MANY_FUN_ARGS);
                return NULL;
            }
            fun->nargs++;
        } while (js_MatchToken(cx, ts, TOK_COMMA));

        MUST_MATCH_TOKEN(TOK_RP, JSMSG_PAREN_AFTER_FORMAL);
    }

    MUST_MATCH_TOKEN(TOK_LC, JSMSG_CURLY_BEFORE_BODY);
    pn->pn_pos.begin = CURRENT_TOKEN(ts).pos.begin;

    TREE_CONTEXT_INIT(&funtc);
    body = FunctionBody(cx, ts, fun, &funtc);
    if (!body)
        return NULL;

    MUST_MATCH_TOKEN(TOK_RC, JSMSG_CURLY_AFTER_BO