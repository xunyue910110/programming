/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Robert Ginda, <rginda@netscape.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
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

#include "jsd_xpc.h"
#include "jsdbgapi.h"
#include "jscntxt.h"
#include "jsfun.h"

#include "nsIXPConnect.h"
#include "nsIGenericFactory.h"
#include "nsIServiceManager.h"
#include "nsIScriptGlobalObject.h"
#include "nsIObserver.h"
#include "nsIObserverService.h"
#include "nsICategoryManager.h"
#include "nsIJSRuntimeService.h"
#include "nsIEventQueueService.h"
#include "nsMemory.h"
#include "jsdebug.h"
#include "nsReadableUtils.h"
#include "nsCRT.h"

/* XXX this stuff is used by NestEventLoop, a temporary hack to be refactored
 * later */
#include "nsWidgetsCID.h"
#include "nsIScriptContext.h"
#include "nsIAppShell.h"
#include "nsIJSContextStack.h"

/*
 * defining CAUTIOUS_SCRIPTHOOK makes jsds disable GC while calling out to the
 * script hook.  This was a hack to avoid some js engine problems that should
 * be fixed now (see Mozilla bug 77636).
 */
#undef CAUTIOUS_SCRIPTHOOK

#ifdef DEBUG_verbose
#   define DEBUG_COUNT(name, count)                                             \
        { if ((count % 10) == 0) printf (name ": %i\n", count); }
#   define DEBUG_CREATE(name, count) {count++; DEBUG_COUNT ("+++++ "name,count)}
#   define DEBUG_DESTROY(name, count) {count--; DEBUG_COUNT ("----- "name,count)}
#else
#   define DEBUG_CREATE(name, count) 
#   define DEBUG_DESTROY(name, count)
#endif

#define ASSERT_VALID_CONTEXT   { if (!mCx) return NS_ERROR_NOT_AVAILABLE; }
#define ASSERT_VALID_EPHEMERAL { if (!mValid) return NS_ERROR_NOT_AVAILABLE; }

#define JSDSERVICE_CID                               \
{ /* f1299dc2-1dd1-11b2-a347-ee6b7660e048 */         \
     0xf1299dc2,                                     \
     0x1dd1,                                         \
     0x11b2,                                         \
    {0xa3, 0x47, 0xee, 0x6b, 0x76, 0x60, 0xe0, 0x48} \
}

#define JSDASO_CID                                   \
{ /* 2fd6b7f6-eb8c-4f32-ad26-113f2c02d0fe */         \
     0x2fd6b7f6,                                     \
     0xeb8c,                                         \
     0x4f32,                                         \
    {0xad, 0x26, 0x11, 0x3f, 0x2c, 0x02, 0xd0, 0xfe} \
}

#define JSDS_MAJOR_VERSION 1
#define JSDS_MINOR_VERSION 2

#define NS_CATMAN_CTRID   "@mozilla.org/categorymanager;1"
#define NS_JSRT_CTRID     "@mozilla.org/js/xpc/RuntimeService;1"

#define AUTOREG_CATEGORY  "xpcom-autoregistration"
#define APPSTART_CATEGORY "app-startup"
#define JSD_AUTOREG_ENTRY "JSDebugger Startup Observer"
#define JSD_STARTUP_ENTRY "JSDebugger Startup Observer,service"

JS_STATIC_DLL_CALLBACK (JSBool)
jsds_GCCallbackProc (JSContext *cx, JSGCStatus status);

/*******************************************************************************
 * global vars
 ******************************************************************************/

const char implementationString[] = "Mozilla JavaScript Debugger Service";
static NS_DEFINE_CID(kAppShellCID, NS_APPSHELL_CID);
static NS_DEFINE_CID(kEventQueueServiceCID, NS_EVENTQUEUESERVICE_CID);

const char jsdServiceCtrID[] = "@mozilla.org/js/jsd/debugger-service;1";
const char jsdASObserverCtrID[] = "@mozilla.org/js/jsd/app-start-observer;2";

#ifdef DEBUG_verbose
PRUint32 gScriptCount   = 0;
PRUint32 gValueCount    = 0;
PRUint32 gPropertyCount = 0;
PRUint32 gContextCount  = 0;
PRUint32 gFrameCount  = 0;
#endif

static jsdService   *gJsds       = 0;
static JSGCCallback  gLastGCProc = jsds_GCCallbackProc;
static JSGCStatus    gGCStatus   = JSGC_END;

static struct DeadScript {
    PRCList     links;
    JSDContext *jsdc;
    jsdIScript *script;
} *gDeadScripts = nsnull;

enum PatternType {
    ptIgnore     = 0U,
    ptStartsWith = 1U,
    ptEndsWith   = 2U,
    ptContains   = 3U,
    ptEquals     = 4U
};

static struct FilterRecord {
    PRCList      links;
    jsdIFilter  *filterObject;
    void        *glob;
    char        *urlPattern;    
    PRUint32     patternLength;
    PatternType  patternType;
    PRUint32     startLine;
    PRUint32     endLine;
} *gFilters = nsnull;

static struct LiveEphemeral *gLiveValues      = nsnull;
static struct LiveEphemeral *gLiveProperties  = nsnull;
static struct LiveEphemeral *gLiveContexts    = nsnull;
static struct LiveEphemeral *gLiveStackFrames = nsnull;

/*******************************************************************************
 * utility functions for ephemeral lists
 *******************************************************************************/
already_AddRefed<jsdIEphemeral>
jsds_FindEphemeral (LiveEphemeral **listHead, void *key)
{
    if (!*listHead)
        return nsnull;
    
    LiveEphemeral *lv_record = 
        NS_REINTERPRET_CAST (LiveEphemeral *,
                             PR_NEXT_LINK(&(*listHead)->links));
    do
    {
        if (lv_record->key == key)
        {
            NS_IF_ADDREF(lv_record->value);
            return lv_record->value;
        }
        lv_record = NS_REINTERPRET_CAST (LiveEphemeral *,
                                         PR_NEXT_LINK(&lv_record->links));
    }
    while (lv_record != *listHead);

    return nsnull;
}

void
jsds_InvalidateAllEphemerals (LiveEphemeral **listHead)
{
    LiveEphemeral *lv_record = 
        NS_REINTERPRET_CAST (LiveEphemeral *,
                             PR_NEXT_LINK(&(*listHead)->links));
    while (*listHead)
    {
        LiveEphemeral *next =
            NS_REINTERPRET_CAST (LiveEphemeral *,
                                 PR_NEXT_LINK(&lv_record->links));
        lv_record->value->Invalidate();
        lv_record = next;
    }
}

void
jsds_InsertEphemeral (LiveEphemeral **listHead, LiveEphemeral *item)
{
    if (*listHead) {
        /* if the list exists, add to it */
        PR_APPEND_LINK(&item->links, &(*listHead)->links);
    } else {
        /* otherwise create the list */
        PR_INIT_CLIST(&item->links);
        *listHead = item;
    }
}

void
jsds_RemoveEphemeral (LiveEphemeral **listHead, LiveEphemeral *item)
{
    LiveEphemeral *next = NS_REINTERPRET_CAST (LiveEphemeral *,
                                               PR_NEXT_LINK(&item->links));

    if (next == item)
    {
        /* if the current item is also the next item, we're the only element,
         * null out the list head */
        NS_ASSERTION (*listHead == item,
                      "How could we not be the head of a one item list?");
        *listHead = nsnull;
    }
    else if (item == *listHead)
    {
        /* otherwise, if we're currently the list head, change it */
        *listHead = next;
    }
    
    PR_REMOVE_AND_INIT_LINK(&item->links);
}

/*******************************************************************************
 * utility functions for filters
 *******************************************************************************/
void
jsds_FreeFilter (FilterRecord *filter)
{
    NS_IF_RELEASE (filter->filterObject);
    if (filter->urlPattern)
        nsMemory::Free(filter->urlPattern);
    PR_Free (filter);
}

/* copies appropriate |filter| attributes into |rec|.
 * False return indicates failure, the contents of |rec| will not be changed.
 */
PRBool
jsds_SyncFilter (FilterRecord *rec, jsdIFilter *filter)
{
    NS_ASSERTION (rec, "jsds_SyncFilter without rec");
    NS_ASSERTION (filter, "jsds_SyncFilter without filter");
    
    JSObject *glob_proper = nsnull;
    nsCOMPtr<nsISupports> glob;
    nsresult rv = filter->GetGlobalObject(getter_AddRefs(glob));
    if (NS_FAILED(rv))
        return PR_FALSE;
    if (glob) {
        nsCOMPtr<nsIScriptGlobalObject> nsiglob = do_QueryInterface(glob);
        if (nsiglob)
            glob_proper = nsiglob->GetGlobalJSObject();
    }
    
    PRUint32 startLine;
    rv = filter->GetStartLine(&startLine);
    if (NS_FAILED(rv))
        return PR_FALSE;

    PRUint32 endLine;
    rv = filter->GetStartLine(&endLine);
    if (NS_FAILED(rv))
        return PR_FALSE;    

    char *urlPattern;
    rv = filter->GetUrlPattern (&urlPattern);
    if (NS_FAILED(rv))
        return PR_FALSE;
    
    if (urlPattern) {
        PRUint32 len = PL_strlen(urlPattern);
        if (urlPattern[0] == '*') {
            /* pattern starts with a *, shift all chars once to the left,
             * including the trailing null. */
            memmove (&urlPattern[0], &urlPattern[1], len);

            if (urlPattern[len - 2] == '*') {
                /* pattern is in the format "*foo*", overwrite the final * with
                 * a null. */
                urlPattern[len - 2] = '\0';
                rec->patternType = ptContains;
                rec->patternLength = len - 2;
            } else {
                /* pattern is in the format "*foo", just make a note of the
                 * new length. */
                rec->patternType = ptEndsWith;
                rec->patternLength = len - 1;
            }
        } else if (urlPattern[len - 1] == '*') {
            /* pattern is in the format "foo*", overwrite the final * with a 
             * null. */
            urlPattern[len - 1] = '\0';
            rec->patternType = ptStartsWith;
            rec->patternLength = len - 1;
        } else {
            /* pattern is in the format "foo". */
            rec->patternType = ptEquals;
            rec->patternLength = len;
        }
    } else {
        rec->patternType = ptIgnore;
        rec->patternLength = 0;
    }

    /* we got everything we need without failing, now copy it into rec. */

    if (rec->filterObject != filter) {
        NS_IF_RELEASE(rec->filterObject);
        NS_ADDREF(filter);
        rec->filterObject = filter;
    }
    
    rec->glob = glob_proper;
    
    rec->startLine     = startLine;
    rec->endLine       = endLine;
    
    if (rec->urlPattern)
        nsMemory::Free (rec->urlPattern);
    rec->urlPattern = urlPattern;

    return PR_TRUE;
            
}

FilterRecord *
jsds_FindFilter (jsdIFilter *filter)
{
    if (!gFilters)
        return nsnull;
    
    FilterRecord *current = gFilters;
    
    do {
        if (current->filterObject == filter)
            return current;
        current = NS_REINTERPRET_CAST(FilterRecord *,
                                      PR_NEXT_LINK(&current->links));
    } while (current != gFilters);
    
    return nsnull;
}

/* returns true if the hook should be executed. */
PRBool
jsds_FilterHook (JSDContext *jsdc, JSDThreadState *state)
{
    JSContext *cx = JSD_GetJSContext (jsdc, state);
    void *glob = NS_STATIC_CAST(void *, JS_GetGlobalObject (cx));

    if (!glob) {
        NS_WARNING("No global in threadstate");
        return PR_FALSE;
    }
    
    JSDStackFrameInfo *frame = JSD_GetStackFrame (jsdc, state);

    if (!frame) {
        NS_WARNING("No frame in threadstate");
        return PR_FALSE;
    }

    JSDScript *script = JSD_GetScriptForStackFrame (jsdc, state, frame);
    if (!script)
        return PR_TRUE;

    jsuint pc = JSD_GetPCForStackFrame (jsdc, state, frame);

    const char *url = JSD_GetScriptFilename (jsdc, script);
    if (!url) {
        NS_WARNING ("Script with no filename");
        return PR_FALSE;
    }

    if (!gFilters)
        return PR_TRUE;    

    PRUint32 currentLine = JSD_GetClosestLine (jsdc, script, pc);
    PRUint32 len = 0;
    FilterRecord *currentFilter = gFilters;
    do {
        PRUint32 flags = 0;
        nsresult rv = currentFilter->filterObject->GetFlags(&flags);
        NS_ASSERTION(NS_SUCCEEDED(rv), "Error getting flags for filter");
        if (flags & jsdIFilter::FLAG_ENABLED) {
            /* if there is no glob, or the globs match */
            if ((!currentFilter->glob || currentFilter->glob == glob) &&
                /* and there is no start line, or the start line is before 
                 * or equal to the current */
                (!currentFilter->startLine || 
                 currentFilter->startLine <= currentLine) &&
                /* and there is no end line, or the end line is after
                 * or equal to the current */
                (!currentFilter->endLine ||
                 currentFilter->endLine >= currentLine)) {
                /* then we're going to have to compare the url. */
                if (currentFilter->patternType == ptIgnore)
                    return flags & jsdIFilter::FLAG_PASS;

                if (!len)
                    len = PL_strlen(url);
                
                if (len >= currentFilter->patternLength) {
                    switch (currentFilter->patternType) {
                        case ptEquals:
                            if (!PL_strcmp(currentFilter->urlPattern, url))
                                return flags & jsdIFilter::FLAG_PASS;
                            break;
                        case ptStartsWith:
                            if (!PL_strncmp(currentFilter->urlPattern, url, 
                                           currentFilter->patternLength))
                                return flags & jsdIFilter::FLAG_PASS;
                            break;
                        case ptEndsWith:
                            if (!PL_strcmp(currentFilter->urlPattern,
                                           &url[len - 
                                               currentFilter->patternLength]))
                                return flags & jsdIFilter::FLAG_PASS;
                            break;
                        case ptContains:
                            if (PL_strstr(url, currentFilter->urlPattern))
                                return flags & jsdIFilter::FLAG_PASS;
                            break;
                        default:
                            NS_ASSERTION(0, "Invalid pattern type");
                    }
                }                
            }
        }
        currentFilter = NS_REINTERPRET_CAST(FilterRecord *,
                                            PR_NEXT_LINK(&currentFilter->links));
    } while (currentFilter != gFilters);

    return PR_TRUE;
    
}

/*******************************************************************************
 * c callbacks
 *******************************************************************************/

JS_STATIC_DLL_CALLBACK (void)
jsds_NotifyPendingDeadScripts (JSContext *cx)
{
    nsCOMPtr<jsdIScriptHook> hook = 0;   
    gJsds->GetScriptHook (getter_AddRefs(hook));

    DeadScript *ds;
#ifdef CAUTIOUS_SCRIPTHOOK
    JSRuntime *rt = JS_GetRuntime(cx);
#endif
    gJsds->Pause(nsnull);
    while (gDeadScripts) {
        ds = gDeadScripts;
        
        if (hook)
        {
            /* tell the user this script has been destroyed */
#ifdef CAUTIOUS_SCRIPTHOOK
            JS_UNKEEP_ATOMS(rt);
#endif
            hook->OnScriptDestroyed (ds->script);
#ifdef CAUTIOUS_SCRIPTHOOK
            JS_KEEP_ATOMS(rt);
#endif
        }
        /* get next deleted script */
        gDeadScripts = NS_REINTERPRET_CAST(DeadScript *,
                                           PR_NEXT_LINK(&ds->links));
        if (gDeadScripts == ds) {
            /* last script in the list */
            gDeadScripts = nsnull;
        }
        
        /* take ourselves out of the circular list */
        PR_REMOVE_LINK(&ds->links);
        /* addref came from the FromPtr call in jsds_ScriptHookProc */
        NS_RELEASE(ds->script);
        /* free the struct! */
        PR_Free(ds);
    }

    gJsds->UnPause(nsnull);
}

JS_STATIC_DLL_CALLBACK (JSBool)
jsds_GCCallbackProc (JSContext *cx, JSGCStatus status)
{
    gGCStatus = status;
#ifdef DEBUG_verbose
    printf ("new gc status is %i\n", status);
#endif
    if (status == JSGC_END && gDeadScripts)
        jsds_NotifyPendingDeadScripts (cx);
    
    if (gLastGCProc)
        return gLastGCProc (cx, status);
    
    return JS_TRUE;
}

JS_STATIC_DLL_CALLBACK (uintN)
jsds_ErrorHookProc (JSDContext *jsdc, JSContext *cx, const char *message,
                    JSErrorReport *report, void *callerdata)
{
    static PRBool running = PR_FALSE;

    nsCOMPtr<jsdIErrorHook> hook;
    gJsds->GetErrorHook(getter_AddRefs(hook));
    if (!hook)
        return JSD_ERROR_REPORTER_PASS_ALONG;

    if (running)
        return JSD_ERROR_REPORTER_PASS_ALONG;
    
    running = PR_TRUE;
    
    nsCOMPtr<jsdIValue> val;
    if (JS_IsExceptionPending(cx)) {
        jsval jv;
        JS_GetPendingException(cx, &jv);
        JSDValue *jsdv = JSD_NewValue (jsdc, jv);
        val = getter_AddRefs(jsdValue::FromPtr(jsdc, jsdv));
    }
    
    const char *fileName;
    PRUint32    line;
    PRUint32    pos;
    PRUint32    flags;
    PRUint32    errnum;
    PRBool      rval;
    if (report) {
        fileName = report->filename;
        line = report->lineno;
        pos = report->tokenptr - report->linebuf;
        flags = report->flags;
        errnum = report->errorNumber;
    }
    else
    {
        fileName = 0;
        line     = 0;
        pos      = 0;
        flags    = 0;
        errnum   = 0;
    }
    
    gJsds->Pause(nsnull);
    hook->OnError (message, fileName, line, pos, flags, errnum, val, &rval);
    gJsds->UnPause(nsnull);
    
    running = PR_FALSE;
    if (!rval)
        return JSD_ERROR_REPORTER_DEBUG;
    
    return JSD_ERROR_REPORTER_PASS_ALONG;
}

JS_STATIC_DLL_CALLBACK (JSBool)
jsds_CallHookProc (JSDContext* jsdc, JSDThreadState* jsdthreadstate,
                   uintN type, void* callerdata)
{
    nsCOMPtr<jsdICallHook> hook;

    switch (type)
    {
        case JSD_HOOK_TOPLEVEL_START:
        case JSD_HOOK_TOPLEVEL_END:
            gJsds->GetTopLevelHook(getter_AddRefs(hook));
            break;
            
        case JSD_HOOK_FUNCTION_CALL:
        case JSD_HOOK_FUNCTION_RETURN:
            gJsds->GetFunctionHook(getter_AddRefs(hook));
            break;

        default:
            NS_ASSERTION (0, "Unknown hook type.");
    }
    
    if (!hook)
        return JS_TRUE;

    if (!jsds_FilterHook (jsdc, jsdthreadstate))
        return JS_FALSE;

    JSDStackFrameInfo *native_frame = JSD_GetStackFrame (jsdc, jsdthreadstate);
    nsCOMPtr<jsdIStackFrame> frame =
        getter_AddRefs(jsdStackFrame::FromPtr(jsdc, jsdthreadstate,
                                              native_frame));
    gJsds->Pause(nsnull);
    hook->OnCall(frame, type);    
    gJsds->UnPause(nsnull);
    jsdStackFrame::InvalidateAll();

    return JS_TRUE;
}

JS_STATIC_DLL_CALLBACK (PRUint32)
jsds_ExecutionHookProc (JSDContext* jsdc, JSDThreadState* jsdthreadstate,
                        uintN type, void* callerdata, jsval* rval)
{
    nsCOMPtr<jsdIExecutionHook> hook(0);
    PRUint32 hook_rv = JSD_HOOK_RETURN_CONTINUE;
    nsCOMPtr<jsdIValue> js_rv;

    switch (type)
    {
        case JSD_HOOK_INTERRUPTED:
            gJsds->GetInterruptHook(getter_AddRefs(hook));
            break;
        case JSD_HOOK_DEBUG_REQUESTED:
            gJsds->GetDebugHook(getter_AddRefs(hook));
            break;
        case JSD_HOOK_DEBUGGER_KEYWORD:
            gJsds->GetDebuggerHook(getter_AddRefs(hook));
            break;
        case JSD_HOOK_BREAKPOINT:
            {
                /* we can't pause breakpoints the way we pause the other
                 * execution hooks (at least, not easily.)  Instead we bail
                 * here if the service is paused. */
                PRUint32 level;
                gJsds->GetPauseDepth(&level);
                if (!level)
                    gJsds->GetBreakpointHook(getter_AddRefs(hook));
            }
            break;
        case JSD_HOOK_THROW:
        {
            hook_rv = JSD_HOOK_RETURN_CONTINUE_THROW;
            gJsds->GetThrowHook(getter_AddRefs(hook));
            if (hook) {
                JSDValue *jsdv = JSD_GetException (jsdc, jsdthreadstate);
                js_rv = getter_AddRefs(jsdValue::FromPtr (jsdc, jsdv));
            }
            break;
        }
        default:
            NS_ASSERTION (0, "Unknown hook type.");
    }

    if (!hook)
        return hook_rv;
    
    if (!jsds_FilterHook (jsdc, jsdthreadstate))
        return JSD_HOOK_RETURN_CONTINUE;
    
    JSDStackFrameInfo *native_frame = JSD_GetStackFrame (jsdc, jsdthreadstate);
    nsCOMPtr<jsdIStackFrame> frame =
        getter_AddRefs(jsdStackFrame::FromPtr(jsdc, jsdthreadstate,
                                              native_frame));
    gJsds->Pause(nsnull);
    jsdIValue *inout_rv = js_rv;
    NS_IF_ADDREF(inout_rv);
    hook->OnExecute (frame, type, &inout_rv, &hook_rv);
    js_rv = inout_rv;
    NS_IF_RELEASE(inout_rv);
    gJsds->UnPause(nsnull);
    jsdStackFrame::InvalidateAll();
        
    if (hook_rv == JSD_HOOK_RETURN_RET_WITH_VAL ||
        hook_rv == JSD_HOOK_RETURN_THROW_WITH_VAL) {
        if (js_rv) {
            JSDValue *jsdv;
            js_rv->GetJSDValue (&jsdv);
            *rval = JSD_GetValueWrappedJSVal(jsdc, jsdv);
        } else {
            *rval = JSVAL_VOID;
        }
    }
    
    return hook_rv;
}

JS_STATIC_DLL_CALLBACK (void)
jsds_ScriptHookProc (JSDContext* jsdc, JSDScript* jsdscript, JSBool creating,
                     void* callerdata)
{
#ifdef CAUTIOUS_SCRIPTHOOK
    JSContext *cx = JSD_GetDefaultJSContext(jsdc);
    JSRuntime *rt = JS_GetRuntime(cx);
#endif

    nsCOMPtr<jsdIScriptHook> hook;
    gJsds->GetScriptHook (getter_AddRefs(hook));
    
    if (creating) {
        /* a script is being created */
        if (!hook) {
            /* nobody cares, just exit */
            return;
        }
            
        nsCOMPtr<jsdIScript> script = 
            getter_AddRefs(jsdScript::FromPtr(jsdc, jsdscript));
#ifdef CAUTIOUS_SCRIPTHOOK
        JS_UNKEEP_ATOMS(rt);
#endif
        gJsds->Pause(nsnull);
        hook->OnScriptCreated (script);
        gJsds->UnPause(nsnull);
#ifdef CAUTIOUS_SCRIPTHOOK
        JS_KEEP_ATOMS(rt);
#endif
    } else {
        /* a script is being destroyed.  even if there is no registered hook
         * we'll still need to invalidate the jsdIScript record, in order
         * to remove the reference held in the JSDScript private data. */
        nsCOMPtr<jsdIScript> jsdis = 
            NS_STATIC_CAST(jsdIScript *, JSD_GetScriptPrivate(jsdscript));
        if (!jsdis)
            return;
        
        jsdis->Invalidate();
        if (!hook)
            return;
        
        if (gGCStatus == JSGC_END) {
            /* if GC *isn't* running, we can tell the user about the script
             * delete now. */
#ifdef CAUTIOUS_SCRIPTHOOK
            JS_UNKEEP_ATOMS(rt);
#endif
                
            gJsds->Pause(nsnull);
            hook->OnScriptDestroyed (jsdis);
            gJsds->UnPause(nsnull);
#ifdef CAUTIOUS_SCRIPTHOOK
            JS_KEEP_ATOMS(rt);
#endif
        } else {
            /* if a GC *is* running, we've got to wait until it's done before
             * we can execute any JS, so we queue the notification in a PRCList
             * until GC tells us it's done. See jsds_GCCallbackProc(). */
            DeadScript *ds = PR_NEW(DeadScript);
            if (!ds)
                return; /* NS_ERROR_OUT_OF_MEMORY */
        
            ds->jsdc = jsdc;
            ds->script = jsdis;
            NS_ADDREF(ds->script);
            if (gDeadScripts)
                /* if the queue exists, add to it */
                PR_APPEND_LINK(&ds->links, &gDeadScripts->links);
            else {
                /* otherwise create the queue */
                PR_INIT_CLIST(&ds->links);
                gDeadScripts = ds;
            }
        }
    }            
}

/*******************************************************************************
 * reflected jsd data structures
 *******************************************************************************/

/* Contexts */
/*
NS_IMPL_THREADSAFE_ISUPPORTS1(jsdContext, jsdIContext); 

NS_IMETHODIMP
jsdContext::GetJSDContext(JSDContext **_rval)
{
    *_rval = mCx;
    return NS_OK;
}
*/

/* Objects */
NS_IMPL_THREADSAFE_ISUPPORTS1(jsdObject, jsdIObject)

NS_IMETHODIMP
jsdObject::GetJSDContext(JSDContext **_rval)
{
    *_rval = mCx;
    return NS_OK;
}

NS_IMETHODIMP
jsdObject::GetJSDObject(JSDObject **_rval)
{
    *_rval = mObject;
    return NS_OK;
}

NS_IMETHODIMP
jsdObject::GetCreatorURL(char **_rval)
{
    const char *url = JSD_GetObjectNewURL(mCx, mObject);
    if (url) {
        *_rval = PL_strdup(url);
        if (!*_rval)
            return NS_ERROR_OUT_OF_MEMORY;
    } else {
        *_rval = nsnull;
    }
    return NS_OK;
}

NS_IMETHODIMP
jsdObject::GetCreatorLine(PRUint32 *_rval)
{
    *_rval = JSD_GetObjectNewLineNumber(mCx, mObject);
    return NS_OK;
}

NS_IMETHODIMP
jsdObject::GetConstructorURL(char **_rval)
{
    const char *url = JSD_GetObjectConstructorURL(mCx, mObject);
    if (url) {
        *_rval = PL_strdup(url);
        if (!*_rval)
            return NS_ERROR_OUT_OF_MEMORY;
    } else {
        *_rval = nsnull;
    }
    return NS_OK;
}

NS_IMETHODIMP
jsdObject::GetConstructorLine(PRUint32 *_rval)
{
    *_rval = JSD_GetObjectConstructorLineNumber(mCx, mObject);
    return NS_OK;
}

NS_IMETHODIMP
jsdObject::GetValue(jsdIValue **_rval)
{
    JSDValue *jsdv = JSD_GetValueForObject (mCx, mObject);
    
    *_rval = jsdValue::FromPtr (mCx, jsdv);
    return NS_OK;
}

/* Properties */
NS_IMPL_THREADSAFE_ISUPPORTS2(jsdProperty, jsdIProperty, jsdIEphemeral)

jsdProperty::jsdProperty (JSDContext *aCx, JSDProperty *aProperty) :
    mCx(aCx), mProperty(aProperty)
{
    DEBUG_CREATE ("jsdProperty", gPropertyCount);
    mValid = (aCx && aProperty);
    mLiveListEntry.value = this;
    jsds_InsertEphemeral (&gLiveProperties, &mLiveListEntry);
}

jsdProperty::~jsdProperty () 
{
    DEBUG_DESTROY ("jsdProperty", gPropertyCount);
    if (mValid)
        Invalidate();
}

NS_IMETHODIMP
jsdProperty::Invalidate()
{
    ASSERT_VALID_EPHEMERAL;
    mValid = PR_FALSE;
    jsds_RemoveEphemeral (&gLiveProperties, &mLiveListEntry);
    JSD_DropProperty (mCx, mProperty);
    return NS_OK;
}

void
jsdProperty::InvalidateAll()
{
    if (gLiveProperties)
        jsds_InvalidateAllEphemerals (&gLiveProperties);
}

NS_IMETHODIMP
jsdProperty::GetJSDContext(JSDContext **_rval)
{
    *_rval = mCx;
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetJSDProperty(JSDProperty **_rval)
{
    *_rval = mProperty;
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetIsValid(PRBool *_rval)
{
    *_rval = mValid;
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetAlias(jsdIValue **_rval)
{
    JSDValue *jsdv = JSD_GetPropertyValue (mCx, mProperty);
    
    *_rval = jsdValue::FromPtr (mCx, jsdv);
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetFlags(PRUint32 *_rval)
{
    *_rval = JSD_GetPropertyFlags (mCx, mProperty);
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetName(jsdIValue **_rval)
{
    JSDValue *jsdv = JSD_GetPropertyName (mCx, mProperty);
    
    *_rval = jsdValue::FromPtr (mCx, jsdv);
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetValue(jsdIValue **_rval)
{
    JSDValue *jsdv = JSD_GetPropertyValue (mCx, mProperty);
    
    *_rval = jsdValue::FromPtr (mCx, jsdv);
    return NS_OK;
}

NS_IMETHODIMP
jsdProperty::GetVarArgSlot(PRUint32 *_rval)
{
    *_rval = JSD_GetPropertyVarArgSlot (mCx, mProperty);
    return NS_OK;
}

/* Scripts */
NS_IMPL_THREADSAFE_ISUPPORTS2(jsdScript, jsdIScript, jsdIEphemeral)

jsdScript::jsdScript (JSDContext *aCx, JSDScript *aScript) : mValid(PR_FALSE),
                                                             mTag(0),
                                                             mCx(aCx),
                                                             mScript(aScript),
                                                             mFileName(0), 
                                                             mFunctionName(0),
                                                             mBaseLineNumber(0),
                                                             mLineExtent(0),
                                                             mPPLineMap(0),
                                                             mFirstPC(0)
{
    DEBUG_CREATE ("jsdScript", gScriptCount);

    if (mScript) {
        /* copy the script's information now, so we have it later, when it
         * gets destroyed. */
        JSD_LockScriptSubsystem(mCx);
        mFileName = new nsCString(JSD_GetScriptFilename(mCx, mScript));
        mFunctionName =
            new nsCString(JSD_GetScriptFunctionName(mCx, mScript));
        mBaseLineNumber = JSD_GetScriptBaseLineNumber(mCx, mScript);
        mLineExtent = JSD_GetScriptLineExtent(mCx, mScript);
        mFirstPC = JSD_GetClosestPC(mCx, mScript, 0);
        JSD_UnlockScriptSubsystem(mCx);
        
        mValid = PR_TRUE;
    }
}

jsdScript::~jsdScript () 
{
    DEBUG_DESTROY ("jsdScript", gScriptCount);
    if (mFileName)
        delete mFileName;
    if (mFunctionName)
        delete mFunctionName;

    if (mPPLineMap)
        PR_Free(mPPLineMap);

    /* Invalidate() needs to be called to release an owning reference to
     * ourselves, so if we got here without being invalidated, something
     * has gone wrong with our ref count. */
    NS_ASSERTION (!mValid, "Script destroyed without being invalidated.");
}

/*
 * This method populates a line <-> pc map for a pretty printed version of this
 * script.  It does this by decompiling, and then recompiling the script.  The
 * resulting script is scanned for the line map, and then left as GC fodder.
 */
PCMapEntry *
jsdScript::CreatePPLineMap()
{    
    JSContext  *cx  = JSD_GetDefaultJSContext (mCx);
    JSObject   *obj = JS_NewObject(cx, NULL, NULL, NULL);
    JSFunction *fun = JSD_GetJSFunction (mCx, mScript);
    JSScript   *script;
    PRUint32    baseLine;
    PRBool      scriptOwner = PR_FALSE;
    
    if (fun) {
        if (fun->nargs > 12)
            return nsnull;
        JSString *jsstr = JS_DecompileFunctionBody (cx, fun, 4);
        if (!jsstr)
            return nsnull;
    
        const char *argnames[] = {"arg1", "arg2", "arg3", "arg4", 
                                  "arg5", "arg6", "arg7", "arg8",
                                  "arg9", "arg10", "arg11", "arg12" };
        fun = JS_CompileUCFunction (cx, obj, "ppfun", fun->nargs, argnames,
                                    JS_GetStringChars(jsstr),
                                    JS_GetStringLength(jsstr),
                                    "x-jsd:ppbuffer?type=function", 3);
        if (!fun || !(script = JS_GetFunctionScript(cx, fun)))
            return nsnull;
        baseLine = 3;
    } else {
        JSString *jsstr = JS_DecompileScript (cx, JSD_GetJSScript(mCx, mScript),
                                              "ppscript", 4);
        if (!jsstr)
            return nsnull;

        script = JS_CompileUCScript (cx, obj,
                                     JS_GetStringChars(jsstr),
                                     JS_GetStringLength(jsstr),
                                     "x-jsd:ppbuffer?type=script", 1);
        if (!script)
            return nsnull;
        scriptOwner = PR_TRUE;
        baseLine = 1;
    }
        
    PRUint32 scriptExtent = JS_GetScriptLineExtent (cx, script);
    jsbytecode