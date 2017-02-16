/*
 * xinclude.c : Code to implement XInclude processing
 *
 * World Wide Web Consortium W3C Last Call Working Draft 10 November 2003
 * http://www.w3.org/TR/2003/WD-xinclude-20031110
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/xpointer.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlerror.h>
#include <libxml/encoding.h>
#include <libxml/globals.h>

#ifdef LIBXML_XINCLUDE_ENABLED
#include <libxml/xinclude.h>


#define XINCLUDE_MAX_DEPTH 40

/* #define DEBUG_XINCLUDE */
#ifdef DEBUG_XINCLUDE
#ifdef LIBXML_DEBUG_ENABLED
#include <libxml/debugXML.h>
#endif
#endif

/************************************************************************
 *									*
 *			XInclude context handling			*
 *									*
 ************************************************************************/

/*
 * An XInclude context
 */
typedef xmlChar *xmlURL;

typedef struct _xmlXIncludeRef xmlXIncludeRef;
typedef xmlXIncludeRef *xmlXIncludeRefPtr;
struct _xmlXIncludeRef {
    xmlChar              *URI; /* the fully resolved resource URL */
    xmlChar         *fragment; /* the fragment in the URI */
    xmlDocPtr		  doc; /* the parsed document */
    xmlNodePtr            ref; /* the node making the reference in the source */
    xmlNodePtr            inc; /* the included copy */
    int                   xml; /* xml or txt */
    int                 count; /* how many refs use that specific doc */
    xmlXPathObjectPtr    xptr; /* the xpointer if needed */
    int		      emptyFb; /* flag to show fallback empty */
};

struct _xmlXIncludeCtxt {
    xmlDocPtr             doc; /* the source document */
    int               incBase; /* the first include for this document */
    int                 incNr; /* number of includes */
    int                incMax; /* size of includes tab */
    xmlXIncludeRefPtr *incTab; /* array of included references */

    int                 txtNr; /* number of unparsed documents */
    int                txtMax; /* size of unparsed documents tab */
    xmlNodePtr        *txtTab; /* array of unparsed text nodes */
    xmlURL         *txturlTab; /* array of unparsed text URLs */

    xmlChar *             url; /* the current URL processed */
    int                 urlNr; /* number of URLs stacked */
    int                urlMax; /* size of URL stack */
    xmlChar *         *urlTab; /* URL stack */

    int              nbErrors; /* the number of errors detected */
    int                legacy; /* using XINCLUDE_OLD_NS */
    int            parseFlags; /* the flags used for parsing XML documents */
    xmlChar *		 base; /* the current xml:base */
};

static int
xmlXIncludeDoProcess(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc, xmlNodePtr tree);


/************************************************************************
 *									*
 * 			XInclude error handler				*
 *									*
 ************************************************************************/

/**
 * xmlXIncludeErrMemory:
 * @extra:  extra information
 *
 * Handle an out of memory condition
 */
static void
xmlXIncludeErrMemory(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node,
                     const char *extra)
{
    if (ctxt != NULL)
	ctxt->nbErrors++;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, node, XML_FROM_XINCLUDE,
                    XML_ERR_NO_MEMORY, XML_ERR_ERROR, NULL, 0,
		    extra, NULL, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}

/**
 * xmlXIncludeErr:
 * @ctxt: the XInclude context
 * @node: the context node
 * @msg:  the error message
 * @extra:  extra information
 *
 * Handle an XInclude error
 */
static void
xmlXIncludeErr(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node, int error,
               const char *msg, const xmlChar *extra)
{
    if (ctxt != NULL)
	ctxt->nbErrors++;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, node, XML_FROM_XINCLUDE,
                    error, XML_ERR_ERROR, NULL, 0,
		    (const char *) extra, NULL, NULL, 0, 0,
		    msg, (const char *) extra);
}

#if 0
/**
 * xmlXIncludeWarn:
 * @ctxt: the XInclude context
 * @node: the context node
 * @msg:  the error message
 * @extra:  extra information
 *
 * Emit an XInclude warning.
 */
static void
xmlXIncludeWarn(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node, int error,
               const char *msg, const xmlChar *extra)
{
    __xmlRaiseError(NULL, NULL, NULL, ctxt, node, XML_FROM_XINCLUDE,
                    error, XML_ERR_WARNING, NULL, 0,
		    (const char *) extra, NULL, NULL, 0, 0,
		    msg, (const char *) extra);
}
#endif

/**
 * xmlXIncludeGetProp:
 * @ctxt:  the XInclude context
 * @cur:  the node
 * @name:  the attribute name
 *
 * Get an XInclude attribute
 *
 * Returns the value (to be freed) or NULL if not found
 */
static xmlChar *
xmlXIncludeGetProp(xmlXIncludeCtxtPtr ctxt, xmlNodePtr cur,
                   const xmlChar *name) {
    xmlChar *ret;

    ret = xmlGetNsProp(cur, XINCLUDE_NS, name);
    if (ret != NULL)
        return(ret);
    if (ctxt->legacy != 0) {
	ret = xmlGetNsProp(cur, XINCLUDE_OLD_NS, name);
	if (ret != NULL)
	    return(ret);
    }
    ret = xmlGetProp(cur, name);
    return(ret);
}
/**
 * xmlXIncludeFreeRef:
 * @ref: the XInclude reference
 *
 * Free an XInclude reference
 */
static void
xmlXIncludeFreeRef(xmlXIncludeRefPtr ref) {
    if (ref == NULL)
	return;
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Freeing ref\n");
#endif
    if (ref->doc != NULL) {
#ifdef DEBUG_XINCLUDE
	xmlGenericError(xmlGenericErrorContext, "Freeing doc %s\n", ref->URI);
#endif
	xmlFreeDoc(ref->doc);
    }
    if (ref->URI != NULL)
	xmlFree(ref->URI);
    if (ref->fragment != NULL)
	xmlFree(ref->fragment);
    if (ref->xptr != NULL)
	xmlXPathFreeObject(ref->xptr);
    xmlFree(ref);
}

/**
 * xmlXIncludeNewRef:
 * @ctxt: the XInclude context
 * @URI:  the resource URI
 *
 * Creates a new reference within an XInclude context
 *
 * Returns the new set
 */
static xmlXIncludeRefPtr
xmlXIncludeNewRef(xmlXIncludeCtxtPtr ctxt, const xmlChar *URI,
	          xmlNodePtr ref) {
    xmlXIncludeRefPtr ret;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "New ref %s\n", URI);
#endif
    ret = (xmlXIncludeRefPtr) xmlMalloc(sizeof(xmlXIncludeRef));
    if (ret == NULL) {
        xmlXIncludeErrMemory(ctxt, ref, "growing XInclude context");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlXIncludeRef));
    if (URI == NULL)
	ret->URI = NULL;
    else
	ret->URI = xmlStrdup(URI);
    ret->fragment = NULL;
    ret->ref = ref;
    ret->doc = 0;
    ret->count = 0;
    ret->xml = 0;
    ret->inc = NULL;
    if (ctxt->incMax == 0) {
	ctxt->incMax = 4;
        ctxt->incTab = (xmlXIncludeRefPtr *) xmlMalloc(ctxt->incMax *
					      sizeof(ctxt->incTab[0]));
        if (ctxt->incTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, ref, "growing XInclude context");
	    xmlXIncludeFreeRef(ret);
	    return(NULL);
	}
    }
    if (ctxt->incNr >= ctxt->incMax) {
	ctxt->incMax *= 2;
        ctxt->incTab = (xmlXIncludeRefPtr *) xmlRealloc(ctxt->incTab,
	             ctxt->incMax * sizeof(ctxt->incTab[0]));
        if (ctxt->incTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, ref, "growing XInclude context");
	    xmlXIncludeFreeRef(ret);
	    return(NULL);
	}
    }
    ctxt->incTab[ctxt->incNr++] = ret;
    return(ret);
}

/**
 * xmlXIncludeNewContext:
 * @doc:  an XML Document
 *
 * Creates a new XInclude context
 *
 * Returns the new set
 */
xmlXIncludeCtxtPtr
xmlXIncludeNewContext(xmlDocPtr doc) {
    xmlXIncludeCtxtPtr ret;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "New context\n");
#endif
    if (doc == NULL)
	return(NULL);
    ret = (xmlXIncludeCtxtPtr) xmlMalloc(sizeof(xmlXIncludeCtxt));
    if (ret == NULL) {
	xmlXIncludeErrMemory(NULL, (xmlNodePtr) doc,
	                     "creating XInclude context");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlXIncludeCtxt));
    ret->doc = doc;
    ret->incNr = 0;
    ret->incBase = 0;
    ret->incMax = 0;
    ret->incTab = NULL;
    ret->nbErrors = 0;
    return(ret);
}

/**
 * xmlXIncludeURLPush:
 * @ctxt:  the parser context
 * @value:  the url
 *
 * Pushes a new url on top of the url stack
 *
 * Returns -1 in case of error, the index in the stack otherwise
 */
static int
xmlXIncludeURLPush(xmlXIncludeCtxtPtr ctxt,
	           const xmlChar *value)
{
    if (ctxt->urlNr > XINCLUDE_MAX_DEPTH) {
	xmlXIncludeErr(ctxt, NULL, XML_XINCLUDE_RECURSION,
	               "detected a recursion in %s\n", value);
	return(-1);
    }
    if (ctxt->urlTab == NULL) {
	ctxt->urlMax = 4;
	ctxt->urlNr = 0;
	ctxt->urlTab = (xmlChar * *) xmlMalloc(
		        ctxt->urlMax * sizeof(ctxt->urlTab[0]));
        if (ctxt->urlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "adding URL");
            return (-1);
        }
    }
    if (ctxt->urlNr >= ctxt->urlMax) {
        ctxt->urlMax *= 2;
        ctxt->urlTab =
            (xmlChar * *) xmlRealloc(ctxt->urlTab,
                                      ctxt->urlMax *
                                      sizeof(ctxt->urlTab[0]));
        if (ctxt->urlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "adding URL");
            return (-1);
        }
    }
    ctxt->url = ctxt->urlTab[ctxt->urlNr] = xmlStrdup(value);
    return (ctxt->urlNr++);
}

/**
 * xmlXIncludeURLPop:
 * @ctxt: the parser context
 *
 * Pops the top URL from the URL stack
 */
static void
xmlXIncludeURLPop(xmlXIncludeCtxtPtr ctxt)
{
    xmlChar * ret;

    if (ctxt->urlNr <= 0)
        return;
    ctxt->urlNr--;
    if (ctxt->urlNr > 0)
        ctxt->url = ctxt->urlTab[ctxt->urlNr - 1];
    else
        ctxt->url = NULL;
    ret = ctxt->urlTab[ctxt->urlNr];
    ctxt->urlTab[ctxt->urlNr] = 0;
    if (ret != NULL)
	xmlFree(ret);
}

/**
 * xmlXIncludeFreeContext:
 * @ctxt: the XInclude context
 *
 * Free an XInclude context
 */
void
xmlXIncludeFreeContext(xmlXIncludeCtxtPtr ctxt) {
    int i;

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Freeing context\n");
#endif
    if (ctxt == NULL)
	return;
    while (ctxt->urlNr > 0)
	xmlXIncludeURLPop(ctxt);
    if (ctxt->urlTab != NULL)
	xmlFree(ctxt->urlTab);
    for (i = 0;i < ctxt->incNr;i++) {
	if (ctxt->incTab[i] != NULL)
	    xmlXIncludeFreeRef(ctxt->incTab[i]);
    }
    for (i = 0;i < ctxt->txtNr;i++) {
	if (ctxt->txturlTab[i] != NULL)
	    xmlFree(ctxt->txturlTab[i]);
    }
    if (ctxt->incTab != NULL)
	xmlFree(ctxt->incTab);
    if (ctxt->txtTab != NULL)
	xmlFree(ctxt->txtTab);
    if (ctxt->txturlTab != NULL)
	xmlFree(ctxt->txturlTab);
    if (ctxt->base != NULL) {
        xmlFree(ctxt->base);
    }
    xmlFree(ctxt);
}

/**
 * xmlXIncludeParseFile:
 * @ctxt:  the XInclude context
 * @URL:  the URL or file path
 * 
 * parse a document for XInclude
 */
static xmlDocPtr
xmlXIncludeParseFile(xmlXIncludeCtxtPtr ctxt, const char *URL) {
    xmlDocPtr ret;
    xmlParserCtxtPtr pctxt;
    char *directory = NULL;
    xmlParserInputPtr inputStream;

    xmlInitParser();

    pctxt = xmlNewParserCtxt();
    if (pctxt == NULL) {
	xmlXIncludeErrMemory(ctxt, NULL, "cannot allocate parser context");
	return(NULL);
    }
    /*
     * try to ensure that new documents included are actually
     * built with the same dictionary as the including document.
     */
    if ((ctxt->doc != NULL) && (ctxt->doc->dict != NULL) &&
        (pctxt->dict != NULL)) {
	xmlDictFree(pctxt->dict);
	pctxt->dict = ctxt->doc->dict;
	xmlDictReference(pctxt->dict);
    }

    xmlCtxtUseOptions(pctxt, ctxt->parseFlags | XML_PARSE_DTDLOAD);
    
    inputStream = xmlLoadExternalEntity(URL, NULL, pctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(pctxt);
	return(NULL);
    }

    inputPush(pctxt, inputStream);

    if ((pctxt->directory == NULL) && (directory == NULL))
        directory = xmlParserGetDirectory(URL);
    if ((pctxt->directory == NULL) && (directory != NULL))
        pctxt->directory = (char *) xmlStrdup((xmlChar *) directory);

    pctxt->loadsubset = XML_DETECT_IDS;

    xmlParseDocument(pctxt);

    if (pctxt->wellFormed) {
        ret = pctxt->myDoc;
    }
    else {
        ret = NULL;
	if (pctxt->myDoc != NULL)
	    xmlFreeDoc(pctxt->myDoc);
        pctxt->myDoc = NULL;
    }
    xmlFreeParserCtxt(pctxt);
    
    return(ret);
}

/**
 * xmlXIncludeAddNode:
 * @ctxt:  the XInclude context
 * @cur:  the new node
 * 
 * Add a new node to process to an XInclude context
 */
static int
xmlXIncludeAddNode(xmlXIncludeCtxtPtr ctxt, xmlNodePtr cur) {
    xmlXIncludeRefPtr ref;
    xmlURIPtr uri;
    xmlChar *URL;
    xmlChar *fragment = NULL;
    xmlChar *href;
    xmlChar *parse;
    xmlChar *base;
    xmlChar *URI;
    int xml = 1, i; /* default Issue 64 */
    int local = 0;


    if (ctxt == NULL)
	return(-1);
    if (cur == NULL)
	return(-1);

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Add node\n");
#endif
    /*
     * read the attributes
     */
    href = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_HREF);
    if (href == NULL) {
	href = xmlStrdup(BAD_CAST ""); /* @@@@ href is now optional */
	if (href == NULL) 
	    return(-1);
	local = 1;
    }
    if (href[0] == '#')
	local = 1;
    parse = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_PARSE);
    if (parse != NULL) {
	if (xmlStrEqual(parse, XINCLUDE_PARSE_XML))
	    xml = 1;
	else if (xmlStrEqual(parse, XINCLUDE_PARSE_TEXT))
	    xml = 0;
	else {
	    xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_PARSE_VALUE,
	                   "invalid value %s for 'parse'\n", parse);
	    if (href != NULL)
		xmlFree(href);
	    if (parse != NULL)
		xmlFree(parse);
	    return(-1);
	}
    }

    /*
     * compute the URI
     */
    base = xmlNodeGetBase(ctxt->doc, cur);
    if (base == NULL) {
	URI = xmlBuildURI(href, ctxt->doc->URL);
    } else {
	URI = xmlBuildURI(href, base);
    }
    if (URI == NULL) {
	xmlChar *escbase;
	xmlChar *eschref;
	/*
	 * Some escaping may be needed
	 */
	escbase = xmlURIEscape(base);
	eschref = xmlURIEscape(href);
	URI = xmlBuildURI(eschref, escbase);
	if (escbase != NULL)
	    xmlFree(escbase);
	if (eschref != NULL)
	    xmlFree(eschref);
    }
    if (parse != NULL)
	xmlFree(parse);
    if (href != NULL)
	xmlFree(href);
    if (base != NULL)
	xmlFree(base);
    if (URI == NULL) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_HREF_URI,
	               "failed build URL\n", NULL);
	return(-1);
    }
    fragment = xmlXIncludeGetProp(ctxt, cur, XINCLUDE_PARSE_XPOINTER);

    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)URI);
    if (uri == NULL) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_HREF_URI,
	               "invalid value URI %s\n", URI);
	if (fragment != NULL)
	    xmlFree(fragment);
	xmlFree(URI);
	return(-1);
    }

    if (uri->fragment != NULL) {
        if (ctxt->legacy != 0) {
	    if (fragment == NULL) {
		fragment = (xmlChar *) uri->fragment;
	    } else {
		xmlFree(uri->fragment);
	    }
	} else {
	    xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_FRAGMENT_ID,
       "Invalid fragment identifier in URI %s use the xpointer attribute\n",
                           URI);
	    if (fragment != NULL)
	        xmlFree(fragment);
	    xmlFreeURI(uri);
	    xmlFree(URI);
	    return(-1);
	}
	uri->fragment = NULL;
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    xmlFree(URI);
    if (URL == NULL) {
	xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_HREF_URI,
	               "invalid value URI %s\n", URI);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }

    /*
     * Check the URL against the stack for recursions
     */
    if (!local) {
	for (i = 0;i < ctxt->urlNr;i++) {
	    if (xmlStrEqual(URL, ctxt->urlTab[i])) {
		xmlXIncludeErr(ctxt, cur, XML_XINCLUDE_RECURSION,
		               "detected a recursion in %s\n", URL);
		return(-1);
	    }
	}
    }

    ref = xmlXIncludeNewRef(ctxt, URL, cur);
    if (ref == NULL) {
	return(-1);
    }
    ref->fragment = fragment;
    ref->doc = NULL;
    ref->xml = xml;
    ref->count = 1;
    xmlFree(URL);
    return(0);
}

/**
 * xmlXIncludeRecurseDoc:
 * @ctxt:  the XInclude context
 * @doc:  the new document
 * @url:  the associated URL
 * 
 * The XInclude recursive nature is handled at this point.
 */
static void
xmlXIncludeRecurseDoc(xmlXIncludeCtxtPtr ctxt, xmlDocPtr doc,
	              const xmlURL url ATTRIBUTE_UNUSED) {
    xmlXIncludeCtxtPtr newctxt;
    int i;

    /*
     * Avoid recursion in already substitued resources
    for (i = 0;i < ctxt->urlNr;i++) {
	if (xmlStrEqual(doc->URL, ctxt->urlTab[i]))
	    return;
    }
     */

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Recursing in doc %s\n", doc->URL);
#endif
    /*
     * Handle recursion here.
     */

    newctxt = xmlXIncludeNewContext(doc);
    if (newctxt != NULL) {
	/*
	 * Copy the existing document set
	 */
	newctxt->incMax = ctxt->incMax;
	newctxt->incNr = ctxt->incNr;
        newctxt->incTab = (xmlXIncludeRefPtr *) xmlMalloc(newctxt->incMax *
		                          sizeof(newctxt->incTab[0]));
        if (newctxt->incTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, (xmlNodePtr) doc, "processing doc");
	    xmlFree(newctxt);
	    return;
	}
	/*
	 * copy the urlTab
	 */
	newctxt->urlMax = ctxt->urlMax;
	newctxt->urlNr = ctxt->urlNr;
	newctxt->urlTab = ctxt->urlTab;

	/*
	 * Inherit the existing base
	 */
	newctxt->base = xmlStrdup(ctxt->base);

	/*
	 * Inherit the documents already in use by other includes
	 */
	newctxt->incBase = ctxt->incNr;
	for (i = 0;i < ctxt->incNr;i++) {
	    newctxt->incTab[i] = ctxt->incTab[i];
	    newctxt->incTab[i]->count++; /* prevent the recursion from
					    freeing it */
	}
	/*
	 * The new context should also inherit the Parse Flags
	 * (bug 132597)
	 */
	newctxt->parseFlags = ctxt->parseFlags;
	xmlXIncludeDoProcess(newctxt, doc, xmlDocGetRootElement(doc));
	for (i = 0;i < ctxt->incNr;i++) {
	    newctxt->incTab[i]->count--;
	    newctxt->incTab[i] = NULL;
	}

	/* urlTab may have been reallocated */
	ctxt->urlTab = newctxt->urlTab;
	ctxt->urlMax = newctxt->urlMax;

	newctxt->urlMax = 0;
	newctxt->urlNr = 0;
	newctxt->urlTab = NULL;

	xmlXIncludeFreeContext(newctxt);
    }
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Done recursing in doc %s\n", url);
#endif
}

/**
 * xmlXIncludeAddTxt:
 * @ctxt:  the XInclude context
 * @txt:  the new text node
 * @url:  the associated URL
 * 
 * Add a new txtument to the list
 */
static void
xmlXIncludeAddTxt(xmlXIncludeCtxtPtr ctxt, xmlNodePtr txt, const xmlURL url) {
#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Adding text %s\n", url);
#endif
    if (ctxt->txtMax == 0) {
	ctxt->txtMax = 4;
        ctxt->txtTab = (xmlNodePtr *) xmlMalloc(ctxt->txtMax *
		                          sizeof(ctxt->txtTab[0]));
        if (ctxt->txtTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
        ctxt->txturlTab = (xmlURL *) xmlMalloc(ctxt->txtMax *
		                          sizeof(ctxt->txturlTab[0]));
        if (ctxt->txturlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
    }
    if (ctxt->txtNr >= ctxt->txtMax) {
	ctxt->txtMax *= 2;
        ctxt->txtTab = (xmlNodePtr *) xmlRealloc(ctxt->txtTab,
	             ctxt->txtMax * sizeof(ctxt->txtTab[0]));
        if (ctxt->txtTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
        ctxt->txturlTab = (xmlURL *) xmlRealloc(ctxt->txturlTab,
	             ctxt->txtMax * sizeof(ctxt->txturlTab[0]));
        if (ctxt->txturlTab == NULL) {
	    xmlXIncludeErrMemory(ctxt, NULL, "processing text");
	    return;
	}
    }
    ctxt->txtTab[ctxt->txtNr] = txt;
    ctxt->txturlTab[ctxt->txtNr] = xmlStrdup(url);
    ctxt->txtNr++;
}

/************************************************************************
 *									*
 *			Node copy with specific semantic		*
 *									*
 ************************************************************************/

/**
 * xmlXIncludeCopyNode:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @elem:  the element
 * 
 * Make a copy of the node while preserving the XInclude semantic
 * of the Infoset copy
 */
static xmlNodePtr
xmlXIncludeCopyNode(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	            xmlDocPtr source, xmlNodePtr elem) {
    xmlNodePtr result = NULL;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(elem == NULL))
	return(NULL);
    if (elem->type == XML_DTD_NODE)
	return(NULL);
    result = xmlDocCopyNode(elem, target, 1);
    return(result);
}

/**
 * xmlXIncludeCopyNodeList:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @elem:  the element list
 * 
 * Make a copy of the node list while preserving the XInclude semantic
 * of the Infoset copy
 */
static xmlNodePtr
xmlXIncludeCopyNodeList(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlNodePtr elem) {
    xmlNodePtr cur, res, result = NULL, last = NULL;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(elem == NULL))
	return(NULL);
    cur = elem;
    while (cur != NULL) {
	res = xmlXIncludeCopyNode(ctxt, target, source, cur);
	if (res != NULL) {
	    if (result == NULL) {
		result = last = res;
	    } else {
		last->next = res;
		res->prev = last;
		last = res;
	    }
	}
	cur = cur->next;
    }
    return(result);
}

/**
 * xmlXIncludeGetNthChild:
 * @cur:  the node
 * @no:  the child number
 *
 * Returns the @n'th element child of @cur or NULL
 */
static xmlNodePtr
xmlXIncludeGetNthChild(xmlNodePtr cur, int no) {
    int i;
    if (cur == NULL) 
	return(cur);
    cur = cur->children;
    for (i = 0;i <= no;cur = cur->next) {
	if (cur == NULL) 
	    return(cur);
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_DOCUMENT_NODE) ||
	    (cur->type == XML_HTML_DOCUMENT_NODE)) {
	    i++;
	    if (i == no)
		break;
	}
    }
    return(cur);
}

xmlNodePtr xmlXPtrAdvanceNode(xmlNodePtr cur, int *level); /* in xpointer.c */
/**
 * xmlXIncludeCopyRange:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @obj:  the XPointer result from the evaluation.
 *
 * Build a node list tree copy of the XPointer result.
 *
 * Returns an xmlNodePtr list or NULL.
 *         The caller has to free the node tree.
 */
static xmlNodePtr
xmlXIncludeCopyRange(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlXPathObjectPtr range) {
    /* pointers to generated nodes */
    xmlNodePtr list = NULL, last = NULL, listParent = NULL;
    xmlNodePtr tmp, tmp2;
    /* pointers to traversal nodes */
    xmlNodePtr start, cur, end;
    int index1, index2;
    int level = 0, lastLevel = 0, endLevel = 0, endFlag = 0;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(range == NULL))
	return(NULL);
    if (range->type != XPATH_RANGE)
	return(NULL);
    start = (xmlNodePtr) range->user;

    if (start == NULL)
	return(NULL);
    end = (xmlNodePtr)range->user2;
    if (end == NULL)
	return(xmlDocCopyNode(start, target, 1));

    cur = start;
    index1 = range->index;
    index2 = range->index2;
    /*
     * level is depth of the current node under consideration
     * list is the pointer to the root of the output tree
     * listParent is a pointer to the parent of output tree (within
       the included file) in case we need to add another level
     * last is a pointer to the last node added to the output tree
     * lastLevel is the depth of last (relative to the root)
     */
    while (cur != NULL) {
	/*
	 * Check if our output tree needs a parent
	 */
	if (level < 0) {
	    while (level < 0) {
	        /* copy must include namespaces and properties */
	        tmp2 = xmlDocCopyNode(listParent, target, 2);
	        xmlAddChild(tmp2, list);
	        list = tmp2;
	        listParent = listParent->parent;
	        level++;
	    }
	    last = list;
	    lastLevel = 0;
	}
	/*
	 * Check whether we need to change our insertion point
	 */
	while (level < lastLevel) {
	    last = last->parent;
	    lastLevel --;
	}
	if (cur == end) {	/* Are we at the end of the range? */
	    if (cur->type == XML_TEXT_NODE) {
		const xmlChar *content = cur->content;
		int len;

		if (content == NULL) {
		    tmp = xmlNewTextLen(NULL, 0);
		} else {
		    len = index2;
		    if ((cur == start) && (index1 > 1)) {
			content += (index1 - 1);
			len -= (index1 - 1);
			index1 = 0;
		    } else {
			len = index2;
		    }
		    tmp = xmlNewTextLen(content, len);
		}
		/* single sub text node selection */
		if (list == NULL)
		    return(tmp);
		/* prune and return full set */
		if (level == lastLevel)
		    xmlAddNextSibling(last, tmp);
		else 
		    xmlAddChild(last, tmp);
		return(list);
	    } else {	/* ending node not a text node */
	        endLevel = level;	/* remember the level of the end node */
		endFlag = 1;
		/* last node - need to take care of properties + namespaces */
		tmp = xmlDocCopyNode(cur, target, 2);
		if (list == NULL) {
		    list = tmp;
		    listParent = cur->parent;
		} else {
		    if (level == lastLevel)
			xmlAddNextSibling(last, tmp);
		    else {
			xmlAddChild(last, tmp);
			lastLevel = level;
		    }
		}
		last = tmp;

		if (index2 > 1) {
		    end = xmlXIncludeGetNthChild(cur, index2 - 1);
		    index2 = 0;
		}
		if ((cur == start) && (index1 > 1)) {
		    cur = xmlXIncludeGetNthChild(cur, index1 - 1);
		    index1 = 0;
		}  else {
		    cur = cur->children;
		}
		level++;	/* increment level to show change */
		/*
		 * Now gather the remaining nodes from cur to end
		 */
		continue;	/* while */
	    }
	} else if (cur == start) {	/* Not at the end, are we at start? */
	    if ((cur->type == XML_TEXT_NODE) ||
		(cur->type == XML_CDATA_SECTION_NODE)) {
		const xmlChar *content = cur->content;

		if (content == NULL) {
		    tmp = xmlNewTextLen(NULL, 0);
		} else {
		    if (index1 > 1) {
			content += (index1 - 1);
			index1 = 0;
		    }
		    tmp = xmlNewText(content);
		}
		last = list = tmp;
		listParent = cur->parent;
	    } else {		/* Not text node */
	        /*
		 * start of the range - need to take care of
		 * properties and namespaces
		 */
		tmp = xmlDocCopyNode(cur, target, 2);
		list = last = tmp;
		listParent = cur->parent;
		if (index1 > 1) {	/* Do we need to position? */
		    cur = xmlXIncludeGetNthChild(cur, index1 - 1);
		    level = lastLevel = 1;
		    index1 = 0;
		    /*
		     * Now gather the remaining nodes from cur to end
		     */
		    continue; /* while */
		}
	    }
	} else {
	    tmp = NULL;
	    switch (cur->type) {
		case XML_DTD_NODE:
		case XML_ELEMENT_DECL:
		case XML_ATTRIBUTE_DECL:
		case XML_ENTITY_NODE:
		    /* Do not copy DTD informations */
		    break;
		case XML_ENTITY_DECL:
		    /* handle crossing entities -> stack needed */
		    break;
		case XML_XINCLUDE_START:
		case XML_XINCLUDE_END:
		    /* don't consider it part of the tree content */
		    break;
		case XML_ATTRIBUTE_NODE:
		    /* Humm, should not happen ! */
		    break;
		default:
		    /*
		     * Middle of the range - need to take care of
		     * properties and namespaces
		     */
		    tmp = xmlDocCopyNode(cur, target, 2);
		    break;
	    }
	    if (tmp != NULL) {
		if (level == lastLevel)
		    xmlAddNextSibling(last, tmp);
		else {
		    xmlAddChild(last, tmp);
		    lastLevel = level;
		}
		last = tmp;
	    }
	}
	/*
	 * Skip to next node in document order
	 */
	cur = xmlXPtrAdvanceNode(cur, &level);
	if (endFlag && (level >= endLevel))
	    break;
    }
    return(list);
}

/**
 * xmlXIncludeBuildNodeList:
 * @ctxt:  the XInclude context
 * @target:  the document target
 * @source:  the document source
 * @obj:  the XPointer result from the evaluation.
 *
 * Build a node list tree copy of the XPointer result.
 * This will drop Attributes and Namespace declarations.
 *
 * Returns an xmlNodePtr list or NULL.
 *         the caller has to free the node tree.
 */
static xmlNodePtr
xmlXIncludeCopyXPointer(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlXPathObjectPtr obj) {
    xmlNodePtr list = NULL, last = NULL;
    int i;

    if (source == NULL)
	source = ctxt->doc;
    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(obj == NULL))
	return(NULL);
    switch (obj->type) {
        case XPATH_NODESET: {
	    xmlNodeSetPtr set = obj->nodesetval;
	    if (set == NULL)
		return(NULL);
	    for (i = 0;i < set->nodeNr;i++) {
		if (set->nodeTab[i] == NULL)
		    continue;
		switch (set->nodeTab[i]->type) {
		    case XML_TEXT_NODE:
		    case XML_CDATA_SECTION_NODE:
		    case XML_ELEMENT_NODE:
		    case XML_ENTITY_REF_NODE:
		    case XML_ENTITY_NODE:
		    case XML_PI_NODE:
		    case XML_COMMENT_NODE:
		    case XML_DOCUMENT_NODE:
		    case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
		    case XML_DOCB_DOCUMENT_NODE:
#endif
		    case XML_XINCLUDE_END:
			break;
		    case XML_XINCLUDE_START: {
	                xmlNodePtr tmp, cur = set->nodeTab[i];

			cur = cur->next;
			while (cur != NULL) {
			    switch(cur->type) {
				case XML_TEXT_NODE:
				case XML_CDATA_SECTION_NODE:
				case XML_ELEMENT_NODE:
				case XML_ENTITY_REF_NODE:
				case XML_ENTITY_NODE:
				case XML_PI_NODE:
				case XML_COMMENT_NODE:
				    tmp = xmlXIncludeCopyNode(ctxt, target,
							      source, cur);
				    if (last == NULL) {
					list = last = tmp;
				    } else {
					xmlAddNextSibling(last, tmp);
					last = tmp;
				    }
				    cur = cur->next;
				    continue;
				default:
				    break;
			    }
			    break;
			}
			continue;
		    }
		    case XML_ATTRIBUTE_NODE:
		    case XML_NAMESPACE_DECL:
		    case XML_DOCUMENT_TYPE_NODE:
		    case XML_DOCUMENT_FRAG_NODE:
		    case XML_NOTATION_NODE:
		    case XML_DTD_NODE:
		    case XML_ELEMENT_DECL:
		    case XML_ATTRIBUTE_DECL:
		    case XML_ENTITY_DECL:
			continue; /* for */
		}
		if (last == NULL)
		    list = last = xmlXIncludeCopyNode(ctxt, target, source,
			                              set->nodeTab[i]);
		else {
		    xmlAddNextSibling(last,
			    xmlXIncludeCopyNode(ctxt, target, source,
				                set->nodeTab[i]));
		    if (last->next != NULL)
			last = last->next;
		}
	    }
	    break;
	}
	case XPATH_LOCATIONSET: {
	    xmlLocationSetPtr set = (xmlLocationSetPtr) obj->user;
	    if (set == NULL)
		return(NULL);
	    for (i = 0;i < set->locNr;i++) {
		if (last == NULL)
		    list = last = xmlXIncludeCopyXPointer(ctxt, target, source,
			                                  set->locTab[i]);
		else
		    xmlAddNextSibling(last,
			    xmlXIncludeCopyXPointer(ctxt, target, source,
				                    set->locTab[i]));
		if (last != NULL) {
		    while (last->next != NULL)
			last = last->next;
		}
	    }
	    break;
	}
#ifdef LIBXML_XPTR_ENABLED
	case XPATH_RANGE:
	    return(xmlXIncludeCopyRange(ctxt, target, source, obj));
#endif
	case XPATH_POINT:
	    /* points are ignored in XInclude */
	    break;
	default:
	    break;
    }
    return(list);
}
/************************************************************************
 *									*
 *			XInclude I/O handling				*
 *									*
 ************************************************************************/

typedef struct _xmlXIncludeMergeData xmlXIncludeMergeData;
typedef xmlXIncludeMergeData *xmlXIncludeMergeDataPtr;
struct _xmlXIncludeMergeData {
    xmlDocPtr doc;
    xmlXIncludeCtxtPtr ctxt;
};

/**
 * xmlXIncludeMergeOneEntity:
 * @ent: the entity
 * @doc:  the including doc
 * @nr: the entity name
 *
 * Inplements the merge of one entity
 */
static void
xmlXIncludeMergeEntity(xmlEntityPtr ent, xmlXIncludeMergeDataPtr data,
	               xmlChar *name ATTRIBUTE_UNUSED) {
    xmlEntityPtr ret, prev;
    xmlDocPtr doc;
    xmlXIncludeCtxtPtr ctxt;

    if ((ent == NULL) || (data == NULL))
	return;
    ctxt = data->ctxt;
    doc = data->doc;
    if ((ctxt == NULL) || (doc == NULL))
	return;
    switch (ent->etype) {
        case XML_INTERNAL_PARAMETER_ENTITY:
        case XML_EXTERNAL_PARAMETER_ENTITY:
        case XML_INTERNAL_PREDEFINED_ENTITY:
	    return;
        case XML_INTERNAL_GENERAL_ENTITY:
        case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
        case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	    break;
    }
    ret = xmlAddDocEntity(doc, ent->name, ent->etype, ent->ExternalID,
			  ent->SystemID, ent->content);
    if (ret != NULL) {
	if (ent->URI != NULL)
	    ret->URI = xmlStrdup(ent->URI);
    } else {
	prev = xmlGetDocEntity(doc, ent->name);
	if (prev != NULL) {
	    if (ent->etype != prev->etype)
		goto error;
	
	    if ((ent->SystemID != NUL