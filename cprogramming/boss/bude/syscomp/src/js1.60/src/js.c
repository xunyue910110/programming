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
 * JS shell.
 */
#include "jsstddef.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h"
#include "jsutil.h"
#include "jsprf.h"
#include "jsapi.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsdbgapi.h"
#include "jsemit.h"
#include "jsfun.h"
#include "jsgc.h"
#include "jslock.h"
#include "jsobj.h"
#include "jsparse.h"
#include "jsscope.h"
#include "jsscript.h"

#ifdef PERLCONNECT
#include "perlconnect/jsperl.h"
#endif

#ifdef LIVECONNECT
#include "jsjava.h"
#endif

#ifdef JSDEBUGGER
#include "jsdebug.h"
#ifdef JSDEBUGGER_JAVA_UI
#include "jsdjava.h"
#endif /* JSDEBUGGER_JAVA_UI */
#ifdef JSDEBUGGER_C_UI
#include "jsdb.h"
#endif /* JSDEBUGGER_C_UI */
#endif /* JSDEBUGGER */

#ifdef XP_UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#if defined(XP_WIN) || defined(XP_OS2)
#include <io.h>     /* for isatty() */
#endif

typedef enum JSShellExitCode {
    EXITCODE_RUNTIME_ERROR      = 3,
    EXITCODE_FILE_NOT_FOUND     = 4,
    EXITCODE_OUT_OF_MEMORY      = 5
} JSShellExitCode;

size_t gStackChunkSize = 8192;
static size_t gMaxStackSize = 0;
static jsuword gStackBase;
int gExitCode = 0;
JSBool gQuitting = JS_FALSE;
FILE *gErrFile = NULL;
FILE *gOutFile = NULL;

#ifdef JSDEBUGGER
static JSDContext *_jsdc;
#ifdef JSDEBUGGER_JAVA_UI
static JSDJContext *_jsdjc;
#endif /* JSDEBUGGER_JAVA_UI */
#endif /* JSDEBUGGER */

static JSBool reportWarnings = JS_TRUE;
static JSBool compileOnly = JS_FALSE;

typedef enum JSShellErrNum {
#define MSG_DEF(name, number, count, exception, format) \
    name = number,
#include "jsshell.msg"
#undef MSG_DEF
    JSShellErr_Limit
#undef MSGDEF
} JSShellErrNum;

static const JSErrorFormatString *
my_GetErrorMessage(void *userRef, const char *locale, const uintN errorNumber);

#ifdef EDITLINE
extern char     *readline(const char *prompt);
extern void     add_history(char *line);
#endif

static JSBool
GetLine(JSContext *cx, char *bufp, FILE *file, const char *prompt) {
#ifdef EDITLINE
    /*
     * Use readline only if file is stdin, because there's no way to specify
     * another handle.  Are other filehandles interactive?
     */
    if (file == stdin) {
        char *linep = readline(prompt);
        if (!linep)
            return JS_FALSE;
        if (linep[0] != '\0')
            add_history(linep);
        strcpy(bufp, linep);
        JS_free(cx, linep);
        bufp += strlen(bufp);
        *bufp++ = '\n';
        *bufp = '\0';
    } else
#endif
    {
        char line[256];
        fprintf(gOutFile, prompt);
        fflush(gOutFile);
        if (!fgets(line, sizeof line, file))
            return JS_FALSE;
        strcpy(bufp, line);
    }
    return JS_TRUE;
}

static void
Process(JSContext *cx, JSObject *obj, char *filename)
{
    JSBool ok, hitEOF;
    JSScript *script;
    jsval result;
    JSString *str;
    char buffer[4096];
    char *bufp;
    int lineno;
    int startline;
    FILE *file;
    jsuword stackLimit;

    if (!filename || strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (!file) {
            JS_ReportErrorNumber(cx, my_GetErrorMessage, NULL,
                                 JSSMSG_CANT_OPEN, filename, strerror(errno));
            gExitCode = EXITCODE_FILE_NOT_FOUND;
            return;
        }
    }

    if (gMaxStackSize == 0) {
        /*
         * Disable checking for stack overflow if limit is zero.
         */
        stackLimit = 0;
    } else {
#if JS_STACK_GROWTH_DIRECTION > 0
        stackLimit = gStackBase + gMaxStackSize;
#else
        stackLimit = gStackBase - gMaxStackSize;
#endif
    }
    JS_SetThreadStackLimit(cx, stackLimit);

    if (!isatty(fileno(file))) {
        /*
         * It's not interactive - just execute it.
         *
         * Support the UNIX #! shell hack; gobble the first line if it starts
         * with '#'.  TODO - this isn't quite compatible with sharp variables,
         * as a legal js program (using sharp variables) might start with '#'.
         * But that would require multi-character lookahead.
         */
        int ch = fgetc(file);
        if (ch == '#') {
            while((ch = fgetc(file)) != EOF) {
                if (ch == '\n' || ch == '\r')
                    break;
            }
        }
        ungetc(ch, file);
        script = JS_CompileFileHandle(cx, obj, filename, file);
        if (script) {
            if (!compileOnly)
                (void)JS_ExecuteScript(cx, obj, script, &result);
            JS_DestroyScript(cx, script);
        }
        return;
    }

    /* It's an interactive filehandle; drop into read-eval-print loop. */
    lineno = 1;
    hitEOF = JS_FALSE;
    do {
        bufp = buffer;
        *bufp = '\0';

        /*
         * Accumulate lines until we get a 'compilable unit' - one that either
         * generates an error (before running out of source) or that compiles
         * cleanly.  This should be whenever we get a complete statement that
         * coincides with the end of a line.
         */
        startline = lineno;
        do {
            if (!GetLine(cx, bufp, file, startline == lineno ? "js> " : "")) {
                hitEOF = JS_TRUE;
                break;
            }
            bufp += strlen(bufp);
            lineno++;
        } while (!JS_BufferIsCompilableUnit(cx, obj, buffer, strlen(buffer)));

        /* Clear any pending exception from previous failed compiles.  */
        JS_ClearPendingException(cx);
        script = JS_CompileScript(cx, obj, buffer, strlen(buffer), "typein",
                                  startline);
        if (script) {
            if (!compileOnly) {
                ok = JS_ExecuteScript(cx, obj, script, &result);
                if (ok && result != JSVAL_VOID) {
                    str = JS_ValueToString(cx, result);
                    if (str)
                        fprintf(gOutFile, "%s\n", JS_GetStringBytes(str));
                    else
                        ok = JS_FALSE;
                }
            }
            JS_DestroyScript(cx, script);
        }
    } while (!hitEOF && !gQuitting);
    fprintf(gOutFile, "\n");
    return;
}

static int
usage(void)
{
    fprintf(gErrFile, "%s\n", JS_GetImplementationVersion());
    fprintf(gErrFile, "usage: js [-PswWxC] [-b branchlimit] [-c stackchunksize] [-v version] [-f scriptfile] [-e script] [-S maxstacksize] [scriptfile] [scriptarg...]\n");
    return 2;
}

static uint32 gBranchCount;
static uint32 gBranchLimit;

static JSBool
my_BranchCallback(JSContext *cx, JSScript *script)
{
    if (++gBranchCount == gBranchLimit) {
        if (script) {
            if (script->filename)
                fprintf(gErrFile, "%s:", script->filename);
            fprintf(gErrFile, "%u: script branch callback (%u callbacks)\n",
                    script->lineno, gBranchLimit);
        } else {
            fprintf(gErrFile, "native branch callback (%u callbacks)\n",
                    gBranchLimit);
        }
        gBranchCount = 0;
        return JS_FALSE;
    }
    if ((gBranchCount & 0x3fff) == 1)
        JS_MaybeGC(cx);
    return JS_TRUE;
}

extern JSClass global_class;

static int
ProcessArgs(JSContext *cx, JSObject *obj, char **argv, int argc)
{
    int i, j, length;
    JSObject *argsObj;
    char *filename = NULL;
    JSBool isInteractive = JS_TRUE;

    /*
     * Scan past all optional arguments so we can create the arguments object
     * before processing any -f options, which must interleave properly with
     * -v and -w options.  This requires two passes, and without getopt, we'll
     * have to keep the option logic here and in the second for loop in sync.
     */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] == '\0') {
            ++i;
            break;
        }
        switch (argv[i][1]) {
          case 'b':
          case 'c':
          case 'f':
          case 'e':
          case 'v':
          case 'S':
            ++i;
            break;
          default:;
        }
    }

    /*
     * Create arguments early and define it to root it, so it's safe from any
     * GC calls nested below, and so it is available to -f <file> arguments.
     */
    argsObj = JS_NewArrayObject(cx, 0, NULL);
    if (!argsObj)
        return 1;
    if (!JS_DefineProperty(cx, obj, "arguments", OBJECT_TO_JSVAL(argsObj),
                           NULL, NULL, 0)) {
        return 1;
    }

    length = argc - i;
    for (j = 0; j < length; j++) {
        JSString *str = JS_NewStringCopyZ(cx, argv[i++]);
        if (!str)
            return 1;
        if (!JS_DefineElement(cx, argsObj, j, STRING_TO_JSVAL(str),
                              NULL, NULL, JSPROP_ENUMERATE)) {
            return 1;
        }
    }

    for (i = 0; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] == '\0') {
            filename = argv[i++];
            isInteractive = JS_FALSE;
            break;
        }

        switch (argv[i][1]) {
        case 'v':
            if (++i == argc) {
                return usage();
            }
            JS_SetVersion(cx, (JSVersion) atoi(argv[i]));
            break;

        case 'w':
            reportWarnings = JS_TRUE;
            break;

        case 'W':
            reportWarnings = JS_FALSE;
            break;

        case 's':
            JS_ToggleOptions(cx, JSOPTION_STRICT);
            break;

        case 'x':
            JS_ToggleOptions(cx, JSOPTION_XML);
            break;

        case 'P':
            if (JS_GET_CLASS(cx, JS_GetPrototype(cx, obj)) != &global_class) {
                JSObject *gobj;

                if (!JS_SealObject(cx, obj, JS_TRUE))
                    return JS_FALSE;
                gobj = JS_NewObject(cx, &global_class, NULL, NULL);
                if (!gobj)
                    return JS_FALSE;
                if (!JS_SetPrototype(cx, gobj, obj))
                    return JS_FALSE;
                JS_SetParent(cx, gobj, NULL);
                JS_SetGlobalObject(cx, gobj);
                obj = gobj;
            }
            break;

        case 'b':
            gBranchLimit = atoi(argv[++i]);
            JS_SetBranchCallback(cx, my_BranchCallback);
            JS_ToggleOptions(cx, JSOPTION_NATIVE_BRANCH_CALLBACK);
            break;

        case 'c':
            /* set stack chunk size */
            gStackChunkSize = atoi(argv[++i]);
            break;

        case 'f':
            if (++i == argc) {
                return usage();
            }
            Process(cx, obj, argv[i]);
            /*
             * XXX: js -f foo.js should interpret foo.js and then
             * drop into interactive mode, but that breaks the test
             * harness. Just execute foo.js for now.
             */
            isInteractive = JS_FALSE;
            break;

        case 'e':
        {
            jsval rval;

            if (++i == argc) {
                return usage();
            }

            /* Pass a filename of -e to imitate PERL */
            JS_EvaluateScript(cx, obj, argv[i], strlen(argv[i]),
                              "-e", 1, &rval);

            isInteractive = JS_FALSE;
            break;

        }
        case 'C':
            compileOnly = JS_TRUE;
            isInteractive = JS_FALSE;
            break;

        case 'S':
            if (++i == argc) {
                return usage();
            }
            /* Set maximum stack size. */
            gMaxStackSize = atoi(argv[i]);
            break;

        default:
            return usage();
        }
    }

    if (filename || isInteractive)
        Process(cx, obj, filename);
    return gExitCode;
}


static JSBool
Version(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc > 0 && JSVAL_IS_INT(argv[0]))
        *rval = INT_TO_JSVAL(JS_SetVersion(cx, (JSVersion) JSVAL_TO_INT(argv[0])));
    else
        *rval = INT_TO_JSVAL(JS_GetVersion(cx));
    return JS_TRUE;
}

static struct {
    const char  *name;
    uint32      flag;
} js_options[] = {
    {"strict",          JSOPTION_STRICT},
    {"werror",          JSOPTION_WERROR},
    {"atline",          JSOPTION_ATLINE},
    {"xml",             JSOPTION_XML},
    {0,                 0}
};

static JSBool
Options(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 optset, flag;
    uintN i, j, found;
    JSString *str;
    const char *opt;
    char *names;

    optset = 0;
    for (i = 0; i < argc; i++) {
        str = JS_ValueToString(cx, argv[i]);
        if (!str)
            return JS_FALSE;
        opt = JS_GetStringBytes(str);
        for (j = 0; js_options[j].name; j++) {
            if (strcmp(js_options[j].name, opt) == 0) {
                optset |= js_options[j].flag;
                break;
            }
        }
    }
    optset = JS_ToggleOptions(cx, optset);

    names = NULL;
    found = 0;
    while (optset != 0) {
        flag = optset;
        optset &= optset - 1;
        flag &= ~optset;
        for (j = 0; js_options[j].name; j++) {
            if (js_options[j].flag == flag) {
                names = JS_sprintf_append(names, "%s%s",
                                          names ? "," : "", js_options[j].name);
                found++;
                break;
            }
        }
    }
    if (!found)
        names = strdup("");
    if (!names) {
        JS_ReportOutOfMemory(cx);
        return JS_FALSE;
    }

    str = JS_NewString(cx, names, strlen(names));
    if (!str) {
        free(names);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static void
my_LoadErrorReporter(JSContext *cx, const char *message, JSErrorReport *report);

static void
my_ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report);

static JSBool
Load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i;
    JSString *str;
    const char *filename;
    JSScript *script;
    JSBool ok;
    jsval result;
    JSErrorReporter older;
    uint32 oldopts;

    for (i = 0; i < argc; i++) {
        str = JS_ValueToString(cx, argv[i]);
        if (!str)
            return JS_FALSE;
        argv[i] = STRING_TO_JSVAL(str);
        filename = JS_GetStringBytes(str);
        errno = 0;
        older = JS_SetErrorReporter(cx, my_LoadErrorReporter);
        oldopts = JS_GetOptions(cx);
        JS_SetOptions(cx, oldopts | JSOPTION_COMPILE_N_GO);
        script = JS_CompileFile(cx, obj, filename);
        if (!script) {
            ok = JS_FALSE;
        } else {
            ok = !compileOnly
                 ? JS_ExecuteScript(cx, obj, script, &result)
                 : JS_TRUE;
            JS_DestroyScript(cx, script);
        }
        JS_SetOptions(cx, oldopts);
        JS_SetErrorReporter(cx, older);
        if (!ok)
            return JS_FALSE;
    }

    return JS_TRUE;
}

/*
 * function readline()
 * Provides a hook for scripts to read a line from stdin.
 */
static JSBool
ReadLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
#define BUFSIZE 256
    FILE *from;
    char *buf, *tmp;
    size_t bufsize, buflength, gotlength;
    JSString *str;

    from = stdin;
    buflength = 0;
    bufsize = BUFSIZE;
    buf = JS_malloc(cx, bufsize);
    if (!buf)
        return JS_FALSE;

    while ((gotlength = 
            js_fgets(buf + buflength, bufsize - buflength, from)) > 0) {
        buflength += gotlength;

        /* Are we done? */
        if (buf[buflength - 1] == '\n') {
            buf[buflength - 1] = '\0';
            break;
        }

        /* Else, grow our buffer for another pass. */
        tmp = JS_realloc(cx, buf, bufsize * 2);
        if (!tmp) {
            JS_free(cx, buf);
            return JS_FALSE;
        }

        bufsize *= 2;
        buf = tmp;
    }

    /* Treat the empty string specially. */
    if (buflength == 0) {
        *rval = JS_GetEmptyStringValue(cx);
        JS_free(cx, buf);
        return JS_TRUE;
    }

    /* Shrink the buffer to the real size. */
    tmp = JS_realloc(cx, buf, buflength);
    if (!tmp) {
        JS_free(cx, buf);
        return JS_FALSE;
    }

    buf = tmp;

    /* 
     * Turn buf into a JSString. Note that buflength includes the trailing null
     * character.
     */
    str = JS_NewString(cx, buf, buflength - 1);
    if (!str) {
        JS_free(cx, buf);
        return JS_FALSE;
    }

    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static JSBool
Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i, n;
    JSString *str;

    for (i = n = 0; i < argc; i++) {
        str = JS_ValueToString(cx, argv[i]);
        if (!str)
            return JS_FALSE;
        fprintf(gOutFile, "%s%s", i ? " " : "", JS_GetStringBytes(str));
    }
    n++;
    if (n)
        fputc('\n', gOutFile);
    return JS_TRUE;
}

static JSBool
Help(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSBool
Quit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
#ifdef LIVECONNECT
    JSJ_SimpleShutdown();
#endif

    JS_ConvertArguments(cx, argc, argv,"/ i", &gExitCode);

    gQuitting = JS_TRUE;
    return JS_FALSE;
}

static JSBool
GC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSRuntime *rt;
    uint32 preBytes;

    rt = cx->runtime;
    preBytes = rt->gcBytes;
#ifdef GC_MARK_DEBUG
    if (argc && JSVAL_IS_STRING(argv[0])) {
        char *name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
        FILE *file = fopen(name, "w");
        if (!file) {
            fprintf(gErrFile, "gc: can't open %s: %s\n", strerror(errno));
            return JS_FALSE;
        }
        js_DumpGCHeap = file;
    } else {
        js_DumpGCHeap = stdout;
    }
#endif
    JS_GC(cx);
#ifdef GC_MARK_DEBUG
    if (js_DumpGCHeap != stdout)
        fclose(js_DumpGCHeap);
    js_DumpGCHeap = NULL;
#endif
    fprintf(gOutFile, "before %lu, after %lu, break %08lx\n",
            (unsigned long)preBytes, (unsigned long)rt->gcBytes,
#ifdef XP_UNIX
            (unsigned long)sbrk(0)
#else
            0
#endif
            );
#ifdef JS_GCMETER
    js_DumpGCStats(rt, stdout);
#endif
    return JS_TRUE;
}

static JSScript *
ValueToScript(JSContext *cx, jsval v)
{
    JSScript *script;
    JSFunction *fun;

    if (!JSVAL_IS_PRIMITIVE(v) &&
        JS_GET_CLASS(cx, JSVAL_TO_OBJECT(v)) == &js_ScriptClass) {
        script = (JSScript *) JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
    } else {
        fun = JS_ValueToFunction(cx, v);
        if (!fun)
            return NULL;
        script = FUN_SCRIPT(fun);
    }
    return script;
}

static JSBool
GetTrapArgs(JSContext *cx, uintN argc, jsval *argv, JSScript **scriptp,
            int32 *ip)
{
    jsval v;
    uintN intarg;
    JSScript *script;

    *scriptp = cx->fp->down->script;
    *ip = 0;
    if (argc != 0) {
        v = argv[0];
        intarg = 0;
        if (!JSVAL_IS_PRIMITIVE(v) &&
            (JS_GET_CLASS(cx, JSVAL_TO_OBJECT(v)) == &js_FunctionClass ||
             JS_GET_CLASS(cx, JSVAL_TO_OBJECT(v)) == &js_ScriptClass)) {
            script = ValueToScript(cx, v);
            if (!script)
                return JS_FALSE;
            *scriptp = script;
            intarg++;
        }
        if (argc > intarg) {
            if (!JS_ValueToInt32(cx, argv[intarg], ip))
                return JS_FALSE;
        }
    }
    return JS_TRUE;
}

static JSTrapStatus
TrapHandler(JSContext *cx, JSScript *script, jsbytecode *pc, jsval *rval,
            void *closure)
{
    JSString *str;
    JSStackFrame *caller;

    str = (JSString *) closure;
    caller = JS_GetScriptedCaller(cx, NULL);
    if (!JS_EvaluateScript(cx, caller->scopeChain,
                           JS_GetStringBytes(str), JS_GetStringLength(str),
                           caller->script->filename, caller->script->lineno,
                           rval)) {
        return JSTRAP_ERROR;
    }
    if (*rval != JSVAL_VOID)
        return JSTRAP_RETURN;
    return JSTRAP_CONTINUE;
}

static JSBool
Trap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
    JSScript *script;
    int32 i;

    if (argc == 0) {
        JS_ReportErrorNumber(cx, my_GetErrorMessage, NULL, JSSMSG_TRAP_USAGE);
        return JS_FALSE;
    }
    argc--;
    str = JS_ValueToString(cx, argv[argc]);
    if (!str)
        return JS_FALSE;
    argv[argc] = STRING_TO_JSVAL(str);
    if (!GetTrapArgs(cx, argc, argv, &script, &i))
        return JS_FALSE;
    return JS_SetTrap(cx, script, script->code + i, TrapHandler, str);
}

static JSBool
Untrap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSScript *script;
    int32 i;

    if (!GetTrapArgs(cx, argc, argv, &script, &i))
        return JS_FALSE;
    JS_ClearTrap(cx, script, script->code + i, NULL, NULL);
    return JS_TRUE;
}

static JSBool
LineToPC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSScript *script;
    int32 i;
    uintN lineno;
    jsbytecode *pc;

    if (argc == 0) {
        JS_ReportErrorNumber(cx, my_GetErrorMessage, NULL, JSSMSG_LINE2PC_USAGE);
        return JS_FALSE;
    }
    script = cx->fp->down->script;
    if (!GetTrapArgs(cx, argc, argv, &script, &i))
        return JS_FALSE;
    lineno = (i == 0) ? script->lineno : (uintN)i;
    pc = JS_LineNumberToPC(cx, script, lineno);
    if (!pc)
        return JS_FALSE;
    *rval = INT_TO_JSVAL(PTRDIFF(pc, script->code, jsbytecode));
    return JS_TRUE;
}

static JSBool
PCToLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSScript *script;
    int32 i;
    uintN lineno;

    if (!GetTrapArgs(cx, argc, argv, &script, &i))
        return JS_FALSE;
    lineno = JS_PCToLineNumber(cx, script, script->code + i);
    if (!lineno)
        return JS_FALSE;
    *rval = INT_TO_JSVAL(lineno);
    return JS_TRUE;
}

#ifdef DEBUG

static void
SrcNotes(JSContext *cx, JSScript *script)
{
    uintN offset, delta, caseOff;
    jssrcnote *notes, *sn;
    JSSrcNoteType type;
    jsatomid atomIndex;
    JSAtom *atom;

    fprintf(gOutFile, "\nSource notes:\n");
    offset = 0;
    notes = SCRIPT_NOTES(script);
    for (sn = notes; !SN_IS_TERMINATOR(sn); sn = SN_NEXT(sn)) {
        delta = SN_DELTA(sn);
        offset += delta;
        fprintf(gOutFile, "%3u: %5u [%4u] %-8s",
                PTRDIFF(sn, notes, jssrcnote), offset, delta,
                js_SrcNoteSpec[SN_TYPE(sn)].name);
        type = (JSSrcNoteType) SN_TYPE(sn);
        switch (type) {
          case SRC_SETLINE:
            fprintf(gOutFile, " lineno %u", (uintN) js_GetSrcNoteOffset(sn, 0));
            break;
          case SRC_FOR:
            fprintf(gOutFile, " cond %u update %u tail %u",
                   (uintN) js_GetSrcNoteOffset(sn, 0),
                   (uintN) js_GetSrcNoteOffset(sn, 1),
                   (uintN) js_GetSrcNoteOffset(sn, 2));
            break;
          case SRC_COND:
          case SRC_IF_ELSE:
          case SRC_WHILE:
          case SRC_PCBASE:
          case SRC_PCDELTA:
            fprintf(gOutFile, " offset %u", (uintN) js_GetSrcNoteOffset(sn, 0));
            break;
          case SRC_LABEL:
          case SRC_LABELBRACE:
          case SRC_BREAK2LABEL:
          case SRC_CONT2LABEL:
          case SRC_FUNCDEF: {
            const char *bytes;
            JSFunction *fun;
            JSString *str;

            atomIndex = (jsatomid) js_GetSrcNoteOffset(sn, 0);
            atom = js_GetAtom(cx, &script->atomMap, atomIndex);
            if (type != SRC_FUNCDEF) {
                bytes = js_AtomToPrintableString(cx, atom);
            } else {
                fun = (JSFunction *)
                    JS_GetPrivate(cx, ATOM_TO_OBJECT(atom));
                str = JS_DecompileFunction(cx, fun, JS_DONT_PRETTY_PRINT);
                bytes = str ? JS_GetStringBytes(str) : "N/A";
            }
            fprintf(gOutFile, " atom %u (%s)", (uintN)atomIndex, bytes);
            break;
          }
          case SRC_SWITCH:
            fprintf(gOutFile, " length %u", (uintN) js_GetSrcNoteOffset(sn, 0));
            caseOff = (uintN) js_GetSrcNoteOffset(sn, 1);
            if (caseOff)
                fprintf(gOutFile, " first case offset %u", caseOff);
            break;
          case SRC_CATCH:
            delta = (uintN) js_GetSrcNoteOffset(sn, 0);
            if (delta)
                fprintf(gOutFile, " guard size %u", delta);
            break;
          default:;
        }
        fputc('\n', gOutFile);
    }
}

static JSBool
Notes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i;
    JSScript *script;

    for (i = 0; i < argc; i++) {
        script = ValueToScript(cx, argv[i]);
        if (!script)
            continue;

        SrcNotes(cx, script);
    }
    return JS_TRUE;
}

static JSBool
TryNotes(JSContext *cx, JSScript *script)
{
    JSTryNote *tn = script->trynotes;

    if (!tn)
        return JS_TRUE;
    fprintf(gOutFile, "\nException table:\nstart\tend\tcatch\n");
    while (tn->start && tn->catchStart) {
        fprintf(gOutFile, "  %d\t%d\t%d\n",
               tn->start, tn->start + tn->length, tn->catchStart);
        tn++;
    }
    return JS_TRUE;
}

static JSBool
Disassemble(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSBool lines;
    uintN i;
    JSScript *script;

    if (argc > 0 &&
        JSVAL_IS_STRING(argv[0]) &&
        !strcmp(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])), "-l")) {
        lines = JS_TRUE;
        argv++, argc--;
    } else {
        lines = JS_FALSE;
    }
    for (i = 0; i < argc; i++) {
        script = ValueToScript(cx, argv[i]);
        if (!script)
            continue;

        if (JSVAL_IS_FUNCTION(cx, argv[i])) {
            JSFunction *fun = JS_ValueToFunction(cx, argv[i]);
            if (fun && (fun->flags & JSFUN_FLAGS_MASK)) {
                uint8 flags = fun->flags;
                fputs("flags:", stdout);

#define SHOW_FLAG(flag) if (flags & JSFUN_##flag) fputs(" " #flag, stdout);

                SHOW_FLAG(LAMBDA);
                SHOW_FLAG(SETTER);
                SHOW_FLAG(GETTER);
                SHOW_FLAG(BOUND_METHOD);
                SHOW_FLAG(HEAVYWEIGHT);

#undef SHOW_FLAG
                putchar('\n');
            }
        }

        if (!js_Disassemble(cx, script, lines, stdout))
            return JS_FALSE;
        SrcNotes(cx, script);
        TryNotes(cx, script);
    }
    return JS_TRUE;
}

static JSBool
DisassWithSrc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
#define LINE_BUF_LEN 512
    uintN i, len, line1, line2, bupline;
    JSScript *script;
    FILE *file;
    char linebuf[LINE_BUF_LEN];
    jsbytecode *pc, *end;
    static char sep[] = ";-------------------------";

    for (i = 0; i < argc; i++) {
        script = ValueToScript(cx, argv[i]);
        if (!script)
            continue;

        if (!script || !script->filename) {
            JS_ReportErrorNumber(cx, my_GetErrorMessage, NULL,
                                            JSSMSG_FILE_SCRIPTS_ONLY);
            return JS_FALSE;
        }

        file = fopen(script->filename, "r");
        if (!file) {
            JS_ReportErrorNumber(cx, my_GetErrorMessage, NULL,
                            JSSMSG_CANT_OPEN,
                            script->filename, strerror(errno));
            return JS_FALSE;
        }

        pc = script->code;
        end = pc + script->length;

        /* burn the leading lines */
        line2 = JS_PCToLineNumber(cx, script, pc);
        for (line1 = 0; line1 < line2 - 1; line1++)
            fgets(linebuf, LINE_BUF_LEN, file);

        bupline = 0;
        while (pc < end) {
            line2 = JS_PCToLineNumber(cx, script, pc);

            if (line2 < line1) {
                if (bupline != line2) {
                    bupline = line2;
                    fprintf(gOutFile, "%s %3u: BACKUP\n", sep, line2);
                }
            } else {
                if (bupline && line1 == line2)
                    fprintf(gOutFile, "%s %3u: RESTORE\n", sep, line2);
                bupline = 0;
                while (line1 < line2) {
                    if (!fgets(linebuf, LINE_BUF_LEN, file)) {
                        JS_ReportErrorNumber(cx, my_GetErrorMessage, NULL,
                                             JSSMSG_UNEXPECTED_EOF,
                                             script->filename);
                        goto bail;
                    }
                    line1++;
                    fprintf(gOutFile, "%s %3u: %s", sep, line1, linebuf);
                }
            }

            len = js_Disassemble1(cx, script, pc,
                                  PTRDIFF(pc, script->code, jsbytecode),
                                  JS_TRUE, stdout);
            if (!len)
                return JS_FALSE;
            pc += len;
        }

      bail:
        fclose(file);
    }
    return JS_TRUE;
#undef LINE_BUF_LEN
}

static JSBool
Tracing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSBool bval;
    JSString *str;

    if (argc == 0) {
        *rval = BOOLEAN_TO_JSVAL(cx->tracefp != 0);
        return JS_TRUE;
    }

    switch (JS_TypeOfValue(cx, argv[0])) {
      case JSTYPE_NUMBER:
        bval = JSVAL_IS_INT(argv[0])
               ? JSVAL_TO_INT(argv[0])
               : (jsint) *JSVAL_TO_DOUBLE(argv[0]);
        break;
      case JSTYPE_BOOLEAN:
        bval = JSVAL_TO_BOOLEAN(argv[0]);
        break;
      default:
        str = JS_ValueToString(cx, argv[0]);
        if (!str)
            return JS_FALSE;
        fprintf(gErrFile, "tracing: illegal argument %s\n",
                JS_GetStringBytes(str));
        return JS_TRUE;
    }
    cx->tracefp = bval ? stderr : NULL;
    return JS_TRUE;
}

typedef struct DumpAtomArgs {
    JSContext   *cx;
    FILE        *fp;
} DumpAtomArgs;

static int
DumpAtom(JSHashEntry *he, int i, void *arg)
{
    DumpAtomArgs *args = (DumpAtomArgs *)arg;
    FILE *fp = args->fp;
    JSAtom *atom = (JSAtom *)he;

    fprintf(fp, "%3d %08x %5lu ",
            i, (uintN)he->keyHash, (unsigned long)atom->number);
    if (ATOM_IS_STRING(atom))
        fprintf(fp, "\"%s\"\n", js_AtomToPrintableString(args->cx, atom));
    else if (ATOM_IS_INT(atom))
        fprintf(fp, "%ld\n", (long)ATOM_TO_INT(atom));
    else
        fprintf(fp, "%.16g\n", *ATOM_TO_DOUBLE(atom));
    return HT_ENUMERATE_NEXT;
}

static void
DumpScope(JSContext *cx, JSObject *obj, FILE *fp)
{
    uintN i;
    JSScope *scope;
    JSScopeProperty *sprop;

    i = 0;
    scope = OBJ_SCOPE(obj);
    for (sprop = SCOPE_LAST_PROP(scope); sprop; sprop = sprop->parent) {
        if (SCOPE_HAD_MIDDLE_DELETE(scope) && !SCOPE_HAS_PROPERTY(scope, sprop))
            continue;
        fprintf(fp, "%3u %p", i, sprop)