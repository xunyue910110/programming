/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=4 sw=4 et tw=78:
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
 * The Original Code is SpiderMonkey E4X code, released August, 2004.
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

#include "jsstddef.h"
#include "jsconfig.h"

#if JS_HAS_XML_SUPPORT

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsbit.h"
#include "jsprf.h"
#include "jsutil.h"
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jsbool.h"
#include "jscntxt.h"
#include "jsfun.h"
#include "jsgc.h"
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
#include "jsxml.h"

#ifdef DEBUG
#include <string.h>     /* for #ifdef DEBUG memset calls */
#endif

/*
 * NOTES
 * - in the js shell, you must use the -x command line option, or call
 *   options('xml') before compiling anything that uses XML literals
 *
 * TODO
 * - XXXbe patrol
 * - Fuse objects and their JSXML* private data into single GC-things
 * - fix function::foo vs. x.(foo == 42) collision using proper namespacing
 * - fix the !TCF_HAS_DEFXMLNS optimization in js_FoldConstants
 * - JSCLASS_DOCUMENT_OBSERVER support -- live two-way binding to Gecko's DOM!
 * - JS_TypeOfValue sure could use a cleaner interface to "types"
 */

#ifdef DEBUG_brendan
#define METERING        1
#endif

#ifdef METERING
static struct {
    jsrefcount  qname;
    jsrefcount  qnameobj;
    jsrefcount  liveqname;
    jsrefcount  liveqnameobj;
    jsrefcount  namespace;
    jsrefcount  namespaceobj;
    jsrefcount  livenamespace;
    jsrefcount  livenamespaceobj;
    jsrefcount  xml;
    jsrefcount  xmlobj;
    jsrefcount  livexml;
    jsrefcount  livexmlobj;
} xml_stats;

#define METER(x)        JS_ATOMIC_INCREMENT(&(x))
#define UNMETER(x)      JS_ATOMIC_DECREMENT(&(x))
#else
#define METER(x)        /* nothing */
#define UNMETER(x)      /* nothing */
#endif

/*
 * Random utilities and global functions.
 */
const char js_AnyName_str[]       = "AnyName";
const char js_AttributeName_str[] = "AttributeName";
const char js_isXMLName_str[]     = "isXMLName";
const char js_XMLList_str[]       = "XMLList";
const char js_localName_str[]     = "localName";
const char js_xml_parent_str[]    = "parent";
const char js_prefix_str[]        = "prefix";
const char js_toXMLString_str[]   = "toXMLString";
const char js_uri_str[]           = "uri";

const char js_amp_entity_str[]    = "&amp;";
const char js_gt_entity_str[]     = "&gt;";
const char js_lt_entity_str[]     = "&lt;";
const char js_quot_entity_str[]   = "&quot;";

#define IS_EMPTY(str) (JSSTRING_LENGTH(str) == 0)
#define IS_STAR(str)  (JSSTRING_LENGTH(str) == 1 && *JSSTRING_CHARS(str) == '*')

static JSBool
xml_isXMLName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    *rval = BOOLEAN_TO_JSVAL(js_IsXMLName(cx, argv[0]));
    return JS_TRUE;
}

/*
 * Namespace class and library functions.
 */
enum namespace_tinyid {
    NAMESPACE_PREFIX = -1,
    NAMESPACE_URI = -2
};

static JSBool
namespace_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSXMLNamespace *ns;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;

    ns = (JSXMLNamespace *)
         JS_GetInstancePrivate(cx, obj, &js_NamespaceClass.base, NULL);
    if (!ns)
        return JS_TRUE;

    switch (JSVAL_TO_INT(id)) {
      case NAMESPACE_PREFIX:
        *vp = ns->prefix ? STRING_TO_JSVAL(ns->prefix) : JSVAL_VOID;
        break;
      case NAMESPACE_URI:
        *vp = STRING_TO_JSVAL(ns->uri);
        break;
    }
    return JS_TRUE;
}

static void
namespace_finalize(JSContext *cx, JSObject *obj)
{
    JSXMLNamespace *ns;
    JSRuntime *rt;

    ns = (JSXMLNamespace *) JS_GetPrivate(cx, obj);
    if (!ns)
        return;
    JS_ASSERT(ns->object == obj);
    ns->object = NULL;
    UNMETER(xml_stats.livenamespaceobj);

    rt = cx->runtime;
    if (rt->functionNamespaceObject == obj)
        rt->functionNamespaceObject = NULL;
}

static void
namespace_mark_vector(JSContext *cx, JSXMLNamespace **vec, uint32 len,
                      void *arg)
{
    uint32 i;
    JSXMLNamespace *ns;

    for (i = 0; i < len; i++) {
        ns = vec[i];
        {
#ifdef GC_MARK_DEBUG
            char buf[100];

            JS_snprintf(buf, sizeof buf, "%s=%s",
                        ns->prefix ? JS_GetStringBytes(ns->prefix) : "",
                        JS_GetStringBytes(ns->uri));
#else
            const char *buf = NULL;
#endif
            JS_MarkGCThing(cx, ns, buf, arg);
        }
    }
}

static uint32
namespace_mark(JSContext *cx, JSObject *obj, void *arg)
{
    JSXMLNamespace *ns;

    ns = (JSXMLNamespace *) JS_GetPrivate(cx, obj);
    JS_MarkGCThing(cx, ns, js_private_str, arg);
    return 0;
}

static JSBool
namespace_equality(JSContext *cx, JSObject *obj, jsval v, JSBool *bp)
{
    JSXMLNamespace *ns, *ns2;
    JSObject *obj2;

    ns = (JSXMLNamespace *) JS_GetPrivate(cx, obj);
    JS_ASSERT(JSVAL_IS_OBJECT(v));
    obj2 = JSVAL_TO_OBJECT(v);
    if (!obj2 || OBJ_GET_CLASS(cx, obj2) != &js_NamespaceClass.base) {
        *bp = JS_FALSE;
    } else {
        ns2 = (JSXMLNamespace *) JS_GetPrivate(cx, obj2);
        *bp = !js_CompareStrings(ns->uri, ns2->uri);
    }
    return JS_TRUE;
}

JS_FRIEND_DATA(JSExtendedClass) js_NamespaceClass = {
  { "Namespace",
    JSCLASS_HAS_PRIVATE | JSCLASS_CONSTRUCT_PROTOTYPE | JSCLASS_IS_EXTENDED,
    JS_PropertyStub,   JS_PropertyStub,   namespace_getProperty, NULL,
    JS_EnumerateStub,  JS_ResolveStub,    JS_ConvertStub,    namespace_finalize,
    NULL,              NULL,              NULL,              NULL,
    NULL,              NULL,              namespace_mark,    NULL },
    namespace_equality,
    NULL, NULL,
    JSCLASS_NO_RESERVED_MEMBERS
};

#define NAMESPACE_ATTRS                                                       \
    (JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_SHARED)

static JSPropertySpec namespace_props[] = {
    {js_prefix_str,    NAMESPACE_PREFIX,  NAMESPACE_ATTRS,   0, 0},
    {js_uri_str,       NAMESPACE_URI,     NAMESPACE_ATTRS,   0, 0},
    {0,0,0,0,0}
};

static JSBool
namespace_toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                   jsval *rval)
{
    JSXMLNamespace *ns;

    ns = (JSXMLNamespace *)
         JS_GetInstancePrivate(cx, obj, &js_NamespaceClass.base, argv);
    if (!ns)
        return JS_FALSE;

    *rval = STRING_TO_JSVAL(ns->uri);
    return JS_TRUE;
}

static JSFunctionSpec namespace_methods[] = {
    {js_toString_str,  namespace_toString,        0,0,0},
    {0,0,0,0,0}
};

JSXMLNamespace *
js_NewXMLNamespace(JSContext *cx, JSString *prefix, JSString *uri,
                   JSBool declared)
{
    JSXMLNamespace *ns;

    ns = (JSXMLNamespace *)
         js_NewGCThing(cx, GCX_NAMESPACE, sizeof(JSXMLNamespace));
    if (!ns)
        return NULL;
    ns->object = NULL;
    ns->prefix = prefix;
    ns->uri = uri;
    ns->declared = declared;
    METER(xml_stats.namespace);
    METER(xml_stats.livenamespace);
    return ns;
}

void
js_MarkXMLNamespace(JSContext *cx, JSXMLNamespace *ns, void *arg)
{
    JS_MarkGCThing(cx, ns->object, js_object_str, arg);
    JS_MarkGCThing(cx, ns->prefix, js_prefix_str, arg);
    JS_MarkGCThing(cx, ns->uri, js_uri_str, arg);
}

void
js_FinalizeXMLNamespace(JSContext *cx, JSXMLNamespace *ns)
{
    UNMETER(xml_stats.livenamespace);
}

JSObject *
js_NewXMLNamespaceObject(JSContext *cx, JSString *prefix, JSString *uri,
                         JSBool declared)
{
    JSXMLNamespace *ns;

    ns = js_NewXMLNamespace(cx, prefix, uri, declared);
    if (!ns)
        return NULL;
    return js_GetXMLNamespaceObject(cx, ns);
}

JSObject *
js_GetXMLNamespaceObject(JSContext *cx, JSXMLNamespace *ns)
{
    JSObject *obj;

    obj = ns->object;
    if (obj) {
        JS_ASSERT(JS_GetPrivate(cx, obj) == ns);
        return obj;
    }
    obj = js_NewObject(cx, &js_NamespaceClass.base, NULL, NULL);
    if (!obj || !JS_SetPrivate(cx, obj, ns)) {
        cx->newborn[GCX_OBJECT] = NULL;
        return NULL;
    }
    ns->object = obj;
    METER(xml_stats.namespaceobj);
    METER(xml_stats.livenamespaceobj);
    return obj;
}

/*
 * QName class and library functions.
 */
enum qname_tinyid {
    QNAME_URI = -1,
    QNAME_LOCALNAME = -2
};

static JSBool
qname_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSXMLQName *qn;

    if (!JSVAL_IS_INT(id))
        return JS_TRUE;

    qn = (JSXMLQName *)
         JS_GetInstancePrivate(cx, obj, &js_QNameClass.base, NULL);
    if (!qn)
        return JS_TRUE;

    switch (JSVAL_TO_INT(id)) {
      case QNAME_URI:
        *vp = qn->uri ? STRING_TO_JSVAL(qn->uri) : JSVAL_NULL;
        break;
      case QNAME_LOCALNAME:
        *vp = STRING_TO_JSVAL(qn->localName);
        break;
    }
    return JS_TRUE;
}

static void
qname_finalize(JSContext *cx, JSObject *obj)
{
    JSXMLQName *qn;

    qn = (JSXMLQName *) JS_GetPrivate(cx, obj);
    if (!qn)
        return;
    JS_ASSERT(qn->object == obj);
    qn->object = NULL;
    UNMETER(xml_stats.liveqnameobj);
}

static void
anyname_finalize(JSContext* cx, JSObject* obj)
{
    JSRuntime *rt;

    /* Make sure the next call to js_GetAnyName doesn't try to use obj. */
    rt = cx->runtime;
    if (rt->anynameObject == obj)
        rt->anynameObject = NULL;

    qname_finalize(cx, obj);
}

static uint32
qname_mark(JSContext *cx, JSObject *obj, void *arg)
{
    JSXMLQName *qn;

    qn = (JSXMLQName *) JS_GetPrivate(cx, obj);
    JS_MarkGCThing(cx, qn, js_private_str, arg);
    return 0;
}

static JSBool
qname_identity(JSXMLQName *qna, JSXMLQName *qnb)
{
    if (!qna->uri ^ !qnb->uri)
        return JS_FALSE;
    if (qna->uri && js_CompareStrings(qna->uri, qnb->uri))
        return JS_FALSE;
    return !js_CompareStrings(qna->localName, qnb->localName);
}

static JSBool
qname_equality(JSContext *cx, JSObject *obj, jsval v, JSBool *bp)
{
    JSXMLQName *qn, *qn2;
    JSObject *obj2;

    qn = (JSXMLQName *) JS_GetPrivate(cx, obj);
    JS_ASSERT(JSVAL_IS_OBJECT(v));
    obj2 = JSVAL_TO_OBJECT(v);
    if (!obj2 || OBJ_GET_CLASS(cx, obj2) != &js_QNameClass.base) {
        *bp = JS_FALSE;
    } else {
        qn2 = (JSXMLQName *) JS_GetPrivate(cx, obj2);
        *bp = qname_identity(qn, qn2);
    }
    return JS_TRUE;
}

JS_FRIEND_DATA(JSExtendedClass) js_QNameClass = {
  { "QName",
    JSCLASS_HAS_PRIVATE | JSCLASS_CONSTRUCT_PROTOTYPE | JSCLASS_IS_EXTENDED,
    JS_PropertyStub,   JS_PropertyStub,   qname_getProperty, NULL,
    JS_EnumerateStub,  JS_ResolveStub,    JS_ConvertStub,    qname_finalize,
    NULL,              NULL,              NULL,              NULL,
    NULL,              NULL,              qname_mark,        NULL },
    qname_equality,
    NULL, NULL,
    JSCLASS_NO_RESERVED_MEMBERS
};

/*
 * Classes for the ECMA-357-internal types AttributeName and AnyName, which
 * are like QName, except that they have no property getters.  They share the
 * qname_toString method, and therefore are exposed as constructable objects
 * in this implementation.
 */
JS_FRIEND_DATA(JSClass) js_AttributeNameClass = {
    js_AttributeName_str, JSCLASS_HAS_PRIVATE | JSCLASS_CONSTRUCT_PROTOTYPE,
    JS_PropertyStub,   JS_PropertyStub,   JS_PropertyStub,   JS_PropertyStub,
    JS_EnumerateStub,  JS_ResolveStub,    JS_ConvertStub,    qname_finalize,
    NULL,              NULL,              NULL,              NULL,
    NULL,              NULL,              qname_mark,        NULL
};

JS_FRIEND_DATA(JSClass) js_AnyNameClass = {
    js_AnyName_str,    JSCLASS_HAS_PRIVATE | JSCLASS_CONSTRUCT_PROTOTYPE,
    JS_PropertyStub,   JS_PropertyStub,   JS_PropertyStub,   JS_PropertyStub,
    JS_EnumerateStub,  JS_ResolveStub,    JS_ConvertStub,    anyname_finalize,
    NULL,              NULL,              NULL,              NULL,
    NULL,              NULL,              qname_mark,        NULL
};

#define QNAME_ATTRS                                                           \
    (JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_SHARED)

static JSPropertySpec qname_props[] = {
    {js_uri_str,       QNAME_URI,         QNAME_ATTRS,       0, 0},
    {js_localName_str, QNAME_LOCALNAME,   QNAME_ATTRS,       0, 0},
    {0,0,0,0,0}
};

static JSBool
qname_toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
               jsval *rval)
{
    JSClass *clasp;
    JSXMLQName *qn;
    JSString *str, *qualstr;
    size_t length;
    jschar *chars;

    clasp = OBJ_GET_CLASS(cx, obj);
    if (clasp == &js_AttributeNameClass || clasp == &js_AnyNameClass) {
        qn = (JSXMLQName *) JS_GetPrivate(cx, obj);
    } else {
        qn = (JSXMLQName *)
             JS_GetInstancePrivate(cx, obj, &js_QNameClass.base, argv);
        if (!qn)
            return JS_FALSE;
    }

    if (!qn->uri) {
        /* No uri means wildcard qualifier. */
        str = ATOM_TO_STRING(cx->runtime->atomState.starQualifierAtom);
    } else if (IS_EMPTY(qn->uri)) {
        /* Empty string for uri means localName is in no namespace. */
        str = cx->runtime->emptyString;
    } else {
        qualstr = ATOM_TO_STRING(cx->runtime->atomState.qualifierAtom);
        str = js_ConcatStrings(cx, qn->uri, qualstr);
        if (!str)
            return JS_FALSE;
    }
    str = js_ConcatStrings(cx, str, qn->localName);
    if (!str)
        return JS_FALSE;

    if (str && clasp == &js_AttributeNameClass) {
        length = JSSTRING_LENGTH(str);
        chars = (jschar *) JS_malloc(cx, (length + 2) * sizeof(jschar));
        if (!chars)
            return JS_FALSE;
        *chars = '@';
        js_strncpy(chars + 1, JSSTRING_CHARS(str), length);
        chars[++length] = 0;
        str = js_NewString(cx, chars, length, 0);
        if (!str) {
            JS_free(cx, chars);
            return JS_FALSE;
        }
    }

    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

static JSFunctionSpec qname_methods[] = {
    {js_toString_str,  qname_toString,    0,0,0},
    {0,0,0,0,0}
};

JSXMLQName *
js_NewXMLQName(JSContext *cx, JSString *uri, JSString *prefix,
               JSString *localName)
{
    JSXMLQName *qn;

    qn = (JSXMLQName *) js_NewGCThing(cx, GCX_QNAME, sizeof(JSXMLQName));
    if (!qn)
        return NULL;
    qn->object = NULL;
    qn->uri = uri;
    qn->prefix = prefix;
    qn->localName = localName;
    METER(xml_stats.qname);
    METER(xml_stats.liveqname);
    return qn;
}

void
js_MarkXMLQName(JSContext *cx, JSXMLQName *qn, void *arg)
{
    JS_MarkGCThing(cx, qn->object, js_object_str, arg);
    JS_MarkGCThing(cx, qn->uri, js_uri_str, arg);
    JS_MarkGCThing(cx, qn->prefix, js_prefix_str, arg);
    JS_MarkGCThing(cx, qn->localName, js_localName_str, arg);
}

void
js_FinalizeXMLQName(JSContext *cx, JSXMLQName *qn)
{
    UNMETER(xml_stats.liveqname);
}

JSObject *
js_NewXMLQNameObject(JSContext *cx, JSString *uri, JSString *prefix,
                     JSString *localName)
{
    JSXMLQName *qn;

    qn = js_NewXMLQName(cx, uri, prefix, localName);
    if (!qn)
        return NULL;
    return js_GetXMLQNameObject(cx, qn);
}

JSObject *
js_GetXMLQNameObject(JSContext *cx, JSXMLQName *qn)
{
    JSObject *obj;

    obj = qn->object;
    if (obj) {
        JS_ASSERT(JS_GetPrivate(cx, obj) == qn);
        return obj;
    }
    obj = js_NewObject(cx, &js_QNameClass.base, NULL, NULL);
    if (!obj || !JS_SetPrivate(cx, obj, qn)) {
        cx->newborn[GCX_OBJECT] = NULL;
        return NULL;
    }
    qn->object = obj;
    METER(xml_stats.qnameobj);
    METER(xml_stats.liveqnameobj);
    return obj;
}

JSObject *
js_GetAttributeNameObject(JSContext *cx, JSXMLQName *qn)
{
    JSObject *obj;

    obj = qn->object;
    if (obj) {
        if (OBJ_GET_CLASS(cx, obj) == &js_AttributeNameClass)
            return obj;
        qn = js_NewXMLQName(cx, qn->uri, qn->prefix, qn->localName);
        if (!qn)
            return NULL;
    }

    obj = js_NewObject(cx, &js_AttributeNameClass, NULL, NULL);
    if (!obj || !JS_SetPrivate(cx, obj, qn)) {
        cx->newborn[GCX_OBJECT] = NULL;
        return NULL;
    }

    qn->object = obj;
    METER(xml_stats.qnameobj);
    METER(xml_stats.liveqnameobj);
    return obj;
}

JSObject *
js_ConstructXMLQNameObject(JSContext *cx, jsval nsval, jsval lnval)
{
    jsval argv[2];

    /*
     * ECMA-357 11.1.2,
     * The _QualifiedIdentifier : PropertySelector :: PropertySelector_
     * production, step 2.
     */
    if (!JSVAL_IS_PRIMITIVE(nsval) &&
        OBJ_GET_CLASS(cx, JSVAL_TO_OBJECT(nsval)) == &js_AnyNameClass) {
        nsval = JSVAL_NULL;
    }

    argv[0] = nsval;
    argv[1] = lnval;
    return js_ConstructObject(cx, &js_QNameClass.base, NULL, NULL, 2, argv);
}

static JSBool
IsXMLName(const jschar *cp, size_t n)
{
    JSBool rv;
    jschar c;

    rv = JS_FALSE;
    if (n != 0 && JS_ISXMLNSSTART(*cp)) {
        while (--n != 0) {
            c = *++cp;
            if (!JS_ISXMLNS(c))
                return rv;
        }
        rv = JS_TRUE;
    }
    return rv;
}

JSBool
js_IsXMLName(JSContext *cx, jsval v)
{
    JSClass *clasp;
    JSXMLQName *qn;
    JSString *name;
    JSErrorReporter older;

    /*
     * Inline specialization of the QName constructor called with v passed as
     * the only argument, to compute the localName for the constructed qname,
     * without actually allocating the object or computing its uri and prefix.
     * See ECMA-357 13.1.2.1 step 1 and 13.3.2.
     */
    if (!JSVAL_IS_PRIMITIVE(v) &&
        (clasp = OBJ_GET_CLASS(cx, JSVAL_TO_OBJECT(v)),
         clasp == &js_QNameClass.base ||
         clasp == &js_AttributeNameClass ||
         clasp == &js_AnyNameClass)) {
        qn = (JSXMLQName *) JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
        name = qn->localName;
    } else {
        older = JS_SetErrorReporter(cx, NULL);
        name = js_ValueToString(cx, v);
        JS_SetErrorReporter(cx, older);
        if (!name) {
            JS_ClearPendingException(cx);
            return JS_FALSE;
        }
    }

    return IsXMLName(JSSTRING_CHARS(name), JSSTRING_LENGTH(name));
}

static JSBool
Namespace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval urival, prefixval;
    JSObject *uriobj;
    JSBool isNamespace, isQName;
    JSClass *clasp;
    JSString *empty, *prefix;
    JSXMLNamespace *ns, *ns2;
    JSXMLQName *qn;

    urival = argv[argc > 1];
    isNamespace = isQName = JS_FALSE;
    if (!JSVAL_IS_PRIMITIVE(urival)) {
        uriobj = JSVAL_TO_OBJECT(urival);
        clasp = OBJ_GET_CLASS(cx, uriobj);
        isNamespace = (clasp == &js_NamespaceClass.base);
        isQName = (clasp == &js_QNameClass.base);
    }
#ifdef __GNUC__         /* suppress bogus gcc warnings */
    else uriobj = NULL;
#endif

    if (!(cx->fp->flags & JSFRAME_CONSTRUCTING)) {
        /* Namespace called as function. */
        if (argc == 1 && isNamespace) {
            /* Namespace called with one Namespace argument is identity. */
            *rval = urival;
            return JS_TRUE;
        }

        /* Create and return a new QName object exactly as if constructed. */
        obj = js_NewObject(cx, &js_NamespaceClass.base, NULL, NULL);
        if (!obj)
            return JS_FALSE;
        *rval = OBJECT_TO_JSVAL(obj);
    }
    METER(xml_stats.namespaceobj);
    METER(xml_stats.livenamespaceobj);

    /*
     * Create and connect private data to rooted obj early, so we don't have
     * to worry about rooting string newborns hanging off of the private data
     * further below.
     */
    empty = cx->runtime->emptyString;
    ns = js_NewXMLNamespace(cx, empty, empty, JS_FALSE);
    if (!ns)
        return JS_FALSE;
    if (!JS_SetPrivate(cx, obj, ns))
        return JS_FALSE;
    ns->object = obj;

    if (argc == 1) {
        if (isNamespace) {
            ns2 = (JSXMLNamespace *) JS_GetPrivate(cx, uriobj);
            ns->uri = ns2->uri;
            ns->prefix = ns2->prefix;
        } else if (isQName &&
                   (qn = (JSXMLQName *) JS_GetPrivate(cx, uriobj))->uri) {
            ns->uri = qn->uri;
            ns->prefix = qn->prefix;
        } else {
            ns->uri = js_ValueToString(cx, urival);
            if (!ns->uri)
                return JS_FALSE;

            /* NULL here represents *undefined* in ECMA-357 13.2.2 3(c)iii. */
            if (!IS_EMPTY(ns->uri))
                ns->prefix = NULL;
        }
    } else if (argc == 2) {
        if (isQName &&
            (qn = (JSXMLQName *) JS_GetPrivate(cx, uriobj))->uri) {
            ns->uri = qn->uri;
        } else {
            ns->uri = js_ValueToString(cx, urival);
            if (!ns->uri)
                return JS_FALSE;
        }

        prefixval = argv[0];
        if (IS_EMPTY(ns->uri)) {
            if (!JSVAL_IS_VOID(prefixval)) {
                prefix = js_ValueToString(cx, prefixval);
                if (!prefix)
                    return JS_FALSE;
                if (!IS_EMPTY(prefix)) {
                    JS_ReportErrorNumber(cx, js_GetErrorMessage, NULL,
                                         JSMSG_BAD_XML_NAMESPACE,
                                         js_ValueToPrintableString(cx,
                                             STRING_TO_JSVAL(prefix)));
                    return JS_FALSE;
                }
            }
        } else if (JSVAL_IS_VOID(prefixval) || !js_IsXMLName(cx, prefixval)) {
            /* NULL here represents *undefined* in ECMA-357 13.2.2 4(d) etc. */
            ns->prefix = NULL;
        } else {
            prefix = js_ValueToString(cx, prefixval);
            if (!prefix)
                return JS_FALSE;
            ns->prefix = prefix;
        }
    }

    return JS_TRUE;
}

static JSBool
QName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval nameval, nsval;
    JSBool isQName, isNamespace;
    JSXMLQName *qn;
    JSString *uri, *prefix, *name;
    JSObject *nsobj;
    JSClass *clasp;
    JSXMLNamespace *ns;

    nameval = argv[argc > 1];
    isQName =
        !JSVAL_IS_PRIMITIVE(nameval) &&
        OBJ_GET_CLASS(cx, JSVAL_TO_OBJECT(nameval)) == &js_QNameClass.base;

    if (!(cx->fp->flags & JSFRAME_CONSTRUCTING)) {
        /* QName called as function. */
        if (argc == 1 && isQName) {
            /* QName called with one QName argument is identity. */
            *rval = nameval;
            return JS_TRUE;
        }

        /*
         * Create and return a new QName object exactly as if constructed.
         * Use the constructor's clasp so we can be shared by AttributeName
         * (see below after this function).
         */
        obj = js_NewObject(cx,
                           argv
                           ? JS_ValueToFunction(cx, argv[-2])->clasp
                           : &js_QNameClass.base,
                           NULL, NULL);
        if (!obj)
            return JS_FALSE;
        *rval = OBJECT_TO_JSVAL(obj);
    }
    METER(xml_stats.qnameobj);
    METER(xml_stats.liveqnameobj);

    if (isQName) {
        /* If namespace is not specified and name is a QName, clone it. */
        qn = (JSXMLQName *) JS_GetPrivate(cx, JSVAL_TO_OBJECT(nameval));
        if (argc == 1) {
            uri = qn->uri;
            prefix = qn->prefix;
            name = qn->localName;
            goto out;
        }

        /* Namespace and qname were passed -- use the qname's localName. */
        nameval = STRING_TO_JSVAL(qn->localName);
    }

    if (argc == 0) {
        name = cx->runtime->emptyString;
    } else {
        name = js_ValueToString(cx, nameval);
        if (!name)
            return JS_FALSE;

        /* Use argv[1] as a local root for name, even if it was not passed. */
        argv[1] = STRING_TO_JSVAL(name);
    }

    nsval = argv[0];
    if (argc == 1 || JSVAL_IS_VOID(nsval)) {
        if (IS_STAR(name)) {
            nsval = JSVAL_NULL;
        } else {
            if (!js_GetDefaultXMLNamespace(cx, &nsval))
                return JS_FALSE;
        }
    }

    if (JSVAL_IS_NULL(nsval)) {
        /* NULL prefix represents *undefined* in ECMA-357 13.3.2 5(a). */
        uri = prefix = NULL;
    } else {
        /*
         * Inline specialization of the Namespace constructor called with
         * nsval passed as the only argument, to compute the uri and prefix
         * for the constructed namespace, without actually allocating the
         * object or computing other members.  See ECMA-357 13.3.2 6(a) and
         * 13.2.2.
         */
        isNamespace = isQName = JS_FALSE;
        if (!JSVAL_IS_PRIMITIVE(nsval)) {
            nsobj = JSVAL_TO_OBJECT(nsval);
            clasp = OBJ_GET_CLASS(cx, nsobj);
            isNamespace = (clasp == &js_NamespaceClass.base);
            isQName = (clasp == &js_QNameClass.base);
        }
#ifdef __GNUC__         /* suppress bogus gcc warnings */
        else nsobj = NULL;
#endif

        if (isNamespace) {
            ns = (JSXMLNamespace *) JS_GetPrivate(cx, nsobj);
            uri = ns->uri;
            prefix = ns->prefix;
        } else if (isQName &&
                   (qn = (JSXMLQName *) JS_GetPrivate(cx, nsobj))->uri) {
            uri = qn->uri;
            prefix = qn->prefix;
        } else {
            uri = js_ValueToString(cx, nsval);
            if (!uri)
                return JS_FALSE;
            argv[0] = STRING_TO_JSVAL(uri);     /* local root */

            /* NULL here represents *undefined* in ECMA-357 13.2.2 3(c)iii. */
            prefix = IS_EMPTY(uri) ? cx->runtime->emptyString : NULL;
        }
    }

out:
    qn = js_NewXMLQName(cx, uri, prefix, name);
    if (!qn)
        return JS_FALSE;
    if (!JS_SetPrivate(cx, obj, qn))
        return JS_FALSE;
    qn->object = obj;
    return JS_TRUE;
}

static JSBool
AttributeName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
              jsval *rval)
{
    /*
     * Since js_AttributeNameClass was initialized, obj will have that as its
     * class, not js_QNameClass.
     */
    return QName(cx, obj, argc, argv, rval);
}

/*
 * XMLArray library functions.
 */
static JSBool
namespace_identity(const void *a, const void *b)
{
    const JSXMLNamespace *nsa = (const JSXMLNamespace *) a;
    const JSXMLNamespace *nsb = (const JSXMLNamespace *) b;

    if (nsa->prefix && nsb->prefix) {
        if (js_CompareStrings(nsa->prefix, nsb->prefix))
            return JS_FALSE;
    } else {
        if (nsa->prefix || nsb->prefix)
            return JS_FALSE;
    }
    return !js_CompareStrings(nsa->uri, nsb->uri);
}

static JSBool
attr_identity(const void *a, const void *b)
{
    const JSXML *xmla = (const JSXML *) a;
    const JSXML *xmlb = (const JSXML *) b;

    return qname_identity(xmla->name, xmlb->name);
}

static void
XMLArrayCursorInit(JSXMLArrayCursor *cursor, JSXMLArray *array)
{
    JSXMLArrayCursor *next;

    cursor->array = array;
    cursor->index = 0;
    next = cursor->next = array->cursors;
    if (next)
        next->prevp = &cursor->next;
    cursor->prevp = &array->cursors;
    array->cursors = cursor;
    cursor->root = NULL;
}

static void
XMLArrayCursorFinish(JSXMLArrayCursor *cursor)
{
    JSXMLArrayCursor *next;

    if (!cursor->array)
        return;
    next = cursor->next;
    if (next)
        next->prevp = cursor->prevp;
    *cursor->prevp = next;
    cursor->array = NULL;
}

static void *
XMLArrayCursorNext(JSXMLArrayCursor *cursor)
{
    JSXMLArray *array;

    array = cursor->array;
    if (!array || cursor->index >= array->length)
        return NULL;
    return cursor->root = array->vector[cursor->index++];
}

static void *
XMLArrayCursorItem(JSXMLArrayCursor *cursor)
{
    JSXMLArray *array;

    array = cursor->array;
    if (!array || cursor->index >= array->length)
        return NULL;
    return cursor->root = array->vector[cursor->index];
}

static void
XMLArrayCursorMark(JSContext *cx, JSXMLArrayCursor *cursor)
{
    while (cursor) {
        GC_MARK(cx, cursor->root, "cursor->root", NULL);
        cursor = cursor->next;
    }
}

/* NB: called with null cx from the GC, via xml_mark => XMLArrayTrim. */
static JSBool
XMLArraySetCapacity(JSContext *cx, JSXMLArray *array, uint32 capacity)
{
    void **vector;

    if (capacity == 0) {
        /* We could let realloc(p, 0) free this, but purify gets confused. */
        if (array->vector)
            free(array->vector);
        vector = NULL;
    } else {
        if ((size_t)capacity > ~(size_t)0 / sizeof(void *) ||
            !(vector = (void **)
                       realloc(array->vector, capacity * sizeof(void *)))) {
            if (cx)
                JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
    }
    array->capacity = JSXML_PRESET_CAPACITY | capacity;
    array->vector = vector;
    return JS_TRUE;
}

static void
XMLArrayTrim(JSXMLArray *array)
{
    if (array->capacity & JSXML_PRESET_CAPACITY)
        return;
    if (array->length < array->capacity)
        XMLArraySetCapacity(NULL, array, array->length);
}

static JSBool
XMLArrayInit(JSContext *cx, JSXMLArray *array, uint32 capacity)
{
    array->length = array->capacity = 0;
    array->vector = NULL;
    array->cursors = NULL;
    return capacity == 0 || XMLArraySetCapacity(cx, array, capacity);
}

static void
XMLArrayFinish(JSContext *cx, JSXMLArray *array)
{
    JSXMLArrayCursor *cursor;

    JS_free(cx, array->vector);

    while ((cursor = array->cursors) != NULL)
        XMLArrayCursorFinish(cursor);

#ifdef DEBUG
    memset(array, 0xd5, sizeof *array);
#endif
}

#define XML_NOT_FOUND   ((uint32) -1)

static uint32
XMLArrayFindMember(const JSXMLArray *array, void *elt, JSIdentityOp identity)
{
    void **vector;
    uint32 i, n;

    /* The identity op must not reallocate array->vector. */
    vector = array->vector;
    if (identity) {
        for (i = 0, n = array->length; i < n; i++) {
            if (identity(vector[i], elt))
                return i;
        }
    } else {
        for (i = 0, n = array->length; i < n; i++) {
            if (vector[i] == elt)
                return i;
        }
    }
    return XML_NOT_FOUND;
}

/*
 * Grow array vector capacity by powers of two to LINEAR_THRESHOLD, and after
 * that, grow by LINEAR_INCREMENT.  Both must be powers of two, and threshold
 * should be greater than increment.
 */
#define LINEAR_THRESHOLD        256
#define LINEAR_INCREMENT        32

static JSBool
XMLArrayAddMember(JSContext *cx, JSXMLArray *array, uint32 index, void *elt)
{
    uint32 capacity, i;
    int log2;
    void **vector;

    if (index >= array->length) {
        if (index >= JSXML_CAPACITY(array)) {
            /* Arrange to clear JSXML_PRESET_CAPACITY from array->capacity. */
            capacity = index + 1;
            if (index >= LINEAR_THRESHOLD) {
                capacity = JS_ROUNDUP(capacity, LINEAR_INCREMENT);
            } else {
                JS_CEILING_LOG2(log2, capacity);
                capacity = JS_BIT(log2);
            }
            if ((size_t)capacity > ~(size_t)0 / sizeof(void *) ||
                !(vector = (void **)
 