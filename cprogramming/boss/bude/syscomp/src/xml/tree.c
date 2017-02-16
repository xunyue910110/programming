/*
 * tree.c : implementation of access function for an XML tree.
 *
 * References:
 *   XHTML 1.0 W3C REC: http://www.w3.org/TR/2002/REC-xhtml1-20020801/
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h> /* for memset() only ! */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/entities.h>
#include <libxml/valid.h>
#include <libxml/xmlerror.h>
#include <libxml/parserInternals.h>
#include <libxml/globals.h>
#ifdef LIBXML_HTML_ENABLED
#include <libxml/HTMLtree.h>
#endif

int __xmlRegisterCallbacks = 0;

xmlNsPtr xmlNewReconciliedNs(xmlDocPtr doc, xmlNodePtr tree, xmlNsPtr ns);

/************************************************************************
 *									*
 * 		Tree memory error handler				*
 *									*
 ************************************************************************/
/**
 * xmlTreeErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlTreeErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_TREE, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * xmlTreeErr:
 * @code:  the error number
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlTreeErr(int code, xmlNodePtr node, const char *extra)
{
    const char *msg = NULL;

    switch(code) {
        case XML_TREE_INVALID_HEX:
	    msg = "invalid hexadecimal character value\n";
	    break;
	case XML_TREE_INVALID_DEC:
	    msg = "invalid decimal character value\n";
	    break;
	case XML_TREE_UNTERMINATED_ENTITY:
	    msg = "unterminated entity reference %15s\n";
	    break;
	default:
	    msg = "unexpected error number\n";
    }
    __xmlSimpleError(XML_FROM_TREE, code, node, msg, extra);
}

/************************************************************************
 *									*
 * 		A few static variables and macros			*
 *									*
 ************************************************************************/
/* #undef xmlStringText */
const xmlChar xmlStringText[] = { 't', 'e', 'x', 't', 0 };
/* #undef xmlStringTextNoenc */
const xmlChar xmlStringTextNoenc[] =
              { 't', 'e', 'x', 't', 'n', 'o', 'e', 'n', 'c', 0 };
/* #undef xmlStringComment */
const xmlChar xmlStringComment[] = { 'c', 'o', 'm', 'm', 'e', 'n', 't', 0 };

static int xmlCompressMode = 0;
static int xmlCheckDTD = 1;

#define UPDATE_LAST_CHILD_AND_PARENT(n) if ((n) != NULL) {		\
    xmlNodePtr ulccur = (n)->children;					\
    if (ulccur == NULL) {						\
        (n)->last = NULL;						\
    } else {								\
        while (ulccur->next != NULL) {					\
	       	ulccur->parent = (n);					\
		ulccur = ulccur->next;					\
	}								\
	ulccur->parent = (n);						\
	(n)->last = ulccur;						\
}}

/* #define DEBUG_BUFFER */
/* #define DEBUG_TREE */

/************************************************************************
 *									*
 *		Functions to move to entities.c once the 		*
 *		API freeze is smoothen and they can be made public.	*
 *									*
 ************************************************************************/
#include <libxml/hash.h>
 
#ifdef LIBXML_TREE_ENABLED
/**
 * xmlGetEntityFromDtd:
 * @dtd:  A pointer to the DTD to search
 * @name:  The entity name
 *
 * Do an entity lookup in the DTD entity hash table and
 * return the corresponding entity, if found.
 * 
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetEntityFromDtd(xmlDtdPtr dtd, const xmlChar *name) {
    xmlEntitiesTablePtr table;
    
    if((dtd != NULL) && (dtd->entities != NULL)) {
	table = (xmlEntitiesTablePtr) dtd->entities;
	return((xmlEntityPtr) xmlHashLookup(table, name));
    	/* return(xmlGetEntityFromTable(table, name)); */
    }
    return(NULL);
}
/**
 * xmlGetParameterEntityFromDtd:
 * @dtd:  A pointer to the DTD to search
 * @name:  The entity name
 * 
 * Do an entity lookup in the DTD pararmeter entity hash table and
 * return the corresponding entity, if found.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetParameterEntityFromDtd(xmlDtdPtr dtd, const xmlChar *name) {
    xmlEntitiesTablePtr table;
    
    if ((dtd != NULL) && (dtd->pentities != NULL)) {
	table = (xmlEntitiesTablePtr) dtd->pentities;
	return((xmlEntityPtr) xmlHashLookup(table, name));
	/* return(xmlGetEntityFromTable(table, name)); */
    }
    return(NULL);
}
#endif /* LIBXML_TREE_ENABLED */

/************************************************************************
 *									*
 *			QName handling helper				*
 *									*
 ************************************************************************/

/**
 * xmlBuildQName:
 * @ncname:  the Name
 * @prefix:  the prefix
 * @memory:  preallocated memory
 * @len:  preallocated memory length
 *
 * Builds the QName @prefix:@ncname in @memory if there is enough space
 * and prefix is not NULL nor empty, otherwise allocate a new string.
 * If prefix is NULL or empty it returns ncname.
 *
 * Returns the new string which must be freed by the caller if different from
 *         @memory and @ncname or NULL in case of error
 */
xmlChar *
xmlBuildQName(const xmlChar *ncname, const xmlChar *prefix,
	      xmlChar *memory, int len) {
    int lenn, lenp;
    xmlChar *ret;

    if (ncname == NULL) return(NULL);
    if (prefix == NULL) return((xmlChar *) ncname);

    lenn = strlen((char *) ncname);
    lenp = strlen((char *) prefix);

    if ((memory == NULL) || (len < lenn + lenp + 2)) {
	ret = (xmlChar *) xmlMallocAtomic(lenn + lenp + 2);
	if (ret == NULL) {
	    xmlTreeErrMemory("building QName");
	    return(NULL);
	}
    } else {
	ret = memory;
    }
    memcpy(&ret[0], prefix, lenp);
    ret[lenp] = ':';
    memcpy(&ret[lenp + 1], ncname, lenn);
    ret[lenn + lenp + 1] = 0;
    return(ret);
}

/**
 * xmlSplitQName2:
 * @name:  the full QName
 * @prefix:  a xmlChar ** 
 *
 * parse an XML qualified name string
 *
 * [NS 5] QName ::= (Prefix ':')? LocalPart
 *
 * [NS 6] Prefix ::= NCName
 *
 * [NS 7] LocalPart ::= NCName
 *
 * Returns NULL if not a QName, otherwise the local part, and prefix
 *   is updated to get the Prefix if any.
 */

xmlChar *
xmlSplitQName2(const xmlChar *name, xmlChar **prefix) {
    int len = 0;
    xmlChar *ret = NULL;

    if (prefix == NULL) return(NULL);
    *prefix = NULL;
    if (name == NULL) return(NULL);

#ifndef XML_XML_NAMESPACE
    /* xml: prefix is not really a namespace */
    if ((name[0] == 'x') && (name[1] == 'm') &&
        (name[2] == 'l') && (name[3] == ':'))
	return(NULL);
#endif

    /* nasty but valid */
    if (name[0] == ':')
	return(NULL);

    /*
     * we are not trying to validate but just to cut, and yes it will
     * work even if this is as set of UTF-8 encoded chars
     */
    while ((name[len] != 0) && (name[len] != ':')) 
	len++;
    
    if (name[len] == 0)
	return(NULL);

    *prefix = xmlStrndup(name, len);
    if (*prefix == NULL) {
	xmlTreeErrMemory("QName split");
	return(NULL);
    }
    ret = xmlStrdup(&name[len + 1]);
    if (ret == NULL) {
	xmlTreeErrMemory("QName split");
	if (*prefix != NULL) {
	    xmlFree(*prefix);
	    *prefix = NULL;
	}
	return(NULL);
    }

    return(ret);
}

/**
 * xmlSplitQName3:
 * @name:  the full QName
 * @len: an int *
 *
 * parse an XML qualified name string,i
 *
 * returns NULL if it is not a Qualified Name, otherwise, update len
 *         with the lenght in byte of the prefix and return a pointer
 */

const xmlChar *
xmlSplitQName3(const xmlChar *name, int *len) {
    int l = 0;

    if (name == NULL) return(NULL);
    if (len == NULL) return(NULL);

    /* nasty but valid */
    if (name[0] == ':')
	return(NULL);

    /*
     * we are not trying to validate but just to cut, and yes it will
     * work even if this is as set of UTF-8 encoded chars
     */
    while ((name[l] != 0) && (name[l] != ':')) 
	l++;
    
    if (name[l] == 0)
	return(NULL);

    *len = l;

    return(&name[l+1]);
}

/************************************************************************
 *									*
 *		Check Name, NCName and QName strings			*
 *									*
 ************************************************************************/
 
#define CUR_SCHAR(s, l) xmlStringCurrentChar(NULL, s, &l)

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_XPATH_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED) || defined(LIBXML_DEBUG_ENABLED)
/**
 * xmlValidateNCName:
 * @value: the value to check
 * @space: allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of NCName
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlValidateNCName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    if (value == NULL)
        return(-1);

    /*
     * First quick algorithm for ASCII range
     */
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) || ((*cur >= 'A') && (*cur <= 'Z')) ||
	(*cur == '_'))
	cur++;
    else
	goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
	   ((*cur >= 'A') && (*cur <= 'Z')) ||
	   ((*cur >= '0') && (*cur <= '9')) ||
	   (*cur == '_') || (*cur == '-') || (*cur == '.'))
	cur++;
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
	return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if ((!IS_LETTER(c)) && (c != '_'))
	return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
	   (c == '-') || (c == '_') || IS_COMBINING(c) ||
	   IS_EXTENDER(c)) {
	cur += l;
	c = CUR_SCHAR(cur, l);
    }
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (c != 0)
	return(1);

    return(0);
}
#endif

#if defined(LIBXML_TREE_ENABLED) || defined(LIBXML_SCHEMAS_ENABLED)
/**
 * xmlValidateQName:
 * @value: the value to check
 * @space: allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of QName
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlValidateQName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    if (value == NULL)
        return(-1);
    /*
     * First quick algorithm for ASCII range
     */
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) || ((*cur >= 'A') && (*cur <= 'Z')) ||
	(*cur == '_'))
	cur++;
    else
	goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
	   ((*cur >= 'A') && (*cur <= 'Z')) ||
	   ((*cur >= '0') && (*cur <= '9')) ||
	   (*cur == '_') || (*cur == '-') || (*cur == '.'))
	cur++;
    if (*cur == ':') {
	cur++;
	if (((*cur >= 'a') && (*cur <= 'z')) ||
	    ((*cur >= 'A') && (*cur <= 'Z')) ||
	    (*cur == '_'))
	    cur++;
	else
	    goto try_complex;
	while (((*cur >= 'a') && (*cur <= 'z')) ||
	       ((*cur >= 'A') && (*cur <= 'Z')) ||
	       ((*cur >= '0') && (*cur <= '9')) ||
	       (*cur == '_') || (*cur == '-') || (*cur == '.'))
	    cur++;
    }
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
	return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if ((!IS_LETTER(c)) && (c != '_'))
	return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
	   (c == '-') || (c == '_') || IS_COMBINING(c) ||
	   IS_EXTENDER(c)) {
	cur += l;
	c = CUR_SCHAR(cur, l);
    }
    if (c == ':') {
	cur += l;
	c = CUR_SCHAR(cur, l);
	if ((!IS_LETTER(c)) && (c != '_'))
	    return(1);
	cur += l;
	c = CUR_SCHAR(cur, l);
	while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
	       (c == '-') || (c == '_') || IS_COMBINING(c) ||
	       IS_EXTENDER(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (c != 0)
	return(1);
    return(0);
}

/**
 * xmlValidateName:
 * @value: the value to check
 * @space: allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of Name
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlValidateName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    if (value == NULL)
        return(-1);
    /*
     * First quick algorithm for ASCII range
     */
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) || ((*cur >= 'A') && (*cur <= 'Z')) ||
	(*cur == '_') || (*cur == ':'))
	cur++;
    else
	goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
	   ((*cur >= 'A') && (*cur <= 'Z')) ||
	   ((*cur >= '0') && (*cur <= '9')) ||
	   (*cur == '_') || (*cur == '-') || (*cur == '.') || (*cur == ':'))
	cur++;
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
	return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if ((!IS_LETTER(c)) && (c != '_') && (c != ':'))
	return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') || (c == ':') ||
	   (c == '-') || (c == '_') || IS_COMBINING(c) || IS_EXTENDER(c)) {
	cur += l;
	c = CUR_SCHAR(cur, l);
    }
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (c != 0)
	return(1);
    return(0);
}

/**
 * xmlValidateNMToken:
 * @value: the value to check
 * @space: allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of NMToken
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlValidateNMToken(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c,l;

    if (value == NULL)
        return(-1);
    /*
     * First quick algorithm for ASCII range
     */
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) ||
        ((*cur >= 'A') && (*cur <= 'Z')) ||
        ((*cur >= '0') && (*cur <= '9')) ||
        (*cur == '_') || (*cur == '-') || (*cur == '.') || (*cur == ':'))
	cur++;
    else
	goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
	   ((*cur >= 'A') && (*cur <= 'Z')) ||
	   ((*cur >= '0') && (*cur <= '9')) ||
	   (*cur == '_') || (*cur == '-') || (*cur == '.') || (*cur == ':'))
	cur++;
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
	return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (!(IS_LETTER(c) || IS_DIGIT(c) || (c == '.') || (c == ':') ||
        (c == '-') || (c == '_') || IS_COMBINING(c) || IS_EXTENDER(c)))
	return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') || (c == ':') ||
	   (c == '-') || (c == '_') || IS_COMBINING(c) || IS_EXTENDER(c)) {
	cur += l;
	c = CUR_SCHAR(cur, l);
    }
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (c != 0)
	return(1);
    return(0);
}
#endif /* LIBXML_TREE_ENABLED */

/************************************************************************
 *									*
 *		Allocation and deallocation of basic structures		*
 *									*
 ************************************************************************/
 
/**
 * xmlSetBufferAllocationScheme:
 * @scheme:  allocation method to use
 * 
 * Set the buffer allocation method.  Types are
 * XML_BUFFER_ALLOC_EXACT - use exact sizes, keeps memory usage down
 * XML_BUFFER_ALLOC_DOUBLEIT - double buffer when extra needed, 
 *                             improves performance
 */
void
xmlSetBufferAllocationScheme(xmlBufferAllocationScheme scheme) {
    xmlBufferAllocScheme = scheme;
}

/**
 * xmlGetBufferAllocationScheme:
 *
 * Types are
 * XML_BUFFER_ALLOC_EXACT - use exact sizes, keeps memory usage down
 * XML_BUFFER_ALLOC_DOUBLEIT - double buffer when extra needed, 
 *                             improves performance
 * 
 * Returns the current allocation scheme
 */
xmlBufferAllocationScheme
xmlGetBufferAllocationScheme(void) {
    return(xmlBufferAllocScheme);
}

/**
 * xmlNewNs:
 * @node:  the element carrying the namespace
 * @href:  the URI associated
 * @prefix:  the prefix for the namespace
 *
 * Creation of a new Namespace. This function will refuse to create
 * a namespace with a similar prefix than an existing one present on this
 * node.
 * We use href==NULL in the case of an element creation where the namespace
 * was not defined.
 * Returns a new namespace pointer or NULL
 */
xmlNsPtr
xmlNewNs(xmlNodePtr node, const xmlChar *href, const xmlChar *prefix) {
    xmlNsPtr cur;

    if ((node != NULL) && (node->type != XML_ELEMENT_NODE))
	return(NULL);

    if ((prefix != NULL) && (xmlStrEqual(prefix, BAD_CAST "xml")))
	return(NULL);

    /*
     * Allocate a new Namespace and fill the fields.
     */
    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
    if (cur == NULL) {
	xmlTreeErrMemory("building namespace");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNs));
    cur->type = XML_LOCAL_NAMESPACE;

    if (href != NULL)
	cur->href = xmlStrdup(href); 
    if (prefix != NULL)
	cur->prefix = xmlStrdup(prefix); 

    /*
     * Add it at the end to preserve parsing order ...
     * and checks for existing use of the prefix
     */
    if (node != NULL) {
	if (node->nsDef == NULL) {
	    node->nsDef = cur;
	} else {
	    xmlNsPtr prev = node->nsDef;

	    if (((prev->prefix == NULL) && (cur->prefix == NULL)) ||
		(xmlStrEqual(prev->prefix, cur->prefix))) {
		xmlFreeNs(cur);
		return(NULL);
	    }    
	    while (prev->next != NULL) {
	        prev = prev->next;
		if (((prev->prefix == NULL) && (cur->prefix == NULL)) ||
		    (xmlStrEqual(prev->prefix, cur->prefix))) {
		    xmlFreeNs(cur);
		    return(NULL);
		}    
	    }
	    prev->next = cur;
	}
    }
    return(cur);
}

/**
 * xmlSetNs:
 * @node:  a node in the document
 * @ns:  a namespace pointer
 *
 * Associate a namespace to a node, a posteriori.
 */
void
xmlSetNs(xmlNodePtr node, xmlNsPtr ns) {
    if (node == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlSetNs: node == NULL\n");
#endif
	return;
    }
    node->ns = ns;
}

/**
 * xmlFreeNs:
 * @cur:  the namespace pointer
 *
 * Free up the structures associated to a namespace
 */
void
xmlFreeNs(xmlNsPtr cur) {
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeNs : ns == NULL\n");
#endif
	return;
    }
    if (cur->href != NULL) xmlFree((char *) cur->href);
    if (cur->prefix != NULL) xmlFree((char *) cur->prefix);
    xmlFree(cur);
}

/**
 * xmlFreeNsList:
 * @cur:  the first namespace pointer
 *
 * Free up all the structures associated to the chained namespaces.
 */
void
xmlFreeNsList(xmlNsPtr cur) {
    xmlNsPtr next;
    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeNsList : ns == NULL\n");
#endif
	return;
    }
    while (cur != NULL) {
        next = cur->next;
        xmlFreeNs(cur);
	cur = next;
    }
}

/**
 * xmlNewDtd:
 * @doc:  the document pointer
 * @name:  the DTD name
 * @ExternalID:  the external ID
 * @SystemID:  the system ID
 *
 * Creation of a new DTD for the external subset. To create an
 * internal subset, use xmlCreateIntSubset().
 *
 * Returns a pointer to the new DTD structure
 */
xmlDtdPtr
xmlNewDtd(xmlDocPtr doc, const xmlChar *name,
                    const xmlChar *ExternalID, const xmlChar *SystemID) {
    xmlDtdPtr cur;

    if ((doc != NULL) && (doc->extSubset != NULL)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewDtd(%s): document %s already have a DTD %s\n",
	    /* !!! */ (char *) name, doc->name,
	    /* !!! */ (char *)doc->extSubset->name);
#endif
	return(NULL);
    }

    /*
     * Allocate a new DTD and fill the fields.
     */
    cur = (xmlDtdPtr) xmlMalloc(sizeof(xmlDtd));
    if (cur == NULL) {
	xmlTreeErrMemory("building DTD");
	return(NULL);
    }
    memset(cur, 0 , sizeof(xmlDtd));
    cur->type = XML_DTD_NODE;

    if (name != NULL)
	cur->name = xmlStrdup(name); 
    if (ExternalID != NULL)
	cur->ExternalID = xmlStrdup(ExternalID); 
    if (SystemID != NULL)
	cur->SystemID = xmlStrdup(SystemID); 
    if (doc != NULL)
	doc->extSubset = cur;
    cur->doc = doc;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
	xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    return(cur);
}

/**
 * xmlGetIntSubset:
 * @doc:  the document pointer
 *
 * Get the internal subset of a document
 * Returns a pointer to the DTD structure or NULL if not found
 */

xmlDtdPtr
xmlGetIntSubset(xmlDocPtr doc) {
    xmlNodePtr cur;

    if (doc == NULL)
	return(NULL);
    cur = doc->children;
    while (cur != NULL) {
	if (cur->type == XML_DTD_NODE)
	    return((xmlDtdPtr) cur);
	cur = cur->next;
    }
    return((xmlDtdPtr) doc->intSubset);
}

/**
 * xmlCreateIntSubset:
 * @doc:  the document pointer
 * @name:  the DTD name
 * @ExternalID:  the external (PUBLIC) ID
 * @SystemID:  the system ID
 *
 * Create the internal subset of a document
 * Returns a pointer to the new DTD structure
 */
xmlDtdPtr
xmlCreateIntSubset(xmlDocPtr doc, const xmlChar *name,
                   const xmlChar *ExternalID, const xmlChar *SystemID) {
    xmlDtdPtr cur;

    if ((doc != NULL) && (xmlGetIntSubset(doc) != NULL)) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,

     "xmlCreateIntSubset(): document %s already have an internal subset\n",
	    doc->name);
#endif
	return(NULL);
    }

    /*
     * Allocate a new DTD and fill the fields.
     */
    cur = (xmlDtdPtr) xmlMalloc(sizeof(xmlDtd));
    if (cur == NULL) {
	xmlTreeErrMemory("building internal subset");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlDtd));
    cur->type = XML_DTD_NODE;

    if (name != NULL) {
	cur->name = xmlStrdup(name);
	if (cur->name == NULL) {
	    xmlTreeErrMemory("building internal subset");
	    xmlFree(cur);
	    return(NULL);
	}
    }
    if (ExternalID != NULL) {
	cur->ExternalID = xmlStrdup(ExternalID); 
	if (cur->ExternalID  == NULL) {
	    xmlTreeErrMemory("building internal subset");
	    if (cur->name != NULL)
	        xmlFree((char *)cur->name);
	    xmlFree(cur);
	    return(NULL);
	}
    }
    if (SystemID != NULL) {
	cur->SystemID = xmlStrdup(SystemID); 
	if (cur->SystemID == NULL) {
	    xmlTreeErrMemory("building internal subset");
	    if (cur->name != NULL)
	        xmlFree((char *)cur->name);
	    if (cur->ExternalID != NULL)
	        xmlFree((char *)cur->ExternalID);
	    xmlFree(cur);
	    return(NULL);
	}
    }
    if (doc != NULL) {
	doc->intSubset = cur;
	cur->parent = doc;
	cur->doc = doc;
	if (doc->children == NULL) {
	    doc->children = (xmlNodePtr) cur;
	    doc->last = (xmlNodePtr) cur;
	} else {
	    if (doc->type == XML_HTML_DOCUMENT_NODE) {
		xmlNodePtr prev;

		prev = doc->children;
		prev->prev = (xmlNodePtr) cur;
		cur->next = prev;
		doc->children = (xmlNodePtr) cur;
	    } else {
		xmlNodePtr next;

		next = doc->children;
		while ((next != NULL) && (next->type != XML_ELEMENT_NODE))
		    next = next->next;
		if (next == NULL) {
		    cur->prev = doc->last;
		    cur->prev->next = (xmlNodePtr) cur;
		    cur->next = NULL;
		    doc->last = (xmlNodePtr) cur;
		} else {
		    cur->next = next;
		    cur->prev = next->prev;
		    if (cur->prev == NULL)
			doc->children = (xmlNodePtr) cur;
		    else
			cur->prev->next = (xmlNodePtr) cur;
		    next->prev = (xmlNodePtr) cur;
		}
	    }
	}
    }

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
	xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    return(cur);
}

/**
 * DICT_FREE:
 * @str:  a string
 *
 * Free a string if it is not owned by the "dict" dictionnary in the
 * current scope
 */
#define DICT_FREE(str)						\
	if ((str) && ((!dict) || 				\
	    (xmlDictOwns(dict, (const xmlChar *)(str)) == 0)))	\
	    xmlFree((char *)(str));

/**
 * xmlFreeDtd:
 * @cur:  the DTD structure to free up
 *
 * Free a DTD structure.
 */
void
xmlFreeDtd(xmlDtdPtr cur) {
    xmlDictPtr dict = NULL;

    if (cur == NULL) {
	return;
    }
    if (cur->doc != NULL) dict = cur->doc->dict;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
	xmlDeregisterNodeDefaultValue((xmlNodePtr)cur);

    if (cur->children != NULL) {
	xmlNodePtr next, c = cur->children;

	/*
	 * Cleanup all nodes which are not part of the specific lists
	 * of notations, elements, attributes and entities.
	 */
        while (c != NULL) {
	    next = c->next;
	    if ((c->type != XML_NOTATION_NODE) &&
	        (c->type != XML_ELEMENT_DECL) &&
		(c->type != XML_ATTRIBUTE_DECL) &&
		(c->type != XML_ENTITY_DECL)) {
		xmlUnlinkNode(c);
		xmlFreeNode(c);
	    }
	    c = next;
	}
    }
    DICT_FREE(cur->name)
    DICT_FREE(cur->SystemID)
    DICT_FREE(cur->ExternalID)
    /* TODO !!! */
    if (cur->notations != NULL)
        xmlFreeNotationTable((xmlNotationTablePtr) cur->notations);
    
    if (cur->elements != NULL)
        xmlFreeElementTable((xmlElementTablePtr) cur->elements);
    if (cur->attributes != NULL)
        xmlFreeAttributeTable((xmlAttributeTablePtr) cur->attributes);
    if (cur->entities != NULL)
        xmlFreeEntitiesTable((xmlEntitiesTablePtr) cur->entities);
    if (cur->pentities != NULL)
        xmlFreeEntitiesTable((xmlEntitiesTablePtr) cur->pentities);

    xmlFree(cur);
}

/**
 * xmlNewDoc:
 * @version:  xmlChar string giving the version of XML "1.0"
 *
 * Creates a new XML document
 *
 * Returns a new document
 */
xmlDocPtr
xmlNewDoc(const xmlChar *version) {
    xmlDocPtr cur;

    if (version == NULL)
	version = (const xmlChar *) "1.0";

    /*
     * Allocate a new document and fill the fields.
     */
    cur = (xmlDocPtr) xmlMalloc(sizeof(xmlDoc));
    if (cur == NULL) {
	xmlTreeErrMemory("building doc");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlDoc));
    cur->type = XML_DOCUMENT_NODE;

    cur->version = xmlStrdup(version); 
    if (cur->version == NULL) {
	xmlTreeErrMemory("building doc");
	xmlFree(cur);
    	return(NULL);
    }
    cur->standalone = -1;
    cur->compression = -1; /* not initialized */
    cur->doc = cur;
    /*
     * The in memory encoding is always UTF8
     * This field will never change and would
     * be obsolete if not for binary compatibility.
     */
    cur->charset = XML_CHAR_ENCODING_UTF8;

    if ((__xmlRegisterCallbacks) && (xmlRegisterNodeDefaultValue))
	xmlRegisterNodeDefaultValue((xmlNodePtr)cur);
    return(cur);
}

/**
 * xmlFreeDoc:
 * @cur:  pointer to the document
 *
 * Free up all the structures used by a document, tree included.
 */
void
xmlFreeDoc(xmlDocPtr cur) {
    xmlDtdPtr extSubset, intSubset;
    xmlDictPtr dict = NULL;

    if (cur == NULL) {
#ifdef DEBUG_TREE
        xmlGenericError(xmlGenericErrorContext,
		"xmlFreeDoc : document == NULL\n");
#endif
	return;
    }
#ifdef LIBXML_DEBUG_RUNTIME
    xmlDebugCheckDocument(stderr, cur);
#endif

    if (cur != NULL) dict = cur->dict;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
	xmlDeregisterNodeDefaultValue((xmlNodePtr)cur);

    /*
     * Do this before freeing the children list to avoid ID lookups
     */
    if (cur->ids != NULL) xmlFreeIDTable((xmlIDTablePtr) cur->ids);
    cur->ids = NULL;
    if (cur->refs != NULL) xmlFreeRefTable((xmlRefTablePtr) cur->refs);
    cur->refs = NULL;
    extSubset = cur->extSubset;
    intSubset = cur->intSubset;
    if (intSubset == extSubset)
	extSubset = NULL;
    if (extSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) cur->extSubset);
	cur->extSubset = NULL;
	xmlFreeDtd(extSubset);
    }
    if (intSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) cur->intSubset);
	cur->intSubset = NULL;
	xmlFreeDtd(intSubset);
    }

    if (cur->children != NULL) xmlFreeNodeList(cur->children);
    if (cur->oldNs != NULL) xmlFreeNsList(cur->oldNs);

    DICT_FREE(cur->version)
    DICT_FREE(cur->name)
    DICT_FREE(cur->encoding)
    DICT_FREE(cur->URL)
    xmlFree(cur);
    if (dict) xmlDictFree(dict);
}

/**
 * xmlStringLenGetNodeList:
 * @doc:  the document
 * @value:  the value of the text
 * @len:  the length of the string value
 *
 * Parse the value string and build the node list associated. Should
 * produce a flat tree with only TEXTs and ENTITY_REFs.
 * Returns a pointer to the first child
 */
xmlNodePtr
xmlStringLenGetNodeList(xmlDocPtr doc, const xmlChar *value, int len) {
    xmlNodePtr ret = NULL, last = NULL;
    xmlNodePtr node;
    xmlChar *val;
    const xmlChar *cur = value, *end = cur + len;
    const xmlChar *q;
    xmlEntityPtr ent;

    if (value == NULL) return(NULL);

    q = cur;
    while ((cur < end) && (*cur != 0)) {
	if (cur[0] == '&') {
	    int charval = 0;
	    xmlChar tmp;

	    /*
	     * Save the current text.
	     */
            if (cur != q) {
		if ((last != NULL) && (last->type == XML_TEXT_NODE)) {
		    xmlNodeAddContentLen(last, q, cur - q);
		} else {
		    node = xmlNewDocTextLen(doc, q, cur - q);
		    if (node == NULL) return(ret);
		    if (last == NULL)
			last = ret = node;
		    else {
			last->next = node;
			node->prev = last;
			last = node;
		    }
		}
	    }
	    q = cur;
	    if ((cur + 2 < end) && (cur[1] == '#') && (cur[2] == 'x')) {
		cur += 3;
		if (cur < end)
		    tmp = *cur;
		else
		    tmp = 0;
		while (tmp != ';') { /* Non input consuming loop */
		    if ((tmp >= '0') && (tmp <= '9')) 
			charval = charval * 16 + (tmp - '0');
		    else if ((tmp >= 'a') && (tmp <= 'f'))
			charval = charval * 16 + (tmp - 'a') + 10;
		    else if ((tmp >= 'A') && (tmp <= 'F'))
			charval = charval * 16 + (tmp - 'A') + 10;
		    else {
			xmlTreeErr(XML_TREE_INVALID_HEX, (xmlNodePtr) doc,
			           NULL);
			charval = 0;
			break;
		    }
		    cur++;
		    if (cur < end)
			tmp = *cur;
		    else
			tmp = 0;
		}
		if (tmp == ';')
		    cur++;
		q = cur;
	    } else if ((cur + 1 < end) && (cur[1] == '#')) {
		cur += 2;
		if (cur < end)
		    tmp = *cur;
		else
		    tmp = 0;
		while (tmp != ';') { /* Non input consuming loops */
		    if ((tmp >= '0') && (tmp <= '9')) 
			charval = charval * 10 + (tmp - '0');
		    else {
			xmlTreeErr(XML_TREE_INVALID_DEC, (xmlNodePtr) doc,
			           NULL);
			charval = 0;
			break;
		    }
		    cur++;
		    if (cur < end)
			tmp = *cur;
		    else
			tmp = 0;
		}
		if (tmp == ';')
		    cur++;
		q = cur;
	    } else {
		/*
		 * Read the entity string
		 */
		cur++;
		q = cur;
		while ((cur < end) && (*cur != 0) && (*cur != ';')) cur++;
		if ((cur >= end) || (*cur == 0)) {
		    xmlTreeErr(XML_TREE_UNTERMINATED_ENTITY, (xmlNodePtr) doc,
		               (const char *) q);
		    return(ret);
		}
		if (cur != q) {
		    /*
		     * Predefined entities don't generate nodes
		     */
		    val = xmlStrndup(q, cur - q);
		    ent = xmlGetDocEntity(doc, val);
		    if ((ent != NULL) &&
			(ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
			if (last == NULL) {
			    node = xmlNewDocText(doc, ent->content);
			    last = ret = node;
			} else if (last->type != XML_TEXT_NODE) {
			    node = xmlNewDocText(doc, ent->content);
			    last = xmlAddNextSibling(last, node);
			} else
			    xmlNodeAddContent(last, ent->content);
			    
		    } else {
			/*
			 * Create a new REFERENCE_REF node
			 */
			node = xmlNewReference(doc, val);
			if (node == NULL) {
			    if (val != NULL) xmlFree(val);
			    return(ret);
			}
			else if ((ent != NULL) && (ent->children == NULL)) {
			    xmlNodePtr temp;

			    ent->children = xmlStringGetNodeList(doc,
				    (const xmlChar*)node->content);
			    ent->owner = 1;
			    temp = ent->children;
			    while (temp) {
				temp->parent = (xmlNodePtr)ent;
				ent->last = temp;
				temp = temp->next;
			    }
			}
			if (last == NULL) {
			    last = ret = node;
			} else {
			    last = xmlAddNextSibling(las