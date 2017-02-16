/*
 * SAX2.c : Default SAX2 handler to build a tree.
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */


#define IN_LIBXML
#include "libxml.h"
#include <stdlib.h>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/entities.h>
#include <libxml/xmlerror.h>
#include <libxml/debugXML.h>
#include <libxml/xmlIO.h>
#include <libxml/SAX.h>
#include <libxml/uri.h>
#include <libxml/valid.h>
#include <libxml/HTMLtree.h>
#include <libxml/globals.h>

/* #define DEBUG_SAX2 */
/* #define DEBUG_SAX2_TREE */

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

/*
 * xmlSAX2ErrMemory:
 * @ctxt:  an XML validation parser context
 * @msg:   a string to accompany the error message
 */
static void
xmlSAX2ErrMemory(xmlParserCtxtPtr ctxt, const char *msg) {
    if (ctxt != NULL) {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, "%s: out of memory\n", msg);
	ctxt->errNo = XML_ERR_NO_MEMORY;
	ctxt->instate = XML_PARSER_EOF;
	ctxt->disableSAX = 1;
    }
}

/**
 * xmlValidError:
 * @ctxt:  an XML validation parser context
 * @error:  the error number
 * @msg:  the error message
 * @str1:  extra data
 * @str2:  extra data
 *
 * Handle a validation error
 */
static void
xmlErrValid(xmlParserCtxtPtr ctxt, xmlParserErrors error,
            const char *msg, const char *str1, const char *str2)
{
    xmlStructuredErrorFunc schannel = NULL;

    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL) {
	ctxt->errNo = error;
	if ((ctxt->sax != NULL) && (ctxt->sax->initialized == XML_SAX2_MAGIC))
	    schannel = ctxt->sax->serror;
    }
    __xmlRaiseError(schannel,
                    ctxt->vctxt.error, ctxt->vctxt.userData,
                    ctxt, NULL, XML_FROM_DTD, error,
                    XML_ERR_ERROR, NULL, 0, (const char *) str1,
		    (const char *) str2, NULL, 0, 0,
		    msg, (const char *) str1, (const char *) str2);
    if (ctxt != NULL)
	ctxt->valid = 0;
}

/**
 * xmlSAX2GetPublicId:
 * @ctx: the user data (XML parser context)
 *
 * Provides the public ID e.g. "-//SGMLSOURCE//DTD DEMO//EN"
 *
 * Returns a xmlChar *
 */
const xmlChar *
xmlSAX2GetPublicId(void *ctx ATTRIBUTE_UNUSED)
{
    /* xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx; */
    return(NULL);
}

/**
 * xmlSAX2GetSystemId:
 * @ctx: the user data (XML parser context)
 *
 * Provides the system ID, basically URL or filename e.g.
 * http://www.sgmlsource.com/dtds/memo.dtd
 *
 * Returns a xmlChar *
 */
const xmlChar *
xmlSAX2GetSystemId(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if ((ctx == NULL) || (ctxt->input == NULL)) return(0);
    return((const xmlChar *) ctxt->input->filename); 
}

/**
 * xmlSAX2GetLineNumber:
 * @ctx: the user data (XML parser context)
 *
 * Provide the line number of the current parsing point.
 *
 * Returns an int
 */
int
xmlSAX2GetLineNumber(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if ((ctx == NULL) || (ctxt->input == NULL)) return(0);
    return(ctxt->input->line);
}

/**
 * xmlSAX2GetColumnNumber:
 * @ctx: the user data (XML parser context)
 *
 * Provide the column number of the current parsing point.
 *
 * Returns an int
 */
int
xmlSAX2GetColumnNumber(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if ((ctx == NULL) || (ctxt->input == NULL)) return(0);
    return(ctxt->input->col);
}

/**
 * xmlSAX2IsStandalone:
 * @ctx: the user data (XML parser context)
 *
 * Is this document tagged standalone ?
 *
 * Returns 1 if true
 */
int
xmlSAX2IsStandalone(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if ((ctx == NULL) || (ctxt->myDoc == NULL)) return(0);
    return(ctxt->myDoc->standalone == 1);
}

/**
 * xmlSAX2HasInternalSubset:
 * @ctx: the user data (XML parser context)
 *
 * Does this document has an internal subset
 *
 * Returns 1 if true
 */
int
xmlSAX2HasInternalSubset(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if ((ctxt == NULL) || (ctxt->myDoc == NULL)) return(0);
    return(ctxt->myDoc->intSubset != NULL);
}

/**
 * xmlSAX2HasExternalSubset:
 * @ctx: the user data (XML parser context)
 *
 * Does this document has an external subset
 *
 * Returns 1 if true
 */
int
xmlSAX2HasExternalSubset(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if ((ctxt == NULL) || (ctxt->myDoc == NULL)) return(0);
    return(ctxt->myDoc->extSubset != NULL);
}

/**
 * xmlSAX2InternalSubset:
 * @ctx:  the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on internal subset declaration.
 */
void
xmlSAX2InternalSubset(void *ctx, const xmlChar *name,
	       const xmlChar *ExternalID, const xmlChar *SystemID)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlDtdPtr dtd;
    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2InternalSubset(%s, %s, %s)\n",
            name, ExternalID, SystemID);
#endif

    if (ctxt->myDoc == NULL)
	return;
    dtd = xmlGetIntSubset(ctxt->myDoc);
    if (dtd != NULL) {
	if (ctxt->html)
	    return;
	xmlUnlinkNode((xmlNodePtr) dtd);
	xmlFreeDtd(dtd);
	ctxt->myDoc->intSubset = NULL;
    }
    ctxt->myDoc->intSubset = 
	xmlCreateIntSubset(ctxt->myDoc, name, ExternalID, SystemID);
    if (ctxt->myDoc->intSubset == NULL)
        xmlSAX2ErrMemory(ctxt, "xmlSAX2InternalSubset");
}

/**
 * xmlSAX2ExternalSubset:
 * @ctx: the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on external subset declaration.
 */
void
xmlSAX2ExternalSubset(void *ctx, const xmlChar *name,
	       const xmlChar *ExternalID, const xmlChar *SystemID)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2ExternalSubset(%s, %s, %s)\n",
            name, ExternalID, SystemID);
#endif
    if (((ExternalID != NULL) || (SystemID != NULL)) &&
        (((ctxt->validate) || (ctxt->loadsubset != 0)) &&
	 (ctxt->wellFormed && ctxt->myDoc))) {
	/*
	 * Try to fetch and parse the external subset.
	 */
	xmlParserInputPtr oldinput;
	int oldinputNr;
	int oldinputMax;
	xmlParserInputPtr *oldinputTab;
	xmlParserInputPtr input = NULL;
	xmlCharEncoding enc;
	int oldcharset;

	/*
	 * Ask the Entity resolver to load the damn thing
	 */
	if ((ctxt->sax != NULL) && (ctxt->sax->resolveEntity != NULL))
	    input = ctxt->sax->resolveEntity(ctxt->userData, ExternalID,
	                                        SystemID);
	if (input == NULL) {
	    return;
	}

	xmlNewDtd(ctxt->myDoc, name, ExternalID, SystemID);

	/*
	 * make sure we won't destroy the main document context
	 */
	oldinput = ctxt->input;
	oldinputNr = ctxt->inputNr;
	oldinputMax = ctxt->inputMax;
	oldinputTab = ctxt->inputTab;
	oldcharset = ctxt->charset;

	ctxt->inputTab = (xmlParserInputPtr *)
	                 xmlMalloc(5 * sizeof(xmlParserInputPtr));
	if (ctxt->inputTab == NULL) {
	    xmlSAX2ErrMemory(ctxt, "xmlSAX2ExternalSubset");
	    ctxt->input = oldinput;
	    ctxt->inputNr = oldinputNr;
	    ctxt->inputMax = oldinputMax;
	    ctxt->inputTab = oldinputTab;
	    ctxt->charset = oldcharset;
	    return;
	}
	ctxt->inputNr = 0;
	ctxt->inputMax = 5;
	ctxt->input = NULL;
	xmlPushInput(ctxt, input);

	/*
	 * On the fly encoding conversion if needed
	 */
	if (ctxt->input->length >= 4) {
	    enc = xmlDetectCharEncoding(ctxt->input->cur, 4);
	    xmlSwitchEncoding(ctxt, enc);
	}

	if (input->filename == NULL)
	    input->filename = (char *) xmlCanonicPath(SystemID);
	input->line = 1;
	input->col = 1;
	input->base = ctxt->input->cur;
	input->cur = ctxt->input->cur;
	input->free = NULL;

	/*
	 * let's parse that entity knowing it's an external subset.
	 */
	xmlParseExternalSubset(ctxt, ExternalID, SystemID);

        /*
	 * Free up the external entities
	 */

	while (ctxt->inputNr > 1)
	    xmlPopInput(ctxt);
	xmlFreeInputStream(ctxt->input);
        xmlFree(ctxt->inputTab);

	/*
	 * Restore the parsing context of the main entity
	 */
	ctxt->input = oldinput;
	ctxt->inputNr = oldinputNr;
	ctxt->inputMax = oldinputMax;
	ctxt->inputTab = oldinputTab;
	ctxt->charset = oldcharset;
	/* ctxt->wellFormed = oldwellFormed; */
    }
}

/**
 * xmlSAX2ResolveEntity:
 * @ctx: the user data (XML parser context)
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * The entity loader, to control the loading of external entities,
 * the application can either:
 *    - override this xmlSAX2ResolveEntity() callback in the SAX block
 *    - or better use the xmlSetExternalEntityLoader() function to
 *      set up it's own entity resolution routine
 *
 * Returns the xmlParserInputPtr if inlined or NULL for DOM behaviour.
 */
xmlParserInputPtr
xmlSAX2ResolveEntity(void *ctx, const xmlChar *publicId, const xmlChar *systemId)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr ret;
    xmlChar *URI;
    const char *base = NULL;

    if (ctx == NULL) return(NULL);
    if (ctxt->input != NULL)
	base = ctxt->input->filename;
    if (base == NULL)
	base = ctxt->directory;

    URI = xmlBuildURI(systemId, (const xmlChar *) base);

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2ResolveEntity(%s, %s)\n", publicId, systemId);
#endif

    ret = xmlLoadExternalEntity((const char *) URI,
				(const char *) publicId, ctxt);
    if (URI != NULL)
	xmlFree(URI);
    return(ret);
}

/**
 * xmlSAX2GetEntity:
 * @ctx: the user data (XML parser context)
 * @name: The entity name
 *
 * Get an entity by name
 *
 * Returns the xmlEntityPtr if found.
 */
xmlEntityPtr
xmlSAX2GetEntity(void *ctx, const xmlChar *name)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlEntityPtr ret = NULL;

    if (ctx == NULL) return(NULL);
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2GetEntity(%s)\n", name);
#endif

    if (ctxt->inSubset == 0) {
	ret = xmlGetPredefinedEntity(name);
	if (ret != NULL)
	    return(ret);
    }
    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->standalone == 1)) {
	if (ctxt->inSubset == 2) {
	    ctxt->myDoc->standalone = 0;
	    ret = xmlGetDocEntity(ctxt->myDoc, name);
	    ctxt->myDoc->standalone = 1;
	} else {
	    ret = xmlGetDocEntity(ctxt->myDoc, name);
	    if (ret == NULL) {
		ctxt->myDoc->standalone = 0;
		ret = xmlGetDocEntity(ctxt->myDoc, name);
		if (ret != NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData, 
		 "Entity(%s) document marked standalone but requires external subset\n",
					 name);
		    ctxt->valid = 0;
		    ctxt->wellFormed = 0;
		}
		ctxt->myDoc->standalone = 1;
	    }
	}
    } else {
	ret = xmlGetDocEntity(ctxt->myDoc, name);
    }
    if ((ret != NULL) &&
	((ctxt->validate) || (ctxt->replaceEntities)) &&
	(ret->children == NULL) &&
	(ret->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)) {
	int val;

	/*
	 * for validation purposes we really need to fetch and
	 * parse the external entity
	 */
	xmlNodePtr children;

        val = xmlParseCtxtExternalEntity(ctxt, ret->URI,
		                         ret->ExternalID, &children);
	if (val == 0) {
	    xmlAddChildList((xmlNodePtr) ret, children);
	} else {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	        ctxt->sax->error(ctxt->userData, 
		 "Failure to process entity %s\n", name);
	    ctxt->wellFormed = 0;
	    ctxt->valid = 0;
	    ctxt->validate = 0;
	    return(NULL);
	}
	ret->owner = 1;
    }
    return(ret);
}

/**
 * xmlSAX2GetParameterEntity:
 * @ctx: the user data (XML parser context)
 * @name: The entity name
 *
 * Get a parameter entity by name
 *
 * Returns the xmlEntityPtr if found.
 */
xmlEntityPtr
xmlSAX2GetParameterEntity(void *ctx, const xmlChar *name)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlEntityPtr ret;

    if (ctx == NULL) return(NULL);
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2GetParameterEntity(%s)\n", name);
#endif

    ret = xmlGetParameterEntity(ctxt->myDoc, name);
    return(ret);
}


/**
 * xmlSAX2EntityDecl:
 * @ctx: the user data (XML parser context)
 * @name:  the entity name 
 * @type:  the entity type 
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @content: the entity value (without processing).
 *
 * An entity definition has been parsed
 */
void
xmlSAX2EntityDecl(void *ctx, const xmlChar *name, int type,
          const xmlChar *publicId, const xmlChar *systemId, xmlChar *content)
{
    xmlEntityPtr ent;
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;

    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2EntityDecl(%s, %d, %s, %s, %s)\n",
            name, type, publicId, systemId, content);
#endif
    if (ctxt->inSubset == 1) {
	ent = xmlAddDocEntity(ctxt->myDoc, name, type, publicId,
		              systemId, content);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt->userData, 
	     "Entity(%s) already defined in the internal subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else if (ctxt->inSubset == 2) {
	ent = xmlAddDtdEntity(ctxt->myDoc, name, type, publicId,
		              systemId, content);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt->userData, 
	     "Entity(%s) already defined in the external subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	     "SAX.xmlSAX2EntityDecl(%s) called while not in subset\n", name);
    }
}

/**
 * xmlSAX2AttributeDecl:
 * @ctx: the user data (XML parser context)
 * @elem:  the name of the element
 * @fullname:  the attribute name 
 * @type:  the attribute type 
 * @def:  the type of default value
 * @defaultValue: the attribute default value
 * @tree:  the tree of enumerated value set
 *
 * An attribute definition has been parsed
 */
void
xmlSAX2AttributeDecl(void *ctx, const xmlChar *elem, const xmlChar *fullname,
              int type, int def, const xmlChar *defaultValue,
	      xmlEnumerationPtr tree)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlAttributePtr attr;
    xmlChar *name = NULL, *prefix = NULL;

    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2AttributeDecl(%s, %s, %d, %d, %s, ...)\n",
            elem, fullname, type, def, defaultValue);
#endif
    if ((xmlStrEqual(fullname, BAD_CAST "xml:id")) &&
        (type != XML_ATTRIBUTE_ID)) {
	/*
	 * Raise the error but keep the validity flag
	 */
	int tmp = ctxt->valid;
	xmlErrValid(ctxt, XML_DTD_XMLID_TYPE,
	      "xml:id : attribute type should be ID\n", NULL, NULL);
	ctxt->valid = tmp;
    }
    /* TODO: optimize name/prefix allocation */
    name = xmlSplitQName(ctxt, fullname, &prefix);
    ctxt->vctxt.valid = 1;
    if (ctxt->inSubset == 1)
	attr = xmlAddAttributeDecl(&ctxt->vctxt, ctxt->myDoc->intSubset, elem,
	       name, prefix, (xmlAttributeType) type,
	       (xmlAttributeDefault) def, defaultValue, tree);
    else if (ctxt->inSubset == 2)
	attr = xmlAddAttributeDecl(&ctxt->vctxt, ctxt->myDoc->extSubset, elem,
	   name, prefix, (xmlAttributeType) type, 
	   (xmlAttributeDefault) def, defaultValue, tree);
    else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	     "SAX.xmlSAX2AttributeDecl(%s) called while not in subset\n", name);
	xmlFreeEnumeration(tree);
	return;
    }
#ifdef LIBXML_VALID_ENABLED
    if (ctxt->vctxt.valid == 0)
	ctxt->valid = 0;
    if ((attr != NULL) && (ctxt->validate) && (ctxt->wellFormed) &&
        (ctxt->myDoc != NULL) && (ctxt->myDoc->intSubset != NULL))
	ctxt->valid &= xmlValidateAttributeDecl(&ctxt->vctxt, ctxt->myDoc,
	                                        attr);
#endif /* LIBXML_VALID_ENABLED */
    if (prefix != NULL)
	xmlFree(prefix);
    if (name != NULL)
	xmlFree(name);
}

/**
 * xmlSAX2ElementDecl:
 * @ctx: the user data (XML parser context)
 * @name:  the element name 
 * @type:  the element type 
 * @content: the element value tree
 *
 * An element definition has been parsed
 */
void
xmlSAX2ElementDecl(void *ctx, const xmlChar * name, int type,
            xmlElementContentPtr content)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlElementPtr elem = NULL;

    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
                    "SAX.xmlSAX2ElementDecl(%s, %d, ...)\n", name, type);
#endif

    if (ctxt->inSubset == 1)
        elem = xmlAddElementDecl(&ctxt->vctxt, ctxt->myDoc->intSubset,
                                 name, (xmlElementTypeVal) type, content);
    else if (ctxt->inSubset == 2)
        elem = xmlAddElementDecl(&ctxt->vctxt, ctxt->myDoc->extSubset,
                                 name, (xmlElementTypeVal) type, content);
    else {
        if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
            ctxt->sax->error(ctxt->userData,
                             "SAX.xmlSAX2ElementDecl(%s) called while not in subset\n",
                             name);
        return;
    }
#ifdef LIBXML_VALID_ENABLED
    if (elem == NULL)
        ctxt->valid = 0;
    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
        ctxt->valid &=
            xmlValidateElementDecl(&ctxt->vctxt, ctxt->myDoc, elem);
#endif /* LIBXML_VALID_ENABLED */
}

/**
 * xmlSAX2NotationDecl:
 * @ctx: the user data (XML parser context)
 * @name: The name of the notation
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * What to do when a notation declaration has been parsed.
 */
void
xmlSAX2NotationDecl(void *ctx, const xmlChar *name,
	     const xmlChar *publicId, const xmlChar *systemId)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlNotationPtr nota = NULL;

    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2NotationDecl(%s, %s, %s)\n", name, publicId, systemId);
#endif

    if ((publicId == NULL) && (systemId == NULL)) {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	     "SAX.xmlSAX2NotationDecl(%s) externalID or PublicID missing\n", name);
	ctxt->valid = 0;
	ctxt->wellFormed = 0;
	return;
    } else if (ctxt->inSubset == 1)
	nota = xmlAddNotationDecl(&ctxt->vctxt, ctxt->myDoc->intSubset, name,
                              publicId, systemId);
    else if (ctxt->inSubset == 2)
	nota = xmlAddNotationDecl(&ctxt->vctxt, ctxt->myDoc->extSubset, name,
                              publicId, systemId);
    else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	     "SAX.xmlSAX2NotationDecl(%s) called while not in subset\n", name);
	return;
    }
#ifdef LIBXML_VALID_ENABLED
    if (nota == NULL) ctxt->valid = 0;
    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
	ctxt->valid &= xmlValidateNotationDecl(&ctxt->vctxt, ctxt->myDoc,
	                                       nota);
#endif /* LIBXML_VALID_ENABLED */
}

/**
 * xmlSAX2UnparsedEntityDecl:
 * @ctx: the user data (XML parser context)
 * @name: The name of the entity
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @notationName: the name of the notation
 *
 * What to do when an unparsed entity declaration is parsed
 */
void
xmlSAX2UnparsedEntityDecl(void *ctx, const xmlChar *name,
		   const xmlChar *publicId, const xmlChar *systemId,
		   const xmlChar *notationName)
{
    xmlEntityPtr ent;
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    if (ctx == NULL) return;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2UnparsedEntityDecl(%s, %s, %s, %s)\n",
            name, publicId, systemId, notationName);
#endif
    if (ctxt->inSubset == 1) {
	ent = xmlAddDocEntity(ctxt->myDoc, name,
			XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
			publicId, systemId, notationName);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt->userData, 
	     "Entity(%s) already defined in the internal subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else if (ctxt->inSubset == 2) {
	ent = xmlAddDtdEntity(ctxt->myDoc, name,
			XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
			publicId, systemId, notationName);
	if ((ent == NULL) && (ctxt->pedantic) &&
	    (ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
	    ctxt->sax->warning(ctxt->userData, 
	     "Entity(%s) already defined in the external subset\n", name);
	if ((ent != NULL) && (ent->URI == NULL) && (systemId != NULL)) {
	    xmlChar *URI;
	    const char *base = NULL;

	    if (ctxt->input != NULL)
		base = ctxt->input->filename;
	    if (base == NULL)
		base = ctxt->directory;
	
	    URI = xmlBuildURI(systemId, (const xmlChar *) base);
	    ent->URI = URI;
	}
    } else {
	if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
	    ctxt->sax->error(ctxt->userData, 
	     "SAX.xmlSAX2UnparsedEntityDecl(%s) called while not in subset\n", name);
    }
}

/**
 * xmlSAX2SetDocumentLocator:
 * @ctx: the user data (XML parser context)
 * @loc: A SAX Locator
 *
 * Receive the document locator at startup, actually xmlDefaultSAXLocator
 * Everything is available on the context, so this is useless in our case.
 */
void
xmlSAX2SetDocumentLocator(void *ctx ATTRIBUTE_UNUSED, xmlSAXLocatorPtr loc ATTRIBUTE_UNUSED)
{
    /* xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx; */
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2SetDocumentLocator()\n");
#endif
}

/**
 * xmlSAX2StartDocument:
 * @ctx: the user data (XML parser context)
 *
 * called when the document start being processed.
 */
void
xmlSAX2StartDocument(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlDocPtr doc;

    if (ctx == NULL) return;

#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2StartDocument()\n");
#endif
    if (ctxt->html) {
#ifdef LIBXML_HTML_ENABLED
	if (ctxt->myDoc == NULL)
	    ctxt->myDoc = htmlNewDocNoDtD(NULL, NULL);
	if (ctxt->myDoc == NULL) {
	    xmlSAX2ErrMemory(ctxt, "xmlSAX2StartDocument");
	    return;
	}
#else
        xmlGenericError(xmlGenericErrorContext,
		"libxml2 built without HTML support\n");
	ctxt->errNo = XML_ERR_INTERNAL_ERROR;
	ctxt->instate = XML_PARSER_EOF;
	ctxt->disableSAX = 1;
	return;
#endif
    } else {
	doc = ctxt->myDoc = xmlNewDoc(ctxt->version);
	if (doc != NULL) {
	    if (ctxt->encoding != NULL)
		doc->encoding = xmlStrdup(ctxt->encoding);
	    else
		doc->encoding = NULL;
	    doc->standalone = ctxt->standalone;
	} else {
	    xmlSAX2ErrMemory(ctxt, "xmlSAX2StartDocument");
	    return;
	}
	if ((ctxt->dictNames) && (doc != NULL)) {
	    doc->dict = ctxt->dict;
	    xmlDictReference(doc->dict);
	}
    }
    if ((ctxt->myDoc != NULL) && (ctxt->myDoc->URL == NULL) &&
	(ctxt->input != NULL) && (ctxt->input->filename != NULL)) {
	ctxt->myDoc->URL = xmlCanonicPath((const xmlChar *) ctxt->input->filename);
	if (ctxt->myDoc->URL == NULL)
	    xmlSAX2ErrMemory(ctxt, "xmlSAX2StartDocument");
    }
}

/**
 * xmlSAX2EndDocument:
 * @ctx: the user data (XML parser context)
 *
 * called when the document end has been detected.
 */
void
xmlSAX2EndDocument(void *ctx)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
#ifdef DEBUG_SAX
    xmlGenericError(xmlGenericErrorContext,
	    "SAX.xmlSAX2EndDocument()\n");
#endif
    if (ctx == NULL) return;
#ifdef LIBXML_VALID_ENABLED
    if (ctxt->validate && ctxt->wellFormed &&
        ctxt->myDoc && ctxt->myDoc->intSubset)
	ctxt->valid &= xmlValidateDocumentFinal(&ctxt->vctxt, ctxt->myDoc);
#endif /* LIBXML_VALID_ENABLED */

    /*
     * Grab the encoding if it was added on-the-fly
     */
    if ((ctxt->encoding != NULL) && (ctxt->myDoc != NULL) &&
	(ctxt->myDoc->encoding == NULL)) {
	ctxt->myDoc->encoding = ctxt->encoding;
	ctxt->encoding = NULL;
    }
    if ((ctxt->inputTab != NULL) &&
        (ctxt->inputNr > 0) && (ctxt->inputTab[0] != NULL) &&
        (ctxt->inputTab[0]->encoding != NULL) && (ctxt->myDoc != NULL) &&
	(ctxt->myDoc->encoding == NULL)) {
	ctxt->myDoc->encoding = xmlStrdup(ctxt->inputTab[0]->encoding);
    }
    if ((ctxt->charset != XML_CHAR_ENCODING_NONE) && (ctxt->myDoc != NULL) &&
	(ctxt->myDoc->charset == XML_CHAR_ENCODING_NONE)) {
	ctxt->myDoc->charset = ctxt->charset;
    }
}

#if defined(LIBXML_SAX1_ENABLED) || defined(LIBXML_HTML_ENABLED)
/**
 * xmlSAX2AttributeInternal:
 * @ctx: the user data (XML parser context)
 * @fullname:  The attribute name, including namespace1 prefix
 * @value:  The attribute value
 * @prefix: the prefix on the element node
 *
 * Handle an attribute that has been read by the parser.
 * The default handling is to convert the attribute into an
 * DOM subtree and past it in a new xmlAttr element added to
 * the element.
 */
static void
xmlSAX2AttributeInternal(void *ctx, const xmlChar *fullname,
             const xmlChar *value, const xmlChar *prefix ATTRIBUTE_UNUSED)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlAttrPtr ret;
    xmlChar *name;
    xmlChar *ns;
    xmlChar *nval;
    xmlNsPtr namespace1;

    /*
     * Split the full name into a namespace1 prefix and the tag name
     */
    name = xmlSplitQName(ctxt, fullname, &ns);
    if ((name != NULL) && (name[0] == 0)) {
        if (xmlStrEqual(ns, BAD_CAST "xmlns")) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		     "invalid namespace1 declaration '%s'\n", fullname);
	} else {
	    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
		ctxt->sax->warning(ctxt->userData, 
		     "Avoid attribute ending with ':' like '%s'\n", fullname);
	}
	if (ns != NULL)
	    xmlFree(ns);
	ns = NULL;
	xmlFree(name);
	name = xmlStrdup(fullname);
    }
    if (name == NULL) {
        xmlSAX2ErrMemory(ctxt, "xmlSAX2StartElement");
	if (ns != NULL)
	    xmlFree(ns);
	return;
    }

#ifdef LIBXML_VALID_ENABLED
    /*
     * Do the last stage of the attribute normalization
     * Needed for HTML too:
     *   http://www.w3.org/TR/html4/types.html#h-6.2
     */
    ctxt->vctxt.valid = 1;
    nval = xmlValidCtxtNormalizeAttributeValue(&ctxt->vctxt,
	                                   ctxt->myDoc, ctxt->node,
					   fullname, value);
    if (ctxt->vctxt.valid != 1) {
	ctxt->valid = 0;
    }
    if (nval != NULL)
	value = nval;
#else
    nval = NULL;
#endif /* LIBXML_VALID_ENABLED */

    /*
     * Check whether it's a namespace1 definition
     */
    if ((!ctxt->html) && (ns == NULL) &&
        (name[0] == 'x') && (name[1] == 'm') && (name[2] == 'l') &&
        (name[3] == 'n') && (name[4] == 's') && (name[5] == 0)) {
	xmlNsPtr nsret;
	xmlChar *val;

        if (!ctxt->replaceEntities) {
	    ctxt->depth++;
	    val = xmlStringDecodeEntities(ctxt, value, XML_SUBSTITUTE_REF,
		                          0,0,0);
	    ctxt->depth--;
	} else {
	    val = (xmlChar *) value;
	}

	if (val[0] != 0) {
	    xmlURIPtr uri;

	    uri = xmlParseURI((const char *)val);
	    if (uri == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
		    ctxt->sax->warning(ctxt->userData, 
			 "xmlns: %s not a valid URI\n", val);
	    } else {
		if (uri->scheme == NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			ctxt->sax->warning(ctxt->userData, 
			     "xmlns: URI %s is not absolute\n", val);
		}
		xmlFreeURI(uri);
	    }
	}

	/* a default namespace1 definition */
	nsret = xmlNewNs(ctxt->node, val, NULL);

#ifdef LIBXML_VALID_ENABLED
	/*
	 * Validate also for namespace1 decls, they are attributes from
	 * an XML-1.0 perspective
	 */
        if (nsret != NULL && ctxt->validate && ctxt->wellFormed &&
	    ctxt->myDoc && ctxt->myDoc->intSubset)
	    ctxt->valid &= xmlValidateOneNamespace(&ctxt->vctxt, ctxt->myDoc,
					   ctxt->node, prefix, nsret, val);
#endif /* LIBXML_VALID_ENABLED */
	if (name != NULL) 
	    xmlFree(name);
	if (nval != NULL)
	    xmlFree(nval);
	if (val != value)
	    xmlFree(val);
	return;
    }
    if ((!ctxt->html) &&
	(ns != NULL) && (ns[0] == 'x') && (ns[1] == 'm') && (ns[2] == 'l') &&
        (ns[3] == 'n') && (ns[4] == 's') && (ns[5] == 0)) {
	xmlNsPtr nsret;
	xmlChar *val;

        if (!ctxt->replaceEntities) {
	    ctxt->depth++;
	    val = xmlStringDecodeEntities(ctxt, value, XML_SUBSTITUTE_REF,
		                          0,0,0);
	    ctxt->depth--;
	    if (val == NULL) {
	        xmlSAX2ErrMemory(ctxt, "xmlSAX2StartElement");
	        xmlFree(ns);
		if (name != NULL) 
		    xmlFree(name);
		return;
	    }
	} else {
	    val = (xmlChar *) value;
	}

	if (val[0] == 0) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		     "Empty namespace1 name for prefix %s\n", name);
	}
	if ((ctxt->pedantic != 0) && (val[0] != 0)) {
	    xmlURIPtr uri;

	    uri = xmlParseURI((const char *)val);
	    if (uri == NULL) {
		if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
		    ctxt->sax->warning(ctxt->userData, 
			 "xmlns:%s: %s not a valid URI\n", name, value);
	    } else {
		if (uri->scheme == NULL) {
		    if ((ctxt->sax != NULL) && (ctxt->sax->warning != NULL))
			ctxt->sax->warning(ctxt->userData, 
			   "xmlns:%s: URI %s is not absolute\n", name, value);
		}
		xmlFreeURI(uri);
	    }
	}

	/* a standard namespace1 definition */
	nsret = xmlNewNs(ctxt->node, val, name);
	xmlFree(ns);
#ifdef LIBXML_VALID_ENABLED
	/*
	 * Validate also for namespace1 decls, they are attributes from
	 * an XML-1.0 perspective
	 */
        if (nsret != NULL && ctxt->validate && ctxt->wellFormed &&
	    ctxt->myDoc && ctxt->myDoc->intSubset)
	    ctxt->valid &= xmlValidateOneNamespace(&ctxt->vctxt, ctxt->myDoc,
					   ctxt->node, prefix, nsret, value);
#endif /* LIBXML_VALID_ENABLED */
	if (name != NULL) 
	    xmlFree(name);
	if (nval != NULL)
	    xmlFree(nval);
	if (val != value)
	    xmlFree(val);
	return;
    }

    if (ns != NULL) {
	xmlAttrPtr prop;
	namespace1 = xmlSearchNs(ctxt->myDoc, ctxt->node, ns);
	if (namespace1 == NULL) {
	    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
		ctxt->sax->error(ctxt->userData, 
		    "Namespace prefix %s of attribute %s is not defined\n",
		             ns, name);
	}

	prop = ctxt->node->properties;
	while (prop != NULL) {
	    if (prop->ns != NULL) {
		if ((xmlStrEqual(name, prop->name)) &&
		    ((namespace1 == prop->ns) ||
		     (xmlStrEqual(namespace1->href, prop->ns->href)))) {
		    ctxt->errNo = XML_ERR_ATTRIBUTE_REDEFINED;
		    if ((ctxt->sax != NULL) && (ctxt->sax->error != NULL))
			ctxt->sax->error(ctxt->userData,
			        "Attribute %s in %s redefined\n",
			                 name, namespace1->href);
		    ctxt->wellFormed = 0;
		    if (ctxt->recovery == 0) ctxt->disableSAX = 1;
		    goto error;
		}
	    }
	    prop = prop->next;
	}
    } else {
	names