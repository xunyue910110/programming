/**
 * catalog.c: set of generic Catalog related routines 
 *
 * Reference:  SGML Open Technical Resolution TR9401:1997.
 *             http://www.jclark.com/sp/catalog.htm
 *
 *             XML Catalogs Working Draft 06 August 2001
 *             http://www.oasis-open.org/committees/entity/spec-2001-08-06.html
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@imag.fr
 */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_CATALOG_ENABLED
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/hash.h>
#include <libxml/uri.h>
#include <libxml/parserInternals.h>
#include <libxml/catalog.h>
#include <libxml/xmlerror.h>
#include <libxml/threads.h>
#include <libxml/globals.h>

#define MAX_DELEGATE	50
#define MAX_CATAL_DEPTH	50

/**
 * TODO:
 *
 * macro to flag unimplemented blocks
 * XML_CATALOG_PREFER user env to select between system/public prefered
 * option. C.f. Richard Tobin <richard@cogsci.ed.ac.uk>
 *> Just FYI, I am using an environment variable XML_CATALOG_PREFER with
 *> values "system" and "public".  I have made the default be "system" to
 *> match yours.
 */
#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define XML_URN_PUBID "urn:publicid:"
#define XML_CATAL_BREAK ((xmlChar *) -1)
#ifndef XML_XML_DEFAULT_CATALOG
#define XML_XML_DEFAULT_CATALOG "file:///etc/xml/catalog"
#endif
#ifndef XML_SGML_DEFAULT_CATALOG
#define XML_SGML_DEFAULT_CATALOG "file:///etc/sgml/catalog"
#endif

#if defined(_WIN32) && defined(_MSC_VER)
#undef XML_XML_DEFAULT_CATALOG
static char XML_XML_DEFAULT_CATALOG[256] = "file:///etc/xml/catalog";
void* __stdcall GetModuleHandleA(const char*);
unsigned long __stdcall GetModuleFileNameA(void*, char*, unsigned long);
#endif

static xmlChar *xmlCatalogNormalizePublic(const xmlChar *pubID);
static int xmlExpandCatalog(xmlCatalogPtr catal, const char *filename);

/************************************************************************
 *									*
 *			Types, all private				*
 *									*
 ************************************************************************/

typedef enum {
    XML_CATA_REMOVED = -1,
    XML_CATA_NONE = 0,
    XML_CATA_CATALOG,
    XML_CATA_BROKEN_CATALOG,
    XML_CATA_NEXT_CATALOG,
    XML_CATA_GROUP,
    XML_CATA_PUBLIC,
    XML_CATA_SYSTEM,
    XML_CATA_REWRITE_SYSTEM,
    XML_CATA_DELEGATE_PUBLIC,
    XML_CATA_DELEGATE_SYSTEM,
    XML_CATA_URI,
    XML_CATA_REWRITE_URI,
    XML_CATA_DELEGATE_URI,
    SGML_CATA_SYSTEM,
    SGML_CATA_PUBLIC,
    SGML_CATA_ENTITY,
    SGML_CATA_PENTITY,
    SGML_CATA_DOCTYPE,
    SGML_CATA_LINKTYPE,
    SGML_CATA_NOTATION,
    SGML_CATA_DELEGATE,
    SGML_CATA_BASE,
    SGML_CATA_CATALOG,
    SGML_CATA_DOCUMENT,
    SGML_CATA_SGMLDECL
} xmlCatalogEntryType;

typedef struct _xmlCatalogEntry xmlCatalogEntry;
typedef xmlCatalogEntry *xmlCatalogEntryPtr;
struct _xmlCatalogEntry {
    struct _xmlCatalogEntry *next;
    struct _xmlCatalogEntry *parent;
    struct _xmlCatalogEntry *children;
    xmlCatalogEntryType type;
    xmlChar *name;
    xmlChar *value;
    xmlChar *URL;  /* The expanded URL using the base */
    xmlCatalogPrefer prefer;
    int dealloc;
    int depth;
    struct _xmlCatalogEntry *group;
};

typedef enum {
    XML_XML_CATALOG_TYPE = 1,
    XML_SGML_CATALOG_TYPE
} xmlCatalogType;

#define XML_MAX_SGML_CATA_DEPTH 10
struct _xmlCatalog {
    xmlCatalogType type;	/* either XML or SGML */

    /*
     * SGML Catalogs are stored as a simple hash table of catalog entries
     * Catalog stack to check against overflows when building the
     * SGML catalog
     */
    char *catalTab[XML_MAX_SGML_CATA_DEPTH];	/* stack of catals */
    int          catalNr;	/* Number of current catal streams */
    int          catalMax;	/* Max number of catal streams */
    xmlHashTablePtr sgml;

    /*
     * XML Catalogs are stored as a tree of Catalog entries
     */
    xmlCatalogPrefer prefer;
    xmlCatalogEntryPtr xml;
};

/************************************************************************
 *									*
 *			Global variables				*
 *									*
 ************************************************************************/

/*
 * Those are preferences
 */
static int xmlDebugCatalogs = 0;   /* used for debugging */
static xmlCatalogAllow xmlCatalogDefaultAllow = XML_CATA_ALLOW_ALL;
static xmlCatalogPrefer xmlCatalogDefaultPrefer = XML_CATA_PREFER_PUBLIC;

/*
 * Hash table containing all the trees of XML catalogs parsed by
 * the application.
 */
static xmlHashTablePtr xmlCatalogXMLFiles = NULL;

/*
 * The default catalog in use by the application
 */
static xmlCatalogPtr xmlDefaultCatalog = NULL;

/*
 * A mutex for modifying the shared global catalog(s)
 * xmlDefaultCatalog tree.
 * It also protects xmlCatalogXMLFiles
 * The core of this readers/writer scheme is in xmlFetchXMLCatalogFile()
 */
static xmlRMutexPtr xmlCatalogMutex = NULL;

/*
 * Whether the catalog support was initialized.
 */
static int xmlCatalogInitialized = 0;

/************************************************************************
 *									*
 * 			Catalog error handlers				*
 *									*
 ************************************************************************/

/**
 * xmlCatalogErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlCatalogErrMemory(const char *extra)
{
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_CATALOG,
                    XML_ERR_NO_MEMORY, XML_ERR_ERROR, NULL, 0,
		    extra, NULL, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}

/**
 * xmlCatalogErr:
 * @catal: the Catalog entry
 * @node: the context node
 * @msg:  the error message
 * @extra:  extra informations
 *
 * Handle a catalog error
 */
static void
xmlCatalogErr(xmlCatalogEntryPtr catal, xmlNodePtr node, int error,
               const char *msg, const xmlChar *str1, const xmlChar *str2,
	       const xmlChar *str3)
{
    __xmlRaiseError(NULL, NULL, NULL, catal, node, XML_FROM_CATALOG,
                    error, XML_ERR_ERROR, NULL, 0,
		    (const char *) str1, (const char *) str2,
		    (const char *) str3, 0, 0,
		    msg, str1, str2, str3);
}


/************************************************************************
 *									*
 *			Allocation and Freeing				*
 *									*
 ************************************************************************/

/**
 * xmlNewCatalogEntry:
 * @type:  type of entry
 * @name:  name of the entry
 * @value:  value of the entry
 * @prefer:  the PUBLIC vs. SYSTEM current preference value
 * @group:  for members of a group, the group entry 
 *
 * create a new Catalog entry, this type is shared both by XML and 
 * SGML catalogs, but the acceptable types values differs.
 *
 * Returns the xmlCatalogEntryPtr or NULL in case of error
 */
static xmlCatalogEntryPtr
xmlNewCatalogEntry(xmlCatalogEntryType type, const xmlChar *name,
	   const xmlChar *value, const xmlChar *URL, xmlCatalogPrefer prefer,
	   xmlCatalogEntryPtr group) {
    xmlCatalogEntryPtr ret;
    xmlChar *normid = NULL;

    ret = (xmlCatalogEntryPtr) xmlMalloc(sizeof(xmlCatalogEntry));
    if (ret == NULL) {
        xmlCatalogErrMemory("allocating catalog entry");
	return(NULL);
    }
    ret->next = NULL;
    ret->parent = NULL;
    ret->children = NULL;
    ret->type = type;
    if (type == XML_CATA_PUBLIC || type == XML_CATA_DELEGATE_PUBLIC) {
        normid = xmlCatalogNormalizePublic(name);
        if (normid != NULL)
            name = (*normid != 0 ? normid : NULL);
    }
    if (name != NULL)
	ret->name = xmlStrdup(name);
    else
	ret->name = NULL;
    if (normid != NULL)
        xmlFree(normid);
    if (value != NULL)
	ret->value = xmlStrdup(value);
    else
	ret->value = NULL;
    if (URL == NULL)
	URL = value;
    if (URL != NULL)
	ret->URL = xmlStrdup(URL);
    else
	ret->URL = NULL;
    ret->prefer = prefer;
    ret->dealloc = 0;
    ret->depth = 0;
    ret->group = group;
    return(ret);
}

static void
xmlFreeCatalogEntryList(xmlCatalogEntryPtr ret);

/**
 * xmlFreeCatalogEntry:
 * @ret:  a Catalog entry
 *
 * Free the memory allocated to a Catalog entry
 */
static void
xmlFreeCatalogEntry(xmlCatalogEntryPtr ret) {
    if (ret == NULL)
	return;
    /*
     * Entries stored in the file hash must be deallocated
     * only by the file hash cleaner !
     */
    if (ret->dealloc == 1)
	return;

    if (xmlDebugCatalogs) {
	if (ret->name != NULL)
	    xmlGenericError(xmlGenericErrorContext,
		    "Free catalog entry %s\n", ret->name);
	else if (ret->value != NULL)
	    xmlGenericError(xmlGenericErrorContext,
		    "Free catalog entry %s\n", ret->value);
	else
	    xmlGenericError(xmlGenericErrorContext,
		    "Free catalog entry\n");
    }

    if (ret->name != NULL)
	xmlFree(ret->name);
    if (ret->value != NULL)
	xmlFree(ret->value);
    if (ret->URL != NULL)
	xmlFree(ret->URL);
    xmlFree(ret);
}

/**
 * xmlFreeCatalogEntryList:
 * @ret:  a Catalog entry list
 *
 * Free the memory allocated to a full chained list of Catalog entries
 */
static void
xmlFreeCatalogEntryList(xmlCatalogEntryPtr ret) {
    xmlCatalogEntryPtr next;

    while (ret != NULL) {
	next = ret->next;
	xmlFreeCatalogEntry(ret);
	ret = next;
    }
}

/**
 * xmlFreeCatalogHashEntryList:
 * @ret:  a Catalog entry list
 *
 * Free the memory allocated to list of Catalog entries from the
 * catalog file hash.
 */
static void
xmlFreeCatalogHashEntryList(xmlCatalogEntryPtr catal) {
    xmlCatalogEntryPtr children, next;

    if (catal == NULL)
	return;

    children = catal->children;
    while (children != NULL) {
	next = children->next;
	children->dealloc = 0;
	children->children = NULL;
	xmlFreeCatalogEntry(children);
	children = next;
    }
    catal->dealloc = 0;
    xmlFreeCatalogEntry(catal);
}

/**
 * xmlCreateNewCatalog:
 * @type:  type of catalog
 * @prefer:  the PUBLIC vs. SYSTEM current preference value
 *
 * create a new Catalog, this type is shared both by XML and 
 * SGML catalogs, but the acceptable types values differs.
 *
 * Returns the xmlCatalogPtr or NULL in case of error
 */
static xmlCatalogPtr
xmlCreateNewCatalog(xmlCatalogType type, xmlCatalogPrefer prefer) {
    xmlCatalogPtr ret;

    ret = (xmlCatalogPtr) xmlMalloc(sizeof(xmlCatalog));
    if (ret == NULL) {
        xmlCatalogErrMemory("allocating catalog");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlCatalog));
    ret->type = type;
    ret->catalNr = 0;
    ret->catalMax = XML_MAX_SGML_CATA_DEPTH;
    ret->prefer = prefer;
    if (ret->type == XML_SGML_CATALOG_TYPE)
	ret->sgml = xmlHashCreate(10);
    return(ret);
}

/**
 * xmlFreeCatalog:
 * @catal:  a Catalog
 *
 * Free the memory allocated to a Catalog
 */
void
xmlFreeCatalog(xmlCatalogPtr catal) {
    if (catal == NULL)
	return;
    if (catal->xml != NULL)
	xmlFreeCatalogEntryList(catal->xml);
    if (catal->sgml != NULL)
	xmlHashFree(catal->sgml,
		(xmlHashDeallocator) xmlFreeCatalogEntry);
    xmlFree(catal);
}

/************************************************************************
 *									*
 *			Serializing Catalogs				*
 *									*
 ************************************************************************/

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlCatalogDumpEntry:
 * @entry:  the catalog entry
 * @out:  the file.
 *
 * Serialize an SGML Catalog entry
 */
static void
xmlCatalogDumpEntry(xmlCatalogEntryPtr entry, FILE *out) {
    if ((entry == NULL) || (out == NULL))
	return;
    switch (entry->type) {
	case SGML_CATA_ENTITY:
	    fprintf(out, "ENTITY "); break;
	case SGML_CATA_PENTITY:
	    fprintf(out, "ENTITY %%"); break;
	case SGML_CATA_DOCTYPE:
	    fprintf(out, "DOCTYPE "); break;
	case SGML_CATA_LINKTYPE:
	    fprintf(out, "LINKTYPE "); break;
	case SGML_CATA_NOTATION:
	    fprintf(out, "NOTATION "); break;
	case SGML_CATA_PUBLIC:
	    fprintf(out, "PUBLIC "); break;
	case SGML_CATA_SYSTEM:
	    fprintf(out, "SYSTEM "); break;
	case SGML_CATA_DELEGATE:
	    fprintf(out, "DELEGATE "); break;
	case SGML_CATA_BASE:
	    fprintf(out, "BASE "); break;
	case SGML_CATA_CATALOG:
	    fprintf(out, "CATALOG "); break;
	case SGML_CATA_DOCUMENT:
	    fprintf(out, "DOCUMENT "); break;
	case SGML_CATA_SGMLDECL:
	    fprintf(out, "SGMLDECL "); break;
	default:
	    return;
    }
    switch (entry->type) {
	case SGML_CATA_ENTITY:
	case SGML_CATA_PENTITY:
	case SGML_CATA_DOCTYPE:
	case SGML_CATA_LINKTYPE:
	case SGML_CATA_NOTATION:
	    fprintf(out, "%s", (const char *) entry->name); break;
	case SGML_CATA_PUBLIC:
	case SGML_CATA_SYSTEM:
	case SGML_CATA_SGMLDECL:
	case SGML_CATA_DOCUMENT:
	case SGML_CATA_CATALOG:
	case SGML_CATA_BASE:
	case SGML_CATA_DELEGATE:
	    fprintf(out, "\"%s\"", entry->name); break;
	default:
	    break;
    }
    switch (entry->type) {
	case SGML_CATA_ENTITY:
	case SGML_CATA_PENTITY:
	case SGML_CATA_DOCTYPE:
	case SGML_CATA_LINKTYPE:
	case SGML_CATA_NOTATION:
	case SGML_CATA_PUBLIC:
	case SGML_CATA_SYSTEM:
	case SGML_CATA_DELEGATE:
	    fprintf(out, " \"%s\"", entry->value); break;
	default:
	    break;
    }
    fprintf(out, "\n");
}

/**
 * xmlDumpXMLCatalogNode:
 * @catal:  top catalog entry
 * @catalog: pointer to the xml tree
 * @doc: the containing document
 * @ns: the current namespace
 * @cgroup: group node for group members
 *
 * Serializes a Catalog entry, called by xmlDumpXMLCatalog and recursively
 * for group entries
 */
static void xmlDumpXMLCatalogNode(xmlCatalogEntryPtr catal, xmlNodePtr catalog,
		    xmlDocPtr doc, xmlNsPtr ns, xmlCatalogEntryPtr cgroup) {
    xmlNodePtr node;
    xmlCatalogEntryPtr cur;
    /*
     * add all the catalog entries
     */
    cur = catal;
    while (cur != NULL) {
        if (cur->group == cgroup) {
	    switch (cur->type) {
	        case XML_CATA_REMOVED:
		    break;
	        case XML_CATA_BROKEN_CATALOG:
	        case XML_CATA_CATALOG:
		    if (cur == catal) {
			cur = cur->children;
		        continue;
		    }
		    break;
		case XML_CATA_NEXT_CATALOG:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "nextCatalog", NULL);
		    xmlSetProp(node, BAD_CAST "catalog", cur->value);
		    xmlAddChild(catalog, node);
                    break;
		case XML_CATA_NONE:
		    break;
		case XML_CATA_GROUP:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "group", NULL);
		    xmlSetProp(node, BAD_CAST "id", cur->name);
		    if (cur->value != NULL) {
		        xmlNsPtr xns;
			xns = xmlSearchNsByHref(doc, node, XML_XML_NAMESPACE);
			if (xns != NULL)
			    xmlSetNsProp(node, xns, BAD_CAST "base",
			    		 cur->value);
		    }
		    switch (cur->prefer) {
			case XML_CATA_PREFER_NONE:
		            break;
			case XML_CATA_PREFER_PUBLIC:
		            xmlSetProp(node, BAD_CAST "prefer", BAD_CAST "public");
			    break;
			case XML_CATA_PREFER_SYSTEM:
		            xmlSetProp(node, BAD_CAST "prefer", BAD_CAST "system");
			    break;
		    }
		    xmlDumpXMLCatalogNode(cur->next, node, doc, ns, cur);
		    xmlAddChild(catalog, node);
	            break;
		case XML_CATA_PUBLIC:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "public", NULL);
		    xmlSetProp(node, BAD_CAST "publicId", cur->name);
		    xmlSetProp(node, BAD_CAST "uri", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_SYSTEM:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "system", NULL);
		    xmlSetProp(node, BAD_CAST "systemId", cur->name);
		    xmlSetProp(node, BAD_CAST "uri", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_REWRITE_SYSTEM:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "rewriteSystem", NULL);
		    xmlSetProp(node, BAD_CAST "systemIdStartString", cur->name);
		    xmlSetProp(node, BAD_CAST "rewritePrefix", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_DELEGATE_PUBLIC:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "delegatePublic", NULL);
		    xmlSetProp(node, BAD_CAST "publicIdStartString", cur->name);
		    xmlSetProp(node, BAD_CAST "catalog", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_DELEGATE_SYSTEM:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "delegateSystem", NULL);
		    xmlSetProp(node, BAD_CAST "systemIdStartString", cur->name);
		    xmlSetProp(node, BAD_CAST "catalog", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_URI:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "uri", NULL);
		    xmlSetProp(node, BAD_CAST "name", cur->name);
		    xmlSetProp(node, BAD_CAST "uri", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_REWRITE_URI:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "rewriteURI", NULL);
		    xmlSetProp(node, BAD_CAST "uriStartString", cur->name);
		    xmlSetProp(node, BAD_CAST "rewritePrefix", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case XML_CATA_DELEGATE_URI:
		    node = xmlNewDocNode(doc, ns, BAD_CAST "delegateURI", NULL);
		    xmlSetProp(node, BAD_CAST "uriStartString", cur->name);
		    xmlSetProp(node, BAD_CAST "catalog", cur->value);
		    xmlAddChild(catalog, node);
		    break;
		case SGML_CATA_SYSTEM:
		case SGML_CATA_PUBLIC:
		case SGML_CATA_ENTITY:
		case SGML_CATA_PENTITY:
		case SGML_CATA_DOCTYPE:
		case SGML_CATA_LINKTYPE:
		case SGML_CATA_NOTATION:
		case SGML_CATA_DELEGATE:
		case SGML_CATA_BASE:
		case SGML_CATA_CATALOG:
		case SGML_CATA_DOCUMENT:
		case SGML_CATA_SGMLDECL:
		    break;
	    }
        }
	cur = cur->next;
    }
}

static int
xmlDumpXMLCatalog(FILE *out, xmlCatalogEntryPtr catal) {
    int ret;
    xmlDocPtr doc;
    xmlNsPtr ns;
    xmlDtdPtr dtd;
    xmlNodePtr catalog;
    xmlOutputBufferPtr buf;

    /*
     * Rebuild a catalog
     */
    doc = xmlNewDoc(NULL);
    if (doc == NULL)
	return(-1);
    dtd = xmlNewDtd(doc, BAD_CAST "catalog",
	       BAD_CAST "-//OASIS//DTD Entity Resolution XML Catalog V1.0//EN",
BAD_CAST "http://www.oasis-open.org/committees/entity/release/1.0/catalog.dtd");

    xmlAddChild((xmlNodePtr) doc, (xmlNodePtr) dtd);

    ns = xmlNewNs(NULL, XML_CATALOGS_NAMESPACE, NULL);
    if (ns == NULL) {
	xmlFreeDoc(doc);
	return(-1);
    }
    catalog = xmlNewDocNode(doc, ns, BAD_CAST "catalog", NULL);
    if (catalog == NULL) {
	xmlFreeNs(ns);
	xmlFreeDoc(doc);
	return(-1);
    }
    catalog->nsDef = ns;
    xmlAddChild((xmlNodePtr) doc, catalog);

    xmlDumpXMLCatalogNode(catal, catalog, doc, ns, NULL);
    
    /*
     * reserialize it
     */
    buf = xmlOutputBufferCreateFile(out, NULL);
    if (buf == NULL) {
	xmlFreeDoc(doc);
	return(-1);
    }
    ret = xmlSaveFormatFileTo(buf, doc, NULL, 1);

    /*
     * Free it
     */
    xmlFreeDoc(doc);

    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/************************************************************************
 *									*
 *			Converting SGML Catalogs to XML			*
 *									*
 ************************************************************************/

/**
 * xmlCatalogConvertEntry:
 * @entry:  the entry
 * @catal:  pointer to the catalog being converted
 *
 * Convert one entry from the catalog
 */
static void
xmlCatalogConvertEntry(xmlCatalogEntryPtr entry, xmlCatalogPtr catal) {
    if ((entry == NULL) || (catal == NULL) || (catal->sgml == NULL) ||
	(catal->xml == NULL))
	return;
    switch (entry->type) {
	case SGML_CATA_ENTITY:
	    entry->type = XML_CATA_PUBLIC;
	    break;
	case SGML_CATA_PENTITY:
	    entry->type = XML_CATA_PUBLIC;
	    break;
	case SGML_CATA_DOCTYPE:
	    entry->type = XML_CATA_PUBLIC;
	    break;
	case SGML_CATA_LINKTYPE:
	    entry->type = XML_CATA_PUBLIC;
	    break;
	case SGML_CATA_NOTATION:
	    entry->type = XML_CATA_PUBLIC;
	    break;
	case SGML_CATA_PUBLIC:
	    entry->type = XML_CATA_PUBLIC;
	    break;
	case SGML_CATA_SYSTEM:
	    entry->type = XML_CATA_SYSTEM;
	    break;
	case SGML_CATA_DELEGATE:
	    entry->type = XML_CATA_DELEGATE_PUBLIC;
	    break;
	case SGML_CATA_CATALOG:
	    entry->type = XML_CATA_CATALOG;
	    break;
	default:
	    xmlHashRemoveEntry(catal->sgml, entry->name,
		               (xmlHashDeallocator) xmlFreeCatalogEntry);
	    return;
    }
    /*
     * Conversion successful, remove from the SGML catalog
     * and add it to the default XML one
     */
    xmlHashRemoveEntry(catal->sgml, entry->name, NULL);
    entry->parent = catal->xml;
    entry->next = NULL;
    if (catal->xml->children == NULL)
	catal->xml->children = entry;
    else {
	xmlCatalogEntryPtr prev;

	prev = catal->xml->children;
	while (prev->next != NULL)
	    prev = prev->next;
	prev->next = entry;
    }
}

/**
 * xmlConvertSGMLCatalog:
 * @catal: the catalog
 *
 * Convert all the SGML catalog entries as XML ones
 *
 * Returns the number of entries converted if successful, -1 otherwise
 */
int
xmlConvertSGMLCatalog(xmlCatalogPtr catal) {

    if ((catal == NULL) || (catal->type != XML_SGML_CATALOG_TYPE))
	return(-1);

    if (xmlDebugCatalogs) {
	xmlGenericError(xmlGenericErrorContext,
		"Converting SGML catalog to XML\n");
    }
    xmlHashScan(catal->sgml,
		(xmlHashScanner) xmlCatalogConvertEntry,
		&catal);
    return(0);
}

/************************************************************************
 *									*
 *			Helper function					*
 *									*
 ************************************************************************/

/**
 * xmlCatalogUnWrapURN:
 * @urn:  an "urn:publicid:" to unwrap
 *
 * Expand the URN into the equivalent Public Identifier
 *
 * Returns the new identifier or NULL, the string must be deallocated
 *         by the caller.
 */
static xmlChar *
xmlCatalogUnWrapURN(const xmlChar *urn) {
    xmlChar result[2000];
    unsigned int i = 0;

    if (xmlStrncmp(urn, BAD_CAST XML_URN_PUBID, sizeof(XML_URN_PUBID) - 1))
	return(NULL);
    urn += sizeof(XML_URN_PUBID) - 1;
    
    while (*urn != 0) {
	if (i > sizeof(result) - 4)
	    break;
	if (*urn == '+') {
	    result[i++] = ' ';
	    urn++;
	} else if (*urn == ':') {
	    result[i++] = '/';
	    result[i++] = '/';
	    urn++;
	} else if (*urn == ';') {
	    result[i++] = ':';
	    result[i++] = ':';
	    urn++;
	} else if (*urn == '%') {
	    if ((urn[1] == '2') && (urn[2] == 'B'))
		result[i++] = '+';
	    else if ((urn[1] == '3') && (urn[2] == 'A'))
		result[i++] = ':';
	    else if ((urn[1] == '2') && (urn[2] == 'F'))
		result[i++] = '/';
	    else if ((urn[1] == '3') && (urn[2] == 'B'))
		result[i++] = ';';
	    else if ((urn[1] == '2') && (urn[2] == '7'))
		result[i++] = '\'';
	    else if ((urn[1] == '3') && (urn[2] == 'F'))
		result[i++] = '?';
	    else if ((urn[1] == '2') && (urn[2] == '3'))
		result[i++] = '#';
	    else if ((urn[1] == '2') && (urn[2] == '5'))
		result[i++] = '%';
	    else {
		result[i++] = *urn;
		urn++;
		continue;
	    }
	    urn += 3;
	} else {
	    result[i++] = *urn;
	    urn++;
	}
    }
    result[i] = 0;

    return(xmlStrdup(result));
}

/**
 * xmlParseCatalogFile:
 * @filename:  the filename
 *
 * parse an XML file and build a tree. It's like xmlParseFile()
 * except it bypass all catalog lookups.
 *
 * Returns the resulting document tree or NULL in case of error
 */

xmlDocPtr
xmlParseCatalogFile(const char *filename) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;
    char *directory = NULL;
    xmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
#ifdef LIBXML_SAX1_ENABLED
	if (xmlDefaultSAXHandler.error != NULL) {
	    xmlDefaultSAXHandler.error(NULL, "out of memory\n");
	}
#endif
	return(NULL);
    }

    buf = xmlParserInputBufferCreateFilename(filename, XML_CHAR_ENCODING_NONE);
    if (buf == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    inputStream = xmlNewInputStream(ctxt);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    inputStream->filename = (char *) xmlCanonicPath((const xmlChar *)filename);
    inputStream->buf = buf;
    inputStream->base = inputStream->buf->buffer->content;
    inputStream->cur = inputStream->buf->buffer->content;
    inputStream->end = 
	&inputStream->buf->buffer->content[inputStream->buf->buffer->use];

    inputPush(ctxt, inputStream);
    if ((ctxt->directory == NULL) && (directory == NULL))
        directory = xmlParserGetDirectory(filename);
    if ((ctxt->directory == NULL) && (directory != NULL))
        ctxt->directory = directory;
    ctxt->valid = 0;
    ctxt->validate = 0;
    ctxt->loadsubset = 0;
    ctxt->pedantic = 0;
    ctxt->dictNames = 1;

    xmlParseDocument(ctxt);

    if (ctxt->wellFormed)
	ret = ctxt->myDoc;
    else {
        ret = NULL;
        xmlFreeDoc(ctxt->myDoc);
        ctxt->myDoc = NULL;
    }
    xmlFreeParserCtxt(ctxt);
    
    return(ret);
}

/**
 * xmlLoadFileContent:
 * @filename:  a file path
 *
 * Load a file content into memory.
 *
 * Returns a pointer to the 0 terminated string or NULL in case of error
 */
static xmlChar *
xmlLoadFileContent(const char *filename)
{
#ifdef HAVE_STAT
    int fd;
#else
    FILE *fd;
#endif
    int len;
    long size;

#ifdef HAVE_STAT
    struct stat info;
#endif
    xmlChar *content;

    if (filename == NULL)
        return (NULL);

#ifdef HAVE_STAT
    if (stat(filename, &info) < 0)
        return (NULL);
#endif

#ifdef HAVE_STAT
    if ((fd = open(filename, O_RDONLY)) < 0)
#else
    if ((fd = fopen(filename, "rb")) == NULL)
#endif
    {
        return (NULL);
    }
#ifdef HAVE_STAT
    size = info.st_size;
#else
    if (fseek(fd, 0, SEEK_END) || (size = ftell(fd)) == EOF || fseek(fd, 0, SEEK_SET)) {        /* File operations denied? ok, just close and return failure */
        fclose(fd);
        return (NULL);
    }
#endif
    content = (unsigned char *)xmlMallocAtomic(size + 10);
    if (content == NULL) {
        xmlCatalogErrMemory("allocating catalog data");
        return (NULL);
    }
#ifdef HAVE_STAT
    len = read(fd, content, size);
#else
    len = fread(content, 1, size, fd);
#endif
    if (len < 0) {
        xmlFree(content);
        return (NULL);
    }
#ifdef HAVE_STAT
    close(fd);
#else
    fclose(fd);
#endif
    content[len] = 0;

    return(content);
}

/**
 * xmlCatalogNormalizePublic:
 * @pubID:  the public ID string
 *
 *  Normalizes the Public Identifier
 *
 * Implements 6.2. Public Identifier Normalization
 * from http://www.oasis-open.org/committees/entity/spec-2001-08-06.html
 *
 * Returns the new string or NULL, the string must be deallocated
 *         by the caller.
 */
static xmlChar *
xmlCatalogNormalizePublic(const xmlChar *pubID)
{
    int ok = 1;
    int white;
    const xmlChar *p;
    xmlChar *ret;
    xmlChar *q;

    if (pubID == NULL)
        return(NULL);

    white = 1;
    for (p = pubID;*p != 0 && ok;p++) {
        if (!xmlIsBlank_ch(*p))
            white = 0;
        else if (*p == 0x20 && !white)
            white = 1;
        else
            ok = 0;
    }
    if (ok && !white)	/* is normalized */
        return(NULL);

    ret = xmlStrdup(pubID);
    q = ret;
    white = 0;
    for (p = pubID;*p != 0;p++) {
        if (xmlIsBlank_ch(*p)) {
            if (q != ret)
                white = 1;
        } else {
            if (white) {
                *(q++) = 0x20;
                white = 0;
            }
            *(q++) = *p;
        }
    }
    *q = 0;
    return(ret);
}

/************************************************************************
 *									*
 *			The XML Catalog parser				*
 *									*
 ************************************************************************/

static xmlCatalogEntryPtr
xmlParseXMLCatalogFile(xmlCatalogPrefer prefer, const xmlChar *filename);
static void
xmlParseXMLCatalogNodeList(xmlNodePtr cur, xmlCatalogPrefer prefer,
	                   xmlCatalogEntryPtr parent, xmlCatalogEntryPtr cgroup);
static xmlChar *
xmlCatalogListXMLResolve(xmlCatalogEntryPtr catal, const xmlChar *pubID,
	              const xmlChar *sysID);
static xmlChar *
xmlCatalogListXMLResolveURI(xmlCatalogEntryPtr catal, const xmlChar *URI);


/**
 * xmlGetXMLCatalogEntryType:
 * @name:  the name
 *
 * lookup the internal type associated to an XML catalog entry name
 *
 * Returns the type associated with that name
 */
static xmlCatalogEntryType
xmlGetXMLCatalogEntryType(const xmlChar *name) {
    xmlCatalogEntryType type = XML_CATA_NONE;
    if (xmlStrEqual(name, (const xmlChar *) "system"))
	type = XML_CATA_SYSTEM;
    else if (xmlStrEqual(name, (const xmlChar *) "public"))
	type = XML_CATA_PUBLIC;
    else if (xmlStrEqual(name, (const xmlChar *) "rewriteSystem"))
	type = XML_CATA_REWRITE_SYSTEM;
    else if (xmlStrEqual(name, (const xmlChar *) "delegatePublic"))
	type = XML_CATA_DELEGATE_PUBLIC;
    else if (xmlStrEqual(name, (const xmlChar *) "delegateSystem"))
	type = XML_CATA_DELEGATE_SYSTEM;
    else if (xmlStrEqual(name, (const xmlChar *) "uri"))
	type = XML_CATA_URI;
    else if (xmlStrEqual(name, (const xmlChar *) "rewriteURI"))
	type = XML_CATA_REWRITE_URI;
    else if (xmlStrEqual(name, (const xmlChar *) "delegateURI"))
	type = XML_CATA_DELEGATE_URI;
    else if (xmlStrEqual(name, (const xmlChar *) "nextCatalog"))
	type = XML_CATA_NEXT_CATALOG;
    else if (xmlStrEqual(name, (const xmlChar *) "catalog"))
	type = XML_CATA_CATALOG;
    return(type);
}

/**
 * xmlParseXMLCatalogOneNode:
 * @cur:  the XML node
 * @type:  the type of Catalog entry
 * @name:  the name of the node
 * @attrName:  the attribute holding the value
 * @uriAttrName:  the attribute holding the URI-Reference
 * @prefer:  the PUBLIC vs. SYSTEM current preference value
 * @cgroup:  the group which includes this node
 *
 * Finishes the examination of an XML tree node of a catalog and build
 * a Catalog entry from it.
 *
 * Returns the new Catalog entry node or NULL in case of error.
 */
static xmlCatalogEntryPtr
xmlParseXMLCatalogOneNode(xmlNodePtr cur, xmlCatalogEntryType type,
			  const xmlChar *name, const xmlChar *attrName,
			  const xmlChar *uriAttrName, xmlCatalogPrefer prefer,
			  xmlCatalogEntryPtr cgroup) {
    int ok = 1;
    xmlChar *uriValue;
    xmlChar *nameValue = NULL;
    xmlChar *base = NULL;
    xmlChar *URL = NULL;
    xmlCatalogEntryPtr ret = NULL;

    if (attrName != NULL) {
	nameValue = xmlGetProp(cur, attrName);
	if (nameValue == NULL) {
	    xmlCatalogErr(ret, cur, XML_CATALOG_MISSING_ATTR,
			  "%s entry lacks '%s'\n", name, attrName, NULL);
	    ok = 0;
	}
    }
    uriValue = xmlGetProp(cur, uriAttrName);
    if (uriValue == NULL) {
	xmlCatalogErr(ret, cur, XML_CATALOG_MISSING_ATTR,
		"%s entry lacks '%s'\n", name, uriAttrName, NULL);
	ok = 0;
    }
    if (!ok) {
	if (nameValue != NULL)
	    xmlFree(nameValue);
	if (uriValue != NULL)
	    xmlFree(uriValue);
	return(NULL);
    }

    base = xmlNodeGetBase(cur->doc, cur);
    URL = xmlBuildURI(uriValue, base);
    if (URL != NULL) {
	if (xmlDebugCatalogs > 1) {
	    if (nameValue != NULL)
		xmlGenericError(xmlGenericErrorContext,
			"Found %s: '%s' '%s'\n", name, nameValue, URL);
	    else
		xmlGenericError(xmlGenericErrorContext,
			"Found %s: '%s'\n", name, URL);
	}
	ret = xmlNewCatalogEntry(type, nameValue, uriValue, URL, prefer, cgroup);
    } else {
	xmlCatalogErr(ret, cur, XML_CATALOG_ENTRY_BROKEN,
		"%s entry '%s' broken ?: %s\n", name, uriAttrName, uriValue);
    }
    if (nameValue != NULL)
	xmlFree(nameValue);
    if (uriValue != NULL)
	xmlFree(uriValue);
    if (base != NULL)
	xmlFree(base);
    if (URL != NULL)
	xmlFree(URL);
    return(ret);
}

/**
 * xmlParseXMLCatalogNode:
 * @cur:  the XML node
 * @prefer:  the PUBLIC vs. SYSTEM current preference value
 * @parent:  the parent Catalog entry
 * @cgroup:  the group which includes this node
 *
 * Examines an XML tree node of a catalog and build
 * a Catalog entry from it adding it to its parent. The examination can
 * be recursive.
 */
static void
xmlParseXMLCatalogNode(xmlNodePtr cur, xmlCatalogPrefer prefer,
	               xmlCatalogEntryPtr parent, xmlCatalogEntryPtr cgroup)
{
    xmlChar *uri = NULL;
    xmlChar *URL = NULL;
    xmlChar *base = NULL;
    xmlCatalogEntryPtr entry = NULL;

    if (cur == NULL)
        return;
    if (xmlStrEqual(cur->name, BAD_CAST "group")) {
        xmlChar *prop;
	xmlCatalogPrefer pref = XML_CATA_PREFER_NONE;

        prop = xmlGetProp(cur, BAD_CAST "prefer");
        if (prop != NULL) {
            if (xmlStrEqual(prop, BAD_CAST "system")) {
                prefer = XML_CATA_PREFER_SYSTEM;
            } else if (xmlStrEqual(prop, BAD_CAST "public")) {
                prefer = XML_CATA_PREFER_PUBLIC;
            } else {
		xmlCatalogErr(parent, cur, XML_CATALOG_PREFER_VALUE,
                              "Invalid value for prefer: '%s'\n",
			      prop, NULL, NULL);
            }
            xmlFree(p