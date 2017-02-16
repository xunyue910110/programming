/*
 * parser.c : an XML 1.0 parser, namespaces and validity support are mostly
 *            implemented on top of the SAX interfaces
 *
 * References:
 *   The XML specification:
 *     http://www.w3.org/TR/REC-xml
 *   Original 1.0 version:
 *     http://www.w3.org/TR/1998/REC-xml-19980210
 *   XML second edition working draft
 *     http://www.w3.org/TR/2000/WD-xml-2e-20000814
 *
 * Okay this is a big file, the parser core is around 7000 lines, then it
 * is followed by the progressive parser top routines, then the various
 * high level APIs to call the parser and a few miscellaneous functions.
 * A number of helper functions and deprecated ones have been moved to
 * parserInternals.c to reduce this file size.
 * As much as possible the functions are associated with their relative
 * production in the XML specification. A few productions defining the
 * different ranges of character are actually implanted either in 
 * parserInternals.h or parserInternals.c
 * The DOM tree build is realized from the default SAX callbacks in
 * the module SAX.c.
 * The routines doing the validation checks are in valid.c and called either
 * from the SAX callbacks or as standalone functions using a preparsed
 * document.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#if defined(WIN32) && !defined (__CYGWIN__)
#define XML_DIR_SEP '\\'
#else
#define XML_DIR_SEP '/'
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <libxml/xmlmemory.h>
#include <libxml/threads.h>
#include <libxml/globals.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/entities.h>
#include <libxml/xmlerror.h>
#include <libxml/encoding.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/xmlschemastypes.h>
#include <libxml/relaxng.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif


/**
 * xmlParserMaxDepth:
 *
 * arbitrary depth limit for the XML documents that we allow to 
 * process. This is not a limitation of the parser but a safety 
 * boundary feature.
 */
unsigned int xmlParserMaxDepth = 1024;

#define SAX2 1

#define XML_PARSER_BIG_BUFFER_SIZE 300
#define XML_PARSER_BUFFER_SIZE 100

#define SAX_COMPAT_MODE BAD_CAST "SAX compatibility mode document"

/*
 * List of XML prefixed PI allowed by W3C specs
 */

static const char *xmlW3CPIs[] = {
    "xml-stylesheet",
    NULL
};


/* DEPR void xmlParserHandleReference(xmlParserCtxtPtr ctxt); */
xmlEntityPtr xmlParseStringPEReference(xmlParserCtxtPtr ctxt,
                                       const xmlChar **str);

static xmlParserErrors
xmlParseExternalEntityPrivate(xmlDocPtr doc, xmlParserCtxtPtr oldctxt,
	              xmlSAXHandlerPtr sax,
		      void *user_data, int depth, const xmlChar *URL,
		      const xmlChar *ID, xmlNodePtr *list);

#ifdef LIBXML_LEGACY_ENABLED
static void
xmlAddEntityReference(xmlEntityPtr ent, xmlNodePtr firstNode,
                      xmlNodePtr lastNode);
#endif /* LIBXML_LEGACY_ENABLED */

static xmlParserErrors
xmlParseBalancedChunkMemoryInternal(xmlParserCtxtPtr oldctxt,
		      const xmlChar *string, void *user_data, xmlNodePtr *lst);

/************************************************************************
 *									*
 * 		Some factorized error routines				*
 *									*
 ************************************************************************/

/**
 * xmlErrAttributeDup:
 * @ctxt:  an XML parser context
 * @prefix:  the attribute prefix
 * @localname:  the attribute localname
 *
 * Handle a redefinition of attribute error
 */
static void
xmlErrAttributeDup(xmlParserCtxtPtr ctxt, const xmlChar * prefix,
                   const xmlChar * localname)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = XML_ERR_ATTRIBUTE_REDEFINED;
    if (prefix == NULL)
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        ctxt->errNo, XML_ERR_FATAL, NULL, 0,
                        (const char *) localname, NULL, NULL, 0, 0,
                        "Attribute %s redefined\n", localname);
    else
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        ctxt->errNo, XML_ERR_FATAL, NULL, 0,
                        (const char *) prefix, (const char *) localname,
                        NULL, 0, 0, "Attribute %s:%s redefined\n", prefix,
                        localname);
    ctxt->wellFormed = 0;
    if (ctxt->recovery == 0)
        ctxt->disableSAX = 1;
}

/**
 * xmlFatalErr:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @extra:  extra information string
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
xmlFatalErr(xmlParserCtxtPtr ctxt, xmlParserErrors error, const char *info)
{
    const char *errmsg;

    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    switch (error) {
        case XML_ERR_INVALID_HEX_CHARREF:
            errmsg = "CharRef: invalid hexadecimal value\n";
            break;
        case XML_ERR_INVALID_DEC_CHARREF:
            errmsg = "CharRef: invalid decimal value\n";
            break;
        case XML_ERR_INVALID_CHARREF:
            errmsg = "CharRef: invalid value\n";
            break;
        case XML_ERR_INTERNAL_ERROR:
            errmsg = "internal error";
            break;
        case XML_ERR_PEREF_AT_EOF:
            errmsg = "PEReference at end of document\n";
            break;
        case XML_ERR_PEREF_IN_PROLOG:
            errmsg = "PEReference in prolog\n";
            break;
        case XML_ERR_PEREF_IN_EPILOG:
            errmsg = "PEReference in epilog\n";
            break;
        case XML_ERR_PEREF_NO_NAME:
            errmsg = "PEReference: no name\n";
            break;
        case XML_ERR_PEREF_SEMICOL_MISSING:
            errmsg = "PEReference: expecting ';'\n";
            break;
        case XML_ERR_ENTITY_LOOP:
            errmsg = "Detected an entity reference loop\n";
            break;
        case XML_ERR_ENTITY_NOT_STARTED:
            errmsg = "EntityValue: \" or ' expected\n";
            break;
        case XML_ERR_ENTITY_PE_INTERNAL:
            errmsg = "PEReferences forbidden in internal subset\n";
            break;
        case XML_ERR_ENTITY_NOT_FINISHED:
            errmsg = "EntityValue: \" or ' expected\n";
            break;
        case XML_ERR_ATTRIBUTE_NOT_STARTED:
            errmsg = "AttValue: \" or ' expected\n";
            break;
        case XML_ERR_LT_IN_ATTRIBUTE:
            errmsg = "Unescaped '<' not allowed in attributes values\n";
            break;
        case XML_ERR_LITERAL_NOT_STARTED:
            errmsg = "SystemLiteral \" or ' expected\n";
            break;
        case XML_ERR_LITERAL_NOT_FINISHED:
            errmsg = "Unfinished System or Public ID \" or ' expected\n";
            break;
        case XML_ERR_MISPLACED_CDATA_END:
            errmsg = "Sequence ']]>' not allowed in content\n";
            break;
        case XML_ERR_URI_REQUIRED:
            errmsg = "SYSTEM or PUBLIC, the URI is missing\n";
            break;
        case XML_ERR_PUBID_REQUIRED:
            errmsg = "PUBLIC, the Public Identifier is missing\n";
            break;
        case XML_ERR_HYPHEN_IN_COMMENT:
            errmsg = "Comment must not contain '--' (double-hyphen)\n";
            break;
        case XML_ERR_PI_NOT_STARTED:
            errmsg = "xmlParsePI : no target name\n";
            break;
        case XML_ERR_RESERVED_XML_NAME:
            errmsg = "Invalid PI name\n";
            break;
        case XML_ERR_NOTATION_NOT_STARTED:
            errmsg = "NOTATION: Name expected here\n";
            break;
        case XML_ERR_NOTATION_NOT_FINISHED:
            errmsg = "'>' required to close NOTATION declaration\n";
            break;
        case XML_ERR_VALUE_REQUIRED:
            errmsg = "Entity value required\n";
            break;
        case XML_ERR_URI_FRAGMENT:
            errmsg = "Fragment not allowed";
            break;
        case XML_ERR_ATTLIST_NOT_STARTED:
            errmsg = "'(' required to start ATTLIST enumeration\n";
            break;
        case XML_ERR_NMTOKEN_REQUIRED:
            errmsg = "NmToken expected in ATTLIST enumeration\n";
            break;
        case XML_ERR_ATTLIST_NOT_FINISHED:
            errmsg = "')' required to finish ATTLIST enumeration\n";
            break;
        case XML_ERR_MIXED_NOT_STARTED:
            errmsg = "MixedContentDecl : '|' or ')*' expected\n";
            break;
        case XML_ERR_PCDATA_REQUIRED:
            errmsg = "MixedContentDecl : '#PCDATA' expected\n";
            break;
        case XML_ERR_ELEMCONTENT_NOT_STARTED:
            errmsg = "ContentDecl : Name or '(' expected\n";
            break;
        case XML_ERR_ELEMCONTENT_NOT_FINISHED:
            errmsg = "ContentDecl : ',' '|' or ')' expected\n";
            break;
        case XML_ERR_PEREF_IN_INT_SUBSET:
            errmsg =
                "PEReference: forbidden within markup decl in internal subset\n";
            break;
        case XML_ERR_GT_REQUIRED:
            errmsg = "expected '>'\n";
            break;
        case XML_ERR_CONDSEC_INVALID:
            errmsg = "XML conditional section '[' expected\n";
            break;
        case XML_ERR_EXT_SUBSET_NOT_FINISHED:
            errmsg = "Content error in the external subset\n";
            break;
        case XML_ERR_CONDSEC_INVALID_KEYWORD:
            errmsg =
                "conditional section INCLUDE or IGNORE keyword expected\n";
            break;
        case XML_ERR_CONDSEC_NOT_FINISHED:
            errmsg = "XML conditional section not closed\n";
            break;
        case XML_ERR_XMLDECL_NOT_STARTED:
            errmsg = "Text declaration '<?xml' required\n";
            break;
        case XML_ERR_XMLDECL_NOT_FINISHED:
            errmsg = "parsing XML declaration: '?>' expected\n";
            break;
        case XML_ERR_EXT_ENTITY_STANDALONE:
            errmsg = "external parsed entities cannot be standalone\n";
            break;
        case XML_ERR_ENTITYREF_SEMICOL_MISSING:
            errmsg = "EntityRef: expecting ';'\n";
            break;
        case XML_ERR_DOCTYPE_NOT_FINISHED:
            errmsg = "DOCTYPE improperly terminated\n";
            break;
        case XML_ERR_LTSLASH_REQUIRED:
            errmsg = "EndTag: '</' not found\n";
            break;
        case XML_ERR_EQUAL_REQUIRED:
            errmsg = "expected '='\n";
            break;
        case XML_ERR_STRING_NOT_CLOSED:
            errmsg = "String not closed expecting \" or '\n";
            break;
        case XML_ERR_STRING_NOT_STARTED:
            errmsg = "String not started expecting ' or \"\n";
            break;
        case XML_ERR_ENCODING_NAME:
            errmsg = "Invalid XML encoding name\n";
            break;
        case XML_ERR_STANDALONE_VALUE:
            errmsg = "standalone accepts only 'yes' or 'no'\n";
            break;
        case XML_ERR_DOCUMENT_EMPTY:
            errmsg = "Document is empty\n";
            break;
        case XML_ERR_DOCUMENT_END:
            errmsg = "Extra content at the end of the document\n";
            break;
        case XML_ERR_NOT_WELL_BALANCED:
            errmsg = "chunk is not well balanced\n";
            break;
        case XML_ERR_EXTRA_CONTENT:
            errmsg = "extra content at the end of well balanced chunk\n";
            break;
        case XML_ERR_VERSION_MISSING:
            errmsg = "Malformed declaration expecting version\n";
            break;
#if 0
        case:
            errmsg = "\n";
            break;
#endif
        default:
            errmsg = "Unregistered error message\n";
    }
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER, error,
                    XML_ERR_FATAL, NULL, 0, info, NULL, NULL, 0, 0, errmsg,
                    info);
    ctxt->wellFormed = 0;
    if (ctxt->recovery == 0)
        ctxt->disableSAX = 1;
}

/**
 * xmlFatalErrMsg:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
xmlFatalErrMsg(xmlParserCtxtPtr ctxt, xmlParserErrors error,
               const char *msg)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER, error,
                    XML_ERR_FATAL, NULL, 0, NULL, NULL, NULL, 0, 0, msg);
    ctxt->wellFormed = 0;
    if (ctxt->recovery == 0)
        ctxt->disableSAX = 1;
}

/**
 * xmlWarningMsg:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @str1:  extra data
 * @str2:  extra data
 *
 * Handle a warning.
 */
static void
xmlWarningMsg(xmlParserCtxtPtr ctxt, xmlParserErrors error,
              const char *msg, const xmlChar *str1, const xmlChar *str2)
{
    xmlStructuredErrorFunc schannel = NULL;
    
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    if ((ctxt->sax != NULL) && (ctxt->sax->initialized == XML_SAX2_MAGIC))
        schannel = ctxt->sax->serror;
    __xmlRaiseError(schannel,
                    (ctxt->sax) ? ctxt->sax->warning : NULL,
                    ctxt->userData,
                    ctxt, NULL, XML_FROM_PARSER, error,
                    XML_ERR_WARNING, NULL, 0,
		    (const char *) str1, (const char *) str2, NULL, 0, 0,
		    msg, (const char *) str1, (const char *) str2);
}

/**
 * xmlValidityError:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @str1:  extra data
 *
 * Handle a validity error.
 */
static void
xmlValidityError(xmlParserCtxtPtr ctxt, xmlParserErrors error,
              const char *msg, const xmlChar *str1)
{
    xmlStructuredErrorFunc schannel = NULL;

    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    if ((ctxt->sax != NULL) && (ctxt->sax->initialized == XML_SAX2_MAGIC))
        schannel = ctxt->sax->serror;
    __xmlRaiseError(schannel,
                    ctxt->vctxt.error, ctxt->vctxt.userData,
                    ctxt, NULL, XML_FROM_DTD, error,
                    XML_ERR_ERROR, NULL, 0, (const char *) str1,
		    NULL, NULL, 0, 0,
		    msg, (const char *) str1);
    ctxt->valid = 0;
}

/**
 * xmlFatalErrMsgInt:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @val:  an integer value
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
xmlFatalErrMsgInt(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                  const char *msg, int val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, error, XML_ERR_FATAL,
                    NULL, 0, NULL, NULL, NULL, val, 0, msg, val);
    ctxt->wellFormed = 0;
    if (ctxt->recovery == 0)
        ctxt->disableSAX = 1;
}

/**
 * xmlFatalErrMsgStrIntStr:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @str1:  an string info
 * @val:  an integer value
 * @str2:  an string info
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
xmlFatalErrMsgStrIntStr(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                  const char *msg, const xmlChar *str1, int val, 
		  const xmlChar *str2)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, error, XML_ERR_FATAL,
                    NULL, 0, (const char *) str1, (const char *) str2,
		    NULL, val, 0, msg, str1, val, str2);
    ctxt->wellFormed = 0;
    if (ctxt->recovery == 0)
        ctxt->disableSAX = 1;
}

/**
 * xmlFatalErrMsgStr:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @val:  a string value
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
xmlFatalErrMsgStr(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                  const char *msg, const xmlChar * val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL,
                    XML_FROM_PARSER, error, XML_ERR_FATAL,
                    NULL, 0, (const char *) val, NULL, NULL, 0, 0, msg,
                    val);
    ctxt->wellFormed = 0;
    if (ctxt->recovery == 0)
        ctxt->disableSAX = 1;
}

/**
 * xmlErrMsgStr:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @val:  a string value
 *
 * Handle a non fatal parser error
 */
static void
xmlErrMsgStr(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                  const char *msg, const xmlChar * val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL,
                    XML_FROM_PARSER, error, XML_ERR_ERROR,
                    NULL, 0, (const char *) val, NULL, NULL, 0, 0, msg,
                    val);
}

/**
 * xmlNsErr:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the message
 * @info1:  extra information string
 * @info2:  extra information string
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
xmlNsErr(xmlParserCtxtPtr ctxt, xmlParserErrors error,
         const char *msg,
         const xmlChar * info1, const xmlChar * info2,
         const xmlChar * info3)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_NAMESPACE, error,
                    XML_ERR_ERROR, NULL, 0, (const char *) info1,
                    (const char *) info2, (const char *) info3, 0, 0, msg,
                    info1, info2, info3);
    ctxt->nsWellFormed = 0;
}

/************************************************************************
 *									*
 * 		SAX2 defaulted attributes handling			*
 *									*
 ************************************************************************/

/**
 * xmlDetectSAX2:
 * @ctxt:  an XML parser context
 *
 * Do the SAX2 detection and specific intialization
 */
static void
xmlDetectSAX2(xmlParserCtxtPtr ctxt) {
    if (ctxt == NULL) return;
#ifdef LIBXML_SAX1_ENABLED
    if ((ctxt->sax) &&  (ctxt->sax->initialized == XML_SAX2_MAGIC) &&
        ((ctxt->sax->startElementNs != NULL) ||
         (ctxt->sax->endElementNs != NULL))) ctxt->sax2 = 1;
#else
    ctxt->sax2 = 1;
#endif /* LIBXML_SAX1_ENABLED */

    ctxt->str_xml = xmlDictLookup(ctxt->dict, BAD_CAST "xml", 3);
    ctxt->str_xmlns = xmlDictLookup(ctxt->dict, BAD_CAST "xmlns", 5);
    ctxt->str_xml_ns = xmlDictLookup(ctxt->dict, XML_XML_NAMESPACE, 36);
    if ((ctxt->str_xml==NULL) || (ctxt->str_xmlns==NULL) || 
    		(ctxt->str_xml_ns == NULL)) {
        xmlErrMemory(ctxt, NULL);
    }
}

typedef struct _xmlDefAttrs xmlDefAttrs;
typedef xmlDefAttrs *xmlDefAttrsPtr;
struct _xmlDefAttrs {
    int nbAttrs;	/* number of defaulted attributes on that element */
    int maxAttrs;       /* the size of the array */
    const xmlChar *values[4]; /* array of localname/prefix/values */
};

/**
 * xmlAddDefAttrs:
 * @ctxt:  an XML parser context
 * @fullname:  the element fullname
 * @fullattr:  the attribute fullname
 * @value:  the attribute value
 *
 * Add a defaulted attribute for an element
 */
static void
xmlAddDefAttrs(xmlParserCtxtPtr ctxt,
               const xmlChar *fullname,
               const xmlChar *fullattr,
               const xmlChar *value) {
    xmlDefAttrsPtr defaults;
    int len;
    const xmlChar *name;
    const xmlChar *prefix;

    if (ctxt->attsDefault == NULL) {
        ctxt->attsDefault = xmlHashCreate(10);
	if (ctxt->attsDefault == NULL)
	    goto mem_error;
    }

    /*
     * split the element name into prefix:localname , the string found
     * are within the DTD and then not associated to namespace names.
     */
    name = xmlSplitQName3(fullname, &len);
    if (name == NULL) {
        name = xmlDictLookup(ctxt->dict, fullname, -1);
	prefix = NULL;
    } else {
        name = xmlDictLookup(ctxt->dict, name, -1);
	prefix = xmlDictLookup(ctxt->dict, fullname, len);
    }

    /*
     * make sure there is some storage
     */
    defaults = (xmlDefAttrsPtr)xmlHashLookup2(ctxt->attsDefault, name, prefix);
    if (defaults == NULL) {
        defaults = (xmlDefAttrsPtr) xmlMalloc(sizeof(xmlDefAttrs) +
	                   (4 * 4) * sizeof(const xmlChar *));
	if (defaults == NULL)
	    goto mem_error;
	defaults->nbAttrs = 0;
	defaults->maxAttrs = 4;
	xmlHashUpdateEntry2(ctxt->attsDefault, name, prefix, defaults, NULL);
    } else if (defaults->nbAttrs >= defaults->maxAttrs) {
        xmlDefAttrsPtr temp;

        temp = (xmlDefAttrsPtr) xmlRealloc(defaults, sizeof(xmlDefAttrs) +
		       (2 * defaults->maxAttrs * 4) * sizeof(const xmlChar *));
	if (temp == NULL)
	    goto mem_error;
	defaults = temp;
	defaults->maxAttrs *= 2;
	xmlHashUpdateEntry2(ctxt->attsDefault, name, prefix, defaults, NULL);
    }

    /*
     * plit the element name into prefix:localname , the string found
     * are within the DTD and hen not associated to namespace names.
     */
    name = xmlSplitQName3(fullattr, &len);
    if (name == NULL) {
        name = xmlDictLookup(ctxt->dict, fullattr, -1);
	prefix = NULL;
    } else {
        name = xmlDictLookup(ctxt->dict, name, -1);
	prefix = xmlDictLookup(ctxt->dict, fullattr, len);
    }

    defaults->values[4 * defaults->nbAttrs] = name;
    defaults->values[4 * defaults->nbAttrs + 1] = prefix;
    /* intern the string and precompute the end */
    len = xmlStrlen(value);
    value = xmlDictLookup(ctxt->dict, value, len);
    defaults->values[4 * defaults->nbAttrs + 2] = value;
    defaults->values[4 * defaults->nbAttrs + 3] = value + len;
    defaults->nbAttrs++;

    return;

mem_error:
    xmlErrMemory(ctxt, NULL);
    return;
}

/**
 * xmlAddSpecialAttr:
 * @ctxt:  an XML parser context
 * @fullname:  the element fullname
 * @fullattr:  the attribute fullname
 * @type:  the attribute type
 *
 * Register that this attribute is not CDATA
 */
static void
xmlAddSpecialAttr(xmlParserCtxtPtr ctxt,
		  const xmlChar *fullname,
		  const xmlChar *fullattr,
		  int type)
{
    if (ctxt->attsSpecial == NULL) {
        ctxt->attsSpecial = xmlHashCreate(10);
	if (ctxt->attsSpecial == NULL)
	    goto mem_error;
    }

    xmlHashAddEntry2(ctxt->attsSpecial, fullname, fullattr,
                     (void *) (long) type);
    return;

mem_error:
    xmlErrMemory(ctxt, NULL);
    return;
}

/**
 * xmlCheckLanguageID:
 * @lang:  pointer to the string value
 *
 * Checks that the value conforms to the LanguageID production:
 *
 * NOTE: this is somewhat deprecated, those productions were removed from
 *       the XML Second edition.
 *
 * [33] LanguageID ::= Langcode ('-' Subcode)*
 * [34] Langcode ::= ISO639Code |  IanaCode |  UserCode
 * [35] ISO639Code ::= ([a-z] | [A-Z]) ([a-z] | [A-Z])
 * [36] IanaCode ::= ('i' | 'I') '-' ([a-z] | [A-Z])+
 * [37] UserCode ::= ('x' | 'X') '-' ([a-z] | [A-Z])+
 * [38] Subcode ::= ([a-z] | [A-Z])+
 *
 * Returns 1 if correct 0 otherwise
 **/
int
xmlCheckLanguageID(const xmlChar * lang)
{
    const xmlChar *cur = lang;

    if (cur == NULL)
        return (0);
    if (((cur[0] == 'i') && (cur[1] == '-')) ||
        ((cur[0] == 'I') && (cur[1] == '-'))) {
        /*
         * IANA code
         */
        cur += 2;
        while (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||  /* non input consuming */
               ((cur[0] >= 'a') && (cur[0] <= 'z')))
            cur++;
    } else if (((cur[0] == 'x') && (cur[1] == '-')) ||
               ((cur[0] == 'X') && (cur[1] == '-'))) {
        /*
         * User code
         */
        cur += 2;
        while (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||  /* non input consuming */
               ((cur[0] >= 'a') && (cur[0] <= 'z')))
            cur++;
    } else if (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||
               ((cur[0] >= 'a') && (cur[0] <= 'z'))) {
        /*
         * ISO639
         */
        cur++;
        if (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||
            ((cur[0] >= 'a') && (cur[0] <= 'z')))
            cur++;
        else
            return (0);
    } else
        return (0);
    while (cur[0] != 0) {       /* non input consuming */
        if (cur[0] != '-')
            return (0);
        cur++;
        if (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||
            ((cur[0] >= 'a') && (cur[0] <= 'z')))
            cur++;
        else
            return (0);
        while (((cur[0] >= 'A') && (cur[0] <= 'Z')) ||  /* non input consuming */
               ((cur[0] >= 'a') && (cur[0] <= 'z')))
            cur++;
    }
    return (1);
}

/************************************************************************
 *									*
 * 		Parser stacks related functions and macros		*
 *									*
 ************************************************************************/

xmlEntityPtr xmlParseStringEntityRef(xmlParserCtxtPtr ctxt,
                                     const xmlChar ** str);

#ifdef SAX2
/**
 * nsPush:
 * @ctxt:  an XML parser context
 * @prefix:  the namespace prefix or NULL
 * @URL:  the namespace name
 *
 * Pushes a new parser namespace on top of the ns stack
 *
 * Returns -1 in case of error, -2 if the namespace should be discarded
 *	   and the index in the stack otherwise.
 */
static int
nsPush(xmlParserCtxtPtr ctxt, const xmlChar *prefix, const xmlChar *URL)
{
    if (ctxt->options & XML_PARSE_NSCLEAN) {
        int i;
	for (i = 0;i < ctxt->nsNr;i += 2) {
	    if (ctxt->nsTab[i] == prefix) {
		/* in scope */
	        if (ctxt->nsTab[i + 1] == URL)
		    return(-2);
		/* out of scope keep it */
		break;
	    }
	}
    }
    if ((ctxt->nsMax == 0) || (ctxt->nsTab == NULL)) {
	ctxt->nsMax = 10;
	ctxt->nsNr = 0;
	ctxt->nsTab = (const xmlChar **)
	              xmlMalloc(ctxt->nsMax * sizeof(xmlChar *));
	if (ctxt->nsTab == NULL) {
	    xmlErrMemory(ctxt, NULL);
	    ctxt->nsMax = 0;
            return (-1);
	}
    } else if (ctxt->nsNr >= ctxt->nsMax) {
        ctxt->nsMax *= 2;
        ctxt->nsTab = (const xmlChar **)
	              xmlRealloc((char *) ctxt->nsTab,
				 ctxt->nsMax * sizeof(ctxt->nsTab[0]));
        if (ctxt->nsTab == NULL) {
            xmlErrMemory(ctxt, NULL);
	    ctxt->nsMax /= 2;
            return (-1);
        }
    }
    ctxt->nsTab[ctxt->nsNr++] = prefix;
    ctxt->nsTab[ctxt->nsNr++] = URL;
    return (ctxt->nsNr);
}
/**
 * nsPop:
 * @ctxt: an XML parser context
 * @nr:  the number to pop
 *
 * Pops the top @nr parser prefix/namespace from the ns stack
 *
 * Returns the number of namespaces removed
 */
static int
nsPop(xmlParserCtxtPtr ctxt, int nr)
{
    int i;

    if (ctxt->nsTab == NULL) return(0);
    if (ctxt->nsNr < nr) {
        xmlGenericError(xmlGenericErrorContext, "Pbm popping %d NS\n", nr);
        nr = ctxt->nsNr;
    }
    if (ctxt->nsNr <= 0)
        return (0);
    
    for (i = 0;i < nr;i++) {
         ctxt->nsNr--;
	 ctxt->nsTab[ctxt->nsNr] = NULL;
    }
    return(nr);
}
#endif

static int
xmlCtxtGrowAttrs(xmlParserCtxtPtr ctxt, int nr) {
    const xmlChar **atts;
    int *attallocs;
    int maxatts;

    if (ctxt->atts == NULL) {
	maxatts = 55; /* allow for 10 attrs by default */
	atts = (const xmlChar **)
	       xmlMalloc(maxatts * sizeof(xmlChar *));
	if (atts == NULL) goto mem_error;
	ctxt->atts = atts;
	attallocs = (int *) xmlMalloc((maxatts / 5) * sizeof(int));
	if (attallocs == NULL) goto mem_error;
	ctxt->attallocs = attallocs;
	ctxt->maxatts = maxatts;
    } else if (nr + 5 > ctxt->maxatts) {
	maxatts = (nr + 5) * 2;
	atts = (const xmlChar **) xmlRealloc((void *) ctxt->atts,
				     maxatts * sizeof(const xmlChar *));
	if (atts == NULL) goto mem_error;
	ctxt->atts = atts;
	attallocs = (int *) xmlRealloc((void *) ctxt->attallocs,
	                             (maxatts / 5) * sizeof(int));
	if (attallocs == NULL) goto mem_error;
	ctxt->attallocs = attallocs;
	ctxt->maxatts = maxatts;
    }
    return(ctxt->maxatts);
mem_error:
    xmlErrMemory(ctxt, NULL);
    return(-1);
}

/**
 * inputPush:
 * @ctxt:  an XML parser context
 * @value:  the parser input
 *
 * Pushes a new parser input on top of the input stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
int
inputPush(xmlParserCtxtPtr ctxt, xmlParserInputPtr value)
{
    if ((ctxt == NULL) || (value == NULL))
        return(0);
    if (ctxt->inputNr >= ctxt->inputMax) {
        ctxt->inputMax *= 2;
        ctxt->inputTab =
            (xmlParserInputPtr *) xmlRealloc(ctxt->inputTab,
                                             ctxt->inputMax *
                                             sizeof(ctxt->inputTab[0]));
        if (ctxt->inputTab == NULL) {
            xmlErrMemory(ctxt, NULL);
            return (0);
        }
    }
    ctxt->inputTab[ctxt->inputNr] = value;
    ctxt->input = value;
    return (ctxt->inputNr++);
}
/**
 * inputPop:
 * @ctxt: an XML parser context
 *
 * Pops the top parser input from the input stack
 *
 * Returns the input just removed
 */
xmlParserInputPtr
inputPop(xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr ret;

    if (ctxt == NULL)
        return(NULL);
    if (ctxt->inputNr <= 0)
        return (0);
    ctxt->inputNr--;
    if (ctxt->inputNr > 0)
        ctxt->input = ctxt->inputTab[ctxt->inputNr - 1];
    else
        ctxt->input = NULL;
    ret = ctxt->inputTab[ctxt->inputNr];
    ctxt->inputTab[ctxt->inputNr] = 0;
    return (ret);
}
/**
 * nodePush:
 * @ctxt:  an XML parser context
 * @value:  the element node
 *
 * Pushes a new element node on top of the node stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
int
nodePush(xmlParserCtxtPtr ctxt, xmlNodePtr value)
{
    if (ctxt == NULL) return(0);
    if (ctxt->nodeNr >= ctxt->nodeMax) {
        xmlNodePtr *tmp;

	tmp = (xmlNodePtr *) xmlRealloc(ctxt->nodeTab,
                                      ctxt->nodeMax * 2 *
                                      sizeof(ctxt->nodeTab[0]));
        if (tmp == NULL) {
            xmlErrMemory(ctxt, NULL);
            return (0);
        }
        ctxt->nodeTab = tmp;
	ctxt->nodeMax *= 2;
    }
    if (((unsigned int) ctxt->nodeNr) > xmlParserMaxDepth) {
	xmlFatalErrMsgInt(ctxt, XML_ERR_INTERNAL_ERROR,
		 "Excessive depth in document: change xmlParserMaxDepth = %d\n",
			  xmlParserMaxDepth);
	ctxt->instate = XML_PARSER_EOF;
	return(0);
    }
    ctxt->nodeTab[ctxt->nodeNr] = value;
    ctxt->node = value;
    return (ctxt->nodeNr++);
}
/**
 * nodePop:
 * @ctxt: an XML parser context
 *
 * Pops the top element node from the node stack
 *
 * Returns the node just removed
 */
xmlNodePtr
nodePop(xmlParserCtxtPtr ctxt)
{
    xmlNodePtr ret;

    if (ctxt == NULL) return(NULL);
    if (ctxt->nodeNr <= 0)
        return (NULL);
    ctxt->nodeNr--;
    if (ctxt->nodeNr > 0)
        ctxt->node = ctxt->nodeTab[ctxt->nodeNr - 1];
    else
        ctxt->node = NULL;
    ret = ctxt->nodeTab[ctxt->nodeNr];
    ctxt->nodeTab[ctxt->nodeNr] = 0;
    return (ret);
}

#ifdef LIBXML_PUSH_ENABLED
/**
 * nameNsPush:
 * @ctxt:  an XML parser context
 * @value:  the element name
 * @prefix:  the element prefix
 * @URI:  the element namespace name
 *
 * Pushes a new element name/prefix/URL on top of the name stack
 *
 * Returns -1 in case of error, the index in the stack otherwise
 */
static int
nameNsPush(xmlParserCtxtPtr ctxt, const xmlChar * value,
           const xmlChar *prefix, const xmlChar *URI, int nsNr)
{
    if (ctxt->nameNr >= ctxt->nameMax) {
        const xmlChar * *tmp;
        void **tmp2;
        ctxt->nameMax *= 2;
        tmp = (cons