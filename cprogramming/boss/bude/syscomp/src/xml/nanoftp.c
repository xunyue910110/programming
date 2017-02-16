/*
 * nanoftp.c: basic FTP client support
 *
 *  Reference: RFC 959
 */

#ifdef TESTING
#define STANDALONE
#define HAVE_STDLIB_H
#define HAVE_UNISTD_H
#define HAVE_SYS_SOCKET_H
#define HAVE_NETINET_IN_H
#define HAVE_NETDB_H
#define HAVE_SYS_TIME_H
#else /* TESTING */
#define NEED_SOCKETS
#endif /* TESTING */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_FTP_ENABLED
#include <string.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
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
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <libxml/uri.h>
#include <libxml/nanoftp.h>
#include <libxml/globals.h>

/* #define DEBUG_FTP 1  */
#ifdef STANDALONE
#ifndef DEBUG_FTP
#define DEBUG_FTP 1
#endif
#endif


#ifdef __MINGW32__
#define _WINSOCKAPI_
#include <wsockcompat.h>
#include <winsock2.h>
#undef XML_SOCKLEN_T
#define XML_SOCKLEN_T unsigned int
#endif


/**
 * A couple portability macros
 */
#ifndef _WINSOCKAPI_
#ifndef __BEOS__
#define closesocket(s) close(s)
#endif
#define SOCKET int
#endif
#if defined(VMS) || defined(__VMS)
#define XML_SOCKLEN_T unsigned int
#endif

#ifdef __BEOS__
#ifndef PF_INET
#define PF_INET AF_INET
#endif
#endif


#define FTP_COMMAND_OK		200
#define FTP_SYNTAX_ERROR	500
#define FTP_GET_PASSWD		331
#define FTP_BUF_SIZE		512

#define XML_NANO_MAX_URLBUF	4096

typedef struct xmlNanoFTPCtxt {
    char *protocol;	/* the protocol name */
    char *hostname;	/* the host name */
    int port;		/* the port */
    char *path;		/* the path within the URL */
    char *user;		/* user string */
    char *passwd;	/* passwd string */
#ifdef SUPPORT_IP6
    struct sockaddr_storage ftpAddr; /* this is large enough to hold IPv6 address*/
#else
    struct sockaddr_in ftpAddr; /* the socket address struct */
#endif
    int passive;	/* currently we support only passive !!! */
    SOCKET controlFd;	/* the file descriptor for the control socket */
    SOCKET dataFd;	/* the file descriptor for the data socket */
    int state;		/* WRITE / READ / CLOSED */
    int returnValue;	/* the protocol return value */
    /* buffer for data received from the control connection */
    char controlBuf[FTP_BUF_SIZE + 1];
    int controlBufIndex;
    int controlBufUsed;
    int controlBufAnswer;
} xmlNanoFTPCtxt;
typedef xmlNanoFTPCtxt *xmlNanoFTPCtxtPtr;

static int initialized = 0;
static char *proxy = NULL;	/* the proxy name if any */
static int proxyPort = 0;	/* the proxy port if any */
static char *proxyUser = NULL;	/* user for proxy authentication */
static char *proxyPasswd = NULL;/* passwd for proxy authentication */
static int proxyType = 0;	/* uses TYPE or a@b ? */

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
 * xmlFTPErrMemory:
 * @extra:  extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlFTPErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_FTP, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * xmlNanoFTPInit:
 *
 * Initialize the FTP protocol layer.
 * Currently it just checks for proxy informations,
 * and get the hostname
 */

void
xmlNanoFTPInit(void) {
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

    proxyPort = 21;
    env = getenv("no_proxy");
    if (env && ((env[0] == '*' ) && (env[1] == 0)))
	return;
    env = getenv("ftp_proxy");
    if (env != NULL) {
	xmlNanoFTPScanProxy(env);
    } else {
	env = getenv("FTP_PROXY");
	if (env != NULL) {
	    xmlNanoFTPScanProxy(env);
	}
    }
    env = getenv("ftp_proxy_user");
    if (env != NULL) {
	proxyUser = xmlMemStrdup(env);
    }
    env = getenv("ftp_proxy_password");
    if (env != NULL) {
	proxyPasswd = xmlMemStrdup(env);
    }
    initialized = 1;
}

/**
 * xmlNanoFTPCleanup:
 *
 * Cleanup the FTP protocol layer. This cleanup proxy informations.
 */

void
xmlNanoFTPCleanup(void) {
    if (proxy != NULL) {
	xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyUser != NULL) {
	xmlFree(proxyUser);
	proxyUser = NULL;
    }
    if (proxyPasswd != NULL) {
	xmlFree(proxyPasswd);
	proxyPasswd = NULL;
    }
#ifdef _WINSOCKAPI_
    if (initialized)
	WSACleanup();
#endif
    initialized = 0;
}

/**
 * xmlNanoFTPProxy:
 * @host:  the proxy host name
 * @port:  the proxy port
 * @user:  the proxy user name
 * @passwd:  the proxy password
 * @type:  the type of proxy 1 for using SITE, 2 for USER a@b
 *
 * Setup the FTP proxy informations.
 * This can also be done by using ftp_proxy ftp_proxy_user and
 * ftp_proxy_password environment variables.
 */

void
xmlNanoFTPProxy(const char *host, int port, const char *user,
	        const char *passwd, int type) {
    if (proxy != NULL) {
	xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyUser != NULL) {
	xmlFree(proxyUser);
	proxyUser = NULL;
    }
    if (proxyPasswd != NULL) {
	xmlFree(proxyPasswd);
	proxyPasswd = NULL;
    }
    if (host)
	proxy = xmlMemStrdup(host);
    if (user)
	proxyUser = xmlMemStrdup(user);
    if (passwd)
	proxyPasswd = xmlMemStrdup(passwd);
    proxyPort = port;
    proxyType = type;
}

/**
 * xmlNanoFTPScanURL:
 * @ctx:  an FTP context
 * @URL:  The URL used to initialize the context
 *
 * (Re)Initialize an FTP context by parsing the URL and finding
 * the protocol host port and path it indicates.
 */

static void
xmlNanoFTPScanURL(void *ctx, const char *URL) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    const char *cur = URL;
    char buf[XML_NANO_MAX_URLBUF];
    int indx = 0;
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
    while ((*cur != 0) && (indx < XML_NANO_MAX_URLBUF - 1)) {
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
    /* allow user@ and user:pass@ forms */
    {
	const char *p = strchr(cur, '@');
	if(p) {
	    while(indx < XML_NANO_MAX_URLBUF-1) {
		if(cur[0] == ':' || cur[0] == '@') break;
		buf[indx++] = *cur++;
	    }
	    buf[indx] = 0;
	    ctxt->user = xmlMemStrdup(buf);
	    indx = 0;
	    if(cur[0] == ':') {
		cur++;
		while(indx < XML_NANO_MAX_URLBUF-1) {
		    if(cur[0] == '@') break;
		    buf[indx++] = *cur++;
		}
		buf[indx] = 0;
		ctxt->passwd = xmlMemStrdup(buf);
		indx = 0;
	    }
	    cur = p+1;
	}
    }

    while (indx < XML_NANO_MAX_URLBUF - 1) {
	if ((strchr (cur, '[') && !strchr (cur, ']')) ||
		(!strchr (cur, '[') && strchr (cur, ']'))) {
	    xmlGenericError (xmlGenericErrorContext, "\nxmlNanoFTPScanURL: %s",
		    "Syntax Error\n");
	    return;
	}

	if (cur[0] == '[') {
	    cur++;
	    while ((cur[0] != ']') && (indx < XML_NANO_MAX_URLBUF-1))
		buf[indx++] = *cur++;
	    if (indx >= XML_NANO_MAX_URLBUF-1) {
		xmlGenericError(xmlGenericErrorContext,
		                "\nxmlNanoFTPScanURL: %s", "Syntax Error\n");
		return;
	    }

	    if (!strchr (buf, ':')) {
		xmlGenericError (xmlGenericErrorContext, "\nxmlNanoFTPScanURL: %s",
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
	else {     /* address is an IPv4 one*/
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
	while ((*cur != 0) && (indx < XML_NANO_MAX_URLBUF-1))
	    buf[indx++] = *cur++;
	buf[indx] = 0;
	ctxt->path = xmlMemStrdup(buf);
    }	
}

/**
 * xmlNanoFTPUpdateURL:
 * @ctx:  an FTP context
 * @URL:  The URL used to update the context
 *
 * Update an FTP context by parsing the URL and finding
 * new path it indicates. If there is an error in the 
 * protocol, hostname, port or other information, the
 * error is raised. It indicates a new connection has to
 * be established.
 *
 * Returns 0 if Ok, -1 in case of error (other host).
 */

int
xmlNanoFTPUpdateURL(void *ctx, const char *URL) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    const char *cur = URL;
    char buf[XML_NANO_MAX_URLBUF];
    int indx = 0;
    int port = 0;

    if (URL == NULL)
	return(-1);
    if (ctxt == NULL)
	return(-1);
    if (ctxt->protocol == NULL)
	return(-1);
    if (ctxt->hostname == NULL)
	return(-1);
    buf[indx] = 0;
    while ((*cur != 0) && (indx < XML_NANO_MAX_URLBUF-1)) {
        if ((cur[0] == ':') && (cur[1] == '/') && (cur[2] == '/')) {
	    buf[indx] = 0;
	    if (strcmp(ctxt->protocol, buf))
		return(-1);
	    indx = 0;
            cur += 3;
	    break;
	}
	buf[indx++] = *cur++;
    }
    if (*cur == 0)
	return(-1);

    buf[indx] = 0;
    while (indx < XML_NANO_MAX_URLBUF-1) {
	if ((strchr (cur, '[') && !strchr (cur, ']')) ||
		(!strchr (cur, '[') && strchr (cur, ']'))) {
	    xmlGenericError (xmlGenericErrorContext, "\nxmlNanoFTPUpdateURL: %s",
		    "Syntax Error\n");
	    return (-1);
	}

	if (cur[0] == '[') {
	    cur++;
	    while ((cur[0] != ']') && (indx < XML_NANO_MAX_URLBUF-1))
		buf[indx++] = *cur++;

	    if (!strchr (buf, ':')) {
		xmlGenericError (xmlGenericErrorContext, "\nxmlNanoFTPUpdateURL: %s",
			"Use [IPv6]/IPv4 format\n");
		return (-1);
	    }

	    buf[indx] = 0;
	    if (strcmp (ctxt->hostname, buf))
		return (-1);
	    indx = 0;
	    cur += 1;
	    if (cur[0] == ':') {
		cur++;
		while (*cur >= '0' && *cur <= '9') {
		    port *= 10;
		    port += *cur - '0';
		    cur++;
		}

		if (port != ctxt->port)
		    return (-1);
		while ((cur[0] != '/') && (*cur != 0))
		    cur++;
	    }
	    break;
	}
	else {
	    if (cur[0] == ':') {
		buf[indx] = 0;
		if (strcmp (ctxt->hostname, buf))
		    return (-1);
		indx = 0;
		cur += 1;
		while ((*cur >= '0') && (*cur <= '9')) {
		    port *= 10;
		    port += *cur - '0';
		    cur++;
		}
		if (port != ctxt->port)
		    return (-1);
		while ((cur[0] != '/') && (*cur != 0)) 
		    cur++;
		break;
	    }
	    if ((*cur == '/') || (*cur == 0)) {
		buf[indx] = 0;
		if (strcmp (ctxt->hostname, buf))
		    return (-1);
		indx = 0;
		break;
	    }
	}
	buf[indx++] = *cur++;
    }
    if (ctxt->path != NULL) {
	xmlFree(ctxt->path);
	ctxt->path = NULL;
    }

    if (*cur == 0) 
        ctxt->path = xmlMemStrdup("/");
    else {
        indx = 0;
        buf[indx] = 0;
	while ((*cur != 0) && (indx < XML_NANO_MAX_URLBUF-1))
	    buf[indx++] = *cur++;
	buf[indx] = 0;
	ctxt->path = xmlMemStrdup(buf);
    }	
    return(0);
}

/**
 * xmlNanoFTPScanProxy:
 * @URL:  The proxy URL used to initialize the proxy context
 *
 * (Re)Initialize the FTP Proxy context by parsing the URL and finding
 * the protocol host port it indicates.
 * Should be like ftp://myproxy/ or ftp://myproxy:3128/
 * A NULL URL cleans up proxy informations.
 */

void
xmlNanoFTPScanProxy(const char *URL) {
    const char *cur = URL;
    char buf[XML_NANO_MAX_URLBUF];
    int indx = 0;
    int port = 0;

    if (proxy != NULL) { 
        xmlFree(proxy);
	proxy = NULL;
    }
    if (proxyPort != 0) { 
	proxyPort = 0;
    }
#ifdef DEBUG_FTP
    if (URL == NULL)
	xmlGenericError(xmlGenericErrorContext, "Removing FTP proxy info\n");
    else
	xmlGenericError(xmlGenericErrorContext, "Using FTP proxy %s\n", URL);
#endif
    if (URL == NULL) return;
    buf[indx] = 0;
    while ((*cur != 0) && (indx < XML_NANO_MAX_URLBUF-1)) {
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
    while (indx < XML_NANO_MAX_URLBUF-1) {
	if ((strchr (cur, '[') && !strchr (cur, ']')) ||
		(!strchr (cur, '[') && strchr (cur, ']'))) {
	    xmlGenericError (xmlGenericErrorContext, "\nxmlNanoFTPScanProxy: %s",
		    "Syntax error\n");
	    return;
	}

	if (cur[0] == '[') {
	    cur++;
	    while ((cur[0] != ']') && (indx < XML_NANO_MAX_URLBUF-1))
		buf[indx++] = *cur++;
            if (indx >= XML_NANO_MAX_URLBUF-1) {
		xmlGenericError (xmlGenericErrorContext,
			  "\nxmlNanoFTPScanProxy: %s", "Syntax error\n");
		return;
	    }

	    if (!strchr (buf, ':')) {
		xmlGenericError (xmlGenericErrorContext, "\nxmlNanoFTPScanProxy: %s",
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
		    cur++;
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
 * xmlNanoFTPNewCtxt:
 * @URL:  The URL used to initialize the context
 *
 * Allocate and initialize a new FTP context.
 *
 * Returns an FTP context or NULL in case of error.
 */

void*
xmlNanoFTPNewCtxt(const char *URL) {
    xmlNanoFTPCtxtPtr ret;
    char *unescaped;

    ret = (xmlNanoFTPCtxtPtr) xmlMalloc(sizeof(xmlNanoFTPCtxt));
    if (ret == NULL) {
        xmlFTPErrMemory("allocating FTP context");
        return(NULL);
    }

    memset(ret, 0, sizeof(xmlNanoFTPCtxt));
    ret->port = 21;
    ret->passive = 1;
    ret->returnValue = 0;
    ret->controlBufIndex = 0;
    ret->controlBufUsed = 0;
    ret->controlFd = -1;

    unescaped = xmlURIUnescapeString(URL, 0, NULL);
    if (unescaped != NULL) {
	xmlNanoFTPScanURL(ret, unescaped);
	xmlFree(unescaped);
    } else if (URL != NULL)
	xmlNanoFTPScanURL(ret, URL);

    return(ret);
}

/**
 * xmlNanoFTPFreeCtxt:
 * @ctx:  an FTP context
 *
 * Frees the context after closing the connection.
 */

void
xmlNanoFTPFreeCtxt(void * ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    if (ctxt == NULL) return;
    if (ctxt->hostname != NULL) xmlFree(ctxt->hostname);
    if (ctxt->protocol != NULL) xmlFree(ctxt->protocol);
    if (ctxt->path != NULL) xmlFree(ctxt->path);
    ctxt->passive = 1;
    if (ctxt->controlFd >= 0) closesocket(ctxt->controlFd);
    ctxt->controlFd = -1;
    ctxt->controlBufIndex = -1;
    ctxt->controlBufUsed = -1;
    xmlFree(ctxt);
}

/**
 * xmlNanoFTPParseResponse:
 * @buf:  the buffer containing the response
 * @len:  the buffer length
 * 
 * Parsing of the server answer, we just extract the code.
 *
 * returns 0 for errors
 *     +XXX for last line of response
 *     -XXX for response to be continued
 */
static int
xmlNanoFTPParseResponse(char *buf, int len) {
    int val = 0;

    if (len < 3) return(-1);
    if ((*buf >= '0') && (*buf <= '9')) 
        val = val * 10 + (*buf - '0');
    else
        return(0);
    buf++;
    if ((*buf >= '0') && (*buf <= '9')) 
        val = val * 10 + (*buf - '0');
    else
        return(0);
    buf++;
    if ((*buf >= '0') && (*buf <= '9')) 
        val = val * 10 + (*buf - '0');
    else
        return(0);
    buf++;
    if (*buf == '-') 
        return(-val);
    return(val);
}

/**
 * xmlNanoFTPGetMore:
 * @ctx:  an FTP context
 *
 * Read more information from the FTP control connection
 * Returns the number of bytes read, < 0 indicates an error
 */
static int
xmlNanoFTPGetMore(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    int len;
    int size;

    if ((ctxt == NULL) || (ctxt->controlFd < 0)) return(-1);

    if ((ctxt->controlBufIndex < 0) || (ctxt->controlBufIndex > FTP_BUF_SIZE)) {
#ifdef DEBUG_FTP
        xmlGenericError(xmlGenericErrorContext,
		"xmlNanoFTPGetMore : controlBufIndex = %d\n",
		ctxt->controlBufIndex);
#endif
	return(-1);
    }

    if ((ctxt->controlBufUsed < 0) || (ctxt->controlBufUsed > FTP_BUF_SIZE)) {
#ifdef DEBUG_FTP
        xmlGenericError(xmlGenericErrorContext,
		"xmlNanoFTPGetMore : controlBufUsed = %d\n",
		ctxt->controlBufUsed);
#endif
	return(-1);
    }
    if (ctxt->controlBufIndex > ctxt->controlBufUsed) {
#ifdef DEBUG_FTP
        xmlGenericError(xmlGenericErrorContext,
		"xmlNanoFTPGetMore : controlBufIndex > controlBufUsed %d > %d\n",
	       ctxt->controlBufIndex, ctxt->controlBufUsed);
#endif
	return(-1);
    }

    /*
     * First pack the control buffer
     */
    if (ctxt->controlBufIndex > 0) {
	memmove(&ctxt->controlBuf[0], &ctxt->controlBuf[ctxt->controlBufIndex],
		ctxt->controlBufUsed - ctxt->controlBufIndex);
	ctxt->controlBufUsed -= ctxt->controlBufIndex;
	ctxt->controlBufIndex = 0;
    }
    size = FTP_BUF_SIZE - ctxt->controlBufUsed;
    if (size == 0) {
#ifdef DEBUG_FTP
        xmlGenericError(xmlGenericErrorContext,
		"xmlNanoFTPGetMore : buffer full %d \n", ctxt->controlBufUsed);
#endif
	return(0);
    }

    /*
     * Read the amount left on the control connection
     */
    if ((len = recv(ctxt->controlFd, &ctxt->controlBuf[ctxt->controlBufIndex],
		    size, 0)) < 0) {
	__xmlIOErr(XML_FROM_FTP, 0, "recv failed");
	closesocket(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
        return(-1);
    }
#ifdef DEBUG_FTP
    xmlGenericError(xmlGenericErrorContext,
	    "xmlNanoFTPGetMore : read %d [%d - %d]\n", len,
	   ctxt->controlBufUsed, ctxt->controlBufUsed + len);
#endif
    ctxt->controlBufUsed += len;
    ctxt->controlBuf[ctxt->controlBufUsed] = 0;

    return(len);
}

/**
 * xmlNanoFTPReadResponse:
 * @ctx:  an FTP context
 *
 * Read the response from the FTP server after a command.
 * Returns the code number
 */
static int
xmlNanoFTPReadResponse(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char *ptr, *end;
    int len;
    int res = -1, cur = -1;

    if ((ctxt == NULL) || (ctxt->controlFd < 0)) return(-1);

get_more:
    /*
     * Assumes everything up to controlBuf[controlBufIndex] has been read
     * and analyzed.
     */
    len = xmlNanoFTPGetMore(ctx);
    if (len < 0) {
        return(-1);
    }
    if ((ctxt->controlBufUsed == 0) && (len == 0)) {
        return(-1);
    }
    ptr = &ctxt->controlBuf[ctxt->controlBufIndex];
    end = &ctxt->controlBuf[ctxt->controlBufUsed];

#ifdef DEBUG_FTP
    xmlGenericError(xmlGenericErrorContext,
	    "\n<<<\n%s\n--\n", ptr);
#endif
    while (ptr < end) {
        cur = xmlNanoFTPParseResponse(ptr, end - ptr);
	if (cur > 0) {
	    /*
	     * Successfully scanned the control code, scratch
	     * till the end of the line, but keep the index to be
	     * able to analyze the result if needed.
	     */
	    res = cur;
	    ptr += 3;
	    ctxt->controlBufAnswer = ptr - ctxt->controlBuf;
	    while ((ptr < end) && (*ptr != '\n')) ptr++;
	    if (*ptr == '\n') ptr++;
	    if (*ptr == '\r') ptr++;
	    break;
	}
	while ((ptr < end) && (*ptr != '\n')) ptr++;
	if (ptr >= end) {
	    ctxt->controlBufIndex = ctxt->controlBufUsed;
	    goto get_more;
	}
	if (*ptr != '\r') ptr++;
    }

    if (res < 0) goto get_more;
    ctxt->controlBufIndex = ptr - ctxt->controlBuf;
#ifdef DEBUG_FTP
    ptr = &ctxt->controlBuf[ctxt->controlBufIndex];
    xmlGenericError(xmlGenericErrorContext, "\n---\n%s\n--\n", ptr);
#endif

#ifdef DEBUG_FTP
    xmlGenericError(xmlGenericErrorContext, "Got %d\n", res);
#endif
    return(res / 100);
}

/**
 * xmlNanoFTPGetResponse:
 * @ctx:  an FTP context
 *
 * Get the response from the FTP server after a command.
 * Returns the code number
 */

int
xmlNanoFTPGetResponse(void *ctx) {
    int res;

    res = xmlNanoFTPReadResponse(ctx);

    return(res);
}

/**
 * xmlNanoFTPCheckResponse:
 * @ctx:  an FTP context
 *
 * Check if there is a response from the FTP server after a command.
 * Returns the code number, or 0
 */

int
xmlNanoFTPCheckResponse(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    fd_set rfd;
    struct timeval tv;

    if ((ctxt == NULL) || (ctxt->controlFd < 0)) return(-1);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&rfd);
    FD_SET(ctxt->controlFd, &rfd);
    switch(select(ctxt->controlFd + 1, &rfd, NULL, NULL, &tv)) {
	case 0:
	    return(0);
	case -1:
	    __xmlIOErr(XML_FROM_FTP, 0, "select");
	    return(-1);
			
    }

    return(xmlNanoFTPReadResponse(ctx));
}

/**
 * Send the user authentication
 */

static int
xmlNanoFTPSendUser(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200];
    int len;
    int res;

    if (ctxt->user == NULL)
	snprintf(buf, sizeof(buf), "USER anonymous\r\n");
    else
	snprintf(buf, sizeof(buf), "USER %s\r\n", ctxt->user);
    buf[sizeof(buf) - 1] = 0;
    len = strlen(buf);
#ifdef DEBUG_FTP
    xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) {
	__xmlIOErr(XML_FROM_FTP, 0, "send failed");
	return(res);
    }
    return(0);
}

/**
 * Send the password authentication
 */

static int
xmlNanoFTPSendPasswd(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200];
    int len;
    int res;

    if (ctxt->passwd == NULL)
	snprintf(buf, sizeof(buf), "PASS anonymous@\r\n");
    else
	snprintf(buf, sizeof(buf), "PASS %s\r\n", ctxt->passwd);
    buf[sizeof(buf) - 1] = 0;
    len = strlen(buf);
#ifdef DEBUG_FTP
    xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) {
	__xmlIOErr(XML_FROM_FTP, 0, "send failed");
	return(res);
    }
    return(0);
}

/**
 * xmlNanoFTPQuit:
 * @ctx:  an FTP context
 *
 * Send a QUIT command to the server
 *
 * Returns -1 in case of error, 0 otherwise
 */


int
xmlNanoFTPQuit(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    char buf[200];
    int len, res;

    if ((ctxt == NULL) || (ctxt->controlFd < 0)) return(-1);

    snprintf(buf, sizeof(buf), "QUIT\r\n");
    len = strlen(buf);
#ifdef DEBUG_FTP
    xmlGenericError(xmlGenericErrorContext, "%s", buf); /* Just to be consistent, even though we know it can't have a % in it */
#endif
    res = send(ctxt->controlFd, buf, len, 0);
    if (res < 0) {
	__xmlIOErr(XML_FROM_FTP, 0, "send failed");
	return(res);
    }
    return(0);
}

/**
 * xmlNanoFTPConnect:
 * @ctx:  an FTP context
 *
 * Tries to open a control connection
 *
 * Returns -1 in case of error, 0 otherwise
 */

int
xmlNanoFTPConnect(void *ctx) {
    xmlNanoFTPCtxtPtr ctxt = (xmlNanoFTPCtxtPtr) ctx;
    struct hostent *hp;
    int port;
    int res;
    int addrlen = sizeof (struct sockaddr_in);

    if (ctxt == NULL)
	return(-1);
    if (ctxt->hostname == NULL)
	return(-1);

    /*
     * do the blocking DNS query.
     */
    if (proxy) {
        port = proxyPort;
    } else {
	port = ctxt->port;
    }
    if (port == 0)
	port = 21;

    memset (&ctxt->ftpAddr, 0, sizeof(ctxt->ftpAddr));

#ifdef SUPPORT_IP6
    if (have_ipv6 ()) {
	struct addrinfo hints, *tmp, *result;

	result = NULL;
	memset (&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	if (proxy) {
	    if (getaddrinfo (proxy, NULL, &hints, &result) != 0) {
		__xmlIOErr(XML_FROM_FTP, 0, "getaddrinfo failed");
		return (-1);
	    }
	}
	else
	    if (getaddrinfo (ctxt->hostname, NULL, &hints, &result) != 0) {
		__xmlIOErr(XML_FROM_FTP, 0, "getaddrinfo failed");
		return (-1);
	    }

	for (tmp = result; tmp; tmp = tmp->ai_next)
	    if (tmp->ai_family == AF_INET || tmp->ai_family == AF_INET6)
		break;

	if (!tmp) {
	    if (result)
		freeaddrinfo (result);
	    __xmlIOErr(XML_FROM_FTP, 0, "getaddrinfo failed");
	    return (-1);
	}
	if (tmp->ai_addrlen > sizeof(ctxt->ftpAddr)) {
	    __xmlIOErr(XML_FROM_FTP, 0, "gethostbyname address mismatch");
	    return (-1);
	}
	if (tmp->ai_family == AF_INET6) {
	    memcpy (&ctxt->ftpAddr, tmp->ai_addr, tmp->ai_addrlen);
	    ((struct sockaddr_in6 *) &ctxt->ftpAddr)->sin6_port = htons (port);
	    ctxt->controlFd = socket (AF_INET6, SOCK_STREAM, 0);
	}
	else {
	    memcpy (&ctxt->ftpAddr, tmp->ai_addr, tmp->ai_addrlen);
	    ((struct sockaddr_in *) &ctxt->ftpAddr)->sin_port = htons (port);
	    ctxt->controlFd = socket (AF_INET, SOCK_STREAM, 0);
	}
	addrlen = tmp->ai_addrlen;
	freeaddrinfo (result);
    }
    else
#endif
    {
	if (proxy)
	    hp = gethostbyname (proxy);
	else
	    hp = gethostbyname (ctxt->hostname);
	if (hp == NULL) {
	    __xmlIOErr(XML_FROM_FTP, 0, "gethostbyname failed");
	    return (-1);
	}
	if ((unsigned int) hp->h_length >
	    sizeof(((struct sockaddr_in *)&ctxt->ftpAddr)->sin_addr)) {
	    __xmlIOErr(XML_FROM_FTP, 0, "gethostbyname address mismatch");
	    return (-1);
	}

	/*
	 * Prepare the socket
	 */
	((struct sockaddr_in *)&ctxt->ftpAddr)->sin_family = AF_INET;
	memcpy (&((struct sockaddr_in *)&ctxt->ftpAddr)->sin_addr,
		hp->h_addr_list[0], hp->h_length);
	((struct sockaddr_in *)&ctxt->ftpAddr)->sin_port = htons (port);
	ctxt->controlFd = socket (AF_INET, SOCK_STREAM, 0);
	addrlen = sizeof (struct sockaddr_in);
    }

    if (ctxt->controlFd < 0) {
	__xmlIOErr(XML_FROM_FTP, 0, "socket failed");
        return(-1);
    }

    /*
     * Do the connect.
     */
    if (connect(ctxt->controlFd, (struct sockaddr *) &ctxt->ftpAddr,
	    addrlen) < 0) {
	__xmlIOErr(XML_FROM_FTP, 0, "Failed to create a connection");
        closesocket(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
	return(-1);
    }

    /*
     * Wait for the HELLO from the server.
     */
    res = xmlNanoFTPGetResponse(ctxt);
    if (res != 2) {
        closesocket(ctxt->controlFd); ctxt->controlFd = -1;
        ctxt->controlFd = -1;
	return(-1);
    }

    /*
     * State diagram for the login operation on the FTP server
     *
     * Reference: RFC 959
     *
     *                       1
     * +---+   USER    +---+------------->+---+
     * | B |---------->| W | 2       ---->| E |
     * +---+           +---+------  |  -->+---+
     *                  | |       | | |
     *                3 | | 4,5   | | |
     *    --------------   -----  | | |
     *   |                      | | | |
     *   |                      | | | |
     *   |                 ---------  |
     *   |               1|     | |   |
     *   V                |     | |   |
     * +---+   PASS    +---+ 2  |  ------>+---+
     * |   |---------->| W |------------->| S |
     * +---+           +---+   ---------->+---+
     *                  | |   | |     |
     *                3 | |4,5| |     |
     *    --------------   --------   |
     *   |                    | |  |  |
     *   |                    | |  |  |
     *   |                 -----------
     *   |             1,3|   | |  |
     *   V                |  2| |  |
     * +---+   ACCT    +---+--  |   ----->+---+
     * |   |---------->| W | 4,5 -------->| F |
     * +---+           +---+------------->+---+
     *
     * Of course in case of using a proxy this get really nasty and is not
     * standardized at all :-(
     */
    if (proxy) {
        int len;
	char buf[400];

        if (proxyUser != NULL) {
	    /*
	     * We need proxy auth
	     */
	    snprintf(buf, sizeof(buf), "USER %s\r\n", proxyUser);
            buf[sizeof(buf) - 1] = 0;
            len = strlen(buf);
#ifdef DEBUG_FTP
	    xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
	    res = send(ctxt->controlFd, buf, len, 0);
	    if (res < 0) {
		__xmlIOErr(XML_FROM_FTP, 0, "send failed");
		closesocket(ctxt->controlFd);
		ctxt->controlFd = -1;
	        return(res);
	    }
	    res = xmlNanoFTPGetResponse(ctxt);
	    switch (res) {
		case 2:
		    if (proxyPasswd == NULL)
			break;
		case 3:
		    if (proxyPasswd != NULL)
			snprintf(buf, sizeof(buf), "PASS %s\r\n", proxyPasswd);
		    else
			snprintf(buf, sizeof(buf), "PASS anonymous@\r\n");
                    buf[sizeof(buf) - 1] = 0;
                    len = strlen(buf);
#ifdef DEBUG_FTP
		    xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
		    res = send(ctxt->controlFd, buf, len, 0);
		    if (res < 0) {
			__xmlIOErr(XML_FROM_FTP, 0, "send failed");
			closesocket(ctxt->controlFd);
			ctxt->controlFd = -1;
			return(res);
		    }
		    res = xmlNanoFTPGetResponse(ctxt);
		    if (res > 3) {
			closesocket(ctxt->controlFd);
			ctxt->controlFd = -1;
			return(-1);
		    }
		    break;
		case 1:
		    break;
		case 4:
		case 5:
		case -1:
		default:
		    closesocket(ctxt->controlFd);
		    ctxt->controlFd = -1;
		    return(-1);
	    }
	}

	/*
	 * We assume we don't need more authentication to the proxy
	 * and that it succeeded :-\
	 */
	switch (proxyType) {
	    case 0:
		/* we will try in sequence */
	    case 1:
		/* Using SITE command */
		snprintf(buf, sizeof(buf), "SITE %s\r\n", ctxt->hostname);
                buf[sizeof(buf) - 1] = 0;
                len = strlen(buf);
#ifdef DEBUG_FTP
		xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
		res = send(ctxt->controlFd, buf, len, 0);
		if (res < 0) {
		    __xmlIOErr(XML_FROM_FTP, 0, "send failed");
		    closesocket(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(res);
		}
		res = xmlNanoFTPGetResponse(ctxt);
		if (res == 2) {
		    /* we assume it worked :-\ 1 is error for SITE command */
		    proxyType = 1;
		    break;
		}    
		if (proxyType == 1) {
		    closesocket(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(-1);
		}
	    case 2:
		/* USER user@host command */
		if (ctxt->user == NULL)
		    snprintf(buf, sizeof(buf), "USER anonymous@%s\r\n",
			           ctxt->hostname);
		else
		    snprintf(buf, sizeof(buf), "USER %s@%s\r\n",
			           ctxt->user, ctxt->hostname);
                buf[sizeof(buf) - 1] = 0;
                len = strlen(buf);
#ifdef DEBUG_FTP
		xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
		res = send(ctxt->controlFd, buf, len, 0);
		if (res < 0) {
		    __xmlIOErr(XML_FROM_FTP, 0, "send failed");
		    closesocket(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(res);
		}
		res = xmlNanoFTPGetResponse(ctxt);
		if ((res == 1) || (res == 2)) {
		    /* we assume it worked :-\ */
		    proxyType = 2;
		    return(0);
		}    
		if (ctxt->passwd == NULL)
		    snprintf(buf, sizeof(buf), "PASS anonymous@\r\n");
		else
		    snprintf(buf, sizeof(buf), "PASS %s\r\n", ctxt->passwd);
                buf[sizeof(buf) - 1] = 0;
                len = strlen(buf);
#ifdef DEBUG_FTP
		xmlGenericError(xmlGenericErrorContext, "%s", buf);
#endif
		res = send(ctxt->controlFd, buf, len, 0);
		if (res < 0) {
		    __xmlIOErr(XML_FROM_FTP, 0, "send failed");
		    closesocket(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(res);
		}
		res = xmlNanoFTPGetResponse(ctxt);
		if ((res == 1) || (res == 2)) {
		    /* we assume it worked :-\ */
		    proxyType = 2;
		    return(0);
		}
		if (proxyType == 2) {
		    closesocket(ctxt->controlFd); ctxt->controlFd = -1;
		    ctxt->controlFd = -1;
		    return(-1);
		}
	    case 3:
		/*
		 * If you need support for other Proxy authentication scheme
		 * send the code or at least the sequence in 