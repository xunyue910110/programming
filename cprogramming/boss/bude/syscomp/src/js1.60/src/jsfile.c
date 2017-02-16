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
 * JS File object
 */
#if JS_HAS_FILE_OBJECT

#include "jsstddef.h"

/* ----------------- Platform-specific includes and defines ----------------- */
#if defined(XP_WIN) || defined(XP_OS2)
#   include <direct.h>
#   include <io.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   define FILESEPARATOR        '\\'
#   define FILESEPARATOR2       '/'
#   define CURRENT_DIR          "c:\\"
#   define POPEN                _popen
#   define PCLOSE               _pclose
#elif defined(XP_UNIX) || defined(XP_BEOS)
#   include <strings.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <unistd.h>
#   define FILESEPARATOR        '/'
#   define FILESEPARATOR2       '\0'
#   define CURRENT_DIR          "/"
#   define POPEN                popen
#   define PCLOSE               pclose
#endif

/* --------------- Platform-independent includes and defines ---------------- */
#include "jsapi.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsdate.h"
#include "jsdbgapi.h"
#include "jsemit.h"
#include "jsfun.h"
#include "jslock.h"
#include "jsobj.h"
#include "jsparse.h"
#include "jsscan.h"
#include "jsscope.h"
#include "jsscript.h"
#include "jsstr.h"
#include "jsutil.h" /* Added by JSIFY */
#include <string.h>

/* NSPR dependencies */
#include "prio.h"
#include "prerror.h"

#define SPECIAL_FILE_STRING     "Special File"
#define CURRENTDIR_PROPERTY     "currentDir"
#define SEPARATOR_PROPERTY      "separator"
#define FILE_CONSTRUCTOR        "File"
#define PIPE_SYMBOL             '|'

#define ASCII                   0
#define UTF8                    1
#define UCS2                    2

#define asciistring             "text"
#define utfstring               "binary"
#define unicodestring           "unicode"

#define MAX_PATH_LENGTH         1024
#define MODE_SIZE               256
#define NUMBER_SIZE             32
#define MAX_LINE_LENGTH         256
#define URL_PREFIX              "file://"

#define STDINPUT_NAME           "Standard input stream"
#define STDOUTPUT_NAME          "Standard output stream"
#define STDERROR_NAME           "Standard error stream"

#define RESOLVE_PATH            js_canonicalPath        /* js_absolutePath */

/* Error handling */
typedef enum JSFileErrNum {
#define MSG_DEF(name, number, count, exception, format) \
    name = number,
#include "jsfile.msg"
#undef MSG_DEF
    JSFileErr_Limit
#undef MSGDEF
} JSFileErrNum;

#define JSFILE_HAS_DFLT_MSG_STRINGS 1

JSErrorFormatString JSFile_ErrorFormatString[JSFileErr_Limit] = {
#if JSFILE_HAS_DFLT_MSG_STRINGS
#define MSG_DEF(name, number, count, exception, format) \
    { format, count },
#else
#define MSG_DEF(name, number, count, exception, format) \
    { NULL, count },
#endif
#include "jsfile.msg"
#undef MSG_DEF
};

const JSErrorFormatString *
JSFile_GetErrorMessage(void *userRef, const char *locale,
                                                        const uintN errorNumber)
{
    if ((errorNumber > 0) && (errorNumber < JSFileErr_Limit))
        return &JSFile_ErrorFormatString[errorNumber];
    else
        return NULL;
}

#define JSFILE_CHECK_NATIVE(op)                                               \
    if (file->isNative) {                                                     \
        JS_ReportWarning(cx, "Cannot call or access \"%s\" on native file %s",\
                         op, file->path);                                     \
        goto out;                                                             \
    }

#define JSFILE_CHECK_WRITE                                                    \
    if (!file->isOpen) {                                                      \
        JS_ReportWarning(cx,                                                  \
                "File %s is closed, will open it for writing, proceeding",    \
                file->path);                                                  \
        js_FileOpen(cx, obj, file, "write,append,create");                    \
    }                                                                         \
    if (!js_canWrite(cx, file)) {                                             \
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,                \
                             JSFILEMSG_CANNOT_WRITE, file->path);             \
        goto out;                                                             \
    }

#define JSFILE_CHECK_READ                                                     \
    if (!file->isOpen) {                                                      \
        JS_ReportWarning(cx,                                                  \
                "File %s is closed, will open it for reading, proceeding",    \
                file->path);                                                  \
        js_FileOpen(cx, obj, file, "read");                                   \
    }                                                                         \
    if (!js_canRead(cx, file)) {                                              \
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,                \
                             JSFILEMSG_CANNOT_READ, file->path);              \
        goto out;                                                             \
    }

#define JSFILE_CHECK_OPEN(op)                                                 \
    if (!file->isOpen) {                                                      \
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,                \
                             JSFILEMSG_FILE_MUST_BE_CLOSED, op);              \
        goto out;                                                             \
    }

#define JSFILE_CHECK_CLOSED(op)                                               \
    if (file->isOpen) {                                                       \
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,                \
            JSFILEMSG_FILE_MUST_BE_OPEN, op);                                 \
        goto out;                                                             \
    }

#define JSFILE_CHECK_ONE_ARG(op)                                              \
    if (argc != 1) {                                                          \
        char str[NUMBER_SIZE];                                                \
        sprintf(str, "%d", argc);                                             \
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,                \
                             JSFILEMSG_EXPECTS_ONE_ARG_ERROR, op, str);       \
        goto out;                                                             \
    }


/*
    Security mechanism, should define a callback for this.
    The parameters are as follows:
    SECURITY_CHECK(JSContext *cx, JSPrincipals *ps, char *op_name, JSFile *file)
    XXX Should this be a real function returning a JSBool result (and getting
    some typesafety help from the compiler?).
*/
#define SECURITY_CHECK(cx, ps, op, file)    \
        /* Define a callback here... */


/* Structure representing the file internally */
typedef struct JSFile {
    char        *path;          /* the path to the file. */
    JSBool      isOpen;
    int32       mode;           /* mode used to open the file: read, write, append, create, etc.. */
    int32       type;           /* Asciiz, utf, unicode */
    char        byteBuffer[3];  /* bytes read in advance by js_FileRead ( UTF8 encoding ) */
    jsint       nbBytesInBuf;   /* number of bytes stored in the buffer above */
    jschar      charBuffer;     /* character read in advance by readln ( mac files only ) */
    JSBool      charBufferUsed; /* flag indicating if the buffer above is being used */
    JSBool      hasRandomAccess;/* can the file be randomly accessed? false for stdin, and
                                 UTF-encoded files. */
    JSBool      hasAutoflush;   /* should we force a flush for each line break? */
    JSBool      isNative;       /* if the file is using OS-specific file FILE type */
    /* We can actually put the following two in a union since they should never be used at the same time */
    PRFileDesc  *handle;        /* the handle for the file, if open.  */
    FILE        *nativehandle;  /* native handle, for stuff NSPR doesn't do. */
    JSBool      isPipe;         /* if the file is really an OS pipe */
} JSFile;

/* a few forward declarations... */
static JSClass file_class;
JS_PUBLIC_API(JSObject*) js_NewFileObject(JSContext *cx, char *filename);
static JSBool file_open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool file_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

/* New filename manipulation procesures */
/* assumes we don't have leading/trailing spaces */
static JSBool
js_filenameHasAPipe(const char *filename)
{
    if (!filename)
        return JS_FALSE;

    return  filename[0] == PIPE_SYMBOL ||
            filename[strlen(filename) - 1] == PIPE_SYMBOL;
}

static JSBool
js_isAbsolute(const char *name)
{
#if defined(XP_WIN) || defined(XP_OS2)
    return *name && name[1] == ':';
#else
    return (name[0]
#   if defined(XP_UNIX) || defined(XP_BEOS)
            ==
#   else
            !=
#   endif
            FILESEPARATOR);
#endif
}

/*
 * Concatinates base and name to produce a valid filename.
 * Returned string must be freed.
*/
static char*
js_combinePath(JSContext *cx, const char *base, const char *name)
{
    int len = strlen(base);
    char* result = JS_malloc(cx, len + strlen(name) + 2);

    if (!result)
        return NULL;

    strcpy(result, base);

    if (base[len - 1] != FILESEPARATOR && base[len - 1] != FILESEPARATOR2) {
        result[len] = FILESEPARATOR;
        result[len + 1] = '\0';
    }
    strcat(result, name);
    return result;
}

/* Extract the last component from a path name. Returned string must be freed */
static char *
js_fileBaseName(JSContext *cx, const char *pathname)
{
    jsint index, aux;
    char *result;

    index = strlen(pathname)-1;

    /* Chop off trailing seperators. */
    while (index > 0 && (pathname[index]==FILESEPARATOR ||
                         pathname[index]==FILESEPARATOR2)) {
        --index;
    }

    aux = index;

    /* Now find the next separator. */
    while (index >= 0 && pathname[index] != FILESEPARATOR &&
                         pathname[index] != FILESEPARATOR2) {
        --index;
    }

    /* Allocate and copy. */
    result = JS_malloc(cx, aux - index + 1);
    if (!result)
        return NULL;
    strncpy(result, pathname + index + 1, aux - index);
    result[aux - index] = '\0';
    return result;
}

/*
 * Returns everything but the last component from a path name.
 * Returned string must be freed.
 */
static char *
js_fileDirectoryName(JSContext *cx, const char *pathname)
{
    char *result;
    const char *cp, *end;
    size_t pathsize;

    end = pathname + strlen(pathname);
    cp = end - 1;

    /* If this is already a directory, chop off the trailing /s. */
    while (cp >= pathname) {
        if (*cp != FILESEPARATOR && *cp != FILESEPARATOR2)
            break;
        --cp;
    }

    if (cp < pathname && end != pathname) {
        /* There were just /s, return the root. */
        result = JS_malloc(cx, 1 + 1); /* The separator + trailing NUL. */
        result[0] = FILESEPARATOR;
        result[1] = '\0';
        return result;
    }

    /* Now chop off the last portion. */
    while (cp >= pathname) {
        if (*cp == FILESEPARATOR || *cp == FILESEPARATOR2)
            break;
        --cp;
    }

    /* Check if this is a leaf. */
    if (cp < pathname) {
        /* It is, return "pathname/". */
        if (end[-1] == FILESEPARATOR || end[-1] == FILESEPARATOR2) {
            /* Already has its terminating /. */
            return JS_strdup(cx, pathname);
        }

        pathsize = end - pathname + 1;
        result = JS_malloc(cx, pathsize + 1);
        if (!result)
            return NULL;

        strcpy(result, pathname);
        result[pathsize - 1] = FILESEPARATOR;
        result[pathsize] = '\0';

        return result;
    }

    /* Return everything up to and including the seperator. */
    pathsize = cp - pathname + 1;
    result = JS_malloc(cx, pathsize + 1);
    if (!result)
        return NULL;

    strncpy(result, pathname, pathsize);
    result[pathsize] = '\0';

    return result;
}

static char *
js_absolutePath(JSContext *cx, const char * path)
{
    JSObject *obj;
    JSString *str;
    jsval prop;

    if (js_isAbsolute(path)) {
        return JS_strdup(cx, path);
    } else {
        obj = JS_GetGlobalObject(cx);
        if (!JS_GetProperty(cx, obj, FILE_CONSTRUCTOR, &prop)) {
            JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,
                                 JSFILEMSG_FILE_CONSTRUCTOR_UNDEFINED_ERROR);
            return JS_strdup(cx, path);
        }

        obj = JSVAL_TO_OBJECT(prop);
        if (!JS_GetProperty(cx, obj, CURRENTDIR_PROPERTY, &prop)) {
            JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,
                                 JSFILEMSG_FILE_CURRENTDIR_UNDEFINED_ERROR);
            return JS_strdup(cx, path);
        }

        str = JS_ValueToString(cx, prop);
        if (!str)
            return JS_strdup(cx, path);

        /* should we have an array of curr dirs indexed by drive for windows? */
        return js_combinePath(cx, JS_GetStringBytes(str), path);
    }
}

/* Side effect: will remove spaces in the beginning/end of the filename */
static char *
js_canonicalPath(JSContext *cx, char *oldpath)
{
    char *tmp;
    char *path = oldpath;
    char *base, *dir, *current, *result;
    jsint c;
    jsint back = 0;
    unsigned int i = 0, j = strlen(path)-1;

    /* This is probably optional */
	/* Remove possible spaces in the beginning and end */
    while (i < j && path[i] == ' ')
        i++;
    while (j >= 0 && path[j] == ' ')
        j--;

    tmp = JS_malloc(cx, j-i+2);
    if (!tmp)
        return NULL;

    strncpy(tmp, path + i, j - i + 1);
    tmp[j - i + 1] = '\0';

    path = tmp;

    /* Pipe support. */
    if (js_filenameHasAPipe(path))
        return path;

    /* file:// support. */
    if (!strncmp(path, URL_PREFIX, strlen(URL_PREFIX))) {
        tmp = js_canonicalPath(cx, path + strlen(URL_PREFIX));
        JS_free(cx, path);
        return tmp;
    }

    if (!js_isAbsolute(path)) {
        tmp = js_absolutePath(cx, path);
        if (!tmp)
            return NULL;
        path = tmp;
    }

    result = JS_strdup(cx, "");

    current = path;

    base = js_fileBaseName(cx, current);
    dir = js_fileDirectoryName(cx, current);

    while (strcmp(dir, current)) {
        if (!strcmp(base, "..")) {
            back++;
        } else {
            if (back > 0) {
                back--;
            } else {
                tmp = result;
                result = JS_malloc(cx, strlen(base) + 1 + strlen(tmp) + 1);
                if (!result)
                    goto out;

                strcpy(result, base);
                c = strlen(result);
                if (*tmp) {
                    result[c] = FILESEPARATOR;
                    result[c + 1] = '\0';
                    strcat(result, tmp);
                }
                JS_free(cx, tmp);
            }
        }
        JS_free(cx, current);
        JS_free(cx, base);
        current = dir;
        base =  js_fileBaseName(cx, current);
        dir = js_fileDirectoryName(cx, current);
    }

    tmp = result;
    result = JS_malloc(cx, strlen(dir)+1+strlen(tmp)+1);
    if (!result)
        goto out;

    strcpy(result, dir);
    c = strlen(result);
    if (tmp[0]!='\0') {
        if ((result[c-1]!=FILESEPARATOR)&&(result[c-1]!=FILESEPARATOR2)) {
            result[c] = FILESEPARATOR;
            result[c+1] = '\0';
        }
        strcat(result, tmp);
    }

out:
    if (tmp)
        JS_free(cx, tmp);
    if (dir)
        JS_free(cx, dir);
    if (base)
        JS_free(cx, base);
    if (current)
        JS_free(cx, current);

    return result;
}

/* -------------------------- Text conversion ------------------------------- */
/* The following is ripped from libi18n/unicvt.c and include files.. */

/*
 * UTF8 defines and macros
 */
#define ONE_OCTET_BASE          0x00    /* 0xxxxxxx */
#define ONE_OCTET_MASK          0x7F    /* x1111111 */
#define CONTINUING_OCTET_BASE   0x80    /* 10xxxxxx */
#define CONTINUING_OCTET_MASK   0x3F    /* 00111111 */
#define TWO_OCTET_BASE          0xC0    /* 110xxxxx */
#define TWO_OCTET_MASK          0x1F    /* 00011111 */
#define THREE_OCTET_BASE        0xE0    /* 1110xxxx */
#define THREE_OCTET_MASK        0x0F    /* 00001111 */
#define FOUR_OCTET_BASE         0xF0    /* 11110xxx */
#define FOUR_OCTET_MASK         0x07    /* 00000111 */
#define FIVE_OCTET_BASE         0xF8    /* 111110xx */
#define FIVE_OCTET_MASK         0x03    /* 00000011 */
#define SIX_OCTET_BASE          0xFC    /* 1111110x */
#define SIX_OCTET_MASK          0x01    /* 00000001 */

#define IS_UTF8_1ST_OF_1(x) (( (x)&~ONE_OCTET_MASK  ) == ONE_OCTET_BASE)
#define IS_UTF8_1ST_OF_2(x) (( (x)&~TWO_OCTET_MASK  ) == TWO_OCTET_BASE)
#define IS_UTF8_1ST_OF_3(x) (( (x)&~THREE_OCTET_MASK) == THREE_OCTET_BASE)
#define IS_UTF8_1ST_OF_4(x) (( (x)&~FOUR_OCTET_MASK ) == FOUR_OCTET_BASE)
#define IS_UTF8_1ST_OF_5(x) (( (x)&~FIVE_OCTET_MASK ) == FIVE_OCTET_BASE)
#define IS_UTF8_1ST_OF_6(x) (( (x)&~SIX_OCTET_MASK  ) == SIX_OCTET_BASE)
#define IS_UTF8_2ND_THRU_6TH(x) \
                    (( (x)&~CONTINUING_OCTET_MASK  ) == CONTINUING_OCTET_BASE)
#define IS_UTF8_1ST_OF_UCS2(x) \
            IS_UTF8_1ST_OF_1(x) \
            || IS_UTF8_1ST_OF_2(x) \
            || IS_UTF8_1ST_OF_3(x)


#define MAX_UCS2            0xFFFF
#define DEFAULT_CHAR        0x003F  /* Default char is "?" */
#define BYTE_MASK           0xBF
#define BYTE_MARK           0x80


/* Function: one_ucs2_to_utf8_char
 *
 * Function takes one UCS-2 char and writes it to a UTF-8 buffer.
 * We need a UTF-8 buffer because we don't know before this
 * function how many bytes of utf-8 data will be written. It also
 * takes a pointer to the end of the UTF-8 buffer so that we don't
 * overwrite data. This function returns the number of UTF-8 bytes
 * of data written, or -1 if the buffer would have been overrun.
 */

#define LINE_SEPARATOR      0x2028
#define PARAGRAPH_SEPARATOR 0x2029
static int16 one_ucs2_to_utf8_char(unsigned char *tobufp,
                                   unsigned char *tobufendp,
                                   uint16 onechar)
{
    int16 numUTF8bytes = 0;

    if (onechar == LINE_SEPARATOR || onechar == PARAGRAPH_SEPARATOR) {
        strcpy((char*)tobufp, "\n");
        return strlen((char*)tobufp);
    }

    if (onechar < 0x80) {
        numUTF8bytes = 1;
    } else if (onechar < 0x800) {
        numUTF8bytes = 2;
    } else {
        /* 0x800 >= onechar <= MAX_UCS2 */
        numUTF8bytes = 3;
    }

    tobufp += numUTF8bytes;

    /* return error if we don't have space for the whole character */
    if (tobufp > tobufendp) {
        return(-1);
    }

    switch(numUTF8bytes) {
      case 3: *--tobufp = (onechar | BYTE_MARK) & BYTE_MASK; onechar >>=6;
              *--tobufp = (onechar | BYTE_MARK) & BYTE_MASK; onechar >>=6;
              *--tobufp = onechar |  THREE_OCTET_BASE;
              break;

      case 2: *--tobufp = (onechar | BYTE_MARK) & BYTE_MASK; onechar >>=6;
              *--tobufp = onechar | TWO_OCTET_BASE;
              break;

      case 1: *--tobufp = (unsigned char)onechar;
              break;
    }

    return numUTF8bytes;
}

/*
 * utf8_to_ucs2_char
 *
 * Convert a utf8 multibyte character to ucs2
 *
 * inputs: pointer to utf8 character(s)
 *         length of utf8 buffer ("read" length limit)
 *         pointer to return ucs2 character
 *
 * outputs: number of bytes in the utf8 character
 *          -1 if not a valid utf8 character sequence
 *          -2 if the buffer is too short
 */
static int16
utf8_to_ucs2_char(const unsigned char *utf8p, int16 buflen, uint16 *ucs2p)
{
    uint16 lead, cont1, cont2;

    /*
     * Check for minimum buffer length
     */
    if ((buflen < 1) || (utf8p == NULL)) {
        return -2;
    }
    lead = (uint16) (*utf8p);

    /*
     * Check for a one octet sequence
     */
    if (IS_UTF8_1ST_OF_1(lead)) {
        *ucs2p = lead & ONE_OCTET_MASK;
        return 1;
    }

    /*
     * Check for a two octet sequence
     */
    if (IS_UTF8_1ST_OF_2(*utf8p)) {
        if (buflen < 2)
            return -2;
        cont1 = (uint16) *(utf8p+1);
        if (!IS_UTF8_2ND_THRU_6TH(cont1))
            return -1;
        *ucs2p =  (lead & TWO_OCTET_MASK) << 6;
        *ucs2p |= cont1 & CONTINUING_OCTET_MASK;
        return 2;
    }

    /*
     * Check for a three octet sequence
     */
    else if (IS_UTF8_1ST_OF_3(lead)) {
        if (buflen < 3)
            return -2;
        cont1 = (uint16) *(utf8p+1);
        cont2 = (uint16) *(utf8p+2);
        if (   (!IS_UTF8_2ND_THRU_6TH(cont1))
            || (!IS_UTF8_2ND_THRU_6TH(cont2)))
            return -1;
        *ucs2p =  (lead & THREE_OCTET_MASK) << 12;
        *ucs2p |= (cont1 & CONTINUING_OCTET_MASK) << 6;
        *ucs2p |= cont2 & CONTINUING_OCTET_MASK;
        return 3;
    }
    else { /* not a valid utf8/ucs2 character */
        return -1;
    }
}

/* ----------------------------- Helper functions --------------------------- */
/* Ripped off from lm_win.c .. */
/* where is strcasecmp?.. for now, it's case sensitive..
 *
 * strcasecmp is in strings.h, but on windows it's called _stricmp...
 * will need to #ifdef this
*/

static int32
js_FileHasOption(JSContext *cx, const char *oldoptions, const char *name)
{
    char *comma, *equal, *current;
    char *options = JS_strdup(cx, oldoptions);
    int32 found = 0;

    current = options;
    for (;;) {
        comma = strchr(current, ',');
        if (comma) *comma = '\0';
        equal = strchr(current, '=');
        if (equal) *equal = '\0';
        if (strcmp(current, name) == 0) {
            if (!equal || strcmp(equal + 1, "yes") == 0)
                found = 1;
            else
                found = atoi(equal + 1);
        }
        if (equal) *equal = '=';
        if (comma) *comma = ',';
        if (found || !comma)
            break;
        current = comma + 1;
    }
    JS_free(cx, options);
    return found;
}

/* empty the buffer */
static void
js_ResetBuffers(JSFile * file)
{
    file->charBufferUsed = JS_FALSE;
    file->nbBytesInBuf = 0;
}

/* Reset file attributes */
static void
js_ResetAttributes(JSFile * file)
{
    file->mode = file->type = 0;
    file->isOpen = JS_FALSE;
    file->handle = NULL;
    file->nativehandle = NULL;
    file->hasRandomAccess = JS_TRUE; /* Innocent until proven guilty. */
    file->hasAutoflush = JS_FALSE;
    file->isNative = JS_FALSE;
    file->isPipe = JS_FALSE;

    js_ResetBuffers(file);
}

static JSBool
js_FileOpen(JSContext *cx, JSObject *obj, JSFile *file, char *mode){
    JSString *type, *mask;
    jsval v[2];
    jsval rval;

    type =  JS_InternString(cx, asciistring);
    mask =  JS_NewStringCopyZ(cx, mode);
    v[0] = STRING_TO_JSVAL(mask);
    v[1] = STRING_TO_JSVAL(type);

    if (!file_open(cx, obj, 2, v, &rval))
        return JS_FALSE;
    return JS_TRUE;
}

/* Buffered version of PR_Read. Used by js_FileRead */
static int32
js_BufferedRead(JSFile * f, char *buf, int32 len)
{
    int32 count = 0;

    while (f->nbBytesInBuf>0&&len>0) {
        buf[0] = f->byteBuffer[0];
        f->byteBuffer[0] = f->byteBuffer[1];
        f->byteBuffer[1] = f->byteBuffer[2];
        f->nbBytesInBuf--;
        len--;
        buf+=1;
        count++;
    }

    if (len>0) {
        count += (!f->isNative)
                 ? PR_Read(f->handle, buf, len)
                 : fread(buf, 1, len, f->nativehandle);
    }
    return count;
}

static int32
js_FileRead(JSContext *cx, JSFile *file, jschar *buf, int32 len, int32 mode)
{
    unsigned char *aux;
    int32 count = 0, i;
    jsint remainder;
    unsigned char utfbuf[3];

    if (file->charBufferUsed) {
        buf[0] = file->charBuffer;
        buf++;
        len--;
        file->charBufferUsed = JS_FALSE;
    }

    switch (mode) {
      case ASCII:
        aux = (unsigned char*)JS_malloc(cx, len);
        if (!aux)
            return 0;

        count = js_BufferedRead(file, aux, len);
        if (count == -1) {
            JS_free(cx, aux);
            return 0;
        }

        for (i = 0; i < len; i++)
            buf[i] = (jschar)aux[i];

        JS_free(cx, aux);
        break;

      case UTF8:
        remainder = 0;
        for (count = 0;count<len;count++) {
            i = js_BufferedRead(file, utfbuf+remainder, 3-remainder);
            if (i<=0) {
                return count;
            }
            i = utf8_to_ucs2_char(utfbuf, (int16)i, &buf[count] );
            if (i<0) {
                return count;
            } else {
                if (i==1) {
                    utfbuf[0] = utfbuf[1];
                    utfbuf[1] = utfbuf[2];
                    remainder = 2;
                } else if (i==2) {
                    utfbuf[0] = utfbuf[2];
                    remainder = 1;
                } else if (i==3) {
                    remainder = 0;
                }
            }
        }
        while (remainder>0) {
            file->byteBuffer[file->nbBytesInBuf] = utfbuf[0];
            file->nbBytesInBuf++;
            utfbuf[0] = utfbuf[1];
            utfbuf[1] = utfbuf[2];
            remainder--;
        }
        break;

      case UCS2:
        count = js_BufferedRead(file, (char*)buf, len*2) >> 1;
        if (count == -1)
            return 0;

        break;

      default:
        /* Not reached. */
        JS_ASSERT(0);
    }

    if(count == -1) {
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,
                             JSFILEMSG_OP_FAILED, "read", file->path);
    }

    return count;
}

static int32
js_FileSeek(JSContext *cx, JSFile *file, int32 len, int32 mode)
{
    int32 count = 0, i;
    jsint remainder;
    unsigned char utfbuf[3];
    jschar tmp;

    switch (mode) {
      case ASCII:
        count = PR_Seek(file->handle, len, PR_SEEK_CUR);
        break;

      case UTF8:
        remainder = 0;
        for (count = 0;count<len;count++) {
            i = js_BufferedRead(file, utfbuf+remainder, 3-remainder);
            if (i<=0) {
                return 0;
            }
            i = utf8_to_ucs2_char(utfbuf, (int16)i, &tmp );
            if (i<0) {
                return 0;
            } else {
                if (i==1) {
                    utfbuf[0] = utfbuf[1];
                    utfbuf[1] = utfbuf[2];
                    remainder = 2;
                } else if (i==2) {
                    utfbuf[0] = utfbuf[2];
                    remainder = 1;
                } else if (i==3) {
                    remainder = 0;
                }
            }
        }
        while (remainder>0) {
            file->byteBuffer[file->nbBytesInBuf] = utfbuf[0];
            file->nbBytesInBuf++;
            utfbuf[0] = utfbuf[1];
            utfbuf[1] = utfbuf[2];
            remainder--;
        }
        break;

      case UCS2:
        count = PR_Seek(file->handle, len*2, PR_SEEK_CUR)/2;
        break;

      default:
        /* Not reached. */
        JS_ASSERT(0);
    }

    if(count == -1) {
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,
                             JSFILEMSG_OP_FAILED, "seek", file->path);
    }

    return count;
}

static int32
js_FileWrite(JSContext *cx, JSFile *file, jschar *buf, int32 len, int32 mode)
{
    unsigned char   *aux;
    int32           count = 0, i, j;
    unsigned char   *utfbuf;

    switch (mode) {
      case ASCII:
        aux = (unsigned char*)JS_malloc(cx, len);
        if (!aux)
            return 0;

        for (i = 0; i<len; i++)
            aux[i] = buf[i] % 256;

        count = (!file->isNative)
                ? PR_Write(file->handle, aux, len)
                : fwrite(aux, 1, len, file->nativehandle);

        if (count==-1) {
            JS_free(cx, aux);
            return 0;
        }

        JS_free(cx, aux);
        break;

      case UTF8:
        utfbuf = (unsigned char*)JS_malloc(cx, len*3);
        if (!utfbuf)  return 0;
        i = 0;
        for (count = 0;count<len;count++) {
            j = one_ucs2_to_utf8_char(utfbuf+i, utfbuf+len*3, buf[count]);
            if (j==-1) {
                JS_free(cx, utfbuf);
                return 0;
            }
            i+=j;
        }
        j = (!file->isNative) 
            ? PR_Write(file->handle, utfbuf, i)
            : fwrite(utfbuf, 1, i, file->nativehandle);

        if (j<i) {
            JS_free(cx, utfbuf);
            return 0;
        }
        JS_free(cx, utfbuf);
        break;

      case UCS2:
        count = (!file->isNative) 
                ? PR_Write(file->handle, buf, len*2) >> 1
                : fwrite(buf, 1, len*2, file->nativehandle) >> 1;

        if (count == -1)
            return 0;
        break;

      default:
        /* Not reached. */
        JS_ASSERT(0);
    }

    if(count == -1) {
        JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,
                             JSFILEMSG_OP_FAILED, "write", file->path);
    }

    return count;
}

/* ----------------------------- Property checkers -------------------------- */
static JSBool
js_exists(JSContext *cx, JSFile *file)
{
    if (file->isNative) {
        /* It doesn't make sense for a pipe of stdstream. */
        return JS_FALSE;
    }

    return PR_Access(file->path, PR_ACCESS_EXISTS) == PR_SUCCESS;
}

static JSBool
js_canRead(JSContext *cx, JSFile *file)
{
    if (!file->isNative) {
        if (file->isOpen && !(file->mode & PR_RDONLY))
            return JS_FALSE;
        return PR_Access(file->path, PR_ACCESS_READ_OK) == PR_SUCCESS;
    }

    if (file->isPipe) {
        /* Is this pipe open for reading? */
        return file->path[0] == PIPE_SYMBOL;
    }

    return !strcmp(file->path, STDINPUT_NAME);
}

static JSBool
js_canWrite(JSContext *cx, JSFile *file)
{
    if (!file->isNative) {
        if (file->isOpen && !(file->mode & PR_WRONLY))
            return JS_FALSE;
        return PR_Access(file->path, PR_ACCESS_WRITE_OK) == PR_SUCCESS;
    }

    if(file->isPipe) {
        /* Is this pipe open for writing? */
        return file->path[strlen(file->path)-1] == PIPE_SYMBOL;
    }

    return !strcmp(file->path, STDOUTPUT_NAME) ||
           !strcmp(file->path, STDERROR_NAME);
}

static JSBool
js_isFile(JSContext *cx, JSFile *file)
{
    if (!file->isNative) {
        PRFileInfo info;

        if (file->isOpen
            ? PR_GetOpenFileInfo(file->handle, &info)
            : PR_GetFileInfo(file->path, &info) != PR_SUCCESS) {
            JS_ReportErrorNumber(cx, JSFile_GetErrorMessage, NULL,
                                