/*
 * encoding.c : implements the encoding conversion functions needed for XML
 *
 * Related specs: 
 * rfc2044        (UTF-8 and UTF-16) F. Yergeau Alis Technologies
 * rfc2781        UTF-16, an encoding of ISO 10646, P. Hoffman, F. Yergeau
 * [ISO-10646]    UTF-8 and UTF-16 in Annexes
 * [ISO-8859-1]   ISO Latin-1 characters codes.
 * [UNICODE]      The Unicode Consortium, "The Unicode Standard --
 *                Worldwide Character Encoding -- Version 1.0", Addison-
 *                Wesley, Volume 1, 1991, Volume 2, 1992.  UTF-8 is
 *                described in Unicode Technical Report #4.
 * [US-ASCII]     Coded Character Set--7-bit American Standard Code for
 *                Information Interchange, ANSI X3.4-1986.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * Original code for IsoLatin1 and UTF-16 by "Martin J. Duerst" <duerst@w3.org>
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef LIBXML_ICONV_ENABLED
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#endif
#include <libxml/encoding.h>
#include <libxml/xmlmemory.h>
#ifdef LIBXML_HTML_ENABLED
#include <libxml/HTMLparser.h>
#endif
#include <libxml/globals.h>
#include <libxml/xmlerror.h>

static xmlCharEncodingHandlerPtr xmlUTF16LEHandler = NULL;
static xmlCharEncodingHandlerPtr xmlUTF16BEHandler = NULL;

typedef struct _xmlCharEncodingAlias xmlCharEncodingAlias;
typedef xmlCharEncodingAlias *xmlCharEncodingAliasPtr;
struct _xmlCharEncodingAlias {
    const char *name;
    const char *alias;
};

static xmlCharEncodingAliasPtr xmlCharEncodingAliases = NULL;
static int xmlCharEncodingAliasesNb = 0;
static int xmlCharEncodingAliasesMax = 0;

#ifdef LIBXML_ICONV_ENABLED
#if 0
#define DEBUG_ENCODING  /* Define this to get encoding traces */
#endif
#else
#ifdef LIBXML_ISO8859X_ENABLED
static void xmlRegisterCharEncodingHandlersISO8859x (void);
#endif
#endif

static int xmlLittleEndian = 1;


/************************************************************************
 *									*
 *		Conversions To/From UTF8 encoding			*
 *									*
 ************************************************************************/

/**
 * asciiToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ASCII chars
 * @inlen:  the length of @in
 *
 * Take a block of ASCII chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
static int
asciiToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    const unsigned char* processed = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    unsigned int c;
    int bits;

    inend = in + (*inlen);
    while ((in < inend) && (out - outstart + 5 < *outlen)) {
	c= *in++;

	/* assertion: c is a single UTF-4 value */
        if (out >= outend)
	    break;
        if      (c <    0x80) {  *out++=  c;                bits= -6; }
        else { 
	    *outlen = out - outstart;
	    *inlen = processed - base;
	    return(-1);
	}
 
        for ( ; bits >= 0; bits-= 6) {
            if (out >= outend)
	        break;
            *out++= ((c >> bits) & 0x3F) | 0x80;
        }
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlen = processed - base;
    return(*outlen);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * UTF8Toascii:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ASCII
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
static int
UTF8Toascii(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if ((out == NULL) || (outlen == NULL) || (inlen == NULL)) return(-1);
    if (in == NULL) {
        /*
	 * initialization nothing to do
	 */
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
	d = *in++;
	if      (d < 0x80)  { c= d; trailing= 0; }
	else if (d < 0xC0) {
	    /* trailing byte in leading position */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
	else {
	    /* no chance for this in Ascii */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}

	if (inend - in < trailing) {
	    break;
	} 

	for ( ; trailing; trailing--) {
	    if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
		break;
	    c <<= 6;
	    c |= d & 0x3F;
	}

	/* assertion: c is a single UTF-4 value */
	if (c < 0x80) {
	    if (out >= outend)
		break;
	    *out++ = c;
	} else {
	    /* no chance for this in Ascii */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}
	processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(*outlen);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * isolat1ToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Take a block of ISO Latin 1 chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int
isolat1ToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend;
    const unsigned char* inend;
    const unsigned char* instop;

    if ((out == NULL) || (in == NULL) || (outlen == NULL) || (inlen == NULL))
	return(-1);

    outend = out + *outlen;
    inend = in + (*inlen);
    instop = inend;
    
    while (in < inend && out < outend - 1) {
    	if (*in >= 0x80) {
	    *out++ = (((*in) >>  6) & 0x1F) | 0xC0;
        *out++ = ((*in) & 0x3F) | 0x80;
	    ++in;
	}
	if (instop - in > outend - out) instop = in + (outend - out); 
	while (in < instop && *in < 0x80) {
	    *out++ = *in++;
	}
    }	
    if (in < inend && out < outend && *in < 0x80) {
        *out++ = *in++;
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return(*outlen);
}

/**
 * UTF8ToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-8 chars
 * @inlenb:  the length of @in in UTF-8 chars
 *
 * No op copy operation for UTF8 handling.
 *
 * Returns the number of bytes written, or -1 if lack of space.
 *     The value of *inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 */
static int
UTF8ToUTF8(unsigned char* out, int *outlen,
           const unsigned char* inb, int *inlenb)
{
    int len;

    if ((out == NULL) || (inb == NULL) || (outlen == NULL) || (inlenb == NULL))
	return(-1);
    if (*outlen > *inlenb) {
	len = *inlenb;
    } else {
	len = *outlen;
    }
    if (len < 0)
	return(-1);

    memcpy(out, inb, len);

    *outlen = len;
    *inlenb = len;
    return(*outlen);
}


#ifdef LIBXML_OUTPUT_ENABLED
/**
 * UTF8Toisolat1:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ISO Latin 1
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of octets consumed.
 */
int
UTF8Toisolat1(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if ((out == NULL) || (outlen == NULL) || (inlen == NULL)) return(-1);
    if (in == NULL) {
        /*
	 * initialization nothing to do
	 */
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
	d = *in++;
	if      (d < 0x80)  { c= d; trailing= 0; }
	else if (d < 0xC0) {
	    /* trailing byte in leading position */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
	else {
	    /* no chance for this in IsoLat1 */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}

	if (inend - in < trailing) {
	    break;
	} 

	for ( ; trailing; trailing--) {
	    if (in >= inend)
		break;
	    if (((d= *in++) & 0xC0) != 0x80) {
		*outlen = out - outstart;
		*inlen = processed - instart;
		return(-2);
	    }
	    c <<= 6;
	    c |= d & 0x3F;
	}

	/* assertion: c is a single UTF-4 value */
	if (c <= 0xFF) {
	    if (out >= outend)
		break;
	    *out++ = c;
	} else {
	    /* no chance for this in IsoLat1 */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}
	processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(*outlen);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * UTF16LEToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-16LE passwd as a byte array
 * @inlenb:  the length of @in in UTF-16LE chars
 *
 * Take a block of UTF-16LE ushorts in and try to convert it to an UTF-8
 * block of chars out. This function assumes the endian property
 * is the same between the native type of this machine and the
 * inputed one.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding fails (if *in is not a valid utf16 string)
 *     The value of *inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 */
static int
UTF16LEToUTF8(unsigned char* out, int *outlen,
            const unsigned char* inb, int *inlenb)
{
    unsigned char* outstart = out;
    const unsigned char* processed = inb;
    unsigned char* outend = out + *outlen;
    unsigned short* in = (unsigned short*) inb;
    unsigned short* inend;
    unsigned int c, d, inlen;
    unsigned char *tmp;
    int bits;

    if ((*inlenb % 2) == 1)
        (*inlenb)--;
    inlen = *inlenb / 2;
    inend = in + inlen;
    while ((in < inend) && (out - outstart + 5 < *outlen)) {
        if (xmlLittleEndian) {
	    c= *in++;
	} else {
	    tmp = (unsigned char *) in;
	    c = *tmp++;
	    c = c | (((unsigned int)*tmp) << 8);
	    in++;
	}
        if ((c & 0xFC00) == 0xD800) {    /* surrogates */
	    if (in >= inend) {           /* (in > inend) shouldn't happens */
		break;
	    }
	    if (xmlLittleEndian) {
		d = *in++;
	    } else {
		tmp = (unsigned char *) in;
		d = *tmp++;
		d = d | (((unsigned int)*tmp) << 8);
		in++;
	    }
            if ((d & 0xFC00) == 0xDC00) {
                c &= 0x03FF;
                c <<= 10;
                c |= d & 0x03FF;
                c += 0x10000;
            }
            else {
		*outlen = out - outstart;
		*inlenb = processed - inb;
	        return(-2);
	    }
        }

	/* assertion: c is a single UTF-4 value */
        if (out >= outend)
	    break;
        if      (c <    0x80) {  *out++=  c;                bits= -6; }
        else if (c <   0x800) {  *out++= ((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
        else if (c < 0x10000) {  *out++= ((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
        else                  {  *out++= ((c >> 18) & 0x07) | 0xF0;  bits= 12; }
 
        for ( ; bits >= 0; bits-= 6) {
            if (out >= outend)
	        break;
            *out++= ((c >> bits) & 0x3F) | 0x80;
        }
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlenb = processed - inb;
    return(*outlen);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * UTF8ToUTF16LE:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16LE
 * block of chars out.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding failed. 
 */
static int
UTF8ToUTF16LE(unsigned char* outb, int *outlen,
            const unsigned char* in, int *inlen)
{
    unsigned short* out = (unsigned short*) outb;
    const unsigned char* processed = in;
    const unsigned char *const instart = in;
    unsigned short* outstart= out;
    unsigned short* outend;
    const unsigned char* inend= in+*inlen;
    unsigned int c, d;
    int trailing;
    unsigned char *tmp;
    unsigned short tmp1, tmp2;

    /* UTF16LE encoding has no BOM */
    if ((out == NULL) || (outlen == NULL) || (inlen == NULL)) return(-1);
    if (in == NULL) {
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    outend = out + (*outlen / 2);
    while (in < inend) {
      d= *in++;
      if      (d < 0x80)  { c= d; trailing= 0; }
      else if (d < 0xC0) {
          /* trailing byte in leading position */
	  *outlen = (out - outstart) * 2;
	  *inlen = processed - instart;
	  return(-2);
      } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
      else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
      else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
      else {
	/* no chance for this in UTF-16 */
	*outlen = (out - outstart) * 2;
	*inlen = processed - instart;
	return(-2);
      }

      if (inend - in < trailing) {
          break;
      } 

      for ( ; trailing; trailing--) {
          if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
	      break;
          c <<= 6;
          c |= d & 0x3F;
      }

      /* assertion: c is a single UTF-4 value */
        if (c < 0x10000) {
            if (out >= outend)
	        break;
	    if (xmlLittleEndian) {
		*out++ = c;
	    } else {
		tmp = (unsigned char *) out;
		*tmp = c ;
		*(tmp + 1) = c >> 8 ;
		out++;
	    }
        }
        else if (c < 0x110000) {
            if (out+1 >= outend)
	        break;
            c -= 0x10000;
	    if (xmlLittleEndian) {
		*out++ = 0xD800 | (c >> 10);
		*out++ = 0xDC00 | (c & 0x03FF);
	    } else {
		tmp1 = 0xD800 | (c >> 10);
		tmp = (unsigned char *) out;
		*tmp = (unsigned char) tmp1;
		*(tmp + 1) = tmp1 >> 8;
		out++;

		tmp2 = 0xDC00 | (c & 0x03FF);
		tmp = (unsigned char *) out;
		*tmp  = (unsigned char) tmp2;
		*(tmp + 1) = tmp2 >> 8;
		out++;
	    }
        }
        else
	    break;
	processed = in;
    }
    *outlen = (out - outstart) * 2;
    *inlen = processed - instart;
    return(*outlen);
}

/**
 * UTF8ToUTF16:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16
 * block of chars out.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding failed. 
 */
static int
UTF8ToUTF16(unsigned char* outb, int *outlen,
            const unsigned char* in, int *inlen)
{
    if (in == NULL) {
	/*
	 * initialization, add the Byte Order Mark for UTF-16LE
	 */
        if (*outlen >= 2) {
	    outb[0] = 0xFF;
	    outb[1] = 0xFE;
	    *outlen = 2;
	    *inlen = 0;
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
		    "Added FFFE Byte Order Mark\n");
#endif
	    return(2);
	}
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    return (UTF8ToUTF16LE(outb, outlen, in, inlen));
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * UTF16BEToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-16 passed as a byte array
 * @inlenb:  the length of @in in UTF-16 chars
 *
 * Take a block of UTF-16 ushorts in and try to convert it to an UTF-8
 * block of chars out. This function assumes the endian property
 * is the same between the native type of this machine and the
 * inputed one.
 *
 * Returns the number of bytes written, or -1 if lack of space, or -2
 *     if the transcoding fails (if *in is not a valid utf16 string)
 * The value of *inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 */
static int
UTF16BEToUTF8(unsigned char* out, int *outlen,
            const unsigned char* inb, int *inlenb)
{
    unsigned char* outstart = out;
    const unsigned char* processed = inb;
    unsigned char* outend = out + *outlen;
    unsigned short* in = (unsigned short*) inb;
    unsigned short* inend;
    unsigned int c, d, inlen;
    unsigned char *tmp;
    int bits;

    if ((*inlenb % 2) == 1)
        (*inlenb)--;
    inlen = *inlenb / 2;
    inend= in + inlen;
    while (in < inend) {
	if (xmlLittleEndian) {
	    tmp = (unsigned char *) in;
	    c = *tmp++;
	    c = c << 8;
	    c = c | (unsigned int) *tmp;
	    in++;
	} else {
	    c= *in++;
	} 
        if ((c & 0xFC00) == 0xD800) {    /* surrogates */
	    if (in >= inend) {           /* (in > inend) shouldn't happens */
		*outlen = out - outstart;
		*inlenb = processed - inb;
	        return(-2);
	    }
	    if (xmlLittleEndian) {
		tmp = (unsigned char *) in;
		d = *tmp++;
		d = d << 8;
		d = d | (unsigned int) *tmp;
		in++;
	    } else {
		d= *in++;
	    }
            if ((d & 0xFC00) == 0xDC00) {
                c &= 0x03FF;
                c <<= 10;
                c |= d & 0x03FF;
                c += 0x10000;
            }
            else {
		*outlen = out - outstart;
		*inlenb = processed - inb;
	        return(-2);
	    }
        }

	/* assertion: c is a single UTF-4 value */
        if (out >= outend) 
	    break;
        if      (c <    0x80) {  *out++=  c;                bits= -6; }
        else if (c <   0x800) {  *out++= ((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
        else if (c < 0x10000) {  *out++= ((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
        else                  {  *out++= ((c >> 18) & 0x07) | 0xF0;  bits= 12; }
 
        for ( ; bits >= 0; bits-= 6) {
            if (out >= outend) 
	        break;
            *out++= ((c >> bits) & 0x3F) | 0x80;
        }
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlenb = processed - inb;
    return(*outlen);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * UTF8ToUTF16BE:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16BE
 * block of chars out.
 *
 * Returns the number of byte written, or -1 by lack of space, or -2
 *     if the transcoding failed. 
 */
static int
UTF8ToUTF16BE(unsigned char* outb, int *outlen,
            const unsigned char* in, int *inlen)
{
    unsigned short* out = (unsigned short*) outb;
    const unsigned char* processed = in;
    const unsigned char *const instart = in;
    unsigned short* outstart= out;
    unsigned short* outend;
    const unsigned char* inend= in+*inlen;
    unsigned int c, d;
    int trailing;
    unsigned char *tmp;
    unsigned short tmp1, tmp2;

    /* UTF-16BE has no BOM */
    if ((outb == NULL) || (outlen == NULL) || (inlen == NULL)) return(-1);
    if (in == NULL) {
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    outend = out + (*outlen / 2);
    while (in < inend) {
      d= *in++;
      if      (d < 0x80)  { c= d; trailing= 0; }
      else if (d < 0xC0)  {
          /* trailing byte in leading position */
	  *outlen = out - outstart;
	  *inlen = processed - instart;
	  return(-2);
      } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
      else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
      else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
      else {
          /* no chance for this in UTF-16 */
	  *outlen = out - outstart;
	  *inlen = processed - instart;
	  return(-2);
      }

      if (inend - in < trailing) {
          break;
      } 

      for ( ; trailing; trailing--) {
          if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))  break;
          c <<= 6;
          c |= d & 0x3F;
      }

      /* assertion: c is a single UTF-4 value */
        if (c < 0x10000) {
            if (out >= outend)  break;
	    if (xmlLittleEndian) {
		tmp = (unsigned char *) out;
		*tmp = c >> 8;
		*(tmp + 1) = c;
		out++;
	    } else {
		*out++ = c;
	    }
        }
        else if (c < 0x110000) {
            if (out+1 >= outend)  break;
            c -= 0x10000;
	    if (xmlLittleEndian) {
		tmp1 = 0xD800 | (c >> 10);
		tmp = (unsigned char *) out;
		*tmp = tmp1 >> 8;
		*(tmp + 1) = (unsigned char) tmp1;
		out++;

		tmp2 = 0xDC00 | (c & 0x03FF);
		tmp = (unsigned char *) out;
		*tmp = tmp2 >> 8;
		*(tmp + 1) = (unsigned char) tmp2;
		out++;
	    } else {
		*out++ = 0xD800 | (c >> 10);
		*out++ = 0xDC00 | (c & 0x03FF);
	    }
        }
        else
	    break;
	processed = in;
    }
    *outlen = (out - outstart) * 2;
    *inlen = processed - instart;
    return(*outlen);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/************************************************************************
 *									*
 *		Generic encoding handling routines			*
 *									*
 ************************************************************************/

/**
 * xmlDetectCharEncoding:
 * @in:  a pointer to the first bytes of the XML entity, must be at least
 *       2 bytes long (at least 4 if encoding is UTF4 variant).
 * @len:  pointer to the length of the buffer
 *
 * Guess the encoding of the entity using the first bytes of the entity content
 * according to the non-normative appendix F of the XML-1.0 recommendation.
 * 
 * Returns one of the XML_CHAR_ENCODING_... values.
 */
xmlCharEncoding
xmlDetectCharEncoding(const unsigned char* in, int len)
{
    if (in == NULL) 
        return(XML_CHAR_ENCODING_NONE);
    if (len >= 4) {
	if ((in[0] == 0x00) && (in[1] == 0x00) &&
	    (in[2] == 0x00) && (in[3] == 0x3C))
	    return(XML_CHAR_ENCODING_UCS4BE);
	if ((in[0] == 0x3C) && (in[1] == 0x00) &&
	    (in[2] == 0x00) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UCS4LE);
	if ((in[0] == 0x00) && (in[1] == 0x00) &&
	    (in[2] == 0x3C) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UCS4_2143);
	if ((in[0] == 0x00) && (in[1] == 0x3C) &&
	    (in[2] == 0x00) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UCS4_3412);
	if ((in[0] == 0x4C) && (in[1] == 0x6F) &&
	    (in[2] == 0xA7) && (in[3] == 0x94))
	    return(XML_CHAR_ENCODING_EBCDIC);
	if ((in[0] == 0x3C) && (in[1] == 0x3F) &&
	    (in[2] == 0x78) && (in[3] == 0x6D))
	    return(XML_CHAR_ENCODING_UTF8);
	/*
	 * Although not part of the recommendation, we also
	 * attempt an "auto-recognition" of UTF-16LE and
	 * UTF-16BE encodings.
	 */
	if ((in[0] == 0x3C) && (in[1] == 0x00) &&
	    (in[2] == 0x3F) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UTF16LE);
	if ((in[0] == 0x00) && (in[1] == 0x3C) &&
	    (in[2] == 0x00) && (in[3] == 0x3F))
	    return(XML_CHAR_ENCODING_UTF16BE);
    }
    if (len >= 3) {
	/*
	 * Errata on XML-1.0 June 20 2001
	 * We now allow an UTF8 encoded BOM
	 */
	if ((in[0] == 0xEF) && (in[1] == 0xBB) &&
	    (in[2] == 0xBF))
	    return(XML_CHAR_ENCODING_UTF8);
    }
    /* For UTF-16 we can recognize by the BOM */
    if (len >= 2) {
	if ((in[0] == 0xFE) && (in[1] == 0xFF))
	    return(XML_CHAR_ENCODING_UTF16BE);
	if ((in[0] == 0xFF) && (in[1] == 0xFE))
	    return(XML_CHAR_ENCODING_UTF16LE);
    }
    return(XML_CHAR_ENCODING_NONE);
}

/**
 * xmlCleanupEncodingAliases:
 *
 * Unregisters all aliases
 */
void
xmlCleanupEncodingAliases(void) {
    int i;

    if (xmlCharEncodingAliases == NULL)
	return;

    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (xmlCharEncodingAliases[i].name != NULL)
	    xmlFree((char *) xmlCharEncodingAliases[i].name);
	if (xmlCharEncodingAliases[i].alias != NULL)
	    xmlFree((char *) xmlCharEncodingAliases[i].alias);
    }
    xmlCharEncodingAliasesNb = 0;
    xmlCharEncodingAliasesMax = 0;
    xmlFree(xmlCharEncodingAliases);
    xmlCharEncodingAliases = NULL;
}

/**
 * xmlGetEncodingAlias:
 * @alias:  the alias name as parsed, in UTF-8 format (ASCII actually)
 *
 * Lookup an encoding name for the given alias.
 * 
 * Returns NULL if not found, otherwise the original name
 */
const char *
xmlGetEncodingAlias(const char *alias) {
    int i;
    char upper[100];

    if (alias == NULL)
	return(NULL);

    if (xmlCharEncodingAliases == NULL)
	return(NULL);

    for (i = 0;i < 99;i++) {
        upper[i] = toupper(alias[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    /*
     * Walk down the list looking for a definition of the alias
     */
    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (!strcmp(xmlCharEncodingAliases[i].alias, upper)) {
	    return(xmlCharEncodingAliases[i].name);
	}
    }
    return(NULL);
}

/**
 * xmlAddEncodingAlias:
 * @name:  the encoding name as parsed, in UTF-8 format (ASCII actually)
 * @alias:  the alias name as parsed, in UTF-8 format (ASCII actually)
 *
 * Registers an alias @alias for an encoding named @name. Existing alias
 * will be overwritten.
 * 
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlAddEncodingAlias(const char *name, const char *alias) {
    int i;
    char upper[100];

    if ((name == NULL) || (alias == NULL))
	return(-1);

    for (i = 0;i < 99;i++) {
        upper[i] = toupper(alias[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    if (xmlCharEncodingAliases == NULL) {
	xmlCharEncodingAliasesNb = 0;
	xmlCharEncodingAliasesMax = 20;
	xmlCharEncodingAliases = (xmlCharEncodingAliasPtr) 
	      xmlMalloc(xmlCharEncodingAliasesMax * sizeof(xmlCharEncodingAlias));
	if (xmlCharEncodingAliases == NULL)
	    return(-1);
    } else if (xmlCharEncodingAliasesNb >= xmlCharEncodingAliasesMax) {
	xmlCharEncodingAliasesMax *= 2;
	xmlCharEncodingAliases = (xmlCharEncodingAliasPtr) 
	      xmlRealloc(xmlCharEncodingAliases,
		         xmlCharEncodingAliasesMax * sizeof(xmlCharEncodingAlias));
    }
    /*
     * Walk down the list looking for a definition of the alias
     */
    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (!strcmp(xmlCharEncodingAliases[i].alias, upper)) {
	    /*
	     * Replace the definition.
	     */
	    xmlFree((char *) xmlCharEncodingAliases[i].name);
	    xmlCharEncodingAliases[i].name = xmlMemStrdup(name);
	    return(0);
	}
    }
    /*
     * Add the definition
     */
    xmlCharEncodingAliases[xmlCharEncodingAliasesNb].name = xmlMemStrdup(name);
    xmlCharEncodingAliases[xmlCharEncodingAliasesNb].alias = xmlMemStrdup(upper);
    xmlCharEncodingAliasesNb++;
    return(0);
}

/**
 * xmlDelEncodingAlias:
 * @alias:  the alias name as parsed, in UTF-8 format (ASCII actually)
 *
 * Unregisters an encoding alias @alias
 * 
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlDelEncodingAlias(const char *alias) {
    int i;

    if (alias == NULL)
	return(-1);

    if (xmlCharEncodingAliases == NULL)
	return(-1);
    /*
     * Walk down the list looking for a definition of the alias
     */
    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (!strcmp(xmlCharEncodingAliases[i].alias, alias)) {
	    xmlFree((char *) xmlCharEncodingAliases[i].name);
	    xmlFree((char *) xmlCharEncodingAliases[i].alias);
	    xmlCharEncodingAliasesNb--;
	    memmove(&xmlCharEncodingAliases[i], &xmlCharEncodingAliases[i + 1],
		    sizeof(xmlCharEncodingAlias) * (xmlCharEncodingAliasesNb - i));
	    return(0);
	}
    }
    return(-1);
}

/**
 * xmlParseCharEncoding:
 * @name:  the encoding name as parsed, in UTF-8 format (ASCII actually)
 *
 * Compare the string to the encoding schemes already known. Note
 * that the comparison is case insensitive accordingly to the section
 * [XML] 4.3.3 Character Encoding in Entities.
 * 
 * Returns one of the XML_CHAR_ENCODING_... values or XML_CHAR_ENCODING_NONE
 * if not recognized.
 */
xmlCharEncoding
xmlParseCharEncoding(const char* name)
{
    const char *alias;
    char upper[500];
    int i;

    if (name == NULL)
	return(XML_CHAR_ENCODING_NONE);

    /*
     * Do the alias resolution
     */
    alias = xmlGetEncodingAlias(name);
    if (alias != NULL)
	name = alias;

    for (i = 0;i < 499;i++) {
        upper[i] = toupper(name[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    if (!strcmp(upper, "")) return(XML_CHAR_ENCODING_NONE);
    if (!strcmp(upper, "UTF-8")) return(XML_CHAR_ENCODING_UTF8);
    if (!strcmp(upper, "UTF8")) return(XML_CHAR_ENCODING_UTF8);

    /*
     * NOTE: if we were able to parse this, the endianness of UTF16 is
     *       already found and in use
     */
    if (!strcmp(upper, "UTF-16")) return(XML_CHAR_ENCODING_UTF16LE);
    if (!strcmp(upper, "UTF16")) return(XML_CHAR_ENCODING_UTF16LE);
    
    if (!strcmp(upper, "ISO-10646-UCS-2")) return(XML_CHAR_ENCODING_UCS2);
    if (!strcmp(upper, "UCS-2")) return(XML_CHAR_ENCODING_UCS2);
    if (!strcmp(upper, "UCS2")) return(XML_CHAR_ENCODING_UCS2);

    /*
     * NOTE: if we were able to parse this, the endianness of UCS4 is
     *       already found and in use
     */
    if (!strcmp(upper, "ISO-10646-UCS-4")) return(XML_CHAR_ENCODING_UCS4LE);
    if (!strcmp(upper, "UCS-4")) return(XML_CHAR_ENCODING_UCS4LE);
    if (!strcmp(upper, "UCS4")) return(XML_CHAR_ENCODING_UCS4LE);

    
    if (!strcmp(upper,  "ISO-8859-1")) return(XML_CHAR_ENCODING_8859_1);
    if (!strcmp(upper,  "ISO-LATIN-1")) return(XML_CHAR_ENCODING_8859_1);
    if (!strcmp(upper,  "ISO LATIN 1")) return(XML_CHAR_ENCODING_8859_1);

    if (!strcmp(upper,  "ISO-8859-2")) return(XML_CHAR_ENCODING_8859_2);
    if (!strcmp(upper,  "ISO-LATIN-2")) return(XML_CHAR_ENCODING_8859_2);
    if (!strcmp(upper,  "ISO LATIN 2")) return(XML_CHAR_ENCODING_8859_2);

    if (!strcmp(upper,  "ISO-8859-3")) return(XML_CHAR_ENCODING_8859_3);
    if (!strcmp(upper,  "ISO-8859-4")) return(XML_CHAR_ENCODING_8859_4);
    if (!strcmp(upper,  "ISO-8859-5")) return(XML_CHAR_ENCODING_8859_5);
    if (!strcmp(upper,  "ISO-8859-6")) return(XML_CHAR_ENCODING_8859_6);
    if (!strcmp(upper,  "ISO-8859-7")) return(XML_CHAR_ENCODING_8859_7);
    if (!strcmp(upper,  "ISO-8859-8")) return(XML_CHAR_ENCODING_8859_8);
    if (!strcmp(upper,  "ISO-8859-9")) return(XML_CHAR_ENCODING_8859_9);

    if (!strcmp(upper, "ISO-2022-JP")) return(XML_CHAR_ENCODING_2022_JP);
    if (!strcmp(upper, "SHIFT_JIS")) return(XML_CHAR_ENCODING_SHIFT_JIS);
    if (!strcmp(upper, "EUC-JP")) return(XML_CHAR_ENCODING_EUC_JP);

#ifdef DEBUG_ENCODING
    xmlGenericError(xmlGenericErrorContext, "Unknown encoding %s\n", name);
#endif
    return(XML_CHAR_ENCODING_ERROR);
}

/**
 * xmlGetCharEncodingName:
 * @enc:  the encoding
 *
 * The "canonical" name for XML encoding.
 * C.f. http://www.w3.org/TR/REC-xml#charencoding
 * Section 4.3.3  Character Encoding in Entities
 *
 * Returns the canonical name for the given encoding
 */

const char*
xmlGetCharEncodingName(xmlCharEncoding enc) {
    switch (enc) {
        case XML_CHAR_ENCODING_ERROR:
	    return(NULL);
        case XML_CHAR_ENCODING_NONE:
	    return(NULL);
        case XML_CHAR_ENCODING_UTF8:
	    return("UTF-8");
        case XML_CHA