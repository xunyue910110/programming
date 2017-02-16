/**
 * uri.c: set of generic URI related routines 
 *
 * Reference: RFC 2396
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/uri.h>
#include <libxml/globals.h>
#include <libxml/xmlerror.h>

/************************************************************************
 *									*
 *		Macros to differentiate various character type		*
 *			directly extracted from RFC 2396		*
 *									*
 ************************************************************************/

/*
 * alpha    = lowalpha | upalpha
 */
#define IS_ALPHA(x) (IS_LOWALPHA(x) || IS_UPALPHA(x))


/*
 * lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" |
 *            "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" |
 *            "u" | "v" | "w" | "x" | "y" | "z"
 */

#define IS_LOWALPHA(x) (((x) >= 'a') && ((x) <= 'z'))

/*
 * upalpha = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" |
 *           "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" |
 *           "U" | "V" | "W" | "X" | "Y" | "Z"
 */
#define IS_UPALPHA(x) (((x) >= 'A') && ((x) <= 'Z'))

#ifdef IS_DIGIT
#undef IS_DIGIT
#endif
/*
 * digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 */
#define IS_DIGIT(x) (((x) >= '0') && ((x) <= '9'))

/*
 * alphanum = alpha | digit
 */

#define IS_ALPHANUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

/*
 * hex = digit | "A" | "B" | "C" | "D" | "E" | "F" |
 *               "a" | "b" | "c" | "d" | "e" | "f"
 */

#define IS_HEX(x) ((IS_DIGIT(x)) || (((x) >= 'a') && ((x) <= 'f')) || \
	    (((x) >= 'A') && ((x) <= 'F')))

/*
 * mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
 */

#define IS_MARK(x) (((x) == '-') || ((x) == '_') || ((x) == '.') ||	\
    ((x) == '!') || ((x) == '~') || ((x) == '*') || ((x) == '\'') ||	\
    ((x) == '(') || ((x) == ')'))


/*
 * reserved = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ","
 */

#define IS_RESERVED(x) (((x) == ';') || ((x) == '/') || ((x) == '?') ||	\
        ((x) == ':') || ((x) == '@') || ((x) == '&') || ((x) == '=') ||	\
	((x) == '+') || ((x) == '$') || ((x) == ','))

/*
 * unreserved = alphanum | mark
 */

#define IS_UNRESERVED(x) (IS_ALPHANUM(x) || IS_MARK(x))

/*
 * escaped = "%" hex hex
 */

#define IS_ESCAPED(p) ((*(p) == '%') && (IS_HEX((p)[1])) &&		\
	    (IS_HEX((p)[2])))

/*
 * uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" |
 *                        "&" | "=" | "+" | "$" | ","
 */
#define IS_URIC_NO_SLASH(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||\
	        ((*(p) == ';')) || ((*(p) == '?')) || ((*(p) == ':')) ||\
	        ((*(p) == '@')) || ((*(p) == '&')) || ((*(p) == '=')) ||\
	        ((*(p) == '+')) || ((*(p) == '$')) || ((*(p) == ',')))

/*
 * pchar = unreserved | escaped | ":" | "@" | "&" | "=" | "+" | "$" | ","
 */
#define IS_PCHAR(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||	\
	        ((*(p) == ':')) || ((*(p) == '@')) || ((*(p) == '&')) ||\
	        ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||\
	        ((*(p) == ',')))

/*
 * rel_segment   = 1*( unreserved | escaped |
 *                 ";" | "@" | "&" | "=" | "+" | "$" | "," )
 */

#define IS_SEGMENT(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||	\
          ((*(p) == ';')) || ((*(p) == '@')) || ((*(p) == '&')) ||	\
	  ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||	\
	  ((*(p) == ',')))

/*
 * scheme = alpha *( alpha | digit | "+" | "-" | "." )
 */

#define IS_SCHEME(x) ((IS_ALPHA(x)) || (IS_DIGIT(x)) ||			\
	              ((x) == '+') || ((x) == '-') || ((x) == '.'))

/*
 * reg_name = 1*( unreserved | escaped | "$" | "," |
 *                ";" | ":" | "@" | "&" | "=" | "+" )
 */

#define IS_REG_NAME(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||	\
       ((*(p) == '$')) || ((*(p) == ',')) || ((*(p) == ';')) ||		\
       ((*(p) == ':')) || ((*(p) == '@')) || ((*(p) == '&')) ||		\
       ((*(p) == '=')) || ((*(p) == '+')))

/*
 * userinfo = *( unreserved | escaped | ";" | ":" | "&" | "=" |
 *                      "+" | "$" | "," )
 */
#define IS_USERINFO(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||	\
       ((*(p) == ';')) || ((*(p) == ':')) || ((*(p) == '&')) ||		\
       ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||		\
       ((*(p) == ',')))

/*
 * uric = reserved | unreserved | escaped
 */

#define IS_URIC(p) ((IS_UNRESERVED(*(p))) || (IS_ESCAPED(p)) ||		\
	            (IS_RESERVED(*(p))))

/*                                                                              
* unwise = "{" | "}" | "|" | "\" | "^" | "[" | "]" | "`"
*/                                                                             

#define IS_UNWISE(p)                                                    \
      (((*(p) == '{')) || ((*(p) == '}')) || ((*(p) == '|')) ||         \
       ((*(p) == '\\')) || ((*(p) == '^')) || ((*(p) == '[')) ||        \
       ((*(p) == ']')) || ((*(p) == '`')))  

/*
 * Skip to next pointer char, handle escaped sequences
 */

#define NEXT(p) ((*p == '%')? p += 3 : p++)

/*
 * Productions from the spec.
 *
 *    authority     = server | reg_name
 *    reg_name      = 1*( unreserved | escaped | "$" | "," |
 *                        ";" | ":" | "@" | "&" | "=" | "+" )
 *
 * path          = [ abs_path | opaque_part ]
 */

/************************************************************************
 *									*
 *			Generic URI structure functions			*
 *									*
 ************************************************************************/

/**
 * xmlCreateURI:
 *
 * Simply creates an empty xmlURI
 *
 * Returns the new structure or NULL in case of error
 */
xmlURIPtr
xmlCreateURI(void) {
    xmlURIPtr ret;

    ret = (xmlURIPtr) xmlMalloc(sizeof(xmlURI));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlCreateURI: out of memory\n");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlURI));
    return(ret);
}

/**
 * xmlSaveUri:
 * @uri:  pointer to an xmlURI
 *
 * Save the URI as an escaped string
 *
 * Returns a new string (to be deallocated by caller)
 */
xmlChar *
xmlSaveUri(xmlURIPtr uri) {
    xmlChar *ret = NULL;
    const char *p;
    int len;
    int max;

    if (uri == NULL) return(NULL);


    max = 80;
    ret = (xmlChar *) xmlMallocAtomic((max + 1) * sizeof(xmlChar));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlSaveUri: out of memory\n");
	return(NULL);
    }
    len = 0;

    if (uri->scheme != NULL) {
	p = uri->scheme;
	while (*p != 0) {
	    if (len >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret, (max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    ret[len++] = *p++;
	}
	if (len >= max) {
	    max *= 2;
	    ret = (xmlChar *) xmlRealloc(ret, (max + 1) * sizeof(xmlChar));
	    if (ret == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlSaveUri: out of memory\n");
		return(NULL);
	    }
	}
	ret[len++] = ':';
    }
    if (uri->opaque != NULL) {
	p = uri->opaque;
	while (*p != 0) {
	    if (len + 3 >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret, (max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    if (IS_RESERVED(*(p)) || IS_UNRESERVED(*(p)))
		ret[len++] = *p++;
	    else {
		int val = *(unsigned char *)p++;
		int hi = val / 0x10, lo = val % 0x10;
		ret[len++] = '%';
		ret[len++] = hi + (hi > 9? 'A'-10 : '0');
		ret[len++] = lo + (lo > 9? 'A'-10 : '0');
	    }
	}
    } else {
	if (uri->server != NULL) {
	    if (len + 3 >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret, (max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    ret[len++] = '/';
	    ret[len++] = '/';
	    if (uri->user != NULL) {
		p = uri->user;
		while (*p != 0) {
		    if (len + 3 >= max) {
			max *= 2;
			ret = (xmlChar *) xmlRealloc(ret,
				(max + 1) * sizeof(xmlChar));
			if (ret == NULL) {
			    xmlGenericError(xmlGenericErrorContext,
				    "xmlSaveUri: out of memory\n");
			    return(NULL);
			}
		    }
		    if ((IS_UNRESERVED(*(p))) ||
			((*(p) == ';')) || ((*(p) == ':')) ||
			((*(p) == '&')) || ((*(p) == '=')) ||
			((*(p) == '+')) || ((*(p) == '$')) ||
			((*(p) == ',')))
			ret[len++] = *p++;
		    else {
			int val = *(unsigned char *)p++;
			int hi = val / 0x10, lo = val % 0x10;
			ret[len++] = '%';
			ret[len++] = hi + (hi > 9? 'A'-10 : '0');
			ret[len++] = lo + (lo > 9? 'A'-10 : '0');
		    }
		}
		if (len + 3 >= max) {
		    max *= 2;
		    ret = (xmlChar *) xmlRealloc(ret,
			    (max + 1) * sizeof(xmlChar));
		    if (ret == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSaveUri: out of memory\n");
			return(NULL);
		    }
		}
		ret[len++] = '@';
	    }
	    p = uri->server;
	    while (*p != 0) {
		if (len >= max) {
		    max *= 2;
		    ret = (xmlChar *) xmlRealloc(ret,
			    (max + 1) * sizeof(xmlChar));
		    if (ret == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSaveUri: out of memory\n");
			return(NULL);
		    }
		}
		ret[len++] = *p++;
	    }
	    if (uri->port > 0) {
		if (len + 10 >= max) {
		    max *= 2;
		    ret = (xmlChar *) xmlRealloc(ret,
			    (max + 1) * sizeof(xmlChar));
		    if (ret == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSaveUri: out of memory\n");
			return(NULL);
		    }
		}
		len += snprintf((char *) &ret[len], max - len, ":%d", uri->port);
	    }
	} else if (uri->authority != NULL) {
	    if (len + 3 >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret,
			(max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    ret[len++] = '/';
	    ret[len++] = '/';
	    p = uri->authority;
	    while (*p != 0) {
		if (len + 3 >= max) {
		    max *= 2;
		    ret = (xmlChar *) xmlRealloc(ret,
			    (max + 1) * sizeof(xmlChar));
		    if (ret == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSaveUri: out of memory\n");
			return(NULL);
		    }
		}
		if ((IS_UNRESERVED(*(p))) ||
                    ((*(p) == '$')) || ((*(p) == ',')) || ((*(p) == ';')) ||
                    ((*(p) == ':')) || ((*(p) == '@')) || ((*(p) == '&')) ||
                    ((*(p) == '=')) || ((*(p) == '+')))
		    ret[len++] = *p++;
		else {
		    int val = *(unsigned char *)p++;
		    int hi = val / 0x10, lo = val % 0x10;
		    ret[len++] = '%';
		    ret[len++] = hi + (hi > 9? 'A'-10 : '0');
		    ret[len++] = lo + (lo > 9? 'A'-10 : '0');
		}
	    }
	} else if (uri->scheme != NULL) {
	    if (len + 3 >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret,
			(max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    ret[len++] = '/';
	    ret[len++] = '/';
	}
	if (uri->path != NULL) {
	    p = uri->path;
	    while (*p != 0) {
		if (len + 3 >= max) {
		    max *= 2;
		    ret = (xmlChar *) xmlRealloc(ret,
			    (max + 1) * sizeof(xmlChar));
		    if (ret == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSaveUri: out of memory\n");
			return(NULL);
		    }
		}
		if ((IS_UNRESERVED(*(p))) || ((*(p) == '/')) ||
                    ((*(p) == ';')) || ((*(p) == '@')) || ((*(p) == '&')) ||
	            ((*(p) == '=')) || ((*(p) == '+')) || ((*(p) == '$')) ||
	            ((*(p) == ',')))
		    ret[len++] = *p++;
		else {
		    int val = *(unsigned char *)p++;
		    int hi = val / 0x10, lo = val % 0x10;
		    ret[len++] = '%';
		    ret[len++] = hi + (hi > 9? 'A'-10 : '0');
		    ret[len++] = lo + (lo > 9? 'A'-10 : '0');
		}
	    }
	}
	if (uri->query != NULL) {
	    if (len + 3 >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret,
			(max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    ret[len++] = '?';
	    p = uri->query;
	    while (*p != 0) {
		if (len + 3 >= max) {
		    max *= 2;
		    ret = (xmlChar *) xmlRealloc(ret,
			    (max + 1) * sizeof(xmlChar));
		    if (ret == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlSaveUri: out of memory\n");
			return(NULL);
		    }
		}
		if ((IS_UNRESERVED(*(p))) || (IS_RESERVED(*(p)))) 
		    ret[len++] = *p++;
		else {
		    int val = *(unsigned char *)p++;
		    int hi = val / 0x10, lo = val % 0x10;
		    ret[len++] = '%';
		    ret[len++] = hi + (hi > 9? 'A'-10 : '0');
		    ret[len++] = lo + (lo > 9? 'A'-10 : '0');
		}
	    }
	}
    }
    if (uri->fragment != NULL) {
	if (len + 3 >= max) {
	    max *= 2;
	    ret = (xmlChar *) xmlRealloc(ret,
		    (max + 1) * sizeof(xmlChar));
	    if (ret == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlSaveUri: out of memory\n");
		return(NULL);
	    }
	}
	ret[len++] = '#';
	p = uri->fragment;
	while (*p != 0) {
	    if (len + 3 >= max) {
		max *= 2;
		ret = (xmlChar *) xmlRealloc(ret,
			(max + 1) * sizeof(xmlChar));
		if (ret == NULL) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlSaveUri: out of memory\n");
		    return(NULL);
		}
	    }
	    if ((IS_UNRESERVED(*(p))) || (IS_RESERVED(*(p)))) 
		ret[len++] = *p++;
	    else {
		int val = *(unsigned char *)p++;
		int hi = val / 0x10, lo = val % 0x10;
		ret[len++] = '%';
		ret[len++] = hi + (hi > 9? 'A'-10 : '0');
		ret[len++] = lo + (lo > 9? 'A'-10 : '0');
	    }
	}
    }
    if (len >= max) {
	max *= 2;
	ret = (xmlChar *) xmlRealloc(ret, (max + 1) * sizeof(xmlChar));
	if (ret == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlSaveUri: out of memory\n");
	    return(NULL);
	}
    }
    ret[len++] = 0;
    return(ret);
}

/**
 * xmlPrintURI:
 * @stream:  a FILE* for the output
 * @uri:  pointer to an xmlURI
 *
 * Prints the URI in the stream @steam.
 */
void
xmlPrintURI(FILE *stream, xmlURIPtr uri) {
    xmlChar *out;

    out = xmlSaveUri(uri);
    if (out != NULL) {
	fprintf(stream, "%s", (char *) out);
	xmlFree(out);
    }
}

/**
 * xmlCleanURI:
 * @uri:  pointer to an xmlURI
 *
 * Make sure the xmlURI struct is free of content
 */
static void
xmlCleanURI(xmlURIPtr uri) {
    if (uri == NULL) return;

    if (uri->scheme != NULL) xmlFree(uri->scheme);
    uri->scheme = NULL;
    if (uri->server != NULL) xmlFree(uri->server);
    uri->server = NULL;
    if (uri->user != NULL) xmlFree(uri->user);
    uri->user = NULL;
    if (uri->path != NULL) xmlFree(uri->path);
    uri->path = NULL;
    if (uri->fragment != NULL) xmlFree(uri->fragment);
    uri->fragment = NULL;
    if (uri->opaque != NULL) xmlFree(uri->opaque);
    uri->opaque = NULL;
    if (uri->authority != NULL) xmlFree(uri->authority);
    uri->authority = NULL;
    if (uri->query != NULL) xmlFree(uri->query);
    uri->query = NULL;
}

/**
 * xmlFreeURI:
 * @uri:  pointer to an xmlURI
 *
 * Free up the xmlURI struct
 */
void
xmlFreeURI(xmlURIPtr uri) {
    if (uri == NULL) return;

    if (uri->scheme != NULL) xmlFree(uri->scheme);
    if (uri->server != NULL) xmlFree(uri->server);
    if (uri->user != NULL) xmlFree(uri->user);
    if (uri->path != NULL) xmlFree(uri->path);
    if (uri->fragment != NULL) xmlFree(uri->fragment);
    if (uri->opaque != NULL) xmlFree(uri->opaque);
    if (uri->authority != NULL) xmlFree(uri->authority);
    if (uri->query != NULL) xmlFree(uri->query);
    xmlFree(uri);
}

/************************************************************************
 *									*
 *			Helper functions				*
 *									*
 ************************************************************************/

/**
 * xmlNormalizeURIPath:
 * @path:  pointer to the path string
 *
 * Applies the 5 normalization steps to a path string--that is, RFC 2396
 * Section 5.2, steps 6.c through 6.g.
 *
 * Normalization occurs directly on the string, no new allocation is done
 *
 * Returns 0 or an error code
 */
int
xmlNormalizeURIPath(char *path) {
    char *cur, *out;

    if (path == NULL)
	return(-1);

    /* Skip all initial "/" chars.  We want to get to the beginning of the
     * first non-empty segment.
     */
    cur = path;
    while (cur[0] == '/')
      ++cur;
    if (cur[0] == '\0')
      return(0);

    /* Keep everything we've seen so far.  */
    out = cur;

    /*
     * Analyze each segment in sequence for cases (c) and (d).
     */
    while (cur[0] != '\0') {
	/*
	 * c) All occurrences of "./", where "." is a complete path segment,
	 *    are removed from the buffer string.
	 */
	if ((cur[0] == '.') && (cur[1] == '/')) {
	    cur += 2;
	    /* '//' normalization should be done at this point too */
	    while (cur[0] == '/')
		cur++;
	    continue;
	}

	/*
	 * d) If the buffer string ends with "." as a complete path segment,
	 *    that "." is removed.
	 */
	if ((cur[0] == '.') && (cur[1] == '\0'))
	    break;

	/* Otherwise keep the segment.  */
	while (cur[0] != '/') {
            if (cur[0] == '\0')
              goto done_cd;
	    (out++)[0] = (cur++)[0];
	}
	/* nomalize // */
	while ((cur[0] == '/') && (cur[1] == '/'))
	    cur++;

        (out++)[0] = (cur++)[0];
    }
 done_cd:
    out[0] = '\0';

    /* Reset to the beginning of the first segment for the next sequence.  */
    cur = path;
    while (cur[0] == '/')
      ++cur;
    if (cur[0] == '\0')
	return(0);

    /*
     * Analyze each segment in sequence for cases (e) and (f).
     *
     * e) All occurrences of "<segment>/../", where <segment> is a
     *    complete path segment not equal to "..", are removed from the
     *    buffer string.  Removal of these path segments is performed
     *    iteratively, removing the leftmost matching pattern on each
     *    iteration, until no matching pattern remains.
     *
     * f) If the buffer string ends with "<segment>/..", where <segment>
     *    is a complete path segment not equal to "..", that
     *    "<segment>/.." is removed.
     *
     * To satisfy the "iterative" clause in (e), we need to collapse the
     * string every time we find something that needs to be removed.  Thus,
     * we don't need to keep two pointers into the string: we only need a
     * "current position" pointer.
     */
    while (1) {
        char *segp, *tmp;

        /* At the beginning of each iteration of this loop, "cur" points to
         * the first character of the segment we want to examine.
         */

        /* Find the end of the current segment.  */
        segp = cur;
        while ((segp[0] != '/') && (segp[0] != '\0'))
          ++segp;

        /* If this is the last segment, we're done (we need at least two
         * segments to meet the criteria for the (e) and (f) cases).
         */
        if (segp[0] == '\0')
          break;

        /* If the first segment is "..", or if the next segment _isn't_ "..",
         * keep this segment and try the next one.
         */
        ++segp;
        if (((cur[0] == '.') && (cur[1] == '.') && (segp == cur+3))
            || ((segp[0] != '.') || (segp[1] != '.')
                || ((segp[2] != '/') && (segp[2] != '\0')))) {
          cur = segp;
          continue;
        }

        /* If we get here, remove this segment and the next one and back up
         * to the previous segment (if there is one), to implement the
         * "iteratively" clause.  It's pretty much impossible to back up
         * while maintaining two pointers into the buffer, so just compact
         * the whole buffer now.
         */

        /* If this is the end of the buffer, we're done.  */
        if (segp[2] == '\0') {
          cur[0] = '\0';
          break;
        }
        /* Valgrind complained, strcpy(cur, segp + 3); */
	/* string will overlap, do not use strcpy */
	tmp = cur;
	segp += 3;
	while ((*tmp++ = *segp++) != 0);

        /* If there are no previous segments, then keep going from here.  */
        segp = cur;
        while ((segp > path) && ((--segp)[0] == '/'))
          ;
        if (segp == path)
          continue;

        /* "segp" is pointing to the end of a previous segment; find it's
         * start.  We need to back up to the previous segment and start
         * over with that to handle things like "foo/bar/../..".  If we
         * don't do this, then on the first pass we'll remove the "bar/..",
         * but be pointing at the second ".." so we won't realize we can also
         * remove the "foo/..".
         */
        cur = segp;
        while ((cur > path) && (cur[-1] != '/'))
          --cur;
    }
    out[0] = '\0';

    /*
     * g) If the resulting buffer string still begins with one or more
     *    complete path segments of "..", then the reference is
     *    considered to be in error. Implementations may handle this
     *    error by retaining these components in the resolved path (i.e.,
     *    treating them as part of the final URI), by removing them from
     *    the resolved path (i.e., discarding relative levels above the
     *    root), or by avoiding traversal of the reference.
     *
     * We discard them from the final path.
     */
    if (path[0] == '/') {
      cur = path;
      while ((cur[0] == '/') && (cur[1] == '.') && (cur[2] == '.')
             && ((cur[3] == '/') || (cur[3] == '\0')))
	cur += 3;

      if (cur != path) {
	out = path;
	while (cur[0] != '\0')
          (out++)[0] = (cur++)[0];
	out[0] = 0;
      }
    }

    return(0);
}

static int is_hex(char c) {
    if (((c >= '0') && (c <= '9')) ||
        ((c >= 'a') && (c <= 'f')) ||
        ((c >= 'A') && (c <= 'F')))
	return(1);
    return(0);
}

/**
 * xmlURIUnescapeString:
 * @str:  the string to unescape
 * @len:   the length in bytes to unescape (or <= 0 to indicate full string)
 * @target:  optional destination buffer
 *
 * Unescaping routine, does not do validity checks !
 * Output is direct unsigned char translation of %XX values (no encoding)
 *
 * Returns an copy of the string, but unescaped
 */
char *
xmlURIUnescapeString(const char *str, int len, char *target) {
    char *ret, *out;
    const char *in;

    if (str == NULL)
	return(NULL);
    if (len <= 0) len = strlen(str);
    if (len < 0) return(NULL);

    if (target == NULL) {
	ret = (char *) xmlMallocAtomic(len + 1);
	if (ret == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlURIUnescapeString: out of memory\n");
	    return(NULL);
	}
    } else
	ret = target;
    in = str;
    out = ret;
    while(len > 0) {
	if ((len > 2) && (*in == '%') && (is_hex(in[1])) && (is_hex(in[2]))) {
	    in++;
	    if ((*in >= '0') && (*in <= '9')) 
	        *out = (*in - '0');
	    else if ((*in >= 'a') && (*in <= 'f'))
	        *out = (*in - 'a') + 10;
	    else if ((*in >= 'A') && (*in <= 'F'))
	        *out = (*in - 'A') + 10;
	    in++;
	    if ((*in >= '0') && (*in <= '9')) 
	        *out = *out * 16 + (*in - '0');
	    else if ((*in >= 'a') && (*in <= 'f'))
	        *out = *out * 16 + (*in - 'a') + 10;
	    else if ((*in >= 'A') && (*in <= 'F'))
	        *out = *out * 16 + (*in - 'A') + 10;
	    in++;
	    len -= 3;
	    out++;
	} else {
	    *out++ = *in++;
	    len--;
	}
    }
    *out = 0;
    return(ret);
}

/**
 * xmlURIEscapeStr:
 * @str:  string to escape
 * @list: exception list string of chars not to escape
 *
 * This routine escapes a string to hex, ignoring reserved characters (a-z)
 * and the characters in the exception list.
 *
 * Returns a new escaped string or NULL in case of error.
 */
xmlChar *
xmlURIEscapeStr(const xmlChar *str, const xmlChar *list) {
    xmlChar *ret, ch;
    const xmlChar *in;

    unsigned int len, out;

    if (str == NULL)
	return(NULL);
    len = xmlStrlen(str);
    if (!(len > 0)) return(NULL);

    len += 20;
    ret = (xmlChar *) xmlMallocAtomic(len);
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlURIEscapeStr: out of memory\n");
	return(NULL);
    }
    in = (const xmlChar *) str;
    out = 0;
    while(*in != 0) {
	if (len - out <= 3) {
	    len += 20;
	    ret = (xmlChar *) xmlRealloc(ret, len);
	    if (ret == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xmlURIEscapeStr: out of memory\n");
		return(NULL);
	    }
	}

	ch = *in;

	if ((ch != '@') && (!IS_UNRESERVED(ch)) && (!xmlStrchr(list, ch))) {
	    unsigned char val;
	    ret[out++] = '%';
	    val = ch >> 4;
	    if (val <= 9)
		ret[out++] = '0' + val;
	    else
		ret[out++] = 'A' + val - 0xA;
	    val = ch & 0xF;
	    if (val <= 9)
		ret[out++] = '0' + val;
	    else
		ret[out++] = 'A' + val - 0xA;
	    in++;
	} else {
	    ret[out++] = *in++;
	}

    }
    ret[out] = 0;
    return(ret);
}

/**
 * xmlURIEscape:
 * @str:  the string of the URI to escape
 *
 * Escaping routine, does not do validity checks !
 * It will try to escape the chars needing this, but this is heuristic
 * based it's impossible to be sure.
 *
 * Returns an copy of the string, but escaped
 *
 * 25 May 2001
 * Uses xmlParseURI and xmlURIEscapeStr to try to escape correctly
 * according to RFC2396.
 *   - Carl Douglas
 */
xmlChar *
xmlURIEscape(const xmlChar * str)
{
    xmlChar *ret, *segment = NULL;
    xmlURIPtr uri;
    int ret2;

#define NULLCHK(p) if(!p) { \
                   xmlGenericError(xmlGenericErrorContext, \
                        "xmlURIEscape: out of memory\n"); \
                   return NULL; }

    if (str == NULL)
        return (NULL);

    uri = xmlCreateURI();
    if (uri != NULL) {
	/*
	 * Allow escaping errors in the unescaped form
	 */
        uri->cleanup = 1;
        ret2 = xmlParseURIReference(uri, (const char *)str);
        if (ret2) {
            xmlFreeURI(uri);
            return (NULL);
        }
    }

    if (!uri)
        return NULL;

    ret = NULL;

    if (uri->scheme) {
        segment = xmlURIEscapeStr(BAD_CAST uri->scheme, BAD_CAST "+-.");
        NULLCHK(segment)
        ret = xmlStrcat(ret, segment);
        ret = xmlStrcat(ret, BAD_CAST ":");
        xmlFree(segment);
    }

    if (uri->authority) {
        segment =
            xmlURIEscapeStr(BAD_CAST uri->authority, BAD_CAST "/?;:@");
        NULLCHK(segment)
        ret = xmlStrcat(ret, BAD_CAST "//");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->user) {
        segment = xmlURIEscapeStr(BAD_CAST uri->user, BAD_CAST ";:&=+$,");
        NULLCHK(segment)
		ret = xmlStrcat(ret,BAD_CAST "//");	
        ret = xmlStrcat(ret, segment);
        ret = xmlStrcat(ret, BAD_CAST "@");
        xmlFree(segment);
    }

    if (uri->server) {
        segment = xmlURIEscapeStr(BAD_CAST uri->server, BAD_CAST "/?;:@");
        NULLCHK(segment)
		if (uri->user == NULL)
       		ret = xmlStrcat(ret, BAD_CAST "//");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->port) {
        xmlChar port[10];

        snprintf((char *) port, 10, "%d", uri->port);
        ret = xmlStrcat(ret, BAD_CAST ":");
        ret = xmlStrcat(ret, port);
    }

    if (uri->path) {
        segment =
            xmlURIEscapeStr(BAD_CAST uri->path, BAD_CAST ":@&=+$,/?;");
        NULLCHK(segment)
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->query) {
        segment =
            xmlURIEscapeStr(BAD_CAST uri->query, BAD_CAST ";/?:@&=+,$");
        NULLCHK(segment)
        ret = xmlStrcat(ret, BAD_CAST "?");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->opaque) {
        segment = xmlURIEscapeStr(BAD_CAST uri->opaque, BAD_CAST "");
        NULLCHK(segment)
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    if (uri->fragment) {
        segment = xmlURIEscapeStr(BAD_CAST uri->fragment, BAD_CAST "#");
        NULLCHK(segment)
        ret = xmlStrcat(ret, BAD_CAST "#");
        ret = xmlStrcat(ret, segment);
        xmlFree(segment);
    }

    xmlFreeURI(uri);
#undef NULLCHK

    return (ret);
}

/************************************************************************
 *									*
 *			Escaped URI parsing				*
 *									*
 ************************************************************************/

/**
 * xmlParseURIFragment:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse an URI fragment string and fills in the appropriate fields
 * of the @uri structure.
 * 
 * fragment = *uric
 *
 * Returns 0 or the error code
 */
static int
xmlParseURIFragment(xmlURIPtr uri, const char **str)
{
    const char *cur = *str;

    if (str == NULL)
        return (-1);

    while (IS_URIC(cur) || IS_UNWISE(cur))
        NEXT(cur);
    if (uri != NULL) {
        if (uri->fragment != NULL)
            xmlFree(uri->fragment);
        uri->fragment = xmlURIUnescapeString(*str, cur - *str, NULL);
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIQuery:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse the query part of an URI
 * 
 * query = *uric
 *
 * Returns 0 or the error code
 */
static int
xmlParseURIQuery(xmlURIPtr uri, const char **str)
{
    const char *cur = *str;

    if (str == NULL)
        return (-1);

    while (IS_URIC(cur) || ((uri != NULL) && (uri->cleanup) && (IS_UNWISE(cur))))
        NEXT(cur);
    if (uri != NULL) {
        if (uri->query != NULL)
            xmlFree(uri->query);
        uri->query = xmlURIUnescapeString(*str, cur - *str, NULL);
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIScheme:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse an URI scheme
 * 
 * scheme = alpha *( alpha | digit | "+" | "-" | "." )
 *
 * Returns 0 or the error code
 */
static int
xmlParseURIScheme(xmlURIPtr uri, const char **str) {
    const char *cur;

    if (str == NULL)
	return(-1);
    
    cur = *str;
    if (!IS_ALPHA(*cur))
	return(2);
    cur++;
    while (IS_SCHEME(*cur)) cur++;
    if (uri != NULL) {
	if (uri->scheme != NULL) xmlFree(uri->scheme);
	/* !!! strndup */
	uri->scheme = xmlURIUnescapeString(*str, cur - *str, NULL);
    }
    *str = cur;
    return(0);
}

/**
 * xmlParseURIOpaquePart:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse an URI opaque part
 * 
 * opaque_part = uric_no_slash *uric
 *
 * Returns 0 or the error code
 */
static int
xmlParseURIOpaquePart(xmlURIPtr uri, const char **str)
{
    const char *cur;

    if (str == NULL)
        return (-1);

    cur = *str;
    if (!(IS_URIC_NO_SLASH(cur) || ((uri != NULL) && (uri->cleanup) && (IS_UNWISE(cur))))) {
        return (3);
    }
    NEXT(cur);
    while (IS_URIC(cur) || ((uri != NULL) && (uri->cleanup) && (IS_UNWISE(cur))))
        NEXT(cur);
    if (uri != NULL) {
        if (uri->opaque != NULL)
            xmlFree(uri->opaque);
        uri->opaque = xmlURIUnescapeString(*str, cur - *str, NULL);
    }
    *str = cur;
    return (0);
}

/**
 * xmlParseURIServer:
 * @uri:  pointer to an URI structure
 * @str:  pointer to the string to analyze
 *
 * Parse a server subpart of an URI, it's a finer grain analysis
 * of the authority part.
 * 
 * server        = [ [ userinfo "@" ] hostport ]
 * userinfo      = *( unreserved | escaped |
 *                       ";" | ":" | "&" | "=" | "+" | "$" | "," )
 * hostport      = host [ ":" port ]
 * host          = hostname | IPv4address
 * hostname      = *( domainlabel "." ) toplabel [ "." ]
 * domainlabel   = alphanum | alphanum *( alphanum | "-" ) alphanum
 * toplabel      = alpha | alpha *( alphanum | "-" ) alphanum
 * IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit
 * port          = *digit
 *
 * Returns 0 or the error code
 */
static int
xmlParseURIServer(xmlURIPtr uri, const char **str) {
    const char *cur;
    const char *host, *tmp;
    const int IPmax = 4;
    int oct;

    if (str == NULL)
	return(-1);
    
    cur = *str;

    /*
     * is there an userinfo ?
     */
    while (IS_USERINFO(cur)) NEXT(cur);
    if (*cur == '@') {
	if (uri != NULL) {
	    if (uri->user != NULL) xmlFree(uri->user);
	    uri->user = xmlURIUnescapeString(*str, cur - *str, NULL);
	}
	cur++;
    } else {
	if (uri != NULL) {
	    if (uri->user != NULL) xmlFree(uri->user);
	    uri->user = NULL;
	}
        cur = *str;
    }
    /*
     * This can be empty in the case where there is no server
     */
    host = cur;
    if (*cur == '/') {
	if (uri != NULL) {
	    if (uri->authority != NULL) xmlFree(uri->authority);
	    uri->authority = NULL;
	    if (uri->server != NULL) xmlFree(uri->server);
	    uri->server = NULL;
	    uri->port = 0;
	}
	return(0);
    }
    /*
     * host part of hostport can derive either an IPV4 address
     * or an unre