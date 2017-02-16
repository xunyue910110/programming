/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=8 et tw=80:
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
 * JS bytecode descriptors, disassemblers, and decompilers.
 */
#include "jsstddef.h"
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsdtoa.h"
#include "jsprf.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsdbgapi.h"
#include "jsemit.h"
#include "jsfun.h"
#include "jslock.h"
#include "jsobj.h"
#include "jsopcode.h"
#include "jsregexp.h"
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"

const char js_const_str[]       = "const";
const char js_var_str[]         = "var";
const char js_function_str[]    = "function";
const char js_in_str[]          = "in";
const char js_instanceof_str[]  = "instanceof";
const char js_new_str[]         = "new";
const char js_delete_str[]      = "delete";
const char js_typeof_str[]      = "typeof";
const char js_void_str[]        = "void";
const char js_null_str[]        = "null";
const char js_this_str[]        = "this";
const char js_false_str[]       = "false";
const char js_true_str[]        = "true";
const char js_default_str[]     = "default";

const char *js_incop_str[]      = {"++", "--"};

/* Pollute the namespace locally for MSVC Win16, but not for WatCom.  */
#ifdef __WINDOWS_386__
    #ifdef FAR
        #undef FAR
    #endif
#else  /* !__WINDOWS_386__ */
#ifndef FAR
#define FAR
#endif
#endif /* !__WINDOWS_386__ */

const JSCodeSpec FAR js_CodeSpec[] = {
#define OPDEF(op,val,name,token,length,nuses,ndefs,prec,format) \
    {name,token,length,nuses,ndefs,prec,format},
#include "jsopcode.tbl"
#undef OPDEF
};

uintN js_NumCodeSpecs = sizeof (js_CodeSpec) / sizeof js_CodeSpec[0];

/************************************************************************/

static ptrdiff_t
GetJumpOffset(jsbytecode *pc, jsbytecode *pc2)
{
    uint32 type;

    type = (js_CodeSpec[*pc].format & JOF_TYPEMASK);
    if (JOF_TYPE_IS_EXTENDED_JUMP(type))
        return GET_JUMPX_OFFSET(pc2);
    return GET_JUMP_OFFSET(pc2);
}

#ifdef DEBUG

JS_FRIEND_API(JSBool)
js_Disassemble(JSContext *cx, JSScript *script, JSBool lines, FILE *fp)
{
    jsbytecode *pc, *end;
    uintN len;

    pc = script->code;
    end = pc + script->length;
    while (pc < end) {
        if (pc == script->main)
            fputs("main:\n", fp);
        len = js_Disassemble1(cx, script, pc,
                              PTRDIFF(pc, script->code, jsbytecode),
                              lines, fp);
        if (!len)
            return JS_FALSE;
        pc += len;
    }
    return JS_TRUE;
}

JS_FRIEND_API(uintN)
js_Disassemble1(JSContext *cx, JSScript *script, jsbytecode *pc, uintN loc,
                JSBool lines, FILE *fp)
{
    JSOp op;
    const JSCodeSpec *cs;
    ptrdiff_t len, off, jmplen;
    uint32 type;
    JSAtom *atom;
    JSString *str;

    op = (JSOp)*pc;
    if (op >= JSOP_LIMIT) {
        char numBuf1[12], numBuf2[12];
        JS_snprintf(numBuf1, sizeof numBuf1, "%d", op);
        JS_snprintf(numBuf2, sizeof numBuf2, "%d", JSOP_LIMIT);
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                             JSMSG_BYTECODE_TOO_BIG, numBuf1, numBuf2);
        return 0;
    }
    cs = &js_CodeSpec[op];
    len = (ptrdiff_t) cs->length;
    fprintf(fp, "%05u:", loc);
    if (lines)
        fprintf(fp, "%4u", JS_PCToLineNumber(cx, script, pc));
    fprintf(fp, "  %s", cs->name);
    type = cs->format & JOF_TYPEMASK;
    switch (type) {
      case JOF_BYTE:
        if (op == JSOP_TRAP) {
            op = JS_GetTrapOpcode(cx, script, pc);
            if (op == JSOP_LIMIT)
                return 0;
            len = (ptrdiff_t) js_CodeSpec[op].length;
        }
        break;

      case JOF_JUMP:
      case JOF_JUMPX:
        off = GetJumpOffset(pc, pc);
        fprintf(fp, " %u (%d)", loc + off, off);
        break;

      case JOF_CONST:
        atom = GET_ATOM(cx, script, pc);
        str = js_ValueToSource(cx, ATOM_KEY(atom));
        if (!str)
            return 0;
        fprintf(fp, " %s", JS_GetStringBytes(str));
        break;

      case JOF_UINT16:
        fprintf(fp, " %u", GET_ARGC(pc));
        break;

#if JS_HAS_SWITCH_STATEMENT
      case JOF_TABLESWITCH:
      case JOF_TABLESWITCHX:
      {
        jsbytecode *pc2;
        jsint i, low, high;

        jmplen = (type == JOF_TABLESWITCH) ? JUMP_OFFSET_LEN
                                           : JUMPX_OFFSET_LEN;
        pc2 = pc;
        off = GetJumpOffset(pc, pc2);
        pc2 += jmplen;
        low = GET_JUMP_OFFSET(pc2);
        pc2 += JUMP_OFFSET_LEN;
        high = GET_JUMP_OFFSET(pc2);
        pc2 += JUMP_OFFSET_LEN;
        fprintf(fp, " defaultOffset %d low %d high %d", off, low, high);
        for (i = low; i <= high; i++) {
            off = GetJumpOffset(pc, pc2);
            fprintf(fp, "\n\t%d: %d", i, off);
            pc2 += jmplen;
        }
        len = 1 + pc2 - pc;
        break;
      }

      case JOF_LOOKUPSWITCH:
      case JOF_LOOKUPSWITCHX:
      {
        jsbytecode *pc2;
        jsatomid npairs;

        jmplen = (type == JOF_LOOKUPSWITCH) ? JUMP_OFFSET_LEN
                                            : JUMPX_OFFSET_LEN;
        pc2 = pc;
        off = GetJumpOffset(pc, pc2);
        pc2 += jmplen;
        npairs = GET_ATOM_INDEX(pc2);
        pc2 += ATOM_INDEX_LEN;
        fprintf(fp, " offset %d npairs %u", off, (uintN) npairs);
        while (npairs) {
            atom = GET_ATOM(cx, script, pc2);
            pc2 += ATOM_INDEX_LEN;
            off = GetJumpOffset(pc, pc2);
            pc2 += jmplen;

            str = js_ValueToSource(cx, ATOM_KEY(atom));
            if (!str)
                return 0;
            fprintf(fp, "\n\t%s: %d", JS_GetStringBytes(str), off);
            npairs--;
        }
        len = 1 + pc2 - pc;
        break;
      }
#endif /* JS_HAS_SWITCH_STATEMENT */

      case JOF_QARG:
        fprintf(fp, " %u", GET_ARGNO(pc));
        break;

      case JOF_QVAR:
        fprintf(fp, " %u", GET_VARNO(pc));
        break;

#if JS_HAS_LEXICAL_CLOSURE
      case JOF_INDEXCONST:
        fprintf(fp, " %u", GET_VARNO(pc));
        pc += VARNO_LEN;
        atom = GET_ATOM(cx, script, pc);
        str = js_ValueToSource(cx, ATOM_KEY(atom));
        if (!str)
            return 0;
        fprintf(fp, " %s", JS_GetStringBytes(str));
        break;
#endif

      case JOF_UINT24:
        if (op == JSOP_FINDNAME) {
            /* Special case to avoid a JOF_FINDNAME just for this op. */
            atom = js_GetAtom(cx, &script->atomMap, GET_LITERAL_INDEX(pc));
            str = js_ValueToSource(cx, ATOM_KEY(atom));
            if (!str)
                return 0;
            fprintf(fp, " %s", JS_GetStringBytes(str));
            break;
        }

        JS_ASSERT(op == JSOP_UINT24 || op == JSOP_LITERAL);
        fprintf(fp, " %u", GET_LITERAL_INDEX(pc));
        break;

      case JOF_LITOPX:
        atom = js_GetAtom(cx, &script->atomMap, GET_LITERAL_INDEX(pc));
        str = js_ValueToSource(cx, ATOM_KEY(atom));
        if (!str)
            return 0;

        /*
         * Bytecode: JSOP_LITOPX <uint24> op [<varno> if JSOP_DEFLOCALFUN].
         * Advance pc to point at op.
         */
        pc += 1 + LITERAL_INDEX_LEN;
        op = *pc;
        cs = &js_CodeSpec[op];
        fprintf(fp, " %s op %s", JS_GetStringBytes(str), cs->name);
#if JS_HAS_LEXICAL_CLOSURE
        if ((cs->format & JOF_TYPEMASK) == JOF_INDEXCONST)
            fprintf(fp, " %u", GET_VARNO(pc));
#endif

        /*
         * Set len to advance pc to skip op and any other immediates (namely,
         * <varno> if JSOP_DEFLOCALFUN).
         */
        JS_ASSERT(cs->length > ATOM_INDEX_LEN);
        len = cs->length - ATOM_INDEX_LEN;
        break;

      default: {
        char numBuf[12];
        JS_snprintf(numBuf, sizeof numBuf, "%lx", (unsigned long) cs->format);
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                             JSMSG_UNKNOWN_FORMAT, numBuf);
        return 0;
      }
    }
    fputs("\n", fp);
    return len;
}

#endif /* DEBUG */

/************************************************************************/

/*
 * Sprintf, but with unlimited and automatically allocated buffering.
 */
typedef struct Sprinter {
    JSContext       *context;       /* context executing the decompiler */
    JSArenaPool     *pool;          /* string allocation pool */
    char            *base;          /* base address of buffer in pool */
    size_t          size;           /* size of buffer allocated at base */
    ptrdiff_t       offset;         /* offset of next free char in buffer */
} Sprinter;

#define INIT_SPRINTER(cx, sp, ap, off) \
    ((sp)->context = cx, (sp)->pool = ap, (sp)->base = NULL, (sp)->size = 0,  \
     (sp)->offset = off)

#define OFF2STR(sp,off) ((sp)->base + (off))
#define STR2OFF(sp,str) ((str) - (sp)->base)
#define RETRACT(sp,str) ((sp)->offset = STR2OFF(sp, str))

static JSBool
SprintAlloc(Sprinter *sp, size_t nb)
{
    if (!sp->base) {
        JS_ARENA_ALLOCATE_CAST(sp->base, char *, sp->pool, nb);
    } else {
        JS_ARENA_GROW_CAST(sp->base, char *, sp->pool, sp->size, nb);
    }
    if (!sp->base) {
        JS_ReportOutOfMemory(sp->context);
        return JS_FALSE;
    }
    sp->size += nb;
    return JS_TRUE;
}

static ptrdiff_t
SprintPut(Sprinter *sp, const char *s, size_t len)
{
    ptrdiff_t nb, offset;
    char *bp;

    /* Allocate space for s, including the '\0' at the end. */
    nb = (sp->offset + len + 1) - sp->size;
    if (nb > 0 && !SprintAlloc(sp, nb))
        return -1;

    /* Advance offset and copy s into sp's buffer. */
    offset = sp->offset;
    sp->offset += len;
    bp = sp->base + offset;
    memmove(bp, s, len);
    bp[len] = 0;
    return offset;
}

static ptrdiff_t
Sprint(Sprinter *sp, const char *format, ...)
{
    va_list ap;
    char *bp;
    ptrdiff_t offset;

    va_start(ap, format);
    bp = JS_vsmprintf(format, ap);      /* XXX vsaprintf */
    va_end(ap);
    if (!bp) {
        JS_ReportOutOfMemory(sp->context);
        return -1;
    }
    offset = SprintPut(sp, bp, strlen(bp));
    free(bp);
    return offset;
}

const jschar js_EscapeMap[] = {
    '\b', 'b',
    '\f', 'f',
    '\n', 'n',
    '\r', 'r',
    '\t', 't',
    '\v', 'v',
    '"',  '"',
    '\'', '\'',
    '\\', '\\',
    0
};

static char *
QuoteString(Sprinter *sp, JSString *str, jschar quote)
{
    ptrdiff_t off, len, nb;
    const jschar *s, *t, *u, *z;
    char *bp;
    jschar c;
    JSBool ok;

    /* Sample off first for later return value pointer computation. */
    off = sp->offset;
    if (quote && Sprint(sp, "%c", (char)quote) < 0)
        return NULL;

    /* Loop control variables: z points at end of string sentinel. */
    s = JSSTRING_CHARS(str);
    z = s + JSSTRING_LENGTH(str);
    for (t = s; t < z; s = ++t) {
        /* Move t forward from s past un-quote-worthy characters. */
        c = *t;
        while (JS_ISPRINT(c) && c != quote && c != '\\' && !(c >> 8)) {
            c = *++t;
            if (t == z)
                break;
        }
        len = PTRDIFF(t, s, jschar);

        /* Allocate space for s, including the '\0' at the end. */
        nb = (sp->offset + len + 1) - sp->size;
        if (nb > 0 && !SprintAlloc(sp, nb))
            return NULL;

        /* Advance sp->offset and copy s into sp's buffer. */
        bp = sp->base + sp->offset;
        sp->offset += len;
        while (--len >= 0)
            *bp++ = (char) *s++;
        *bp = '\0';

        if (t == z)
            break;

        /* Use js_EscapeMap, \u, or \x only if necessary. */
        if ((u = js_strchr(js_EscapeMap, c)) != NULL) {
            ok = Sprint(sp, "\\%c", (char)u[1]) >= 0;
        } else {
#ifdef JS_C_STRINGS_ARE_UTF8
            /* If this is a surrogate pair, make sure to print the pair. */
            if (c >= 0xD800 && c <= 0xDBFF) {
                jschar buffer[3];
                buffer[0] = c;
                buffer[1] = *++t;
                buffer[2] = 0;
                if (t == z) {
                    char numbuf[10];
                    JS_snprintf(numbuf, sizeof numbuf, "0x%x", c);
                    JS_ReportErrorFlagsAndNumber(cx, JSREPORT_ERROR,
                                                 js_GetErrorMessage, NULL,
                                                 JSMSG_BAD_SURROGATE_CHAR,
                                                 numbuf);
                    ok = JS_FALSE;
                    break;
                }
                ok = Sprint(sp, "%hs", buffer) >= 0;
            } else {
                /* Print as UTF-8 string. */
                ok = Sprint(sp, "%hc", c) >= 0;
            }
#else
            /* Use \uXXXX or \xXX  if the string can't be displayed as UTF-8. */
            ok = Sprint(sp, (c >> 8) ? "\\u%04X" : "\\x%02X", c) >= 0;
#endif
        }
        if (!ok)
            return NULL;
    }

    /* Sprint the closing quote and return the quoted string. */
    if (quote && Sprint(sp, "%c", (char)quote) < 0)
        return NULL;

    /*
     * If we haven't Sprint'd anything yet, Sprint an empty string so that
     * the OFF2STR below gives a valid result.
     */
    if (off == sp->offset && Sprint(sp, "") < 0)
        return NULL;
    return OFF2STR(sp, off);
}

JSString *
js_QuoteString(JSContext *cx, JSString *str, jschar quote)
{
    void *mark;
    Sprinter sprinter;
    char *bytes;
    JSString *escstr;

    mark = JS_ARENA_MARK(&cx->tempPool);
    INIT_SPRINTER(cx, &sprinter, &cx->tempPool, 0);
    bytes = QuoteString(&sprinter, str, quote);
    escstr = bytes ? JS_NewStringCopyZ(cx, bytes) : NULL;
    JS_ARENA_RELEASE(&cx->tempPool, mark);
    return escstr;
}

/************************************************************************/

struct JSPrinter {
    Sprinter        sprinter;       /* base class state */
    JSArenaPool     pool;           /* string allocation pool */
    uintN           indent;         /* indentation in spaces */
    JSPackedBool    pretty;         /* pretty-print: indent, use newlines */
    JSPackedBool    grouped;        /* in parenthesized expression context */
    JSScript        *script;        /* script being printed */
    JSScope         *scope;         /* script function scope */
};

/*
 * Hack another flag, a la JS_DONT_PRETTY_PRINT, into uintN indent parameters
 * to functions such as js_DecompileFunction and js_NewPrinter.  This time, as
 * opposed to JS_DONT_PRETTY_PRINT back in the dark ages, we can assume that a
 * uintN is at least 32 bits.
 */
#define JS_IN_GROUP_CONTEXT 0x10000

JSPrinter *
js_NewPrinter(JSContext *cx, const char *name, uintN indent, JSBool pretty)
{
    JSPrinter *jp;

    jp = (JSPrinter *) JS_malloc(cx, sizeof(JSPrinter));
    if (!jp)
        return NULL;
    INIT_SPRINTER(cx, &jp->sprinter, &jp->pool, 0);
    JS_InitArenaPool(&jp->pool, name, 256, 1);
    jp->indent = indent & ~JS_IN_GROUP_CONTEXT;
    jp->pretty = pretty;
    jp->grouped = (indent & JS_IN_GROUP_CONTEXT) != 0;
    jp->script = NULL;
    jp->scope = NULL;
    return jp;
}

void
js_DestroyPrinter(JSPrinter *jp)
{
    JS_FinishArenaPool(&jp->pool);
    JS_free(jp->sprinter.context, jp);
}

JSString *
js_GetPrinterOutput(JSPrinter *jp)
{
    JSContext *cx;
    JSString *str;

    cx = jp->sprinter.context;
    if (!jp->sprinter.base)
        return cx->runtime->emptyString;
    str = JS_NewStringCopyZ(cx, jp->sprinter.base);
    if (!str)
        return NULL;
    JS_FreeArenaPool(&jp->pool);
    INIT_SPRINTER(cx, &jp->sprinter, &jp->pool, 0);
    return str;
}

int
js_printf(JSPrinter *jp, const char *format, ...)
{
    va_list ap;
    char *bp, *fp;
    int cc;

    if (*format == '\0')
        return 0;

    va_start(ap, format);

    /* If pretty-printing, expand magic tab into a run of jp->indent spaces. */
    if (*format == '\t') {
        if (jp->pretty && Sprint(&jp->sprinter, "%*s", jp->indent, "") < 0)
            return -1;
        format++;
    }

    /* Suppress newlines (must be once per format, at the end) if not pretty. */
    fp = NULL;
    if (!jp->pretty && format[cc = strlen(format) - 1] == '\n') {
        fp = JS_strdup(jp->sprinter.context, format);
        if (!fp)
            return -1;
        fp[cc] = '\0';
        format = fp;
    }

    /* Allocate temp space, convert format, and put. */
    bp = JS_vsmprintf(format, ap);      /* XXX vsaprintf */
    if (fp) {
        JS_free(jp->sprinter.context, fp);
        format = NULL;
    }
    if (!bp) {
        JS_ReportOutOfMemory(jp->sprinter.context);
        return -1;
    }

    cc = strlen(bp);
    if (SprintPut(&jp->sprinter, bp, (size_t)cc) < 0)
        cc = -1;
    free(bp);

    va_end(ap);
    return cc;
}

JSBool
js_puts(JSPrinter *jp, const char *s)
{
    return SprintPut(&jp->sprinter, s, strlen(s)) >= 0;
}

/************************************************************************/

typedef struct SprintStack {
    Sprinter    sprinter;       /* sprinter for postfix to infix buffering */
    ptrdiff_t   *offsets;       /* stack of postfix string offsets */
    jsbytecode  *opcodes;       /* parallel stack of JS opcodes */
    uintN       top;            /* top of stack index */
    JSPrinter   *printer;       /* permanent output goes here */
} SprintStack;

/* Gap between stacked strings to allow for insertion of parens and commas. */
#define PAREN_SLOP      (2 + 1)

/*
 * These pseudo-ops help js_DecompileValueGenerator decompile JSOP_SETNAME,
 * JSOP_SETPROP, and JSOP_SETELEM, respectively.  See the first assertion in
 * PushOff.
 */
#define JSOP_GETPROP2   254
#define JSOP_GETELEM2   255

static JSBool
PushOff(SprintStack *ss, ptrdiff_t off, JSOp op)
{
    uintN top;

#if JSOP_LIMIT > JSOP_GETPROP2
#error JSOP_LIMIT must be <= JSOP_GETPROP2
#endif
    if (!SprintAlloc(&ss->sprinter, PAREN_SLOP))
        return JS_FALSE;

    /* ss->top points to the next free slot; be paranoid about overflow. */
    top = ss->top;
    JS_ASSERT(top < ss->printer->script->depth);
    if (top >= ss->printer->script->depth) {
        JS_ReportOutOfMemory(ss->sprinter.context);
        return JS_FALSE;
    }

    /* The opcodes stack must contain real bytecodes that index js_CodeSpec. */
    ss->offsets[top] = off;
    ss->opcodes[top] = (op == JSOP_GETPROP2) ? JSOP_GETPROP
                     : (op == JSOP_GETELEM2) ? JSOP_GETELEM
                     : (jsbytecode) op;
    ss->top = ++top;
    ss->sprinter.offset += PAREN_SLOP;
    return JS_TRUE;
}

static ptrdiff_t
PopOff(SprintStack *ss, JSOp op)
{
    uintN top;
    const JSCodeSpec *cs, *topcs;
    ptrdiff_t off;

    /* ss->top points to the next free slot; be paranoid about underflow. */
    top = ss->top;
    JS_ASSERT(top != 0);
    if (top == 0)
        return 0;

    ss->top = --top;
    topcs = &js_CodeSpec[ss->opcodes[top]];
    cs = &js_CodeSpec[op];
    if (topcs->prec != 0 && topcs->prec < cs->prec) {
        ss->offsets[top] -= 2;
        ss->sprinter.offset = ss->offsets[top];
        off = Sprint(&ss->sprinter, "(%s)",
                     OFF2STR(&ss->sprinter, ss->sprinter.offset + 2));
    } else {
        off = ss->sprinter.offset = ss->offsets[top];
    }
    return off;
}

#if JS_HAS_SWITCH_STATEMENT
typedef struct TableEntry {
    jsval       key;
    ptrdiff_t   offset;
    JSAtom      *label;
    jsint       order;          /* source order for stable tableswitch sort */
} TableEntry;

static int
CompareOffsets(const void *v1, const void *v2, void *arg)
{
    const TableEntry *te1 = (const TableEntry *) v1,
                     *te2 = (const TableEntry *) v2;

    if (te1->offset == te2->offset)
        return (int) (te1->order - te2->order);
    return (int) (te1->offset - te2->offset);
}

static JSBool
Decompile(SprintStack *ss, jsbytecode *pc, intN nb);

static JSBool
DecompileSwitch(SprintStack *ss, TableEntry *table, uintN tableLength,
                jsbytecode *pc, ptrdiff_t switchLength,
                ptrdiff_t defaultOffset, JSBool isCondSwitch)
{
    JSContext *cx;
    JSPrinter *jp;
    char *lval, *rval;
    uintN i;
    ptrdiff_t diff, off, off2, caseExprOff;
    jsval key;
    JSString *str;

    cx = ss->sprinter.context;
    jp = ss->printer;

    lval = OFF2STR(&ss->sprinter, PopOff(ss, JSOP_NOP));
    js_printf(jp, "\tswitch (%s) {\n", lval);

    if (tableLength) {
        diff = table[0].offset - defaultOffset;
        if (diff > 0) {
            jp->indent += 2;
            js_printf(jp, "\t%s:\n", js_default_str);
            jp->indent += 2;
            if (!Decompile(ss, pc + defaultOffset, diff))
                return JS_FALSE;
            jp->indent -= 4;
        }

        caseExprOff = isCondSwitch
                      ? (ptrdiff_t) js_CodeSpec[JSOP_CONDSWITCH].length
                      : 0;

        for (i = 0; i < tableLength; i++) {
            off = table[i].offset;
            off2 = (i + 1 < tableLength) ? table[i + 1].offset : switchLength;

            key = table[i].key;
            if (isCondSwitch) {
                ptrdiff_t nextCaseExprOff;

                /*
                 * key encodes the JSOP_CASE bytecode's offset from switchtop.
                 * The next case expression follows immediately, unless we are
                 * at the last case.
                 */
                nextCaseExprOff = (ptrdiff_t)JSVAL_TO_INT(key);
                nextCaseExprOff += js_CodeSpec[pc[nextCaseExprOff]].length;
                jp->indent += 2;
                if (!Decompile(ss, pc + caseExprOff,
                               nextCaseExprOff - caseExprOff)) {
                    return JS_FALSE;
                }
                caseExprOff = nextCaseExprOff;
            } else {
                /*
                 * key comes from an atom, not the decompiler, so we need to
                 * quote it if it's a string literal.  But if table[i].label
                 * is non-null, key was constant-propagated and label is the
                 * name of the const we should show as the case label.  We set
                 * key to undefined so this identifier is escaped, if required
                 * by non-ASCII characters, but not quoted, by QuoteString.
                 */
                if (table[i].label) {
                    str = ATOM_TO_STRING(table[i].label);
                    key = JSVAL_VOID;
                } else {
                    str = js_ValueToString(cx, key);
                    if (!str)
                        return JS_FALSE;
                }
                rval = QuoteString(&ss->sprinter, str,
                                   (jschar)(JSVAL_IS_STRING(key) ? '"' : 0));
                if (!rval)
                    return JS_FALSE;
                RETRACT(&ss->sprinter, rval);
                jp->indent += 2;
                js_printf(jp, "\tcase %s:\n", rval);
            }

            jp->indent += 2;
            if (off <= defaultOffset && defaultOffset < off2) {
                diff = defaultOffset - off;
                if (diff != 0) {
                    if (!Decompile(ss, pc + off, diff))
                        return JS_FALSE;
                    off = defaultOffset;
                }
                jp->indent -= 2;
                js_printf(jp, "\t%s:\n", js_default_str);
                jp->indent += 2;
            }
            if (!Decompile(ss, pc + off, off2 - off))
                return JS_FALSE;
            jp->indent -= 4;
        }
    }

    if (defaultOffset == switchLength) {
        jp->indent += 2;
        js_printf(jp, "\t%s:;\n", js_default_str);
        jp->indent -= 2;
    }
    js_printf(jp, "\t}\n");
    return JS_TRUE;
}
#endif

static JSAtom *
GetSlotAtom(JSPrinter *jp, JSPropertyOp getter, uintN slot)
{
    JSScope *scope;
    JSScopeProperty *sprop;
    JSObject *obj, *proto;

    scope = jp->scope;
    while (scope) {
        for (sprop = SCOPE_LAST_PROP(scope); sprop; sprop = sprop->parent) {
            if (sprop->getter != getter)
                continue;
            JS_ASSERT(sprop->flags & SPROP_HAS_SHORTID);
            JS_ASSERT(JSID_IS_ATOM(sprop->id));
            if ((uintN) sprop->shortid == slot)
                return JSID_TO_ATOM(sprop->id);
        }
        obj = scope->object;
        if (!obj)
            break;
        proto = OBJ_GET_PROTO(jp->sprinter.context, obj);
        if (!proto)
            break;
        scope = OBJ_SCOPE(proto);
    }
    return NULL;
}

static const char *
VarPrefix(jssrcnote *sn)
{
    if (sn) {
        if (SN_TYPE(sn) == SRC_VAR)
            return "var ";
        if (SN_TYPE(sn) == SRC_CONST)
            return "const ";
    }
    return "";
}

static JSBool
Decompile(SprintStack *ss, jsbytecode *pc, intN nb)
{
    JSContext *cx;
    JSPrinter *jp, *jp2;
    jsbytecode *endpc, *done, *forelem_tail, *forelem_done;
    ptrdiff_t tail, todo, len, oplen, cond, next;
    JSOp op, lastop, saveop;
    const JSCodeSpec *cs, *topcs;
    jssrcnote *sn, *sn2;
    const char *lval, *rval, *xval, *fmt;
    jsint i, argc;
    char **argv;
    jsatomid atomIndex;
    JSAtom *atom;
    JSObject *obj;
    JSFunction *fun;
    JSString *str;
    JSBool ok;
#if JS_HAS_XML_SUPPORT
    JSBool foreach, inXML, quoteAttr;
#else
#define inXML JS_FALSE
#endif
    jsval val;
    int stackDummy;
    static const char catch_cookie[] = "/*CATCH*/";
    static const char with_cookie[] = "/*WITH*/";

/*
 * Local macros
 */
#define DECOMPILE_CODE(pc,nb)   if (!Decompile(ss, pc, nb)) return JS_FALSE
#define POP_STR()               OFF2STR(&ss->sprinter, PopOff(ss, op))
#define LOCAL_ASSERT(expr)      JS_ASSERT(expr); if (!(expr)) return JS_FALSE

/*
 * Callers know that ATOM_IS_STRING(atom), and we leave it to the optimizer to
 * common ATOM_TO_STRING(atom) here and near the call sites.
 */
#define ATOM_IS_IDENTIFIER(atom)                                              \
    (!ATOM_KEYWORD(atom) && js_IsIdentifier(ATOM_TO_STRING(atom)))

/*
 * Given an atom already fetched from jp->script's atom map, quote/escape its
 * string appropriately into rval, and select fmt from the quoted and unquoted
 * alternatives.
 */
#define GET_QUOTE_AND_FMT(qfmt, ufmt, rval)                                   \
    JS_BEGIN_MACRO                                                            \
        jschar quote_;                                                        \
        if (!ATOM_IS_IDENTIFIER(atom)) {                                      \
            quote_ = '\'';                                                    \
            fmt = qfmt;                                                       \
        } else {                                                              \
            quote_ = 0;                                                       \
            fmt = ufmt;                                                       \
        }                                                                     \
        rval = QuoteString(&ss->sprinter, ATOM_TO_STRING(atom), quote_);      \
        if (!rval)                                                            \
            return JS_FALSE;                                                  \
    JS_END_MACRO

/*
 * Get atom from jp->script's atom map, quote/escape its string appropriately
 * into rval, and select fmt from the quoted and unquoted alternatives.
 */
#define GET_ATOM_QUOTE_AND_FMT(qfmt, ufmt, rval)                              \
    JS_BEGIN_MACRO                                                            \
        atom = GET_ATOM(cx, jp->script, pc);                                  \
        GET_QUOTE_AND_FMT(qfmt, ufmt, rval);                                  \
    JS_END_MACRO

    cx = ss->sprinter.context;
    if (!JS_CHECK_STACK_SIZE(cx, stackDummy)) {
        JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_OVER_RECURSED);
        return JS_FALSE;
    }

    jp = ss->printer;
    endpc = pc + nb;
    forelem_tail = forelem_done = NULL;
    tail = -1;
    todo = -2;                  /* NB: different from Sprint() error return. */
    op = JSOP_NOP;
    sn = NULL;
    rval = NULL;
#if JS_HAS_XML_SUPPORT
    foreach = inXML = quoteAttr = JS_FALSE;
#endif

    while (pc < endpc) {
        lastop = op;
        op = saveop = (JSOp) *pc;
        if (op >= JSOP_LIMIT) {
            switch (op) {
              case JSOP_GETPROP2:
                saveop = JSOP_GETPROP;
                break;
              case JSOP_GETELEM2:
                saveop = JSOP_GETELEM;
                break;
              default:;
            }
        }
        cs = &js_CodeSpec[saveop];
        len = oplen = cs->length;

        if (cs->token) {
            switch (cs->nuses) {
              case 2:
                rval = POP_STR();
                lval = POP_STR();
                sn = js_GetSrcNote(jp->script, pc);
                if (sn && SN_TYPE(sn) == SRC_ASSIGNOP) {
                    /* Print only the right operand of the assignment-op. */
                    todo = SprintPut(&ss->sprinter, rval, strlen(rval));
                } else if (!inXML) {
                    todo = Sprint(&ss->sprinter, "%s %s %s",
                                  lval, cs->token, rval);
                } else {
                    /* In XML, just concatenate the two operands. */
                    JS_ASSERT(op == JSOP_ADD);
                    todo = Sprint(&ss->sprinter, "%s%s", lval, rval);
                }
                break;

              case 1:
                rval = POP_STR();
                todo = Sprint(&ss->sprinter, "%s%s", cs->token, rval);
                break;

              case 0:
                todo = SprintPut(&ss->sprinter, cs->token, strlen(cs->token));
                break;

              default:
                todo = -2;
                break;
            }
        } else {
            switch (op) {
#define BEGIN_LITOPX_CASE(OP)                                                 \
              case OP:                                                        \
                atomIndex = GET_ATOM_INDEX(pc);                               \
              do_##OP:                                                        \
                atom = js_GetAtom(cx, &jp->script->atomMap, atomIndex);

#define END_LITOPX_CASE                                                       \
                break;

              case JSOP_NOP:
                /*
                 * Check for a do-while loop, a for-loop with an empty
                 * initializer part, a labeled statement, a function
                 * definition, or try/finally.
                 */
                sn = js_GetSrcNote(jp->script, pc);
                todo = -2;
                switch (sn ? SN_TYPE(sn) : SRC_NULL) {
#if JS_HAS_DO_WHILE_LOOP
                  case SRC_WHILE:
                    js_pr