/*
 * HTMLparser.c : an HTML 4.0 non-verifying parser
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_HTML_ENABLED

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
#include <libxml/xmlerror.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/entities.h>
#include <libxml/encoding.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/globals.h>
#include <libxml/uri.h>

#define HTML_MAX_NAMELEN 1000
#define HTML_PARSER_BIG_BUFFER_SIZE 1000
#define HTML_PARSER_BUFFER_SIZE 100

/* #define DEBUG */
/* #define DEBUG_PUSH */

static int htmlOmittedDefaultValue = 1;

xmlChar * htmlDecodeEntities(htmlParserCtxtPtr ctxt, int len,
			     xmlChar end, xmlChar  end2, xmlChar end3);
static void htmlParseComment(htmlParserCtxtPtr ctxt);

/************************************************************************
 *									*
 * 		Some factorized error routines				*
 *									*
 ************************************************************************/

/**
 * htmlErrMemory:
 * @ctxt:  an HTML parser context
 * @extra:  extra informations
 *
 * Handle a redefinition of attribute error
 */
static void
htmlErrMemory(xmlParserCtxtPtr ctxt, const char *extra)
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
 * htmlParseErr:
 * @ctxt:  an HTML parser context
 * @error:  the error number
 * @msg:  the error message
 * @str1:  string infor
 * @str2:  string infor
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
htmlParseErr(xmlParserCtxtPtr ctxt, xmlParserErrors error,
             const char *msg, const xmlChar *str1, const xmlChar *str2)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
	ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_HTML, error,
                    XML_ERR_ERROR, NULL, 0,
		    (const char *) str1, (const char *) str2,
		    NULL, 0, 0,
		    msg, str1, str2);
    if (ctxt != NULL)
	ctxt->wellFormed = 0;
}

/**
 * htmlParseErrInt:
 * @ctxt:  an HTML parser context
 * @error:  the error number
 * @msg:  the error message
 * @val:  integer info
 *
 * Handle a fatal parser error, i.e. violating Well-Formedness constraints
 */
static void
htmlParseErrInt(xmlParserCtxtPtr ctxt, xmlParserErrors error,
             const char *msg, int val)
{
    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if (ctxt != NULL)
	ctxt->errNo = error;
    __xmlRaiseError(NULL, NULL, NULL, ctxt, NULL, XML_FROM_HTML, error,
                    XML_ERR_ERROR, NULL, 0, NULL, NULL,
		    NULL, val, 0, msg, val);
    if (ctxt != NULL)
	ctxt->wellFormed = 0;
}

/************************************************************************
 *									*
 * 		Parser stacks related functions and macros		*
 *									*
 ************************************************************************/

/**
 * htmlnamePush:
 * @ctxt:  an HTML parser context
 * @value:  the element name
 *
 * Pushes a new element name on top of the name stack
 *
 * Returns 0 in case of error, the index in the stack otherwise
 */
static int
htmlnamePush(htmlParserCtxtPtr ctxt, const xmlChar * value)
{
    if (ctxt->nameNr >= ctxt->nameMax) {
        ctxt->nameMax *= 2;
        ctxt->nameTab = (const xmlChar * *)
                         xmlRealloc((xmlChar * *)ctxt->nameTab,
                                    ctxt->nameMax *
                                    sizeof(ctxt->nameTab[0]));
        if (ctxt->nameTab == NULL) {
            htmlErrMemory(ctxt, NULL);
            return (0);
        }
    }
    ctxt->nameTab[ctxt->nameNr] = value;
    ctxt->name = value;
    return (ctxt->nameNr++);
}
/**
 * htmlnamePop:
 * @ctxt: an HTML parser context
 *
 * Pops the top element name from the name stack
 *
 * Returns the name just removed
 */
static const xmlChar *
htmlnamePop(htmlParserCtxtPtr ctxt)
{
    const xmlChar *ret;

    if (ctxt->nameNr <= 0)
        return (0);
    ctxt->nameNr--;
    if (ctxt->nameNr < 0)
        return (0);
    if (ctxt->nameNr > 0)
        ctxt->name = ctxt->nameTab[ctxt->nameNr - 1];
    else
        ctxt->name = NULL;
    ret = ctxt->nameTab[ctxt->nameNr];
    ctxt->nameTab[ctxt->nameNr] = 0;
    return (ret);
}

/*
 * Macros for accessing the content. Those should be used only by the parser,
 * and not exported.
 *
 * Dirty macros, i.e. one need to make assumption on the context to use them
 *
 *   CUR_PTR return the current pointer to the xmlChar to be parsed.
 *   CUR     returns the current xmlChar value, i.e. a 8 bit value if compiled
 *           in ISO-Latin or UTF-8, and the current 16 bit value if compiled
 *           in UNICODE mode. This should be used internally by the parser
 *           only to compare to ASCII values otherwise it would break when
 *           running with UTF-8 encoding.
 *   NXT(n)  returns the n'th next xmlChar. Same as CUR is should be used only
 *           to compare on ASCII based substring.
 *   UPP(n)  returns the n'th next xmlChar converted to uppercase. Same as CUR
 *           it should be used only to compare on ASCII based substring.
 *   SKIP(n) Skip n xmlChar, and must also be used only to skip ASCII defined
 *           strings without newlines within the parser.
 *
 * Clean macros, not dependent of an ASCII context, expect UTF-8 encoding
 *
 *   CURRENT Returns the current char value, with the full decoding of
 *           UTF-8 if we are using this mode. It returns an int.
 *   NEXT    Skip to the next character, this does the proper decoding
 *           in UTF-8 mode. It also pop-up unfinished entities on the fly.
 *   NEXTL(l) Skip the current unicode character of l xmlChars long.
 *   COPY(to) copy one char to *to, increment CUR_PTR and to accordingly
 */

#define UPPER (toupper(*ctxt->input->cur))

#define SKIP(val) ctxt->nbChars += (val),ctxt->input->cur += (val),ctxt->input->col+=(val)

#define NXT(val) ctxt->input->cur[(val)]

#define UPP(val) (toupper(ctxt->input->cur[(val)]))

#define CUR_PTR ctxt->input->cur

#define SHRINK if ((ctxt->input->cur - ctxt->input->base > 2 * INPUT_CHUNK) && \
		   (ctxt->input->end - ctxt->input->cur < 2 * INPUT_CHUNK)) \
	xmlParserInputShrink(ctxt->input)

#define GROW if ((ctxt->progressive == 0) &&				\
		 (ctxt->input->end - ctxt->input->cur < INPUT_CHUNK))	\
	xmlParserInputGrow(ctxt->input, INPUT_CHUNK)

#define CURRENT ((int) (*ctxt->input->cur))

#define SKIP_BLANKS htmlSkipBlankChars(ctxt)

/* Inported from XML */

/* #define CUR (ctxt->token ? ctxt->token : (int) (*ctxt->input->cur)) */
#define CUR ((int) (*ctxt->input->cur))
#define NEXT xmlNextChar(ctxt)

#define RAW (ctxt->token ? -1 : (*ctxt->input->cur))
#define NXT(val) ctxt->input->cur[(val)]
#define CUR_PTR ctxt->input->cur


#define NEXTL(l) do {							\
    if (*(ctxt->input->cur) == '\n') {					\
	ctxt->input->line++; ctxt->input->col = 1;			\
    } else ctxt->input->col++;						\
    ctxt->token = 0; ctxt->input->cur += l; ctxt->nbChars++;		\
  } while (0)
    
/************
    \
    if (*ctxt->input->cur == '%') xmlParserHandlePEReference(ctxt);	\
    if (*ctxt->input->cur == '&') xmlParserHandleReference(ctxt);
 ************/

#define CUR_CHAR(l) htmlCurrentChar(ctxt, &l)
#define CUR_SCHAR(s, l) xmlStringCurrentChar(ctxt, s, &l)

#define COPY_BUF(l,b,i,v)						\
    if (l == 1) b[i++] = (xmlChar) v;					\
    else i += xmlCopyChar(l,&b[i],v)

/**
 * htmlCurrentChar:
 * @ctxt:  the HTML parser context
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer. Implement the end of line normalization:
 * 2.11 End-of-Line Handling
 * If the encoding is unspecified, in the case we find an ISO-Latin-1
 * char, then the encoding converter is plugged in automatically.
 *
 * Returns the current char value and its length
 */

static int
htmlCurrentChar(xmlParserCtxtPtr ctxt, int *len) {
    if (ctxt->instate == XML_PARSER_EOF)
	return(0);

    if (ctxt->token != 0) {
	*len = 0;
	return(ctxt->token);
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
	        htmlParseErrInt(ctxt, XML_ERR_INVALID_CHAR,
				"Char 0x%X out of allowed range\n", val);
	    }    
	    return(val);
	} else {
	    /* 1-byte code */
	    *len = 1;
	    return((int) *ctxt->input->cur);
	}
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    if ((int) *ctxt->input->cur < 0x80)
	return((int) *ctxt->input->cur);

    /*
     * Humm this is bad, do an automatic flow conversion
     */
    xmlSwitchEncoding(ctxt, XML_CHAR_ENCODING_8859_1);
    ctxt->charset = XML_CHAR_ENCODING_UTF8;
    return(xmlCurrentChar(ctxt, len));

encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertized in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    {
        char buffer[150];

	snprintf(buffer, 149, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			ctxt->input->cur[0], ctxt->input->cur[1],
			ctxt->input->cur[2], ctxt->input->cur[3]);
	htmlParseErr(ctxt, XML_ERR_INVALID_ENCODING,
		     "Input is not proper UTF-8, indicate encoding !\n",
		     BAD_CAST buffer, NULL);
    }

    ctxt->charset = XML_CHAR_ENCODING_8859_1; 
    *len = 1;
    return((int) *ctxt->input->cur);
}

/**
 * htmlSkipBlankChars:
 * @ctxt:  the HTML parser context
 *
 * skip all blanks character found at that point in the input streams.
 *
 * Returns the number of space chars skipped
 */

static int
htmlSkipBlankChars(xmlParserCtxtPtr ctxt) {
    int res = 0;

    while (IS_BLANK_CH(*(ctxt->input->cur))) {
	if ((*ctxt->input->cur == 0) &&
	    (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0)) {
		xmlPopInput(ctxt);
	} else {
	    if (*(ctxt->input->cur) == '\n') {
		ctxt->input->line++; ctxt->input->col = 1;
	    } else ctxt->input->col++;
	    ctxt->input->cur++;
	    ctxt->nbChars++;
	    if (*ctxt->input->cur == 0)
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
	}
	res++;
    }
    return(res);
}



/************************************************************************
 *									*
 * 		The list of HTML elements and their properties		*
 *									*
 ************************************************************************/

/*
 *  Start Tag: 1 means the start tag can be ommited
 *  End Tag:   1 means the end tag can be ommited
 *             2 means it's forbidden (empty elements)
 *             3 means the tag is stylistic and should be closed easily
 *  Depr:      this element is deprecated
 *  DTD:       1 means that this element is valid only in the Loose DTD
 *             2 means that this element is valid only in the Frameset DTD
 *
 * Name,Start Tag,End Tag,Save End,Empty,Deprecated,DTD,inline,Description
	, subElements , impliedsubelt , Attributes, userdata
 */

/* Definitions and a couple of vars for HTML Elements */

#define FONTSTYLE "tt", "i", "b", "u", "s", "strike", "big", "small"
#define NB_FONTSTYLE 8
#define PHRASE "em", "strong", "dfn", "code", "samp", "kbd", "var", "cite", "abbr", "acronym"
#define NB_PHRASE 10
#define SPECIAL "a", "img", "applet", "object", "font", "basefont", "br", "script", "map", "q", "sub", "sup", "span", "bdo", "iframe"
#define NB_SPECIAL 15
#define INLINE PCDATA FONTSTYLE PHRASE SPECIAL FORMCTRL
#define NB_INLINE NB_PCDATA + NB_FONTSTYLE + NB_PHRASE + NB_SPECIAL + NB_FORMCTRL
#define BLOCK HEADING, LIST "pre", "p", "dl", "div", "center", "noscript", "noframes", "blockquote", "form", "isindex", "hr", "table", "fieldset", "address"
#define NB_BLOCK NB_HEADING + NB_LIST + 14
#define FORMCTRL "input", "select", "textarea", "label", "button"
#define NB_FORMCTRL 5
#define PCDATA
#define NB_PCDATA 0
#define HEADING "h1", "h2", "h3", "h4", "h5", "h6"
#define NB_HEADING 6
#define LIST "ul", "ol", "dir", "menu"
#define NB_LIST 4
#define MODIFIER
#define NB_MODIFIER 0
#define FLOW BLOCK,INLINE
#define NB_FLOW NB_BLOCK + NB_INLINE
#define EMPTY NULL


static const char* html_flow[] = { FLOW, NULL } ;
static const char* html_inline[] = { INLINE, NULL } ;

/* placeholders: elts with content but no subelements */
static const char* html_pcdata[] = { NULL } ;
#define html_cdata html_pcdata


/* ... and for HTML Attributes */

#define COREATTRS "id", "class", "style", "title"
#define NB_COREATTRS 4
#define I18N "lang", "dir"
#define NB_I18N 2
#define EVENTS "onclick", "ondblclick", "onmousedown", "onmouseup", "onmouseover", "onmouseout", "onkeypress", "onkeydown", "onkeyup"
#define NB_EVENTS 9
#define ATTRS COREATTRS,I18N,EVENTS
#define NB_ATTRS NB_NB_COREATTRS + NB_I18N + NB_EVENTS
#define CELLHALIGN "align", "char", "charoff"
#define NB_CELLHALIGN 3
#define CELLVALIGN "valign"
#define NB_CELLVALIGN 1

static const char* html_attrs[] = { ATTRS, NULL } ;
static const char* core_i18n_attrs[] = { COREATTRS, I18N, NULL } ;
static const char* core_attrs[] = { COREATTRS, NULL } ;
static const char* i18n_attrs[] = { I18N, NULL } ;


/* Other declarations that should go inline ... */
static const char* a_attrs[] = { ATTRS, "charset", "type", "name",
	"href", "hreflang", "rel", "rev", "accesskey", "shape", "coords",
	"tabindex", "onfocus", "onblur", NULL } ;
static const char* target_attr[] = { "target", NULL } ;
static const char* rows_cols_attr[] = { "rows", "cols", NULL } ;
static const char* alt_attr[] = { "alt", NULL } ;
static const char* src_alt_attrs[] = { "src", "alt", NULL } ;
static const char* href_attrs[] = { "href", NULL } ;
static const char* clear_attrs[] = { "clear", NULL } ;
static const char* inline_p[] = { INLINE, "p", NULL } ;
static const char* flow_param[] = { FLOW, "param", NULL } ;
static const char* applet_attrs[] = { COREATTRS , "codebase",
		"archive", "alt", "name", "height", "width", "align",
		"hspace", "vspace", NULL } ;
static const char* area_attrs[] = { "shape", "coords", "href", "nohref",
	"tabindex", "accesskey", "onfocus", "onblur", NULL } ;
static const char* basefont_attrs[] =
	{ "id", "size", "color", "face", NULL } ;
static const char* quote_attrs[] = { ATTRS, "cite", NULL } ;
static const char* body_contents[] = { FLOW, "ins", "del", NULL } ;
static const char* body_attrs[] = { ATTRS, "onload", "onunload", NULL } ;
static const char* body_depr[] = { "background", "bgcolor", "text",
	"link", "vlink", "alink", NULL } ;
static const char* button_attrs[] = { ATTRS, "name", "value", "type",
	"disabled", "tabindex", "accesskey", "onfocus", "onblur", NULL } ;


static const char* col_attrs[] = { ATTRS, "span", "width", CELLHALIGN, CELLVALIGN, NULL } ;
static const char* col_elt[] = { "col", NULL } ;
static const char* edit_attrs[] = { ATTRS, "datetime", "cite", NULL } ;
static const char* compact_attrs[] = { ATTRS, "compact", NULL } ;
static const char* dl_contents[] = { "dt", "dd", NULL } ;
static const char* compact_attr[] = { "compact", NULL } ;
static const char* label_attr[] = { "label", NULL } ;
static const char* fieldset_contents[] = { FLOW, "legend" } ;
static const char* font_attrs[] = { COREATTRS, I18N, "size", "color", "face" , NULL } ;
static const char* form_contents[] = { HEADING, LIST, INLINE, "pre", "p", "div", "center", "noscript", "noframes", "blockquote", "isindex", "hr", "table", "fieldset", "address", NULL } ;
static const char* form_attrs[] = { ATTRS, "method", "enctype", "accept", "name", "onsubmit", "onreset", "accept-charset", NULL } ;
static const char* frame_attrs[] = { COREATTRS, "longdesc", "name", "src", "frameborder", "marginwidth", "marginheight", "noresize", "scrolling" , NULL } ;
static const char* frameset_attrs[] = { COREATTRS, "rows", "cols", "onload", "onunload", NULL } ;
static const char* frameset_contents[] = { "frameset", "frame", "noframes", NULL } ;
static const char* head_attrs[] = { I18N, "profile", NULL } ;
static const char* head_contents[] = { "title", "isindex", "base", "script", "style", "meta", "link", "object", NULL } ;
static const char* hr_depr[] = { "align", "noshade", "size", "width", NULL } ;
static const char* version_attr[] = { "version", NULL } ;
static const char* html_content[] = { "head", "body", "frameset", NULL } ;
static const char* iframe_attrs[] = { COREATTRS, "longdesc", "name", "src", "frameborder", "marginwidth", "marginheight", "scrolling", "align", "height", "width", NULL } ;
static const char* img_attrs[] = { ATTRS, "longdesc", "name", "height", "width", "usemap", "ismap", NULL } ;
static const char* input_attrs[] = { ATTRS, "type", "name", "value", "checked", "disabled", "readonly", "size", "maxlength", "src", "alt", "usemap", "ismap", "tabindex", "accesskey", "onfocus", "onblur", "onselect", "onchange", "accept", NULL } ;
static const char* prompt_attrs[] = { COREATTRS, I18N, "prompt", NULL } ;
static const char* label_attrs[] = { ATTRS, "for", "accesskey", "onfocus", "onblur", NULL } ;
static const char* legend_attrs[] = { ATTRS, "accesskey", NULL } ;
static const char* align_attr[] = { "align", NULL } ;
static const char* link_attrs[] = { ATTRS, "charset", "href", "hreflang", "type", "rel", "rev", "media", NULL } ;
static const char* map_contents[] = { BLOCK, "area", NULL } ;
static const char* name_attr[] = { "name", NULL } ;
static const char* action_attr[] = { "action", NULL } ;
static const char* blockli_elt[] = { BLOCK, "li", NULL } ;
static const char* meta_attrs[] = { I18N, "http-equiv", "name", "scheme", NULL } ;
static const char* content_attr[] = { "content", NULL } ;
static const char* type_attr[] = { "type", NULL } ;
static const char* noframes_content[] = { "body", FLOW MODIFIER, NULL } ;
static const char* object_contents[] = { FLOW, "param", NULL } ;
static const char* object_attrs[] = { ATTRS, "declare", "classid", "codebase", "data", "type", "codetype", "archive", "standby", "height", "width", "usemap", "name", "tabindex", NULL } ;
static const char* object_depr[] = { "align", "border", "hspace", "vspace", NULL } ;
static const char* ol_attrs[] = { "type", "compact", "start", NULL} ;
static const char* option_elt[] = { "option", NULL } ;
static const char* optgroup_attrs[] = { ATTRS, "disabled", NULL } ;
static const char* option_attrs[] = { ATTRS, "disabled", "label", "selected", "value", NULL } ;
static const char* param_attrs[] = { "id", "value", "valuetype", "type", NULL } ;
static const char* width_attr[] = { "width", NULL } ;
static const char* pre_content[] = { PHRASE, "tt", "i", "b", "u", "s", "strike", "a", "br", "script", "map", "q", "span", "bdo", "iframe", NULL } ;
static const char* script_attrs[] = { "charset", "src", "defer", "event", "for", NULL } ;
static const char* language_attr[] = { "language", NULL } ;
static const char* select_content[] = { "optgroup", "option", NULL } ;
static const char* select_attrs[] = { ATTRS, "name", "size", "multiple", "disabled", "tabindex", "onfocus", "onblur", "onchange", NULL } ;
static const char* style_attrs[] = { I18N, "media", "title", NULL } ;
static const char* table_attrs[] = { ATTRS "summary", "width", "border", "frame", "rules", "cellspacing", "cellpadding", "datapagesize", NULL } ;
static const char* table_depr[] = { "align", "bgcolor", NULL } ;
static const char* table_contents[] = { "caption", "col", "colgroup", "thead", "tfoot", "tbody", "tr", NULL} ;
static const char* tr_elt[] = { "tr", NULL } ;
static const char* talign_attrs[] = { ATTRS, CELLHALIGN, CELLVALIGN, NULL} ;
static const char* th_td_depr[] = { "nowrap", "bgcolor", "width", "height", NULL } ;
static const char* th_td_attr[] = { ATTRS, "abbr", "axis", "headers", "scope", "rowspan", "colspan", CELLHALIGN, CELLVALIGN, NULL } ;
static const char* textarea_attrs[] = { ATTRS, "name", "disabled", "readonly", "tabindex", "accesskey", "onfocus", "onblur", "onselect", "onchange", NULL } ;
static const char* tr_contents[] = { "th", "td", NULL } ;
static const char* bgcolor_attr[] = { "bgcolor", NULL } ;
static const char* li_elt[] = { "li", NULL } ;
static const char* ul_depr[] = { "type", "compact", NULL} ;
static const char* dir_attr[] = { "dir", NULL} ;

#define DECL (const char**)

static const htmlElemDesc
html40ElementTable[] = {
{ "a",		0, 0, 0, 0, 0, 0, 1, "anchor ",
	DECL html_inline , NULL , DECL a_attrs , DECL target_attr, NULL
},
{ "abbr",	0, 0, 0, 0, 0, 0, 1, "abbreviated form",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "acronym",	0, 0, 0, 0, 0, 0, 1, "",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "address",	0, 0, 0, 0, 0, 0, 0, "information on author ",
	DECL inline_p  , NULL , DECL html_attrs, NULL, NULL
},
{ "applet",	0, 0, 0, 0, 1, 1, 2, "java applet ",
	DECL flow_param , NULL , NULL , DECL applet_attrs, NULL
},
{ "area",	0, 2, 2, 1, 0, 0, 0, "client-side image map area ",
	EMPTY ,  NULL , DECL area_attrs , DECL target_attr, DECL alt_attr
},
{ "b",		0, 3, 0, 0, 0, 0, 1, "bold text style",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "base",	0, 2, 2, 1, 0, 0, 0, "document base uri ",
	EMPTY , NULL , NULL , DECL target_attr, DECL href_attrs
},
{ "basefont",	0, 2, 2, 1, 1, 1, 1, "base font size " ,
	EMPTY , NULL , NULL, DECL basefont_attrs, NULL
},
{ "bdo",	0, 0, 0, 0, 0, 0, 1, "i18n bidi over-ride ",
	DECL html_inline , NULL , DECL core_i18n_attrs, NULL, DECL dir_attr
},
{ "big",	0, 3, 0, 0, 0, 0, 1, "large text style",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "blockquote",	0, 0, 0, 0, 0, 0, 0, "long quotation ",
	DECL html_flow , NULL , DECL quote_attrs , NULL, NULL
},
{ "body",	1, 1, 0, 0, 0, 0, 0, "document body ",
	DECL body_contents , "div" , DECL body_attrs, DECL body_depr, NULL
},
{ "br",		0, 2, 2, 1, 0, 0, 1, "forced line break ",
	EMPTY , NULL , DECL core_attrs, DECL clear_attrs , NULL
},
{ "button",	0, 0, 0, 0, 0, 0, 2, "push button ",
	DECL html_flow MODIFIER , NULL , DECL button_attrs, NULL, NULL
},
{ "caption",	0, 0, 0, 0, 0, 0, 0, "table caption ",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "center",	0, 3, 0, 0, 1, 1, 0, "shorthand for div align=center ",
	DECL html_flow , NULL , NULL, DECL html_attrs, NULL
},
{ "cite",	0, 0, 0, 0, 0, 0, 1, "citation",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "code",	0, 0, 0, 0, 0, 0, 1, "computer code fragment",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "col",	0, 2, 2, 1, 0, 0, 0, "table column ",
	EMPTY , NULL , DECL col_attrs , NULL, NULL
},
{ "colgroup",	0, 1, 0, 0, 0, 0, 0, "table column group ",
	DECL col_elt , "col" , DECL col_attrs , NULL, NULL
},
{ "dd",		0, 1, 0, 0, 0, 0, 0, "definition description ",
	DECL html_flow , NULL , DECL html_attrs, NULL, NULL
},
{ "del",	0, 0, 0, 0, 0, 0, 2, "deleted text ",
	DECL html_flow , NULL , DECL edit_attrs , NULL, NULL
},
{ "dfn",	0, 0, 0, 0, 0, 0, 1, "instance definition",
	DECL html_inline , NULL , DECL html_attrs, NULL, NULL
},
{ "dir",	0, 0, 0, 0, 1, 1, 0, "directory list",
	DECL blockli_elt, "li" , NULL, DECL compact_attrs, NULL
},
{ "div",	0, 0, 0, 0, 0, 0, 0, "generic language/style container",
	DECL html_flow, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "dl",		0, 0, 0, 0, 0, 0, 0, "definition list ",
	DECL dl_contents , "dd" , html_attrs, DECL compact_attr, NULL
},
{ "dt",		0, 1, 0, 0, 0, 0, 0, "definition term ",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "em",		0, 3, 0, 0, 0, 0, 1, "emphasis",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "fieldset",	0, 0, 0, 0, 0, 0, 0, "form control group ",
	DECL fieldset_contents , NULL, DECL html_attrs, NULL, NULL
},
{ "font",	0, 3, 0, 0, 1, 1, 1, "local change to font ",
	DECL html_inline, NULL, NULL, DECL font_attrs, NULL
},
{ "form",	0, 0, 0, 0, 0, 0, 0, "interactive form ",
	DECL form_contents, "fieldset", DECL form_attrs , DECL target_attr, DECL action_attr
},
{ "frame",	0, 2, 2, 1, 0, 2, 0, "subwindow " ,
	EMPTY, NULL, NULL, DECL frame_attrs, NULL
},
{ "frameset",	0, 0, 0, 0, 0, 2, 0, "window subdivision" ,
	DECL frameset_contents, "noframes" , NULL , DECL frameset_attrs, NULL
},
{ "h1",		0, 0, 0, 0, 0, 0, 0, "heading ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "h2",		0, 0, 0, 0, 0, 0, 0, "heading ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "h3",		0, 0, 0, 0, 0, 0, 0, "heading ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "h4",		0, 0, 0, 0, 0, 0, 0, "heading ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "h5",		0, 0, 0, 0, 0, 0, 0, "heading ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "h6",		0, 0, 0, 0, 0, 0, 0, "heading ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "head",	1, 1, 0, 0, 0, 0, 0, "document head ",
	DECL head_contents, NULL, DECL head_attrs, NULL, NULL
},
{ "hr",		0, 2, 2, 1, 0, 0, 0, "horizontal rule " ,
	EMPTY, NULL, DECL html_attrs, DECL hr_depr, NULL
},
{ "html",	1, 1, 0, 0, 0, 0, 0, "document root element ",
	DECL html_content , NULL , DECL i18n_attrs, DECL version_attr, NULL
},
{ "i",		0, 3, 0, 0, 0, 0, 1, "italic text style",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "iframe",	0, 0, 0, 0, 0, 1, 2, "inline subwindow ",
	DECL html_flow, NULL, NULL, DECL iframe_attrs, NULL
},
{ "img",	0, 2, 2, 1, 0, 0, 1, "embedded image ",
	EMPTY, NULL, DECL img_attrs, DECL align_attr, src_alt_attrs
},
{ "input",	0, 2, 2, 1, 0, 0, 1, "form control ",
	EMPTY, NULL, DECL input_attrs , DECL align_attr, NULL
},
{ "ins",	0, 0, 0, 0, 0, 0, 2, "inserted text",
	DECL html_flow, NULL, DECL edit_attrs, NULL, NULL
},
{ "isindex",	0, 2, 2, 1, 1, 1, 0, "single line prompt ",
	EMPTY, NULL, NULL, DECL prompt_attrs, NULL
},
{ "kbd",	0, 0, 0, 0, 0, 0, 1, "text to be entered by the user",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "label",	0, 0, 0, 0, 0, 0, 1, "form field label text ",
	DECL html_inline MODIFIER, NULL, DECL label_attrs , NULL, NULL
},
{ "legend",	0, 0, 0, 0, 0, 0, 0, "fieldset legend ",
	DECL html_inline, NULL, DECL legend_attrs , DECL align_attr, NULL
},
{ "li",		0, 1, 1, 0, 0, 0, 0, "list item ",
	DECL html_flow, NULL, DECL html_attrs, NULL, NULL
},
{ "link",	0, 2, 2, 1, 0, 0, 0, "a media-independent link ",
	EMPTY, NULL, DECL link_attrs, DECL target_attr, NULL
},
{ "map",	0, 0, 0, 0, 0, 0, 2, "client-side image map ",
	DECL map_contents , NULL, DECL html_attrs , NULL, name_attr
},
{ "menu",	0, 0, 0, 0, 1, 1, 0, "menu list ",
	DECL blockli_elt , NULL, NULL, DECL compact_attrs, NULL
},
{ "meta",	0, 2, 2, 1, 0, 0, 0, "generic metainformation ",
	EMPTY, NULL, DECL meta_attrs , NULL , DECL content_attr
},
{ "noframes",	0, 0, 0, 0, 0, 2, 0, "alternate content container for non frame-based rendering ",
	DECL noframes_content, "body" , DECL html_attrs, NULL, NULL
},
{ "noscript",	0, 0, 0, 0, 0, 0, 0, "alternate content container for non script-based rendering ",
	DECL html_flow, "div", DECL html_attrs, NULL, NULL
},
{ "object",	0, 0, 0, 0, 0, 0, 2, "generic embedded object ",
	DECL object_contents , "div" , DECL object_attrs, DECL object_depr, NULL
},
{ "ol",		0, 0, 0, 0, 0, 0, 0, "ordered list ",
	DECL li_elt , "li" , DECL html_attrs, DECL ol_attrs, NULL
},
{ "optgroup",	0, 0, 0, 0, 0, 0, 0, "option group ",
	option_elt , "option", DECL optgroup_attrs, NULL, DECL label_attr
},
{ "option",	0, 1, 0, 0, 0, 0, 0, "selectable choice " ,
	DECL html_pcdata, NULL, DECL option_attrs, NULL, NULL
},
{ "p",		0, 1, 0, 0, 0, 0, 0, "paragraph ",
	DECL html_inline, NULL, DECL html_attrs, DECL align_attr, NULL
},
{ "param",	0, 2, 2, 1, 0, 0, 0, "named property value ",
	EMPTY, NULL, DECL param_attrs, NULL, name_attr
},
{ "pre",	0, 0, 0, 0, 0, 0, 0, "preformatted text ",
	DECL pre_content, NULL, DECL html_attrs, DECL width_attr, NULL
},
{ "q",		0, 0, 0, 0, 0, 0, 1, "short inline quotation ",
	DECL html_inline, NULL, DECL quote_attrs, NULL, NULL
},
{ "s",		0, 3, 0, 0, 1, 1, 1, "strike-through text style",
	DECL html_inline, NULL, NULL, DECL html_attrs, NULL
},
{ "samp",	0, 0, 0, 0, 0, 0, 1, "sample program output, scripts, etc.",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "script",	0, 0, 0, 0, 0, 0, 2, "script statements ",
	DECL html_cdata, NULL, DECL script_attrs, DECL language_attr, DECL type_attr
},
{ "select",	0, 0, 0, 0, 0, 0, 1, "option selector ",
	DECL select_content, NULL, DECL select_attrs, NULL, NULL
},
{ "small",	0, 3, 0, 0, 0, 0, 1, "small text style",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "span",	0, 0, 0, 0, 0, 0, 1, "generic language/style container ",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "strike",	0, 3, 0, 0, 1, 1, 1, "strike-through text",
	DECL html_inline, NULL, NULL, DECL html_attrs, NULL
},
{ "strong",	0, 3, 0, 0, 0, 0, 1, "strong emphasis",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "style",	0, 0, 0, 0, 0, 0, 0, "style info ",
	DECL html_cdata, NULL, DECL style_attrs, NULL, DECL type_attr
},
{ "sub",	0, 3, 0, 0, 0, 0, 1, "subscript",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "sup",	0, 3, 0, 0, 0, 0, 1, "superscript ",
	DECL html_inline, NULL, DECL html_attrs, NULL, NULL
},
{ "table",	0, 0, 0, 0, 0, 0, 0, "",
	DECL table_contents , "tr" , DECL table_attrs , DECL table_depr, NULL
},
{ "tbody",	1, 0, 0, 0, 0, 0, 0, "table body ",
	DECL tr_elt , "tr" , DECL talign_attrs, NULL, NULL
},
{ "td",		0, 0, 0, 0, 0, 0, 0, "table data cell",
	DECL html_flow, NULL, DECL th_td_attr, DECL th_td_depr, NULL
},
{ "textarea",	0, 0, 0, 0, 0, 0, 1, "multi-line text field ",
	DECL html_pcdata, NULL, DECL textarea_attrs, NULL, DECL rows_cols_attr
},
{ "tfoot",	0, 1, 0, 0, 0, 0, 0, "table footer ",
	DECL tr_elt , "tr" , DECL talign_attrs, NULL, NULL
},
{ "th",		0, 1, 0, 0, 0, 0, 0, "table header cell",
	DECL html_flow, NULL, DECL th_td_attr, DECL th_td_depr, NULL
},
{ "thead",	0, 1, 0, 0, 0, 0, 0, "table header ",
	DECL tr_elt , "tr" , DECL talign_attrs, NULL, NULL
},
{ "title",	0, 0, 0, 0, 0, 0, 0, "document title ",
	DECL html_pcdata, NULL, DECL i18n_attrs, NULL, NULL
},
{ "tr",		0, 0, 0, 0, 0, 0, 0, "table row ",
	DECL tr_contents , "td" , DECL talign_att