/*
 * xmlIO.c : implementation of the I/O interfaces used by the parser
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * 14 Nov 2000 ht - for VMS, truncated name of long functions to under 32 char
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

/* Figure a portable way to know if a file is a directory. */
#ifndef HAVE_STAT
#  ifdef HAVE__STAT
     /* MS C library seems to define stat and _stat. The definition
        is identical. Still, mapping them to each other causes a warning. */
#    ifndef _MSC_VER
#      define stat(x,y) _stat(x,y)
#    endif
#    define HAVE_STAT
#  endif
#endif
#ifdef HAVE_STAT
#  ifndef S_ISDIR
#    ifdef _S_ISDIR
#      define S_ISDIR(x) _S_ISDIR(x)
#    else
#      ifdef S_IFDIR
#        ifndef S_IFMT
#          ifdef _S_IFMT
#            define S_IFMT _S_IFMT
#          endif
#        endif
#        ifdef S_IFMT
#          define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#        endif
#      endif
#    endif
#  endif
#endif

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlIO.h>
#include <libxml/uri.h>
#include <libxml/nanohttp.h>
#include <libxml/nanoftp.h>
#include <libxml/xmlerror.h>
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxml/globals.h>

/* #define VERBOSE_FAILURE */
/* #define DEBUG_EXTERNAL_ENTITIES */
/* #define DEBUG_INPUT */

#ifdef DEBUG_INPUT
#define MINLEN 40
#else
#define MINLEN 4000
#endif

/*
 * Input I/O callback sets
 */
typedef struct _xmlInputCallback {
    xmlInputMatchCallback matchcallback;
    xmlInputOpenCallback opencallback;
    xmlInputReadCallback readcallback;
    xmlInputCloseCallback closecallback;
} xmlInputCallback;

#define MAX_INPUT_CALLBACK 15

static xmlInputCallback xmlInputCallbackTable[MAX_INPUT_CALLBACK];
static int xmlInputCallbackNr = 0;
static int xmlInputCallbackInitialized = 0;

#ifdef LIBXML_OUTPUT_ENABLED
/*
 * Output I/O callback sets
 */
typedef struct _xmlOutputCallback {
    xmlOutputMatchCallback matchcallback;
    xmlOutputOpenCallback opencallback;
    xmlOutputWriteCallback writecallback;
    xmlOutputCloseCallback closecallback;
} xmlOutputCallback;

#define MAX_OUTPUT_CALLBACK 15

static xmlOutputCallback xmlOutputCallbackTable[MAX_OUTPUT_CALLBACK];
static int xmlOutputCallbackNr = 0;
static int xmlOutputCallbackInitialized = 0;
#endif /* LIBXML_OUTPUT_ENABLED */

/************************************************************************
 *									*
 * 		Tree memory error handler				*
 *									*
 ************************************************************************/

static const char *IOerr[] = {
    "Unknown IO error",         /* UNKNOWN */
    "Permission denied",	/* EACCES */
    "Resource temporarily unavailable",/* EAGAIN */
    "Bad file descriptor",	/* EBADF */
    "Bad message",		/* EBADMSG */
    "Resource busy",		/* EBUSY */
    "Operation canceled",	/* ECANCELED */
    "No child processes",	/* ECHILD */
    "Resource deadlock avoided",/* EDEADLK */
    "Domain error",		/* EDOM */
    "File exists",		/* EEXIST */
    "Bad address",		/* EFAULT */
    "File too large",		/* EFBIG */
    "Operation in progress",	/* EINPROGRESS */
    "Interrupted function call",/* EINTR */
    "Invalid argument",		/* EINVAL */
    "Input/output error",	/* EIO */
    "Is a directory",		/* EISDIR */
    "Too many open files",	/* EMFILE */
    "Too many links",		/* EMLINK */
    "Inappropriate message buffer length",/* EMSGSIZE */
    "Filename too long",	/* ENAMETOOLONG */
    "Too many open files in system",/* ENFILE */
    "No such device",		/* ENODEV */
    "No such file or directory",/* ENOENT */
    "Exec format error",	/* ENOEXEC */
    "No locks available",	/* ENOLCK */
    "Not enough space",		/* ENOMEM */
    "No space left on device",	/* ENOSPC */
    "Function not implemented",	/* ENOSYS */
    "Not a directory",		/* ENOTDIR */
    "Directory not empty",	/* ENOTEMPTY */
    "Not supported",		/* ENOTSUP */
    "Inappropriate I/O control operation",/* ENOTTY */
    "No such device or address",/* ENXIO */
    "Operation not permitted",	/* EPERM */
    "Broken pipe",		/* EPIPE */
    "Result too large",		/* ERANGE */
    "Read-only file system",	/* EROFS */
    "Invalid seek",		/* ESPIPE */
    "No such process",		/* ESRCH */
    "Operation timed out",	/* ETIMEDOUT */
    "Improper link",		/* EXDEV */
    "Attempt to load network entity %s", /* XML_IO_NETWORK_ATTEMPT */
    "encoder error",		/* XML_IO_ENCODER */
    "flush error",
    "write error",
    "no input",
    "buffer full",
    "loading error",
    "not a socket",		/* ENOTSOCK */
    "already connected",	/* EISCONN */
    "connection refused",	/* ECONNREFUSED */
    "unreachable network",	/* ENETUNREACH */
    "adddress in use",		/* EADDRINUSE */
    "already in use",		/* EALREADY */
    "unknown address familly",	/* EAFNOSUPPORT */
};

/**
 * xmlIOErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlIOErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_IO, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * __xmlIOErr:
 * @code:  the error number
 * @
 * @extra:  extra informations
 *
 * Handle an I/O error
 */
void
__xmlIOErr(int domain, int code, const char *extra)
{
    unsigned int idx;

    if (code == 0) {
#ifdef HAVE_ERRNO_H
	if (errno == 0) code = 0;
#ifdef EACCES
        else if (errno == EACCES) code = XML_IO_EACCES;
#endif
#ifdef EAGAIN
        else if (errno == EAGAIN) code = XML_IO_EAGAIN;
#endif
#ifdef EBADF
        else if (errno == EBADF) code = XML_IO_EBADF;
#endif
#ifdef EBADMSG
        else if (errno == EBADMSG) code = XML_IO_EBADMSG;
#endif
#ifdef EBUSY
        else if (errno == EBUSY) code = XML_IO_EBUSY;
#endif
#ifdef ECANCELED
        else if (errno == ECANCELED) code = XML_IO_ECANCELED;
#endif
#ifdef ECHILD
        else if (errno == ECHILD) code = XML_IO_ECHILD;
#endif
#ifdef EDEADLK
        else if (errno == EDEADLK) code = XML_IO_EDEADLK;
#endif
#ifdef EDOM
        else if (errno == EDOM) code = XML_IO_EDOM;
#endif
#ifdef EEXIST
        else if (errno == EEXIST) code = XML_IO_EEXIST;
#endif
#ifdef EFAULT
        else if (errno == EFAULT) code = XML_IO_EFAULT;
#endif
#ifdef EFBIG
        else if (errno == EFBIG) code = XML_IO_EFBIG;
#endif
#ifdef EINPROGRESS
        else if (errno == EINPROGRESS) code = XML_IO_EINPROGRESS;
#endif
#ifdef EINTR
        else if (errno == EINTR) code = XML_IO_EINTR;
#endif
#ifdef EINVAL
        else if (errno == EINVAL) code = XML_IO_EINVAL;
#endif
#ifdef EIO
        else if (errno == EIO) code = XML_IO_EIO;
#endif
#ifdef EISDIR
        else if (errno == EISDIR) code = XML_IO_EISDIR;
#endif
#ifdef EMFILE
        else if (errno == EMFILE) code = XML_IO_EMFILE;
#endif
#ifdef EMLINK
        else if (errno == EMLINK) code = XML_IO_EMLINK;
#endif
#ifdef EMSGSIZE
        else if (errno == EMSGSIZE) code = XML_IO_EMSGSIZE;
#endif
#ifdef ENAMETOOLONG
        else if (errno == ENAMETOOLONG) code = XML_IO_ENAMETOOLONG;
#endif
#ifdef ENFILE
        else if (errno == ENFILE) code = XML_IO_ENFILE;
#endif
#ifdef ENODEV
        else if (errno == ENODEV) code = XML_IO_ENODEV;
#endif
#ifdef ENOENT
        else if (errno == ENOENT) code = XML_IO_ENOENT;
#endif
#ifdef ENOEXEC
        else if (errno == ENOEXEC) code = XML_IO_ENOEXEC;
#endif
#ifdef ENOLCK
        else if (errno == ENOLCK) code = XML_IO_ENOLCK;
#endif
#ifdef ENOMEM
        else if (errno == ENOMEM) code = XML_IO_ENOMEM;
#endif
#ifdef ENOSPC
        else if (errno == ENOSPC) code = XML_IO_ENOSPC;
#endif
#ifdef ENOSYS
        else if (errno == ENOSYS) code = XML_IO_ENOSYS;
#endif
#ifdef ENOTDIR
        else if (errno == ENOTDIR) code = XML_IO_ENOTDIR;
#endif
#ifdef ENOTEMPTY
        else if (errno == ENOTEMPTY) code = XML_IO_ENOTEMPTY;
#endif
#ifdef ENOTSUP
        else if (errno == ENOTSUP) code = XML_IO_ENOTSUP;
#endif
#ifdef ENOTTY
        else if (errno == ENOTTY) code = XML_IO_ENOTTY;
#endif
#ifdef ENXIO
        else if (errno == ENXIO) code = XML_IO_ENXIO;
#endif
#ifdef EPERM
        else if (errno == EPERM) code = XML_IO_EPERM;
#endif
#ifdef EPIPE
        else if (errno == EPIPE) code = XML_IO_EPIPE;
#endif
#ifdef ERANGE
        else if (errno == ERANGE) code = XML_IO_ERANGE;
#endif
#ifdef EROFS
        else if (errno == EROFS) code = XML_IO_EROFS;
#endif
#ifdef ESPIPE
        else if (errno == ESPIPE) code = XML_IO_ESPIPE;
#endif
#ifdef ESRCH
        else if (errno == ESRCH) code = XML_IO_ESRCH;
#endif
#ifdef ETIMEDOUT
        else if (errno == ETIMEDOUT) code = XML_IO_ETIMEDOUT;
#endif
#ifdef EXDEV
        else if (errno == EXDEV) code = XML_IO_EXDEV;
#endif
#ifdef ENOTSOCK
        else if (errno == ENOTSOCK) code = XML_IO_ENOTSOCK;
#endif
#ifdef EISCONN
        else if (errno == EISCONN) code = XML_IO_EISCONN;
#endif
#ifdef ECONNREFUSED
        else if (errno == ECONNREFUSED) code = XML_IO_ECONNREFUSED;
#endif
#ifdef ETIMEDOUT
        else if (errno == ETIMEDOUT) code = XML_IO_ETIMEDOUT;
#endif
#ifdef ENETUNREACH
        else if (errno == ENETUNREACH) code = XML_IO_ENETUNREACH;
#endif
#ifdef EADDRINUSE
        else if (errno == EADDRINUSE) code = XML_IO_EADDRINUSE;
#endif
#ifdef EINPROGRESS
        else if (errno == EINPROGRESS) code = XML_IO_EINPROGRESS;
#endif
#ifdef EALREADY
        else if (errno == EALREADY) code = XML_IO_EALREADY;
#endif
#ifdef EAFNOSUPPORT
        else if (errno == EAFNOSUPPORT) code = XML_IO_EAFNOSUPPORT;
#endif
        else code = XML_IO_UNKNOWN;
#endif /* HAVE_ERRNO_H */
    }
    idx = 0;
    if (code >= XML_IO_UNKNOWN) idx = code - XML_IO_UNKNOWN;
    if (idx >= (sizeof(IOerr) / sizeof(IOerr[0]))) idx = 0;
    
    __xmlSimpleError(domain, code, NULL, IOerr[idx], extra);
}

/**
 * xmlIOErr:
 * @code:  the error number
 * @extra:  extra informations
 *
 * Handle an I/O error
 */
static void
xmlIOErr(int code, const char *extra)
{
    __xmlIOErr(XML_FROM_IO, code, extra);
}

/**
 * __xmlLoaderErr:
 * @ctx: the parser context
 * @extra:  extra informations
 *
 * Handle a resource access error
 */
void
__xmlLoaderErr(void *ctx, const char *msg, const char *filename)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;
    xmlErrorLevel level = XML_ERR_ERROR;

    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
	return;
    if ((ctxt != NULL) && (ctxt->sax != NULL)) {
        if (ctxt->validate) {
	    channel = ctxt->sax->error;
	    level = XML_ERR_ERROR;
	} else {
	    channel = ctxt->sax->warning;
	    level = XML_ERR_WARNING;
	}
	if (ctxt->sax->initialized == XML_SAX2_MAGIC)
	    schannel = ctxt->sax->serror;
	data = ctxt->userData;
    }
    __xmlRaiseError(schannel, channel, data, ctxt, NULL, XML_FROM_IO,
                    XML_IO_LOAD_ERROR, level, NULL, 0,
		    filename, NULL, NULL, 0, 0,
		    msg, filename);
                    
}

/************************************************************************
 *									*
 * 		Tree memory error handler				*
 *									*
 ************************************************************************/
/**
 * xmlNormalizeWindowsPath:
 * @path: the input file path
 *
 * This function is obsolete. Please see xmlURIFromPath in uri.c for
 * a better solution.
 *
 * Returns a canonicalized version of the path
 */
xmlChar *
xmlNormalizeWindowsPath(const xmlChar *path)
{
    return xmlCanonicPath(path);
}

/**
 * xmlCleanupInputCallbacks:
 *
 * clears the entire input callback table. this includes the
 * compiled-in I/O. 
 */
void
xmlCleanupInputCallbacks(void)
{
    int i;

    if (!xmlInputCallbackInitialized)
        return;

    for (i = xmlInputCallbackNr - 1; i >= 0; i--) {
        xmlInputCallbackTable[i].matchcallback = NULL;
        xmlInputCallbackTable[i].opencallback = NULL;
        xmlInputCallbackTable[i].readcallback = NULL;
        xmlInputCallbackTable[i].closecallback = NULL;
    }

    xmlInputCallbackNr = 0;
    xmlInputCallbackInitialized = 0;
}

/**
 * xmlPopInputCallbacks:
 *
 * Clear the top input callback from the input stack. this includes the
 * compiled-in I/O. 
 *
 * Returns the number of input callback registered or -1 in case of error.
 */
int
xmlPopInputCallbacks(void)
{
    if (!xmlInputCallbackInitialized)
        return(-1);

    if (xmlInputCallbackNr <= 0)
        return(-1);
        
    xmlInputCallbackNr--;
    xmlInputCallbackTable[xmlInputCallbackNr].matchcallback = NULL;
    xmlInputCallbackTable[xmlInputCallbackNr].opencallback = NULL;
    xmlInputCallbackTable[xmlInputCallbackNr].readcallback = NULL;
    xmlInputCallbackTable[xmlInputCallbackNr].closecallback = NULL;

    return(xmlInputCallbackNr);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlCleanupOutputCallbacks:
 *
 * clears the entire output callback table. this includes the
 * compiled-in I/O callbacks. 
 */
void
xmlCleanupOutputCallbacks(void)
{
    int i;

    if (!xmlOutputCallbackInitialized)
        return;

    for (i = xmlOutputCallbackNr - 1; i >= 0; i--) {
        xmlOutputCallbackTable[i].matchcallback = NULL;
        xmlOutputCallbackTable[i].opencallback = NULL;
        xmlOutputCallbackTable[i].writecallback = NULL;
        xmlOutputCallbackTable[i].closecallback = NULL;
    }

    xmlOutputCallbackNr = 0;
    xmlOutputCallbackInitialized = 0;
}
#endif /* LIBXML_OUTPUT_ENABLED */

/************************************************************************
 *									*
 *		Standard I/O for file accesses				*
 *									*
 ************************************************************************/

/**
 * xmlCheckFilename:
 * @path:  the path to check
 *
 * function checks to see if @path is a valid source
 * (file, socket...) for XML.
 *
 * if stat is not available on the target machine,
 * returns 1.  if stat fails, returns 0 (if calling
 * stat on the filename fails, it can't be right).
 * if stat succeeds and the file is a directory,
 * returns 2.  otherwise returns 1.
 */

int
xmlCheckFilename (const char *path)
{
#ifdef HAVE_STAT
    struct stat stat_buffer;

    if (path == NULL)
        return(0);

    if (stat(path, &stat_buffer) == -1)
        return 0;

#ifdef S_ISDIR
    if (S_ISDIR(stat_buffer.st_mode)) {
        return 2;
    }
#endif
#endif
    if (path == NULL)
        return(0);

    return 1;
}

static int
xmlNop(void) {
    return(0);
}

/**
 * xmlFdRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to read
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFdRead (void * context, char * buffer, int len) {
    int ret;

    ret = read((int) (long) context, &buffer[0], len);
    if (ret < 0) xmlIOErr(0, "read()");
    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFdWrite:
 * @context:  the I/O context
 * @buffer:  where to get data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFdWrite (void * context, const char * buffer, int len) {
    int ret;

    ret = write((int) (long) context, &buffer[0], len);
    if (ret < 0) xmlIOErr(0, "write()");
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlFdClose:
 * @context:  the I/O context
 *
 * Close an I/O channel
 *
 * Returns 0 in case of success and error code otherwise
 */
static int
xmlFdClose (void * context) {
    int ret;
    ret = close((int) (long) context);
    if (ret < 0) xmlIOErr(0, "close()");
    return(ret);
}

/**
 * xmlFileMatch:
 * @filename:  the URI for matching
 *
 * input from FILE *
 *
 * Returns 1 if matches, 0 otherwise
 */
int
xmlFileMatch (const char *filename ATTRIBUTE_UNUSED) {
    return(1);
}

/**
 * xmlFileOpen_real:
 * @filename:  the URI for matching
 *
 * input from FILE *, supports compressed input
 * if @filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlFileOpen_real (const char *filename) {
    const char *path = NULL;
    FILE *fd;

    if (filename == NULL)
        return(NULL);

    if (!strcmp(filename, "-")) {
	fd = stdin;
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);
    if (!xmlCheckFilename(path))
        return(NULL);

#if defined(WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
    fd = fopen(path, "rb");
#else
    fd = fopen(path, "r");
#endif /* WIN32 */
    if (fd == NULL) xmlIOErr(0, path);
    return((void *) fd);
}

/**
 * xmlFileOpen:
 * @filename:  the URI for matching
 *
 * Wrapper around xmlFileOpen_real that try it with an unescaped
 * version of @filename, if this fails fallback to @filename
 *
 * Returns a handler or NULL in case or failure
 */
void *
xmlFileOpen (const char *filename) {
    char *unescaped;
    void *retval;

    unescaped = xmlURIUnescapeString(filename, 0, NULL);
    if (unescaped != NULL) {
	retval = xmlFileOpen_real(unescaped);
	xmlFree(unescaped);
    } else {
	retval = xmlFileOpen_real(filename);
    }
    return retval;
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFileOpenW:
 * @filename:  the URI for matching
 *
 * output to from FILE *,
 * if @filename is "-" then the standard output is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlFileOpenW (const char *filename) {
    const char *path = NULL;
    FILE *fd;

    if (!strcmp(filename, "-")) {
	fd = stdout;
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);

    fd = fopen(path, "wb");
    if (fd == NULL) xmlIOErr(0, path);
    return((void *) fd);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlFileRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written or < 0 in case of failure
 */
int
xmlFileRead (void * context, char * buffer, int len) {
    int ret;
    if ((context == NULL) || (buffer == NULL)) 
        return(-1);
    ret = fread(&buffer[0], 1,  len, (FILE *) context);
    if (ret < 0) xmlIOErr(0, "fread()");
    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFileWrite:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFileWrite (void * context, const char * buffer, int len) {
    int items;

    if ((context == NULL) || (buffer == NULL)) 
        return(-1);
    items = fwrite(&buffer[0], len, 1, (FILE *) context);
    if ((items == 0) && (ferror((FILE *) context))) {
        xmlIOErr(0, "fwrite()");
	return(-1);
    }
    return(items * len);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlFileClose:
 * @context:  the I/O context
 *
 * Close an I/O channel
 *
 * Returns 0 or -1 in case of error
 */
int
xmlFileClose (void * context) {
    FILE *fil;
    int ret;

    if (context == NULL)
        return(-1);
    fil = (FILE *) context;
    if ((fil == stdout) || (fil == stderr)) {
        ret = fflush(fil);
	if (ret < 0)
	    xmlIOErr(0, "fflush()");
	return(0);
    }
    if (fil == stdin)
	return(0);
    ret = ( fclose((FILE *) context) == EOF ) ? -1 : 0;
    if (ret < 0)
        xmlIOErr(0, "fclose()");
    return(ret);
}

/**
 * xmlFileFlush:
 * @context:  the I/O context
 *
 * Flush an I/O channel
 */
static int
xmlFileFlush (void * context) {
    int ret;

    if (context == NULL)
        return(-1);
    ret = ( fflush((FILE *) context) == EOF ) ? -1 : 0;
    if (ret < 0)
        xmlIOErr(0, "fflush()");
    return(ret);
}

#ifdef HAVE_ZLIB_H
/************************************************************************
 *									*
 *		I/O for compressed file accesses			*
 *									*
 ************************************************************************/
/**
 * xmlGzfileMatch:
 * @filename:  the URI for matching
 *
 * input from compressed file test
 *
 * Returns 1 if matches, 0 otherwise
 */
static int
xmlGzfileMatch (const char *filename ATTRIBUTE_UNUSED) {
    return(1);
}

/**
 * xmlGzfileOpen_real:
 * @filename:  the URI for matching
 *
 * input from compressed file open
 * if @filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlGzfileOpen_real (const char *filename) {
    const char *path = NULL;
    gzFile fd;

    if (!strcmp(filename, "-")) {
        fd = gzdopen(dup(0), "rb");
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);
    if (!xmlCheckFilename(path))
        return(NULL);

    fd = gzopen(path, "rb");
    return((void *) fd);
}

/**
 * xmlGzfileOpen:
 * @filename:  the URI for matching
 *
 * Wrapper around xmlGzfileOpen if the open fais, it will
 * try to unescape @filename
 */
static void *
xmlGzfileOpen (const char *filename) {
    char *unescaped;
    void *retval;

    retval = xmlGzfileOpen_real(filename);
    if (retval == NULL) {
	unescaped = xmlURIUnescapeString(filename, 0, NULL);
	if (unescaped != NULL) {
	    retval = xmlGzfileOpen_real(unescaped);
	}
	xmlFree(unescaped);
    }
    return retval;
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlGzfileOpenW:
 * @filename:  the URI for matching
 * @compression:  the compression factor (0 - 9 included)
 *
 * input from compressed file open
 * if @filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlGzfileOpenW (const char *filename, int compression) {
    const char *path = NULL;
    char mode[15];
    gzFile fd;

    snprintf(mode, sizeof(mode), "wb%d", compression);
    if (!strcmp(filename, "-")) {
        fd = gzdopen(dup(1), mode);
	return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[17];
#else
	path = &filename[16];
#endif
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
#if defined (_WIN32) || defined (__DJGPP__) && !defined(__CYGWIN__)
	path = &filename[8];
#else
	path = &filename[7];
#endif
    } else 
	path = filename;

    if (path == NULL)
	return(NULL);

    fd = gzopen(path, mode);
    return((void *) fd);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlGzfileRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the compressed I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlGzfileRead (void * context, char * buffer, int len) {
    int ret;

    ret = gzread((gzFile) context, &buffer[0], len);
    if (ret < 0) xmlIOErr(0, "gzread()");
    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlGzfileWrite:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the compressed I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlGzfileWrite (void * context, const char * buffer, int len) {
    int ret;

    ret = gzwrite((gzFile) context, (char *) &buffer[0], len);
    if (ret < 0) xmlIOErr(0, "gzwrite()");
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlGzfileClose:
 * @context:  the I/O context
 *
 * Close a compressed I/O channel
 */
static int
xmlGzfileClose (void * context) {
    int ret;

    ret =  (gzclose((gzFile) context) == Z_OK ) ? 0 : -1;
    if (ret < 0) xmlIOErr(0, "gzclose()");
    return(ret);
}
#endif /* HAVE_ZLIB_H */

#ifdef LIBXML_HTTP_ENABLED
/************************************************************************
 *									*
 *			I/O for HTTP file accesses			*
 *									*
 ************************************************************************/

#ifdef LIBXML_OUTPUT_ENABLED
typedef struct xmlIOHTTPWriteCtxt_
{
    int			compression;

    char *		uri;

    void *		doc_buff;

} xmlIOHTTPWriteCtxt, *xmlIOHTTPWriteCtxtPtr;

#ifdef HAVE_ZLIB_H

#define DFLT_WBITS		( -15 )
#define DFLT_MEM_LVL		( 8 )
#define GZ_MAGIC1		( 0x1f )
#define GZ_MAGIC2		( 0x8b )
#define LXML_ZLIB_OS_CODE	( 0x03 )
#define INIT_HTTP_BUFF_SIZE	( 32768 )
#define DFLT_ZLIB_RATIO		( 5 )

/*
**  Data structure and functions to work with sending compressed data
**  via HTTP.
*/

typedef struct xmlZMemBuff_
{
   unsigned long	size;
   unsigned long	crc;

   unsigned char *	zbuff;
   z_stream		zctrl;

} xmlZMemBuff, *xmlZMemBuffPtr;

/**
 * append_reverse_ulong
 * @buff:  Compressed memory buffer
 * @data:  Unsigned long to append
 *
 * Append a unsigned long in reverse byte order to the end of the
 * memory buffer.
 */
static void
append_reverse_ulong( xmlZMemBuff * buff, unsigned long data ) {

    int		idx;

    if ( buff == NULL )
	return;

    /*
    **  This is plagiarized from putLong in gzio.c (zlib source) where
    **  the number "4" is hardcoded.  If zlib is ever patched to 
    **  support 64 bit file sizes, this code would need to be patched
    **  as well.
    */

    for ( idx = 0; idx < 4; idx++ ) {
	*buff->zctrl.next_out = ( data & 0xff );
	data >>= 8;
	buff->zctrl.next_out++;
    }

    return;
}

/**
 *
 * xmlFreeZMemBuff
 * @buff:  The memory buffer context to clear
 *
 * Release all the resources associated with the compressed memory buffer.
 */
static void
xmlFreeZMemBuff( xmlZMemBuffPtr buff ) {

#ifdef DEBUG_HTTP
    int z_err;
#endif

    if ( buff == NULL )
	return;

    xmlFree( buff->zbuff );
#ifdef DEBUG_HTTP
    z_err = deflateEnd( &buff->zctrl );
    if ( z_err != Z_OK )
	xmlGenericError( xmlGenericErrorContext,
			"xmlFreeZMemBuff:  Error releasing zlib context:  %d\n",
			z_err );
#else
    deflateEnd( &buff->zctrl );
#endif

    xmlFree( buff );
    return;
}

/**
 * xmlCreateZMemBuff
 *@compression:	Compression value to use
 *
 * Create a memory buffer to hold the compressed XML document.  The
 * compressed document in memory will end up being identical to what
 * would be created if gzopen/gzwrite/gzclose were being used to 
 * write the document to disk.  The code for the header/trailer data to
 * the compression is plagiarized from the zlib source files.
 */
static void *
xmlCreateZMemBuff( int compression ) {

    int			z_err;
    int			hdr_lgth;
    xmlZMemBuffPtr	buff = NULL;

    if ( ( compression < 1 ) || ( compression > 9 ) )
	return ( NULL );

    /*  Create the control and data areas  */

    buff = xmlMalloc( sizeof( xmlZMemBuff ) );
    if ( buff == NULL ) {
	xmlIOErrMemory("creating buffer context");
	return ( NULL );
    }

    (void)memset( buff, 0, sizeof( xmlZMemBuff ) );
    buff->size = INIT_HTTP_BUFF_SIZE;
    buff->zbuff = xmlMalloc( buff->size );
    if ( buff->zbuff == NULL ) {
	xmlFreeZMemBuff( buff );
	xmlIOErrMemory("creating buffer");
	return ( NULL );
    }

    z_err = deflateInit2( &buff->zctrl, compression, Z_DEFLATED,
			    DFLT_WBITS, DFLT_MEM_LVL, Z_DEFAULT_STRATEGY );
    if ( z_err != Z_OK ) {
	xmlChar msg[500];
	xmlFreeZMemBuff( buff );
	buff = NULL;
	xmlStrPrintf(msg, 500,
		    (const xmlChar *) "xmlCreateZMemBuff:  %s %d\n",
		    "Error initializing compression context.  ZLIB error:",
		    z_err );
	xmlIOErr(XML_IO_WRITE, (const char *) msg);
	return ( NULL );
    }

    /*  Set the header data.  The CRC will be needed for the trailer  */
    buff->crc = crc32( 0L, Z_NULL, 0 );
    hdr_lgth = snprintf( (char *)buff->zbuff, buff->size,
			"%c%c%c%c%c%c%c%c%c%c",
			GZ_MAGIC1, GZ_MAGIC2, Z_DEFLATED, 
			0, 0, 0, 0, 0, 0, LXML_ZLIB_OS_CODE );
    buff->zctrl.next_out  = buff->zbuff + hdr_lgth;
    buff->zctrl.avail_out = buff->size - hdr_lgth;

    return ( buff );
}

/**
 * xmlZMemBuffExtend
 * @buff:  Buffer used to compress and consolidate data.
 * @ext_amt:   Number of bytes to extend the buffer.
 *
 * Extend the internal buffer used to store the compressed data by the
 * specified amount.
 *
 * Returns 0 on success or -1 on failure to extend the buffer.  On failure
 * the original buffer still exists at the original size.
 */
static int
xmlZMemBuffExtend( xmlZMemBuffPtr buff, size_t ext_amt ) {

    int			rc = -1;
    size_t		new_size;
    size_t		cur_used;

    unsigned char *	tmp_ptr = NULL;

    if ( buff == NULL )
	return ( -1 );

    else if ( ext_amt == 0 )
	return ( 0 );

    cur_used = buff->zctrl.next_out - buff->zbuff;
    new_size = buff->size + ext_amt;

#ifdef DEBUG_HTTP
    if ( cur_used > new_size ) 
	xmlGenericError( xmlGenericErrorContext,
			"xmlZMemBuffExtend:  %s\n%s %d bytes.\n",
			"Buffer overwrite detected during compressed memory",
			"buffer extension.  Overflowed by", 
			(cur_used - new_size ) );
#endif

    tmp_ptr = xmlRealloc( buff->zbuff, new_size );
    if ( tmp_ptr != NULL ) {
	rc = 0;
	buff->size  = new_size;
	buff->zbuff = tmp_ptr;
	buff->zctrl.next_out  = tmp_ptr + cur_used;
	buff->zctrl.avail_out = new_size - cur_used;
    }
    else {
	xmlChar msg[500];
	xmlStrPrintf(msg, 500,
		    (const xmlChar *) "xmlZMemBuffExtend:  %s %lu bytes.\n",
		    "Allocation failure extending output buffer to",
		    new_size );
	xmlIOErr(XML_IO_WRITE, (const char *) msg);
    }

    return ( rc );
}

/**
 * xmlZMemBuffAppend
 * @buff:  Buffer used to compress and consolidate data
 * @src:   Uncompressed source content to append to buffer
 * @len:   Length of source data to append to buffer
 *
 * Compress and append data to the internal buffer.  The data buffer
 * will be expanded if needed to store the additional data.
 *
 * Returns the number of bytes appended to the buffer or -1 on error.
 */
static int
xmlZMemBuffAppend( xmlZMemBuffPtr buff, const char * src, int len ) {

    int		z_err;
    size_t	min_accept;

    if ( ( buff == NULL ) || ( src == NULL ) )
	return ( -1 );

    buff->zctrl.avail_in = len;
    buff->zctrl.next_in  = (unsigned char *)src;
    while ( buff->zctrl.avail_in > 0 ) {
	/*
	**  Extend the buffer prior to deflate call if a reasonable amount
	**  of output buffer space is not available.
	*/
	min_accept = buff->zctrl.avail_in / DFLT_ZLIB_RATIO;
	if ( buff->zctrl.avail_out <= min_accept ) {
	    if ( xmlZMemBuffExtend( buff, buff->size ) == -1 )
		return ( -1 );
	}

	z_err = deflate( &buff->zctrl, Z_NO_FLUSH );
	if ( z_err != Z_OK ) {
	    xmlChar msg[500];
	    xmlStrPrintf(msg, 500,
			(const xmlChar *) "xmlZMemBuffAppend:  %s %d %s - %d",
			"Compression error while appending",
			len, "bytes to buffer.  ZLIB error", z_err );
	    xmlIOErr(XML_IO_WRITE, (const char *) msg);
	    return ( -1 );
	}
    }

    buff->crc = crc32( buff->crc, (unsigned char *)src, len );

    return ( len );
}

/**
 * xmlZMemBuffGetContent
 * @buff:  Compressed memory content buffer
 * @data_ref:  Pointer reference to point to compressed content
 *
 * Flushes the compression buffers, appends gzip file trailers and
 * returns the compressed content and length of the compressed data.
 * NOTE:  The gzip trailer code here is plagiarized from zlib source.
 *
 * Returns the length of the compressed data or -1 on error.
 */
static int
xmlZMemBuffGetContent( xmlZMemBuffPtr buff, char ** data_ref ) {

    int		zlgth = -1;
    int		z_err;

    