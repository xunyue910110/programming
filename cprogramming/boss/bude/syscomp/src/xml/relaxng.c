/*
 * relaxng.c : implementation of the Relax-NG handling and validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

/**
 * TODO:
 * - add support for DTD compatibility spec
 *   http://www.oasis-open.org/committees/relax-ng/compatibility-20011203.html
 * - report better mem allocations pbms at runtime and abort immediately.
 */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_SCHEMAS_ENABLED

#include <string.h>
#include <stdio.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/hash.h>
#include <libxml/uri.h>

#include <libxml/relaxng.h>

#include <libxml/xmlschemastypes.h>
#include <libxml/xmlautomata.h>
#include <libxml/xmlregexp.h>
#include <libxml/xmlschemastypes.h>

/*
 * The Relax-NG namespace1
 */
static const xmlChar *xmlRelaxNGNs = (const xmlChar *)
    "http://relaxng.org/ns/structure/1.0";

#define IS_RELAXNG(node, type)						\
   ((node != NULL) && (node->ns != NULL) &&				\
    (xmlStrEqual(node->name, (const xmlChar *) type)) &&		\
    (xmlStrEqual(node->ns->href, xmlRelaxNGNs)))


/* #define DEBUG 1 */

/* #define DEBUG_GRAMMAR 1 */

/* #define DEBUG_CONTENT 1 */

/* #define DEBUG_TYPE 1 */

/* #define DEBUG_VALID 1 */

/* #define DEBUG_INTERLEAVE 1 */

/* #define DEBUG_LIST 1 */

/* #define DEBUG_INCLUDE */

/* #define DEBUG_ERROR 1 */

/* #define DEBUG_COMPILE 1 */

/* #define DEBUG_PROGRESSIVE 1 */

#define MAX_ERROR 5

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

typedef struct _xmlRelaxNGSchema xmlRelaxNGSchema;
typedef xmlRelaxNGSchema *xmlRelaxNGSchemaPtr;

typedef struct _xmlRelaxNGDefine xmlRelaxNGDefine;
typedef xmlRelaxNGDefine *xmlRelaxNGDefinePtr;

typedef struct _xmlRelaxNGDocument xmlRelaxNGDocument;
typedef xmlRelaxNGDocument *xmlRelaxNGDocumentPtr;

typedef struct _xmlRelaxNGInclude xmlRelaxNGInclude;
typedef xmlRelaxNGInclude *xmlRelaxNGIncludePtr;

typedef enum {
    XML_RELAXNG_COMBINE_UNDEFINED = 0,  /* undefined */
    XML_RELAXNG_COMBINE_CHOICE, /* choice */
    XML_RELAXNG_COMBINE_INTERLEAVE      /* interleave */
} xmlRelaxNGCombine;

typedef enum {
    XML_RELAXNG_CONTENT_ERROR = -1,
    XML_RELAXNG_CONTENT_EMPTY = 0,
    XML_RELAXNG_CONTENT_SIMPLE,
    XML_RELAXNG_CONTENT_COMPLEX
} xmlRelaxNGContentType;

typedef struct _xmlRelaxNGGrammar xmlRelaxNGGrammar;
typedef xmlRelaxNGGrammar *xmlRelaxNGGrammarPtr;

struct _xmlRelaxNGGrammar {
    xmlRelaxNGGrammarPtr parent;        /* the parent grammar if any */
    xmlRelaxNGGrammarPtr children;      /* the children grammar if any */
    xmlRelaxNGGrammarPtr next;  /* the next grammar if any */
    xmlRelaxNGDefinePtr start;  /* <start> content */
    xmlRelaxNGCombine combine;  /* the default combine value */
    xmlRelaxNGDefinePtr startList;      /* list of <start> definitions */
    xmlHashTablePtr defs;       /* define* */
    xmlHashTablePtr refs;       /* references */
};


typedef enum {
    XML_RELAXNG_NOOP = -1,      /* a no operation from simplification  */
    XML_RELAXNG_EMPTY = 0,      /* an empty pattern */
    XML_RELAXNG_NOT_ALLOWED,    /* not allowed top */
    XML_RELAXNG_EXCEPT,         /* except present in nameclass defs */
    XML_RELAXNG_TEXT,           /* textual content */
    XML_RELAXNG_ELEMENT,        /* an element */
    XML_RELAXNG_DATATYPE,       /* extenal data type definition */
    XML_RELAXNG_PARAM,          /* extenal data type parameter */
    XML_RELAXNG_VALUE,          /* value from an extenal data type definition */
    XML_RELAXNG_LIST,           /* a list of patterns */
    XML_RELAXNG_ATTRIBUTE,      /* an attrbute following a pattern */
    XML_RELAXNG_DEF,            /* a definition */
    XML_RELAXNG_REF,            /* reference to a definition */
    XML_RELAXNG_EXTERNALREF,    /* reference to an external def */
    XML_RELAXNG_PARENTREF,      /* reference to a def in the parent grammar */
    XML_RELAXNG_OPTIONAL,       /* optional patterns */
    XML_RELAXNG_ZEROORMORE,     /* zero or more non empty patterns */
    XML_RELAXNG_ONEORMORE,      /* one or more non empty patterns */
    XML_RELAXNG_CHOICE,         /* a choice between non empty patterns */
    XML_RELAXNG_GROUP,          /* a pair/group of non empty patterns */
    XML_RELAXNG_INTERLEAVE,     /* interleaving choice of non-empty patterns */
    XML_RELAXNG_START           /* Used to keep track of starts on grammars */
} xmlRelaxNGType;

#define IS_NULLABLE		(1 << 0)
#define IS_NOT_NULLABLE		(1 << 1)
#define IS_INDETERMINIST	(1 << 2)
#define IS_MIXED		(1 << 3)
#define IS_TRIABLE		(1 << 4)
#define IS_PROCESSED		(1 << 5)
#define IS_COMPILABLE		(1 << 6)
#define IS_NOT_COMPILABLE	(1 << 7)

struct _xmlRelaxNGDefine {
    xmlRelaxNGType type;        /* the type of definition */
    xmlNodePtr node;            /* the node in the source */
    xmlChar *name;              /* the element local name if present */
    xmlChar *ns;                /* the namespace1 local name if present */
    xmlChar *value;             /* value when available */
    void *data;                 /* data lib or specific pointer */
    xmlRelaxNGDefinePtr content;        /* the expected content */
    xmlRelaxNGDefinePtr parent; /* the parent definition, if any */
    xmlRelaxNGDefinePtr next;   /* list within grouping sequences */
    xmlRelaxNGDefinePtr attrs;  /* list of attributes for elements */
    xmlRelaxNGDefinePtr nameClass;      /* the nameClass definition if any */
    xmlRelaxNGDefinePtr nextHash;       /* next define in defs/refs hash tables */
    short depth;                /* used for the cycle detection */
    short dflags;               /* define related flags */
    xmlRegexpPtr contModel;     /* a compiled content model if available */
};

/**
 * _xmlRelaxNG:
 *
 * A RelaxNGs definition
 */
struct _xmlRelaxNG {
    void *_private;             /* unused by the library for users or bindings */
    xmlRelaxNGGrammarPtr topgrammar;
    xmlDocPtr doc;

    int idref;                  /* requires idref checking */

    xmlHashTablePtr defs;       /* define */
    xmlHashTablePtr refs;       /* references */
    xmlRelaxNGDocumentPtr documents;    /* all the documents loaded */
    xmlRelaxNGIncludePtr includes;      /* all the includes loaded */
    int defNr;                  /* number of defines used */
    xmlRelaxNGDefinePtr *defTab;        /* pointer to the allocated definitions */

};

#define XML_RELAXNG_IN_ATTRIBUTE	(1 << 0)
#define XML_RELAXNG_IN_ONEORMORE	(1 << 1)
#define XML_RELAXNG_IN_LIST		(1 << 2)
#define XML_RELAXNG_IN_DATAEXCEPT	(1 << 3)
#define XML_RELAXNG_IN_START		(1 << 4)
#define XML_RELAXNG_IN_OOMGROUP		(1 << 5)
#define XML_RELAXNG_IN_OOMINTERLEAVE	(1 << 6)
#define XML_RELAXNG_IN_EXTERNALREF	(1 << 7)
#define XML_RELAXNG_IN_ANYEXCEPT	(1 << 8)
#define XML_RELAXNG_IN_NSEXCEPT		(1 << 9)

struct _xmlRelaxNGParserCtxt {
    void *userData;             /* user specific data block */
    xmlRelaxNGValidityErrorFunc error;  /* the callback in case of errors */
    xmlRelaxNGValidityWarningFunc warning;      /* the callback in case of warning */
    xmlStructuredErrorFunc serror;
    xmlRelaxNGValidErr err;

    xmlRelaxNGPtr schema;       /* The schema in use */
    xmlRelaxNGGrammarPtr grammar;       /* the current grammar */
    xmlRelaxNGGrammarPtr parentgrammar; /* the parent grammar */
    int flags;                  /* parser flags */
    int nbErrors;               /* number of errors at parse time */
    int nbWarnings;             /* number of warnings at parse time */
    const xmlChar *define;      /* the current define scope */
    xmlRelaxNGDefinePtr def;    /* the current define */

    int nbInterleaves;
    xmlHashTablePtr interleaves;        /* keep track of all the interleaves */

    xmlRelaxNGDocumentPtr documents;    /* all the documents loaded */
    xmlRelaxNGIncludePtr includes;      /* all the includes loaded */
    xmlChar *URL;
    xmlDocPtr document;

    int defNr;                  /* number of defines used */
    int defMax;                 /* number of defines aloocated */
    xmlRelaxNGDefinePtr *defTab;        /* pointer to the allocated definitions */

    const char *buffer;
    int size;

    /* the document stack */
    xmlRelaxNGDocumentPtr doc;  /* Current parsed external ref */
    int docNr;                  /* Depth of the parsing stack */
    int docMax;                 /* Max depth of the parsing stack */
    xmlRelaxNGDocumentPtr *docTab;      /* array of docs */

    /* the include stack */
    xmlRelaxNGIncludePtr inc;   /* Current parsed include */
    int incNr;                  /* Depth of the include parsing stack */
    int incMax;                 /* Max depth of the parsing stack */
    xmlRelaxNGIncludePtr *incTab;       /* array of incs */

    int idref;                  /* requires idref checking */

    /* used to compile content models */
    xmlAutomataPtr am;          /* the automata */
    xmlAutomataStatePtr state;  /* used to build the automata */

    int crng;			/* compact syntax and other flags */
    int freedoc;		/* need to free the document */
};

#define FLAGS_IGNORABLE		1
#define FLAGS_NEGATIVE		2
#define FLAGS_MIXED_CONTENT	4

/**
 * xmlRelaxNGInterleaveGroup:
 *
 * A RelaxNGs partition set associated to lists of definitions
 */
typedef struct _xmlRelaxNGInterleaveGroup xmlRelaxNGInterleaveGroup;
typedef xmlRelaxNGInterleaveGroup *xmlRelaxNGInterleaveGroupPtr;
struct _xmlRelaxNGInterleaveGroup {
    xmlRelaxNGDefinePtr rule;   /* the rule to satisfy */
    xmlRelaxNGDefinePtr *defs;  /* the array of element definitions */
    xmlRelaxNGDefinePtr *attrs; /* the array of attributes definitions */
};

#define IS_DETERMINIST		1
#define IS_NEEDCHECK		2

/**
 * xmlRelaxNGPartitions:
 *
 * A RelaxNGs partition associated to an interleave group
 */
typedef struct _xmlRelaxNGPartition xmlRelaxNGPartition;
typedef xmlRelaxNGPartition *xmlRelaxNGPartitionPtr;
struct _xmlRelaxNGPartition {
    int nbgroups;               /* number of groups in the partitions */
    xmlHashTablePtr triage;     /* hash table used to direct nodes to the
                                 * right group when possible */
    int flags;                  /* determinist ? */
    xmlRelaxNGInterleaveGroupPtr *groups;
};

/**
 * xmlRelaxNGValidState:
 *
 * A RelaxNGs validation state
 */
#define MAX_ATTR 20
typedef struct _xmlRelaxNGValidState xmlRelaxNGValidState;
typedef xmlRelaxNGValidState *xmlRelaxNGValidStatePtr;
struct _xmlRelaxNGValidState {
    xmlNodePtr node;            /* the current node */
    xmlNodePtr seq;             /* the sequence of children left to validate */
    int nbAttrs;                /* the number of attributes */
    int maxAttrs;               /* the size of attrs */
    int nbAttrLeft;             /* the number of attributes left to validate */
    xmlChar *value;             /* the value when operating on string */
    xmlChar *endvalue;          /* the end value when operating on string */
    xmlAttrPtr *attrs;          /* the array of attributes */
};

/**
 * xmlRelaxNGStates:
 *
 * A RelaxNGs container for validation state
 */
typedef struct _xmlRelaxNGStates xmlRelaxNGStates;
typedef xmlRelaxNGStates *xmlRelaxNGStatesPtr;
struct _xmlRelaxNGStates {
    int nbState;                /* the number of states */
    int maxState;               /* the size of the array */
    xmlRelaxNGValidStatePtr *tabState;
};

#define ERROR_IS_DUP	1

/**
 * xmlRelaxNGValidError:
 *
 * A RelaxNGs validation error
 */
typedef struct _xmlRelaxNGValidError xmlRelaxNGValidError;
typedef xmlRelaxNGValidError *xmlRelaxNGValidErrorPtr;
struct _xmlRelaxNGValidError {
    xmlRelaxNGValidErr err;     /* the error number */
    int flags;                  /* flags */
    xmlNodePtr node;            /* the current node */
    xmlNodePtr seq;             /* the current child */
    const xmlChar *arg1;        /* first arg */
    const xmlChar *arg2;        /* second arg */
};

/**
 * xmlRelaxNGValidCtxt:
 *
 * A RelaxNGs validation context
 */

struct _xmlRelaxNGValidCtxt {
    void *userData;             /* user specific data block */
    xmlRelaxNGValidityErrorFunc error;  /* the callback in case of errors */
    xmlRelaxNGValidityWarningFunc warning;      /* the callback in case of warning */
    xmlStructuredErrorFunc serror;
    int nbErrors;               /* number of errors in validation */

    xmlRelaxNGPtr schema;       /* The schema in use */
    xmlDocPtr doc;              /* the document being validated */
    int flags;                  /* validation flags */
    int depth;                  /* validation depth */
    int idref;                  /* requires idref checking */
    int errNo;                  /* the first error found */

    /*
     * Errors accumulated in branches may have to be stacked to be
     * provided back when it's sure they affect validation.
     */
    xmlRelaxNGValidErrorPtr err;        /* Last error */
    int errNr;                  /* Depth of the error stack */
    int errMax;                 /* Max depth of the error stack */
    xmlRelaxNGValidErrorPtr errTab;     /* stack of errors */

    xmlRelaxNGValidStatePtr state;      /* the current validation state */
    xmlRelaxNGStatesPtr states; /* the accumulated state list */

    xmlRelaxNGStatesPtr freeState;      /* the pool of free valid states */
    int freeStatesNr;
    int freeStatesMax;
    xmlRelaxNGStatesPtr *freeStates;    /* the pool of free state groups */

    /*
     * This is used for "progressive" validation
     */
    xmlRegExecCtxtPtr elem;     /* the current element regexp */
    int elemNr;                 /* the number of element validated */
    int elemMax;                /* the max depth of elements */
    xmlRegExecCtxtPtr *elemTab; /* the stack of regexp runtime */
    int pstate;                 /* progressive state */
    xmlNodePtr pnode;           /* the current node */
    xmlRelaxNGDefinePtr pdef;   /* the non-streamable definition */
    int perr;                   /* signal error in content model
                                 * outside the regexp */
};

/**
 * xmlRelaxNGInclude:
 *
 * Structure associated to a RelaxNGs document element
 */
struct _xmlRelaxNGInclude {
    xmlRelaxNGIncludePtr next;  /* keep a chain of includes */
    xmlChar *href;              /* the normalized href value */
    xmlDocPtr doc;              /* the associated XML document */
    xmlRelaxNGDefinePtr content;        /* the definitions */
    xmlRelaxNGPtr schema;       /* the schema */
};

/**
 * xmlRelaxNGDocument:
 *
 * Structure associated to a RelaxNGs document element
 */
struct _xmlRelaxNGDocument {
    xmlRelaxNGDocumentPtr next; /* keep a chain of documents */
    xmlChar *href;              /* the normalized href value */
    xmlDocPtr doc;              /* the associated XML document */
    xmlRelaxNGDefinePtr content;        /* the definitions */
    xmlRelaxNGPtr schema;       /* the schema */
};


/************************************************************************
 *									*
 * 		Some factorized error routines				*
 *									*
 ************************************************************************/

/**
 * xmlRngPErrMemory:
 * @ctxt:  an Relax-NG parser context
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
xmlRngPErrMemory(xmlRelaxNGParserCtxtPtr ctxt, const char *extra)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
        ctxt->nbErrors++;
        schannel = ctxt->serror;
    }
    if (extra)
        __xmlRaiseError(schannel, channel, data,
                        NULL, NULL, XML_FROM_RELAXNGP,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
                        NULL, NULL, 0, 0,
                        "Memory allocation failed : %s\n", extra);
    else
        __xmlRaiseError(schannel, channel, data,
                        NULL, NULL, XML_FROM_RELAXNGP,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, NULL,
                        NULL, NULL, 0, 0, "Memory allocation failed\n");
}

/**
 * xmlRngVErrMemory:
 * @ctxt:  a Relax-NG validation context
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
xmlRngVErrMemory(xmlRelaxNGValidCtxtPtr ctxt, const char *extra)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
        ctxt->nbErrors++;
        schannel = ctxt->serror;
    }
    if (extra)
        __xmlRaiseError(schannel, channel, data,
                        NULL, NULL, XML_FROM_RELAXNGV,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
                        NULL, NULL, 0, 0,
                        "Memory allocation failed : %s\n", extra);
    else
        __xmlRaiseError(schannel, channel, data,
                        NULL, NULL, XML_FROM_RELAXNGV,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, NULL,
                        NULL, NULL, 0, 0, "Memory allocation failed\n");
}

/**
 * xmlRngPErr:
 * @ctxt:  a Relax-NG parser context
 * @node:  the node raising the error
 * @error:  the error code
 * @msg:  message
 * @str1:  extra info
 * @str2:  extra info
 *
 * Handle a Relax NG Parsing error
 */
static void
xmlRngPErr(xmlRelaxNGParserCtxtPtr ctxt, xmlNodePtr node, int error,
           const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
        ctxt->nbErrors++;
        schannel = ctxt->serror;
    }
    __xmlRaiseError(schannel, channel, data,
                    NULL, node, XML_FROM_RELAXNGP,
                    error, XML_ERR_ERROR, NULL, 0,
                    (const char *) str1, (const char *) str2, NULL, 0, 0,
                    msg, str1, str2);
}

/**
 * xmlRngVErr:
 * @ctxt:  a Relax-NG validation context
 * @node:  the node raising the error
 * @error:  the error code
 * @msg:  message
 * @str1:  extra info
 * @str2:  extra info
 *
 * Handle a Relax NG Validation error
 */
static void
xmlRngVErr(xmlRelaxNGValidCtxtPtr ctxt, xmlNodePtr node, int error,
           const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;

    if (ctxt != NULL) {
        channel = ctxt->error;
        data = ctxt->userData;
        ctxt->nbErrors++;
        schannel = ctxt->serror;
    }
    __xmlRaiseError(schannel, channel, data,
                    NULL, node, XML_FROM_RELAXNGV,
                    error, XML_ERR_ERROR, NULL, 0,
                    (const char *) str1, (const char *) str2, NULL, 0, 0,
                    msg, str1, str2);
}

/************************************************************************
 * 									*
 * 		Preliminary type checking interfaces			*
 * 									*
 ************************************************************************/

/**
 * xmlRelaxNGTypeHave:
 * @data:  data needed for the library
 * @type:  the type name
 * @value:  the value to check
 *
 * Function provided by a type library to check if a type is exported
 *
 * Returns 1 if yes, 0 if no and -1 in case of error.
 */
typedef int (*xmlRelaxNGTypeHave) (void *data, const xmlChar * type);

/**
 * xmlRelaxNGTypeCheck:
 * @data:  data needed for the library
 * @type:  the type name
 * @value:  the value to check
 * @result:  place to store the result if needed
 *
 * Function provided by a type library to check if a value match a type
 *
 * Returns 1 if yes, 0 if no and -1 in case of error.
 */
typedef int (*xmlRelaxNGTypeCheck) (void *data, const xmlChar * type,
                                    const xmlChar * value, void **result,
                                    xmlNodePtr node);

/**
 * xmlRelaxNGFacetCheck:
 * @data:  data needed for the library
 * @type:  the type name
 * @facet:  the facet name
 * @val:  the facet value
 * @strval:  the string value
 * @value:  the value to check
 *
 * Function provided by a type library to check a value facet
 *
 * Returns 1 if yes, 0 if no and -1 in case of error.
 */
typedef int (*xmlRelaxNGFacetCheck) (void *data, const xmlChar * type,
                                     const xmlChar * facet,
                                     const xmlChar * val,
                                     const xmlChar * strval, void *value);

/**
 * xmlRelaxNGTypeFree:
 * @data:  data needed for the library
 * @result:  the value to free
 *
 * Function provided by a type library to free a returned result
 */
typedef void (*xmlRelaxNGTypeFree) (void *data, void *result);

/**
 * xmlRelaxNGTypeCompare:
 * @data:  data needed for the library
 * @type:  the type name
 * @value1:  the first value
 * @value2:  the second value
 *
 * Function provided by a type library to compare two values accordingly
 * to a type.
 *
 * Returns 1 if yes, 0 if no and -1 in case of error.
 */
typedef int (*xmlRelaxNGTypeCompare) (void *data, const xmlChar * type,
                                      const xmlChar * value1,
                                      xmlNodePtr ctxt1,
                                      void *comp1,
                                      const xmlChar * value2,
                                      xmlNodePtr ctxt2);
typedef struct _xmlRelaxNGTypeLibrary xmlRelaxNGTypeLibrary;
typedef xmlRelaxNGTypeLibrary *xmlRelaxNGTypeLibraryPtr;
struct _xmlRelaxNGTypeLibrary {
    const xmlChar *namespace1;   /* the datatypeLibrary value */
    void *data;                 /* data needed for the library */
    xmlRelaxNGTypeHave have;    /* the export function */
    xmlRelaxNGTypeCheck check;  /* the checking function */
    xmlRelaxNGTypeCompare comp; /* the compare function */
    xmlRelaxNGFacetCheck facet; /* the facet check function */
    xmlRelaxNGTypeFree freef;   /* the freeing function */
};

/************************************************************************
 * 									*
 * 			Allocation functions				*
 * 									*
 ************************************************************************/
static void xmlRelaxNGFreeGrammar(xmlRelaxNGGrammarPtr grammar);
static void xmlRelaxNGFreeDefine(xmlRelaxNGDefinePtr define);
static void xmlRelaxNGNormExtSpace(xmlChar * value);
static void xmlRelaxNGFreeInnerSchema(xmlRelaxNGPtr schema);
static int xmlRelaxNGEqualValidState(xmlRelaxNGValidCtxtPtr ctxt
                                     ATTRIBUTE_UNUSED,
                                     xmlRelaxNGValidStatePtr state1,
                                     xmlRelaxNGValidStatePtr state2);
static void xmlRelaxNGFreeValidState(xmlRelaxNGValidCtxtPtr ctxt,
                                     xmlRelaxNGValidStatePtr state);

/**
 * xmlRelaxNGFreeDocument:
 * @docu:  a document structure
 *
 * Deallocate a RelaxNG document structure.
 */
static void
xmlRelaxNGFreeDocument(xmlRelaxNGDocumentPtr docu)
{
    if (docu == NULL)
        return;

    if (docu->href != NULL)
        xmlFree(docu->href);
    if (docu->doc != NULL)
        xmlFreeDoc(docu->doc);
    if (docu->schema != NULL)
        xmlRelaxNGFreeInnerSchema(docu->schema);
    xmlFree(docu);
}

/**
 * xmlRelaxNGFreeDocumentList:
 * @docu:  a list of  document structure
 *
 * Deallocate a RelaxNG document structures.
 */
static void
xmlRelaxNGFreeDocumentList(xmlRelaxNGDocumentPtr docu)
{
    xmlRelaxNGDocumentPtr next;

    while (docu != NULL) {
        next = docu->next;
        xmlRelaxNGFreeDocument(docu);
        docu = next;
    }
}

/**
 * xmlRelaxNGFreeInclude:
 * @incl:  a include structure
 *
 * Deallocate a RelaxNG include structure.
 */
static void
xmlRelaxNGFreeInclude(xmlRelaxNGIncludePtr incl)
{
    if (incl == NULL)
        return;

    if (incl->href != NULL)
        xmlFree(incl->href);
    if (incl->doc != NULL)
        xmlFreeDoc(incl->doc);
    if (incl->schema != NULL)
        xmlRelaxNGFree(incl->schema);
    xmlFree(incl);
}

/**
 * xmlRelaxNGFreeIncludeList:
 * @incl:  a include structure list
 *
 * Deallocate a RelaxNG include structure.
 */
static void
xmlRelaxNGFreeIncludeList(xmlRelaxNGIncludePtr incl)
{
    xmlRelaxNGIncludePtr next;

    while (incl != NULL) {
        next = incl->next;
        xmlRelaxNGFreeInclude(incl);
        incl = next;
    }
}

/**
 * xmlRelaxNGNewRelaxNG:
 * @ctxt:  a Relax-NG validation context (optional)
 *
 * Allocate a new RelaxNG structure.
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlRelaxNGPtr
xmlRelaxNGNewRelaxNG(xmlRelaxNGParserCtxtPtr ctxt)
{
    xmlRelaxNGPtr ret;

    ret = (xmlRelaxNGPtr) xmlMalloc(sizeof(xmlRelaxNG));
    if (ret == NULL) {
        xmlRngPErrMemory(ctxt, NULL);
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlRelaxNG));

    return (ret);
}

/**
 * xmlRelaxNGFreeInnerSchema:
 * @schema:  a schema structure
 *
 * Deallocate a RelaxNG schema structure.
 */
static void
xmlRelaxNGFreeInnerSchema(xmlRelaxNGPtr schema)
{
    if (schema == NULL)
        return;

    if (schema->doc != NULL)
        xmlFreeDoc(schema->doc);
    if (schema->defTab != NULL) {
        int i;

        for (i = 0; i < schema->defNr; i++)
            xmlRelaxNGFreeDefine(schema->defTab[i]);
        xmlFree(schema->defTab);
    }

    xmlFree(schema);
}

/**
 * xmlRelaxNGFree:
 * @schema:  a schema structure
 *
 * Deallocate a RelaxNG structure.
 */
void
xmlRelaxNGFree(xmlRelaxNGPtr schema)
{
    if (schema == NULL)
        return;

    if (schema->topgrammar != NULL)
        xmlRelaxNGFreeGrammar(schema->topgrammar);
    if (schema->doc != NULL)
        xmlFreeDoc(schema->doc);
    if (schema->documents != NULL)
        xmlRelaxNGFreeDocumentList(schema->documents);
    if (schema->includes != NULL)
        xmlRelaxNGFreeIncludeList(schema->includes);
    if (schema->defTab != NULL) {
        int i;

        for (i = 0; i < schema->defNr; i++)
            xmlRelaxNGFreeDefine(schema->defTab[i]);
        xmlFree(schema->defTab);
    }

    xmlFree(schema);
}

/**
 * xmlRelaxNGNewGrammar:
 * @ctxt:  a Relax-NG validation context (optional)
 *
 * Allocate a new RelaxNG grammar.
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlRelaxNGGrammarPtr
xmlRelaxNGNewGrammar(xmlRelaxNGParserCtxtPtr ctxt)
{
    xmlRelaxNGGrammarPtr ret;

    ret = (xmlRelaxNGGrammarPtr) xmlMalloc(sizeof(xmlRelaxNGGrammar));
    if (ret == NULL) {
        xmlRngPErrMemory(ctxt, NULL);
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlRelaxNGGrammar));

    return (ret);
}

/**
 * xmlRelaxNGFreeGrammar:
 * @grammar:  a grammar structure
 *
 * Deallocate a RelaxNG grammar structure.
 */
static void
xmlRelaxNGFreeGrammar(xmlRelaxNGGrammarPtr grammar)
{
    if (grammar == NULL)
        return;

    if (grammar->children != NULL) {
        xmlRelaxNGFreeGrammar(grammar->children);
    }
    if (grammar->next != NULL) {
        xmlRelaxNGFreeGrammar(grammar->next);
    }
    if (grammar->refs != NULL) {
        xmlHashFree(grammar->refs, NULL);
    }
    if (grammar->defs != NULL) {
        xmlHashFree(grammar->defs, NULL);
    }

    xmlFree(grammar);
}

/**
 * xmlRelaxNGNewDefine:
 * @ctxt:  a Relax-NG validation context
 * @node:  the node in the input document.
 *
 * Allocate a new RelaxNG define.
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlRelaxNGDefinePtr
xmlRelaxNGNewDefine(xmlRelaxNGParserCtxtPtr ctxt, xmlNodePtr node)
{
    xmlRelaxNGDefinePtr ret;

    if (ctxt->defMax == 0) {
        ctxt->defMax = 16;
        ctxt->defNr = 0;
        ctxt->defTab = (xmlRelaxNGDefinePtr *)
            xmlMalloc(ctxt->defMax * sizeof(xmlRelaxNGDefinePtr));
        if (ctxt->defTab == NULL) {
            xmlRngPErrMemory(ctxt, "allocating define\n");
            return (NULL);
        }
    } else if (ctxt->defMax <= ctxt->defNr) {
        xmlRelaxNGDefinePtr *tmp;

        ctxt->defMax *= 2;
        tmp = (xmlRelaxNGDefinePtr *) xmlRealloc(ctxt->defTab,
                                                 ctxt->defMax *
                                                 sizeof
                                                 (xmlRelaxNGDefinePtr));
        if (tmp == NULL) {
            xmlRngPErrMemory(ctxt, "allocating define\n");
            return (NULL);
        }
        ctxt->defTab = tmp;
    }
    ret = (xmlRelaxNGDefinePtr) xmlMalloc(sizeof(xmlRelaxNGDefine));
    if (ret == NULL) {
        xmlRngPErrMemory(ctxt, "allocating define\n");
        return (NULL);
    }
    memset(ret, 0, sizeof(xmlRelaxNGDefine));
    ctxt->defTab[ctxt->defNr++] = ret;
    ret->node = node;
    ret->depth = -1;
    return (ret);
}

/**
 * xmlRelaxNGFreePartition:
 * @partitions:  a partition set structure
 *
 * Deallocate RelaxNG partition set structures.
 */
static void
xmlRelaxNGFreePartition(xmlRelaxNGPartitionPtr partitions)
{
    xmlRelaxNGInterleaveGroupPtr group;
    int j;

    if (partitions != NULL) {
        if (partitions->groups != NULL) {
            for (j = 0; j < partitions->nbgroups; j++) {
                group = partitions->groups[j];
                if (group != NULL) {
                    if (group->defs != NULL)
                        xmlFree(group->defs);
                    if (group->attrs != NULL)
                        xmlFree(group->attrs);
                    xmlFree(group);
                }
            }
            xmlFree(partitions->groups);
        }
        if (partitions->triage != NULL) {
            xmlHashFree(partitions->triage, NULL);
        }
        xmlFree(partitions);
    }
}

/**
 * xmlRelaxNGFreeDefine:
 * @define:  a define structure
 *
 * Deallocate a RelaxNG define structure.
 */
static void
xmlRelaxNGFreeDefine(xmlRelaxNGDefinePtr define)
{
    if (define == NULL)
        return;

    if ((define->type == XML_RELAXNG_VALUE) && (define->attrs != NULL)) {
        xmlRelaxNGTypeLibraryPtr lib;

        lib = (xmlRelaxNGTypeLibraryPtr) define->data;
        if ((lib != NULL) && (lib->freef != NULL))
            lib->freef(lib->data, (void *) define->attrs);
    }
    if ((define->data != NULL) && (define->type == XML_RELAXNG_INTERLEAVE))
        xmlRelaxNGFreePartition((xmlRelaxNGPartitionPtr) define->data);
    if ((define->data != NULL) && (define->type == XML_RELAXNG_CHOICE))
        xmlHashFree((xmlHashTablePtr) define->data, NULL);
    if (define->name != NULL)
        xmlFree(define->name);
    if (define->ns != NULL)
        xmlFree(define->ns);
    if (define->value != NULL)
        xmlFree(define->value);
    if (define->contModel != NULL)
        xmlRegFreeRegexp(define->contModel);
    xmlFree(define);
}

/**
 * xmlRelaxNGNewStates:
 * @ctxt:  a Relax-NG validation context
 * @size:  the default size for the container
 *
 * Allocate a new RelaxNG validation state container
 *
 * Returns the newly allocated structure or NULL in case or error
 */
static xmlRelaxNGStatesPtr
xmlRelaxNGNewStates(xmlRelaxNGValidCtxtPtr ctxt, int size)
{
    xmlRelaxNGStatesPtr ret;

    if ((ctxt != NULL) &&
        (ctxt->freeState != NULL) && (ctxt->freeStatesNr > 0)) {
        ctxt->freeStatesNr--;
        ret = ctxt->freeStates[ctxt->freeStatesNr];
        ret->nbState = 0;
        return (ret);
    }
    if (size < 16)
        size = 16;

    ret = (xmlRelaxNGStatesPtr) xmlMalloc(sizeof(xmlRelaxNGStates) +
                                          (size -
                                           1) *
                                          sizeof(xmlRelaxNGValidStatePtr));
    if (ret == NULL) {
        xmlRngVErrMemory(ctxt, "allocating states\n");
        return (NULL);
    }
    ret->nbState = 0;
    ret->maxState = size;
    ret->tabState = (xmlRelaxNGValidStatePtr *) xmlMalloc((size) *
                                                          sizeof
                                                          (xmlRelaxNGValidStatePtr));
    if (ret->tabState == NULL) {
        xmlRngVErrMemory(ctxt, "allocating states\n");
        xmlFree(ret);
        return (NULL);
    }
    return (ret);
}

/**
 * xmlRelaxNGAddStateUniq:
 * @ctxt:  a Relax-NG validation context
 * @states:  the states container
 * @state:  the validation state
 *
 * Add a RelaxNG validation state to the container without checking
 * for unicity.
 *
 * Return 1 in case of success and 0 if this is a duplicate and -1 on error
 */
static int
x