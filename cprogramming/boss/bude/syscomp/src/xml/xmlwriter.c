
/*
 * xmlwriter.c: XML text writer implementation
 *
 * For license and disclaimer see the license and disclaimer of
 * libxml2.
 *
 * alfred@mickautsch.de
 */

#define IN_LIBXML
#include <string.h>

#include "libxml.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/uri.h>
#include <libxml/HTMLtree.h>

#ifdef LIBXML_WRITER_ENABLED

#include <libxml/xmlwriter.h>

#define B64LINELEN 72
#define B64CRLF "\r\n"

/*
 * Types are kept private
 */
typedef enum {
    XML_TEXTWRITER_NONE = 0,
    XML_TEXTWRITER_NAME,
    XML_TEXTWRITER_ATTRIBUTE,
    XML_TEXTWRITER_TEXT,
    XML_TEXTWRITER_PI,
    XML_TEXTWRITER_PI_TEXT,
    XML_TEXTWRITER_CDATA,
    XML_TEXTWRITER_DTD,
    XML_TEXTWRITER_DTD_TEXT,
    XML_TEXTWRITER_DTD_ELEM,
    XML_TEXTWRITER_DTD_ELEM_TEXT,
    XML_TEXTWRITER_DTD_ATTL,
    XML_TEXTWRITER_DTD_ATTL_TEXT,
    XML_TEXTWRITER_DTD_ENTY,    /* entity */
    XML_TEXTWRITER_DTD_ENTY_TEXT,
    XML_TEXTWRITER_DTD_PENT,    /* parameter entity */
    XML_TEXTWRITER_COMMENT
} xmlTextWriterState;

typedef struct _xmlTextWriterStackEntry xmlTextWriterStackEntry;

struct _xmlTextWriterStackEntry {
    xmlChar *name;
    xmlTextWriterState state;
};

typedef struct _xmlTextWriterNsStackEntry xmlTextWriterNsStackEntry;
struct _xmlTextWriterNsStackEntry {
    xmlChar *prefix;
    xmlChar *uri;
    xmlLinkPtr elem;
};

struct _xmlTextWriter {
    xmlOutputBufferPtr out;     /* output buffer */
    xmlListPtr nodes;           /* element name stack */
    xmlListPtr nsstack;         /* name spaces stack */
    int level;
    int indent;                 /* enable indent */
    int doindent;               /* internal indent flag */
    xmlChar *ichar;             /* indent character */
    char qchar;                 /* character used for quoting attribute values */
    xmlParserCtxtPtr ctxt;
    int no_doc_free;
};

static void xmlFreeTextWriterStackEntry(xmlLinkPtr lk);
static int xmlCmpTextWriterStackEntry(const void *data0,
                                      const void *data1);
static void xmlFreeTextWriterNsStackEntry(xmlLinkPtr lk);
static int xmlCmpTextWriterNsStackEntry(const void *data0,
                                        const void *data1);
static int xmlTextWriterWriteMemCallback(void *context,
                                         const xmlChar * str, int len);
static int xmlTextWriterCloseMemCallback(void *context);
static int xmlTextWriterWriteDocCallback(void *context,
                                         const xmlChar * str, int len);
static int xmlTextWriterCloseDocCallback(void *context);

static xmlChar *xmlTextWriterVSprintf(const char *format, va_list argptr);
static int xmlOutputBufferWriteBase64(xmlOutputBufferPtr out, int len,
                                      const unsigned char *data);
static void xmlTextWriterStartDocumentCallback(void *ctx);
static int xmlTextWriterWriteIndent(xmlTextWriterPtr writer);
static int
  xmlTextWriterHandleStateDependencies(xmlTextWriterPtr writer,
                                       xmlTextWriterStackEntry * p);

/**
 * xmlWriterErrMsg:
 * @ctxt:  a writer context
 * @error:  the error number
 * @msg:  the error message
 *
 * Handle a writer error
 */
static void
xmlWriterErrMsg(xmlTextWriterPtr ctxt, xmlParserErrors error,
               const char *msg)
{
    if (ctxt != NULL) {
	__xmlRaiseError(NULL, NULL, NULL, ctxt->ctxt,
	            NULL, XML_FROM_WRITER, error, XML_ERR_FATAL,
		    NULL, 0, NULL, NULL, NULL, 0, 0, msg);
    } else {
	__xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_WRITER, error,
                    XML_ERR_FATAL, NULL, 0, NULL, NULL, NULL, 0, 0, msg);
    }
}

/**
 * xmlWriterErrMsgInt:
 * @ctxt:  a writer context
 * @error:  the error number
 * @msg:  the error message
 * @val:  an int
 *
 * Handle a writer error
 */
static void
xmlWriterErrMsgInt(xmlTextWriterPtr ctxt, xmlParserErrors error,
               const char *msg, int val)
{
    if (ctxt != NULL) {
	__xmlRaiseError(NULL, NULL, NULL, ctxt->ctxt,
	            NULL, XML_FROM_WRITER, error, XML_ERR_FATAL,
		    NULL, 0, NULL, NULL, NULL, val, 0, msg, val);
    } else {
	__xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_WRITER, error,
                    XML_ERR_FATAL, NULL, 0, NULL, NULL, NULL, val, 0, msg, val);
    }
}

/**
 * xmlNewTextWriter:
 * @out:  an xmlOutputBufferPtr
 *
 * Create a new xmlNewTextWriter structure using an xmlOutputBufferPtr
 * NOTE: the @out parameter will be deallocated when the writer is closed
 *       (if the call succeed.)
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
xmlTextWriterPtr
xmlNewTextWriter(xmlOutputBufferPtr out)
{
    xmlTextWriterPtr ret;

    ret = (xmlTextWriterPtr) xmlMalloc(sizeof(xmlTextWriter));
    if (ret == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriter : out of memory!\n");
        return NULL;
    }
    memset(ret, 0, (size_t) sizeof(xmlTextWriter));

    ret->nodes = xmlListCreate((xmlListDeallocator)
                               xmlFreeTextWriterStackEntry,
                               (xmlListDataCompare)
                               xmlCmpTextWriterStackEntry);
    if (ret->nodes == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriter : out of memory!\n");
        xmlFree(ret);
        return NULL;
    }

    ret->nsstack = xmlListCreate((xmlListDeallocator)
                                 xmlFreeTextWriterNsStackEntry,
                                 (xmlListDataCompare)
                                 xmlCmpTextWriterNsStackEntry);
    if (ret->nsstack == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriter : out of memory!\n");
        xmlListDelete(ret->nodes);
        xmlFree(ret);
        return NULL;
    }

    ret->out = out;
    ret->ichar = xmlStrdup(BAD_CAST " ");
    ret->qchar = '"';

    if (!ret->ichar) {
        xmlListDelete(ret->nodes);
        xmlListDelete(ret->nsstack);
        xmlFree(ret);
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriter : out of memory!\n");
        return NULL;
    }
    ret->no_doc_free = 0;

    return ret;
}

/**
 * xmlNewTextWriterFilename:
 * @uri:  the URI of the resource for the output
 * @compression:  compress the output?
 *
 * Create a new xmlNewTextWriter structure with @uri as output
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
xmlTextWriterPtr
xmlNewTextWriterFilename(const char *uri, int compression)
{
    xmlTextWriterPtr ret;
    xmlOutputBufferPtr out;

    out = xmlOutputBufferCreateFilename(uri, NULL, compression);
    if (out == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriterFilename : out of memory!\n");
        return NULL;
    }

    ret = xmlNewTextWriter(out);
    if (ret == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriterFilename : out of memory!\n");
        xmlOutputBufferClose(out);
        return NULL;
    }

    ret->indent = 0;
    ret->doindent = 0;
    return ret;
}

/**
 * xmlNewTextWriterMemory:
 * @buf:  xmlBufferPtr
 * @compression:  compress the output?
 *
 * Create a new xmlNewTextWriter structure with @buf as output
 * TODO: handle compression
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
xmlTextWriterPtr
xmlNewTextWriterMemory(xmlBufferPtr buf, int compression ATTRIBUTE_UNUSED)
{
    xmlTextWriterPtr ret;
    xmlOutputBufferPtr out;

/*::todo handle compression */
    out = xmlOutputBufferCreateIO((xmlOutputWriteCallback)
                                  xmlTextWriterWriteMemCallback,
                                  (xmlOutputCloseCallback)
                                  xmlTextWriterCloseMemCallback,
                                  (void *) buf, NULL);
    if (out == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriterMemory : out of memory!\n");
        return NULL;
    }

    ret = xmlNewTextWriter(out);
    if (ret == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_NO_MEMORY,
                        "xmlNewTextWriterMemory : out of memory!\n");
        xmlOutputBufferClose(out);
        return NULL;
    }

    return ret;
}

/**
 * xmlNewTextWriterPushParser:
 * @ctxt: xmlParserCtxtPtr to hold the new XML document tree
 * @compression:  compress the output?
 *
 * Create a new xmlNewTextWriter structure with @ctxt as output
 * NOTE: the @ctxt context will be freed with the resulting writer
 *       (if the call succeeds).
 * TODO: handle compression
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
xmlTextWriterPtr
xmlNewTextWriterPushParser(xmlParserCtxtPtr ctxt,
                           int compression ATTRIBUTE_UNUSED)
{
    xmlTextWriterPtr ret;
    xmlOutputBufferPtr out;

    if (ctxt == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterPushParser : invalid context!\n");
        return NULL;
    }

    out = xmlOutputBufferCreateIO((xmlOutputWriteCallback)
                                  xmlTextWriterWriteDocCallback,
                                  (xmlOutputCloseCallback)
                                  xmlTextWriterCloseDocCallback,
                                  (void *) ctxt, NULL);
    if (out == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterPushParser : error at xmlOutputBufferCreateIO!\n");
        return NULL;
    }

    ret = xmlNewTextWriter(out);
    if (ret == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterPushParser : error at xmlNewTextWriter!\n");
        xmlOutputBufferClose(out);
        return NULL;
    }

    ret->ctxt = ctxt;

    return ret;
}

/**
 * xmlNewTextWriterDoc:
 * @doc: address of a xmlDocPtr to hold the new XML document tree
 * @compression:  compress the output?
 *
 * Create a new xmlNewTextWriter structure with @*doc as output
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
xmlTextWriterPtr
xmlNewTextWriterDoc(xmlDocPtr * doc, int compression)
{
    xmlTextWriterPtr ret;
    xmlSAXHandler saxHandler;
    xmlParserCtxtPtr ctxt;

    memset(&saxHandler, '\0', sizeof(saxHandler));
    xmlSAX2InitDefaultSAXHandler(&saxHandler, 1);
    saxHandler.startDocument = xmlTextWriterStartDocumentCallback;
    saxHandler.startElement = xmlSAX2StartElement;
    saxHandler.endElement = xmlSAX2EndElement;

    ctxt = xmlCreatePushParserCtxt(&saxHandler, NULL, NULL, 0, NULL);
    if (ctxt == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterDoc : error at xmlCreatePushParserCtxt!\n");
        return NULL;
    }
    /*
     * For some reason this seems to completely break if node names
     * are interned.
     */
    ctxt->dictNames = 0;

    ctxt->myDoc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
    if (ctxt->myDoc == NULL) {
        xmlFreeParserCtxt(ctxt);
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterDoc : error at xmlNewDoc!\n");
        return NULL;
    }

    ret = xmlNewTextWriterPushParser(ctxt, compression);
    if (ret == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterDoc : error at xmlNewTextWriterPushParser!\n");
        return NULL;
    }

    xmlSetDocCompressMode(ctxt->myDoc, compression);

    if (doc != NULL) {
        *doc = ctxt->myDoc;
	ret->no_doc_free = 1;
    }

    return ret;
}

/**
 * xmlNewTextWriterTree:
 * @doc: xmlDocPtr
 * @node: xmlNodePtr or NULL for doc->children
 * @compression:  compress the output?
 *
 * Create a new xmlNewTextWriter structure with @doc as output
 * starting at @node
 *
 * Returns the new xmlTextWriterPtr or NULL in case of error
 */
xmlTextWriterPtr
xmlNewTextWriterTree(xmlDocPtr doc, xmlNodePtr node, int compression)
{
    xmlTextWriterPtr ret;
    xmlSAXHandler saxHandler;
    xmlParserCtxtPtr ctxt;

    if (doc == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterTree : invalid document tree!\n");
        return NULL;
    }

    memset(&saxHandler, '\0', sizeof(saxHandler));
    xmlSAX2InitDefaultSAXHandler(&saxHandler, 1);
    saxHandler.startDocument = xmlTextWriterStartDocumentCallback;
    saxHandler.startElement = xmlSAX2StartElement;
    saxHandler.endElement = xmlSAX2EndElement;

    ctxt = xmlCreatePushParserCtxt(&saxHandler, NULL, NULL, 0, NULL);
    if (ctxt == NULL) {
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterDoc : error at xmlCreatePushParserCtxt!\n");
        return NULL;
    }
    /*
     * For some reason this seems to completely break if node names
     * are interned.
     */
    ctxt->dictNames = 0;

    ret = xmlNewTextWriterPushParser(ctxt, compression);
    if (ret == NULL) {
        xmlFreeParserCtxt(ctxt);
        xmlWriterErrMsg(NULL, XML_ERR_INTERNAL_ERROR,
                        "xmlNewTextWriterDoc : error at xmlNewTextWriterPushParser!\n");
        return NULL;
    }

    ctxt->myDoc = doc;
    ctxt->node = node;
    ret->no_doc_free = 1;

    xmlSetDocCompressMode(doc, compression);

    return ret;
}

/**
 * xmlFreeTextWriter:
 * @writer:  the xmlTextWriterPtr
 *
 * Deallocate all the resources associated to the writer
 */
void
xmlFreeTextWriter(xmlTextWriterPtr writer)
{
    if (writer == NULL)
        return;

    if (writer->out != NULL)
        xmlOutputBufferClose(writer->out);

    if (writer->nodes != NULL)
        xmlListDelete(writer->nodes);

    if (writer->nsstack != NULL)
        xmlListDelete(writer->nsstack);

    if (writer->ctxt != NULL) {
        if ((writer->ctxt->myDoc != NULL) && (writer->no_doc_free == 0)) {
	    xmlFreeDoc(writer->ctxt->myDoc);
	    writer->ctxt->myDoc = NULL;
	}
        xmlFreeParserCtxt(writer->ctxt);
    }

    if (writer->ichar != NULL)
        xmlFree(writer->ichar);
    xmlFree(writer);
}

/**
 * xmlTextWriterStartDocument:
 * @writer:  the xmlTextWriterPtr
 * @version:  the xml version ("1.0") or NULL for default ("1.0")
 * @encoding:  the encoding or NULL for default
 * @standalone: "yes" or "no" or NULL for default
 *
 * Start a new xml document
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterStartDocument(xmlTextWriterPtr writer, const char *version,
                           const char *encoding, const char *standalone)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlCharEncodingHandlerPtr encoder;

    if ((writer == NULL) || (writer->out == NULL)) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterStartDocument : invalid writer!\n");
        return -1;
    }

    lk = xmlListFront(writer->nodes);
    if ((lk != NULL) && (xmlLinkGetData(lk) != NULL)) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterStartDocument : not allowed in this context!\n");
        return -1;
    }

    encoder = NULL;
    if (encoding != NULL) {
        encoder = xmlFindCharEncodingHandler(encoding);
        if (encoder == NULL) {
            xmlWriterErrMsg(writer, XML_ERR_NO_MEMORY,
                            "xmlTextWriterStartDocument : out of memory!\n");
            return -1;
        }
    }

    writer->out->encoder = encoder;
    if (encoder != NULL) {
        writer->out->conv = xmlBufferCreateSize(4000);
        xmlCharEncOutFunc(encoder, writer->out->conv, NULL);
    } else
        writer->out->conv = NULL;

    sum = 0;
    count = xmlOutputBufferWriteString(writer->out, "<?xml version=");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
    if (count < 0)
        return -1;
    sum += count;
    if (version != 0)
        count = xmlOutputBufferWriteString(writer->out, version);
    else
        count = xmlOutputBufferWriteString(writer->out, "1.0");
    if (count < 0)
        return -1;
    sum += count;
    count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
    if (count < 0)
        return -1;
    sum += count;
    if (writer->out->encoder != 0) {
        count = xmlOutputBufferWriteString(writer->out, " encoding=");
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
        count =
            xmlOutputBufferWriteString(writer->out,
                                       writer->out->encoder->name);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    if (standalone != 0) {
        count = xmlOutputBufferWriteString(writer->out, " standalone=");
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWriteString(writer->out, standalone);
        if (count < 0)
            return -1;
        sum += count;
        count = xmlOutputBufferWrite(writer->out, 1, &writer->qchar);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "?>\n");
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndDocument:
 * @writer:  the xmlTextWriterPtr
 *
 * End an xml document. All open elements are closed
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterEndDocument(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterEndDocument : invalid writer!\n");
        return -1;
    }

    sum = 0;
    while ((lk = xmlListFront(writer->nodes)) != NULL) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p == 0)
            break;
        switch (p->state) {
            case XML_TEXTWRITER_NAME:
            case XML_TEXTWRITER_ATTRIBUTE:
            case XML_TEXTWRITER_TEXT:
                count = xmlTextWriterEndElement(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_PI:
            case XML_TEXTWRITER_PI_TEXT:
                count = xmlTextWriterEndPI(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_CDATA:
                count = xmlTextWriterEndCDATA(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_DTD:
            case XML_TEXTWRITER_DTD_TEXT:
            case XML_TEXTWRITER_DTD_ELEM:
            case XML_TEXTWRITER_DTD_ELEM_TEXT:
            case XML_TEXTWRITER_DTD_ATTL:
            case XML_TEXTWRITER_DTD_ATTL_TEXT:
            case XML_TEXTWRITER_DTD_ENTY:
            case XML_TEXTWRITER_DTD_ENTY_TEXT:
            case XML_TEXTWRITER_DTD_PENT:
                count = xmlTextWriterEndDTD(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            case XML_TEXTWRITER_COMMENT:
                count = xmlTextWriterEndComment(writer);
                if (count < 0)
                    return -1;
                sum += count;
                break;
            default:
                break;
        }
    }

    if (!writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }
    return sum;
}

/**
 * xmlTextWriterStartComment:
 * @writer:  the xmlTextWriterPtr
 *
 * Start an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterStartComment(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterStartComment : invalid writer!\n");
        return -1;
    }

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_TEXT:
                case XML_TEXTWRITER_NONE:
                    break;
                case XML_TEXTWRITER_NAME:
                    count = xmlOutputBufferWriteString(writer->out, ">");
                    if (count < 0)
                        return -1;
                    sum += count;
                    if (writer->indent) {
                        count =
                            xmlOutputBufferWriteString(writer->out, "\n");
                        if (count < 0)
                            return -1;
                        sum += count;
                    }
                    p->state = XML_TEXTWRITER_TEXT;
                    break;
                default:
                    return -1;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlWriterErrMsg(writer, XML_ERR_NO_MEMORY,
                        "xmlTextWriterStartElement : out of memory!\n");
        return -1;
    }

    p->name = 0;
    p->state = XML_TEXTWRITER_COMMENT;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        if (count < 0)
            return -1;
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<!--");
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterEndComment:
 * @writer:  the xmlTextWriterPtr
 *
 * End the current xml coment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterEndComment(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterEndComment : invalid writer!\n");
        return -1;
    }

    lk = xmlListFront(writer->nodes);
    if (lk == 0) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterEndComment : not allowed in this context!\n");
        return -1;
    }

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_COMMENT:
            count = xmlOutputBufferWriteString(writer->out, "-->");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        if (count < 0)
            return -1;
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterWriteFormatComment:
 * @writer:  the xmlTextWriterPtr
 * @format:  format string (see printf)
 * @...:  extra parameters for the format
 *
 * Write an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterWriteFormatComment(xmlTextWriterPtr writer,
                                const char *format, ...)
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = xmlTextWriterWriteVFormatComment(writer, format, ap);

    va_end(ap);
    return rc;
}

/**
 * xmlTextWriterWriteVFormatComment:
 * @writer:  the xmlTextWriterPtr
 * @format:  format string (see printf)
 * @argptr:  pointer to the first member of the variable argument list.
 *
 * Write an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterWriteVFormatComment(xmlTextWriterPtr writer,
                                 const char *format, va_list argptr)
{
    int rc;
    xmlChar *buf;

    if (writer == NULL) {
        xmlWriterErrMsg(writer, XML_ERR_INTERNAL_ERROR,
                        "xmlTextWriterWriteVFormatComment : invalid writer!\n");
        return -1;
    }

    buf = xmlTextWriterVSprintf(format, argptr);
    if (buf == 0)
        return 0;

    rc = xmlTextWriterWriteComment(writer, buf);

    xmlFree(buf);
    return rc;
}

/**
 * xmlTextWriterWriteComment:
 * @writer:  the xmlTextWriterPtr
 * @content:  comment string
 *
 * Write an xml comment.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterWriteComment(xmlTextWriterPtr writer, const xmlChar * content)
{
    int count;
    int sum;

    sum = 0;
    count = xmlTextWriterStartComment(writer);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterWriteString(writer, content);
    if (count < 0)
        return -1;
    sum += count;
    count = xmlTextWriterEndComment(writer);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartElement:
 * @writer:  the xmlTextWriterPtr
 * @name:  element name
 *
 * Start an xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterStartElement(xmlTextWriterPtr writer, const xmlChar * name)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    sum = 0;
    lk = xmlListFront(writer->nodes);
    if (lk != 0) {
        p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
        if (p != 0) {
            switch (p->state) {
                case XML_TEXTWRITER_PI:
                case XML_TEXTWRITER_PI_TEXT:
                    return -1;
                case XML_TEXTWRITER_NONE:
                    break;
                case XML_TEXTWRITER_NAME:
                    count = xmlOutputBufferWriteString(writer->out, ">");
                    if (count < 0)
                        return -1;
                    sum += count;
                    if (writer->indent)
                        count =
                            xmlOutputBufferWriteString(writer->out, "\n");
                    p->state = XML_TEXTWRITER_TEXT;
                    break;
                default:
                    break;
            }
        }
    }

    p = (xmlTextWriterStackEntry *)
        xmlMalloc(sizeof(xmlTextWriterStackEntry));
    if (p == 0) {
        xmlWriterErrMsg(writer, XML_ERR_NO_MEMORY,
                        "xmlTextWriterStartElement : out of memory!\n");
        return -1;
    }

    p->name = xmlStrdup(name);
    if (p->name == 0) {
        xmlWriterErrMsg(writer, XML_ERR_NO_MEMORY,
                        "xmlTextWriterStartElement : out of memory!\n");
        xmlFree(p);
        return -1;
    }
    p->state = XML_TEXTWRITER_NAME;

    xmlListPushFront(writer->nodes, p);

    if (writer->indent) {
        count = xmlTextWriterWriteIndent(writer);
        sum += count;
    }

    count = xmlOutputBufferWriteString(writer->out, "<");
    if (count < 0)
        return -1;
    sum += count;
    count =
        xmlOutputBufferWriteString(writer->out, (const char *) p->name);
    if (count < 0)
        return -1;
    sum += count;

    return sum;
}

/**
 * xmlTextWriterStartElementNS:
 * @writer:  the xmlTextWriterPtr
 * @prefix:  namespace prefix or NULL
 * @name:  element local name
 * @namespaceURI:  namespace URI or NULL
 *
 * Start an xml element with namespace support.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterStartElementNS(xmlTextWriterPtr writer,
                            const xmlChar * prefix, const xmlChar * name,
                            const xmlChar * namespaceURI)
{
    int count;
    int sum;
    xmlChar *buf;

    if ((writer == NULL) || (name == NULL) || (*name == '\0'))
        return -1;

    buf = 0;
    if (prefix != 0) {
        buf = xmlStrdup(prefix);
        buf = xmlStrcat(buf, BAD_CAST ":");
    }
    buf = xmlStrcat(buf, name);

    sum = 0;
    count = xmlTextWriterStartElement(writer, buf);
    xmlFree(buf);
    if (count < 0)
        return -1;
    sum += count;

    if (namespaceURI != 0) {
        buf = xmlStrdup(BAD_CAST "xmlns");
        if (prefix != 0) {
            buf = xmlStrcat(buf, BAD_CAST ":");
            buf = xmlStrcat(buf, prefix);
        }

        count = xmlTextWriterWriteAttribute(writer, buf, namespaceURI);
        xmlFree(buf);
        if (count < 0)
            return -1;
        sum += count;
    }

    return sum;
}

/**
 * xmlTextWriterEndElement:
 * @writer:  the xmlTextWriterPtr
 *
 * End the current xml element.
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterEndElement(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_ATTRIBUTE:
            count = xmlTextWriterEndAttribute(writer);
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_NAME:
            if (writer->indent) /* next element needs indent */
                writer->doindent = 1;
            count = xmlOutputBufferWriteString(writer->out, "/>");
            if (count < 0)
                return -1;
            sum += count;
            break;
        case XML_TEXTWRITER_TEXT:
            if ((writer->indent) && (writer->doindent)) {
                count = xmlTextWriterWriteIndent(writer);
                sum += count;
                writer->doindent = 1;
            } else
                writer->doindent = 1;
            count = xmlOutputBufferWriteString(writer->out, "</");
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out,
                                               (const char *) p->name);
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            break;
        default:
            return -1;
    }

    if (writer->indent) {
        count = xmlOutputBufferWriteString(writer->out, "\n");
        sum += count;
    }

    xmlListPopFront(writer->nodes);
    return sum;
}

/**
 * xmlTextWriterFullEndElement:
 * @writer:  the xmlTextWriterPtr
 *
 * End the current xml element. Writes an end tag even if the element is empty
 *
 * Returns the bytes written (may be 0 because of buffering) or -1 in case of error
 */
int
xmlTextWriterFullEndElement(xmlTextWriterPtr writer)
{
    int count;
    int sum;
    xmlLinkPtr lk;
    xmlTextWriterStackEntry *p;

    if (writer == NULL)
        return -1;

    lk = xmlListFront(writer->nodes);
    if (lk == 0)
        return -1;

    p = (xmlTextWriterStackEntry *) xmlLinkGetData(lk);
    if (p == 0)
        return -1;

    sum = 0;
    switch (p->state) {
        case XML_TEXTWRITER_ATTRIBUTE:
            count = xmlTextWriterEndAttribute(writer);
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_NAME:
            count = xmlOutputBufferWriteString(writer->out, ">");
            if (count < 0)
                return -1;
            sum += count;
            /* fallthrough */
        case XML_TEXTWRITER_TEXT:
            count = xmlOutputBufferWriteString(writer->out, "</");
            if (count < 0)
                return -1;
            sum += count;
            count = xmlOutputBufferWriteString(writer->out,
                                               (const char *) p->name);
 