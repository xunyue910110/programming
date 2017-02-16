/*
 * "Canonical XML" implementation 
 * http://www.w3.org/TR/xml-c14n
 * 
 * "Exclusive XML Canonicalization" implementation
 * http://www.w3.org/TR/xml-exc-c14n
 *
 * See Copyright for the status of this software.
 * 
 * Author: Aleksey Sanin <aleksey@aleksey.com>
 */
#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_C14N_ENABLED
#ifdef LIBXML_OUTPUT_ENABLED

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>
#include <libxml/globals.h>
#include <libxml/xpathInternals.h>
#include <libxml/c14n.h>

/************************************************************************
 *									*
 *		Some declaration better left private ATM		*
 *									*
 ************************************************************************/

typedef enum {
    XMLC14N_BEFORE_DOCUMENT_ELEMENT = 0,
    XMLC14N_INSIDE_DOCUMENT_ELEMENT = 1,
    XMLC14N_AFTER_DOCUMENT_ELEMENT = 2
} xmlC14NPosition;

typedef struct _xmlC14NVisibleNsStack {
    int nsCurEnd;	 /* number of nodes in the set */
    int nsPrevStart; 	 /* the begginning of the stack for previous visible node */
    int nsPrevEnd;	 /* the end of the stack for previous visible node */
    int nsMax;		 /* size of the array as allocated */
    xmlNsPtr 	*nsTab;	 /* array of ns in no particular order */	      
    xmlNodePtr	*nodeTab;/* array of nodes in no particular order */
} xmlC14NVisibleNsStack, *xmlC14NVisibleNsStackPtr;

typedef struct _xmlC14NCtx {
    /* input parameters */
    xmlDocPtr doc;
    xmlC14NIsVisibleCallback is_visible_callback;
    void* user_data;    
    int with_comments;
    xmlOutputBufferPtr buf;

    /* position in the XML document */
    xmlC14NPosition pos;
    int parent_is_doc;
    xmlC14NVisibleNsStackPtr ns_rendered;

    /* exclusive canonicalization */
    int exclusive;
    xmlChar **inclusive_ns_prefixes;

    /* error number */
    int error;
} xmlC14NCtx, *xmlC14NCtxPtr;

static xmlC14NVisibleNsStackPtr	xmlC14NVisibleNsStackCreate	(void);
static void			xmlC14NVisibleNsStackDestroy	(xmlC14NVisibleNsStackPtr cur);
static void 			xmlC14NVisibleNsStackAdd	(xmlC14NVisibleNsStackPtr cur, 
								 xmlNsPtr ns,
								 xmlNodePtr node);
static void 			xmlC14NVisibleNsStackSave	(xmlC14NVisibleNsStackPtr cur,
								 xmlC14NVisibleNsStackPtr state);
static void 			xmlC14NVisibleNsStackRestore	(xmlC14NVisibleNsStackPtr cur,
								 xmlC14NVisibleNsStackPtr state);
static void 			xmlC14NVisibleNsStackShift	(xmlC14NVisibleNsStackPtr cur);
static int			xmlC14NVisibleNsStackFind	(xmlC14NVisibleNsStackPtr cur, 
								 xmlNsPtr ns);
static int			xmlExcC14NVisibleNsStackFind	(xmlC14NVisibleNsStackPtr cur, 
								 xmlNsPtr ns,
								 xmlC14NCtxPtr ctx);

static int			xmlC14NIsNodeInNodeset		(xmlNodeSetPtr nodes,
								 xmlNodePtr node,
								 xmlNodePtr parent);



static int xmlC14NProcessNode(xmlC14NCtxPtr ctx, xmlNodePtr cur);
static int xmlC14NProcessNodeList(xmlC14NCtxPtr ctx, xmlNodePtr cur);
typedef enum {
    XMLC14N_NORMALIZE_ATTR = 0,
    XMLC14N_NORMALIZE_COMMENT = 1,
    XMLC14N_NORMALIZE_PI = 2,
    XMLC14N_NORMALIZE_TEXT = 3
} xmlC14NNormalizationMode;

static xmlChar *xmlC11NNormalizeString(const xmlChar * input,
                                       xmlC14NNormalizationMode mode);

#define 	xmlC11NNormalizeAttr( a ) \
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_ATTR)
#define 	xmlC11NNormalizeComment( a ) \
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_COMMENT)
#define 	xmlC11NNormalizePI( a )	\
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_PI)
#define 	xmlC11NNormalizeText( a ) \
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_TEXT)

#define 	xmlC14NIsVisible( ctx, node, parent ) \
     (((ctx)->is_visible_callback != NULL) ? \
	(ctx)->is_visible_callback((ctx)->user_data, \
		(xmlNodePtr)(node), (xmlNodePtr)(parent)) : 1)

/************************************************************************
 *									*
 * 		Some factorized error routines				*
 *									*
 ************************************************************************/

/**
 * xmlC14NErrMemory:
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
xmlC14NErrMemory(const char *extra)
{
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_C14N,
		    XML_ERR_NO_MEMORY, XML_ERR_ERROR, NULL, 0, extra,
		    NULL, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}

/**
 * xmlC14NErr:
 * @ctxt:  a C14N evaluation context
 * @node:  the context node
 * @error:  the erorr code
 * @msg:  the message
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
xmlC14NErr(xmlC14NCtxPtr ctxt, xmlNodePtr node, int error,
           const char * msg)
{
    if (ctxt != NULL)
        ctxt->error = error;
    __xmlRaiseError(NULL, NULL, NULL,
		    ctxt, node, XML_FROM_C14N, error,
		    XML_ERR_ERROR, NULL, 0,
		    NULL, NULL, NULL, 0, 0, msg);
}

/************************************************************************
 *									*
 *		The implementation internals				*
 *									*
 ************************************************************************/
#define XML_NAMESPACES_DEFAULT		16

static int			
xmlC14NIsNodeInNodeset(xmlNodeSetPtr nodes, xmlNodePtr node, xmlNodePtr parent) {
    if((nodes != NULL) && (node != NULL)) {
	if(node->type != XML_NAMESPACE_DECL) {
	    return(xmlXPathNodeSetContains(nodes, node));
	} else {
	    xmlNs ns;
	    
	    memcpy(&ns, node, sizeof(ns)); 
	    
	    /* this is a libxml hack! check xpath.c for details */
	    if((parent != NULL) && (parent->type == XML_ATTRIBUTE_NODE)) {
		ns.next = (xmlNsPtr)parent->parent;
	    } else {
		ns.next = (xmlNsPtr)parent; 
	    }

	    /* 
	     * If the input is an XPath node-set, then the node-set must explicitly 
	     * contain every node to be rendered to the canonical form.
	     */
	    return(xmlXPathNodeSetContains(nodes, (xmlNodePtr)&ns));
	}
    }
    return(1);
}

static xmlC14NVisibleNsStackPtr
xmlC14NVisibleNsStackCreate(void) {
    xmlC14NVisibleNsStackPtr ret;

    ret = (xmlC14NVisibleNsStackPtr) xmlMalloc(sizeof(xmlC14NVisibleNsStack));
    if (ret == NULL) {
        xmlC14NErrMemory("creating stack");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlC14NVisibleNsStack));
    return(ret);
}

static void
xmlC14NVisibleNsStackDestroy(xmlC14NVisibleNsStackPtr cur) {
    if(cur == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlC14NVisibleNsStackDestroy: cur is null.\n");
#endif
	return;
    }
    if(cur->nsTab != NULL) {
	memset(cur->nsTab, 0, cur->nsMax * sizeof(xmlNsPtr));
	xmlFree(cur->nsTab);
    }
    if(cur->nodeTab != NULL) {
	memset(cur->nodeTab, 0, cur->nsMax * sizeof(xmlNodePtr));
	xmlFree(cur->nodeTab);
    }
    memset(cur, 0, sizeof(xmlC14NVisibleNsStack));
    xmlFree(cur);
    
}

static void 
xmlC14NVisibleNsStackAdd(xmlC14NVisibleNsStackPtr cur, xmlNsPtr ns, xmlNodePtr node) {
    if((cur == NULL) || 
       ((cur->nsTab == NULL) && (cur->nodeTab != NULL)) ||
       ((cur->nsTab != NULL) && (cur->nodeTab == NULL))) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlC14NVisibleNsStackAdd: cur is null.\n");
#endif
	return;
    }

    if ((cur->nsTab == NULL) && (cur->nodeTab == NULL)) {
        cur->nsTab = (xmlNsPtr*) xmlMalloc(XML_NAMESPACES_DEFAULT * sizeof(xmlNsPtr));
        cur->nodeTab = (xmlNodePtr*) xmlMalloc(XML_NAMESPACES_DEFAULT * sizeof(xmlNodePtr));
	if ((cur->nsTab == NULL) || (cur->nodeTab == NULL)) {
	    xmlC14NErrMemory("adding node to stack");
	    return;
	}
	memset(cur->nsTab, 0 , XML_NAMESPACES_DEFAULT * sizeof(xmlNsPtr));
	memset(cur->nodeTab, 0 , XML_NAMESPACES_DEFAULT * sizeof(xmlNodePtr));
        cur->nsMax = XML_NAMESPACES_DEFAULT;
    } else if(cur->nsMax == cur->nsCurEnd) {
	void *tmp;	
	int tmpSize;
	
	tmpSize = 2 * cur->nsMax;
	tmp = xmlRealloc(cur->nsTab, tmpSize * sizeof(xmlNsPtr));
	if (tmp == NULL) {
	    xmlC14NErrMemory("adding node to stack");
	    return;
	}
	cur->nsTab = (xmlNsPtr*)tmp;

	tmp = xmlRealloc(cur->nodeTab, tmpSize * sizeof(xmlNodePtr));
	if (tmp == NULL) {
	    xmlC14NErrMemory("adding node to stack");
	    return;
	}
	cur->nodeTab = (xmlNodePtr*)tmp;

	cur->nsMax = tmpSize;
    }
    cur->nsTab[cur->nsCurEnd] = ns;
    cur->nodeTab[cur->nsCurEnd] = node;

    ++cur->nsCurEnd;
}

static void
xmlC14NVisibleNsStackSave(xmlC14NVisibleNsStackPtr cur, xmlC14NVisibleNsStackPtr state) {
    if((cur == NULL) || (state == NULL)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlC14NVisibleNsStackSave: cur or state is null.\n");
#endif
	return;
    }
    
    state->nsCurEnd = cur->nsCurEnd;
    state->nsPrevStart = cur->nsPrevStart;
    state->nsPrevEnd = cur->nsPrevEnd;
}

static void
xmlC14NVisibleNsStackRestore(xmlC14NVisibleNsStackPtr cur, xmlC14NVisibleNsStackPtr state) {
    if((cur == NULL) || (state == NULL)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlC14NVisibleNsStackRestore: cur or state is null.\n");
#endif
	return;
    }
    cur->nsCurEnd = state->nsCurEnd;
    cur->nsPrevStart = state->nsPrevStart;
    cur->nsPrevEnd = state->nsPrevEnd;
}

static void 
xmlC14NVisibleNsStackShift(xmlC14NVisibleNsStackPtr cur) {
    if(cur == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlC14NVisibleNsStackRestore: cur is null.\n");
#endif
	return;
    }
    cur->nsPrevStart = cur->nsPrevEnd;
    cur->nsPrevEnd = cur->nsCurEnd;
}

static int
xmlC14NStrEqual(const xmlChar *str1, const xmlChar *str2) {
    if (str1 == str2) return(1);
    if (str1 == NULL) return((*str2) == '\0');
    if (str2 == NULL) return((*str1) == '\0');
    do {
	if (*str1++ != *str2) return(0);
    } while (*str2++);
    return(1);
}

/**
 * xmlC14NVisibleNsStackFind:
 * @ctx:		the C14N context 
 * @ns:			the namespace to check
 *
 * Checks whether the given namespace was already rendered or not
 *
 * Returns 1 if we already wrote this namespace or 0 otherwise
 */
static int
xmlC14NVisibleNsStackFind(xmlC14NVisibleNsStackPtr cur, xmlNsPtr ns)
{
    int i;
    const xmlChar *prefix;
    const xmlChar *href;
    int has_empty_ns;
        
    if(cur == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlC14NVisibleNsStackFind: cur is null.\n");
#endif
        return (0);
    }

    /*
     * if the default namespace xmlns="" is not defined yet then 
     * we do not want to print it out
     */
    prefix = ((ns == NULL) || (ns->prefix == NULL)) ? BAD_CAST "" : ns->prefix;
    href = ((ns == NULL) || (ns->href == NULL)) ? BAD_CAST "" : ns->href;
    has_empty_ns = (xmlC14NStrEqual(prefix, NULL) && xmlC14NStrEqual(href, NULL));

    if (cur->nsTab != NULL) {
	int start = (has_empty_ns) ? 0 : cur->nsPrevStart;
        for (i = cur->nsCurEnd - 1; i >= start; --i) {
            xmlNsPtr ns1 = cur->nsTab[i];
	    
	    if(xmlC14NStrEqual(prefix, (ns1 != NULL) ? ns1->prefix : NULL)) {
		return(xmlC14NStrEqual(href, (ns1 != NULL) ? ns1->href : NULL));
	    }
        }
    }
    return(has_empty_ns);
}

static int			
xmlExcC14NVisibleNsStackFind(xmlC14NVisibleNsStackPtr cur, xmlNsPtr ns, xmlC14NCtxPtr ctx) {
    int i;
    const xmlChar *prefix;
    const xmlChar *href;
    int has_empty_ns;
        
    if(cur == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
            "xmlExcC14NVisibleNsStackFind: cur is null.\n");
#endif
        return (0);
    }

    /*
     * if the default namespace xmlns="" is not defined yet then 
     * we do not want to print it out
     */
    prefix = ((ns == NULL) || (ns->prefix == NULL)) ? BAD_CAST "" : ns->prefix;
    href = ((ns == NULL) || (ns->href == NULL)) ? BAD_CAST "" : ns->href;
    has_empty_ns = (xmlC14NStrEqual(prefix, NULL) && xmlC14NStrEqual(href, NULL));

    if (cur->nsTab != NULL) {
	int start = 0;
        for (i = cur->nsCurEnd - 1; i >= start; --i) {
            xmlNsPtr ns1 = cur->nsTab[i];
	    
	    if(xmlC14NStrEqual(prefix, (ns1 != NULL) ? ns1->prefix : NULL)) {
		if(xmlC14NStrEqual(href, (ns1 != NULL) ? ns1->href : NULL)) {
	    	    return(xmlC14NIsVisible(ctx, ns1, cur->nodeTab[i]));
		} else {
		    return(0);
		}
	    }
        }
    }
    return(has_empty_ns);
}




/**
 * xmlC14NIsXmlNs:
 * @ns: 		the namespace to check
 *  		
 * Checks whether the given namespace is a default "xml:" namespace
 * with href="http://www.w3.org/XML/1998/namespace"
 *
 * Returns 1 if the node is default or 0 otherwise
 */

/* todo: make it a define? */
static int
xmlC14NIsXmlNs(xmlNsPtr ns)
{
    return ((ns != NULL) &&
            (xmlStrEqual(ns->prefix, BAD_CAST "xml")) &&
            (xmlStrEqual(ns->href,
                         BAD_CAST
                         "http://www.w3.org/XML/1998/namespace")));
}


/**
 * xmlC14NNsCompare:
 * @ns1:		the pointer to first namespace
 * @ns2: 		the pointer to second namespace
 *
 * Compares the namespaces by names (prefixes).
 *
 * Returns -1 if ns1 < ns2, 0 if ns1 == ns2 or 1 if ns1 > ns2.
 */
static int
xmlC14NNsCompare(xmlNsPtr ns1, xmlNsPtr ns2)
{
    if (ns1 == ns2)
        return (0);
    if (ns1 == NULL)
        return (-1);
    if (ns2 == NULL)
        return (1);

    return (xmlStrcmp(ns1->prefix, ns2->prefix));
}


/**
 * xmlC14NPrintNamespaces:
 * @ns:			the pointer to namespace
 * @ctx: 		the C14N context
 *
 * Prints the given namespace to the output buffer from C14N context.
 *
 * Returns 1 on success or 0 on fail.
 */
static int
xmlC14NPrintNamespaces(const xmlNsPtr ns, xmlC14NCtxPtr ctx)
{

    if ((ns == NULL) || (ctx == NULL)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NPrintNamespace: namespace or context pointer is null\n");
#endif
        return 0;
    }

    if (ns->prefix != NULL) {
        xmlOutputBufferWriteString(ctx->buf, " xmlns:");
        xmlOutputBufferWriteString(ctx->buf, (const char *) ns->prefix);
        xmlOutputBufferWriteString(ctx->buf, "=\"");
    } else {
        xmlOutputBufferWriteString(ctx->buf, " xmlns=\"");
    }
    if(ns->href != NULL) {
	xmlOutputBufferWriteString(ctx->buf, (const char *) ns->href);
    }
    xmlOutputBufferWriteString(ctx->buf, "\"");
    return (1);
}

/**
 * xmlC14NProcessNamespacesAxis:
 * @ctx: 		the C14N context
 * @node:		the current node
 *
 * Prints out canonical namespace axis of the current node to the
 * buffer from C14N context as follows 
 *
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Namespace Axis
 * Consider a list L containing only namespace nodes in the 
 * axis and in the node-set in lexicographic order (ascending). To begin 
 * processing L, if the first node is not the default namespace node (a node 
 * with no namespace URI and no local name), then generate a space followed 
 * by xmlns="" if and only if the following conditions are met:
 *    - the element E that owns the axis is in the node-set
 *    - The nearest ancestor element of E in the node-set has a default 
 *	    namespace node in the node-set (default namespace nodes always 
 *      have non-empty values in XPath)
 * The latter condition eliminates unnecessary occurrences of xmlns="" in 
 * the canonical form since an element only receives an xmlns="" if its 
 * default namespace is empty and if it has an immediate parent in the 
 * canonical form that has a non-empty default namespace. To finish 
 * processing  L, simply process every namespace node in L, except omit 
 * namespace node with local name xml, which defines the xml prefix, 
 * if its string value is http://www.w3.org/XML/1998/namespace.
 *
 * Exclusive XML Canonicalization v 1.0 (http://www.w3.org/TR/xml-exc-c14n)
 * Canonical XML applied to a document subset requires the search of the 
 * ancestor nodes of each orphan element node for attributes in the xml 
 * namespace, such as xml:lang and xml:space. These are copied into the 
 * element node except if a declaration of the same attribute is already 
 * in the attribute axis of the element (whether or not it is included in 
 * the document subset). This search and copying are omitted from the 
 * Exclusive XML Canonicalization method.
 *
 * Returns 0 on success or -1 on fail.
 */
static int
xmlC14NProcessNamespacesAxis(xmlC14NCtxPtr ctx, xmlNodePtr cur, int visible)
{
    xmlNodePtr n;
    xmlNsPtr ns, tmp;
    xmlListPtr list;
    int already_rendered;
    int has_empty_ns = 0;
    
    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NProcessNamespacesAxis: Null context or node pointer or type != XML_ELEMENT_NODE.\n");
#endif
        return (-1);
    }

    /*
     * Create a sorted list to store element namespaces
     */
    list = xmlListCreate(NULL, (xmlListDataCompare) xmlC14NNsCompare);
    if (list == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NProcessNamespacesAxis: list creation failed\n");
#endif
        return (-1);
    }

    /* check all namespaces */
    for(n = cur; n != NULL; n = n->parent) {
	for(ns = n->nsDef; ns != NULL; ns = ns->next) {
	    tmp = xmlSearchNs(cur->doc, cur, ns->prefix);
	    
	    if((tmp == ns) && !xmlC14NIsXmlNs(ns) && xmlC14NIsVisible(ctx, ns, cur)) {
		already_rendered = xmlC14NVisibleNsStackFind(ctx->ns_rendered, ns);
		if(visible) {
        	    xmlC14NVisibleNsStackAdd(ctx->ns_rendered, ns, cur);
		}
		if(!already_rendered) {
		    xmlListInsert(list, ns); 
		}
    		if(xmlStrlen(ns->prefix) == 0) {
		    has_empty_ns = 1;
		}
	    }
	}
    }
	
    /**
     * if the first node is not the default namespace node (a node with no 
     * namespace URI and no local name), then generate a space followed by 
     * xmlns="" if and only if the following conditions are met:
     *  - the element E that owns the axis is in the node-set
     *  - the nearest ancestor element of E in the node-set has a default 
     *     namespace node in the node-set (default namespace nodes always 
     *     have non-empty values in XPath)
     */
    if(visible && !has_empty_ns) {
        static xmlNs ns_default;

        memset(&ns_default, 0, sizeof(ns_default));
        if(!xmlC14NVisibleNsStackFind(ctx->ns_rendered, &ns_default)) {
    	    xmlC14NPrintNamespaces(&ns_default, ctx);
	}
    }
	
    
    /* 
     * print out all elements from list 
     */
    xmlListWalk(list, (xmlListWalker) xmlC14NPrintNamespaces, (const void *) ctx);

    /* 
     * Cleanup
     */
    xmlListDelete(list);
    return (0);
}


/**
 * xmlExcC14NProcessNamespacesAxis:
 * @ctx: 		the C14N context
 * @node:		the current node
 *
 * Prints out exclusive canonical namespace axis of the current node to the
 * buffer from C14N context as follows 
 *
 * Exclusive XML Canonicalization
 * http://www.w3.org/TR/xml-exc-c14n
 *
 * If the element node is in the XPath subset then output the node in 
 * accordance with Canonical XML except for namespace nodes which are 
 * rendered as follows:
 *
 * 1. Render each namespace node iff:
 *    * it is visibly utilized by the immediate parent element or one of 
 *      its attributes, or is present in InclusiveNamespaces PrefixList, and
 *    * its prefix and value do not appear in ns_rendered. ns_rendered is 
 *      obtained by popping the state stack in order to obtain a list of 
 *      prefixes and their values which have already been rendered by 
 *      an output ancestor of the namespace node's parent element.
 * 2. Append the rendered namespace node to the list ns_rendered of namespace 
 * nodes rendered by output ancestors. Push ns_rendered on state stack and 
 * recurse.
 * 3. After the recursion returns, pop thestate stack.
 *
 *
 * Returns 0 on success or -1 on fail.
 */
static int
xmlExcC14NProcessNamespacesAxis(xmlC14NCtxPtr ctx, xmlNodePtr cur, int visible)
{
    xmlNsPtr ns;
    xmlListPtr list;
    xmlAttrPtr attr;
    int already_rendered;
    int has_empty_ns = 0;
    int has_visibly_utilized_empty_ns = 0;
    int has_empty_ns_in_inclusive_list = 0;
        
    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlExcC14NProcessNamespacesAxis: Null context or node pointer or type != XML_ELEMENT_NODE.\n");
#endif
        return (-1);
    }

    if(!ctx->exclusive) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlExcC14NProcessNamespacesAxis: called for non-exclusive canonization or rendered stack is NULL.\n");
#endif
        return (-1);

    }

    /*
     * Create a sorted list to store element namespaces
     */
    list = xmlListCreate(NULL, (xmlListDataCompare) xmlC14NNsCompare);
    if (list == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlExcC14NProcessNamespacesAxis: list creation failed\n");
#endif
        return (-1);
    }

    /* 
     * process inclusive namespaces:
     * All namespace nodes appearing on inclusive ns list are 
     * handled as provided in Canonical XML
     */
    if(ctx->inclusive_ns_prefixes != NULL) {
	xmlChar *prefix; 
	int i;
	
	for (i = 0; ctx->inclusive_ns_prefixes[i] != NULL; ++i) {
	    prefix = ctx->inclusive_ns_prefixes[i];
	    /*
	     * Special values for namespace with empty prefix
	     */
            if (xmlStrEqual(prefix, BAD_CAST "#default")
                || xmlStrEqual(prefix, BAD_CAST "")) {
                prefix = NULL;
		has_empty_ns_in_inclusive_list = 1;
            }
	
	    ns = xmlSearchNs(cur->doc, cur, prefix);	    
	    if((ns != NULL) && !xmlC14NIsXmlNs(ns) && xmlC14NIsVisible(ctx, ns, cur)) {
		already_rendered = xmlC14NVisibleNsStackFind(ctx->ns_rendered, ns);
		if(visible) {
    	    	    xmlC14NVisibleNsStackAdd(ctx->ns_rendered, ns, cur);
		}
		if(!already_rendered) {
	    	    xmlListInsert(list, ns); 
		}
    		if(xmlStrlen(ns->prefix) == 0) {
		    has_empty_ns = 1;
		}
	    }
	}
    }
    
    /* add node namespace */
    if(cur->ns != NULL) {
	ns = cur->ns;
    } else {
        ns = xmlSearchNs(cur->doc, cur, NULL);
	has_visibly_utilized_empty_ns = 1;
    }
    if((ns != NULL) && !xmlC14NIsXmlNs(ns)) {
	if(visible && xmlC14NIsVisible(ctx, ns, cur)) { 
	    if(!xmlExcC14NVisibleNsStackFind(ctx->ns_rendered, ns, ctx)) {
		xmlListInsert(list, ns);
	    }
	}
	if(visible) {
    	    xmlC14NVisibleNsStackAdd(ctx->ns_rendered, ns, cur); 
	}
	if(xmlStrlen(ns->prefix) == 0) {
	    has_empty_ns = 1;
	}
    }
    
        
    /* add attributes */
    for(attr = cur->properties; attr != NULL; attr = attr->next) {
        /* 
         * we need to check that attribute is visible and has non
         * default namespace (XML Namespaces: "default namespaces 
    	 * do not apply directly to attributes")	 
         */
	if((attr->ns != NULL) && xmlC14NIsVisible(ctx, attr, cur)) {
	    already_rendered = xmlExcC14NVisibleNsStackFind(ctx->ns_rendered, attr->ns, ctx);
	    xmlC14NVisibleNsStackAdd(ctx->ns_rendered, attr->ns, cur); 
	    if(!already_rendered && visible) {
		xmlListInsert(list, attr->ns); 
	    }
	    if(xmlStrlen(attr->ns->prefix) == 0) {
		has_empty_ns = 1;
	    }
	} else if(attr->ns == NULL) {
	    has_visibly_utilized_empty_ns = 1;
	}
    }

    /*
     * Process xmlns=""
     */
    if(visible && has_visibly_utilized_empty_ns && 
	    !has_empty_ns && !has_empty_ns_in_inclusive_list) {
        static xmlNs ns_default;

        memset(&ns_default, 0, sizeof(ns_default));
	
        already_rendered = xmlExcC14NVisibleNsStackFind(ctx->ns_rendered, &ns_default, ctx);
	if(!already_rendered) {
    	    xmlC14NPrintNamespaces(&ns_default, ctx);
	}
    } else if(visible && !has_empty_ns && has_empty_ns_in_inclusive_list) {
        static xmlNs ns_default;

        memset(&ns_default, 0, sizeof(ns_default));
        if(!xmlC14NVisibleNsStackFind(ctx->ns_rendered, &ns_default)) {
    	    xmlC14NPrintNamespaces(&ns_default, ctx);
	}
    }

    

    /* 
     * print out all elements from list 
     */
    xmlListWalk(list, (xmlListWalker) xmlC14NPrintNamespaces, (const void *) ctx);

    /* 
     * Cleanup
     */
    xmlListDelete(list);
    return (0);
}


/**
 * xmlC14NAttrsCompare:
 * @attr1:		the pointer tls o first attr
 * @attr2: 		the pointer to second attr
 *
 * Prints the given attribute to the output buffer from C14N context.
 *
 * Returns -1 if attr1 < attr2, 0 if attr1 == attr2 or 1 if attr1 > attr2.
 */
static int
xmlC14NAttrsCompare(xmlAttrPtr attr1, xmlAttrPtr attr2)
{
    int ret = 0;

    /*
     * Simple cases
     */
    if (attr1 == attr2)
        return (0);
    if (attr1 == NULL)
        return (-1);
    if (attr2 == NULL)
        return (1);
    if (attr1->ns == attr2->ns) {
        return (xmlStrcmp(attr1->name, attr2->name));
    }

    /* 
     * Attributes in the default namespace are first
     * because the default namespace is not applied to
     * unqualified attributes
     */
    if (attr1->ns == NULL)
        return (-1);
    if (attr2->ns == NULL)
        return (1);
    if (attr1->ns->prefix == NULL)
        return (-1);
    if (attr2->ns->prefix == NULL)
        return (1);

    ret = xmlStrcmp(attr1->ns->href, attr2->ns->href);
    if (ret == 0) {
        ret = xmlStrcmp(attr1->name, attr2->name);
    }
    return (ret);
}


/**
 * xmlC14NPrintAttrs:
 * @attr:		the pointer to attr
 * @ctx: 		the C14N context
 *
 * Prints out canonical attribute urrent node to the
 * buffer from C14N context as follows 
 *
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Returns 1 on success or 0 on fail.
 */
static int
xmlC14NPrintAttrs(const xmlAttrPtr attr, xmlC14NCtxPtr ctx)
{
    xmlChar *value;
    xmlChar *buffer;

    if ((attr == NULL) || (ctx == NULL)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NPrintAttrs: attr == NULL or ctx == NULL\n");
#endif
        return (0);
    }

    xmlOutputBufferWriteString(ctx->buf, " ");
    if (attr->ns != NULL && xmlStrlen(attr->ns->prefix) > 0) {
        xmlOutputBufferWriteString(ctx->buf,
                                   (const char *) attr->ns->prefix);
        xmlOutputBufferWriteString(ctx->buf, ":");
    }
    xmlOutputBufferWriteString(ctx->buf, (const char *) attr->name);
    xmlOutputBufferWriteString(ctx->buf, "=\"");

    value = xmlNodeListGetString(attr->doc, attr->children, 1);
    /* todo: should we log an error if value==NULL ? */
    if (value != NULL) {
        buffer = xmlC11NNormalizeAttr(value);
        xmlFree(value);
        if (buffer != NULL) {
            xmlOutputBufferWriteString(ctx->buf, (const char *) buffer);
            xmlFree(buffer);
        } else {
#ifdef DEBUG_C14N
            xmlGenericError(xmlGenericErrorContext,
                            "xmlC14NPrintAttrs: xmlC11NNormalizeAttr failed\n");
#endif
            return (0);
        }
    }
    xmlOutputBufferWriteString(ctx->buf, "\"");
    return (1);
}

/**
 * xmlC14NProcessAttrsAxis:
 * @ctx: 		the C14N context
 * @cur:		the current node
 *
 * Prints out canonical attribute axis of the current node to the
 * buffer from C14N context as follows 
 *
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Attribute Axis 
 * In lexicographic order (ascending), process each node that 
 * is in the element's attribute axis and in the node-set.
 * 
 * The processing of an element node E MUST be modified slightly 
 * when an XPath node-set is given as input and the element's 
 * parent is omitted from the node-set.
 *
 *
 * Exclusive XML Canonicalization v 1.0 (http://www.w3.org/TR/xml-exc-c14n)
 *
 * Canonical XML applied to a document subset requires the search of the 
 * ancestor nodes of each orphan element node for attributes in the xml 
 * namespace, such as xml:lang and xml:space. These are copied into the 
 * element node except if a declaration of the same attribute is already 
 * in the attribute axis of the element (whether or not it is included in 
 * the document subset). This search and copying are omitted from the 
 * Exclusive XML Canonicalization method.
 *
 * Returns 0 on success or -1 on fail.
 */
static int
xmlC14NProcessAttrsAxis(xmlC14NCtxPtr ctx, xmlNodePtr cur)
{
    xmlAttrPtr attr;
    xmlListPtr list;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NProcessAttrsAxis: Null context or node pointer or type != XML_ELEMENT_NODE.\n");
#endif
        return (-1);
    }

    /*
     * Create a sorted list to store element attributes
     */
    list = xmlListCreate(NULL, (xmlListDataCompare) xmlC14NAttrsCompare);
    if (list == NULL) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NProcessAttrsAxis: list creation failed\n");
#endif
        return (-1);
    }

    /* 
     * Add all visible attributes from current node. 
     */
    attr = cur->properties;
    while (attr != NULL) {
        /* check that attribute is visible */
        if (xmlC14NIsVisible(ctx, attr, cur)) {
            xmlListInsert(list, attr);
        }
        attr = attr->next;
    }

    /* 
     * include attributes in "xml" namespace defined in ancestors
     * (only for non-exclusive XML Canonicalization)
     */
    if ((!ctx->exclusive) && (cur->parent != NULL)
        && (!xmlC14NIsVisible(ctx, cur->parent, cur->parent->parent))) {
        /*
         * If XPath node-set is not specified then the parent is always 
         * visible!
         */
        cur = cur->parent;
        while (cur != NULL) {
            attr = cur->properties;
            while (attr != NULL) {
                if ((attr->ns != NULL)
                    && (xmlStrEqual(attr->ns->prefix, BAD_CAST "xml"))) {
                    if (xmlListSearch(list, attr) == NULL) {
                        xmlListInsert(list, attr);
                    }
                }
                attr = attr->next;
            }
            cur = cur->parent;
        }
    }

    /* 
     * print out all elements from list 
     */
    xmlListWalk(list, (xmlListWalker) xmlC14NPrintAttrs, (const void *) ctx);

    /* 
     * Cleanup
     */
    xmlListDelete(list);
    return (0);
}

/** 
 * xmlC14NCheckForRelativeNamespaces:
 * @ctx:		the C14N context
 * @cur:		the current element node
 *
 * Checks that current element node has no relative namespaces defined
 *
 * Returns 0 if the node has no relative namespaces or -1 otherwise.
 */
static int
xmlC14NCheckForRelativeNamespaces(xmlC14NCtxPtr ctx, xmlNodePtr cur)
{
    xmlNsPtr ns;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
#ifdef DEBUG_C14N
        xmlGenericError(xmlGenericErrorContext,
                        "xmlC14NCheckForRelativeNamespaces: Null context or node pointer or type != XML_ELEMENT_NODE.\n");
#endif
        return (-1);
    }

    ns = cur->nsDef;
    while (ns != NULL) {
        if (xmlStrlen(ns->href) > 0) {
            xmlURIPtr uri;

            uri = xmlParseURI((const char *) ns->href);
            if (uri == NULL) {
#ifdef DEBUG_C14N
                xmlGenericError(xmlGenericErrorContext,
                                "xmlC14NCheckForRelativeNamespaces: unable to parse uri=\"%s\".\n",
                                ns->href);
#endif
                return (-1);
            }
            if (xmlStrlen((const xmlChar *) uri->scheme) == 0) {
                xmlFreeURI(uri);
                return (-1);
            }
            if ((!xmlStrEqual
                 ((const xmlChar *) uri->scheme, BAD_CAST "urn"))
                && (xmlStrlen((const xmlChar *) uri->server) == 0)) {
                xmlFreeURI(uri);
                return (-1);
            }
            xmlFreeURI(uri);
        }
        ns = ns->next;
    }
    return (0);
}

/**
 * xmlC14NProcessElementNode:
 * @ctx: 		the pointer to C14N context object
 * @cur:		the node to process
 *  		
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Element Nodes
 * If the element is not in the node-set, then the result is obtained 
 * by processing the namespace axis, then the attribute axis, then 
 * processing the child nodes of the element that are in the node-set 
 * (in document order). If the element is in the node-set, then the result 
 * is an open angle bracket (<), the element QName, the result of 
 * processing the namespace axis, the result of processing the attribute 
 * axis, a close angle bracket (>), the result of processing 