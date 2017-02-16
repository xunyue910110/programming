/*
 * xmllint.c : a small tester program for XML input.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#include "libxml.h"

#include <string.h>
#include <stdarg.h>
#include <assert.h>

#if defined (_WIN32) && !defined(__CYGWIN__)
#if defined (_MSC_VER) || defined(__BORLANDC__)
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define gettimeofday(p1,p2)
#endif /* _MSC_VER */
#endif /* _WIN32 */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef __MINGW32__
#define _WINSOCKAPI_
#include <wsockcompat.h>
#include <winsock2.h>
#undef XML_SOCKLEN_T
#define XML_SOCKLEN_T unsigned int
#endif

#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
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
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
/* seems needed for Solaris */
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#ifdef HAVE_LIBHISTORY
#include <readline/history.h>
#endif
#endif

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/debugXML.h>
#include <libxml/xmlerror.h>
#ifdef LIBXML_XINCLUDE_ENABLED
#include <libxml/xinclude.h>
#endif
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxml/globals.h>
#include <libxml/xmlreader.h>
#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/relaxng.h>
#include <libxml/xmlschemas.h>
#endif
#ifdef LIBXML_PATTERN_ENABLED
#include <libxml/pattern.h>
#endif
#ifdef LIBXML_C14N_ENABLED
#include <libxml/c14n.h>
#endif

#ifndef XML_XML_DEFAULT_CATALOG
#define XML_XML_DEFAULT_CATALOG "file:///etc/xml/catalog"
#endif

typedef enum {
    XMLLINT_RETURN_OK = 0,	/* No error */
    XMLLINT_ERR_UNCLASS,	/* Unclassified */
    XMLLINT_ERR_DTD,		/* Error in DTD */
    XMLLINT_ERR_VALID,		/* Validation error */
    XMLLINT_ERR_RDFILE,		/* CtxtReadFile error */
    XMLLINT_ERR_SCHEMACOMP,	/* Schema compilation */
    XMLLINT_ERR_OUT,		/* Error writing output */
    XMLLINT_ERR_SCHEMAPAT,	/* Error in schema pattern */
    XMLLINT_ERR_RDREGIS,	/* Error in Reader registration */
    XMLLINT_ERR_MEM		/* Out of memory error */
} xmllintReturnCode;
#ifdef LIBXML_DEBUG_ENABLED
static int shell = 0;
static int debugent = 0;
#endif
static int debug = 0;
static int maxmem = 0;
#ifdef LIBXML_TREE_ENABLED
static int copy = 0;
#endif /* LIBXML_TREE_ENABLED */
static int recovery = 0;
static int noent = 0;
static int noblanks = 0;
static int noout = 0;
static int nowrap = 0;
#ifdef LIBXML_OUTPUT_ENABLED
static int format = 0;
static const char *output = NULL;
static int compress = 0;
#endif /* LIBXML_OUTPUT_ENABLED */
#ifdef LIBXML_VALID_ENABLED
static int valid = 0;
static int postvalid = 0;
static char * dtdvalid = NULL;
static char * dtdvalidfpi = NULL;
#endif
#ifdef LIBXML_SCHEMAS_ENABLED
static char * relaxng = NULL;
static xmlRelaxNGPtr relaxngschemas = NULL;
static char * schema = NULL;
static xmlSchemaPtr wxschemas = NULL;
#endif
static int repeat = 0;
static int insert = 0;
#if defined(LIBXML_HTML_ENABLED) || defined(LIBXML_VALID_ENABLED)
static int html = 0;
static int xmlout = 0;
#endif
static int htmlout = 0;
#ifdef LIBXML_PUSH_ENABLED
static int push = 0;
#endif /* LIBXML_PUSH_ENABLED */
#ifdef HAVE_SYS_MMAN_H
static int memory = 0;
#endif
static int testIO = 0;
static char *encoding = NULL;
#ifdef LIBXML_XINCLUDE_ENABLED
static int xinclude = 0;
#endif
static int dtdattrs = 0;
static int loaddtd = 0;
static xmllintReturnCode progresult = XMLLINT_RETURN_OK;
static int timing = 0;
static int generate = 0;
static int dropdtd = 0;
#ifdef LIBXML_CATALOG_ENABLED
static int catalogs = 0;
static int nocatalogs = 0;
#endif
#ifdef LIBXML_C14N_ENABLED
static int canonical = 0;
#endif
#ifdef LIBXML_READER_ENABLED
static int stream = 0;
static int walker = 0;
#endif /* LIBXML_READER_ENABLED */
static int chkregister = 0;
static int nbregister = 0;
#ifdef LIBXML_SAX1_ENABLED
static int sax1 = 0;
#endif /* LIBXML_SAX1_ENABLED */
#ifdef LIBXML_PATTERN_ENABLED
static const char *pattern = NULL;
static xmlPatternPtr patternc = NULL;
#endif
static int options = 0;

/************************************************************************
 *									*
 *		 Entity loading control and customization.		*
 *									*
 ************************************************************************/
#define MAX_PATHS 64
static xmlChar *paths[MAX_PATHS + 1];
static int nbpaths = 0;
static int load_trace = 0;

static
void parsePath(const xmlChar *path) {
    const xmlChar *cur;

    if (path == NULL)
	return;
    while (*path != 0) {
	if (nbpaths >= MAX_PATHS) {
	    fprintf(stderr, "MAX_PATHS reached: too many paths\n");
	    return;
	}
	cur = path;
	while ((*cur == ' ') || (*cur == ':'))
	    cur++;
	path = cur;
	while ((*cur != 0) && (*cur != ' ') && (*cur != ':'))
	    cur++;
	if (cur != path) {
	    paths[nbpaths] = xmlStrndup(path, cur - path);
	    if (paths[nbpaths] != NULL)
		nbpaths++;
	    path = cur;
	}
    }
}

xmlExternalEntityLoader defaultEntityLoader = NULL;

static xmlParserInputPtr 
xmllintExternalEntityLoader(const char *URL, const char *ID,
			     xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr ret;
    warningSAXFunc warning = NULL;

    int i;
    const char *lastsegment = URL;
    const char *iter = URL;

    if (nbpaths > 0) {
	while (*iter != 0) {
	    if (*iter == '/')
		lastsegment = iter + 1;
	    iter++;
	}
    }

    if ((ctxt != NULL) && (ctxt->sax != NULL)) {
	warning = ctxt->sax->warning;
	ctxt->sax->warning = NULL;
    }

    if (defaultEntityLoader != NULL) {
	ret = defaultEntityLoader(URL, ID, ctxt);
	if (ret != NULL) {
	    if (warning != NULL)
		ctxt->sax->warning = warning;
	    if (load_trace) {
		fprintf \
			(stderr,
			 "Loaded URL=\"%s\" ID=\"%s\"\n",
			 URL ? URL : "(null)",
			 ID ? ID : "(null)");
	    }
	    return(ret);
	}
    }
    for (i = 0;i < nbpaths;i++) {
	xmlChar *newURL;

	newURL = xmlStrdup((const xmlChar *) paths[i]);
	newURL = xmlStrcat(newURL, (const xmlChar *) "/");
	newURL = xmlStrcat(newURL, (const xmlChar *) lastsegment);
	if (newURL != NULL) {
	    ret = defaultEntityLoader((const char *)newURL, ID, ctxt);
	    if (ret != NULL) {
		if (warning != NULL)
		    ctxt->sax->warning = warning;
		if (load_trace) {
		    fprintf \
		    	(stderr,
		    	 "Loaded URL=\"%s\" ID=\"%s\"\n",
			 newURL,
		    	 ID ? ID : "(null)");
		}
		xmlFree(newURL);
		return(ret);
	    }
	    xmlFree(newURL);
	}
    }
    if (warning != NULL) {
	ctxt->sax->warning = warning;
	if (URL != NULL)
	    warning(ctxt, "failed to load external entity \"%s\"\n", URL);
	else if (ID != NULL)
	    warning(ctxt, "failed to load external entity \"%s\"\n", ID);
    }
    return(NULL);
}
/************************************************************************
 * 									*
 * Memory allocation consumption debugging				*
 * 									*
 ************************************************************************/

static void
OOM(void)
{
    fprintf(stderr, "Ran out of memory needs > %d bytes\n", maxmem);
    progresult = XMLLINT_ERR_MEM;
}

static void
myFreeFunc(void *mem)
{
    xmlMemFree(mem);
}
static void *
myMallocFunc(size_t size)
{
    void *ret;

    ret = xmlMemMalloc(size);
    if (ret != NULL) {
        if (xmlMemUsed() > maxmem) {
            OOM();
            xmlMemFree(ret);
            return (NULL);
        }
    }
    return (ret);
}
static void *
myReallocFunc(void *mem, size_t size)
{
    void *ret;

    ret = xmlMemRealloc(mem, size);
    if (ret != NULL) {
        if (xmlMemUsed() > maxmem) {
            OOM();
            xmlMemFree(ret);
            return (NULL);
        }
    }
    return (ret);
}
static char *
myStrdupFunc(const char *str)
{
    char *ret;

    ret = xmlMemoryStrdup(str);
    if (ret != NULL) {
        if (xmlMemUsed() > maxmem) {
            OOM();
            xmlFree(ret);
            return (NULL);
        }
    }
    return (ret);
}
/************************************************************************
 * 									*
 * Internal timing routines to remove the necessity to have		*
 * unix-specific function calls.					*
 * 									*
 ************************************************************************/

#ifndef HAVE_GETTIMEOFDAY 
#ifdef HAVE_SYS_TIMEB_H
#ifdef HAVE_SYS_TIME_H
#ifdef HAVE_FTIME

static int
my_gettimeofday(struct timeval *tvp, void *tzp)
{
	struct timeb timebuffer;

	ftime(&timebuffer);
	if (tvp) {
		tvp->tv_sec = timebuffer.time;
		tvp->tv_usec = timebuffer.millitm * 1000L;
	}
	return (0);
}
#define HAVE_GETTIMEOFDAY 1
#define gettimeofday my_gettimeofday

#endif /* HAVE_FTIME */
#endif /* HAVE_SYS_TIME_H */
#endif /* HAVE_SYS_TIMEB_H */
#endif /* !HAVE_GETTIMEOFDAY */

#if defined(HAVE_GETTIMEOFDAY)
static struct timeval begin, end;

/*
 * startTimer: call where you want to start timing
 */
static void
startTimer(void)
{
    gettimeofday(&begin, NULL);
}

/*
 * endTimer: call where you want to stop timing and to print out a
 *           message about the timing performed; format is a printf
 *           type argument
 */
static void
endTimer(const char *fmt, ...)
{
    long msec;
    va_list ap;

    gettimeofday(&end, NULL);
    msec = end.tv_sec - begin.tv_sec;
    msec *= 1000;
    msec += (end.tv_usec - begin.tv_usec) / 1000;

#ifndef HAVE_STDARG_H
#error "endTimer required stdarg functions"
#endif
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, " took %ld ms\n", msec);
}
#elif defined(HAVE_TIME_H)
/*
 * No gettimeofday function, so we have to make do with calling clock.
 * This is obviously less accurate, but there's little we can do about
 * that.
 */
#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 100
#endif

static clock_t begin, end;
static void
startTimer(void)
{
    begin = clock();
}
static void
endTimer(const char *fmt, ...)
{
    long msec;
    va_list ap;

    end = clock();
    msec = ((end - begin) * 1000) / CLOCKS_PER_SEC;

#ifndef HAVE_STDARG_H
#error "endTimer required stdarg functions"
#endif
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, " took %ld ms\n", msec);
}
#else

/*
 * We don't have a gettimeofday or time.h, so we just don't do timing
 */
static void
startTimer(void)
{
    /*
     * Do nothing
     */
}
static void
endTimer(char *format, ...)
{
    /*
     * We cannot do anything because we don't have a timing function
     */
#ifdef HAVE_STDARG_H
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fprintf(stderr, " was not timed\n", msec);
#else
    /* We don't have gettimeofday, time or stdarg.h, what crazy world is
     * this ?!
     */
#endif
}
#endif
/************************************************************************
 * 									*
 * 			HTML ouput					*
 * 									*
 ************************************************************************/
char buffer[50000];

static void
xmlHTMLEncodeSend(void) {
    char *result;

    result = (char *) xmlEncodeEntitiesReentrant(NULL, BAD_CAST buffer);
    if (result) {
	xmlGenericError(xmlGenericErrorContext, "%s", result);
	xmlFree(result);
    }
    buffer[0] = 0;
}

/**
 * xmlHTMLPrintFileInfo:
 * @input:  an xmlParserInputPtr input
 * 
 * Displays the associated file and line informations for the current input
 */

static void
xmlHTMLPrintFileInfo(xmlParserInputPtr input) {
    int len;
    xmlGenericError(xmlGenericErrorContext, "<p>");

    len = strlen(buffer);
    if (input != NULL) {
	if (input->filename) {
	    snprintf(&buffer[len], sizeof(buffer) - len, "%s:%d: ", input->filename,
		    input->line);
	} else {
	    snprintf(&buffer[len], sizeof(buffer) - len, "Entity: line %d: ", input->line);
	}
    }
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLPrintFileContext:
 * @input:  an xmlParserInputPtr input
 * 
 * Displays current context within the input content for error tracking
 */

static void
xmlHTMLPrintFileContext(xmlParserInputPtr input) {
    const xmlChar *cur, *base;
    int len;
    int n;

    if (input == NULL) return;
    xmlGenericError(xmlGenericErrorContext, "<pre>\n");
    cur = input->cur;
    base = input->base;
    while ((cur > base) && ((*cur == '\n') || (*cur == '\r'))) {
	cur--;
    }
    n = 0;
    while ((n++ < 80) && (cur > base) && (*cur != '\n') && (*cur != '\r'))
        cur--;
    if ((*cur == '\n') || (*cur == '\r')) cur++;
    base = cur;
    n = 0;
    while ((*cur != 0) && (*cur != '\n') && (*cur != '\r') && (n < 79)) {
	len = strlen(buffer);
        snprintf(&buffer[len], sizeof(buffer) - len, "%c", 
		    (unsigned char) *cur++);
	n++;
    }
    len = strlen(buffer);
    snprintf(&buffer[len], sizeof(buffer) - len, "\n");
    cur = input->cur;
    while ((*cur == '\n') || (*cur == '\r'))
	cur--;
    n = 0;
    while ((cur != base) && (n++ < 80)) {
	len = strlen(buffer);
        snprintf(&buffer[len], sizeof(buffer) - len, " ");
        base++;
    }
    len = strlen(buffer);
    snprintf(&buffer[len], sizeof(buffer) - len, "^\n");
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</pre>");
}

/**
 * xmlHTMLError:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an error messages, gives file, line, position and
 * extra parameters.
 */
static void
xmlHTMLError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;
    int len;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input != NULL) && (input->filename == NULL) && (ctxt->inputNr > 1)) {
        input = ctxt->inputTab[ctxt->inputNr - 2];
    }
        
    xmlHTMLPrintFileInfo(input);

    xmlGenericError(xmlGenericErrorContext, "<b>error</b>: ");
    va_start(args, msg);
    len = strlen(buffer);
    vsnprintf(&buffer[len],  sizeof(buffer) - len, msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLWarning:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a warning messages, gives file, line, position and
 * extra parameters.
 */
static void
xmlHTMLWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;
    int len;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input != NULL) && (input->filename == NULL) && (ctxt->inputNr > 1)) {
        input = ctxt->inputTab[ctxt->inputNr - 2];
    }
        

    xmlHTMLPrintFileInfo(input);
        
    xmlGenericError(xmlGenericErrorContext, "<b>warning</b>: ");
    va_start(args, msg);
    len = strlen(buffer);    
    vsnprintf(&buffer[len],  sizeof(buffer) - len, msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLValidityError:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an validity error messages, gives file,
 * line, position and extra parameters.
 */
static void
xmlHTMLValidityError(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;
    int len;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input->filename == NULL) && (ctxt->inputNr > 1))
        input = ctxt->inputTab[ctxt->inputNr - 2];
        
    xmlHTMLPrintFileInfo(input);

    xmlGenericError(xmlGenericErrorContext, "<b>validity error</b>: ");
    len = strlen(buffer);
    va_start(args, msg);
    vsnprintf(&buffer[len],  sizeof(buffer) - len, msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/**
 * xmlHTMLValidityWarning:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a validity warning messages, gives file, line,
 * position and extra parameters.
 */
static void
xmlHTMLValidityWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;
    int len;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input->filename == NULL) && (ctxt->inputNr > 1))
        input = ctxt->inputTab[ctxt->inputNr - 2];

    xmlHTMLPrintFileInfo(input);
        
    xmlGenericError(xmlGenericErrorContext, "<b>validity warning</b>: ");
    va_start(args, msg);
    len = strlen(buffer); 
    vsnprintf(&buffer[len],  sizeof(buffer) - len, msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}

/************************************************************************
 * 									*
 * 			Shell Interface					*
 * 									*
 ************************************************************************/
#ifdef LIBXML_DEBUG_ENABLED
#ifdef LIBXML_XPATH_ENABLED
/**
 * xmlShellReadline:
 * @prompt:  the prompt value
 *
 * Read a string
 * 
 * Returns a pointer to it or NULL on EOF the caller is expected to
 *     free the returned string.
 */
static char *
xmlShellReadline(char *prompt) {
#ifdef HAVE_LIBREADLINE
    char *line_read;

    /* Get a line from the user. */
    line_read = readline (prompt);

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
	add_history (line_read);

    return (line_read);
#else
    char line_read[501];
    char *ret;
    int len;

    if (prompt != NULL)
	fprintf(stdout, "%s", prompt);
    if (!fgets(line_read, 500, stdin))
        return(NULL);
    line_read[500] = 0;
    len = strlen(line_read);
    ret = (char *) malloc(len + 1);
    if (ret != NULL) {
	memcpy (ret, line_read, len + 1);
    }
    return(ret);
#endif
}
#endif /* LIBXML_XPATH_ENABLED */
#endif /* LIBXML_DEBUG_ENABLED */

/************************************************************************
 * 									*
 * 			I/O Interfaces					*
 * 									*
 ************************************************************************/

static int myRead(FILE *f, char * buf, int len) {
    return(fread(buf, 1, len, f));
}
static void myClose(FILE *f) {
  if (f != stdin) {
    fclose(f);
  }
}

#ifdef LIBXML_READER_ENABLED
/************************************************************************
 * 									*
 * 			Stream Test processing				*
 * 									*
 ************************************************************************/
static void processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
	name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d", 
	    xmlTextReaderDepth(reader),
	    xmlTextReaderNodeType(reader),
	    name,
	    xmlTextReaderIsEmptyElement(reader),
	    xmlTextReaderHasValue(reader));
    if (value == NULL)
	printf("\n");
    else {
	printf(" %s\n", value);
    }
#ifdef LIBXML_PATTERN_ENABLED
    if (patternc) {
        if (xmlPatternMatch(patternc, xmlTextReaderCurrentNode(reader)) == 1) {
	    printf("Node matches pattern %s\n", pattern);
	}
    }
#endif
}

static void streamFile(char *filename) {
    xmlTextReaderPtr reader;
    int ret;
#ifdef HAVE_SYS_MMAN_H
    int fd = -1;
    struct stat info;
    const char *base = NULL;
    xmlParserInputBufferPtr input = NULL;

    if (memory) {
	if (stat(filename, &info) < 0) 
	    return;
	if ((fd = open(filename, O_RDONLY)) < 0)
	    return;
	base = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, fd, 0) ;
	if (base == (void *) MAP_FAILED)
	    return;

	reader = xmlReaderForMemory(base, info.st_size, filename,
	                            NULL, options);
    } else
#endif
	reader = xmlReaderForFile(filename, NULL, options);


    if (reader != NULL) {
#ifdef LIBXML_VALID_ENABLED
	if (valid)
	    xmlTextReaderSetParserProp(reader, XML_PARSER_VALIDATE, 1);
	else
#endif /* LIBXML_VALID_ENABLED */
	    xmlTextReaderSetParserProp(reader, XML_PARSER_LOADDTD, 1);
#ifdef LIBXML_SCHEMAS_ENABLED
	if (relaxng != NULL) {
	    if ((timing) && (!repeat)) {
		startTimer();
	    }
	    ret = xmlTextReaderRelaxNGValidate(reader, relaxng);
	    if (ret < 0) {
		xmlGenericError(xmlGenericErrorContext,
			"Relax-NG schema %s failed to compile\n", relaxng);
		progresult = XMLLINT_ERR_SCHEMACOMP;
		relaxng = NULL;
	    }
	    if ((timing) && (!repeat)) {
		endTimer("Compiling the schemas");
	    }
	}
#endif

	/*
	 * Process all nodes in sequence
	 */
	if ((timing) && (!repeat)) {
	    startTimer();
	}
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
	    if ((debug)
#ifdef LIBXML_PATTERN_ENABLED
	        || (patternc)
#endif
	       )
		processNode(reader);
	    ret = xmlTextReaderRead(reader);
	}
	if ((timing) && (!repeat)) {
#ifdef LIBXML_SCHEMAS_ENABLED
	    if (relaxng != NULL)
		endTimer("Parsing and validating");
	    else
#endif
#ifdef LIBXML_VALID_ENABLED
	    if (valid)
		endTimer("Parsing and validating");
	    else
#endif
	    endTimer("Parsing");
	}

#ifdef LIBXML_VALID_ENABLED
	if (valid) {
	    if (xmlTextReaderIsValid(reader) != 1) {
		xmlGenericError(xmlGenericErrorContext,
			"Document %s does not validate\n", filename);
		progresult = XMLLINT_ERR_VALID;
	    }
	}
#endif /* LIBXML_VALID_ENABLED */
#ifdef LIBXML_SCHEMAS_ENABLED
	if (relaxng != NULL) {
	    if (xmlTextReaderIsValid(reader) != 1) {
		fprintf(stderr, "%s fails to validate\n", filename);
		progresult = XMLLINT_ERR_VALID;
	    } else {
		fprintf(stderr, "%s validates\n", filename);
	    }
	}
#endif
	/*
	 * Done, cleanup and status
	 */
	xmlFreeTextReader(reader);
	if (ret != 0) {
	    fprintf(stderr, "%s : failed to parse\n", filename);
	    progresult = XMLLINT_ERR_UNCLASS;
	}
    } else {
	fprintf(stderr, "Unable to open %s\n", filename);
	progresult = XMLLINT_ERR_UNCLASS;
    }
#ifdef HAVE_SYS_MMAN_H
    if (memory) {
        xmlFreeParserInputBuffer(input);
	munmap((char *) base, info.st_size);
	close(fd);
    }
#endif
}

static void walkDoc(xmlDocPtr doc) {
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderWalker(doc);
    if (reader != NULL) {
	if ((timing) && (!repeat)) {
	    startTimer();
	}
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
	    if ((debug)
#ifdef LIBXML_PATTERN_ENABLED
	        || (patternc)
#endif
	       )
		processNode(reader);
	    ret = xmlTextReaderRead(reader);
	}
	if ((timing) && (!repeat)) {
	    endTimer("walking through the doc");
	}
	xmlFreeTextReader(reader);
	if (ret != 0) {
	    fprintf(stderr, "failed to walk through the doc\n");
	    progresult = XMLLINT_ERR_UNCLASS;
	}
    } else {
	fprintf(stderr, "Failed to crate a reader from the document\n");
	progresult = XMLLINT_ERR_UNCLASS;
    }
}
#endif /* LIBXML_READER_ENABLED */

/************************************************************************
 * 									*
 * 			Tree Test processing				*
 * 									*
 ************************************************************************/
static void parseAndPrintFile(char *filename, xmlParserCtxtPtr rectxt) {
    xmlDocPtr doc = NULL;
#ifdef LIBXML_TREE_ENABLED
    xmlDocPtr tmp;
#endif /* LIBXML_TREE_ENABLED */

    if ((timing) && (!repeat))
	startTimer();
    

#ifdef LIBXML_TREE_ENABLED
    if (filename == NULL) {
	if (generate) {
	    xmlNodePtr n;

	    doc = xmlNewDoc(BAD_CAST "1.0");
	    n = xmlNewDocNode(doc, NULL, BAD_CAST "info", NULL);
	    xmlNodeSetContent(n, BAD_CAST "abc");
	    xmlDocSetRootElement(doc, n);
	}
    }
#endif /* LIBXML_TREE_ENABLED */
#ifdef LIBXML_HTML_ENABLED
#ifdef LIBXML_PUSH_ENABLED
    else if ((html) && (push)) {
        FILE *f;

#if defined(_WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
	f = fopen(filename, "rb");
#else
	f = fopen(filename, "r");
#endif
        if (f != NULL) {
            int res, size = 3;
            char chars[4096];
            htmlParserCtxtPtr ctxt;

            /* if (repeat) */
                size = 4096;
            res = fread(chars, 1, 4, f);
            if (res > 0) {
                ctxt = htmlCreatePushParserCtxt(NULL, NULL,
                            chars, res, filename, XML_CHAR_ENCODING_NONE);
                while ((res = fread(chars, 1, size, f)) > 0) {
                    htmlParseChunk(ctxt, chars, res, 0);
                }
                htmlParseChunk(ctxt, chars, 0, 1);
                doc = ctxt->myDoc;
                htmlFreeParserCtxt(ctxt);
            }
            fclose(f);
        }
    }
#endif /* LIBXML_PUSH_ENABLED */
    else if (html) {
	doc = htmlReadFile(filename, NULL, options);
    }
#endif /* LIBXML_HTML_ENABLED */
    else {
#ifdef LIBXML_PUSH_ENABLED
	/*
	 * build an XML tree from a string;
	 */
	if (push) {
	    FILE *f;

	    /* '-' Usually means stdin -<sven@zen.org> */
	    if ((filename[0] == '-') && (filename[1] == 0)) {
	      f = stdin;
	    } else {
#if defined(_WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
		f = fopen(filename, "rb");
#else
		f = fopen(filename, "r");
#endif
	    }
	    if (f != NULL) {
		int ret;
	        int res, size = 1024;
	        char chars[1024];
                xmlParserCtxtPtr ctxt;

		/* if (repeat) size = 1024; */
		res = fread(chars, 1, 4, f);
		if (res > 0) {
		    ctxt = xmlCreatePushParserCtxt(NULL, NULL,
		                chars, res, filename);
		    xmlCtxtUseOptions(ctxt, options);
		    while ((res = fread(chars, 1, size, f)) > 0) {
			xmlParseChunk(ctxt, chars, res, 0);
		    }
		    xmlParseChunk(ctxt, chars, 0, 1);
		    doc = ctxt->myDoc;
		    ret = ctxt->wellFormed;
		    xmlFreeParserCtxt(ctxt);
		    if (!ret) {
			xmlFreeDoc(doc);
			doc = NULL;
		    }
	        }
	    }
	} else
#endif /* LIBXML_PUSH_ENABLED */
        if (testIO) {
	    if ((filename[0] == '-') && (filename[1] == 0)) {
	        doc = xmlReadFd(0, NULL, NULL, options);
	    } else {
	        FILE *f;

#if defined(_WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
		f = fopen(filename, "rb");
#else
		f = fopen(filename, "r");
#endif
		if (f != NULL) {
		    if (rectxt == NULL)
			doc = xmlReadIO((xmlInputReadCallback) myRead,
					(xmlInputCloseCallback) myClose, f,
					filename, NULL, options);
		    else
			doc = xmlCtxtReadIO(rectxt,
			                (xmlInputReadCallback) myRead,
					(xmlInputCloseCallback) myClose, f,
					filename, NULL, options);
		} else
		    doc = NULL;
	    }
	} else if (htmlout) {
	    xmlParserCtxtPtr ctxt;

	    if (rectxt == NULL)
		ctxt = xmlNewParserCtxt();
	    else
	        ctxt = rectxt;
	    if (ctxt == NULL) {	      
	        doc = NULL;
	    } else {
	        ctxt->sax->error = xmlHTMLError;
	        ctxt->sax->warning = xmlHTMLWarning;
	        ctxt->vctxt.error = xmlHTMLValidityError;
	        ctxt->vctxt.warning = xmlHTMLValidityWarning;

		doc = xmlCtxtReadFile(ctxt, filename, NULL, options);

		if (rectxt == NULL)
		    xmlFreeParserCtxt(ctxt);
	    }
#ifdef HAVE_SYS_MMAN_H
	} else if (memory) {
	    int fd;
	    struct stat info;
	    const char *base;
	    if (stat(filename, &info) < 0) 
		return;
	    if ((fd = open(filename, O_RDONLY)) < 0)
		return;
	    base = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, fd, 0) ;
	    if (base == (void *) MAP_FAILED)
	        return;

	    if (rectxt == NULL)
		doc = xmlReadMemory((char *) base, info.st_size,
		                    filename, NULL, options);
	    else
		doc = xmlCtxtReadMemory(rectxt, (char *) base, info.st_size,
			                filename, NULL, options);
	        
	    munmap((char *) base, info.st_size);
#endif
#ifdef LIBXML_VALID_ENABLED
	} else if (valid) {
	    xmlParserCtxtPtr ctxt = NULL;

	    if (rectxt == NULL)
		ctxt = xmlNewParserCtxt();
	    else
	        ctxt = rectxt;
	    if (ctxt == NULL) {	      
	        doc = NULL;
	    } else {
		doc = xmlCtxtReadFile(ctxt, filename, NULL, options);

		if (ctxt->valid == 0)
		    progresult = XMLLINT_ERR_RDFILE;
		if (rectxt == NULL)
		    xmlFreeParserCtxt(ctxt);
	    }
#endif /* LIBXML_VALID_ENABLED */
	} else {
	    if (rectxt != NULL)
	        doc = xmlCtxtReadFile(rectxt, filename, NULL, options);
	    else
		doc = xmlReadFile(filename, NULL, options);
	}
    }

    /*
     * If we don't have a document we might as well give up.  Do we
     * want an error message here?  <sven@zen.org> */
    if (doc == NULL) {
	progresult = XMLLINT_ERR_UNCLASS;
	return;
    }

    if ((timing) && (!repeat)) {
	endTimer("Parsing");
    }

    /*
     * Remove DOCTYPE nodes
     */
    if (dropdtd) {
	xmlDtdPtr dtd;

	dtd = xmlGetIntSubset(doc);
	if (dtd != NULL) {
	    xmlUnlinkNode((xmlNodePtr)dtd);
	    xmlFreeDtd(dtd);
	}
    }

#ifdef LIBXML_XINCLUDE_ENABLED
    if (xinclude) {
	if ((timing) && (!repeat)) {
	    startTimer();
	}
	xmlXIncludeProcessFlags(doc, options);
	if ((timing) && (!repeat)) {
	    endTimer("Xinclude processing");
	}
    }
#endif

#ifdef LIBXML_DEBUG_ENABLED
#ifdef LIBXML_XPATH_ENABLED
    /*
     * shell interaction
     */
    if (shell)  
        xmlShell(doc, filename, xmlShellReadline, stdout);
#endif
#endif

#ifdef LIBXML_TREE_ENABLED
    /*
     * test intermediate copy if needed.
     */
    if (copy) {
        tmp = doc;
	if (timing) {
	    startTimer();
	}
	doc = xmlCopyDoc(doc, 1);
	if (timing) {
	    endTimer("Copying");
	}
	if (timing) {
	    startTimer();
	}
	xmlFreeDoc(tmp);
	if (timing) {
	    endTimer("Freeing original");
	}
    }
#endif /* LIBXML_TREE_ENABLED */

#ifdef LIBXML_VALID_ENABLED
    if ((insert) && (!html)) {
        const xmlChar* list[256];
	int nb, i;
	xmlNodePtr node;

	if (doc->children != NULL) {
	    node = doc->children;
	    while ((node != NULL) && (node->last == NULL)) node = node->next;
	    if (node != NULL) {
		nb = xmlValidGetValidElements(node->last, NULL, list, 256);
		if (nb < 0) {
		    fprintf(stderr, "could not get valid list of elements\n");
		} else if (nb == 0) {
		    fprintf(stderr, "No element can be inserted under root\n");
		} else {
		    fprintf(stderr, "%d element types can be inserted under root:\n",
		           nb);
		    for (i = 0;i < nb;i++) {
			 fprintf(stderr, "%s\n", (char *) list[i]);
		    }
		}
	    }
	}    
    }else
#endif /* LIBXML_VALID_ENABLED */
#ifdef LIBXML_READER_ENABLED
    if (walker) {
        walkDoc(doc);
    }
#endif /* LIBXML_READER_ENABLED */
#ifdef LIBXML_OUTPUT_ENABLED
    if (noout == 0) {
        int ret;

	/*
	 * print it.
	 */
#ifdef LIBXML_DEBUG_ENABLED
	if (!debug) {
#endif
	    if ((timing) && (!repeat)) {
		startTimer();
	    }
#ifdef LIBXML_HTML_ENABLED
            if ((html) && (!xmlout)) {
		if (compress) {
		    htmlSaveFile(output ? output : "-", doc);
		}
		else if (encoding != NULL) {
		    if ( format ) {
			htmlSaveFileFormat(output ? output : "-", doc, encoding, 1);
		    }
		    else {
			htmlSaveFileFormat(output ? output : "-", doc, encoding, 0);
		    }
		}
		else if (format) {
		    htmlSaveFileFormat(output ? output : "-", doc, NULL, 1);
		}
		else {
		    FILE *out;
		    if (output == NULL)
			out = stdout;
		    else {
			out = fopen(output,"wb");
		    }
		    if (out != NULL) {
			if (htmlDocDump(out, doc) < 0)
			    progresult = XMLLINT_ERR_OUT;

			if (output != NULL)
			    fclose(out);
		    } else {
			fprintf(stderr, "failed to open %s\n", output);
			progresult = XMLLINT_ERR_OUT;
		    }
		}
		if ((timing) && (!repeat)) {
		    endTimer("Saving");
		}
	    } else
#endif
#ifdef LIBXML_C14N_ENABLED
            if (canonical) {
	        xmlChar *result = NULL;
		int size;

		size = xmlC14NDocDumpMemory(doc, NULL, 0, NULL, 1, &result);
		if (size >= 0) {
		    write(1, result, size);
		    xmlFree(result);
		} else {
		    fprintf(stderr, "Failed to canonicalize\n");
		    progresult = XMLLINT_ERR_OUT;
		}
	    } else
#endif
#ifdef HAVE_SYS_MMAN_H
	    if (memory) {
		xmlChar *result;
		int len;

		if (encoding != NULL) {
		    if ( format ) {
		        xmlDocDumpFormatMemoryEnc(doc, &result, &len, encoding, 1);
		    } else { 
			xmlDocDumpMemoryEnc(doc, &result, &len, encoding);
		    }
		} else {
		    if (format)
			xmlDocDumpFormatMemory(doc, &result, &len, 1);
		    else
			xmlD