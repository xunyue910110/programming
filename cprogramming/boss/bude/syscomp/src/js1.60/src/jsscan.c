/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=8 et tw=78:
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
 * JS lexical scanner.
 */
#include "jsstddef.h"
#include <stdio.h>      /* first to avoid trouble on some systems */
#include <errno.h>
#include <limits.h>
#include <math.h>
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsdtoa.h"
#include "jsprf.h"
#include "jsapi.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsemit.h"
#include "jsexn.h"
#include "jsnum.h"
#include "jsopcode.h"
#include "jsregexp.h"
#include "jsscan.h"
#include "jsscript.h"

#if JS_HAS_XML_SUPPORT
#include "jsparse.h"
#include "jsxml.h"
#endif

#define RESERVE_JAVA_KEYWORDS
#define RESERVE_ECMA_KEYWORDS

#define MAX_KEYWORD_LENGTH      12

static struct keyword {
    const char  *name;
    JSTokenType tokentype;      /* JSTokenType */
    JSOp        op;             /* JSOp */
    JSVersion   version;        /* JSVersion */
} keywords[] = {
    {"break",           TOK_BREAK,              JSOP_NOP,   JSVERSION_DEFAULT},
    {"case",            TOK_CASE,               JSOP_NOP,   JSVERSION_DEFAULT},
    {"continue",        TOK_CONTINUE,           JSOP_NOP,   JSVERSION_DEFAULT},
    {js_default_str,    TOK_DEFAULT,            JSOP_NOP,   JSVERSION_DEFAULT},
    {js_delete_str,     TOK_DELETE,             JSOP_NOP,   JSVERSION_DEFAULT},
    {"do",              TOK_DO,                 JSOP_NOP,   JSVERSION_DEFAULT},
    {"else",            TOK_ELSE,               JSOP_NOP,   JSVERSION_DEFAULT},
    {"export",          TOK_EXPORT,             JSOP_NOP,   JSVERSION_1_2},
    {js_false_str,      TOK_PRIMARY,            JSOP_FALSE, JSVERSION_DEFAULT},
    {"for",             TOK_FOR,                JSOP_NOP,   JSVERSION_DEFAULT},
    {js_function_str,   TOK_FUNCTION,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"if",              TOK_IF,                 JSOP_NOP,   JSVERSION_DEFAULT},
    {js_in_str,         TOK_IN,                 JSOP_IN,    JSVERSION_DEFAULT},
    {js_new_str,        TOK_NEW,                JSOP_NEW,   JSVERSION_DEFAULT},
    {js_null_str,       TOK_PRIMARY,            JSOP_NULL,  JSVERSION_DEFAULT},
    {"return",          TOK_RETURN,             JSOP_NOP,   JSVERSION_DEFAULT},
    {"switch",          TOK_SWITCH,             JSOP_NOP,   JSVERSION_DEFAULT},
    {js_this_str,       TOK_PRIMARY,            JSOP_THIS,  JSVERSION_DEFAULT},
    {js_true_str,       TOK_PRIMARY,            JSOP_TRUE,  JSVERSION_DEFAULT},
    {js_typeof_str,     TOK_UNARYOP,            JSOP_TYPEOF,JSVERSION_DEFAULT},
    {js_var_str,        TOK_VAR,                JSOP_DEFVAR,JSVERSION_DEFAULT},
    {js_void_str,       TOK_UNARYOP,            JSOP_VOID,  JSVERSION_DEFAULT},
    {"while",           TOK_WHILE,              JSOP_NOP,   JSVERSION_DEFAULT},
    {"with",            TOK_WITH,               JSOP_NOP,   JSVERSION_DEFAULT},

#if JS_HAS_CONST
    {js_const_str,      TOK_VAR,                JSOP_DEFCONST,JSVERSION_DEFAULT},
#else
    {js_const_str,      TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
#endif

#if JS_HAS_EXCEPTIONS
    {"try",             TOK_TRY,                JSOP_NOP,   JSVERSION_DEFAULT},
    {"catch",           TOK_CATCH,              JSOP_NOP,   JSVERSION_DEFAULT},
    {"finally",         TOK_FINALLY,            JSOP_NOP,   JSVERSION_DEFAULT},
    {"throw",           TOK_THROW,              JSOP_NOP,   JSVERSION_DEFAULT},
#else
    {"try",             TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"catch",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"finally",         TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"throw",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
#endif

#if JS_HAS_INSTANCEOF
    {js_instanceof_str, TOK_INSTANCEOF,         JSOP_INSTANCEOF,JSVERSION_1_4},
#else
    {js_instanceof_str, TOK_RESERVED,           JSOP_NOP,   JSVERSION_1_4},
#endif

#ifdef RESERVE_JAVA_KEYWORDS
    {"abstract",        TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"boolean",         TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"byte",            TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"char",            TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"class",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"double",          TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"extends",         TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"final",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"float",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"goto",            TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"implements",      TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"import",          TOK_IMPORT,             JSOP_NOP,   JSVERSION_DEFAULT},
    {"int",             TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"interface",       TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"long",            TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"native",          TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"package",         TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"private",         TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"protected",       TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"public",          TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"short",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"static",          TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"super",           TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"synchronized",    TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"throws",          TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"transient",       TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
    {"volatile",        TOK_RESERVED,           JSOP_NOP,   JSVERSION_DEFAULT},
#endif

#ifdef RESERVE_ECMA_KEYWORDS
    {"enum",           TOK_RESERVED,            JSOP_NOP,   JSVERSION_1_3},
#endif

#if JS_HAS_DEBUGGER_KEYWORD
    {"debugger",       TOK_DEBUGGER,            JSOP_NOP,   JSVERSION_1_3},
#elif defined(RESERVE_ECMA_KEYWORDS)
    {"debugger",       TOK_RESERVED,            JSOP_NOP,   JSVERSION_1_3},
#endif
    {0,                TOK_EOF,                 JSOP_NOP,   JSVERSION_DEFAULT}
};

JSBool
js_InitScanner(JSContext *cx)
{
    struct keyword *kw;
    size_t length;
    JSAtom *atom;

    for (kw = keywords; kw->name; kw++) {
        length = strlen(kw->name);
        JS_ASSERT(length <= MAX_KEYWORD_LENGTH);
        atom = js_Atomize(cx, kw->name, length, ATOM_PINNED);
        if (!atom)
            return JS_FALSE;
        ATOM_SET_KEYWORD(atom, kw);
    }
    return JS_TRUE;
}

JS_FRIEND_API(void)
js_MapKeywords(void (*mapfun)(const char *))
{
    struct keyword *kw;

    for (kw = keywords; kw->name; kw++)
        mapfun(kw->name);
}

JSTokenStream *
js_NewTokenStream(JSContext *cx, const jschar *base, size_t length,
                  const char *filename, uintN lineno,
                  JSPrincipals *principals)
{
    JSTokenStream *ts;

    ts = js_NewBufferTokenStream(cx, base, length);
    if (!ts)
        return NULL;
    ts->filename = filename;
    ts->lineno = lineno;
    if (principals)
        JSPRINCIPALS_HOLD(cx, principals);
    ts->principals = principals;
    return ts;
}

#define TBMIN   64

static JSBool
GrowTokenBuf(JSStringBuffer *sb, size_t newlength)
{
    JSContext *cx;
    jschar *base;
    ptrdiff_t offset, length;
    size_t tbsize;
    JSArenaPool *pool;

    cx = sb->data;
    base = sb->base;
    offset = PTRDIFF(sb->ptr, base, jschar);
    pool = &cx->tempPool;
    if (!base) {
        tbsize = TBMIN * sizeof(jschar);
        length = TBMIN - 1;
        JS_ARENA_ALLOCATE_CAST(base, jschar *, pool, tbsize);
    } else {
        length = PTRDIFF(sb->limit, base, jschar);
        tbsize = (length + 1) * sizeof(jschar);
        length += length + 1;
        JS_ARENA_GROW_CAST(base, jschar *, pool, tbsize, tbsize);
    }
    if (!base) {
        JS_ReportOutOfMemory(cx);
        sb->base = STRING_BUFFER_ERROR_BASE;
        return JS_FALSE;
    }
    sb->base = base;
    sb->limit = base + length;
    sb->ptr = base + offset;
    return JS_TRUE;
}

JS_FRIEND_API(JSTokenStream *)
js_NewBufferTokenStream(JSContext *cx, const jschar *base, size_t length)
{
    size_t nb;
    JSTokenStream *ts;

    nb = sizeof(JSTokenStream) + JS_LINE_LIMIT * sizeof(jschar);
    JS_ARENA_ALLOCATE_CAST(ts, JSTokenStream *, &cx->tempPool, nb);
    if (!ts) {
        JS_ReportOutOfMemory(cx);
        return NULL;
    }
    memset(ts, 0, nb);
    ts->lineno = 1;
    ts->linebuf.base = ts->linebuf.limit = ts->linebuf.ptr = (jschar *)(ts + 1);
    ts->userbuf.base = (jschar *)base;
    ts->userbuf.limit = (jschar *)base + length;
    ts->userbuf.ptr = (jschar *)base;
    ts->tokenbuf.grow = GrowTokenBuf;
    ts->tokenbuf.data = cx;
    ts->listener = cx->runtime->sourceHandler;
    ts->listenerData = cx->runtime->sourceHandlerData;
    return ts;
}

JS_FRIEND_API(JSTokenStream *)
js_NewFileTokenStream(JSContext *cx, const char *filename, FILE *defaultfp)
{
    jschar *base;
    JSTokenStream *ts;
    FILE *file;

    JS_ARENA_ALLOCATE_CAST(base, jschar *, &cx->tempPool,
                           JS_LINE_LIMIT * sizeof(jschar));
    if (!base)
        return NULL;
    ts = js_NewBufferTokenStream(cx, base, JS_LINE_LIMIT);
    if (!ts)
        return NULL;
    if (!filename || strcmp(filename, "-") == 0) {
        file = defaultfp;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL, JSMSG_CANT_OPEN,
                                 filename, "No such file or directory");
            return NULL;
        }
    }
    ts->userbuf.ptr = ts->userbuf.limit;
    ts->file = file;
    ts->filename = filename;
    return ts;
}

JS_FRIEND_API(JSBool)
js_CloseTokenStream(JSContext *cx, JSTokenStream *ts)
{
    if (ts->flags & TSF_OWNFILENAME)
        JS_free(cx, (void *) ts->filename);
    if (ts->principals)
        JSPRINCIPALS_DROP(cx, ts->principals);
    return !ts->file || fclose(ts->file) == 0;
}

JS_FRIEND_API(int)
js_fgets(char *buf, int size, FILE *file)
{
    int n, i, c;
    JSBool crflag;

    n = size - 1;
    if (n < 0)
        return -1;

    crflag = JS_FALSE;
    for (i = 0; i < n && (c = getc(file)) != EOF; i++) {
        buf[i] = c;
        if (c == '\n') {        /* any \n ends a line */
            i++;                /* keep the \n; we know there is room for \0 */
            break;
        }
        if (crflag) {           /* \r not followed by \n ends line at the \r */
            ungetc(c, file);
            break;              /* and overwrite c in buf with \0 */
        }
        crflag = (c == '\r');
    }

    buf[i] = '\0';
    return i;
}

static int32
GetChar(JSTokenStream *ts)
{
    int32 c;
    ptrdiff_t i, j, len, olen;
    JSBool crflag;
    char cbuf[JS_LINE_LIMIT];
    jschar *ubuf, *nl;

    if (ts->ungetpos != 0) {
        c = ts->ungetbuf[--ts->ungetpos];
    } else {
        do {
            if (ts->linebuf.ptr == ts->linebuf.limit) {
                len = PTRDIFF(ts->userbuf.limit, ts->userbuf.ptr, jschar);
                if (len <= 0) {
                    if (!ts->file) {
                        ts->flags |= TSF_EOF;
                        return EOF;
                    }

                    /* Fill ts->userbuf so that \r and \r\n convert to \n. */
                    crflag = (ts->flags & TSF_CRFLAG) != 0;
                    len = js_fgets(cbuf, JS_LINE_LIMIT - crflag, ts->file);
                    if (len <= 0) {
                        ts->flags |= TSF_EOF;
                        return EOF;
                    }
                    olen = len;
                    ubuf = ts->userbuf.base;
                    i = 0;
                    if (crflag) {
                        ts->flags &= ~TSF_CRFLAG;
                        if (cbuf[0] != '\n') {
                            ubuf[i++] = '\n';
                            len++;
                            ts->linepos--;
                        }
                    }
                    for (j = 0; i < len; i++, j++)
                        ubuf[i] = (jschar) (unsigned char) cbuf[j];
                    ts->userbuf.limit = ubuf + len;
                    ts->userbuf.ptr = ubuf;
                }
                if (ts->listener) {
                    ts->listener(ts->filename, ts->lineno, ts->userbuf.ptr, len,
                                 &ts->listenerTSData, ts->listenerData);
                }

                nl = ts->saveEOL;
                if (!nl) {
                    /*
                     * Any one of \n, \r, or \r\n ends a line (the longest
                     * match wins).  Also allow the Unicode line and paragraph
                     * separators.
                     */
                    for (nl = ts->userbuf.ptr; nl < ts->userbuf.limit; nl++) {
                        /*
                         * Try to prevent value-testing on most characters by
                         * filtering out characters that aren't 000x or 202x.
                         */
                        if ((*nl & 0xDFD0) == 0) {
                            if (*nl == '\n')
                                break;
                            if (*nl == '\r') {
                                if (nl + 1 < ts->userbuf.limit && nl[1] == '\n')
                                    nl++;
                                break;
                            }
                            if (*nl == LINE_SEPARATOR || *nl == PARA_SEPARATOR)
                                break;
                        }
                    }
                }

                /*
                 * If there was a line terminator, copy thru it into linebuf.
                 * Else copy JS_LINE_LIMIT-1 bytes into linebuf.
                 */
                if (nl < ts->userbuf.limit)
                    len = PTRDIFF(nl, ts->userbuf.ptr, jschar) + 1;
                if (len >= JS_LINE_LIMIT) {
                    len = JS_LINE_LIMIT - 1;
                    ts->saveEOL = nl;
                } else {
                    ts->saveEOL = NULL;
                }
                js_strncpy(ts->linebuf.base, ts->userbuf.ptr, len);
                ts->userbuf.ptr += len;
                olen = len;

                /*
                 * Make sure linebuf contains \n for EOL (don't do this in
                 * userbuf because the user's string might be readonly).
                 */
                if (nl < ts->userbuf.limit) {
                    if (*nl == '\r') {
                        if (ts->linebuf.base[len-1] == '\r') {
                            /*
                             * Does the line segment end in \r?  We must check
                             * for a \n at the front of the next segment before
                             * storing a \n into linebuf.  This case matters
                             * only when we're reading from a file.
                             */
                            if (nl + 1 == ts->userbuf.limit && ts->file) {
                                len--;
                                ts->flags |= TSF_CRFLAG; /* clear NLFLAG? */
                                if (len == 0) {
                                    /*
                                     * This can happen when a segment ends in
                                     * \r\r.  Start over.  ptr == limit in this
                                     * case, so we'll fall into buffer-filling
                                     * code.
                                     */
                                    return GetChar(ts);
                                }
                            } else {
                                ts->linebuf.base[len-1] = '\n';
                            }
                        }
                    } else if (*nl == '\n') {
                        if (nl > ts->userbuf.base &&
                            nl[-1] == '\r' &&
                            ts->linebuf.base[len-2] == '\r') {
                            len--;
                            JS_ASSERT(ts->linebuf.base[len] == '\n');
                            ts->linebuf.base[len-1] = '\n';
                        }
                    } else if (*nl == LINE_SEPARATOR || *nl == PARA_SEPARATOR) {
                        ts->linebuf.base[len-1] = '\n';
                    }
                }

                /* Reset linebuf based on adjusted segment length. */
                ts->linebuf.limit = ts->linebuf.base + len;
                ts->linebuf.ptr = ts->linebuf.base;

                /* Update position of linebuf within physical userbuf line. */
                if (!(ts->flags & TSF_NLFLAG))
                    ts->linepos += ts->linelen;
                else
                    ts->linepos = 0;
                if (ts->linebuf.limit[-1] == '\n')
                    ts->flags |= TSF_NLFLAG;
                else
                    ts->flags &= ~TSF_NLFLAG;

                /* Update linelen from original segment length. */
                ts->linelen = olen;
            }
            c = *ts->linebuf.ptr++;
        } while (JS_ISFORMAT(c));
    }
    if (c == '\n')
        ts->lineno++;
    return c;
}

static void
UngetChar(JSTokenStream *ts, int32 c)
{
    if (c == EOF)
        return;
    JS_ASSERT(ts->ungetpos < sizeof ts->ungetbuf / sizeof ts->ungetbuf[0]);
    if (c == '\n')
        ts->lineno--;
    ts->ungetbuf[ts->ungetpos++] = (jschar)c;
}

static int32
PeekChar(JSTokenStream *ts)
{
    int32 c;

    c = GetChar(ts);
    UngetChar(ts, c);
    return c;
}

/*
 * Peek n chars ahead into ts.  Return true if n chars were read, false if
 * there weren't enough characters in the input stream.  This function cannot
 * be used to peek into or past a newline.
 */
static JSBool
PeekChars(JSTokenStream *ts, intN n, jschar *cp)
{
    intN i, j;
    int32 c;

    for (i = 0; i < n; i++) {
        c = GetChar(ts);
        if (c == EOF)
            break;
        if (c == '\n') {
            UngetChar(ts, c);
            break;
        }
        cp[i] = (jschar)c;
    }
    for (j = i - 1; j >= 0; j--)
        UngetChar(ts, cp[j]);
    return i == n;
}

static void
SkipChars(JSTokenStream *ts, intN n)
{
    while (--n >= 0)
        GetChar(ts);
}

static JSBool
MatchChar(JSTokenStream *ts, int32 expect)
{
    int32 c;

    c = GetChar(ts);
    if (c == expect)
        return JS_TRUE;
    UngetChar(ts, c);
    return JS_FALSE;
}

static JSBool
ReportCompileErrorNumber(JSContext *cx, void *handle, uintN flags,
                         uintN errorNumber, JSErrorReport *report,
                         JSBool charArgs, va_list ap)
{
    JSString *linestr = NULL;
    JSTokenStream *ts = NULL;
    JSCodeGenerator *cg = NULL;
#if JS_HAS_XML_SUPPORT
    JSParseNode *pn = NULL;
#endif
    JSErrorReporter onError;
    JSTokenPos *tp;
    JSStackFrame *fp;
    uintN index;
    char *message;
    JSBool warning;

    memset(report, 0, sizeof (struct JSErrorReport));
    report->flags = flags;
    report->errorNumber = errorNumber;
    message = NULL;

    if (!js_ExpandErrorArguments(cx, js_GetErrorMessage, NULL,
                                 errorNumber, &message, report, &warning,
                                 charArgs, ap)) {
        return JS_FALSE;
    }

    js_AddRoot(cx, &linestr, "error line buffer");

    switch (flags & JSREPORT_HANDLE) {
      case JSREPORT_TS:
        ts = handle;
        break;
      case JSREPORT_CG:
        cg = handle;
        break;
#if JS_HAS_XML_SUPPORT
      case JSREPORT_PN:
        pn = handle;
        ts = pn->pn_ts;
        break;
#endif
    }

    JS_ASSERT(!ts || ts->linebuf.limit < ts->linebuf.base + JS_LINE_LIMIT);
    onError = cx->errorReporter;
    if (onError) {
        /*
         * We are typically called with non-null ts and null cg from jsparse.c.
         * We can be called with null ts from the regexp compilation functions.
         * The code generator (jsemit.c) may pass null ts and non-null cg.
         */
        do {
            if (ts) {
                report->filename = ts->filename;
#if JS_HAS_XML_SUPPORT
                if (pn) {
                    report->lineno = pn->pn_pos.begin.lineno;
                    if (report->lineno != ts->lineno)
                        break;
                }
#endif
                report->lineno = ts->lineno;
                linestr = js_NewStringCopyN(cx, ts->linebuf.base,
                                            PTRDIFF(ts->linebuf.limit,
                                                    ts->linebuf.base,
                                                    jschar),
                                            0);
                report->linebuf = linestr
                                 ? JS_GetStringBytes(linestr)
                                 : NULL;
                tp = &ts->tokens[(ts->cursor+ts->lookahead) & NTOKENS_MASK].pos;
#if JS_HAS_XML_SUPPORT
                if (pn)
                    tp = &pn->pn_pos;
#endif
                /*
                 * FIXME: What should instead happen here is that we should
                 * find error-tokens in userbuf, if !ts->file.  That will
                 * allow us to deliver a more helpful error message, which
                 * includes all or part of the bad string or bad token.  The
                 * code here yields something that looks truncated.
                 * See https://bugzilla.mozilla.org/show_bug.cgi?id=352970
                 */ 
                index = 0;
                if (tp->begin.lineno == tp->end.lineno) {
                    if (tp->begin.index < ts->linepos)
                        break;

                    index = tp->begin.index - ts->linepos;
                }

                report->tokenptr = linestr ? report->linebuf + index : NULL;
                report->uclinebuf = linestr ? JS_GetStringChars(linestr) : NULL;
                report->uctokenptr = linestr ? report->uclinebuf + index : NULL;
                break;
            }

            if (cg) {
                report->filename = cg->filename;
                report->lineno = CG_CURRENT_LINE(cg);
                break;
            }

            /*
             * If we can't find out where the error was based on the current frame,
             * see if the next frame has a script/pc combo we can use.
             */
            for (fp = cx->fp; fp; fp = fp->down) {
                if (fp->script && fp->pc) {
                    report->filename = fp->script->filename;
                    report->lineno = js_PCToLineNumber(cx, fp->script, fp->pc);
                    break;
                }
            }
        } while (0);

#if JS_HAS_ERROR_EXCEPTIONS
        /*
         * If there's a runtime exception type associated with this error
         * number, set that as the pending exception.  For errors occuring at
         * compile time, this is very likely to be a JSEXN_SYNTAXERR.
         *
         * If an exception is thrown but not caught, the JSREPORT_EXCEPTION
         * flag will be set in report.flags.  Proper behavior for an error
         * reporter is to ignore a report with this flag for all but top-level
         * compilation errors.  The exception will remain pending, and so long
         * as the non-top-level "load", "eval", or "compile" native function
         * returns false, the top-level reporter will eventually receive the
         * uncaught exception report.
         *
         * XXX it'd probably be best if there was only one call to this
         * function, but there seem to be two error reporter call points.
         */

        /*
         * Try to raise an exception only if there isn't one already set --
         * otherwise the exception will describe the last compile-time error,
         * which is likely spurious.
         */
        if (!ts || !(ts->flags & TSF_ERROR)) {
            if (js_ErrorToException(cx, message, report))
                onError = NULL;
        }

        /*
         * Suppress any compile-time errors that don't occur at the top level.
         * This may still fail, as interplevel may be zero in contexts where we
         * don't really want to call the error reporter, as when js is called
         * by other code which could catch the error.
         */
        if (cx->interpLevel != 0 && !JSREPORT_IS_WARNING(flags))
            onError = NULL;
#endif
        if (onError) {
            JSDebugErrorHook hook = cx->runtime->debugErrorHook;

            /*
             * If debugErrorHook is present then we give it a chance to veto
             * sending the error on to the regular error reporter.
             */
            if (hook && !hook(cx, message, report,
                              cx->runtime->debugErrorHookData)) {
                onError = NULL;
            }
        }
        if (onError)
            (*onError)(cx, message, report);
    }

    if (message)
        JS_free(cx, message);
    if (report->ucmessage)
        JS_free(cx, (void *)report->ucmessage);

    js_RemoveRoot(cx->runtime, &linestr);

    if (ts && !JSREPORT_IS_WARNING(flags)) {
        /* Set the error flag to suppress spurious reports. */
        ts->flags |= TSF_ERROR;
    }

    return warning;
}

JSBool
js_ReportCompileErrorNumber(JSContext *cx, void *handle, uintN flags,
                            uintN errorNumber, ...)
{
    va_list ap;
    JSErrorReport report;
    JSBool warning;

    if ((flags & JSREPORT_STRICT) && !JS_HAS_STRICT_OPTION(cx))
        return JS_TRUE;

    va_start(ap, errorNumber);
    warning = ReportCompileErrorNumber(cx, handle, flags, errorNumber,
                                       &report, JS_TRUE, ap);
    va_end(ap);

    /* 
     * We have to do this here because js_ReportCompileErrorNumberUC doesn't
     * need to do this.
     */
    if (report.messageArgs) {
        int i = 0;
        while (report.messageArgs[i])
            JS_free(cx, (void *)report.messageArgs[i++]);
        JS_free(cx, (void *)report.messageArgs);
    }

    return warning;
}

JSBool
js_ReportCompileErrorNumberUC(JSContext *cx, void *handle, uintN flags,
                              uintN errorNumber, ...)
{
    va_list ap;
    JSErrorReport report;
    JSBool warning;

    if ((flags & JSREPORT_STRICT) && !JS_HAS_STRICT_OPTION(cx))
        return JS_TRUE;
 
    va_start(ap, errorNumber);
    warning = ReportCompileErrorNumber(cx, handle, flags, errorNumber,
                                       &report, JS_FALSE, ap);
    va_end(ap);

    if (report.messageArgs)
        JS_free(cx, (void *)report.messageArgs);

    return warning;
}

static JSBool
GrowStringBuffer(JSStringBuffer *sb, size_t newlength)
{
    ptrdiff_t offset;
    jschar *bp;

    offset = PTRDIFF(sb->ptr, sb->base, jschar);
    JS_ASSERT(offset >= 0);
    newlength += offset + 1;
    if ((size_t)offset < newlength && newlength < ~(size_t)0 / sizeof(jschar))
        bp = realloc(sb->base, newlength * sizeof(jschar));
    else
        bp = NULL;
    if (!bp) {
        free(sb->base);
        sb->base = STRING_BUFFER_ERROR_BASE;
        return JS_FALSE;
    }
    sb->base = bp;
    sb->ptr = bp + offset;
    sb->limit = bp + newlength - 1;
    return JS_TRUE;
}

static void
FreeStringBuffer(JSStringBuffer *sb)
{
    JS_ASSERT(STRING_BUFFER_OK(sb));
    if (sb->base)
        free(sb->base);
}

void
js_InitStringBuffer(JSStringBuffer *sb)
{
    sb->base = sb->limit = sb->ptr = NULL;
    sb->data = NULL;
    sb->grow = GrowStringBuffer;
    sb->free = FreeStringBuffer;
}

void
js_FinishStringBuffer(JSStringBuffer *sb)
{
    sb->free(sb);
}

#define ENSURE_STRING_BUFFER(sb,n) \
    ((sb)->ptr + (n) <= (sb)->limit || sb->grow(sb, n))

static void
FastAppendChar(JSStringBuffer *sb, jschar c)
{
    if (!STRING_BUFFER_OK(sb))
        return;
    if (!ENSURE_STRING_BUFFER(sb, 1))
        return;
    *sb->ptr++ = c;
}

void
js_AppendChar(JSStringBuffer *sb, jschar c)
{
    jschar *bp;

    if (!STRING_BUFFER_OK(sb))
        return;
    if (!ENSURE_STRING_BUFFER(sb, 1))
        return;
    bp = sb->ptr;
    *bp++ = c;
    *bp = 0;
    sb->ptr = bp;
}

#if JS_HAS_XML_SUPPORT

void
js_RepeatChar(JSStringBuffer *sb, jschar c, uintN count)
{
    jschar *bp;

    if (!STRING_BUFFER_OK(sb) || count == 0)
        return;
    if (!ENSURE_STRING_BUFFER(sb, count))
        return;
    for (bp = sb->ptr; count; --count)
        *bp++ = c;
    *bp = 0;
    sb->ptr = bp;
}

void
js_AppendCString(JSStringBuffer *sb, const char *asciiz)
{
    size_t length;
    jschar *bp;

    if (!STRING_BUFFER_OK(sb) || *asciiz == '\0')
        return;
    length = strlen(asciiz);
    if (!ENSURE_STRING_BUFFER(sb, length))
        return;
    for (bp = sb->ptr; length; --length)
        *bp++ = (jschar) *asciiz++;
    *bp = 0;
    sb->ptr = bp;
}

void
js_AppendJSString(JSStringBuffer *sb, JSString *str)
{
    size_t length;
    jschar *bp;

    if (!STRING_BUFFER_OK(sb))
        return;
    length = JSSTRING_LENGTH(str);
    if (length == 0 || !ENSURE_STRING_BUFFER(sb, length))
        return;
    bp = sb->ptr;
    js_strncpy(bp, JSSTRING_CHARS(str), length);
    bp += length;
    *bp = 0;
    sb->ptr = bp;
}

static JSBool
GetXMLEntity(JSContext *cx, JSTokenStream *ts)
{
    ptrdiff_t offset, length, i;
    int32 c, d;
    JSBool ispair;
    jschar *bp, digit;
    char *bytes;
    JSErrNum msg;

    /* Put the entity, including the '&' already scanned, in ts->tokenbuf. */
    offset = PTRDIFF(ts->tokenbuf.ptr, ts->tokenbuf.base, jschar);
    FastAppendChar(&ts->tokenbuf, '&');
    while ((c = GetChar(ts)) != ';') {
        if (c == EOF || c == '\n') {
            js_ReportCompileErrorNumber(cx, ts,
                                        JSREPORT_TS | JSREPORT_ERROR,
                                        JSMSG_END_OF_XML_ENTITY);
            return JS_FALSE;
        }
        FastAppendChar(&ts->tokenbuf, (jschar) c);
    }

    /* Let length be the number of jschars after the '&', including the ';'. */
    length = PTRDIFF(ts->tokenbuf.ptr, ts->tokenbuf.base, jschar) - offset;
    bp = ts->tokenbuf.base + offset;
    c = d = 0;
    ispair = JS_FALSE;
    if (length > 2 && bp[1] == '#') {
        /* Match a well-formed XML Character Reference. */
        i = 2;
        if (length > 3 && JS_TOLOWER(bp[i]) == 'x') {
            if (length > 9)     /* at most 6 hex digits allowed */
                goto badncr;
            while (++i < length) {
                di