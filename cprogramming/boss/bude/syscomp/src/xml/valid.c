/*
 * valid.c : part of the code use to do the DTD handling and the validity
 *           checking
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/hash.h>
#include <libxml/uri.h>
#include <libxml/valid.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlerror.h>
#include <libxml/list.h>
#include <libxml/globals.h>

static xmlElementPtr xmlGetDtdElementDesc2(xmlDtdPtr dtd, const xmlChar *name,
	                           int create);
/* #define DEBUG_VALID_ALGO */
/* #define DEBUG_REGEXP_ALGO */

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

/************************************************************************
 *									*
 *			Error handling routines				*
 *									*
 ************************************************************************/

/**
 * xmlVErrMemory:
 * @ctxt:  an XML validation parser context
 * @extra:  extra informations
 *
 * Handle an out of memory error
 */
static void
xmlVErrMemory(xmlValidCtxtPtr ctxt, const char *extra)
{
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
	/* Use the special values to detect if it is part of a parsing
	   context */
	if ((ctxt->finishDtd == XML_CTXT_FINISH_DTD_0) ||
	    (ctxt->finishDtd == XML_CTXT_FINISH_DTD_1)) {
	    pctxt = (xmlParserCtxtPtr)ctxt->userData;
	}
    }
    if (extra)
        __xmlRaiseError(NULL, channel, data,
                        pctxt, NULL, XML_FROM_VALID, XML_ERR_NO_MEMORY,
                        XML_ERR_FATAL, NULL, 0, extra, NULL, NULL, 0, 0,
                        "Memory allocation failed : %s\n", extra);
    else
        __xmlRaiseError(NULL, channel, data,
                        pctxt, NULL, XML_FROM_VALID, XML_ERR_NO_MEMORY,
                        XML_ERR_FATAL, NULL, 0, NULL, NULL, NULL, 0, 0,
                        "Memory allocation failed\n");
}

/**
 * xmlErrValid:
 * @ctxt:  an XML validation parser context
 * @error:  the error number
 * @extra:  extra informations
 *
 * Handle a validation error
 */
static void
xmlErrValid(xmlValidCtxtPtr ctxt, xmlParserErrors error,
            const char *msg, const char *extra)
{
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
	/* Use the special values to detect if it is part of a parsing
	   context */
	if ((ctxt->finishDtd == XML_CTXT_FINISH_DTD_0) ||
	    (ctxt->finishDtd == XML_CTXT_FINISH_DTD_1)) {
	    pctxt = (xmlParserCtxtPtr)ctxt->userData;
	}
    }
    if (extra)
        __xmlRaiseError(NULL, channel, data,
                        pctxt, NULL, XML_FROM_VALID, error,
                        XML_ERR_ERROR, NULL, 0, extra, NULL, NULL, 0, 0,
                        msg, extra);
    else
        __xmlRaiseError(NULL, channel, data,
                        pctxt, NULL, XML_FROM_VALID, error,
                        XML_ERR_ERROR, NULL, 0, NULL, NULL, NULL, 0, 0,
                        msg);
}

#if defined(LIBXML_VALID_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlErrValidNode:
 * @ctxt:  an XML validation parser context
 * @node:  the node raising the error
 * @error:  the error number
 * @str1:  extra informations
 * @str2:  extra informations
 * @str3:  extra informations
 *
 * Handle a validation error, provide contextual informations
 */
static void
xmlErrValidNode(xmlValidCtxtPtr ctxt,
                xmlNodePtr node, xmlParserErrors error,
                const char *msg, const xmlChar * str1,
                const xmlChar * str2, const xmlChar * str3)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
	/* Use the special values to detect if it is part of a parsing
	   context */
	if ((ctxt->finishDtd == XML_CTXT_FINISH_DTD_0) ||
	    (ctxt->finishDtd == XML_CTXT_FINISH_DTD_1)) {
	    pctxt = (xmlParserCtxtPtr)ctxt->userData;
	}
    }
    __xmlRaiseError(schannel, channel, data, pctxt, node, XML_FROM_VALID, error,
                    XML_ERR_ERROR, NULL, 0,
                    (const char *) str1,
                    (const char *) str1,
                    (const char *) str3, 0, 0, msg, str1, str2, str3);
}
#endif /* LIBXML_VALID_ENABLED or LIBXML_SCHEMAS_ENABLED */

#ifdef LIBXML_VALID_ENABLED
/**
 * xmlErrValidNodeNr:
 * @ctxt:  an XML validation parser context
 * @node:  the node raising the error
 * @error:  the error number
 * @str1:  extra informations
 * @int2:  extra informations
 * @str3:  extra informations
 *
 * Handle a validation error, provide contextual informations
 */
static void
xmlErrValidNodeNr(xmlValidCtxtPtr ctxt,
                xmlNodePtr node, xmlParserErrors error,
                const char *msg, const xmlChar * str1,
                int int2, const xmlChar * str3)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
	/* Use the special values to detect if it is part of a parsing
	   context */
	if ((ctxt->finishDtd == XML_CTXT_FINISH_DTD_0) ||
	    (ctxt->finishDtd == XML_CTXT_FINISH_DTD_1)) {
	    pctxt = (xmlParserCtxtPtr)ctxt->userData;
	}
    }
    __xmlRaiseError(schannel, channel, data, pctxt, node, XML_FROM_VALID, error,
                    XML_ERR_ERROR, NULL, 0,
                    (const char *) str1,
                    (const char *) str3,
                    NULL, int2, 0, msg, str1, int2, str3);
}

/**
 * xmlErrValidWarning:
 * @ctxt:  an XML validation parser context
 * @node:  the node raising the error
 * @error:  the error number
 * @str1:  extra information
 * @str2:  extra information
 * @str3:  extra information
 *
 * Handle a validation error, provide contextual information
 */
static void
xmlErrValidWarning(xmlValidCtxtPtr ctxt,
                xmlNodePtr node, xmlParserErrors error,
                const char *msg, const xmlChar * str1,
                const xmlChar * str2, const xmlChar * str3)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    xmlParserCtxtPtr pctxt = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
	/* Use the special values to detect if it is part of a parsing
	   context */
	if ((ctxt->finishDtd == XML_CTXT_FINISH_DTD_0) ||
	    (ctxt->finishDtd == XML_CTXT_FINISH_DTD_1)) {
	    pctxt = (xmlParserCtxtPtr)ctxt->userData;
	}
    }
    __xmlRaiseError(schannel, channel, data, pctxt, node, XML_FROM_VALID, error,
                    XML_ERR_WARNING, NULL, 0,
                    (const char *) str1,
                    (const char *) str1,
                    (const char *) str3, 0, 0, msg, str1, str2, str3);
}



#ifdef LIBXML_REGEXP_ENABLED
/*
 * If regexp are enabled we can do continuous validation without the
 * need of a tree to validate the content model. this is done in each
 * callbacks.
 * Each xmlValidState represent the validation state associated to the
 * set of nodes currently open from the document root to the current element.
 */


typedef struct _xmlValidState {
    xmlElementPtr	 elemDecl;	/* pointer to the content model */
    xmlNodePtr           node;		/* pointer to the current node */
    xmlRegExecCtxtPtr    exec;		/* regexp runtime */
} _xmlValidState;


static int
vstateVPush(xmlValidCtxtPtr ctxt, xmlElementPtr elemDecl, xmlNodePtr node) {
    if ((ctxt->vstateMax == 0) || (ctxt->vstateTab == NULL)) {
	ctxt->vstateMax = 10;
	ctxt->vstateTab = (xmlValidState *) xmlMalloc(ctxt->vstateMax *
		              sizeof(ctxt->vstateTab[0]));
        if (ctxt->vstateTab == NULL) {
	    xmlVErrMemory(ctxt, "malloc failed");
	    return(-1);
	}
    }

    if (ctxt->vstateNr >= ctxt->vstateMax) {
        xmlValidState *tmp;

	tmp = (xmlValidState *) xmlRealloc(ctxt->vstateTab,
	             2 * ctxt->vstateMax * sizeof(ctxt->vstateTab[0]));
        if (tmp == NULL) {
	    xmlVErrMemory(ctxt, "realloc failed");
	    return(-1);
	}
	ctxt->vstateMax *= 2;
	ctxt->vstateTab = tmp;
    }
    ctxt->vstate = &ctxt->vstateTab[ctxt->vstateNr];
    ctxt->vstateTab[ctxt->vstateNr].elemDecl = elemDecl;
    ctxt->vstateTab[ctxt->vstateNr].node = node;
    if ((elemDecl != NULL) && (elemDecl->etype == XML_ELEMENT_TYPE_ELEMENT)) {
	if (elemDecl->contModel == NULL)
	    xmlValidBuildContentModel(ctxt, elemDecl);
	if (elemDecl->contModel != NULL) {
	    ctxt->vstateTab[ctxt->vstateNr].exec = 
		xmlRegNewExecCtxt(elemDecl->contModel, NULL, NULL);
	} else {
	    ctxt->vstateTab[ctxt->vstateNr].exec = NULL;
	    xmlErrValidNode(ctxt, (xmlNodePtr) elemDecl,
	                    XML_ERR_INTERNAL_ERROR,
			    "Failed to build content model regexp for %s\n",
			    node->name, NULL, NULL);
	}
    }
    return(ctxt->vstateNr++);
}

static int
vstateVPop(xmlValidCtxtPtr ctxt) {
    xmlElementPtr elemDecl;

    if (ctxt->vstateNr < 1) return(-1);
    ctxt->vstateNr--;
    elemDecl = ctxt->vstateTab[ctxt->vstateNr].elemDecl;
    ctxt->vstateTab[ctxt->vstateNr].elemDecl = NULL;
    ctxt->vstateTab[ctxt->vstateNr].node = NULL;
    if ((elemDecl != NULL) && (elemDecl->etype == XML_ELEMENT_TYPE_ELEMENT)) {
	xmlRegFreeExecCtxt(ctxt->vstateTab[ctxt->vstateNr].exec);
    }
    ctxt->vstateTab[ctxt->vstateNr].exec = NULL;
    if (ctxt->vstateNr >= 1)
	ctxt->vstate = &ctxt->vstateTab[ctxt->vstateNr - 1];
    else
	ctxt->vstate = NULL;
    return(ctxt->vstateNr);
}

#else /* not LIBXML_REGEXP_ENABLED */
/*
 * If regexp are not enabled, it uses a home made algorithm less
 * complex and easier to
 * debug/maintain than a generic NFA -> DFA state based algo. The
 * only restriction is on the deepness of the tree limited by the
 * size of the occurs bitfield
 *
 * this is the content of a saved state for rollbacks
 */

#define ROLLBACK_OR	0
#define ROLLBACK_PARENT	1

typedef struct _xmlValidState {
    xmlElementContentPtr cont;	/* pointer to the content model subtree */
    xmlNodePtr           node;	/* pointer to the current node in the list */
    long                 occurs;/* bitfield for multiple occurrences */
    unsigned char        depth; /* current depth in the overall tree */
    unsigned char        state; /* ROLLBACK_XXX */
} _xmlValidState;

#define MAX_RECURSE 25000
#define MAX_DEPTH ((sizeof(_xmlValidState.occurs)) * 8)
#define CONT ctxt->vstate->cont
#define NODE ctxt->vstate->node
#define DEPTH ctxt->vstate->depth
#define OCCURS ctxt->vstate->occurs
#define STATE ctxt->vstate->state

#define OCCURRENCE (ctxt->vstate->occurs & (1 << DEPTH))
#define PARENT_OCCURRENCE (ctxt->vstate->occurs & ((1 << DEPTH) - 1))

#define SET_OCCURRENCE ctxt->vstate->occurs |= (1 << DEPTH)
#define RESET_OCCURRENCE ctxt->vstate->occurs &= ((1 << DEPTH) - 1)

static int
vstateVPush(xmlValidCtxtPtr ctxt, xmlElementContentPtr cont,
	    xmlNodePtr node, unsigned char depth, long occurs,
	    unsigned char state) {
    int i = ctxt->vstateNr - 1;

    if (ctxt->vstateNr > MAX_RECURSE) {
	return(-1);
    }
    if (ctxt->vstateTab == NULL) {
	ctxt->vstateMax = 8;
	ctxt->vstateTab = (xmlValidState *) xmlMalloc(
		     ctxt->vstateMax * sizeof(ctxt->vstateTab[0]));
	if (ctxt->vstateTab == NULL) {
	    xmlVErrMemory(ctxt, "malloc failed");
	    return(-1);
	}
    }
    if (ctxt->vstateNr >= ctxt->vstateMax) {
        xmlValidState *tmp;

        tmp = (xmlValidState *) xmlRealloc(ctxt->vstateTab,
	             2 * ctxt->vstateMax * sizeof(ctxt->vstateTab[0]));
        if (tmp == NULL) {
	    xmlVErrMemory(ctxt, "malloc failed");
	    return(-1);
	}
	ctxt->vstateMax *= 2;
	ctxt->vstateTab = tmp;
	ctxt->vstate = &ctxt->vstateTab[0];
    }
    /*
     * Don't push on the stack a state already here
     */
    if ((i >= 0) && (ctxt->vstateTab[i].cont == cont) &&
	(ctxt->vstateTab[i].node == node) &&
	(ctxt->vstateTab[i].depth == depth) &&
	(ctxt->vstateTab[i].occurs == occurs) &&
	(ctxt->vstateTab[i].state == state))
	return(ctxt->vstateNr);
    ctxt->vstateTab[ctxt->vstateNr].cont = cont;
    ctxt->vstateTab[ctxt->vstateNr].node = node;
    ctxt->vstateTab[ctxt->vstateNr].depth = depth;
    ctxt->vstateTab[ctxt->vstateNr].occurs = occurs;
    ctxt->vstateTab[ctxt->vstateNr].state = state;
    return(ctxt->vstateNr++);
}

static int
vstateVPop(xmlValidCtxtPtr ctxt) {
    if (ctxt->vstateNr <= 1) return(-1);
    ctxt->vstateNr--;
    ctxt->vstate = &ctxt->vstateTab[0];
    ctxt->vstate->cont =  ctxt->vstateTab[ctxt->vstateNr].cont;
    ctxt->vstate->node = ctxt->vstateTab[ctxt->vstateNr].node;
    ctxt->vstate->depth = ctxt->vstateTab[ctxt->vstateNr].depth;
    ctxt->vstate->occurs = ctxt->vstateTab[ctxt->vstateNr].occurs;
    ctxt->vstate->state = ctxt->vstateTab[ctxt->vstateNr].state;
    return(ctxt->vstateNr);
}

#endif /* LIBXML_REGEXP_ENABLED */

static int
nodeVPush(xmlValidCtxtPtr ctxt, xmlNodePtr value)
{
    if (ctxt->nodeMax <= 0) {
        ctxt->nodeMax = 4;
        ctxt->nodeTab =
            (xmlNodePtr *) xmlMalloc(ctxt->nodeMax *
                                     sizeof(ctxt->nodeTab[0]));
        if (ctxt->nodeTab == NULL) {
	    xmlVErrMemory(ctxt, "malloc failed");
            ctxt->nodeMax = 0;
            return (0);
        }
    }
    if (ctxt->nodeNr >= ctxt->nodeMax) {
        xmlNodePtr *tmp;
        tmp = (xmlNodePtr *) xmlRealloc(ctxt->nodeTab,
			      ctxt->nodeMax * 2 * sizeof(ctxt->nodeTab[0]));
        if (tmp == NULL) {
	    xmlVErrMemory(ctxt, "realloc failed");
            return (0);
        }
        ctxt->nodeMax *= 2;
	ctxt->nodeTab = tmp;
    }
    ctxt->nodeTab[ctxt->nodeNr] = value;
    ctxt->node = value;
    return (ctxt->nodeNr++);
}
static xmlNodePtr
nodeVPop(xmlValidCtxtPtr ctxt)
{
    xmlNodePtr ret;

    if (ctxt->nodeNr <= 0)
        return (0);
    ctxt->nodeNr--;
    if (ctxt->nodeNr > 0)
        ctxt->node = ctxt->nodeTab[ctxt->nodeNr - 1];
    else
        ctxt->node = NULL;
    ret = ctxt->nodeTab[ctxt->nodeNr];
    ctxt->nodeTab[ctxt->nodeNr] = 0;
    return (ret);
}

#ifdef DEBUG_VALID_ALGO
static void
xmlValidPrintNode(xmlNodePtr cur) {
    if (cur == NULL) {
	xmlGenericError(xmlGenericErrorContext, "null");
	return;
    }
    switch (cur->type) {
	case XML_ELEMENT_NODE:
	    xmlGenericError(xmlGenericErrorContext, "%s ", cur->name);
	    break;
	case XML_TEXT_NODE:
	    xmlGenericError(xmlGenericErrorContext, "text ");
	    break;
	case XML_CDATA_SECTION_NODE:
	    xmlGenericError(xmlGenericErrorContext, "cdata ");
	    break;
	case XML_ENTITY_REF_NODE:
	    xmlGenericError(xmlGenericErrorContext, "&%s; ", cur->name);
	    break;
	case XML_PI_NODE:
	    xmlGenericError(xmlGenericErrorContext, "pi(%s) ", cur->name);
	    break;
	case XML_COMMENT_NODE:
	    xmlGenericError(xmlGenericErrorContext, "comment ");
	    break;
	case XML_ATTRIBUTE_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?attr? ");
	    break;
	case XML_ENTITY_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?ent? ");
	    break;
	case XML_DOCUMENT_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?doc? ");
	    break;
	case XML_DOCUMENT_TYPE_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?doctype? ");
	    break;
	case XML_DOCUMENT_FRAG_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?frag? ");
	    break;
	case XML_NOTATION_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?nota? ");
	    break;
	case XML_HTML_DOCUMENT_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?html? ");
	    break;
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?docb? ");
	    break;
#endif
	case XML_DTD_NODE:
	    xmlGenericError(xmlGenericErrorContext, "?dtd? ");
	    break;
	case XML_ELEMENT_DECL:
	    xmlGenericError(xmlGenericErrorContext, "?edecl? ");
	    break;
	case XML_ATTRIBUTE_DECL:
	    xmlGenericError(xmlGenericErrorContext, "?adecl? ");
	    break;
	case XML_ENTITY_DECL:
	    xmlGenericError(xmlGenericErrorContext, "?entdecl? ");
	    break;
	case XML_NAMESPACE_DECL:
	    xmlGenericError(xmlGenericErrorContext, "?nsdecl? ");
	    break;
	case XML_XINCLUDE_START:
	    xmlGenericError(xmlGenericErrorContext, "incstart ");
	    break;
	case XML_XINCLUDE_END:
	    xmlGenericError(xmlGenericErrorContext, "incend ");
	    break;
    }
}

static void
xmlValidPrintNodeList(xmlNodePtr cur) {
    if (cur == NULL)
	xmlGenericError(xmlGenericErrorContext, "null ");
    while (cur != NULL) {
	xmlValidPrintNode(cur);
	cur = cur->next;
    }
}

static void
xmlValidDebug(xmlNodePtr cur, xmlElementContentPtr cont) {
    char expr[5000];

    expr[0] = 0;
    xmlGenericError(xmlGenericErrorContext, "valid: ");
    xmlValidPrintNodeList(cur);
    xmlGenericError(xmlGenericErrorContext, "against ");
    xmlSnprintfElementContent(expr, 5000, cont, 1);
    xmlGenericError(xmlGenericErrorContext, "%s\n", expr);
}

static void
xmlValidDebugState(xmlValidStatePtr state) {
    xmlGenericError(xmlGenericErrorContext, "(");
    if (state->cont == NULL)
	xmlGenericError(xmlGenericErrorContext, "null,");
    else
	switch (state->cont->type) {
            case XML_ELEMENT_CONTENT_PCDATA:
		xmlGenericError(xmlGenericErrorContext, "pcdata,");
		break;
            case XML_ELEMENT_CONTENT_ELEMENT:
		xmlGenericError(xmlGenericErrorContext, "%s,",
			        state->cont->name);
		break;
            case XML_ELEMENT_CONTENT_SEQ:
		xmlGenericError(xmlGenericErrorContext, "seq,");
		break;
            case XML_ELEMENT_CONTENT_OR:
		xmlGenericError(xmlGenericErrorContext, "or,");
		break;
	}
    xmlValidPrintNode(state->node);
    xmlGenericError(xmlGenericErrorContext, ",%d,%X,%d)",
	    state->depth, state->occurs, state->state);
}

static void
xmlValidStateDebug(xmlValidCtxtPtr ctxt) {
    int i, j;

    xmlGenericError(xmlGenericErrorContext, "state: ");
    xmlValidDebugState(ctxt->vstate);
    xmlGenericError(xmlGenericErrorContext, " stack: %d ",
	    ctxt->vstateNr - 1);
    for (i = 0, j = ctxt->vstateNr - 1;(i < 3) && (j > 0);i++,j--)
	xmlValidDebugState(&ctxt->vstateTab[j]);
    xmlGenericError(xmlGenericErrorContext, "\n");
}

/*****
#define DEBUG_VALID_STATE(n,c) xmlValidDebug(n,c);
 *****/

#define DEBUG_VALID_STATE(n,c) xmlValidStateDebug(ctxt);
#define DEBUG_VALID_MSG(m)					\
    xmlGenericError(xmlGenericErrorContext, "%s\n", m);
        
#else
#define DEBUG_VALID_STATE(n,c)
#define DEBUG_VALID_MSG(m)
#endif

/* TODO: use hash table for accesses to elem and attribute definitions */


#define CHECK_DTD						\
   if (doc == NULL) return(0);					\
   else if ((doc->intSubset == NULL) &&				\
	    (doc->extSubset == NULL)) return(0)

xmlAttributePtr xmlScanAttributeDecl(xmlDtdPtr dtd, const xmlChar *elem);

#ifdef LIBXML_REGEXP_ENABLED

/************************************************************************
 *									*
 *		Content model validation based on the regexps		*
 *									*
 ************************************************************************/

/**
 * xmlValidBuildAContentModel:
 * @content:  the content model
 * @ctxt:  the schema parser context
 * @name:  the element name whose content is being built
 *
 * Generate the automata sequence needed for that type
 *
 * Returns 1 if successful or 0 in case of error.
 */
static int
xmlValidBuildAContentModel(xmlElementContentPtr content,
		           xmlValidCtxtPtr ctxt,
		           const xmlChar *name) {
    if (content == NULL) {
	xmlErrValidNode(ctxt, NULL, XML_ERR_INTERNAL_ERROR,
			"Found NULL content in content model of %s\n",
			name, NULL, NULL);
	return(0);
    }
    switch (content->type) {
	case XML_ELEMENT_CONTENT_PCDATA:
	    xmlErrValidNode(ctxt, NULL, XML_ERR_INTERNAL_ERROR,
			    "Found PCDATA in content model of %s\n",
		            name, NULL, NULL);
	    return(0);
	    break;
	case XML_ELEMENT_CONTENT_ELEMENT: {
	    xmlAutomataStatePtr oldstate = ctxt->state;
	    xmlChar fn[50];
	    xmlChar *fullname;
	    
	    fullname = xmlBuildQName(content->name, content->prefix, fn, 50);
	    if (fullname == NULL) {
	        xmlVErrMemory(ctxt, "Building content model");
		return(0);
	    }

	    switch (content->ocur) {
		case XML_ELEMENT_CONTENT_ONCE:
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, fullname, NULL);
		    break;
		case XML_ELEMENT_CONTENT_OPT:
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, fullname, NULL);
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    break;
		case XML_ELEMENT_CONTENT_PLUS:
		    ctxt->state = xmlAutomataNewTransition(ctxt->am,
			    ctxt->state, NULL, fullname, NULL);
		    xmlAutomataNewTransition(ctxt->am, ctxt->state,
			                     ctxt->state, fullname, NULL);
		    break;
		case XML_ELEMENT_CONTENT_MULT:
		    ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
		    			    ctxt->state, NULL);
		    xmlAutomataNewTransition(ctxt->am,
		    	    ctxt->state, ctxt->state, fullname, NULL);
		    break;
	    }
	    if ((fullname != fn) && (fullname != content->name))
		xmlFree(fullname);
	    break;
	}
	case XML_ELEMENT_CONTENT_SEQ: {
	    xmlAutomataStatePtr oldstate, oldend;
	    xmlElementContentOccur ocur;

	    /*
	     * Simply iterate over the content
	     */
	    oldstate = ctxt->state;
	    ocur = content->ocur;
	    if (ocur != XML_ELEMENT_CONTENT_ONCE) {
		ctxt->state = xmlAutomataNewEpsilon(ctxt->am, oldstate, NULL);
		oldstate = ctxt->state;
	    }
	    do {
		xmlValidBuildAContentModel(content->c1, ctxt, name);
		content = content->c2;
	    } while ((content->type == XML_ELEMENT_CONTENT_SEQ) &&
		     (content->ocur == XML_ELEMENT_CONTENT_ONCE));
	    xmlValidBuildAContentModel(content, ctxt, name);
	    oldend = ctxt->state;
	    ctxt->state = xmlAutomataNewEpsilon(ctxt->am, oldend, NULL);
	    switch (ocur) {
		case XML_ELEMENT_CONTENT_ONCE:
		    break;
		case XML_ELEMENT_CONTENT_OPT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    break;
		case XML_ELEMENT_CONTENT_MULT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
		case XML_ELEMENT_CONTENT_PLUS:
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
	    }
	    break;
	}
	case XML_ELEMENT_CONTENT_OR: {
	    xmlAutomataStatePtr oldstate, oldend;
	    xmlElementContentOccur ocur;

	    ocur = content->ocur;
	    if ((ocur == XML_ELEMENT_CONTENT_PLUS) || 
		(ocur == XML_ELEMENT_CONTENT_MULT)) {
		ctxt->state = xmlAutomataNewEpsilon(ctxt->am,
			ctxt->state, NULL);
	    }
	    oldstate = ctxt->state;
	    oldend = xmlAutomataNewState(ctxt->am);

	    /*
	     * iterate over the subtypes and remerge the end with an
	     * epsilon transition
	     */
	    do {
		ctxt->state = oldstate;
		xmlValidBuildAContentModel(content->c1, ctxt, name);
		xmlAutomataNewEpsilon(ctxt->am, ctxt->state, oldend);
		content = content->c2;
	    } while ((content->type == XML_ELEMENT_CONTENT_OR) &&
		     (content->ocur == XML_ELEMENT_CONTENT_ONCE));
	    ctxt->state = oldstate;
	    xmlValidBuildAContentModel(content, ctxt, name);
	    xmlAutomataNewEpsilon(ctxt->am, ctxt->state, oldend);
	    ctxt->state = xmlAutomataNewEpsilon(ctxt->am, oldend, NULL);
	    switch (ocur) {
		case XML_ELEMENT_CONTENT_ONCE:
		    break;
		case XML_ELEMENT_CONTENT_OPT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    break;
		case XML_ELEMENT_CONTENT_MULT:
		    xmlAutomataNewEpsilon(ctxt->am, oldstate, ctxt->state);
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
		case XML_ELEMENT_CONTENT_PLUS:
		    xmlAutomataNewEpsilon(ctxt->am, oldend, oldstate);
		    break;
	    }
	    break;
	}
	default:
	    xmlErrValid(ctxt, XML_ERR_INTERNAL_ERROR,
	                "ContentModel broken for element %s\n",
			(const char *) name);
	    return(0);
    }
    return(1);
}
/**
 * xmlValidBuildContentModel:
 * @ctxt:  a validation context
 * @elem:  an element declaration node
 *
 * (Re)Build the automata associated to the content model of this
 * element
 *
 * Returns 1 in case of success, 0 in case of error
 */
int
xmlValidBuildContentModel(xmlValidCtxtPtr ctxt, xmlElementPtr elem) {

    if ((ctxt == NULL) || (elem == NULL))
	return(0);
    if (elem->type != XML_ELEMENT_DECL)
	return(0);
    if (elem->etype != XML_ELEMENT_TYPE_ELEMENT)
	return(1);
    /* TODO: should we rebuild in this case ? */
    if (elem->contModel != NULL) {
	if (!xmlRegexpIsDeterminist(elem->contModel)) {
	    ctxt->valid = 0;
	    return(0);
	}
	return(1);
    }

    ctxt->am = xmlNewAutomata();
    if (ctxt->am == NULL) {
	xmlErrValidNode(ctxt, (xmlNodePtr) elem,
	                XML_ERR_INTERNAL_ERROR,
	                "Cannot create automata for element %s\n",
		        elem->name, NULL, NULL);
	return(0);
    }
    ctxt->state = xmlAutomataGetInitState(ctxt->am);
    xmlValidBuildAContentModel(elem->content, ctxt, elem->name);
    xmlAutomataSetFinalState(ctxt->am, ctxt->state);
    elem->contModel = xmlAutomataCompile(ctxt->am);
    if (xmlRegexpIsDeterminist(elem->contModel) != 1) {
	char expr[5000];
	expr[0] = 0;
	xmlSnprintfElementContent(expr, 5000, elem->content, 1);
	xmlErrValidNode(ctxt, (xmlNodePtr) elem,
	                XML_DTD_CONTENT_NOT_DETERMINIST,
	       "Content model of %s is not determinist: %s\n",
	       elem->name, BAD_CAST expr, NULL);
#ifdef DEBUG_REGEXP_ALGO
        xmlRegexpPrint(stderr, elem->contModel);
#endif
        ctxt->valid = 0;
	ctxt->state = NULL;
	xmlFreeAutomata(ctxt->am);
	ctxt->am = NULL;
	return(0);
    }
    ctxt->state = NULL;
    xmlFreeAutomata(ctxt->am);
    ctxt->am = NULL;
    return(1);
}

#endif /* LIBXML_REGEXP_ENABLED */

/****************************************************************
 *								*
 *	Util functions for data allocation/deallocation		*
 *								*
 ****************************************************************/

/**
 * xmlNewValidCtxt:
 *
 * Allocate a validation context structure.
 *
 * Returns NULL if not, otherwise the new validation context structure
 */
xmlValidCtxtPtr xmlNewValidCtxt(void) {
    xmlValidCtxtPtr ret;

    if ((ret = (xmlValidCtxtPtr)xmlMalloc(sizeof (xmlValidCtxt))) == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return (NULL);
    }

    (void) memset(ret, 0, sizeof (xmlValidCtxt));

    return (ret);
}

/**
 * xmlFreeValidCtxt:
 * @cur:  the validation context to free
 *
 * Free a validation context structure.
 */
void
xmlFreeValidCtxt(xmlValidCtxtPtr cur) {
    if (cur->vstateTab != NULL)
        xmlFree(cur->vstateTab);
    if (cur->nodeTab != NULL)
        xmlFree(cur->nodeTab);
    xmlFree(cur);
}

#endif /* LIBXML_VALID_ENABLED */

/**
 * xmlNewElementContent:
 * @name:  the subelement name or NULL
 * @type:  the type of element content decl
 *
 * Allocate an element content structure.
 *
 * Returns NULL if not, otherwise the new element content structure
 */
xmlElementContentPtr
xmlNewElementContent(const xmlChar *name, xmlElementContentType type) {
    xmlElementContentPtr ret;

    switch(type) {
	case XML_ELEMENT_CONTENT_ELEMENT:
	    if (name == NULL) {
	        xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
			"xmlNewElementContent : name == NULL !\n",
			NULL);
	    }
	    break;
        case XML_ELEMENT_CONTENT_PCDATA:
	case XML_ELEMENT_CONTENT_SEQ:
	case XML_ELEMENT_CONTENT_OR:
	    if (name != NULL) {
	        xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR,
			"xmlNewElementContent : name != NULL !\n",
			NULL);
	    }
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR, 
		    "Internal: ELEMENT content corrupted invalid type\n",
		    NULL);
	    return(NULL);
    }
    ret = (xmlElementContentPtr) xmlMalloc(sizeof(xmlElementContent));
    if (ret == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlElementContent));
    ret->type = type;
    ret->ocur = XML_ELEMENT_CONTENT_ONCE;
    if (name != NULL) {
	xmlChar *prefix = NULL;
	ret->name = xmlSplitQName2(name, &prefix);
	if (ret->name == NULL)
	    ret->name = xmlStrdup(name);
	ret->prefix = prefix;
    } else {
        ret->name = NULL;
	ret->prefix = NULL;
    }
    ret->c1 = ret->c2 = ret->parent = NULL;
    return(ret);
}

/**
 * xmlCopyElementContent:
 * @cur:  An element content pointer.
 *
 * Build a copy of an element content description.
 * 
 * Returns the new xmlElementContentPtr or NULL in case of error.
 */
xmlElementContentPtr
xmlCopyElementContent(xmlElementContentPtr cur) {
    xmlElementContentPtr ret;

    if (cur == NULL) return(NULL);
    ret = xmlNewElementContent((xmlChar *) cur->name, cur->type);
    if (ret == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    if (cur->prefix != NULL)
	ret->prefix = xmlStrdup(cur->prefix);
    ret->ocur = cur->ocur;
    if (cur->c1 != NULL) ret->c1 = xmlCopyElementContent(cur->c1);
    if (ret->c1 != NULL)
	ret->c1->parent = ret;
    if (cur->c2 != NULL) ret->c2 = xmlCopyElementContent(cur->c2);
    if (ret->c2 != NULL)
	ret->c2->parent = ret;
    return(ret);
}

/**
 * xmlFreeElementContent:
 * @cur:  the element content tree to free
 *
 * Free an element content structure. This is a recursive call !
 */
void
xmlFreeElementContent(xmlElementContentPtr cur) {
    if (cur == NULL) return;
    switch (cur->type) {
	case XML_ELEMENT_CONTENT_PCDATA:
	case XML_ELEMENT_CONTENT_ELEMENT:
	case XML_ELEMENT_CONTENT_SEQ:
	case XML_ELEMENT_CONTENT_OR:
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR, 
		    "Internal: ELEMENT content corrupted invalid type\n",
		    NULL);
	    return;
    }
    if (cur->c1 != NULL) xmlFreeElementContent(cur->c1);
    if (cur->c2 != NULL) xmlFreeElementContent(cur->c2);
    if (cur->name != NULL) xmlFree((xmlChar *) cur->name);
    if (cur->prefix != NULL) xmlFree((xmlChar *) cur->prefix);
    xmlFree(cur);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlDumpElementContent:
 * @buf:  An XML buffer
 * @content:  An element table
 * @glob: 1 if one must print the englobing parenthesis, 0 otherwise
 *
 * This will dump the content of the element table as an XML DTD definition
 */
static void
xmlDumpElementContent(xmlBufferPtr buf, xmlElementContentPtr content, int glob) {
    if (content == NULL) return;

    if (glob) xmlBufferWriteChar(buf, "(");
    switch (content->type) {
        case XML_ELEMENT_CONTENT_PCDATA:
            xmlBufferWriteChar(buf, "#PCDATA");
	    break;
	case XML_ELEMENT_CONTENT_ELEMENT:
	    if (content->prefix != NULL) {
		xmlBufferWriteCHAR(buf, content->prefix);
		xmlBufferWriteChar(buf, ":");
	    }
	    xmlBufferWriteCHAR(buf, content->name);
	    break;
	case XML_ELEMENT_CONTENT_SEQ:
	    if ((content->c1->type == XML_ELEMENT_CONTENT_OR) ||
	        (content->c1->type == XML_ELEMENT_CONTENT_SEQ))
		xmlDumpElementContent(buf, content->c1, 1);
	    else
		xmlDumpElementContent(buf, content->c1, 0);
            xmlBufferWriteChar(buf, " , ");
	    if ((content->c2->type == XML_ELEMENT_CONTENT_OR) ||
	        ((content->c2->type == XML_ELEMENT_CONTENT_SEQ) &&
		 (content->c2->ocur != XML_ELEMENT_CONTENT_ONCE)))
		xmlDumpElementContent(buf, content->c2, 1);
	    else
		xmlDumpElementContent(buf, content->c2, 0);
	    break;
	case XML_ELEMENT_CONTENT_OR:
	    if ((content->c1->type == XML_ELEMENT_CONTENT_OR) ||
	        (content->c1->type == XML_ELEMENT_CONTENT_SEQ))
		xmlDumpElementContent(buf, content->c1, 1);
	    else
		xmlDumpElementContent(buf, content->c1, 0);
            xmlBufferWriteChar(buf, " | ");
	    if ((content->c2->type == XML_ELEMENT_CONTENT_SEQ) ||
	        ((content->c2->type == XML_ELEMENT_CONTENT_OR) &&
		 (content->c2->ocur != XML_ELEMENT_CONTENT_ONCE)))
		xmlDumpElementContent(buf, content->c2, 1);
	    else
		xmlDumpElementContent(buf, content->c2, 0);
	    break;
	default:
	    xmlErrValid(NULL, XML_ERR_INTERNAL_ERROR, 
		    "Internal: ELEMENT content corrupted invalid type\n",
		    NULL);
    }
    if (glob)
        xmlBufferWriteChar(buf, ")");
    switch (content->ocur) {
        case XML_ELEMENT_CONTENT_ONCE:
	    break;
        case XML_ELEMENT_CONTENT_OPT:
	    xmlBufferWriteChar(buf, "?");
	    break;
        case XML_ELEMENT_CONTENT_MULT:
	    xmlBufferWriteChar(buf, "*");
	    break;
        case XML_ELEMENT_CONTENT_PLUS:
	    xmlBufferWriteChar(buf, "+");
	    break;
    }
}

/**
 * xmlSprintf