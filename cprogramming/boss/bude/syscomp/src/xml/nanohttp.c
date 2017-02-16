/*
 * nanohttp.c: minimalist HTTP GET implementation to fetch external subsets.
 *             focuses on size, streamability, reentrancy and portability
 *
 * This is clearly not a general purpose HTTP implementation
 * If you look for one, check:
 *         http://www.w3.org/Library/
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */
 
/* TODO add compression support, Send the Accept- , and decompress on the
        fly with ZLIB if found at compile-time */

#define NEED_SOCKETS
#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_HTTP_ENABLED
#include <string.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_RESOLV_H
#ifdef HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
#include <resolv.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h> 
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef SUPPORT_IP6
#include <resolv.h>
#endif

#ifdef VMS
#include <stropts>
#define XML_SOCKLEN_T unsigned int
#define SOCKET int
#endif


#ifdef __MINGW32__
#define _WINSOCKAPI_
#include <wsockcompat.h>
#include <winsock2.h>
#undef XML_SOCKLEN_T
#define XML_SOCKLEN_T unsigned int
#endif


#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h> /* for xmlStr(n)casecmp() */
#include <libxml/nanohttp.h>
#include <libxml/globals.h>
#include <libxml/uri.h>

/**
 * A couple portability macros
 */
#ifndef _WINSOCKAPI_
#ifndef __BEOS__
#define closesocket(s) close(s)
#endif
#define SOCKET int
#endif

#ifdef __BEOS__
#ifndef PF_INET
#define PF_INET AF_INET
#endif
#endif

#ifndef XML_SOCKLEN_T
#define XML_SOCKLEN_T unsigned int
#endif
#ifndef SOCKET
#define SOCKET int
#endif

#ifdef STANDALONE
#define DEBUG_HTTP
#define xmlStrncasecmp(a, b, n) strncasecmp((char *)a, (char *)b, n)
#define xmlStrcasecmpi(a, b) strcasecmp((char *)a, (char *)b)
#endif

#define XML_NANO_HTTP_MAX_REDIR	10

#define XML_NANO_HTTP_CHUNK	4096

#define XML_NANO_HTTP_CLOSED	0
#define XML_NANO_HTTP_WRITE	1
#define XML_NANO_HTTP_READ	2
#define XML_NANO_HTTP_NONE	4

typedef struct xmlNanoHTTPCtxt {
    char *protocol;	/* the protocol name */
    char *hostname;	/* the host name */
    int port;		/* the port */
    char *path;		/* the path within the URL */
    SOCKET fd;		/* the file descriptor for the socket */
    int state;		/* WRITE / READ / CLOSED */
    char *out;		/* buffer sent (zero terminated) */
    char *outptr;	/* index within the buffer sent */
    char *in;		/* the receiving buffer */
    char *content;	/* the start of the content */
    char *inptr;	/* the next byte to read from network */
    char *inrptr;	/* the next byte to give back to the client */
    int inlen;		/* len of the input buffer */
    int last;		/* return code for last operation */
    int returnValue;	/* the protocol return value */
    int ContentLength;  /* specified content length from HTTP header */
    char *contentType;	/* the MIME type for the input */
    char *location;	/* the new URL in case of redirect */
    char *authHeader;	/* contents of {WWW,Proxy}-Authenticate header */
    char *encoding;	/* encoding extracted from the contentType */
    char *mimeType;	/* Mime-Type extracted from the contentType */
} xmlNanoHTTPCtxt, *xmlNanoHTTPCtxtPtr;

static int initialized = 0;
static char *proxy = NULL;	 /* the proxy name if any */
static int proxyPort;	/* the proxy port if any */
static unsigned int timeout = 60;/* the select() timeout in seconds */

static int xmlNanoHTTPFetchContent( void * ctx, char ** ptr, int * len );

/**
 * xmlHTTPErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlHTTPErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_HTTP, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * A portability function
 */
static int socket_errno(void) {
#ifdef _WINSOCKAPI_
    return(WSAGetLastError());
#else
    return(errno);
#endif
}

#ifdef SUPPORT_IP6
static
int have_ipv6(void) {
    int s;

    s = socket (AF_INET6, SOCK_STREAM, 0);
    if (s != -1) {
	close (s);
	return (1);
    }
    return (0);
}
#endif

/**
 * xmlNanoHTTPInit:
 *
 * Initialize the HTTP protocol layer.
 * Currently it just checks for proxy informations
 */

void
xmlNanoHTTPInit(void) {
    const char *env;
#ifdef _WINSOCKAPI_
    WSADATA wsaData;    
#endif

    if (initialized)
	return;

#ifdef _WINSOCKAPI_
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	return;
#endif

    if (proxy == NULL) {
	proxyPort = 80;
	env = getenv("no_proxy");
	if (env && ((env[0] == '*') && (env[1] == 0)))
	    goto done;
	env = getenv("http_proxy");
	if (env != NULL) {
	    xmlNanoHTTPScanProxy(env);
	    goto done;
	}
	env = getenv("HTTP_PROXY");
	if (env != NULL) {
	    xmlNanoHTTPScanProxy(env);
	    goto done;
	}
    }
done:
    initialized = 1;
}

/**
 * xmlNanoHTTPCleanup:
 *
 * Cleanup the HTTP protocol layer.
 */

void
xmlNanoHTTPCleanup(void) {
    if (proxy != NULL)
	xmlFree(proxy);
#ifdef _WINSOCKAPI_
    if (initialized)
	WSACleanup();
#endif
    initialized = 0;
    return;
}

/**
 * xmlNanoHTTPScanURL:
 * @ctxt:  an HTTP context
 * @URL:  The URL used to initialize the context
 *
 * (Re)Initialize an HTTP context by parsing the URL and finding
 * the protocol host port and path it indicates.
 */

static void
xmlNanoHTTPScanURL(xmlNanoHTTPCtxtPtr ctxt, const char *URL) {
    const char *cur = URL;
    char buf[4096];
    int indx = 0;
    const int indxMax = 4096 - 1;
    int port = 0;

    if (ctxt->protocol != NULL) { 
        xmlFree(ctxt->protocol);
	ctxt->protocol = NULL;
    }
    if (ctxt->hostname != NULL) { 
        xmlFree(ctxt->hostname);
	ctxt->hostname = NULL;
    }
    if (ctxt->path != NULL) { 
        xmlFree(ctxt->path);
	ctxt->path = NULL;
    }
    if (URL == NULL) return;
    buf[indx] = 0;
    while ((*cur != 0) && (indx < indxMax)) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[indx] = 0;
	    ctxt->protocol = xmlMemStrdup(buf);
	    indx = 0;
            cur += 3;
	    break;
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0) return;

    buf[indx] = 0;
    while (indx < indxMax) {
	if ((strchr (cur, '[') && !strchr (cur, ']')) ||
		(!strchr (cur, '[') && strchr (cur, ']'))) {
	    __xmlIOErr(XML_FROM_HTTP, XML_HTTP_URL_SYNTAX, 
	               "Syntax Error\n");
	    return;
	}

	if (cur[0] == '[') {
	    cur++;
	    while ((cur[0] != ']') && (indx < indxMax))
		buf[indx++] = *cur++;
    
	    if (!strchr (buf, ':')) {
		__xmlIOErr(XML_FROM_HTTP, XML_HTTP_USE_IP,
			"Use [IPv6]/IPv4 format\n");
		return;
	    }

	    buf[indx] = 0;
	    ctxt->hostname = xmlMemStrdup (buf);
	    indx = 0;
	    cur += 1;
	    if (cur[0] == ':') {
		cur++;
		while (*cur >= '0' && *cur <= '9') {
		    port *= 10;
		    port += *cur - '0';
		    cur++;
		}

		if (port != 0) ctxt->port = port;
		while ((cur[0] != '/') && (*cur != 0))
		    cur++;
	    }
	    break;
        }
	else {
	    if (cur[0] == ':') {
		buf[indx] = 0;
		ctxt->hostname = xmlMemStrdup (buf);
		indx = 0;
		cur += 1;
		while ((*cur >= '0') && (*cur <= '9')) {
		    port *= 10;
		    port += *cur - '0';
		    cur++;
		}
		if (port != 0) ctxt->port = port;
		while ((cur[0] != '/') && (*cur != 0)) 
		    cur++;
		break;
	    }
	    if ((*cur == '/') || (*cur == 0)) {
		buf[indx] = 0;
		ctxt->hostname = xmlMemStrdup (buf);
		indx = 0;
		break;
	    }
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0) 
        ctxt->path = xmlMemStrdup("/");
    else {
        indx = 0;
        buf[indx] = 0;
	while ((*cur != 0) && (indx < indxMax))
	    buf[indx++] = *cur++;
	buf[indx] = 0;
	ctxt->path = xmlMemStrdup(buf);
    }	
}

/**
 * xmlNanoHTTPScanProxy:
 * @URL:  The proxy URL used to initialize the proxy context
 *
 * (Re)Initialize the HTTP Proxy context by parsing the URL and finding
 * the protocol host port it indicates.
 * Should be like http://myproxy/ or http://myproxy:3128/
 * A NULL URL cleans up proxy informations.
 */

void
xmlNanoHTTPScanProxy(const char *URL) {
    const char *cur = URL;
    char buf[4096];
    int indx = 0;
    const int indxMax = 4096 - 1;
    int port = 0;

    if (proxy != NULL) { 
        xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyPort != 0) { 
	proxyPort = 0;
    }
#ifdef DEBUG_HTTP
    if (URL == NULL)
	xmlGenericError(xmlGenericErrorContext,
		"Removing HTTP proxy info\n");
    else
	xmlGenericError(xmlGenericErrorContext,
		"Using HTTP proxy %s\n", URL);
#endif
    if (URL == NULL) return;
    buf[indx] = 0;
    while ((*cur != 0) && (indx < indxMax)) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[indx] = 0;
	    indx = 0;
            cur += 3;
	    break;
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0) return;

    buf[indx] = 0;
    while (indx < indxMax) {
	if ((strchr (cur, '[') && !strchr (cur, ']')) ||
		(!strchr (cur, '[') && strchr (cur, ']'))) {
	    __xmlIOErr(XML_FROM_HTTP, XML_HTTP_URL_SYNTAX, "Syntax Error\n");
	    return;
	}

	if (cur[0] == '[') {
	    cur++;
	    while ((cur[0] != ']') && (indx < indxMax))
		buf[indx++] = *cur++;

	    if (!strchr (buf, ':')) {
		__xmlIOErr(XML_FROM_HTTP, XML_HTTP_USE_IP,
			"Use [IPv6]/IPv4 format\n");
		return;
	    }

	    buf[indx] = 0;
	    proxy = xmlMemStrdup (buf);
	    indx = 0;
	    cur += 1;
	    if (cur[0] == ':') {
	        cur++;
		while (*cur >= '0' && *cur <= '9') {
		    port *= 10;
		    port += *cur - '0';
		    cur++;
		}

		if (port != 0) proxyPort = port;
		while ((cur[0] != '/') && (*cur != 0))
		    cur ++;
	    }
	    break;
	}
	else {
	    if (cur[0] == ':') {
		buf[indx] = 0;
		proxy = xmlMemStrdup (buf);
		indx = 0;
		cur += 1;
		while ((*cur >= '0') && (*cur <= '9')) {
		    port *= 10;
		    port += *cur - '0';
		    cur++;
		}
		if (port != 0) proxyPort = port;
		while ((cur[0] != '/') && (*cur != 0)) 
		    cur++;
		break;
	    }
	    if ((*cur == '/') || (*cur == 0)) {
		buf[indx] = 0;
		proxy = xmlMemStrdup (buf);
		indx = 0;
		break;
	    }
	}
	buf[indx++] = *cur++;
    }
}

/**
 * xmlNanoHTTPNewCtxt:
 * @URL:  The URL used to initialize the context
 *
 * Allocate and initialize a new HTTP context.
 *
 * Returns an HTTP context or NULL in case of error.
 */

static xmlNanoHTTPCtxtPtr
xmlNanoHTTPNewCtxt(const char *URL) {
    xmlNanoHTTPCtxtPtr ret;

    ret = (xmlNanoHTTPCtxtPtr) xmlMalloc(sizeof(xmlNanoHTTPCtxt));
    if (ret == NULL) {
        xmlHTTPErrMemory("allocating context");
        return(NULL);
    }

    memset(ret, 0, sizeof(xmlNanoHTTPCtxt));
    ret->port = 80;
    ret->returnValue = 0;
    ret->fd = -1;
    ret->ContentLength = -1;

    xmlNanoHTTPScanURL(ret, URL);

    return(ret);
}

/**
 * xmlNanoHTTPFreeCtxt:
 * @ctxt:  an HTTP context
 *
 * Frees the context after closing the connection.
 */

static void
xmlNanoHTTPFreeCtxt(xmlNanoHTTPCtxtPtr ctxt) {
    if (ctxt == NULL) return;
    if (ctxt->hostname != NULL) xmlFree(ctxt->hostname);
    if (ctxt->protocol != NULL) xmlFree(ctxt->protocol);
    if (ctxt->path != NULL) xmlFree(ctxt->path);
    if (ctxt->out != NULL) xmlFree(ctxt->out);
    if (ctxt->in != NULL) xmlFree(ctxt->in);
    if (ctxt->contentType != NULL) xmlFree(ctxt->contentType);
    if (ctxt->encoding != NULL) xmlFree(ctxt->encoding);
    if (ctxt->mimeType != NULL) xmlFree(ctxt->mimeType);
    if (ctxt->location != NULL) xmlFree(ctxt->location);
    if (ctxt->authHeader != NULL) xmlFree(ctxt->authHeader);
    ctxt->state = XML_NANO_HTTP_NONE;
    if (ctxt->fd >= 0) closesocket(ctxt->fd);
    ctxt->fd = -1;
    xmlFree(ctxt);
}

/**
 * xmlNanoHTTPSend:
 * @ctxt:  an HTTP context
 *
 * Send the input needed to initiate the processing on the server side
 * Returns number of bytes sent or -1 on error.
 */

static int
xmlNanoHTTPSend(xmlNanoHTTPCtxtPtr ctxt, const char * xmt_ptr, int outlen) {

    int 	total_sent = 0;

    if ( (ctxt->state & XML_NANO_HTTP_WRITE) && (xmt_ptr != NULL ) ) {
        while (total_sent < outlen) {
            int nsent = send(ctxt->fd, xmt_ptr + total_sent,
                                      outlen - total_sent, 0);
            if (nsent>0)
                total_sent += nsent;
	    else if ( ( nsent == -1 ) && 
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
	    	      ( socket_errno( ) != EAGAIN ) &&
#endif
		        ( socket_errno( ) != EWOULDBLOCK ) ) {
		__xmlIOErr(XML_FROM_HTTP, 0, "send failed\n");
		if ( total_sent == 0 )
		    total_sent = -1;
		break;
	    }
	    else {
	        /*
		**  No data sent
		**  Since non-blocking sockets are used, wait for 
		**  socket to be writable or default timeout prior
		**  to retrying.
		*/

		struct timeval	tv;
		fd_set		wfd;

		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		FD_ZERO( &wfd );
		FD_SET( ctxt->fd, &wfd );
		(void)select( ctxt->fd + 1, NULL, &wfd, NULL, &tv );
	    }
	}
    }

    return total_sent;
}

/**
 * xmlNanoHTTPRecv:
 * @ctxt:  an HTTP context
 *
 * Read information coming from the HTTP connection.
 * This is a blocking call (but it blocks in select(), not read()).
 *
 * Returns the number of byte read or -1 in case of error.
 */

static int
xmlNanoHTTPRecv(xmlNanoHTTPCtxtPtr ctxt) {
    fd_set rfd;
    struct timeval tv;


    while (ctxt->state & XML_NANO_HTTP_READ) {
	if (ctxt->in == NULL) {
	    ctxt->in = (char *) xmlMallocAtomic(65000 * sizeof(char));
	    if (ctxt->in == NULL) {
		xmlHTTPErrMemory("allocating input");
	        ctxt->last = -1;
		return(-1);
	    }
	    ctxt->inlen = 65000;
	    ctxt->inptr = ctxt->content = ctxt->inrptr = ctxt->in;
	}
	if (ctxt->inrptr > ctxt->in + XML_NANO_HTTP_CHUNK) {
	    int delta = ctxt->inrptr - ctxt->in;
	    int len = ctxt->inptr - ctxt->inrptr;
	    
	    memmove(ctxt->in, ctxt->inrptr, len);
	    ctxt->inrptr -= delta;
	    ctxt->content -= delta;
	    ctxt->inptr -= delta;
	}
        if ((ctxt->in + ctxt->inlen) < (ctxt->inptr + XML_NANO_HTTP_CHUNK)) {
	    int d_inptr = ctxt->inptr - ctxt->in;
	    int d_content = ctxt->content - ctxt->in;
	    int d_inrptr = ctxt->inrptr - ctxt->in;
	    char *	tmp_ptr = ctxt->in;

	    ctxt->inlen *= 2;
            ctxt->in = (char *) xmlRealloc(tmp_ptr, ctxt->inlen);
	    if (ctxt->in == NULL) {
		xmlHTTPErrMemory("allocating input buffer");
		xmlFree( tmp_ptr );
	        ctxt->last = -1;
		return(-1);
	    }
            ctxt->inptr = ctxt->in + d_inptr;
            ctxt->content = ctxt->in + d_content;
            ctxt->inrptr = ctxt->in + d_inrptr;
	}
	ctxt->last = recv(ctxt->fd, ctxt->inptr, XML_NANO_HTTP_CHUNK, 0);
	if (ctxt->last > 0) {
	    ctxt->inptr += ctxt->last;
	    return(ctxt->last);
	}
	if (ctxt->last == 0) {
	    return(0);
	}
	if (ctxt->last == -1) {
	    switch (socket_errno()) {
		case EINPROGRESS:
		case EWOULDBLOCK:
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
		case EAGAIN:
#endif
		    break;

		case ECONNRESET:
		case ESHUTDOWN:
		    return ( 0 );

		default:
		    __xmlIOErr(XML_FROM_HTTP, 0, "recv failed\n");
		    return(-1);
	    }
	}

	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	FD_ZERO(&rfd);
	FD_SET(ctxt->fd, &rfd);
	
	if ( (select(ctxt->fd+1, &rfd, NULL, NULL, &tv)<1)
#if defined(EINTR)
		&& (errno != EINTR)
#endif
	)
		return(0);
    }
    return(0);
}

/**
 * xmlNanoHTTPReadLine:
 * @ctxt:  an HTTP context
 *
 * Read one line in the HTTP server output, usually for extracting
 * the HTTP protocol informations from the answer header.
 *
 * Returns a newly allocated string with a copy of the line, or NULL
 *         which indicate the end of the input.
 */

static char *
xmlNanoHTTPReadLine(xmlNanoHTTPCtxtPtr ctxt) {
    char buf[4096];
    char *bp = buf;
    int	rc;
    
    while (bp - buf < 4095) {
	if (ctxt->inrptr == ctxt->inptr) {
	    if ( (rc = xmlNanoHTTPRecv(ctxt)) == 0) {
		if (bp == buf)
		    return(NULL);
		else
		    *bp = 0;
		return(xmlMemStrdup(buf));
	    }
	    else if ( rc == -1 ) {
	        return ( NULL );
	    }
	}
	*bp = *ctxt->inrptr++;
	if (*bp == '\n') {
	    *bp = 0;
	    return(xmlMemStrdup(buf));
	}
	if (*bp != '\r')
	    bp++;
    }
    buf[4095] = 0;
    return(xmlMemStrdup(buf));
}


/**
 * xmlNanoHTTPScanAnswer:
 * @ctxt:  an HTTP context
 * @line:  an HTTP header line
 *
 * Try to extract useful informations from the server answer.
 * We currently parse and process:
 *  - The HTTP revision/ return code
 *  - The Content-Type, Mime-Type and charset used
 *  - The Location for redirect processing.
 *
 * Returns -1 in case of failure, the file descriptor number otherwise
 */

static void
xmlNanoHTTPScanAnswer(xmlNanoHTTPCtxtPtr ctxt, const char *line) {
    const char *cur = line;

    if (line == NULL) return;

    if (!strncmp(line, "HTTP/", 5)) {
        int version = 0;
	int ret = 0;

	cur += 5;
	while ((*cur >= '0') && (*cur <= '9')) {
	    version *= 10;
	    version += *cur - '0';
	    cur++;
	}
	if (*cur == '.') {
	    cur++;
	    if ((*cur >= '0') && (*cur <= '9')) {
		version *= 10;
		version += *cur - '0';
		cur++;
	    }
	    while ((*cur >= '0') && (*cur <= '9'))
		cur++;
	} else
	    version *= 10;
	if ((*cur != ' ') && (*cur != '\t')) return;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if ((*cur < '0') || (*cur > '9')) return;
	while ((*cur >= '0') && (*cur <= '9')) {
	    ret *= 10;
	    ret += *cur - '0';
	    cur++;
	}
	if ((*cur != 0) && (*cur != ' ') && (*cur != '\t')) return;
	ctxt->returnValue = ret;
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"Content-Type:", 13)) {
        const xmlChar *charset, *last, *mime;
        cur += 13;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->contentType != NULL)
	    xmlFree(ctxt->contentType);
	ctxt->contentType = xmlMemStrdup(cur);
	mime = (const xmlChar *) cur;
	last = mime;
	while ((*last != 0) && (*last != ' ') && (*last != '\t') &&
	       (*last != ';') && (*last != ','))
	    last++;
	if (ctxt->mimeType != NULL)
	    xmlFree(ctxt->mimeType);
	ctxt->mimeType = (char *) xmlStrndup(mime, last - mime);
	charset = xmlStrstr(BAD_CAST ctxt->contentType, BAD_CAST "charset=");
	if (charset != NULL) {
	    charset += 8;
	    last = charset;
	    while ((*last != 0) && (*last != ' ') && (*last != '\t') &&
	           (*last != ';') && (*last != ','))
		last++;
	    if (ctxt->encoding != NULL)
	        xmlFree(ctxt->encoding);
	    ctxt->encoding = (char *) xmlStrndup(charset, last - charset);
	}
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"ContentType:", 12)) {
        const xmlChar *charset, *last, *mime;
        cur += 12;
	if (ctxt->contentType != NULL) return;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	ctxt->contentType = xmlMemStrdup(cur);
	mime = (const xmlChar *) cur;
	last = mime;
	while ((*last != 0) && (*last != ' ') && (*last != '\t') &&
	       (*last != ';') && (*last != ','))
	    last++;
	if (ctxt->mimeType != NULL)
	    xmlFree(ctxt->mimeType);
	ctxt->mimeType = (char *) xmlStrndup(mime, last - mime);
	charset = xmlStrstr(BAD_CAST ctxt->contentType, BAD_CAST "charset=");
	if (charset != NULL) {
	    charset += 8;
	    last = charset;
	    while ((*last != 0) && (*last != ' ') && (*last != '\t') &&
	           (*last != ';') && (*last != ','))
		last++;
	    if (ctxt->encoding != NULL)
	        xmlFree(ctxt->encoding);
	    ctxt->encoding = (char *) xmlStrndup(charset, last - charset);
	}
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"Location:", 9)) {
        cur += 9;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->location != NULL)
	    xmlFree(ctxt->location);
	if (*cur == '/') {
	    xmlChar *tmp_http = xmlStrdup(BAD_CAST "http://");
	    xmlChar *tmp_loc = 
	        xmlStrcat(tmp_http, (const xmlChar *) ctxt->hostname);
	    ctxt->location = 
	        (char *) xmlStrcat (tmp_loc, (const xmlChar *) cur);
	} else {
	    ctxt->location = xmlMemStrdup(cur);
	}
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"WWW-Authenticate:", 17)) {
        cur += 17;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->authHeader != NULL)
	    xmlFree(ctxt->authHeader);
	ctxt->authHeader = xmlMemStrdup(cur);
    } else if (!xmlStrncasecmp(BAD_CAST line, BAD_CAST"Proxy-Authenticate:", 19)) {
        cur += 19;
	while ((*cur == ' ') || (*cur == '\t')) cur++;
	if (ctxt->authHeader != NULL)
	    xmlFree(ctxt->authHeader);
	ctxt->authHeader = xmlMemStrdup(cur);
    } else if ( !xmlStrncasecmp( BAD_CAST line, BAD_CAST"Content-Length:", 15) ) {
	cur += 15;
	ctxt->ContentLength = strtol( cur, NULL, 10 );
    }
}

/**
 * xmlNanoHTTPConnectAttempt:
 * @addr:  a socket address structure
 *
 * Attempt a connection to the given IP:port endpoint. It forces
 * non-blocking semantic on the socket, and allow 60 seconds for
 * the host to answer.
 *
 * Returns -1 in case of failure, the file descriptor number otherwise
 */

static int
xmlNanoHTTPConnectAttempt(struct sockaddr *addr)
{
    fd_set wfd;
#ifdef _WINSOCKAPI_
    fd_set xfd;
#endif
    struct timeval tv;
    int status;
    int addrlen;
    SOCKET s;
    
#ifdef SUPPORT_IP6
    if (addr->sa_family == AF_INET6) {
	s = socket (PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	addrlen = sizeof (struct sockaddr_in6);
    }
    else
#endif
    {
	s = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	addrlen = sizeof (struct sockaddr_in);
    }
    if (s==-1) {
#ifdef DEBUG_HTTP
	perror("socket");
#endif
	__xmlIOErr(XML_FROM_HTTP, 0, "socket failed\n");
	return(-1);
    }
    
#ifdef _WINSOCKAPI_
    {
	u_long one = 1;

	status = ioctlsocket(s, FIONBIO, &one) == SOCKET_ERROR ? -1 : 0;
    }
#else /* _WINSOCKAPI_ */
#if defined(VMS)
    {
	int enable = 1;
	status = ioctl(s, FIONBIO, &enable);
    }
#else /* VMS */
#if defined(__BEOS__)
	{
		bool noblock = true;
		status = setsockopt(s, SOL_SOCKET, SO_NONBLOCK, &noblock, sizeof(noblock));
	}
#else /* __BEOS__ */
    if ((status = fcntl(s, F_GETFL, 0)) != -1) {
#ifdef O_NONBLOCK
	status |= O_NONBLOCK;
#else /* O_NONBLOCK */
#ifdef F_NDELAY
	status |= F_NDELAY;
#endif /* F_NDELAY */
#endif /* !O_NONBLOCK */
	status = fcntl(s, F_SETFL, status);
    }
    if (status < 0) {
#ifdef DEBUG_HTTP
	perror("nonblocking");
#endif
	__xmlIOErr(XML_FROM_HTTP, 0, "error setting non-blocking IO\n");
	closesocket(s);
	return(-1);
    }
#endif /* !__BEOS__ */
#endif /* !VMS */
#endif /* !_WINSOCKAPI_ */

    if (connect (s, addr, addrlen) == -1) {
	switch (socket_errno()) {
	    case EINPROGRESS:
	    case EWOULDBLOCK:
		break;
	    default:
		__xmlIOErr(XML_FROM_HTTP, 0, "error connecting to HTTP server");
		closesocket(s);
		return(-1);
	}
    }	
    
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    
    FD_ZERO(&wfd);
    FD_SET(s, &wfd);

#ifdef _WINSOCKAPI_    
    FD_ZERO(&xfd);
    FD_SET(s, &xfd);
    
    switch(select(s+1, NULL, &wfd, &xfd, &tv))
#else
    switch(select(s+1, NULL, &wfd, NULL, &tv))
#endif
    {
	case 0:
	    /* Time out */
	    __xmlIOErr(XML_FROM_HTTP, 0, "Connect attempt timed out");
	    closesocket(s);
	    return(-1);
	case -1:
	    /* Ermm.. ?? */
	    __xmlIOErr(XML_FROM_HTTP, 0, "Connect failed");
	    closesocket(s);
	    return(-1);
    }

    if ( FD_ISSET(s, &wfd)
#ifdef _WINSOCKAPI_
                           || FD_ISSET(s, &xfd)
#endif
                                                ) {
	XML_SOCKLEN_T len;
	len = sizeof(status);
#ifdef SO_ERROR
	if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&status, &len) < 0 ) {
	    /* Solaris error code */
	    __xmlIOErr(XML_FROM_HTTP, 0, "getsockopt failed\n");
	    return (-1);
	}
#endif
	if ( status ) {
	    __xmlIOErr(XML_FROM_HTTP, 0, "Error connecting to remote host");
	    closesocket(s);
	    errno = status;
	    return (-1);
	}
    } else {
	/* pbm */
	__xmlIOErr(XML_FROM_HTTP, 0, "select failed\n");
	closesocket(s);
	return (-1);
    }
    
    return(s);
}
 
/**
 * xmlNanoHTTPConnectHost:
 * @host:  the host name
 * @port:  the port number
 *
 * Attempt a connection to the given host:port endpoint. It tries
 * the multiple IP provided by the DNS if available.
 *
 * Returns -1 in case of failure, the file descriptor number otherwise
 */

static int
xmlNanoHTTPConnectHost(const char *host, int port)
{
    struct hostent *h;
    struct sockaddr *addr = NULL;
    struct in_addr ia;
    struct sockaddr_in sockin;

#ifdef SUPPORT_IP6
    struct in6_addr ia6;
    struct sockaddr_in6 sockin6;
#endif
    int i;
    int s;

    memset (&sockin, 0, sizeof(sockin));
#ifdef SUPPORT_IP6
    memset (&sockin6, 0, sizeof(sockin6));
    if (have_ipv6 ())
#if !defined(HAVE_GETADDRINFO) && defined(RES_USE_INET6)
    {
	if (!(_res.options & RES_INIT))
	    res_init();
	_res.options |= RES_USE_INET6;
    }
#elif defined(HAVE_GETADDRINFO)
    {
	int status;
	struct addrinfo hints, *res, *result;

	result = NULL;
	memset (&hints, 0,sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo (host, NULL, &hints, &result);
	if (status) {
	    __xmlIOErr(XML_FROM_HTTP, 0, "getaddrinfo failed\n");
	    return (-1);
	}

	for (res = result; res; res = res->ai_next) {
	    if (res->ai_family == AF_INET || res->ai_family == AF_INET6) {
		if (res->ai_family == AF_INET6) {
		    if (res->ai_addrlen > sizeof(sockin6)) {
			__xmlIOErr(XML_FROM_HTTP, 0, "address size mismatch\n");
			freeaddrinfo (result);
			return (-1);
		    }
		    memcpy (&sockin6, res->ai_addr, res->ai_addrlen);
		    sockin6.sin6_port = htons (port);
		    addr = (struct sockaddr *)&sockin6;
		}
		else {
		    if (res->ai_addrlen > sizeof(sockin)) {
			__xmlIOErr(XML_FROM_HTTP, 0, "address size mismatch\n");
			freeaddrinfo (result);
			return (-1);
		    }
		    memcpy (&sockin, res->ai_addr, res->ai_addrlen);
		    sockin.sin_port = htons (port);
		    addr = (struct sockaddr *)&sockin;
		}

		s = xmlNanoHTTPConnectAttempt (addr);
		if (s != -1) {
		    freeaddrinfo (result);
		    return (s);
		}
	    }
	}
	if (result)
	    freeaddrinfo (result);
	return (-1);
    } else
#endif
#endif
    {   
	h = gethostbyname (host);
	if (h == NULL) {

/*
 * Okay, I got fed up by the non-portability of this error message
 * extraction code. it work on Linux, if it work on your platform
 * and one want to enable it, send me the defined(foobar) needed
 */
#if defined(HAVE_NETDB_H) && defined(HOST_NOT_FOUND) && defined(linux)
	    const char *h_err_txt = "";

	    switch (h_errno) {
		case HOST_NOT_FOUND:
		    h_err_txt = "Authoritive host not found";
		    break;

		case TRY_AGAIN:
		    h_err_txt =
			"Non-authoritive host not found or server failure.";
		    break;

		case NO_RECOVERY:
		    h_err_txt =
			"Non-recoverable errors:  FORMERR, REFUSED, or NOTIMP.";
		    break;

		case NO_ADDRESS:
		    h_err_txt =
			"Valid name, no data record of requested type.";
		    break;

		default:
		    h_err_txt = "No error text defined.";
		    break;
	    }
	    __xmlIOErr(XML_FROM_HTTP, 0, h_err_txt);
#else
	    __xmlIOErr(XML_FROM_HTTP, 0, "Failed to resolve host");
#endif
	    return (-1);
	}

	for (i = 0; h->h_addr_list[i]; i++) {
	    if (h->h_addrtype == AF_INET) {
		/* A records (IPv4) */
		if ((unsigned int) h->h_length > sizeof(ia)) {
		    __xmlIOErr(XML_FROM_HTTP, 0, "address size mismatch\n");
		    return (-1);
		}
		memcpy (&ia, h->h_addr_list[i], h->h_length);
		sockin.sin_family = h->h_addrtype;
		sockin.sin_addr = ia;
		sockin.sin_port = htons (port);
		addr = (struct sockaddr *) &sockin;
#ifdef SUPPORT_IP6
	    } else if (have_ipv6 () && (h->h_addrtype == AF_INET6)) {
		/* AAAA records (IPv6) */
		if ((unsigned int) h->h_length > sizeof(ia6)) {
		    __xmlIOErr(XML_FROM_HTTP, 0, "address size mismatch\n");
		    return (-1);
		}
		memcpy (&ia6, h->h_addr_list[i], h->h_length);
		sockin6.sin6_family = h->h_addrtype;
		sockin6.sin6_addr = ia6;
		sockin6.sin6_port = htons (port);
		addr = (struct sockaddr *) &sockin6;
#endif
	    } else
		break;              /* for */

	    s = xmlNanoHTTPConnectAttempt (addr);
	    if (s != -1)
		return (s);
	}
    }
#ifdef DEBUG_HTTP
    xmlGenericError(xmlGenericErrorContext,
                    "xmlNanoHTTPConnectHost:  unable to connect to '%s'.\n",
                    host);
#endif
    return (-1);
}


/**
 * xmlNanoHTTPOpen:
 * @URL:  The URL to load
 * @contentType:  if available the Content-Type information will be
 *                returned at that location
 *
 * This function try to open a connection to the indicated resource
 * via HTTP GET.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, if provided must be freed by the caller
 */

void*
xmlNanoHTTPOpen(const char *URL, char **contentType) {
    if (contentType != NULL) *contentType = NULL;
    return(xmlNanoHTTPMethod(URL, NULL, NULL, contentType, NULL, 0));
}

/**
 * xmlNanoHTTPOpenRedir:
 * @URL:  The URL to load
 * @contentType:  if available the Content-Type information will be
 *                returned at that location
 * @redir: if available the redirected URL will be returned
 *
 * This function try to open a connection to the indicated resource
 * via HTTP GET.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, if provided must be freed by the caller
 */

void*
xmlNanoHTTPOpenRedir(const char *URL, char **contentType, char **redir) {
    if (contentType != NULL) *contentType = NULL;
    if (redir != NULL) *redir = NULL;
    return(xmlNanoHTTPMethodRedir(URL, NULL, NULL, contentType, redir, NULL,0));
}

/**
 * xmlNanoHTTPRead:
 * @ctx:  the HTTP context
 * @dest:  a buffer
 * @len:  the buffer length
 *
 * This function tries to read @len bytes from the existing HTTP connection
 * and saves them in @dest. This is a blocking call.
 *
 * Returns the number of byte read. 0 is an indication of an end of connection.
 *         -1 indicates a parameter error.
 */
int
xmlNanoHTTPRead(void *ctx, void *dest, int len) {
    xmlNanoHTTPCtxtPtr ctxt = (xmlNanoHTTPCtxtPtr) ctx;

    if (ctx == NULL) return(-1);
    if (dest == NULL) return(-1);
    if (len <= 0) return(0);

    while (ctxt->inptr - ctxt->inrptr < len) {
        if (xmlNanoHTTPRecv(ctxt) <= 0) break;
    }
    if (ctxt->inptr - ctxt->inrptr < len)
        len = ctxt->inptr - ctxt->inrptr;
    memcpy(dest, ctxt->inrptr, len);
    ctxt->inrptr += len;
    return(len);
}

/**
 * xmlNanoHTTPClose:
 * @ctx:  the HTTP context
 *
 * This function closes an HTTP context, it ends up the connection and
 * free all data related to it.
 */
void
xmlNanoHTTPClose(void *ctx) {
    xmlNanoHTTPCtxtPtr ctxt = (xmlNanoHTTPCtxtPtr) ctx;

    if (ctx == NULL) return;

    xmlNanoHTTPFreeCtxt(ctxt);
}

/**
 * xmlNanoHTTPMethodRedir:
 * @URL:  The URL to load
 * @method:  the HTTP method to use
 * @input:  the input string if any
 * @contentType:  the Content-Type information IN and OUT
 * @redir:  the redirected URL OUT
 * @headers:  the extra headers
 * @ilen:  input length
 *
 * This function try to open a connection to the indicated resource
 * via HTTP using the given @method, adding the given extra headers
 * and the input buffer for the request content.
 *
 * Returns NULL in case of failure, otherwise a request handler.
 *     The contentType, or redir, if provided must be freed by the caller
 */

void*
xmlNanoHTTPMethodRedir(const char *URL, const char *method, const char *input,
                  char **contentType, char **redir,
		  const char *headers, int ilen ) {
    xmlNanoHTTPCtxtPtr ctxt;
    char *bp, *p;
    int blen, ret;
    int head;
    int nbRedirects = 0;
    char *redirURL = NULL;
#ifdef DEBUG_HTTP
    int xmt_bytes;
#endif
    
    if (URL == NULL) return(NULL);
    if (method == NULL) method = "GET";
    xmlNanoHTTPInit();

retry:
    if (redirURL == NULL)
	ctxt = xmlNanoHTTPNewCtxt(URL);
    else {
	ctxt = xmlNanoHTTPNewCtxt(redirURL);
	ctxt->location = xmlMemStrdup(redirURL);
    }

    if ( ctxt == NULL ) {
	return ( NULL );
    }

    if ((ctxt->protocol == NULL) || (strcmp(ctxt->protocol, "http"))) {
	__xmlIOErr(XML_FROM_HTTP, XML_HTTP_URL_SYNTAX, "Not a valid HTTP URI");
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    if (ctxt->hostname == NULL) {
	__xmlIOErr(XML_FROM_HTTP, XML_HTTP_UNKNOWN_HOST,
	           "Failed to identify host in URI");
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    if (proxy) {
	blen = strlen(ctxt->hostname) * 2 + 16;
	ret = xmlNanoHTTPConnectHost(proxy, proxyPort);
    }
    else {
	blen = strlen(ctxt->hostname);
	ret = xmlNanoHTTPConnectHost(ctxt-