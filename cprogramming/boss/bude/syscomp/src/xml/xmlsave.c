/*
 * xmlsave.c: Implemetation of the document serializer
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>

#define MAX_INDENT 60

#include <libxml/HTMLtree.h>

/************************************************************************
 *									*
 *			XHTML detection					*
 *									*
 ************************************************************************/
#define XHTML_STRICT_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Strict//EN"
#define XHTML_STRICT_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
#define XHTML_FRAME_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Frameset//EN"
#define XHTML_FRAME_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd"
#define XHTML_TRANS_PUBLIC_ID BAD_CAST \
   "-//W3C//DTD XHTML 1.0 Transitional//EN"
#define XHTML_TRANS_SYSTEM_ID BAD_CAST \
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"

#define XHTML_NS_NAME BAD_CAST "http://www.w3.org/1999/xhtml"
/**
 * xmlIsXHTML:
 * @systemID:  the system identifier
 * @publicID:  the public identifier
 *
 * Try to find if the document correspond to an XHTML DTD
 *
 * Returns 1 if true, 0 if not and -1 in case of error
 */
int
xmlIsXHTML(const xmlChar *systemID, const xmlChar *publicID) {
    if ((systemID == NULL) && (publicID == NULL))
	return(-1);
    if (publicID != NULL) {
	if (xmlStrEqual(publicID, XHTML_STRICT_PUBLIC_ID)) return(1);
	if (xmlStrEqual(publicID, XHTML_FRAME_PUBLIC_ID)) return(1);
	if (xmlStrEqual(publicID, XHTML_TRANS_PUBLIC_ID)) return(1);
    }
    if (systemID != NULL) {
	if (xmlStrEqual(systemID, XHTML_STRICT_SYSTEM_ID)) return(1);
	if (xmlStrEqual(systemID, XHTML_FRAME_SYSTEM_ID)) return(1);
	if (xmlStrEqual(systemID, XHTML_TRANS_SYSTEM_ID)) return(1);
    }
    return(0);
}

#ifdef LIBXML_OUTPUT_ENABLED

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

struct _xmlSaveCtxt {
    void *_private;
    int type;
    int fd;
    const xmlChar *filename;
    const xmlChar *encoding;
    xmlCharEncodingHandlerPtr handler;
    xmlOutputBufferPtr buf;
    xmlDocPtr doc;
    int options;
    int level;
    int format;
    char indent[MAX_INDENT + 1];	/* array for indenting output */
    int indent_nr;
    int indent_size;
    xmlCharEncodingOutputFunc escape;	/* used for element content */
    xmlCharEncodingOutputFunc escapeAttr;/* used for attribute content */
};

/************************************************************************
 *									*
 * 			Output error handlers				*
 *									*
 ************************************************************************/
/**
 * xmlSaveErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlSaveErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_OUTPUT, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * xmlSaveErr:
 * @code:  the error number
 * @node:  the location of the error.
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlSaveErr(int code, xmlNodePtr node, const char *extra)
{
    const char *msg = NULL;

    switch(code) {
        case XML_SAVE_NOT_UTF8:
	    msg = "string is not in UTF-8";
	    break;
	case XML_SAVE_CHAR_INVALID:
	    msg = "invalid character value";
	    break;
	case XML_SAVE_UNKNOWN_ENCODING:
	    msg = "unknown encoding %s";
	    break;
	case XML_SAVE_NO_DOCTYPE:
	    msg = "document has no DOCTYPE";
	    break;
	default:
	    msg = "unexpected error number";
    }
    __xmlSimpleError(XML_FROM_OUTPUT, code, node, msg, extra);
}

/************************************************************************
 *									*
 *			Special escaping routines			*
 *									*
 ************************************************************************/
static unsigned char *
xmlSerializeHexCharRef(unsigned char *out, int val) {
    unsigned char *ptr;

    *out++ = '&';
    *out++ = '#';
    *out++ = 'x';
    if (val < 0x10) ptr = out;
    else if (val < 0x100) ptr = out + 1;
    else if (val < 0x1000) ptr = out + 2;
    else if (val < 0x10000) ptr = out + 3;
    else if (val < 0x100000) ptr = out + 4;
    else ptr = out + 5;
    out = ptr + 1;
    while (val > 0) {
	switch (val & 0xF) {
	    case 0: *ptr-- = '0'; break;
	    case 1: *ptr-- = '1'; break;
	    case 2: *ptr-- = '2'; break;
	    case 3: *ptr-- = '3'; break;
	    case 4: *ptr-- = '4'; break;
	    case 5: *ptr-- = '5'; break;
	    case 6: *ptr-- = '6'; break;
	    case 7: *ptr-- = '7'; break;
	    case 8: *ptr-- = '8'; break;
	    case 9: *ptr-- = '9'; break;
	    case 0xA: *ptr-- = 'A'; break;
	    case 0xB: *ptr-- = 'B'; break;
	    case 0xC: *ptr-- = 'C'; break;
	    case 0xD: *ptr-- = 'D'; break;
	    case 0xE: *ptr-- = 'E'; break;
	    case 0xF: *ptr-- = 'F'; break;
	    default: *ptr-- = '0'; break;
	}
	val >>= 4;
    }
    *out++ = ';';
    *out = 0;
    return(out);
}

/**
 * xmlEscapeEntities:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of unescaped UTF-8 bytes
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and escape them. Used when there is no
 * encoding specified.
 *
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
static int
xmlEscapeEntities(unsigned char* out, int *outlen,
                 const xmlChar* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    int val;

    inend = in + (*inlen);
    
    while ((in < inend) && (out < outend)) {
    	if (*in == '<') {
	    if (outend - out < 4) break;
	    *out++ = '&';
	    *out++ = 'l';
	    *out++ = 't';
	    *out++ = ';';
	    in++;
	    continue;
	} else if (*in == '>') {
	    if (outend - out < 4) break;
	    *out++ = '&';
	    *out++ = 'g';
	    *out++ = 't';
	    *out++ = ';';
	    in++;
	    continue;
	} else if (*in == '&') {
	    if (outend - out < 5) break;
	    *out++ = '&';
	    *out++ = 'a';
	    *out++ = 'm';
	    *out++ = 'p';
	    *out++ = ';';
	    in++;
	    continue;
	} else if (((*in >= 0x20) && (*in < 0x80)) ||
	           (*in == '\n') || (*in == '\t')) {
	    /*
	     * default case, just copy !
	     */
	    *out++ = *in++;
	    continue;
	} else if (*in >= 0x80) {
	    /*
	     * We assume we have UTF-8 input.
	     */
	    if (outend - out < 10) break;

	    if (*in < 0xC0) {
		xmlSaveErr(XML_SAVE_NOT_UTF8, NULL, NULL);
		in++;
		goto error;
	    } else if (*in < 0xE0) {
		if (inend - in < 2) break;
		val = (in[0]) & 0x1F;
		val <<= 6;
		val |= (in[1]) & 0x3F;
		in += 2;
	    } else if (*in < 0xF0) {
		if (inend - in < 3) break;
		val = (in[0]) & 0x0F;
		val <<= 6;
		val |= (in[1]) & 0x3F;
		val <<= 6;
		val |= (in[2]) & 0x3F;
		in += 3;
	    } else if (*in < 0xF8) {
		if (inend - in < 4) break;
		val = (in[0]) & 0x07;
		val <<= 6;
		val |= (in[1]) & 0x3F;
		val <<= 6;
		val |= (in[2]) & 0x3F;
		val <<= 6;
		val |= (in[3]) & 0x3F;
		in += 4;
	    } else {
		xmlSaveErr(XML_SAVE_CHAR_INVALID, NULL, NULL);
		in++;
		goto error;
	    }
	    if (!IS_CHAR(val)) {
		xmlSaveErr(XML_SAVE_CHAR_INVALID, NULL, NULL);
		in++;
		goto error;
	    }

	    /*
	     * We could do multiple things here. Just save as a char ref
	     */
	    out = xmlSerializeHexCharRef(out, val);
	} else if (IS_BYTE_CHAR(*in)) {
	    if (outend - out < 6) break;
	    out = xmlSerializeHexCharRef(out, *in++);
	} else {
	    xmlGenericError(xmlGenericErrorContext,
		"xmlEscapeEntities : char out of range\n");
	    in++;
	    goto error;
	}
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return(0);
error:
    *outlen = out - outstart;
    *inlen = in - base;
    return(-1);
}

/************************************************************************
 *									*
 *			Allocation and deallocation			*
 *									*
 ************************************************************************/
/**
 * xmlSaveCtxtInit:
 * @ctxt: the saving context
 *
 * Initialize a saving context
 */
static void
xmlSaveCtxtInit(xmlSaveCtxtPtr ctxt)
{
    int i;

    if (ctxt == NULL) return;
    if ((ctxt->encoding == NULL) && (ctxt->escape == NULL))
        ctxt->escape = xmlEscapeEntities;
    if (xmlTreeIndentString == NULL) {
        memset(&ctxt->indent[0], 0, MAX_INDENT + 1);
    } else {
	ctxt->indent_size = xmlStrlen((const xmlChar *) xmlTreeIndentString);
	ctxt->indent_nr = MAX_INDENT / ctxt->indent_size;
	for (i = 0;i < ctxt->indent_nr;i++)
	    memcpy(&ctxt->indent[i * ctxt->indent_size], xmlTreeIndentString,
		   ctxt->indent_size);
        ctxt->indent[ctxt->indent_nr * ctxt->indent_size] = 0;
    }
}

/**
 * xmlFreeSaveCtxt:
 *
 * Free a saving context, destroying the ouptut in any remaining buffer
 */
static void
xmlFreeSaveCtxt(xmlSaveCtxtPtr ctxt)
{
    if (ctxt == NULL) return;
    if (ctxt->encoding != NULL)
        xmlFree((char *) ctxt->encoding);
    if (ctxt->buf != NULL)
        xmlOutputBufferClose(ctxt->buf);
    xmlFree(ctxt);
}

/**
 * xmlNewSaveCtxt:
 *
 * Create a new saving context
 *
 * Returns the new structure or NULL in case of error
 */
static xmlSaveCtxtPtr
xmlNewSaveCtxt(const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = (xmlSaveCtxtPtr) xmlMalloc(sizeof(xmlSaveCtxt));
    if (ret == NULL) {
	xmlSaveErrMemory("creating saving context");
	return ( NULL );
    }
    memset(ret, 0, sizeof(xmlSaveCtxt));
    ret->options = options;
    if (encoding != NULL) {
        ret->handler = xmlFindCharEncodingHandler(encoding);
	if (ret->handler == NULL) {
	    xmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, NULL, encoding);
            xmlFreeSaveCtxt(ret);
	    return(NULL);
	}
        ret->encoding = xmlStrdup((const xmlChar *)encoding);
	ret->escape = xmlEscapeEntities;
    }
    xmlSaveCtxtInit(ret);

    return(ret);
}

/************************************************************************
 *									*
 *   		Dumping XML tree content to a simple buffer		*
 *									*
 ************************************************************************/
/**
 * xmlAttrSerializeContent:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @attr:  the attribute pointer
 *
 * Serialize the attribute in the buffer
 */
static void
xmlAttrSerializeContent(xmlOutputBufferPtr buf, xmlAttrPtr attr)
{
    xmlNodePtr children;

    children = attr->children;
    while (children != NULL) {
        switch (children->type) {
            case XML_TEXT_NODE:
	        xmlAttrSerializeTxtContent(buf->buffer, attr->doc,
		                           attr, children->content);
		break;
            case XML_ENTITY_REF_NODE:
                xmlBufferAdd(buf->buffer, BAD_CAST "&", 1);
                xmlBufferAdd(buf->buffer, children->name,
                             xmlStrlen(children->name));
                xmlBufferAdd(buf->buffer, BAD_CAST ";", 1);
                break;
            default:
                /* should not happen unless we have a badly built tree */
                break;
        }
        children = children->next;
    }
}

/************************************************************************
 *									*
 *   		Dumping XML tree content to an I/O output buffer	*
 *									*
 ************************************************************************/

#ifdef LIBXML_HTML_ENABLED
static void
xhtmlNodeDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur);
#endif
static void xmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur);
static void xmlNodeDumpOutputInternal(xmlSaveCtxtPtr ctxt, xmlNodePtr cur);
void xmlNsListDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur);
static void xmlDocContentDumpOutput(xmlSaveCtxtPtr ctxt, xmlDocPtr cur);

/**
 * xmlNsDumpOutput:
 * @buf:  the XML buffer output
 * @cur:  a namespace
 *
 * Dump a local Namespace definition.
 * Should be called in the context of attributes dumps.
 */
static void
xmlNsDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    if ((cur == NULL) || (buf == NULL)) return;
    if ((cur->type == XML_LOCAL_NAMESPACE) && (cur->href != NULL)) {
	if (xmlStrEqual(cur->prefix, BAD_CAST "xml"))
	    return;

        /* Within the context of an element attributes */
	if (cur->prefix != NULL) {
	    xmlOutputBufferWrite(buf, 7, " xmlns:");
	    xmlOutputBufferWriteString(buf, (const char *)cur->prefix);
	} else
	    xmlOutputBufferWrite(buf, 6, " xmlns");
	xmlOutputBufferWrite(buf, 1, "=");
	xmlBufferWriteQuotedString(buf->buffer, cur->href);
    }
}

/**
 * xmlNsListDumpOutput:
 * @buf:  the XML buffer output
 * @cur:  the first namespace
 *
 * Dump a list of local Namespace definitions.
 * Should be called in the context of attributes dumps.
 */
void
xmlNsListDumpOutput(xmlOutputBufferPtr buf, xmlNsPtr cur) {
    while (cur != NULL) {
        xmlNsDumpOutput(buf, cur);
	cur = cur->next;
    }
}

/**
 * xmlDtdDumpOutput:
 * @buf:  the XML buffer output
 * @dtd:  the pointer to the DTD
 * 
 * Dump the XML document DTD, if any.
 */
static void
xmlDtdDumpOutput(xmlSaveCtxtPtr ctxt, xmlDtdPtr dtd) {
    xmlOutputBufferPtr buf;
    int format, level;
    xmlDocPtr doc;

    if (dtd == NULL) return;
    if ((ctxt == NULL) || (ctxt->buf == NULL))
        return;
    buf = ctxt->buf;
    xmlOutputBufferWrite(buf, 10, "<!DOCTYPE ");
    xmlOutputBufferWriteString(buf, (const char *)dtd->name);
    if (dtd->ExternalID != NULL) {
	xmlOutputBufferWrite(buf, 8, " PUBLIC ");
	xmlBufferWriteQuotedString(buf->buffer, dtd->ExternalID);
	xmlOutputBufferWrite(buf, 1, " ");
	xmlBufferWriteQuotedString(buf->buffer, dtd->SystemID);
    }  else if (dtd->SystemID != NULL) {
	xmlOutputBufferWrite(buf, 8, " SYSTEM ");
	xmlBufferWriteQuotedString(buf->buffer, dtd->SystemID);
    }
    if ((dtd->entities == NULL) && (dtd->elements == NULL) &&
        (dtd->attributes == NULL) && (dtd->notations == NULL) &&
	(dtd->pentities == NULL)) {
	xmlOutputBufferWrite(buf, 1, ">");
	return;
    }
    xmlOutputBufferWrite(buf, 3, " [\n");
    /*
     * Dump the notations first they are not in the DTD children list
     * Do this only on a standalone DTD or on the internal subset though.
     */
    if ((dtd->notations != NULL) && ((dtd->doc == NULL) ||
        (dtd->doc->intSubset == dtd))) {
        xmlDumpNotationTable(buf->buffer, (xmlNotationTablePtr) dtd->notations);
    }
    format = ctxt->format;
    level = ctxt->level;
    doc = ctxt->doc;
    ctxt->format = 0;
    ctxt->level = -1;
    ctxt->doc = dtd->doc;
    xmlNodeListDumpOutput(ctxt, dtd->children);
    ctxt->format = format;
    ctxt->level = level;
    ctxt->doc = doc;
    xmlOutputBufferWrite(buf, 2, "]>");
}

/**
 * xmlAttrDumpOutput:
 * @buf:  the XML buffer output
 * @cur:  the attribute pointer
 *
 * Dump an XML attribute
 */
static void
xmlAttrDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    if (buf == NULL) return;
    xmlOutputBufferWrite(buf, 1, " ");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWrite(buf, 1, ":");
    }
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWrite(buf, 2, "=\"");
    xmlAttrSerializeContent(buf, cur);
    xmlOutputBufferWrite(buf, 1, "\"");
}

/**
 * xmlAttrListDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the document
 * @cur:  the first attribute pointer
 * @encoding:  an optional encoding string
 *
 * Dump a list of XML attributes
 */
static void
xmlAttrListDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    if (cur == NULL) return;
    while (cur != NULL) {
        xmlAttrDumpOutput(ctxt, cur);
	cur = cur->next;
    }
}



/**
 * xmlNodeListDumpOutput:
 * @cur:  the first node
 *
 * Dump an XML node list, recursive behaviour, children are printed too.
 */
static void
xmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    while (cur != NULL) {
	if ((ctxt->format) && (xmlIndentTreeOutput) &&
	    (cur->type == XML_ELEMENT_NODE))
	    xmlOutputBufferWrite(buf, ctxt->indent_size *
	                         (ctxt->level > ctxt->indent_nr ? 
				  ctxt->indent_nr : ctxt->level),
				 ctxt->indent);
        xmlNodeDumpOutputInternal(ctxt, cur);
	if (ctxt->format) {
	    xmlOutputBufferWrite(buf, 1, "\n");
	}
	cur = cur->next;
    }
}

/**
 * xmlNodeDumpOutputInternal:
 * @cur:  the current node
 *
 * Dump an XML node, recursive behaviour, children are printed too.
 */
static void
xmlNodeDumpOutputInternal(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    int format;
    xmlNodePtr tmp;
    xmlChar *start, *end;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    if (cur->type == XML_XINCLUDE_START)
	return;
    if (cur->type == XML_XINCLUDE_END)
	return;
    if ((cur->type == XML_DOCUMENT_NODE) ||
        (cur->type == XML_HTML_DOCUMENT_NODE)) {
	xmlDocContentDumpOutput(ctxt, (xmlDocPtr) cur);
	return;
    }
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(ctxt, (xmlDtdPtr) cur);
	return;
    }
    if (cur->type == XML_DOCUMENT_FRAG_NODE) {
        xmlNodeListDumpOutput(ctxt, cur->children);
	return;
    }
    if (cur->type == XML_ELEMENT_DECL) {
        xmlDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
	return;
    }
    if (cur->type == XML_ATTRIBUTE_DECL) {
        xmlDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
	return;
    }
    if (cur->type == XML_ENTITY_DECL) {
        xmlDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
	return;
    }
    if (cur->type == XML_TEXT_NODE) {
	if (cur->content != NULL) {
	    if ((cur->name == xmlStringText) ||
		(cur->name != xmlStringTextNoenc)) {

                xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
	    } else {
		/*
		 * Disable escaping, needed for XSLT
		 */
		xmlOutputBufferWriteString(buf, (const char *) cur->content);
	    }
	}

	return;
    }
    if (cur->type == XML_PI_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    if (cur->content != NULL) {
		xmlOutputBufferWrite(buf, 1, " ");
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	    xmlOutputBufferWrite(buf, 2, "?>");
	} else {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWrite(buf, 2, "?>");
	}
	return;
    }
    if (cur->type == XML_COMMENT_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWrite(buf, 4, "<!--");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    xmlOutputBufferWrite(buf, 3, "-->");
	}
	return;
    }
    if (cur->type == XML_ENTITY_REF_NODE) {
        xmlOutputBufferWrite(buf, 1, "&");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWrite(buf, 1, ";");
	return;
    }
    if (cur->type == XML_CDATA_SECTION_NODE) {
	start = end = cur->content;
	while (*end != '\0') {
	    if ((*end == ']') && (*(end + 1) == ']') && (*(end + 2) == '>')) {
		end = end + 2;
		xmlOutputBufferWrite(buf, 9, "<![CDATA[");
		xmlOutputBufferWrite(buf, end - start, (const char *)start);
		xmlOutputBufferWrite(buf, 3, "]]>");
		start = end;
	    }
	    end++;
	}
	if (start != end) {
	    xmlOutputBufferWrite(buf, 9, "<![CDATA[");
	    xmlOutputBufferWriteString(buf, (const char *)start);
	    xmlOutputBufferWrite(buf, 3, "]]>");
	}
	return;
    }
    if (cur->type == XML_ATTRIBUTE_NODE) {
	xmlAttrDumpOutput(ctxt, (xmlAttrPtr) cur);
	return;
    }
    if (cur->type == XML_NAMESPACE_DECL) {
	xmlNsDumpOutput(buf, (xmlNsPtr) cur);
	return;
    }

    format = ctxt->format;
    if (format == 1) {
	tmp = cur->children;
	while (tmp != NULL) {
	    if ((tmp->type == XML_TEXT_NODE) ||
		(tmp->type == XML_CDATA_SECTION_NODE) ||
		(tmp->type == XML_ENTITY_REF_NODE)) {
		ctxt->format = 0;
		break;
	    }
	    tmp = tmp->next;
	}
    }
    xmlOutputBufferWrite(buf, 1, "<");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->nsDef)
        xmlNsListDumpOutput(buf, cur->nsDef);
    if (cur->properties != NULL)
        xmlAttrListDumpOutput(ctxt, cur->properties);

    if (((cur->type == XML_ELEMENT_NODE) || (cur->content == NULL)) &&
	(cur->children == NULL) && (!xmlSaveNoEmptyTags)) {
        xmlOutputBufferWrite(buf, 2, "/>");
	ctxt->format = format;
	return;
    }
    xmlOutputBufferWrite(buf, 1, ">");
    if ((cur->type != XML_ELEMENT_NODE) && (cur->content != NULL)) {
	xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
    }
    if (cur->children != NULL) {
	if (ctxt->format) xmlOutputBufferWrite(buf, 1, "\n");
	if (ctxt->level >= 0) ctxt->level++;
	xmlNodeListDumpOutput(ctxt, cur->children);
	if (ctxt->level > 0) ctxt->level--;
	if ((xmlIndentTreeOutput) && (ctxt->format))
	    xmlOutputBufferWrite(buf, ctxt->indent_size *
	                         (ctxt->level > ctxt->indent_nr ? 
				  ctxt->indent_nr : ctxt->level),
				 ctxt->indent);
    }
    xmlOutputBufferWrite(buf, 2, "</");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWrite(buf, 1, ">");
    ctxt->format = format;
}

/**
 * xmlDocContentDumpOutput:
 * @cur:  the document
 *
 * Dump an XML document.
 */
static void
xmlDocContentDumpOutput(xmlSaveCtxtPtr ctxt, xmlDocPtr cur) {
#ifdef LIBXML_HTML_ENABLED
    xmlDtdPtr dtd;
    int is_xhtml = 0;
#endif
    const xmlChar *oldenc = cur->encoding;
    const xmlChar *encoding = ctxt->encoding;
    xmlOutputBufferPtr buf;

    xmlInitParser();

    if (ctxt->encoding != NULL)
        cur->encoding = BAD_CAST ctxt->encoding;

    buf = ctxt->buf;
    xmlOutputBufferWrite(buf, 14, "<?xml version=");
    if (cur->version != NULL) 
	xmlBufferWriteQuotedString(buf->buffer, cur->version);
    else
	xmlOutputBufferWrite(buf, 5, "\"1.0\"");
    if (ctxt->encoding == NULL) {
	if (cur->encoding != NULL)
	    encoding = cur->encoding;
	else if (cur->charset != XML_CHAR_ENCODING_UTF8)
	    encoding = (const xmlChar *)
	         xmlGetCharEncodingName((xmlCharEncoding) cur->charset);
    }
    if (encoding != NULL) {
        xmlOutputBufferWrite(buf, 10, " encoding=");
	xmlBufferWriteQuotedString(buf->buffer, (xmlChar *) encoding);
    }
    switch (cur->standalone) {
        case 0:
	    xmlOutputBufferWrite(buf, 16, " standalone=\"no\"");
	    break;
        case 1:
	    xmlOutputBufferWrite(buf, 17, " standalone=\"yes\"");
	    break;
    }
    xmlOutputBufferWrite(buf, 3, "?>\n");

#ifdef LIBXML_HTML_ENABLED
    dtd = xmlGetIntSubset(cur);
    if (dtd != NULL) {
	is_xhtml = xmlIsXHTML(dtd->SystemID, dtd->ExternalID);
	if (is_xhtml < 0) is_xhtml = 0;
    }
    if (is_xhtml) {
	if (encoding != NULL)
	    htmlSetMetaEncoding(cur, (const xmlChar *) ctxt->encoding);
	else
	    htmlSetMetaEncoding(cur, BAD_CAST "UTF-8");
    }
#endif
    if (cur->children != NULL) {
        xmlNodePtr child = cur->children;

	while (child != NULL) {
	    ctxt->level = 0;
#ifdef LIBXML_HTML_ENABLED
	    if (is_xhtml)
		xhtmlNodeDumpOutput(ctxt, child);
	    else
#endif
		xmlNodeDumpOutputInternal(ctxt, child);
	    xmlOutputBufferWrite(buf, 1, "\n");
	    child = child->next;
	}
    }
    if (ctxt->encoding != NULL)
        cur->encoding = oldenc;
}

#ifdef LIBXML_HTML_ENABLED
/************************************************************************
 *									*
 *		Functions specific to XHTML serialization		*
 *									*
 ************************************************************************/

/**
 * xhtmlIsEmpty:
 * @node:  the node
 *
 * Check if a node is an empty xhtml node
 *
 * Returns 1 if the node is an empty node, 0 if not and -1 in case of error
 */
static int
xhtmlIsEmpty(xmlNodePtr node) {
    if (node == NULL)
	return(-1);
    if (node->type != XML_ELEMENT_NODE)
	return(0);
    if ((node->ns != NULL) && (!xmlStrEqual(node->ns->href, XHTML_NS_NAME)))
	return(0);
    if (node->children != NULL)
	return(0);
    switch (node->name[0]) {
	case 'a':
	    if (xmlStrEqual(node->name, BAD_CAST "area"))
		return(1);
	    return(0);
	case 'b':
	    if (xmlStrEqual(node->name, BAD_CAST "br"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "base"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "basefont"))
		return(1);
	    return(0);
	case 'c':
	    if (xmlStrEqual(node->name, BAD_CAST "col"))
		return(1);
	    return(0);
	case 'f':
	    if (xmlStrEqual(node->name, BAD_CAST "frame"))
		return(1);
	    return(0);
	case 'h':
	    if (xmlStrEqual(node->name, BAD_CAST "hr"))
		return(1);
	    return(0);
	case 'i':
	    if (xmlStrEqual(node->name, BAD_CAST "img"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "input"))
		return(1);
	    if (xmlStrEqual(node->name, BAD_CAST "isindex"))
		return(1);
	    return(0);
	case 'l':
	    if (xmlStrEqual(node->name, BAD_CAST "link"))
		return(1);
	    return(0);
	case 'm':
	    if (xmlStrEqual(node->name, BAD_CAST "meta"))
		return(1);
	    return(0);
	case 'p':
	    if (xmlStrEqual(node->name, BAD_CAST "param"))
		return(1);
	    return(0);
    }
    return(0);
}

/**
 * xhtmlAttrListDumpOutput:
 * @cur:  the first attribute pointer
 *
 * Dump a list of XML attributes
 */
static void
xhtmlAttrListDumpOutput(xmlSaveCtxtPtr ctxt, xmlAttrPtr cur) {
    xmlAttrPtr xml_lang = NULL;
    xmlAttrPtr lang = NULL;
    xmlAttrPtr name = NULL;
    xmlAttrPtr id = NULL;
    xmlNodePtr parent;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    parent = cur->parent;
    while (cur != NULL) {
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "id")))
	    id = cur;
	else
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "name")))
	    name = cur;
	else
	if ((cur->ns == NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")))
	    lang = cur;
	else
	if ((cur->ns != NULL) && (xmlStrEqual(cur->name, BAD_CAST "lang")) &&
	    (xmlStrEqual(cur->ns->prefix, BAD_CAST "xml")))
	    xml_lang = cur;
	else if ((cur->ns == NULL) && 
		 ((cur->children == NULL) ||
		  (cur->children->content == NULL) ||
		  (cur->children->content[0] == 0)) &&
		 (htmlIsBooleanAttr(cur->name))) {
	    if (cur->children != NULL)
		xmlFreeNode(cur->children);
	    cur->children = xmlNewText(cur->name);
	    if (cur->children != NULL)
		cur->children->parent = (xmlNodePtr) cur;
	}
        xmlAttrDumpOutput(ctxt, cur);
	cur = cur->next;
    }
    /*
     * C.8
     */
    if ((name != NULL) && (id == NULL)) {
	if ((parent != NULL) && (parent->name != NULL) &&
	    ((xmlStrEqual(parent->name, BAD_CAST "a")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "p")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "div")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "img")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "map")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "applet")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "form")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "frame")) ||
	     (xmlStrEqual(parent->name, BAD_CAST "iframe")))) {
	    xmlOutputBufferWrite(buf, 5, " id=\"");
	    xmlAttrSerializeContent(buf, name);
	    xmlOutputBufferWrite(buf, 1, "\"");
	}
    }
    /*
     * C.7.
     */
    if ((lang != NULL) && (xml_lang == NULL)) {
	xmlOutputBufferWrite(buf, 11, " xml:lang=\"");
	xmlAttrSerializeContent(buf, lang);
	xmlOutputBufferWrite(buf, 1, "\"");
    } else 
    if ((xml_lang != NULL) && (lang == NULL)) {
	xmlOutputBufferWrite(buf, 7, " lang=\"");
	xmlAttrSerializeContent(buf, xml_lang);
	xmlOutputBufferWrite(buf, 1, "\"");
    }
}

/**
 * xhtmlNodeListDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the XHTML document
 * @cur:  the first node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XML node list, recursive behaviour, children are printed too.
 * Note that @format = 1 provide node indenting only if xmlIndentTreeOutput = 1
 * or xmlKeepBlanksDefault(0) was called
 */
static void
xhtmlNodeListDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    while (cur != NULL) {
	if ((ctxt->format) && (xmlIndentTreeOutput) &&
	    (cur->type == XML_ELEMENT_NODE))
	    xmlOutputBufferWrite(buf, ctxt->indent_size *
	                         (ctxt->level > ctxt->indent_nr ? 
				  ctxt->indent_nr : ctxt->level),
				 ctxt->indent);
        xhtmlNodeDumpOutput(ctxt, cur);
	if (ctxt->format) {
	    xmlOutputBufferWrite(buf, 1, "\n");
	}
	cur = cur->next;
    }
}

/**
 * xhtmlNodeDumpOutput:
 * @buf:  the XML buffer output
 * @doc:  the XHTML document
 * @cur:  the current node
 * @level: the imbrication level for indenting
 * @format: is formatting allowed
 * @encoding:  an optional encoding string
 *
 * Dump an XHTML node, recursive behaviour, children are printed too.
 */
static void
xhtmlNodeDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    int format;
    xmlNodePtr tmp;
    xmlChar *start, *end;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    if (cur->type == XML_XINCLUDE_START)
	return;
    if (cur->type == XML_XINCLUDE_END)
	return;
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(ctxt, (xmlDtdPtr) cur);
	return;
    }
    buf = ctxt->buf;
    if (cur->type == XML_ELEMENT_DECL) {
        xmlDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
	return;
    }
    if (cur->type == XML_ATTRIBUTE_DECL) {
        xmlDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
	return;
    }
    if (cur->type == XML_ENTITY_DECL) {
        xmlDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
	return;
    }
    if (cur->type == XML_TEXT_NODE) {
	if (cur->content != NULL) {
	    if ((cur->name == xmlStringText) ||
		(cur->name != xmlStringTextNoenc)) {
                xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
	    } else {
		/*
		 * Disable escaping, needed for XSLT
		 */
		xmlOutputBufferWriteString(buf, (const char *) cur->content);
	    }
	}

	return;
    }
    if (cur->type == XML_PI_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    if (cur->content != NULL) {
		xmlOutputBufferWrite(buf, 1, " ");
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	    xmlOutputBufferWrite(buf, 2, "?>");
	} else {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWrite(buf, 2, "?>");
	}
	return;
    }
    if (cur->type == XML_COMMENT_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWrite(buf, 4, "<!--");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    xmlOutputBufferWrite(buf, 3, "-->");
	}
	return;
    }
    if (cur->type == XML_ENTITY_REF_NODE) {
        xmlOutputBufferWrite(buf, 1, "&");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWrite(buf, 1, ";");
	return;
    }
    if (cur->type == XML_CDATA_SECTION_NODE) {
	start = end = cur->content;
	while (*end != '\0') {
	    if (*end == ']' && *(end + 1) == ']' && *(end + 2) == '>') {
		end = end + 2;
		xmlOutputBufferWrite(buf, 9, "<![CDATA[");
		xmlOutputBufferWrite(buf, end - start, (const char *)start);
		xmlOutputBufferWrite(buf, 3, "]]>");
		start = end;
	    }
	    end++;
	}
	if (start != end) {
	    xmlOutputBufferWrite(buf, 9, "<![CDATA[");
	    xmlOutputBufferWriteString(buf, (const char *)start);
	    xmlOutputBufferWrite(buf, 3, "]]>");
	}
	return;
    }

    format = ctxt->format;
    if (format == 1) {
	tmp = cur->children;
	while (tmp != NULL) {
	    if ((tmp->type == XML_TEXT_NODE) || 
		(tmp->type == XML_ENTITY_REF_NODE)) {
		format = 0;
		break;
	    }
	    tmp = tmp->next;
	}
    }
    xmlOutputBufferWrite(buf, 1, "<");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputB