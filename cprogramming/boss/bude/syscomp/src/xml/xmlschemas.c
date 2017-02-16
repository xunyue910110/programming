/*
 * schemas.c : implementation of the XML Schema handling and
 *             schema validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

/*
 * TODO:
 *   - when types are redefined in includes, check that all
 *     types in the redef list are equal
 *     -> need a type equality operation.
 *   - if we don't intend to use the schema for schemas, we 
 *     need to validate all schema attributes (ref, type, name)
 *     against their types.
 */
#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_SCHEMAS_ENABLED

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/hash.h>
#include <libxml/uri.h>

#include <libxml/xmlschemas.h>
#include <libxml/schemasInternals.h>
#include <libxml/xmlschemastypes.h>
#include <libxml/xmlautomata.h>
#include <libxml/xmlregexp.h>
#include <libxml/dict.h>

/* #define DEBUG 1 */

/* #define DEBUG_CONTENT 1 */

/* #define DEBUG_TYPE 1 */

/* #define DEBUG_CONTENT_REGEXP 1 */

/* #define DEBUG_AUTOMATA 1 */

/* #define DEBUG_ATTR_VALIDATION 1 */

/* #define DEBUG_UNION_VALIDATION 1 */


#define UNBOUNDED (1 << 30)
#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define XML_SCHEMAS_NO_NAMESPACE (const xmlChar *) "##"

/*
 * The XML Schemas namespace1s
 */
static const xmlChar *xmlSchemaNs = (const xmlChar *)
    "http://www.w3.org/2001/XMLSchema";

static const xmlChar *xmlSchemaInstanceNs = (const xmlChar *)
    "http://www.w3.org/2001/XMLSchema-instance";

static const xmlChar *xmlSchemaElemDesElemDecl = (const xmlChar *)
    "Element decl.";
static const xmlChar *xmlSchemaElemDesElemRef = (const xmlChar *)
    "Element ref.";
static const xmlChar *xmlSchemaElemDesAttrDecl = (const xmlChar *)
    "Attribute decl.";
static const xmlChar *xmlSchemaElemDesAttrRef = (const xmlChar *)
    "Attribute ref.";
static const xmlChar *xmlSchemaElemDesST = (const xmlChar *)
    "ST";
static const xmlChar *xmlSchemaElemDesCT = (const xmlChar *)
    "CT";

#define IS_SCHEMA(node, type)						\
   ((node != NULL) && (node->ns != NULL) &&				\
    (xmlStrEqual(node->name, (const xmlChar *) type)) &&		\
    (xmlStrEqual(node->ns->href, xmlSchemaNs)))

#define FREE_AND_NULL(str)						\
    if (str != NULL) {							\
	xmlFree(str);							\
	str = NULL;							\
    }

#define IS_ANYTYPE(item)                           \
    ((item->type == XML_SCHEMA_TYPE_BASIC) &&      \
     (item->builtInType == XML_SCHEMAS_ANYTYPE))   

#define IS_COMPLEX_TYPE(item)                      \
    ((item->type == XML_SCHEMA_TYPE_COMPLEX) ||    \
     (item->builtInType == XML_SCHEMAS_ANYTYPE))

#define IS_SIMPLE_TYPE(item)                       \
    ((item->type == XML_SCHEMA_TYPE_SIMPLE) ||     \
     ((item->type == XML_SCHEMA_TYPE_BASIC) &&     \
      (item->builtInType != XML_SCHEMAS_ANYTYPE))) 

#define XML_SCHEMAS_VAL_WTSP_PRESERVE 0
#define XML_SCHEMAS_VAL_WTSP_REPLACE  1
#define XML_SCHEMAS_VAL_WTSP_COLLAPSE 2

#define XML_SCHEMAS_PARSE_ERROR		1

#define SCHEMAS_PARSE_OPTIONS XML_PARSE_NOENT


/*
* XML_SCHEMA_VAL_XSI_ASSEMBLE_TNS_COMPOSE	 
* allow to assemble schemata with 
* the same target namespace1 from 
* different sources; otherwise, the first 
* encountered schema with a specific target 
* namespace1 will be used only *
   
* 
* XML_SCHEMA_VAL_LOCATE_BY_NSNAME = 1<<2
* locate schemata to be imported
* using the namespace1 name; otherwise
* the location URI will be used */

/*
* xmlSchemaParserOption:
*
* This is the set of XML Schema parser options.
*
typedef enum {
    XML_SCHEMA_PAR_LOCATE_BY_NSNAME	= 1<<0
	* locate schemata to be imported
	* using the namespace1 name; otherwise
	* the location URI will be used *
} xmlSchemaParserOption;
*/

/*
XMLPUBFUN int XMLCALL
	    xmlSchemaParserCtxtSetOptions(xmlSchemaParserCtxtPtr ctxt,
					  int options);
XMLPUBFUN int XMLCALL
	    xmlSchemaParserCtxtGetOptions(xmlSchemaParserCtxtPtr ctxt);

*/

typedef struct _xmlSchemaAssemble xmlSchemaAssemble;
typedef xmlSchemaAssemble *xmlSchemaAssemblePtr;
struct _xmlSchemaAssemble {
    void **items;  /* used for dynamic addition of schemata */
    int nbItems; /* used for dynamic addition of schemata */
    int sizeItems; /* used for dynamic addition of schemata */
};

struct _xmlSchemaParserCtxt {
    void *userData;             /* user specific data block */
    xmlSchemaValidityErrorFunc error;   /* the callback in case of errors */
    xmlSchemaValidityWarningFunc warning;       /* the callback in case of warning */
    xmlSchemaValidError err;
    int nberrors;
    xmlStructuredErrorFunc serror;

    xmlSchemaPtr topschema;	/* The main schema */
    xmlHashTablePtr namespace1s;	/* Hash table of namespace1s to schemas */

    xmlSchemaPtr schema;        /* The schema in use */
    const xmlChar *container;   /* the current element, group, ... */
    int counter;

    const xmlChar *URL;
    xmlDocPtr doc;
    int preserve;		/* Whether the doc should be freed  */

    const char *buffer;
    int size;

    /*
     * Used to build complex element content models
     */
    xmlAutomataPtr am;
    xmlAutomataStatePtr start;
    xmlAutomataStatePtr end;
    xmlAutomataStatePtr state;

    xmlDictPtr dict;		/* dictionnary for interned string names */
    int        includes;	/* the inclusion level, 0 for root or imports */
    xmlSchemaTypePtr ctxtType; /* The current context simple/complex type */
    xmlSchemaTypePtr parentItem; /* The current parent schema item */
    xmlSchemaAssemblePtr assemble;
    int options;
    xmlSchemaValidCtxtPtr vctxt;
};


#define XML_SCHEMAS_ATTR_UNKNOWN 1
#define XML_SCHEMAS_ATTR_CHECKED 2
#define XML_SCHEMAS_ATTR_PROHIBITED 3
#define XML_SCHEMAS_ATTR_MISSING 4
#define XML_SCHEMAS_ATTR_INVALID_VALUE 5
#define XML_SCHEMAS_ATTR_TYPE_NOT_RESOLVED 6
#define XML_SCHEMAS_ATTR_INVALID_FIXED_VALUE 7
#define XML_SCHEMAS_ATTR_DEFAULT 8

typedef struct _xmlSchemaAttrState xmlSchemaAttrState;
typedef xmlSchemaAttrState *xmlSchemaAttrStatePtr;
struct _xmlSchemaAttrState {
    xmlSchemaAttrStatePtr next;
    xmlAttrPtr attr;
    int state;
    xmlSchemaAttributePtr decl;
    const xmlChar *value;
};

/**
 * xmlSchemaValidCtxt:
 *
 * A Schemas validation context
 */

struct _xmlSchemaValidCtxt {
    void *userData;             /* user specific data block */
    xmlSchemaValidityErrorFunc error;   /* the callback in case of errors */
    xmlSchemaValidityWarningFunc warning;       /* the callback in case of warning */
    xmlStructuredErrorFunc serror;

    xmlSchemaPtr schema;        /* The schema in use */
    xmlDocPtr doc;
    xmlParserInputBufferPtr input;
    xmlCharEncoding enc;
    xmlSAXHandlerPtr sax;
    void *user_data;

    xmlDocPtr myDoc;
    int err;
    int nberrors;

    xmlNodePtr node;
    xmlNodePtr cur;
    xmlSchemaTypePtr type;

    xmlRegExecCtxtPtr regexp;
    xmlSchemaValPtr value;

    xmlSchemaAttrStatePtr attrTop;
    xmlSchemaAttrStatePtr attr;
    /* xmlNodePtr scope; not used */
    int valueWS;
    int options;
    xmlNodePtr validationRoot;    
    xmlSchemaParserCtxtPtr pctxt;
    int xsiAssemble;
};

/*
 * These are the entries in the schemas importSchemas hash table
 */
typedef struct _xmlSchemaImport xmlSchemaImport;
typedef xmlSchemaImport *xmlSchemaImportPtr;
struct _xmlSchemaImport {
    const xmlChar *schemaLocation;
    xmlSchemaPtr schema; /* not used any more */
    xmlDocPtr doc;
    int isMain;
};

/*
 * These are the entries associated to includes in a schemas
 */
typedef struct _xmlSchemaInclude xmlSchemaInclude;
typedef xmlSchemaInclude *xmlSchemaIncludePtr;
struct _xmlSchemaInclude {
    xmlSchemaIncludePtr next;

    const xmlChar *schemaLocation;
    xmlDocPtr doc;
};

typedef struct _xmlSchemaParticle xmlSchemaParticle;
typedef xmlSchemaParticle *xmlSchemaParticlePtr;
struct _xmlSchemaParticle {
    xmlSchemaTypeType type;
    xmlSchemaParticlePtr next; /* the next particle if in a list */
    int minOccurs;
    int maxOccurs;
    xmlSchemaTypePtr term;
};


typedef struct _xmlSchemaModelGroup xmlSchemaModelGroup;
typedef xmlSchemaModelGroup *xmlSchemaModelGroupPtr;
struct _xmlSchemaModelGroup {
    xmlSchemaTypeType type;
    int compositor; /* one of all, choice or sequence */
    xmlSchemaParticlePtr particles; /* list of particles */
    xmlSchemaAnnotPtr annot;
};

typedef struct _xmlSchemaModelGroupDef xmlSchemaModelGroupDef;
typedef xmlSchemaModelGroupDef *xmlSchemaModelGroupDefPtr;
struct _xmlSchemaModelGroupDef {
    xmlSchemaTypeType type;
    const xmlChar *name;
    const xmlChar *targetNamespace;
    xmlSchemaModelGroupPtr modelGroup;
    xmlSchemaAnnotPtr annot;
};

/************************************************************************
 * 									*
 * 			Some predeclarations				*
 * 									*
 ************************************************************************/

static int xmlSchemaParseInclude(xmlSchemaParserCtxtPtr ctxt,
                                 xmlSchemaPtr schema,
                                 xmlNodePtr node);
static void
xmlSchemaTypeFixup(xmlSchemaTypePtr typeDecl,
                   xmlSchemaParserCtxtPtr ctxt, const xmlChar * name);
static const char *
xmlSchemaFacetTypeToString(xmlSchemaTypeType type);
static int
xmlSchemaValidateSimpleTypeValue(xmlSchemaValidCtxtPtr ctxt, 
				 xmlSchemaTypePtr type,
				 const xmlChar *value,
				 int fireErrors,				 
				 int applyFacets,
				 int normalize,
				 int checkNodes);
static int
xmlSchemaValidateElementByDeclaration(xmlSchemaValidCtxtPtr ctxt,
				      xmlSchemaElementPtr elemDecl); 
static int
xmlSchemaValidateElementByWildcard(xmlSchemaValidCtxtPtr ctxt,
				   xmlSchemaTypePtr type);
static int
xmlSchemaHasElemOrCharContent(xmlNodePtr node);
static int
xmlSchemaParseImport(xmlSchemaParserCtxtPtr ctxt, xmlSchemaPtr schema,
                     xmlNodePtr node);

/************************************************************************
 *									*
 * 			Datatype error handlers				*
 *									*
 ************************************************************************/

/**
 * xmlSchemaPErrMemory:
 * @node: a context node
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlSchemaPErrMemory(xmlSchemaParserCtxtPtr ctxt,
                    const char *extra, xmlNodePtr node)
{
    if (ctxt != NULL)
        ctxt->nberrors++;
    __xmlSimpleError(XML_FROM_SCHEMASP, XML_ERR_NO_MEMORY, node, NULL,
                     extra);
}

/**
 * xmlSchemaPErr:
 * @ctxt: the parsing context
 * @node: the context node
 * @error: the error code
 * @msg: the error message
 * @str1: extra data
 * @str2: extra data
 * 
 * Handle a parser error
 */
static void
xmlSchemaPErr(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node, int error,
              const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    xmlGenericErrorFunc channel = NULL;
    xmlStructuredErrorFunc schannel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        ctxt->nberrors++;
        channel = ctxt->error;
        data = ctxt->userData;
	schannel = ctxt->serror;
    }
    __xmlRaiseError(schannel, channel, data, ctxt, node, XML_FROM_SCHEMASP,
                    error, XML_ERR_ERROR, NULL, 0,
                    (const char *) str1, (const char *) str2, NULL, 0, 0,
                    msg, str1, str2);
}

/**
 * xmlSchemaPErr2:
 * @ctxt: the parsing context
 * @node: the context node
 * @node: the current child
 * @error: the error code
 * @msg: the error message
 * @str1: extra data
 * @str2: extra data
 * 
 * Handle a parser error
 */
static void
xmlSchemaPErr2(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node,
               xmlNodePtr child, int error,
               const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    if (child != NULL)
        xmlSchemaPErr(ctxt, child, error, msg, str1, str2);
    else
        xmlSchemaPErr(ctxt, node, error, msg, str1, str2);
}


/**
 * xmlSchemaPErrExt:
 * @ctxt: the parsing context
 * @node: the context node
 * @error: the error code 
 * @strData1: extra data
 * @strData2: extra data
 * @strData3: extra data
 * @msg: the message
 * @str1:  extra parameter for the message display
 * @str2:  extra parameter for the message display
 * @str3:  extra parameter for the message display
 * @str4:  extra parameter for the message display
 * @str5:  extra parameter for the message display
 * 
 * Handle a parser error
 */
static void
xmlSchemaPErrExt(xmlSchemaParserCtxtPtr ctxt, xmlNodePtr node, int error,
		const xmlChar * strData1, const xmlChar * strData2, 
		const xmlChar * strData3, const char *msg, const xmlChar * str1, 
		const xmlChar * str2, const xmlChar * str3, const xmlChar * str4,
		const xmlChar * str5)
{

    xmlGenericErrorFunc channel = NULL;
    xmlStructuredErrorFunc schannel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        ctxt->nberrors++;
        channel = ctxt->error;
        data = ctxt->userData;
	schannel = ctxt->serror;
    }
    __xmlRaiseError(schannel, channel, data, ctxt, node, XML_FROM_SCHEMASP,
                    error, XML_ERR_ERROR, NULL, 0,
                    (const char *) strData1, (const char *) strData2, 
		    (const char *) strData3, 0, 0, msg, str1, str2, 
		    str3, str4, str5);
}


/**
 * xmlSchemaVTypeErrMemory:
 * @node: a context node
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlSchemaVErrMemory(xmlSchemaValidCtxtPtr ctxt,
                    const char *extra, xmlNodePtr node)
{
    if (ctxt != NULL) {
        ctxt->nberrors++;
        ctxt->err = XML_SCHEMAV_INTERNAL;
    }
    __xmlSimpleError(XML_FROM_SCHEMASV, XML_ERR_NO_MEMORY, node, NULL,
                     extra);
}

/**
 * xmlSchemaVErr3:
 * @ctxt: the validation context
 * @node: the context node
 * @error: the error code
 * @msg: the error message
 * @str1: extra data
 * @str2: extra data
 * @str3: extra data
 * 
 * Handle a validation error
 */
static void
xmlSchemaVErr3(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node, int error,
               const char *msg, const xmlChar *str1, const xmlChar *str2,
	       const xmlChar *str3)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        ctxt->nberrors++;
	ctxt->err = error;
        channel = ctxt->error;
        schannel = ctxt->serror;
        data = ctxt->userData;
    }
    /* reajust to global error numbers */
    /* Removed, since the old schema error codes have been 
    * substituted for the global error codes.
    *
    * error += XML_SCHEMAV_NOROOT - XML_SCHEMAS_ERR_NOROOT; 
    */
    __xmlRaiseError(schannel, channel, data, ctxt, node, XML_FROM_SCHEMASV,
                    error, XML_ERR_ERROR, NULL, 0,
                    (const char *) str1, (const char *) str2,
		    (const char *) str3, 0, 0,
                    msg, str1, str2, str3);
}

/**
 * xmlSchemaVErrExt:
 * @ctxt: the validation context
 * @node: the context node
 * @error: the error code 
 * @msg: the message
 * @str1:  extra parameter for the message display
 * @str2:  extra parameter for the message display
 * @str3:  extra parameter for the message display
 * @str4:  extra parameter for the message display
 * @str5:  extra parameter for the message display
 * 
 * Handle a validation error
 */
static void
xmlSchemaVErrExt(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node, int error,
		 const char *msg, const xmlChar * str1, 
		 const xmlChar * str2, const xmlChar * str3, 
		 const xmlChar * str4, const xmlChar * str5)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        ctxt->nberrors++;
	ctxt->err = error;
        channel = ctxt->error;
        schannel = ctxt->serror;
        data = ctxt->userData;
    }
    /* reajust to global error numbers */
     /* Removed, since the old schema error codes have been 
    * substituted for the global error codes.
    *
    * error += XML_SCHEMAV_NOROOT - XML_SCHEMAS_ERR_NOROOT;
    */
    __xmlRaiseError(schannel, channel, data, ctxt, node, XML_FROM_SCHEMASP,
                    error, XML_ERR_ERROR, NULL, 0, NULL, NULL, NULL, 0, 0, 
		    msg, str1, str2, str3, str4, str5);
}
/**
 * xmlSchemaVErr:
 * @ctxt: the validation context
 * @node: the context node
 * @error: the error code
 * @msg: the error message
 * @str1: extra data
 * @str2: extra data
 * 
 * Handle a validation error
 */
static void
xmlSchemaVErr(xmlSchemaValidCtxtPtr ctxt, xmlNodePtr node, int error,
              const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        ctxt->nberrors++;
	ctxt->err = error;
        channel = ctxt->error;
        data = ctxt->userData;
        schannel = ctxt->serror;
    }
    /* reajust to global error numbers */
    /* Removed, since the old schema error codes have been 
    * substituted for the global error codes.
    *
    * error += XML_SCHEMAV_NOROOT - XML_SCHEMAS_ERR_NOROOT;
    */
    __xmlRaiseError(schannel, channel, data, ctxt, node, XML_FROM_SCHEMASV,
                    error, XML_ERR_ERROR, NULL, 0,
                    (const char *) str1, (const char *) str2, NULL, 0, 0,
                    msg, str1, str2);
}

/**
 * xmlSchemaGetAttrName:
 * @attr:  the attribute declaration/use
 *
 * Returns the name of the attribute; if the attribute
 * is a reference, the name of the referenced global type will be returned.
 */
static const xmlChar *
xmlSchemaGetAttrName(xmlSchemaAttributePtr attr) 
{
    if (attr->ref != NULL) 
	return(attr->ref);
    else
	return(attr->name);	
}

/**
 * xmlSchemaGetAttrTargetNsURI:
 * @type:  the type (element or attribute)
 *
 * Returns the target namespace1 URI of the type; if the type is a reference,
 * the target namespace1 of the referenced type will be returned.
 */
static const xmlChar *
xmlSchemaGetAttrTargetNsURI(xmlSchemaAttributePtr attr)
{  
    if (attr->ref != NULL)
	return (attr->refNs);
    else
	return(attr->targetNamespace);  
}

/**
 * xmlSchemaFormatNsUriLocal:
 * @buf: the string buffer
 * @uri:  the namespace1 URI
 * @local: the local name
 *
 * Returns a representation of the given URI used
 * for error reports.
 *
 * Returns an empty string, if @ns is NULL, a formatted
 * string otherwise.
 */  
static const xmlChar*   
xmlSchemaFormatNsUriLocal(xmlChar **buf,
			   const xmlChar *uri, const xmlChar *local)
{
    if (*buf != NULL)
	xmlFree(*buf);
    if (uri == NULL) {
	*buf = xmlStrdup(BAD_CAST "{'");
	*buf = xmlStrcat(*buf, local);
    } else {
	*buf = xmlStrdup(BAD_CAST "{'");
	*buf = xmlStrcat(*buf, uri);
	*buf = xmlStrcat(*buf, BAD_CAST "', '");
	*buf = xmlStrcat(*buf, local);	
    }
    *buf = xmlStrcat(*buf, BAD_CAST "'}");
    return ((const xmlChar *) *buf);
}

/**
 * xmlSchemaFormatNsPrefixLocal:
 * @buf: the string buffer
 * @ns:  the namespace1
 * @local: the local name
 *
 * Returns a representation of the given URI used
 * for error reports.
 *
 * Returns an empty string, if @ns is NULL, a formatted
 * string otherwise.
 */  
static const xmlChar*   
xmlSchemaFormatNsPrefixLocal(xmlChar **buf,
			      xmlNsPtr ns, const xmlChar *local)
{
    if (*buf != NULL) {
	xmlFree(*buf);
	*buf = NULL;
    }
    if ((ns == NULL) || (ns->prefix == NULL))
	return(local);
    else {
	*buf = xmlStrdup(ns->prefix);
	*buf = xmlStrcat(*buf, BAD_CAST ":");
	*buf = xmlStrcat(*buf, local);
    }
    return ((const xmlChar *) *buf);
}

/**
 * xmlSchemaFormatItemForReport:
 * @buf: the string buffer
 * @itemDes: the designation of the item
 * @itemName: the name of the item
 * @item: the item as an object 
 * @itemNode: the node of the item
 * @local: the local name
 * @parsing: if the function is used during the parse
 *
 * Returns a representation of the given item used
 * for error reports. 
 *
 * The following order is used to build the resulting 
 * designation if the arguments are not NULL:
 * 1a. If itemDes not NULL -> itemDes
 * 1b. If (itemDes not NULL) and (itemName not NULL)
 *     -> itemDes + itemName
 * 2. If the preceding was NULL and (item not NULL) -> item
 * 3. If the preceding was NULL and (itemNode not NULL) -> itemNode
 * 
 * If the itemNode is an attribute node, the name of the attribute
 * will be appended to the result.
 *
 * Returns the formatted string and sets @buf to the resulting value.
 */  
static xmlChar*   
xmlSchemaFormatItemForReport(xmlChar **buf,		     
		     const xmlChar *itemDes,
		     xmlSchemaTypePtr item,
		     xmlNodePtr itemNode,
		     int parsing)
{
    xmlChar *str = NULL;
    int named = 1;

    if (*buf != NULL) {
	xmlFree(*buf);
	*buf = NULL;
    }
            
    if (itemDes != NULL) {
	*buf = xmlStrdup(itemDes);	
    } else if (item != NULL) {
	if (item->type == XML_SCHEMA_TYPE_BASIC) {
	    if (item->builtInType == XML_SCHEMAS_ANYTYPE)
		*buf = xmlStrdup(BAD_CAST "'anyType'");
	    else if (item->builtInType == XML_SCHEMAS_ANYSIMPLETYPE)
		*buf = xmlStrdup(BAD_CAST "'anySimpleType'");
	    else {
		/* *buf = xmlStrdup(BAD_CAST "bi "); */
		/* *buf = xmlStrcat(*buf, xmlSchemaElemDesST); */
		*buf = xmlStrdup(BAD_CAST "'");
		*buf = xmlStrcat(*buf, item->name);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    }
	} else if (item->type == XML_SCHEMA_TYPE_SIMPLE) {
	    if (item->flags & XML_SCHEMAS_TYPE_GLOBAL) {
		*buf = xmlStrdup(xmlSchemaElemDesST);
		*buf = xmlStrcat(*buf, BAD_CAST " '");
		*buf = xmlStrcat(*buf, item->name);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    } else {
		*buf = xmlStrdup(xmlSchemaElemDesST);
		*buf = xmlStrcat(*buf, BAD_CAST " local");
	    }
	} else if (item->type == XML_SCHEMA_TYPE_COMPLEX) {
	    if (item->flags & XML_SCHEMAS_TYPE_GLOBAL) {
		*buf = xmlStrdup(xmlSchemaElemDesCT);
		*buf = xmlStrcat(*buf, BAD_CAST " '");
		*buf = xmlStrcat(*buf, item->name);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    } else {
		*buf = xmlStrdup(xmlSchemaElemDesCT);
		*buf = xmlStrcat(*buf, BAD_CAST " local");
	    }
	} else if (item->type == XML_SCHEMA_TYPE_ATTRIBUTE) {
	    xmlSchemaAttributePtr attr;

	    attr = (xmlSchemaAttributePtr) item;	    
	    if ((attr->flags & XML_SCHEMAS_TYPE_GLOBAL) ||
		(attr->ref == NULL)) {
		*buf = xmlStrdup(xmlSchemaElemDesAttrDecl);
		*buf = xmlStrcat(*buf, BAD_CAST " '");
		*buf = xmlStrcat(*buf, attr->name);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    } else {
		*buf = xmlStrdup(xmlSchemaElemDesAttrRef);
		*buf = xmlStrcat(*buf, BAD_CAST " '");
		*buf = xmlStrcat(*buf, attr->refPrefix);
		*buf = xmlStrcat(*buf, BAD_CAST ":");
		*buf = xmlStrcat(*buf, attr->ref);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	   }		
	} else if (item->type == XML_SCHEMA_TYPE_ELEMENT) {
	    xmlSchemaElementPtr elem;

	    elem = (xmlSchemaElementPtr) item;	    
	    if ((elem->flags & XML_SCHEMAS_TYPE_GLOBAL) || 
		(elem->ref == NULL)) {
		*buf = xmlStrdup(xmlSchemaElemDesElemDecl);
		*buf = xmlStrcat(*buf, BAD_CAST " '");
		*buf = xmlStrcat(*buf, elem->name);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    } else {
		*buf = xmlStrdup(xmlSchemaElemDesElemRef);
		*buf = xmlStrcat(*buf, BAD_CAST " '");
		*buf = xmlStrcat(*buf, elem->refPrefix);
		*buf = xmlStrcat(*buf, BAD_CAST ":");
		*buf = xmlStrcat(*buf, elem->ref);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    }		
	} else
	    named = 0;
    } else 
	named = 0;

    if ((named == 0) && (itemNode != NULL)) {
	xmlNodePtr elem;

	if (itemNode->type == XML_ATTRIBUTE_NODE)
	    elem = itemNode->parent;
	else 
	    elem = itemNode;
	*buf = xmlStrdup(BAD_CAST "Element '");
	if (parsing)
	    *buf = xmlStrcat(*buf, elem->name);
	else
	    *buf = xmlStrcat(*buf, 
		xmlSchemaFormatNsPrefixLocal(&str, elem->ns, elem->name));
	*buf = xmlStrcat(*buf, BAD_CAST "'");
    }
    if ((itemNode != NULL) && (itemNode->type == XML_ATTRIBUTE_NODE)) {
	*buf = xmlStrcat(*buf, BAD_CAST ", attribute '");
	*buf = xmlStrcat(*buf, xmlSchemaFormatNsPrefixLocal(&str, 
	    itemNode->ns, itemNode->name));
	*buf = xmlStrcat(*buf, BAD_CAST "'");
    }
    FREE_AND_NULL(str);
    
    return (*buf);
}

/**
 * xmlSchemaPFormatItemDes:
 * @buf: the string buffer
 * @item: the item as a schema object
 * @itemNode: the item as a node
 *
 * If the pointer to @buf is not NULL and @but holds no value,
 * the value is set to a item designation using 
 * xmlSchemaFormatItemForReport. This one avoids adding
 * an attribute designation postfix.
 *
 * Returns a string of all enumeration elements.
 */
static void
xmlSchemaPRequestItemDes(xmlChar **buf,
		       xmlSchemaTypePtr item,
		       xmlNodePtr itemNode)
{
    if ((buf == 0) || (*buf != NULL)) 
	return;
    if (itemNode->type == XML_ATTRIBUTE_NODE)
	itemNode = itemNode->parent;
    xmlSchemaFormatItemForReport(buf, NULL, item, itemNode, 1);	
}

/**
 * xmlSchemaFormatFacetEnumSet:
 * @buf: the string buffer
 * @type: the type holding the enumeration facets
 *
 * Builds a string consisting of all enumeration elements.
 *
 * Returns a string of all enumeration elements.
 */
static const xmlChar *
xmlSchemaFormatFacetEnumSet(xmlChar **buf, xmlSchemaTypePtr type)
{
    xmlSchemaFacetLinkPtr link;

    if (*buf != NULL)
	xmlFree(*buf);    
    *buf = NULL;
    for (link = type->facetSet; link != NULL; link = link->next) {
	if (link->facet->type == XML_SCHEMA_FACET_ENUMERATION) {
	    if (*buf == NULL) {
		*buf = xmlStrdup(BAD_CAST "'");
		*buf = xmlStrcat(*buf, link->facet->value);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    } else {
		*buf = xmlStrcat(*buf, BAD_CAST ", '");
		*buf = xmlStrcat(*buf, link->facet->value);
		*buf = xmlStrcat(*buf, BAD_CAST "'");
	    }
	}
    }
    return ((const xmlChar *) *buf);
}

/**
 * xmlSchemaVFacetErr:
 * @ctxt:  the schema validation context
 * @error: the error code
 * @node: the node to be validated  
 * @value: the value of the node
 * @type: the type holding the facet
 * @facet: the facet
 * @message: the error message of NULL
 * @str1: extra data
 * @str2: extra data
 * @str3: extra data
 *
 * Reports a facet validation error.
 * TODO: Should this report the value of an element as well?
 */
static void
xmlSchemaVFacetErr(xmlSchemaValidCtxtPtr ctxt, 
		   xmlParserErrors error,
		   xmlNodePtr node,		   
		   const xmlChar *value,
		   unsigned long length,
		   xmlSchemaTypePtr type,
		   xmlSchemaFacetPtr facet,		   
		   const char *message,
		   const xmlChar *str1,
		   const xmlChar *str2,
		   const xmlChar *str3)
{
    xmlChar *str = NULL, *msg = NULL;
    xmlSchemaTypeType facetType;

    xmlSchemaFormatItemForReport(&msg, NULL, NULL, node, 0);
    msg = xmlStrcat(msg, BAD_CAST " [");
    msg = xmlStrcat(msg, xmlSchemaFormatItemForReport(&str, NULL, type, NULL, 0));
    msg = xmlStrcat(msg, BAD_CAST ", facet '");
    if (error == XML_SCHEMAV_CVC_ENUMERATION_VALID) {
	facetType = XML_SCHEMA_FACET_ENUMERATION;
	/*
	* If enumerations are validated, one must not expect the
	* facet to be given.
	*/	
    } else	
	facetType = facet->type;
    msg = xmlStrcat(msg, BAD_CAST xmlSchemaFacetTypeToString(facetType));
    msg = xmlStrcat(msg, BAD_CAST "']: ");
    if (message == NULL) {
	/*
	* Use a default message.
	*/
	if ((facetType == XML_SCHEMA_FACET_LENGTH) ||
	    (facetType == XML_SCHEMA_FACET_MINLENGTH) ||
	    (facetType == XML_SCHEMA_FACET_MAXLENGTH)) {

	    char len[25], actLen[25];

	    /* FIXME, TODO: What is the max expected string length of the
	    * this value?
	    */
	    if (node->type == XML_ATTRIBUTE_NODE)
		msg = xmlStrcat(msg, BAD_CAST "The value '%s' has a length of '%s'; ");
	    else
		msg = xmlStrcat(msg, BAD_CAST "The value has a length of '%s'; ");

	    snprintf(len, 24, "%lu", xmlSchemaGetFacetValueAsULong(facet));
	    snprintf(actLen, 24, "%lu", length);

	    if (facetType == XML_SCHEMA_FACET_LENGTH)
		msg = xmlStrcat(msg, 
		BAD_CAST "this differs from the allowed length of '%s'.\n");     
	    else if (facetType == XML_SCHEMA_FACET_MAXLENGTH)
		msg = xmlStrcat(msg, 
		BAD_CAST "this exceeds the allowed maximum length of '%s'.\n");
	    else if (facetType == XML_SCHEMA_FACET_MINLENGTH)
		msg = xmlStrcat(msg, 
		BAD_CAST "this underruns the allowed minimum length of '%s'.\n");
	    
	    if (node->type == XML_ATTRIBUTE_NODE)
		xmlSchemaVErrExt(ctxt, node, error,
		    (const char *) msg,
		    value, (const xmlChar *) actLen, (const xmlChar *) len,
		    NULL, NULL);
	    else 
		xmlSchemaVErr(ctxt, node, error,  
		    (const char *) msg,
		    (const xmlChar *) actLen, (const xmlChar *) len);
	
	} else if (facetType == XML_SCHEMA_FACET_ENUMERATION) {
	    msg = xmlStrcat(msg, BAD_CAST "The value '%s' is not an element "
		"of the set {%s}.\n");
	    xmlSchemaVErr(ctxt, node, error, (const char *) msg, value, 
		xmlSchemaFormatFacetEnumSet(&str, type));
	} else if (facetType == XML_SCHEMA_FACET_PATTERN) {
	    msg = xmlStrcat(msg, BAD_CAST "The value '%s' is not accepted "
		"by the pattern '%s'.\n");
	    xmlSchemaVErr(ctxt, node, error, (const char *) msg, value, 
		facet->value);	       
	} else if (node->type == XML_ATTRIBUTE_NODE) {		
	    msg = xmlStrcat(msg, BAD_CAST "The value '%s' is not facet-valid.\n");
	    xmlSchemaVErr(ctxt, node, error, (const char *) msg, value, NULL);
	} else {	    
	    msg = xmlStrcat(msg, BAD_CAST "The value is not facet-valid.\n");
	    xmlSchemaVErr(ctxt, node, error, (const char *) msg, NULL, NULL);
	}
    } else {
	msg = xmlStrcat(msg, (const xmlChar *) message);
	msg = xmlStrcat(msg, BAD_CAST ".\n");
	xmlSchemaVErr3(ctxt, node, error, (const char *) msg, str1, str2, str3);
    }        
    FREE_AND_NULL(str)
    xmlFree(msg);
}

/**
 * xmlSchemaVSimpleTypeErr:
 * @ctxt:  the schema validation context
 * @error: the error code
 * @type: the type used for validation
 * @node: the node containing the validated value
 * @value: the validated value
 *
 * Reports a simple type validation error.
 * TODO: Should this report the value of an element as well?
 */
static void
xmlSchemaVSimpleTypeErr(xmlSchemaValidCtxtPtr ctxt, 
			xmlParserErrors error,			
			xmlNodePtr node,
			const xmlChar *value,
			xmlSchemaTypePtr type)
{
    xmlChar *str = NULL, *msg = NULL;
    
    xmlSchemaFormatItemForReport(&msg, NULL,  NULL, node, 0);    
    msg = xmlStrcat(msg, BAD_CAST " [");
    msg = xmlStrcat(msg, xmlSchemaFormatItemForReport(&str, NULL, type, NULL, 0));
    if (node->type == XML_ATTRIBUTE_NODE) {
	msg = xmlStrcat(msg, BAD_CAST "]: The value '%s' is not valid.\n");
	xmlSchemaVErr(ctxt, node, error, (const char *) msg, value, NULL);
    } else {
	msg = xmlStrcat(msg, BAD_CAST "]: The character content is not valid.\n");
	xmlSchemaVErr(ctxt, node, error, (const char *) msg, NULL, NULL);
    }
    FREE_AND_NULL(str)	
    xmlFree(msg);
}

/**
 * xmlSchemaVComplexTypeErr:
 * @ctxt:  the schema validation context
 * @error: the error code
 * @node: the node containing the validated value
 * @type: the complex type used for validation
 * @message: the error message
 *
 * Reports a complex type validation error.
 */
static void
xmlSchemaVComplexTypeErr(xmlSchemaValidCtxtPtr ctxt, 
			xmlParserErrors error,
			xmlNodePtr node,
			xmlSchemaTypePtr type,			
			const char *message)
{
    xmlChar *str = NULL, *msg = NULL;
    
    xmlSchemaFormatItemForReport(&msg, NULL,  NULL, node, 0);
    if (type != NULL) {
	msg = xmlStrcat(msg, BAD_CAST " [");
	msg = xmlStrcat(msg, xmlSchemaFormatItemForReport(&str, NULL, type, NULL, 0));
	msg = xmlStrcat(msg, BAD_CAST "]");
    }
    msg = xmlStrcat(msg, BAD_CAST ": %s.\n");
    xmlSchemaVErr(ctxt, node, error, (const char *) msg, 
	(const xmlChar *) message, NULL);
    FREE_AND_NULL(str)	
    xmlFree(msg);
}

/**
 * xmlSchemaPMissingAttrErr:
 * @ctxt: the schema validation context
 * @ownerDes: the designation of  the owner
 * @ownerName: the name of the owner
 * @ownerItem: the owner as a schema object
 * @ownerElem: the owner as an element node
 * @node: the parent element node of the missing attribute node
 * @type: the corresponding type of the attribute node
 *
 * Reports an illegal attribute.
 */
static void
xmlSchemaPMissingAttrErr(xmlSchemaParserCtxtPtr ctxt,
			 xmlParserErrors error,			 
			 xmlChar **ownerDes,
			 xmlSchemaTypePtr ownerItem,
			 xmlNodePtr ownerElem,
			 const char *name,
			 const char *message)
{
    xmlChar *des = NULL;

    if (ownerDes == NULL)
	xmlSchemaFormatItemForReport(&des, NULL, ownerItem, ownerElem, 1);
    else if (*ownerDes == NULL) {
	xmlSchemaFormatItemForReport(ownerDes, NULL, ownerItem, ownerElem, 1);
	des = *ownerDes;
    } else 
	des = *ownerDes;      
    if (message != NULL)
	xmlSchemaPErr(ctxt, ownerElem, error, "%s: %s.\n", BAD_CAST des, BAD_CAST message);
    else	
