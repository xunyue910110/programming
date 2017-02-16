/*
 * xpointer.c : Code to handle XML Pointer
 *
 * Base implementation was made accordingly to
 * W3C Candidate Recommendation 7 June 2000
 * http://www.w3.org/TR/2000/CR-xptr-20000607
 *
 * Added support for the element() scheme described in:
 * W3C Proposed Recommendation 13 November 2002
 * http://www.w3.org/TR/2002/PR-xptr-element-20021113/  
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

/*
 * TODO: better handling of error cases, the full expression should
 *       be parsed beforehand instead of a progressive evaluation
 * TODO: Access into entities references are not supported now ...
 *       need a start to be able to pop out of entities refs since
 *       parent is the endity declaration, not the ref.
 */

#include <string.h>
#include <libxml/xpointer.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlerror.h>
#include <libxml/globals.h>

#ifdef LIBXML_XPTR_ENABLED

/* Add support of the xmlns() xpointer scheme to initialize the namespaces */
#define XPTR_XMLNS_SCHEME

/* #define DEBUG_RANGES */
#ifdef DEBUG_RANGES
#ifdef LIBXML_DEBUG_ENABLED
#include <libxml/debugXML.h>
#endif
#endif

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define STRANGE 							\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Internal error at %s:%d\n",				\
            __FILE__, __LINE__);

/************************************************************************
 *									*
 * 		Some factorized error routines				*
 *									*
 ************************************************************************/

/**
 * xmlXPtrErrMemory:
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
xmlXPtrErrMemory(const char *extra)
{
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_XPOINTER,
		    XML_ERR_NO_MEMORY, XML_ERR_ERROR, NULL, 0, extra,
		    NULL, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}

/**
 * xmlXPtrErr:
 * @ctxt:  an XPTR evaluation context
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
xmlXPtrErr(xmlXPathParserContextPtr ctxt, int error,
           const char * msg, const xmlChar *extra)
{
    if (ctxt != NULL)
        ctxt->error = error;
    if ((ctxt == NULL) || (ctxt->context == NULL)) {
	__xmlRaiseError(NULL, NULL, NULL,
			NULL, NULL, XML_FROM_XPOINTER, error,
			XML_ERR_ERROR, NULL, 0,
			(const char *) extra, NULL, NULL, 0, 0,
			msg, extra);
	return;
    }
    ctxt->context->lastError.domain = XML_FROM_XPOINTER;
    ctxt->context->lastError.code = error;
    ctxt->context->lastError.level = XML_ERR_ERROR;
    ctxt->context->lastError.str1 = (char *) xmlStrdup(ctxt->base);
    ctxt->context->lastError.int1 = ctxt->cur - ctxt->base;
    ctxt->context->lastError.node = ctxt->context->debugNode;
    if (ctxt->context->error != NULL) {
	ctxt->context->error(ctxt->context->userData,
	                     &ctxt->context->lastError);
    } else {
	__xmlRaiseError(NULL, NULL, NULL,
			NULL, ctxt->context->debugNode, XML_FROM_XPOINTER,
			error, XML_ERR_ERROR, NULL, 0,
			(const char *) extra, (const char *) ctxt->base, NULL,
			ctxt->cur - ctxt->base, 0,
			msg, extra);
    }
}

/************************************************************************
 *									*
 *		A few helper functions for child sequences		*
 *									*
 ************************************************************************/
/* xmlXPtrAdvanceNode is a private function, but used by xinclude.c */
xmlNodePtr xmlXPtrAdvanceNode(xmlNodePtr cur, int *level);
/**
 * xmlXPtrGetArity:
 * @cur:  the node
 *
 * Returns the number of child for an element, -1 in case of error
 */
static int
xmlXPtrGetArity(xmlNodePtr cur) {
    int i;
    if (cur == NULL) 
	return(-1);
    cur = cur->children;
    for (i = 0;cur != NULL;cur = cur->next) {
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_DOCUMENT_NODE) ||
	    (cur->type == XML_HTML_DOCUMENT_NODE)) {
	    i++;
	}
    }
    return(i);
}

/**
 * xmlXPtrGetIndex:
 * @cur:  the node
 *
 * Returns the index of the node in its parent children list, -1
 *         in case of error
 */
static int
xmlXPtrGetIndex(xmlNodePtr cur) {
    int i;
    if (cur == NULL) 
	return(-1);
    for (i = 1;cur != NULL;cur = cur->prev) {
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_DOCUMENT_NODE) ||
	    (cur->type == XML_HTML_DOCUMENT_NODE)) {
	    i++;
	}
    }
    return(i);
}

/**
 * xmlXPtrGetNthChild:
 * @cur:  the node
 * @no:  the child number
 *
 * Returns the @no'th element child of @cur or NULL
 */
static xmlNodePtr
xmlXPtrGetNthChild(xmlNodePtr cur, int no) {
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

/************************************************************************
 *									*
 *		Handling of XPointer specific types			*
 *									*
 ************************************************************************/

/**
 * xmlXPtrCmpPoints:
 * @node1:  the first node
 * @index1:  the first index
 * @node2:  the second node
 * @index2:  the second index
 *
 * Compare two points w.r.t document order
 *
 * Returns -2 in case of error 1 if first point < second point, 0 if
 *         that's the same point, -1 otherwise
 */
static int
xmlXPtrCmpPoints(xmlNodePtr node1, int index1, xmlNodePtr node2, int index2) {
    if ((node1 == NULL) || (node2 == NULL))
	return(-2);
    /*
     * a couple of optimizations which will avoid computations in most cases
     */
    if (node1 == node2) {
	if (index1 < index2)
	    return(1);
	if (index1 > index2)
	    return(-1);
	return(0);
    }
    return(xmlXPathCmpNodes(node1, node2));
}

/**
 * xmlXPtrNewPoint:
 * @node:  the xmlNodePtr
 * @indx:  the indx within the node
 *
 * Create a new xmlXPathObjectPtr of type point
 *
 * Returns the newly created object.
 */
static xmlXPathObjectPtr
xmlXPtrNewPoint(xmlNodePtr node, int indx) {
    xmlXPathObjectPtr ret;

    if (node == NULL)
	return(NULL);
    if (indx < 0)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating point");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_POINT;
    ret->user = (void *) node;
    ret->index = indx;
    return(ret);
}

/**
 * xmlXPtrRangeCheckOrder:
 * @range:  an object range
 *
 * Make sure the points in the range are in the right order
 */
static void
xmlXPtrRangeCheckOrder(xmlXPathObjectPtr range) {
    int tmp;
    xmlNodePtr tmp2;
    if (range == NULL)
	return;
    if (range->type != XPATH_RANGE)
	return;
    if (range->user2 == NULL)
	return;
    tmp = xmlXPtrCmpPoints((xmlNodePtr)range->user, range->index,
	                     (xmlNodePtr)range->user2, range->index2);
    if (tmp == -1) {
	tmp2 = (xmlNodePtr)range->user;
	range->user = range->user2;
	range->user2 = tmp2;
	tmp = range->index;
	range->index = range->index2;
	range->index2 = tmp;
    }
}

/**
 * xmlXPtrRangesEqual:
 * @range1:  the first range
 * @range2:  the second range
 *
 * Compare two ranges
 *
 * Returns 1 if equal, 0 otherwise
 */
static int
xmlXPtrRangesEqual(xmlXPathObjectPtr range1, xmlXPathObjectPtr range2) {
    if (range1 == range2)
	return(1);
    if ((range1 == NULL) || (range2 == NULL))
	return(0);
    if (range1->type != range2->type)
	return(0);
    if (range1->type != XPATH_RANGE)
	return(0);
    if (range1->user != range2->user)
	return(0);
    if (range1->index != range2->index)
	return(0);
    if (range1->user2 != range2->user2)
	return(0);
    if (range1->index2 != range2->index2)
	return(0);
    return(1);
}

/**
 * xmlXPtrNewRange:
 * @start:  the starting node
 * @startindex:  the start index
 * @end:  the ending point
 * @endindex:  the ending index
 *
 * Create a new xmlXPathObjectPtr of type range
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewRange(xmlNodePtr start, int startindex,
	        xmlNodePtr end, int endindex) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);
    if (end == NULL)
	return(NULL);
    if (startindex < 0)
	return(NULL);
    if (endindex < 0)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start;
    ret->index = startindex;
    ret->user2 = end;
    ret->index2 = endindex;
    xmlXPtrRangeCheckOrder(ret);
    return(ret);
}

/**
 * xmlXPtrNewRangePoints:
 * @start:  the starting point
 * @end:  the ending point
 *
 * Create a new xmlXPathObjectPtr of type range using 2 Points
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewRangePoints(xmlXPathObjectPtr start, xmlXPathObjectPtr end) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);
    if (end == NULL)
	return(NULL);
    if (start->type != XPATH_POINT)
	return(NULL);
    if (end->type != XPATH_POINT)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start->user;
    ret->index = start->index;
    ret->user2 = end->user;
    ret->index2 = end->index;
    xmlXPtrRangeCheckOrder(ret);
    return(ret);
}

/**
 * xmlXPtrNewRangePointNode:
 * @start:  the starting point
 * @end:  the ending node
 *
 * Create a new xmlXPathObjectPtr of type range from a point to a node
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewRangePointNode(xmlXPathObjectPtr start, xmlNodePtr end) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);
    if (end == NULL)
	return(NULL);
    if (start->type != XPATH_POINT)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start->user;
    ret->index = start->index;
    ret->user2 = end;
    ret->index2 = -1;
    xmlXPtrRangeCheckOrder(ret);
    return(ret);
}

/**
 * xmlXPtrNewRangeNodePoint:
 * @start:  the starting node
 * @end:  the ending point
 *
 * Create a new xmlXPathObjectPtr of type range from a node to a point
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewRangeNodePoint(xmlNodePtr start, xmlXPathObjectPtr end) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);
    if (end == NULL)
	return(NULL);
    if (start->type != XPATH_POINT)
	return(NULL);
    if (end->type != XPATH_POINT)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start;
    ret->index = -1;
    ret->user2 = end->user;
    ret->index2 = end->index;
    xmlXPtrRangeCheckOrder(ret);
    return(ret);
}

/**
 * xmlXPtrNewRangeNodes:
 * @start:  the starting node
 * @end:  the ending node
 *
 * Create a new xmlXPathObjectPtr of type range using 2 nodes
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewRangeNodes(xmlNodePtr start, xmlNodePtr end) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);
    if (end == NULL)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start;
    ret->index = -1;
    ret->user2 = end;
    ret->index2 = -1;
    xmlXPtrRangeCheckOrder(ret);
    return(ret);
}

/**
 * xmlXPtrNewCollapsedRange:
 * @start:  the starting and ending node
 *
 * Create a new xmlXPathObjectPtr of type range using a single nodes
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewCollapsedRange(xmlNodePtr start) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start;
    ret->index = -1;
    ret->user2 = NULL;
    ret->index2 = -1;
    return(ret);
}

/**
 * xmlXPtrNewRangeNodeObject:
 * @start:  the starting node
 * @end:  the ending object
 *
 * Create a new xmlXPathObjectPtr of type range from a not to an object
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewRangeNodeObject(xmlNodePtr start, xmlXPathObjectPtr end) {
    xmlXPathObjectPtr ret;

    if (start == NULL)
	return(NULL);
    if (end == NULL)
	return(NULL);
    switch (end->type) {
	case XPATH_POINT:
	case XPATH_RANGE:
	    break;
	case XPATH_NODESET:
	    /*
	     * Empty set ... 
	     */
	    if (end->nodesetval->nodeNr <= 0)
		return(NULL);
	    break;
	default:
	    /* TODO */
	    return(NULL);
    }

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating range");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_RANGE;
    ret->user = start;
    ret->index = -1;
    switch (end->type) {
	case XPATH_POINT:
	    ret->user2 = end->user;
	    ret->index2 = end->index;
	    break;
	case XPATH_RANGE:
	    ret->user2 = end->user2;
	    ret->index2 = end->index2;
	    break;
	case XPATH_NODESET: {
	    ret->user2 = end->nodesetval->nodeTab[end->nodesetval->nodeNr - 1];
	    ret->index2 = -1;
	    break;
	}
	default:
	    STRANGE
	    return(NULL);
    }
    xmlXPtrRangeCheckOrder(ret);
    return(ret);
}

#define XML_RANGESET_DEFAULT	10

/**
 * xmlXPtrLocationSetCreate:
 * @val:  an initial xmlXPathObjectPtr, or NULL
 *
 * Create a new xmlLocationSetPtr of type double and of value @val
 *
 * Returns the newly created object.
 */
xmlLocationSetPtr
xmlXPtrLocationSetCreate(xmlXPathObjectPtr val) {
    xmlLocationSetPtr ret;

    ret = (xmlLocationSetPtr) xmlMalloc(sizeof(xmlLocationSet));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating locationset");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlLocationSet));
    if (val != NULL) {
        ret->locTab = (xmlXPathObjectPtr *) xmlMalloc(XML_RANGESET_DEFAULT *
					     sizeof(xmlXPathObjectPtr));
	if (ret->locTab == NULL) {
	    xmlXPtrErrMemory("allocating locationset");
	    xmlFree(ret);
	    return(NULL);
	}
	memset(ret->locTab, 0 ,
	       XML_RANGESET_DEFAULT * (size_t) sizeof(xmlXPathObjectPtr));
        ret->locMax = XML_RANGESET_DEFAULT;
	ret->locTab[ret->locNr++] = val;
    }
    return(ret);
}

/**
 * xmlXPtrLocationSetAdd:
 * @cur:  the initial range set
 * @val:  a new xmlXPathObjectPtr
 *
 * add a new xmlXPathObjectPtr to an existing LocationSet
 * If the location already exist in the set @val is freed.
 */
void
xmlXPtrLocationSetAdd(xmlLocationSetPtr cur, xmlXPathObjectPtr val) {
    int i;

    if ((cur == NULL) || (val == NULL)) return;

    /*
     * check against doublons
     */
    for (i = 0;i < cur->locNr;i++) {
	if (xmlXPtrRangesEqual(cur->locTab[i], val)) {
	    xmlXPathFreeObject(val);
	    return;
	}
    }

    /*
     * grow the locTab if needed
     */
    if (cur->locMax == 0) {
        cur->locTab = (xmlXPathObjectPtr *) xmlMalloc(XML_RANGESET_DEFAULT *
					     sizeof(xmlXPathObjectPtr));
	if (cur->locTab == NULL) {
	    xmlXPtrErrMemory("adding location to set");
	    return;
	}
	memset(cur->locTab, 0 ,
	       XML_RANGESET_DEFAULT * (size_t) sizeof(xmlXPathObjectPtr));
        cur->locMax = XML_RANGESET_DEFAULT;
    } else if (cur->locNr == cur->locMax) {
        xmlXPathObjectPtr *temp;

        cur->locMax *= 2;
	temp = (xmlXPathObjectPtr *) xmlRealloc(cur->locTab, cur->locMax *
				      sizeof(xmlXPathObjectPtr));
	if (temp == NULL) {
	    xmlXPtrErrMemory("adding location to set");
	    return;
	}
	cur->locTab = temp;
    }
    cur->locTab[cur->locNr++] = val;
}

/**
 * xmlXPtrLocationSetMerge:
 * @val1:  the first LocationSet
 * @val2:  the second LocationSet
 *
 * Merges two rangesets, all ranges from @val2 are added to @val1
 *
 * Returns val1 once extended or NULL in case of error.
 */
xmlLocationSetPtr
xmlXPtrLocationSetMerge(xmlLocationSetPtr val1, xmlLocationSetPtr val2) {
    int i;

    if (val1 == NULL) return(NULL);
    if (val2 == NULL) return(val1);

    /*
     * !!!!! this can be optimized a lot, knowing that both
     *       val1 and val2 already have unicity of their values.
     */

    for (i = 0;i < val2->locNr;i++)
        xmlXPtrLocationSetAdd(val1, val2->locTab[i]);

    return(val1);
}

/**
 * xmlXPtrLocationSetDel:
 * @cur:  the initial range set
 * @val:  an xmlXPathObjectPtr
 *
 * Removes an xmlXPathObjectPtr from an existing LocationSet
 */
void
xmlXPtrLocationSetDel(xmlLocationSetPtr cur, xmlXPathObjectPtr val) {
    int i;

    if (cur == NULL) return;
    if (val == NULL) return;

    /*
     * check against doublons
     */
    for (i = 0;i < cur->locNr;i++)
        if (cur->locTab[i] == val) break;

    if (i >= cur->locNr) {
#ifdef DEBUG
        xmlGenericError(xmlGenericErrorContext, 
	        "xmlXPtrLocationSetDel: Range wasn't found in RangeList\n");
#endif
        return;
    }
    cur->locNr--;
    for (;i < cur->locNr;i++)
        cur->locTab[i] = cur->locTab[i + 1];
    cur->locTab[cur->locNr] = NULL;
}

/**
 * xmlXPtrLocationSetRemove:
 * @cur:  the initial range set
 * @val:  the index to remove
 *
 * Removes an entry from an existing LocationSet list.
 */
void
xmlXPtrLocationSetRemove(xmlLocationSetPtr cur, int val) {
    if (cur == NULL) return;
    if (val >= cur->locNr) return;
    cur->locNr--;
    for (;val < cur->locNr;val++)
        cur->locTab[val] = cur->locTab[val + 1];
    cur->locTab[cur->locNr] = NULL;
}

/**
 * xmlXPtrFreeLocationSet:
 * @obj:  the xmlLocationSetPtr to free
 *
 * Free the LocationSet compound (not the actual ranges !).
 */
void
xmlXPtrFreeLocationSet(xmlLocationSetPtr obj) {
    int i;

    if (obj == NULL) return;
    if (obj->locTab != NULL) {
	for (i = 0;i < obj->locNr; i++) {
            xmlXPathFreeObject(obj->locTab[i]);
	}
	xmlFree(obj->locTab);
    }
    xmlFree(obj);
}

/**
 * xmlXPtrNewLocationSetNodes:
 * @start:  the start NodePtr value
 * @end:  the end NodePtr value or NULL
 *
 * Create a new xmlXPathObjectPtr of type LocationSet and initialize
 * it with the single range made of the two nodes @start and @end
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewLocationSetNodes(xmlNodePtr start, xmlNodePtr end) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating locationset");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_LOCATIONSET;
    if (end == NULL)
	ret->user = xmlXPtrLocationSetCreate(xmlXPtrNewCollapsedRange(start));
    else
	ret->user = xmlXPtrLocationSetCreate(xmlXPtrNewRangeNodes(start,end));
    return(ret);
}

/**
 * xmlXPtrNewLocationSetNodeSet:
 * @set:  a node set
 *
 * Create a new xmlXPathObjectPtr of type LocationSet and initialize
 * it with all the nodes from @set
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrNewLocationSetNodeSet(xmlNodeSetPtr set) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating locationset");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_LOCATIONSET;
    if (set != NULL) {
	int i;
	xmlLocationSetPtr newset;

	newset = xmlXPtrLocationSetCreate(NULL);
	if (newset == NULL)
	    return(ret);

	for (i = 0;i < set->nodeNr;i++)
	    xmlXPtrLocationSetAdd(newset,
		        xmlXPtrNewCollapsedRange(set->nodeTab[i]));

	ret->user = (void *) newset;
    }
    return(ret);
}

/**
 * xmlXPtrWrapLocationSet:
 * @val:  the LocationSet value
 *
 * Wrap the LocationSet @val in a new xmlXPathObjectPtr
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPtrWrapLocationSet(xmlLocationSetPtr val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlXPtrErrMemory("allocating locationset");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_LOCATIONSET;
    ret->user = (void *) val;
    return(ret);
}

/************************************************************************
 *									*
 *			The parser					*
 *									*
 ************************************************************************/

static void xmlXPtrEvalChildSeq(xmlXPathParserContextPtr ctxt, xmlChar *name);

/*
 * Macros for accessing the content. Those should be used only by the parser,
 * and not exported.
 *
 * Dirty macros, i.e. one need to make assumption on the context to use them
 *
 *   CUR_PTR return the current pointer to the xmlChar to be parsed.
 *   CUR     returns the current xmlChar value, i.e. a 8 bit value
 *           in ISO-Latin or UTF-8.
 *           This should be used internally by the parser
 *           only to compare to ASCII values otherwise it would break when
 *           running with UTF-8 encoding.
 *   NXT(n)  returns the n'th next xmlChar. Same as CUR is should be used only
 *           to compare on ASCII based substring.
 *   SKIP(n) Skip n xmlChar, and must also be used only to skip ASCII defined
 *           strings within the parser.
 *   CURRENT Returns the current char value, with the full decoding of
 *           UTF-8 if we are using this mode. It returns an int.
 *   NEXT    Skip to the next character, this does the proper decoding
 *           in UTF-8 mode. It also pop-up unfinished entities on the fly.
 *           It returns the pointer to the current xmlChar.
 */

#define CUR (*ctxt->cur)
#define SKIP(val) ctxt->cur += (val)
#define NXT(val) ctxt->cur[(val)]
#define CUR_PTR ctxt->cur

#define SKIP_BLANKS 							\
    while (IS_BLANK_CH(*(ctxt->cur))) NEXT

#define CURRENT (*ctxt->cur)
#define NEXT ((*ctxt->cur) ?  ctxt->cur++: ctxt->cur)

/*
 * xmlXPtrGetChildNo:
 * @ctxt:  the XPointer Parser context
 * @index:  the child number
 *
 * Move the current node of the nodeset on the stack to the
 * given child if found
 */
static void
xmlXPtrGetChildNo(xmlXPathParserContextPtr ctxt, int indx) {
    xmlNodePtr cur = NULL;
    xmlXPathObjectPtr obj;
    xmlNodeSetPtr oldset;

    CHECK_TYPE(XPATH_NODESET);
    obj = valuePop(ctxt);
    oldset = obj->nodesetval;
    if ((indx <= 0) || (oldset == NULL) || (oldset->nodeNr != 1)) {
	xmlXPathFreeObject(obj);
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
    }
    cur = xmlXPtrGetNthChild(oldset->nodeTab[0], indx);
    if (cur == NULL) {
	xmlXPathFreeObject(obj);
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
    }
    oldset->nodeTab[0] = cur;
    valuePush(ctxt, obj);
}

/**
 * xmlXPtrEvalXPtrPart:
 * @ctxt:  the XPointer Parser context
 * @name:  the preparsed Scheme for the XPtrPart
 * 
 * XPtrPart ::= 'xpointer' '(' XPtrExpr ')'
 *            | Scheme '(' SchemeSpecificExpr ')'
 *
 * Scheme   ::=  NCName - 'xpointer' [VC: Non-XPointer schemes]
 *
 * SchemeSpecificExpr ::= StringWithBalancedParens
 *
 * StringWithBalancedParens ::=  
 *              [^()]* ('(' StringWithBalancedParens ')' [^()]*)*
 *              [VC: Parenthesis escaping]
 *
 * XPtrExpr ::= Expr [VC: Parenthesis escaping]
 *
 * VC: Parenthesis escaping:
 *   The end of an XPointer part is signaled by the right parenthesis ")"
 *   character that is balanced with the left parenthesis "(" character
 *   that began the part. Any unbalanced parenthesis character inside the
 *   expression, even within literals, must be escaped with a circumflex (^)
 *   character preceding it. If the expression contains any literal
 *   occurrences of the circumflex, each must be escaped with an additional
 *   circumflex (that is, ^^). If the unescaped parentheses in the expression
 *   are not balanced, a syntax error results.
 *
 * Parse and evaluate an XPtrPart. Basically it generates the unescaped
 * string and if the scheme is 'xpointer' it will call the XPath interpreter.
 * 
 * TODO: there is no new scheme registration mechanism
 */

static void
xmlXPtrEvalXPtrPart(xmlXPathParserContextPtr ctxt, xmlChar *name) {
    xmlChar *buffer, *cur;
    int len;
    int level;

    if (name == NULL)
    name = xmlXPathParseName(ctxt);
    if (name == NULL)
	XP_ERROR(XPATH_EXPR_ERROR);

    if (CUR != '(')
	XP_ERROR(XPATH_EXPR_ERROR);
    NEXT;
    level = 1;

    len = xmlStrlen(ctxt->cur);
    len++;
    buffer = (xmlChar *) xmlMallocAtomic(len * sizeof (xmlChar));
    if (buffer == NULL) {
        xmlXPtrErrMemory("allocating buffer");
	return;
    }

    cur = buffer;
    while (CUR != 0) {
	if (CUR == ')') {
	    level--;
	    if (level == 0) {
		NEXT;
		break;
	    }
	    *cur++ = CUR;
	} else if (CUR == '(') {
	    level++;
	    *cur++ = CUR;
	} else if (CUR == '^') {
	    NEXT;
	    if ((CUR == ')') || (CUR == '(') || (CUR == '^')) {
		*cur++ = CUR;
	    } else {
		*cur++ = '^';
		*cur++ = CUR;
	    }
	} else {
	    *cur++ = CUR;
	}
	NEXT;
    }
    *cur = 0;

    if ((level != 0) && (CUR == 0)) {
	xmlFree(buffer);
	XP_ERROR(XPTR_SYNTAX_ERROR);
    }

    if (xmlStrEqual(name, (xmlChar *) "xpointer")) {
	const xmlChar *left = CUR_PTR;

	CUR_PTR = buffer;
	/*
	 * To evaluate an xpointer scheme element (4.3) we need:
	 *   context initialized to the root
	 *   context position initalized to 1
	 *   context size initialized to 1
	 */
	ctxt->context->node = (xmlNodePtr)ctxt->context->doc;
	ctxt->context->proximityPosition = 1;
	ctxt->context->contextSize = 1;
	xmlXPathEvalExpr(ctxt);
	CUR_PTR=left;
    } else if (xmlStrEqual(name, (xmlChar *) "element")) {
	const xmlChar *left = CUR_PTR;
	xmlChar *name2;

	CUR_PTR = buffer;
	if (buffer[0] == '/') {
	    xmlXPathRoot(ctxt);
	    xmlXPtrEvalChildSeq(ctxt, NULL);
	} else {
	    name2 = xmlXPathParseName(ctxt);
	    if (name2 == NULL) {
		CUR_PTR = left;
		xmlFree(buffer);
		XP_ERROR(XPATH_EXPR_ERROR);
	    }
	    xmlXPtrEvalChildSeq(ctxt, name2);
	}
	CUR_PTR = left;
#ifdef XPTR_XMLNS_SCHEME
    } else if (xmlStrEqual(name, (xmlChar *) "xmlns")) {
	const xmlChar *left = CUR_PTR;
	xmlChar *prefix;
	xmlChar *URI;
	xmlURIPtr value;

	CUR_PTR = buffer;
        prefix = xmlXPathParseNCName(ctxt);
	if (prefix == NULL) {
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPTR_SYNTAX_ERROR);
	}
	SKIP_BLANKS;
	if (CUR != '=') {
	    xmlFree(prefix);
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPTR_SYNTAX_ERROR);
	}
	NEXT;
	SKIP_BLANKS;
	/* @@ check escaping in the XPointer WD */

	value = xmlParseURI((const char *)ctxt->cur);
	if (value == NULL) {
	    xmlFree(prefix);
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPTR_SYNTAX_ERROR);
	}
	URI = xmlSaveUri(value);
	xmlFreeURI(value);
	if (URI == NULL) {
	    xmlFree(prefix);
	    xmlFree(buffer);
	    xmlFree(name);
	    XP_ERROR(XPATH_MEMORY_ERROR);
	}
	
	xmlXPathRegisterNs(ctxt->context, prefix, URI);
	CUR_PTR = left;
	xmlFree(URI);
	xmlFree(prefix);
#endif /* XPTR_XMLNS_SCHEME */
    } else {
        xmlXPtrErr(ctxt, XML_XPTR_UNKNOWN_SCHEME,
		   "unsupported scheme '%s'\n", name);
    }
    xmlFree(buffer);
    xmlFree(name);
}

/**
 * xmlXPtrEvalFullXPtr:
 * @ctxt:  the XPointer Parser context
 * @name:  the preparsed Scheme for the first XPtrPart
 *
 * FullXPtr ::= XPtrPart (S? XPtrPart)*
 *
 * As the specs says:
 * -----------
 * When multiple XPtrParts are provided, they must be evaluated in
 * left-to-right order. If evaluation of one part fails, the nexti
 * is evaluated. The following conditions cause XPointer part failure:
 *
 * - An unknown scheme
 * - A scheme that does not locate any sub-resource present in the resource
 * - A scheme that is not applicable to the media type of the resource
 *
 * The XPointer application must consume a failed XPointer part and
 * attempt to evaluate the next one, if any. The result of the first
 * XPointer part whose evaluation succeeds is taken to be the fragment
 * located by the XPointer as a whole. If all the parts fail, the result
 * for the XPointer as a whole is a sub-resource error.
 * -----------
 *
 * Parse and evaluate a Full XPtr i.e. possibly a cascade of XPath based
 * expressions or other schemes.
 */
static void
xmlXPtrEvalFullXPtr(xmlXPathParserContextPtr ctxt, xmlChar *name) {
    if (name == NULL)
    name = xmlXPathParseName(ctxt);
    if (name == NULL)
	XP_ERROR(XPATH_EXPR_ERROR);
    while (name != NULL) {
	xmlXPtrEvalXPtrPart(ctxt, name);

	/* in case of syntax error, break here */
	if (ctxt->error != XPATH_EXPRESSION_OK)
	    return;

	/*
	 * If the returned value is a non-empty nodeset
	 * or location set, return here.
	 */
	if (ctxt->value != NULL) {
	    xmlXPathObjectPtr obj = ctxt->value;

	    switch (obj->type) {
		case XPATH_LOCATIONSET: {
		    xmlLocationSetPtr loc = (xmlLocationSetPtr)ctxt->value->user;
		    if ((loc != NULL) && (loc->locNr > 0))
			return;
		    break;
		}
		case XPATH_NODESET: {
		    xmlNodeSetPtr loc = ctxt->value->nodesetval;
		    if ((loc != NULL) && (loc->nodeNr > 0))
			return;
		    break;
		}
		default:
		    break;
	    }

	    /*
	     * Evaluating to improper values is equivalent to
	     * a sub-resource error, clean-up the stack
	     */
	    do {
		obj = valuePop(ctxt);
		if (obj != NULL) {
		    xmlXPathFreeObject(obj);
		}
	    } while (obj != NULL);
	}

	/*
	 * Is there another XPointer part.
	 */
	SKIP_BLANKS;
	name = xmlXPathParseName(ctxt);
    }
}

/**
 * xmlXPtrEvalChildSeq:
 * @ctxt:  the XPointer Parser context
 * @name:  a possible ID name of the child sequence
 *
 *  ChildSeq ::= '/1' ('/' [0-9]*)*
 *             | Name ('/' [0-9]*)+
 *
 * Parse and evaluate a Child Sequence. This routine also handle the
 * case of a Bare Name used to get a document ID.
 */
static void
xmlXPtrEvalChildSeq(xmlXPathParserContextPtr ctxt, xmlChar *name) {
    /*
     * XPointer don't allow by syntax to address in mutirooted trees
     * this might prove useful in some cases, warn about it.
     */
    if ((name == NULL) && (CUR == '/') && (NXT(1) != '1')) {
        xmlXPtrErr(ctxt, XML_XPTR_CHILDSEQ_START,
		   "warning: ChildSeq not starting by /1\n", NULL);
    }

    if (name != NULL) {
	valuePush(ctxt, xmlXPathNewString(name));
	xmlFree(name);
	xmlXPathIdFunction(ctxt, 1);
	CHECK_ERROR;
    }

    while (CUR == '/') {
	int child = 0;
	NEXT;
        
	while ((CUR >= '0') && (CUR <= '9')) {
	    child = child * 10 + (CUR - '0');
	    NEXT;
	}
	xmlXPtrGetChildNo(ctxt, child);
    }
}


/**
 * xmlXPtrEvalXPointer:
 * @ctxt:  the XPointer Parser context
 *
 *  XPointer ::= Name
 *             | ChildSeq
 *             | FullXPtr
 *
 * Parse and evaluate an XPointer
 */
static void
xmlXPtrEvalXPointer(xmlXPathParserContextPtr ctxt) {
    if (ctxt->valueTab == NULL) {
	/* Allocate the value stack */
	ctxt->valueTab = (xmlXPathObjectPtr *) 
			 xmlMalloc(10 * sizeof(xmlXPathObjectPtr));
	if (ctxt->valueTab == NULL) {
	    xmlXPtrErrMemory("allocating evaluation context");
	    return;
	}
	ctxt->valueNr = 0;
	ctxt->valueMax = 10;
	ctxt->value = NULL;
    }
    SKIP_BLANKS;
    if (CUR == '/') {
	xmlXPathRoot(ctxt);
        xmlXPtrEvalChildSeq(ctxt, NULL);
    } else {
	xmlChar *name;

	name = xmlXPathParseName(ctxt);
	if (name == NULL)
	    XP_ERROR(XPATH_EXPR_ERROR);
	if (CUR == '(') {
	    xmlXPtrEvalFullXPtr(ctxt, name);
	    /* Short evaluation */
	    return;
	} else {
	    /* this handle both Bare Names and Child Sequences */
	    xmlXPtrEvalChildSeq(ctxt, name);
	}
    }
    SKIP_BLANKS;
    if (CUR != 0)
	XP_ERROR(XPATH_EXPR_ERROR);
}


/************************************************************************
 *									*
 *			General routines				*
 *									*
 ***************