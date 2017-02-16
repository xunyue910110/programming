/*
 * parserInternals.c : Internal routines (and obsolete ones) needed for the
 *                     XML and HTML parsers.
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

#include <string.h>
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

#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/valid.h>
#include <libxml/entities.h>
#include <libxml/xmlerror.h>
#include <libxml/encoding.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#include <libxml/dict.h>
#include <libxml/SAX.h>
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxml/globals.h>
#include <libxml/chvalid.h>

/*
 * Various global defaults for parsing
 */

/**
 * xmlCheckVersion:
 * @version: the include version number
 *
 * check the compiled lib version against the include one.
 * This can warn or immediately kill the application
 */
void
xmlCheckVersion(int version) {
    int myversion = (int) LIBXML_VERSION;

    xmlInitParser();

    if ((myversion / 10000) != (version / 10000)) {
	xmlGenericError(xmlGenericErrorContext, 
		"Fatal: program compiled against libxml %d using libxml %d\n",
		(version / 10000), (myversion / 10000));
	fprintf(stderr, 
		"Fatal: program compiled against libxml %d using libxml %d\n",
		(version / 10000), (myversion / 10000));
    }
    if ((myversion / 100) < (version / 100)) {
	xmlGenericError(xmlGenericErrorContext, 
		"Warning: program compiled against libxml %d using older %d\n",
		(version / 100), (myversion / 100));
    }
}


/************************************************************************
 *									*
 * 		Some factorized error routines				*
 *									*
 ************************************************************************/


/**
 * xmlErrMemory:
 * @ctxt:  an XML parser context
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
void
xmlErrMemory(xmlParserCtxtPtr ctxt, const char *extra)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL) {
        ctxt->errNo = XML_ERR_NO_MEMORY;
        ctxt->instate = XML_PARSER_EOF;
        ctxt->disableSAX = 1;
    }
    if (extra)
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
                        NULL, NULL, 0, 0,
                        "Memory allocation failed : %s\n", extra);
    else
        __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_PARSER,
                        XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, NULL,
                        NULL, NULL, 0, 0, "Memory allocation failed\n");
}

/**
 * __xmlErrEncoding:
 * @ctxt:  an XML parser context
 * @xmlerr:  the error number
 * @msg:  the error message
 * @str1:  an string info
 * @str2:  an string info
 *
 * Handle an encoding error
 */
void
__xmlErrEncoding(xmlParserCtxtPtr ctxt, xmlParserErrors xmlerr,
                 const char *msg, const xmlChar * str1, const xmlChar * str2)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
        ctxt->errNo = xmlerr;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, xmlerr, XML_ERR_FATAL,
                    NULL, 0, (const char *) str1, (const char *) str2,
                    NULL, 0, 0, msg, str1, str2);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}

/**
 * xmlErrInternal:
 * @ctxt:  an XML parser context
 * @msg:  the error message
 * @str:  error informations
 *
 * Handle an internal error
 */
static void
xmlErrInternal(xmlParserCtxtPtr ctxt, const char *msg, const xmlChar * str)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
        ctxt->errNo = XML_ERR_INTERNAL_ERROR;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, XML_ERR_INTERNAL_ERROR,
                    XML_ERR_FATAL, NULL, 0, (const char *) str, NULL, NULL,
                    0, 0, msg, str);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}

/**
 * xmlErrEncodingInt:
 * @ctxt:  an XML parser context
 * @error:  the error number
 * @msg:  the error message
 * @val:  an integer value
 *
 * n encoding error
 */
static void
xmlErrEncodingInt(xmlParserCtxtPtr ctxt, xmlParserErrors error,
                  const char *msg, int val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
        ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL,
                    ctxt, NULL, XML_FROM_PARSER, error, XML_ERR_FATAL,
                    NULL, 0, NULL, NULL, NULL, val, 0, msg, val);
    if (ctxt != NULL) {
        ctxt->wellFormed = 0;
        if (ctxt->recovery == 0)
            ctxt->disableSAX = 1;
    }
}

/**
 * xmlIsLetter:
 * @c:  an unicode character (int)
 *
 * Check whether the character is allowed by the production
 * [84] Letter ::= BaseChar | Ideographic
 *
 * Returns 0 if not, non-zero otherwise
 */
int
xmlIsLetter(int c) {
    return(IS_BASECHAR(c) || IS_IDEOGRAPHIC(c));
}

/************************************************************************
 *									*
 * 		Input handling functions for progressive parsing	*
 *									*
 ************************************************************************/

/* #define DEBUG_INPUT */
/* #define DEBUG_STACK */
/* #define DEBUG_PUSH */


/* we need to keep enough input to show errors in context */
#define LINE_LEN        80

#ifdef DEBUG_INPUT
#define CHECK_BUFFER(in) check_buffer(in)

static
void check_buffer(xmlParserInputPtr in) {
    if (in->base != in->buf->buffer->content) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: base mismatch problem\n");
    }
    if (in->cur < in->base) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: cur < base problem\n");
    }
    if (in->cur > in->base + in->buf->buffer->use) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlParserInput: cur > base + use problem\n");
    }
    xmlGenericError(xmlGenericErrorContext,"buffer %x : content %x, cur %d, use %d, size %d\n",
            (int) in, (int) in->buf->buffer->content, in->cur - in->base,
	    in->buf->buffer->use, in->buf->buffer->size);
}

#else
#define CHECK_BUFFER(in) 
#endif


/**
 * xmlParserInputRead:
 * @in:  an XML parser input
 * @len:  an indicative size for the lookahead
 *
 * This function refresh the input for the parser. It doesn't try to
 * preserve pointers to the input buffer, and discard already read data
 *
 * Returns the number of xmlChars read, or -1 in case of error, 0 indicate the
 * end of this entity
 */
int
xmlParserInputRead(xmlParserInputPtr in, int len) {
    int ret;
    int used;
    int indx;

    if (in == NULL) return(-1);
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Read\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);
    if (in->buf->readcallback == NULL) return(-1);

    CHECK_BUFFER(in);

    used = in->cur - in->buf->buffer->content;
    ret = xmlBufferShrink(in->buf->buffer, used);
    if (ret > 0) {
	in->cur -= ret;
	in->consumed += ret;
    }
    ret = xmlParserInputBufferRead(in->buf, len);
    if (in->base != in->buf->buffer->content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = in->buf->buffer->content;
	in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);

    return(ret);
}

/**
 * xmlParserInputGrow:
 * @in:  an XML parser input
 * @len:  an indicative size for the lookahead
 *
 * This function increase the input for the parser. It tries to
 * preserve pointers to the input buffer, and keep already read data
 *
 * Returns the number of xmlChars read, or -1 in case of error, 0 indicate the
 * end of this entity
 */
int
xmlParserInputGrow(xmlParserInputPtr in, int len) {
    int ret;
    int indx;

    if (in == NULL) return(-1);
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Grow\n");
#endif
    if (in->buf == NULL) return(-1);
    if (in->base == NULL) return(-1);
    if (in->cur == NULL) return(-1);
    if (in->buf->buffer == NULL) return(-1);

    CHECK_BUFFER(in);

    indx = in->cur - in->base;
    if (in->buf->buffer->use > (unsigned int) indx + INPUT_CHUNK) {

	CHECK_BUFFER(in);

        return(0);
    }
    if (in->buf->readcallback != NULL)
	ret = xmlParserInputBufferGrow(in->buf, len);
    else	
        return(0);

    /*
     * NOTE : in->base may be a "dangling" i.e. freed pointer in this
     *        block, but we use it really as an integer to do some
     *        pointer arithmetic. Insure will raise it as a bug but in
     *        that specific case, that's not !
     */
    if (in->base != in->buf->buffer->content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = in->buf->buffer->content;
	in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);

    return(ret);
}

/**
 * xmlParserInputShrink:
 * @in:  an XML parser input
 *
 * This function removes used input for the parser.
 */
void
xmlParserInputShrink(xmlParserInputPtr in) {
    int used;
    int ret;
    int indx;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "Shrink\n");
#endif
    if (in == NULL) return;
    if (in->buf == NULL) return;
    if (in->base == NULL) return;
    if (in->cur == NULL) return;
    if (in->buf->buffer == NULL) return;

    CHECK_BUFFER(in);

    used = in->cur - in->buf->buffer->content;
    /*
     * Do not shrink on large buffers whose only a tiny fraction
     * was consumed
     */
    if (used > INPUT_CHUNK) {
	ret = xmlBufferShrink(in->buf->buffer, used - LINE_LEN);
	if (ret > 0) {
	    in->cur -= ret;
	    in->consumed += ret;
	}
	in->end = &in->buf->buffer->content[in->buf->buffer->use];
    }

    CHECK_BUFFER(in);

    if (in->buf->buffer->use > INPUT_CHUNK) {
        return;
    }
    xmlParserInputBufferRead(in->buf, 2 * INPUT_CHUNK);
    if (in->base != in->buf->buffer->content) {
        /*
	 * the buffer has been reallocated
	 */
	indx = in->cur - in->base;
	in->base = in->buf->buffer->content;
	in->cur = &in->buf->buffer->content[indx];
    }
    in->end = &in->buf->buffer->content[in->buf->buffer->use];

    CHECK_BUFFER(in);
}

/************************************************************************
 *									*
 * 		UTF8 character input and related functions		*
 *									*
 ************************************************************************/

/**
 * xmlNextChar:
 * @ctxt:  the XML parser context
 *
 * Skip to the next char input char.
 */

void
xmlNextChar(xmlParserCtxtPtr ctxt)
{
    if ((ctxt == NULL) || (ctxt->instate == XML_PARSER_EOF) ||
        (ctxt->input == NULL))
        return;

    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
        if ((*ctxt->input->cur == 0) &&
            (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0) &&
            (ctxt->instate != XML_PARSER_COMMENT)) {
            /*
             * If we are at the end of the current entity and
             * the context allows it, we pop consumed entities
             * automatically.
             * the auto closing should be blocked in other cases
             */
            xmlPopInput(ctxt);
        } else {
            const unsigned char *cur;
            unsigned char c;

            /*
             *   2.11 End-of-Line Handling
             *   the literal two-character sequence "#xD#xA" or a standalone
             *   literal #xD, an XML processor must pass to the application
             *   the single character #xA.
             */
            if (*(ctxt->input->cur) == '\n') {
                ctxt->input->line++;
                ctxt->input->col = 1;
            } else
                ctxt->input->col++;

            /*
             * We are supposed to handle UTF8, check it's valid
             * From rfc2044: encoding of the Unicode values on UTF-8:
             *
             * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
             * 0000 0000-0000 007F   0xxxxxxx
             * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
             * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
             *
             * Check for the 0x110000 limit too
             */
            cur = ctxt->input->cur;

            c = *cur;
            if (c & 0x80) {
	        if (c == 0xC0)
		    goto encoding_error;
                if (cur[1] == 0)
                    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                if ((cur[1] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xe0) == 0xe0) {
                    unsigned int val;

                    if (cur[2] == 0)
                        xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                    if ((cur[2] & 0xc0) != 0x80)
                        goto encoding_error;
                    if ((c & 0xf0) == 0xf0) {
                        if (cur[3] == 0)
                            xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                        if (((c & 0xf8) != 0xf0) ||
                            ((cur[3] & 0xc0) != 0x80))
                            goto encoding_error;
                        /* 4-byte code */
                        ctxt->input->cur += 4;
                        val = (cur[0] & 0x7) << 18;
                        val |= (cur[1] & 0x3f) << 12;
                        val |= (cur[2] & 0x3f) << 6;
                        val |= cur[3] & 0x3f;
                    } else {
                        /* 3-byte code */
                        ctxt->input->cur += 3;
                        val = (cur[0] & 0xf) << 12;
                        val |= (cur[1] & 0x3f) << 6;
                        val |= cur[2] & 0x3f;
                    }
                    if (((val > 0xd7ff) && (val < 0xe000)) ||
                        ((val > 0xfffd) && (val < 0x10000)) ||
                        (val >= 0x110000)) {
			xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
					  "Char 0x%X out of allowed range\n",
					  val);
                    }
                } else
                    /* 2-byte code */
                    ctxt->input->cur += 2;
            } else
                /* 1-byte code */
                ctxt->input->cur++;

            ctxt->nbChars++;
            if (*ctxt->input->cur == 0)
                xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
        }
    } else {
        /*
         * Assume it's a fixed length encoding (1) with
         * a compatible encoding for the ASCII set, since
         * XML constructs only use < 128 chars
         */

        if (*(ctxt->input->cur) == '\n') {
            ctxt->input->line++;
            ctxt->input->col = 1;
        } else
            ctxt->input->col++;
        ctxt->input->cur++;
        ctxt->nbChars++;
        if (*ctxt->input->cur == 0)
            xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
    }
    if ((*ctxt->input->cur == '%') && (!ctxt->html))
        xmlParserHandlePEReference(ctxt);
    if ((*ctxt->input->cur == 0) &&
        (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0))
        xmlPopInput(ctxt);
    return;
encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    if ((ctxt == NULL) || (ctxt->input == NULL) ||
        (ctxt->input->end - ctxt->input->cur < 4)) {
	__xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
		     "Input is not proper UTF-8, indicate encoding !\n",
		     NULL, NULL);
    } else {
        char buffer[150];

	snprintf(buffer, 149, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			ctxt->input->cur[0], ctxt->input->cur[1],
			ctxt->input->cur[2], ctxt->input->cur[3]);
	__xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
		     "Input is not proper UTF-8, indicate encoding !\n%s",
		     BAD_CAST buffer, NULL);
    }
    ctxt->charset = XML_CHAR_ENCODING_8859_1;
    ctxt->input->cur++;
    return;
}

/**
 * xmlCurrentChar:
 * @ctxt:  the XML parser context
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer. Implement the end of line normalization:
 * 2.11 End-of-Line Handling
 * Wherever an external parsed entity or the literal entity value
 * of an internal parsed entity contains either the literal two-character
 * sequence "#xD#xA" or a standalone literal #xD, an XML processor
 * must pass to the application the single character #xA.
 * This behavior can conveniently be produced by normalizing all
 * line breaks to #xA on input, before parsing.)
 *
 * Returns the current char value and its length
 */

int
xmlCurrentChar(xmlParserCtxtPtr ctxt, int *len) {
    if ((ctxt == NULL) || (len == NULL) || (ctxt->input == NULL)) return(0);
    if (ctxt->instate == XML_PARSER_EOF)
	return(0);

    if ((*ctxt->input->cur >= 0x20) && (*ctxt->input->cur <= 0x7F)) {
	    *len = 1;
	    return((int) *ctxt->input->cur);
    }
    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
	/*
	 * We are supposed to handle UTF8, check it's valid
	 * From rfc2044: encoding of the Unicode values on UTF-8:
	 *
	 * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
	 * 0000 0000-0000 007F   0xxxxxxx
	 * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
	 * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
	 *
	 * Check for the 0x110000 limit too
	 */
	const unsigned char *cur = ctxt->input->cur;
	unsigned char c;
	unsigned int val;

	c = *cur;
	if (c & 0x80) {
	    if (c == 0xC0)
		goto encoding_error;
	    if (cur[1] == 0)
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
	    if ((cur[1] & 0xc0) != 0x80)
		goto encoding_error;
	    if ((c & 0xe0) == 0xe0) {

		if (cur[2] == 0)
		    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
		if ((cur[2] & 0xc0) != 0x80)
		    goto encoding_error;
		if ((c & 0xf0) == 0xf0) {
		    if (cur[3] == 0)
			xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
		    if (((c & 0xf8) != 0xf0) ||
			((cur[3] & 0xc0) != 0x80))
			goto encoding_error;
		    /* 4-byte code */
		    *len = 4;
		    val = (cur[0] & 0x7) << 18;
		    val |= (cur[1] & 0x3f) << 12;
		    val |= (cur[2] & 0x3f) << 6;
		    val |= cur[3] & 0x3f;
		} else {
		  /* 3-byte code */
		    *len = 3;
		    val = (cur[0] & 0xf) << 12;
		    val |= (cur[1] & 0x3f) << 6;
		    val |= cur[2] & 0x3f;
		}
	    } else {
	      /* 2-byte code */
		*len = 2;
		val = (cur[0] & 0x1f) << 6;
		val |= cur[1] & 0x3f;
	    }
	    if (!IS_CHAR(val)) {
	        xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
				  "Char 0x%X out of allowed range\n", val);
	    }    
	    return(val);
	} else {
	    /* 1-byte code */
	    *len = 1;
	    if (*ctxt->input->cur == 0xD) {
		if (ctxt->input->cur[1] == 0xA) {
		    ctxt->nbChars++;
		    ctxt->input->cur++;
		}
		return(0xA);
	    }
	    return((int) *ctxt->input->cur);
	}
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    if (*ctxt->input->cur == 0xD) {
	if (ctxt->input->cur[1] == 0xA) {
	    ctxt->nbChars++;
	    ctxt->input->cur++;
	}
	return(0xA);
    }
    return((int) *ctxt->input->cur);
encoding_error:
    /*
     * An encoding problem may arise from a truncated input buffer
     * splitting a character in the middle. In that case do not raise
     * an error but return 0 to endicate an end of stream problem
     */
    if (ctxt->input->end - ctxt->input->cur < 4) {
	*len = 0;
	return(0);
    }

    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    {
        char buffer[150];

	snprintf(buffer, 149, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			ctxt->input->cur[0], ctxt->input->cur[1],
			ctxt->input->cur[2], ctxt->input->cur[3]);
	__xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
		     "Input is not proper UTF-8, indicate encoding !\n%s",
		     BAD_CAST buffer, NULL);
    }
    ctxt->charset = XML_CHAR_ENCODING_8859_1; 
    *len = 1;
    return((int) *ctxt->input->cur);
}

/**
 * xmlStringCurrentChar:
 * @ctxt:  the XML parser context
 * @cur:  pointer to the beginning of the char
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer.
 *
 * Returns the current char value and its length
 */

int
xmlStringCurrentChar(xmlParserCtxtPtr ctxt, const xmlChar * cur, int *len)
{
    if ((len == NULL) || (cur == NULL)) return(0);
    if ((ctxt == NULL) || (ctxt->charset == XML_CHAR_ENCODING_UTF8)) {
        /*
         * We are supposed to handle UTF8, check it's valid
         * From rfc2044: encoding of the Unicode values on UTF-8:
         *
         * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
         * 0000 0000-0000 007F   0xxxxxxx
         * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
         * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
         *
         * Check for the 0x110000 limit too
         */
        unsigned char c;
        unsigned int val;

        c = *cur;
        if (c & 0x80) {
            if ((cur[1] & 0xc0) != 0x80)
                goto encoding_error;
            if ((c & 0xe0) == 0xe0) {

                if ((cur[2] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xf0) == 0xf0) {
                    if (((c & 0xf8) != 0xf0) || ((cur[3] & 0xc0) != 0x80))
                        goto encoding_error;
                    /* 4-byte code */
                    *len = 4;
                    val = (cur[0] & 0x7) << 18;
                    val |= (cur[1] & 0x3f) << 12;
                    val |= (cur[2] & 0x3f) << 6;
                    val |= cur[3] & 0x3f;
                } else {
                    /* 3-byte code */
                    *len = 3;
                    val = (cur[0] & 0xf) << 12;
                    val |= (cur[1] & 0x3f) << 6;
                    val |= cur[2] & 0x3f;
                }
            } else {
                /* 2-byte code */
                *len = 2;
                val = (cur[0] & 0x1f) << 6;
                val |= cur[1] & 0x3f;
            }
            if (!IS_CHAR(val)) {
	        xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
				  "Char 0x%X out of allowed range\n", val);
            }
            return (val);
        } else {
            /* 1-byte code */
            *len = 1;
            return ((int) *cur);
        }
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    return ((int) *cur);
encoding_error:

    /*
     * An encoding problem may arise from a truncated input buffer
     * splitting a character in the middle. In that case do not raise
     * an error but return 0 to endicate an end of stream problem
     */
    if ((ctxt == NULL) || (ctxt->input == NULL) ||
        (ctxt->input->end - ctxt->input->cur < 4)) {
	*len = 0;
	return(0);
    }
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    {
        char buffer[150];

	snprintf(buffer, 149, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			ctxt->input->cur[0], ctxt->input->cur[1],
			ctxt->input->cur[2], ctxt->input->cur[3]);
	__xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
		     "Input is not proper UTF-8, indicate encoding !\n%s",
		     BAD_CAST buffer, NULL);
    }
    *len = 1;
    return ((int) *cur);
}

/**
 * xmlCopyCharMultiByte:
 * @out:  pointer to an array of xmlChar
 * @val:  the char value
 *
 * append the char value in the array 
 *
 * Returns the number of xmlChar written
 */
int
xmlCopyCharMultiByte(xmlChar *out, int val) {
    if (out == NULL) return(0);
    /*
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
     */
    if  (val >= 0x80) {
	xmlChar *savedout = out;
	int bits;
	if (val <   0x800) { *out++= (val >>  6) | 0xC0;  bits=  0; }
	else if (val < 0x10000) { *out++= (val >> 12) | 0xE0;  bits=  6;}
	else if (val < 0x110000)  { *out++= (val >> 18) | 0xF0;  bits=  12; }
	else {
	    xmlErrEncodingInt(NULL, XML_ERR_INVALID_CHAR,
		    "Internal error, xmlCopyCharMultiByte 0x%X out of bound\n",
			      val);
	    return(0);
	}
	for ( ; bits >= 0; bits-= 6)
	    *out++= ((val >> bits) & 0x3F) | 0x80 ;
	return (out - savedout);
    }
    *out = (xmlChar) val;
    return 1;
}

/**
 * xmlCopyChar:
 * @len:  Ignored, compatibility
 * @out:  pointer to an array of xmlChar
 * @val:  the char value
 *
 * append the char value in the array 
 *
 * Returns the number of xmlChar written
 */

int
xmlCopyChar(int len ATTRIBUTE_UNUSED, xmlChar *out, int val) {
    if (out == NULL) return(0);
    /* the len parameter is ignored */
    if  (val >= 0x80) {
	return(xmlCopyCharMultiByte (out, val));
    }
    *out = (xmlChar) val;
    return 1;
}

/************************************************************************
 *									*
 *		Commodity functions to switch encodings			*
 *									*
 ************************************************************************/

/**
 * xmlSwitchEncoding:
 * @ctxt:  the parser context
 * @enc:  the encoding value (number)
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 */
int
xmlSwitchEncoding(xmlParserCtxtPtr ctxt, xmlCharEncoding enc)
{
    xmlCharEncodingHandlerPtr handler;

    if (ctxt == NULL) return(-1);
    switch (enc) {
	case XML_CHAR_ENCODING_ERROR:
	    __xmlErrEncoding(ctxt, XML_ERR_UNKNOWN_ENCODING,
	                   "encoding unknown\n", NULL, NULL);
	    break;
	case XML_CHAR_ENCODING_NONE:
	    /* let's assume it's UTF-8 without the XML decl */
	    ctxt->charset = XML_CHAR_ENCODING_UTF8;
	    return(0);
	case XML_CHAR_ENCODING_UTF8:
	    /* default encoding, no conversion should be needed */
	    ctxt->charset = XML_CHAR_ENCODING_UTF8;

	    /*
	     * Errata on XML-1.0 June 20 2001
	     * Specific handling of the Byte Order Mark for
	     * UTF-8
	     */
	    if ((ctxt->input != NULL) &&
		(ctxt->input->cur[0] == 0xEF) &&
		(ctxt->input->cur[1] == 0xBB) &&
		(ctxt->input->cur[2] == 0xBF)) {
		ctxt->input->cur += 3;
	    }
	    return(0);
    case XML_CHAR_ENCODING_UTF16LE:
    case XML_CHAR_ENCODING_UTF16BE:
        /*The raw input characters are encoded
         *in UTF-16. As we expect this function
         *to be called after xmlCharEncInFunc, we expect
         *ctxt->input->cur to contain UTF-8 encoded characters.
         *So the raw UTF16 Byte Order Mark
         *has also been converted into
         *an UTF-8 BOM. Let's skip that BOM.
         */
        if ((ctxt->input != NULL) &&
            (ctxt->input->cur[0] == 0xEF) &&
            (ctxt->input->cur[1] == 0xBB) &&
            (ctxt->input->cur[2] == 0xBF)) {
            ctxt->input->cur += 3;
        }
	break ;
	default:
	    break;
    }
    handler = xmlGetCharEncodingHandler(enc);
    if (handler == NULL) {
	/*
	 * Default handlers.
	 */
	switch (enc) {
	    case XML_CHAR_ENCODING_ERROR:
		__xmlErrEncoding(ctxt, XML_ERR_UNKNOWN_ENCODING,
			       "encoding unknown\n", NULL, NULL);
		break;
	    case XML_CHAR_ENCODING_NONE:
		/* let's assume it's UTF-8 without the XML decl */
		ctxt->charset = XML_CHAR_ENCODING_UTF8;
		return(0);
	    case XML_CHAR_ENCODING_UTF8:
	    case XML_CHAR_ENCODING_ASCII:
		/* default encoding, no conversion should be needed */
		ctxt->charset = XML_CHAR_ENCODING_UTF8;
		return(0);
	    case XML_CHAR_ENCODING_UTF16LE:
		break;
	    case XML_CHAR_ENCODING_UTF16BE:
		break;
	    case XML_CHAR_ENCODING_UCS4LE:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "USC4 little endian", NULL);
		break;
	    case XML_CHAR_ENCODING_UCS4BE:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "USC4 big endian", NULL);
		break;
	    case XML_CHAR_ENCODING_EBCDIC:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "EBCDIC", NULL);
		break;
	    case XML_CHAR_ENCODING_UCS4_2143:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "UCS4 2143", NULL);
		break;
	    case XML_CHAR_ENCODING_UCS4_3412:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "UCS4 3412", NULL);
		break;
	    case XML_CHAR_ENCODING_UCS2:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "UCS2", NULL);
		break;
	    case XML_CHAR_ENCODING_8859_1:
	    case XML_CHAR_ENCODING_8859_2:
	    case XML_CHAR_ENCODING_8859_3:
	    case XML_CHAR_ENCODING_8859_4:
	    case XML_CHAR_ENCODING_8859_5:
	    case XML_CHAR_ENCODING_8859_6:
	    case XML_CHAR_ENCODING_8859_7:
	    case XML_CHAR_ENCODING_8859_8:
	    case XML_CHAR_ENCODING_8859_9:
		/*
		 * We used to keep the internal content in the
		 * document encoding however this turns being unmaintainable
		 * So xmlGetCharEncodingHandler() will return non-null
		 * values for this now.
		 */
		if ((ctxt->inputNr == 1) &&
		    (ctxt->encoding == NULL) &&
		    (ctxt->input->encoding != NULL)) {
		    ctxt->encoding = xmlStrdup(ctxt->input->encoding);
		}
		ctxt->charset = enc;
		return(0);
	    case XML_CHAR_ENCODING_2022_JP:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "ISO-2022-JP", NULL);
		break;
	    case XML_CHAR_ENCODING_SHIFT_JIS:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "Shift_JIS", NULL);
		break;
	    case XML_CHAR_ENCODING_EUC_JP:
		__xmlErrEncoding(ctxt, XML_ERR_UNSUPPORTED_ENCODING,
			       "encoding not supported %s\n",
			       BAD_CAST "EUC-JP", NULL);
		break;
	}
    }
    if (handler == NULL)
	return(-1);
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    return(xmlSwitchToEncoding(ctxt, handler));
}

/**
 * xmlSwitchInputEncoding:
 * @ctxt:  the parser context
 * @input:  the input stream
 * @handler:  the encoding handler
 *
 * change the input functions when discovering the character encoding
 * of a given entity.
 *
 * Returns 0 in case of success, -1 otherwise
 */
int
xmlSwitchInputEncoding(xmlParserCtxtPtr ctxt, xmlParserInputPtr input,
                       xmlCharEncodingHandlerPtr handler)
{
    int nbchars;

    if (handler == NULL)
        return (-1);
    if (input == NULL)
        return (-1);
    if (input->buf != NULL) {
        if (input->buf->encoder != NULL) {
            /*
             * Check in case the auto encoding detetection triggered
             * in already.
             */
            if (input->buf->encoder == handler)
                return (0);

            /*
             * "UTF-16" can be us