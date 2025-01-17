#ifndef NCBI_CONFIG_REGEXP__H
#define NCBI_CONFIG_REGEXP__H

/*  $Id: pcre_config.h,v 1.1 2011/04/15 07:36:02 sunam Exp $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Author:  Vladimir Ivanov
 *
 * File Description:
 *   Configuration wrapper for PCRE REGEXP library in NCBI C++ Toolkit
 *
 * Based on "config.h" generated the PCRE's autuconfigure.
 *
 * ===========================================================================
 */


/* Real configuration
 */
/*#include "ncbiconf.h"
*/

/* PCRE is written in Standard C, but there are a few non-standard things it
   can cope with, allowing it to run on SunOS4 and other "close to standard" 
   systems.

   You should normally change the definitions of HAVE_STRERROR and 
   HAVE_MEMMOVE to 1. If your system has bcopy() and not memmove(), change 
   the definition of HAVE_BCOPY instead of HAVE_MEMMOVE. If your system 
   has neither bcopy() nor memmove(), leave them both as 0; an emulation 
   function will be used. */

/* Define to empty if the keyword does not work. */
/* #undef const */

/* Define to `unsigned' if <stddef.h> doesn't define size_t. */
/* #undef size_t */

/* The following two definitions are mainly for the benefit of SunOS4, which
   doesn't have the strerror() or memmove() functions that should be present in
   all Standard C libraries. The macros HAVE_STRERROR and HAVE_MEMMOVE should
   normally be defined with the value 1 for other systems, but unfortunately we
   can't make this the default because "configure" files generated by autoconf
   will only change 0 to 1; they won't change 1 to 0 if the functions are not
   found. */

#define HAVE_STRERROR 1
#define HAVE_MEMMOVE 1

/* There are some non-Unix systems that don't even have bcopy(). If this macro
   is false, an emulation is used. If HAVE_MEMMOVE is set to 1, the value of
   HAVE_BCOPY is not relevant. */

#define HAVE_BCOPY 1

/* The value of NEWLINE determines the newline character. The default is to
   leave it up to the compiler, but some sites want to force a particular 
   value. On Unix systems, "configure" can be used to override this default. */

#ifndef NEWLINE
#  define NEWLINE '\n'
#endif

/* Disable support UTF-8 by default */

#define SUPPORT_UTF8 0

/* End */


#endif /* NCBI_CONFIG_REGEXP__H */
