/*
 * debugXML.c : This is a set of routines used for debugging the tree
 *              produced by the XML parser.
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */

#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_DEBUG_ENABLED

#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/HTMLparser.h>
#include <libxml/xmlerror.h>
#include <libxml/globals.h>
#include <libxml/xpathInternals.h>
#include <libxml/uri.h>
#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/relaxng.h>
#endif

typedef struct _xmlDebugCtxt xmlDebugCtxt;
typedef xmlDebugCtxt *xmlDebugCtxtPtr;
struct _xmlDebugCtxt {
    FILE *output;               /* the output file */
    char shift[101];            /* used for indenting */
    int depth;                  /* current depth */
    xmlDocPtr doc;              /* current document */
    xmlNodePtr node;		/* current node */
    xmlDictPtr dict;		/* the doc dictionnary */
    int check;                  /* do just checkings */
    int errors;                 /* number of errors found */
    int nodict;			/* if the document has no dictionnary */
};

static void xmlCtxtDumpNodeList(xmlDebugCtxtPtr ctxt, xmlNodePtr node);

static void
xmlCtxtDumpInitCtxt(xmlDebugCtxtPtr ctxt)
{
    int i;

    ctxt->depth = 0;
    ctxt->check = 0;
    ctxt->errors = 0;
    ctxt->output = stdout;
    ctxt->doc = NULL;
    ctxt->node = NULL;
    ctxt->dict = NULL;
    ctxt->nodict = 0;
    for (i = 0; i < 100; i++)
        ctxt->shift[i] = ' ';
    ctxt->shift[100] = 0;
}

static void
xmlCtxtDumpCleanCtxt(xmlDebugCtxtPtr ctxt ATTRIBUTE_UNUSED)
{
 /* remove the ATTRIBUTE_UNUSED when this is added */
}

/**
 * xmlNsCheckScope:
 * @node: the node
 * @ns: the namespace node
 *
 * Check that a given namespace is in scope on a node.
 *
 * Returns 1 if in scope, -1 in case of argument error, 
 *         -2 if the namespace is not in scope, and -3 if not on
 *         an ancestor node.
 */
static int
xmlNsCheckScope(xmlNodePtr node, xmlNsPtr ns)
{
    xmlNsPtr cur;

    if ((node == NULL) || (ns == NULL))
        return(-1);

    if ((node->type != XML_ELEMENT_NODE) &&
	(node->type != XML_ATTRIBUTE_NODE) &&
	(node->type != XML_DOCUMENT_NODE) &&
	(node->type != XML_TEXT_NODE) &&
	(node->type != XML_HTML_DOCUMENT_NODE) &&
	(node->type != XML_XINCLUDE_START))
	return(-2);

    while ((node != NULL) &&
           ((node->type == XML_ELEMENT_NODE) ||
            (node->type == XML_ATTRIBUTE_NODE) ||
            (node->type == XML_TEXT_NODE) ||
	    (node->type == XML_XINCLUDE_START))) {
	if ((node->type == XML_ELEMENT_NODE) ||
	    (node->type == XML_XINCLUDE_START)) {
	    cur = node->nsDef;
	    while (cur != NULL) {
	        if (cur == ns)
		    return(1);
		if (xmlStrEqual(cur->prefix, ns->prefix))
		    return(-2);
		cur = cur->next;
	    }
	}
	node = node->parent;
    }
    /* the xml namespace may be declared on the document node */
    if ((node != NULL) &&
        ((node->type == XML_DOCUMENT_NODE) ||
	 (node->type == XML_HTML_DOCUMENT_NODE))) {
	 xmlNsPtr oldNs = ((xmlDocPtr) node)->oldNs;
	 if (oldNs == ns)
	     return(1);
    }
    return(-3);
}

static void
xmlCtxtDumpSpaces(xmlDebugCtxtPtr ctxt)
{
    if (ctxt->check)
        return;
    if ((ctxt->output != NULL) && (ctxt->depth > 0)) {
        if (ctxt->depth < 50)
            fprintf(ctxt->output, &ctxt->shift[100 - 2 * ctxt->depth]);
        else
            fprintf(ctxt->output, ctxt->shift);
    }
}

/**
 * xmlDebugErr:
 * @ctxt:  a debug context
 * @error:  the error code
 *
 * Handle a debug error.
 */
static void
xmlDebugErr(xmlDebugCtxtPtr ctxt, int error, const char *msg)
{
    ctxt->errors++;
    __xmlRaiseError(NULL, NULL, NULL,
		    NULL, ctxt->node, XML_FROM_CHECK,
		    error, XML_ERR_ERROR, NULL, 0,
		    NULL, NULL, NULL, 0, 0,
		    msg);
}
static void
xmlDebugErr2(xmlDebugCtxtPtr ctxt, int error, const char *msg, int extra)
{
    ctxt->errors++;
    __xmlRaiseError(NULL, NULL, NULL,
		    NULL, ctxt->node, XML_FROM_CHECK,
		    error, XML_ERR_ERROR, NULL, 0,
		    NULL, NULL, NULL, 0, 0,
		    msg, extra);
}
static void
xmlDebugErr3(xmlDebugCtxtPtr ctxt, int error, const char *msg, const char *extra)
{
    ctxt->errors++;
    __xmlRaiseError(NULL, NULL, NULL,
		    NULL, ctxt->node, XML_FROM_CHECK,
		    error, XML_ERR_ERROR, NULL, 0,
		    NULL, NULL, NULL, 0, 0,
		    msg, extra);
}

/**
 * xmlCtxtNsCheckScope:
 * @ctxt: the debugging context
 * @node: the node
 * @ns: the namespace node
 *
 * Report if a given namespace is is not in scope.
 */
static void
xmlCtxtNsCheckScope(xmlDebugCtxtPtr ctxt, xmlNodePtr node, xmlNsPtr ns)
{
    int ret;

    ret = xmlNsCheckScope(node, ns);
    if (ret == -2) {
        if (ns->prefix == NULL)
	    xmlDebugErr(ctxt, XML_CHECK_NS_SCOPE,
			"Reference to default namespace not in scope\n");
	else
	    xmlDebugErr3(ctxt, XML_CHECK_NS_SCOPE,
			 "Reference to namespace '%s' not in scope\n",
			 (char *) ns->prefix);
    }
    if (ret == -3) {
        if (ns->prefix == NULL)
	    xmlDebugErr(ctxt, XML_CHECK_NS_ANCESTOR,
			"Reference to default namespace not on ancestor\n");
	else
	    xmlDebugErr3(ctxt, XML_CHECK_NS_ANCESTOR,
			 "Reference to namespace '%s' not on ancestor\n",
			 (char *) ns->prefix);
    }
}

/**
 * xmlCtxtCheckString:
 * @ctxt: the debug context
 * @str: the string
 *
 * Do debugging on the string, currently it just checks the UTF-8 content
 */
static void
xmlCtxtCheckString(xmlDebugCtxtPtr ctxt, const xmlChar * str)
{
    if (str == NULL) return;
    if (ctxt->check) {
        if (!xmlCheckUTF8(str)) {
	    xmlDebugErr3(ctxt, XML_CHECK_NOT_UTF8,
			 "String is not UTF-8 %s", (const char *) str);
	}
    }
}

/**
 * xmlCtxtCheckName:
 * @ctxt: the debug context
 * @name: the name
 *
 * Do debugging on the name, for example the dictionnary status and
 * conformance to the Name production.
 */
static void
xmlCtxtCheckName(xmlDebugCtxtPtr ctxt, const xmlChar * name)
{
    if (ctxt->check) {
	if (name == NULL) {
	    xmlDebugErr(ctxt, XML_CHECK_NO_NAME, "Name is NULL");
	    return;
	}
        if (xmlValidateName(name, 0)) {
	    xmlDebugErr3(ctxt, XML_CHECK_NOT_NCNAME,
			 "Name is not an NCName '%s'", (const char *) name);
	}
	if ((ctxt->dict != NULL) &&
	    (!xmlDictOwns(ctxt->dict, name))) {
	    xmlDebugErr3(ctxt, XML_CHECK_OUTSIDE_DICT,
			 "Name is not from the document dictionnary '%s'",
			 (const char *) name);
	}
    }
}

static void
xmlCtxtGenericNodeCheck(xmlDebugCtxtPtr ctxt, xmlNodePtr node) {
    xmlDocPtr doc;
    xmlDictPtr dict;

    doc = node->doc;

    if (node->parent == NULL)
        xmlDebugErr(ctxt, XML_CHECK_NO_PARENT,
	            "Node has no parent\n");
    if (node->doc == NULL) {
        xmlDebugErr(ctxt, XML_CHECK_NO_DOC,
	            "Node has no doc\n");
        dict = NULL;
    } else {
	dict = doc->dict;
	if ((dict == NULL) && (ctxt->nodict == 0)) {
#if 0
            /* desactivated right now as it raises too many errors */
	    if (doc->type == XML_DOCUMENT_NODE)
		xmlDebugErr(ctxt, XML_CHECK_NO_DICT,
			    "Document has no dictionnary\n");
#endif
	    ctxt->nodict = 1;
	}
	if (ctxt->doc == NULL)
	    ctxt->doc = doc;

	if (ctxt->dict == NULL) {
	    ctxt->dict = dict;
	}
    }
    if ((node->parent != NULL) && (node->doc != node->parent->doc) &&
        (!xmlStrEqual(node->name, BAD_CAST "pseudoroot")))
        xmlDebugErr(ctxt, XML_CHECK_WRONG_DOC,
	            "Node doc differs from parent's one\n");
    if (node->prev == NULL) {
        if (node->type == XML_ATTRIBUTE_NODE) {
	    if ((node->parent != NULL) &&
	        (node != (xmlNodePtr) node->parent->properties))
		xmlDebugErr(ctxt, XML_CHECK_NO_PREV,
                    "Attr has no prev and not first of attr list\n");
	        
        } else if ((node->parent != NULL) && (node->parent->children != node))
	    xmlDebugErr(ctxt, XML_CHECK_NO_PREV,
                    "Node has no prev and not first of parent list\n");
    } else {
        if (node->prev->next != node)
	    xmlDebugErr(ctxt, XML_CHECK_WRONG_PREV,
                        "Node prev->next : back link wrong\n");
    }
    if (node->next == NULL) {
	if ((node->parent != NULL) && (node->type != XML_ATTRIBUTE_NODE) &&
	    (node->parent->last != node))
	    xmlDebugErr(ctxt, XML_CHECK_NO_NEXT,
                    "Node has no next and not last of parent list\n");
    } else {
        if (node->next->prev != node)
	    xmlDebugErr(ctxt, XML_CHECK_WRONG_NEXT,
                    "Node next->prev : forward link wrong\n");
        if (node->next->parent != node->parent)
	    xmlDebugErr(ctxt, XML_CHECK_WRONG_PARENT,
                    "Node next->prev : forward link wrong\n");
    }
    if (node->type == XML_ELEMENT_NODE) {
        xmlNsPtr ns;

	ns = node->nsDef;
	while (ns != NULL) {
	    xmlCtxtNsCheckScope(ctxt, node, ns);
	    ns = ns->next;
	}
	if (node->ns != NULL)
	    xmlCtxtNsCheckScope(ctxt, node, node->ns);
    } else if (node->type == XML_ATTRIBUTE_NODE) {
	if (node->ns != NULL)
	    xmlCtxtNsCheckScope(ctxt, node, node->ns);
    }

    if ((node->type != XML_ELEMENT_NODE) &&
	(node->type != XML_ATTRIBUTE_NODE) &&
	(node->type != XML_ATTRIBUTE_DECL) &&
	(node->type != XML_DTD_NODE) &&
	(node->type != XML_HTML_DOCUMENT_NODE) &&
	(node->type != XML_DOCUMENT_NODE)) {
	if (node->content != NULL)
	    xmlCtxtCheckString(ctxt, (const xmlChar *) node->content);
    }
    switch (node->type) {
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
	    xmlCtxtCheckName(ctxt, node->name);
	    break;
        case XML_TEXT_NODE:
	    if ((node->name == xmlStringText) ||
	        (node->name == xmlStringTextNoenc))
		break;
	    /* some case of entity substitution can lead to this */
	    if ((ctxt->dict != NULL) &&
	        (node->name == xmlDictLookup(ctxt->dict, BAD_CAST "nbktext",
		                             7)))
		break;

	    xmlDebugErr3(ctxt, XML_CHECK_WRONG_NAME,
			 "Text node has wrong name '%s'",
			 (const char *) node->name);
	    break;
        case XML_COMMENT_NODE:
	    if (node->name == xmlStringComment)
		break;
	    xmlDebugErr3(ctxt, XML_CHECK_WRONG_NAME,
			 "Comment node has wrong name '%s'",
			 (const char *) node->name);
	    break;
        case XML_PI_NODE:
	    xmlCtxtCheckName(ctxt, node->name);
	    break;
        case XML_CDATA_SECTION_NODE:
	    if (node->name == NULL)
		break;
	    xmlDebugErr3(ctxt, XML_CHECK_NAME_NOT_NULL,
			 "CData section has non NULL name '%s'",
			 (const char *) node->name);
	    break;
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_NAMESPACE_DECL:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
	    break;
    }
}

static void
xmlCtxtDumpString(xmlDebugCtxtPtr ctxt, const xmlChar * str)
{
    int i;

    if (ctxt->check) {
        return;
    }
    /* TODO: check UTF8 content of the string */
    if (str == NULL) {
        fprintf(ctxt->output, "(NULL)");
        return;
    }
    for (i = 0; i < 40; i++)
        if (str[i] == 0)
            return;
        else if (IS_BLANK_CH(str[i]))
            fputc(' ', ctxt->output);
        else if (str[i] >= 0x80)
            fprintf(ctxt->output, "#%X", str[i]);
        else
            fputc(str[i], ctxt->output);
    fprintf(ctxt->output, "...");
}

static void
xmlCtxtDumpDtdNode(xmlDebugCtxtPtr ctxt, xmlDtdPtr dtd)
{
    xmlCtxtDumpSpaces(ctxt);

    if (dtd == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "DTD node is NULL\n");
        return;
    }

    if (dtd->type != XML_DTD_NODE) {
	xmlDebugErr(ctxt, XML_CHECK_NOT_DTD,
	            "Node is not a DTD");
        return;
    }
    if (!ctxt->check) {
        if (dtd->name != NULL)
            fprintf(ctxt->output, "DTD(%s)", (char *) dtd->name);
        else
            fprintf(ctxt->output, "DTD");
        if (dtd->ExternalID != NULL)
            fprintf(ctxt->output, ", PUBLIC %s", (char *) dtd->ExternalID);
        if (dtd->SystemID != NULL)
            fprintf(ctxt->output, ", SYSTEM %s", (char *) dtd->SystemID);
        fprintf(ctxt->output, "\n");
    }
    /*
     * Do a bit of checking
     */
    xmlCtxtGenericNodeCheck(ctxt, (xmlNodePtr) dtd);
}

static void
xmlCtxtDumpAttrDecl(xmlDebugCtxtPtr ctxt, xmlAttributePtr attr)
{
    xmlCtxtDumpSpaces(ctxt);

    if (attr == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "Attribute declaration is NULL\n");
        return;
    }
    if (attr->type != XML_ATTRIBUTE_DECL) {
	xmlDebugErr(ctxt, XML_CHECK_NOT_ATTR_DECL,
	            "Node is not an attribute declaration");
        return;
    }
    if (attr->name != NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "ATTRDECL(%s)", (char *) attr->name);
    } else
	xmlDebugErr(ctxt, XML_CHECK_NO_NAME,
	            "Node attribute declaration has no name");
    if (attr->elem != NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, " for %s", (char *) attr->elem);
    } else
	xmlDebugErr(ctxt, XML_CHECK_NO_ELEM,
	            "Node attribute declaration has no element name");
    if (!ctxt->check) {
        switch (attr->atype) {
            case XML_ATTRIBUTE_CDATA:
                fprintf(ctxt->output, " CDATA");
                break;
            case XML_ATTRIBUTE_ID:
                fprintf(ctxt->output, " ID");
                break;
            case XML_ATTRIBUTE_IDREF:
                fprintf(ctxt->output, " IDREF");
                break;
            case XML_ATTRIBUTE_IDREFS:
                fprintf(ctxt->output, " IDREFS");
                break;
            case XML_ATTRIBUTE_ENTITY:
                fprintf(ctxt->output, " ENTITY");
                break;
            case XML_ATTRIBUTE_ENTITIES:
                fprintf(ctxt->output, " ENTITIES");
                break;
            case XML_ATTRIBUTE_NMTOKEN:
                fprintf(ctxt->output, " NMTOKEN");
                break;
            case XML_ATTRIBUTE_NMTOKENS:
                fprintf(ctxt->output, " NMTOKENS");
                break;
            case XML_ATTRIBUTE_ENUMERATION:
                fprintf(ctxt->output, " ENUMERATION");
                break;
            case XML_ATTRIBUTE_NOTATION:
                fprintf(ctxt->output, " NOTATION ");
                break;
        }
        if (attr->tree != NULL) {
            int indx;
            xmlEnumerationPtr cur = attr->tree;

            for (indx = 0; indx < 5; indx++) {
                if (indx != 0)
                    fprintf(ctxt->output, "|%s", (char *) cur->name);
                else
                    fprintf(ctxt->output, " (%s", (char *) cur->name);
                cur = cur->next;
                if (cur == NULL)
                    break;
            }
            if (cur == NULL)
                fprintf(ctxt->output, ")");
            else
                fprintf(ctxt->output, "...)");
        }
        switch (attr->def) {
            case XML_ATTRIBUTE_NONE:
                break;
            case XML_ATTRIBUTE_REQUIRED:
                fprintf(ctxt->output, " REQUIRED");
                break;
            case XML_ATTRIBUTE_IMPLIED:
                fprintf(ctxt->output, " IMPLIED");
                break;
            case XML_ATTRIBUTE_FIXED:
                fprintf(ctxt->output, " FIXED");
                break;
        }
        if (attr->defaultValue != NULL) {
            fprintf(ctxt->output, "\"");
            xmlCtxtDumpString(ctxt, attr->defaultValue);
            fprintf(ctxt->output, "\"");
        }
        fprintf(ctxt->output, "\n");
    }

    /*
     * Do a bit of checking
     */
    xmlCtxtGenericNodeCheck(ctxt, (xmlNodePtr) attr);
}

static void
xmlCtxtDumpElemDecl(xmlDebugCtxtPtr ctxt, xmlElementPtr elem)
{
    xmlCtxtDumpSpaces(ctxt);

    if (elem == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "Element declaration is NULL\n");
        return;
    }
    if (elem->type != XML_ELEMENT_DECL) {
	xmlDebugErr(ctxt, XML_CHECK_NOT_ELEM_DECL,
	            "Node is not an element declaration");
        return;
    }
    if (elem->name != NULL) {
        if (!ctxt->check) {
            fprintf(ctxt->output, "ELEMDECL(");
            xmlCtxtDumpString(ctxt, elem->name);
            fprintf(ctxt->output, ")");
        }
    } else
	xmlDebugErr(ctxt, XML_CHECK_NO_NAME,
	            "Element declaration has no name");
    if (!ctxt->check) {
        switch (elem->etype) {
            case XML_ELEMENT_TYPE_UNDEFINED:
                fprintf(ctxt->output, ", UNDEFINED");
                break;
            case XML_ELEMENT_TYPE_EMPTY:
                fprintf(ctxt->output, ", EMPTY");
                break;
            case XML_ELEMENT_TYPE_ANY:
                fprintf(ctxt->output, ", ANY");
                break;
            case XML_ELEMENT_TYPE_MIXED:
                fprintf(ctxt->output, ", MIXED ");
                break;
            case XML_ELEMENT_TYPE_ELEMENT:
                fprintf(ctxt->output, ", MIXED ");
                break;
        }
        if ((elem->type != XML_ELEMENT_NODE) && (elem->content != NULL)) {
            char buf[5001];

            buf[0] = 0;
            xmlSnprintfElementContent(buf, 5000, elem->content, 1);
            buf[5000] = 0;
            fprintf(ctxt->output, "%s", buf);
        }
        fprintf(ctxt->output, "\n");
    }

    /*
     * Do a bit of checking
     */
    xmlCtxtGenericNodeCheck(ctxt, (xmlNodePtr) elem);
}

static void
xmlCtxtDumpEntityDecl(xmlDebugCtxtPtr ctxt, xmlEntityPtr ent)
{
    xmlCtxtDumpSpaces(ctxt);

    if (ent == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "Entity declaration is NULL\n");
        return;
    }
    if (ent->type != XML_ENTITY_DECL) {
	xmlDebugErr(ctxt, XML_CHECK_NOT_ENTITY_DECL,
	            "Node is not an entity declaration");
        return;
    }
    if (ent->name != NULL) {
        if (!ctxt->check) {
            fprintf(ctxt->output, "ENTITYDECL(");
            xmlCtxtDumpString(ctxt, ent->name);
            fprintf(ctxt->output, ")");
        }
    } else
	xmlDebugErr(ctxt, XML_CHECK_NO_NAME,
	            "Entity declaration has no name");
    if (!ctxt->check) {
        switch (ent->etype) {
            case XML_INTERNAL_GENERAL_ENTITY:
                fprintf(ctxt->output, ", internal\n");
                break;
            case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
                fprintf(ctxt->output, ", external parsed\n");
                break;
            case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
                fprintf(ctxt->output, ", unparsed\n");
                break;
            case XML_INTERNAL_PARAMETER_ENTITY:
                fprintf(ctxt->output, ", parameter\n");
                break;
            case XML_EXTERNAL_PARAMETER_ENTITY:
                fprintf(ctxt->output, ", external parameter\n");
                break;
            case XML_INTERNAL_PREDEFINED_ENTITY:
                fprintf(ctxt->output, ", predefined\n");
                break;
        }
        if (ent->ExternalID) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, " ExternalID=%s\n",
                    (char *) ent->ExternalID);
        }
        if (ent->SystemID) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, " SystemID=%s\n",
                    (char *) ent->SystemID);
        }
        if (ent->URI != NULL) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, " URI=%s\n", (char *) ent->URI);
        }
        if (ent->content) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, " content=");
            xmlCtxtDumpString(ctxt, ent->content);
            fprintf(ctxt->output, "\n");
        }
    }

    /*
     * Do a bit of checking
     */
    xmlCtxtGenericNodeCheck(ctxt, (xmlNodePtr) ent);
}

static void
xmlCtxtDumpNamespace(xmlDebugCtxtPtr ctxt, xmlNsPtr ns)
{
    xmlCtxtDumpSpaces(ctxt);

    if (ns == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "namespace node is NULL\n");
        return;
    }
    if (ns->type != XML_NAMESPACE_DECL) {
	xmlDebugErr(ctxt, XML_CHECK_NOT_NS_DECL,
	            "Node is not a namespace declaration");
        return;
    }
    if (ns->href == NULL) {
        if (ns->prefix != NULL)
	    xmlDebugErr3(ctxt, XML_CHECK_NO_HREF,
                    "Incomplete namespace %s href=NULL\n",
                    (char *) ns->prefix);
        else
	    xmlDebugErr(ctxt, XML_CHECK_NO_HREF,
                    "Incomplete default namespace href=NULL\n");
    } else {
        if (!ctxt->check) {
            if (ns->prefix != NULL)
                fprintf(ctxt->output, "namespace %s href=",
                        (char *) ns->prefix);
            else
                fprintf(ctxt->output, "default namespace href=");

            xmlCtxtDumpString(ctxt, ns->href);
            fprintf(ctxt->output, "\n");
        }
    }
}

static void
xmlCtxtDumpNamespaceList(xmlDebugCtxtPtr ctxt, xmlNsPtr ns)
{
    while (ns != NULL) {
        xmlCtxtDumpNamespace(ctxt, ns);
        ns = ns->next;
    }
}

static void
xmlCtxtDumpEntity(xmlDebugCtxtPtr ctxt, xmlEntityPtr ent)
{
    xmlCtxtDumpSpaces(ctxt);

    if (ent == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "Entity is NULL\n");
        return;
    }
    if (!ctxt->check) {
        switch (ent->etype) {
            case XML_INTERNAL_GENERAL_ENTITY:
                fprintf(ctxt->output, "INTERNAL_GENERAL_ENTITY ");
                break;
            case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
                fprintf(ctxt->output, "EXTERNAL_GENERAL_PARSED_ENTITY ");
                break;
            case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
                fprintf(ctxt->output, "EXTERNAL_GENERAL_UNPARSED_ENTITY ");
                break;
            case XML_INTERNAL_PARAMETER_ENTITY:
                fprintf(ctxt->output, "INTERNAL_PARAMETER_ENTITY ");
                break;
            case XML_EXTERNAL_PARAMETER_ENTITY:
                fprintf(ctxt->output, "EXTERNAL_PARAMETER_ENTITY ");
                break;
            default:
                fprintf(ctxt->output, "ENTITY_%d ! ", (int) ent->etype);
        }
        fprintf(ctxt->output, "%s\n", ent->name);
        if (ent->ExternalID) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "ExternalID=%s\n",
                    (char *) ent->ExternalID);
        }
        if (ent->SystemID) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "SystemID=%s\n", (char *) ent->SystemID);
        }
        if (ent->URI) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "URI=%s\n", (char *) ent->URI);
        }
        if (ent->content) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "content=");
            xmlCtxtDumpString(ctxt, ent->content);
            fprintf(ctxt->output, "\n");
        }
    }
}

/**
 * xmlCtxtDumpAttr:
 * @output:  the FILE * for the output
 * @attr:  the attribute
 * @depth:  the indentation level.
 *
 * Dumps debug information for the attribute
 */
static void
xmlCtxtDumpAttr(xmlDebugCtxtPtr ctxt, xmlAttrPtr attr)
{
    xmlCtxtDumpSpaces(ctxt);

    if (attr == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "Attr is NULL");
        return;
    }
    if (!ctxt->check) {
        fprintf(ctxt->output, "ATTRIBUTE ");
	xmlCtxtDumpString(ctxt, attr->name);
        fprintf(ctxt->output, "\n");
        if (attr->children != NULL) {
            ctxt->depth++;
            xmlCtxtDumpNodeList(ctxt, attr->children);
            ctxt->depth--;
        }
    }
    if (attr->name == NULL)
	xmlDebugErr(ctxt, XML_CHECK_NO_NAME,
	            "Attribute has no name");

    /*
     * Do a bit of checking
     */
    xmlCtxtGenericNodeCheck(ctxt, (xmlNodePtr) attr);
}

/**
 * xmlCtxtDumpAttrList:
 * @output:  the FILE * for the output
 * @attr:  the attribute list
 * @depth:  the indentation level.
 *
 * Dumps debug information for the attribute list
 */
static void
xmlCtxtDumpAttrList(xmlDebugCtxtPtr ctxt, xmlAttrPtr attr)
{
    while (attr != NULL) {
        xmlCtxtDumpAttr(ctxt, attr);
        attr = attr->next;
    }
}

/**
 * xmlCtxtDumpOneNode:
 * @output:  the FILE * for the output
 * @node:  the node
 * @depth:  the indentation level.
 *
 * Dumps debug information for the element node, it is not recursive
 */
static void
xmlCtxtDumpOneNode(xmlDebugCtxtPtr ctxt, xmlNodePtr node)
{
    if (node == NULL) {
        if (!ctxt->check) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "node is NULL\n");
        }
        return;
    }
    ctxt->node = node;

    switch (node->type) {
        case XML_ELEMENT_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "ELEMENT ");
                if ((node->ns != NULL) && (node->ns->prefix != NULL)) {
                    xmlCtxtDumpString(ctxt, node->ns->prefix);
                    fprintf(ctxt->output, ":");
                }
                xmlCtxtDumpString(ctxt, node->name);
                fprintf(ctxt->output, "\n");
            }
            break;
        case XML_ATTRIBUTE_NODE:
            if (!ctxt->check)
                xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "Error, ATTRIBUTE found here\n");
            break;
        case XML_TEXT_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                if (node->name == (const xmlChar *) xmlStringTextNoenc)
                    fprintf(ctxt->output, "TEXT no enc\n");
                else
                    fprintf(ctxt->output, "TEXT\n");
            }
            break;
        case XML_CDATA_SECTION_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "CDATA_SECTION\n");
            }
            break;
        case XML_ENTITY_REF_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "ENTITY_REF(%s)\n",
                        (char *) node->name);
            }
            break;
        case XML_ENTITY_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "ENTITY\n");
            }
            break;
        case XML_PI_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "PI %s\n", (char *) node->name);
            }
            break;
        case XML_COMMENT_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "COMMENT\n");
            }
            break;
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
            }
            fprintf(ctxt->output, "PBM: DOCUMENT found here\n");
            break;
        case XML_DOCUMENT_TYPE_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "DOCUMENT_TYPE\n");
            }
            break;
        case XML_DOCUMENT_FRAG_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "DOCUMENT_FRAG\n");
            }
            break;
        case XML_NOTATION_NODE:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "NOTATION\n");
            }
            break;
        case XML_DTD_NODE:
            xmlCtxtDumpDtdNode(ctxt, (xmlDtdPtr) node);
            return;
        case XML_ELEMENT_DECL:
            xmlCtxtDumpElemDecl(ctxt, (xmlElementPtr) node);
            return;
        case XML_ATTRIBUTE_DECL:
            xmlCtxtDumpAttrDecl(ctxt, (xmlAttributePtr) node);
            return;
        case XML_ENTITY_DECL:
            xmlCtxtDumpEntityDecl(ctxt, (xmlEntityPtr) node);
            return;
        case XML_NAMESPACE_DECL:
            xmlCtxtDumpNamespace(ctxt, (xmlNsPtr) node);
            return;
        case XML_XINCLUDE_START:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "INCLUDE START\n");
            }
            return;
        case XML_XINCLUDE_END:
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "INCLUDE END\n");
            }
            return;
        default:
            if (!ctxt->check)
                xmlCtxtDumpSpaces(ctxt);
	    xmlDebugErr2(ctxt, XML_CHECK_UNKNOWN_NODE,
	                "Unknown node type %d\n", node->type);
            return;
    }
    if (node->doc == NULL) {
        if (!ctxt->check) {
            xmlCtxtDumpSpaces(ctxt);
        }
        fprintf(ctxt->output, "PBM: doc == NULL !!!\n");
    }
    ctxt->depth++;
    if (node->nsDef != NULL)
        xmlCtxtDumpNamespaceList(ctxt, node->nsDef);
    if (node->properties != NULL)
        xmlCtxtDumpAttrList(ctxt, node->properties);
    if (node->type != XML_ENTITY_REF_NODE) {
        if ((node->type != XML_ELEMENT_NODE) && (node->content != NULL)) {
            if (!ctxt->check) {
                xmlCtxtDumpSpaces(ctxt);
                fprintf(ctxt->output, "content=");
                xmlCtxtDumpString(ctxt, node->content);
                fprintf(ctxt->output, "\n");
            }
        }
    } else {
        xmlEntityPtr ent;

        ent = xmlGetDocEntity(node->doc, node->name);
        if (ent != NULL)
            xmlCtxtDumpEntity(ctxt, ent);
    }
    ctxt->depth--;

    /*
     * Do a bit of checking
     */
    xmlCtxtGenericNodeCheck(ctxt, node);
}

/**
 * xmlCtxtDumpNode:
 * @output:  the FILE * for the output
 * @node:  the node
 * @depth:  the indentation level.
 *
 * Dumps debug information for the element node, it is recursive
 */
static void
xmlCtxtDumpNode(xmlDebugCtxtPtr ctxt, xmlNodePtr node)
{
    if (node == NULL) {
        if (!ctxt->check) {
            xmlCtxtDumpSpaces(ctxt);
            fprintf(ctxt->output, "node is NULL\n");
        }
        return;
    }
    xmlCtxtDumpOneNode(ctxt, node);
    if ((node->children != NULL) && (node->type != XML_ENTITY_REF_NODE)) {
        ctxt->depth++;
        xmlCtxtDumpNodeList(ctxt, node->children);
        ctxt->depth--;
    }
}

/**
 * xmlCtxtDumpNodeList:
 * @output:  the FILE * for the output
 * @node:  the node list
 * @depth:  the indentation level.
 *
 * Dumps debug information for the list of element node, it is recursive
 */
static void
xmlCtxtDumpNodeList(xmlDebugCtxtPtr ctxt, xmlNodePtr node)
{
    while (node != NULL) {
        xmlCtxtDumpNode(ctxt, node);
        node = node->next;
    }
}

static void
xmlCtxtDumpDocHead(xmlDebugCtxtPtr ctxt, xmlDocPtr doc)
{
    if (doc == NULL) {
        if (!ctxt->check)
            fprintf(ctxt->output, "DOCUMENT == NULL !\n");
        return;
    }
    ctxt->node = (xmlNodePtr) doc;

    switch (doc->type) {
        case XML_ELEMENT_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_ELEMENT,
	                "Misplaced ELEMENT node\n");
            break;
        case XML_ATTRIBUTE_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_ATTRIBUTE,
	                "Misplaced ATTRIBUTE node\n");
            break;
        case XML_TEXT_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_TEXT,
	                "Misplaced TEXT node\n");
            break;
        case XML_CDATA_SECTION_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_CDATA,
	                "Misplaced CDATA node\n");
            break;
        case XML_ENTITY_REF_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_ENTITYREF,
	                "Misplaced ENTITYREF node\n");
            break;
        case XML_ENTITY_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_ENTITY,
	                "Misplaced ENTITY node\n");
            break;
        case XML_PI_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_PI,
	                "Misplaced PI node\n");
            break;
        case XML_COMMENT_NODE:
	    xmlDebugErr(ctxt, XML_CHECK_FOUND_COMMENT,
	                "Misplaced COMMENT node\n");
            break;
        case XML_DOCUMENT_N