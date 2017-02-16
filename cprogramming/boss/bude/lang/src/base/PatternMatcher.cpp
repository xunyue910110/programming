
#include "PatternMatcher.h"



#ifndef	_FNMATCH_H
#define	_FNMATCH_H	1

#ifdef	__cplusplus
extern "C" {
#endif

#if defined (__cplusplus) || (defined (__STDC__) && __STDC__)
#undef	__P
#define	__P(protos)	protos
#else /* Not C++ or ANSI C.  */
#undef	__P
#define	__P(protos)	()
/* We can get away without defining `const' here only because in this file
   it is used only inside the prototype for `fnmatch', which is elided in
   non-ANSI C where `const' is problematical.  */
#endif /* C++ or ANSI C.  */


/* We #undef these before defining them because some losing systems
   (HP-UX A.08.07 for example) define these in <unistd.h>.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0) /* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1) /* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2) /* Leading `.' is matched only explicitly.  */

#if !defined (_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 2 || defined (_GNU_SOURCE)
#define	FNM_FILE_NAME	FNM_PATHNAME /* Preferred GNU name.  */
#define	FNM_LEADING_DIR	(1 << 3) /* Ignore `/...' after a match.  */
#define	FNM_CASEFOLD	(1 << 4) /* Compare without regard to case.  */
#endif

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch __P ((const char *__pattern, const char *__string,
			 int __flags));

#ifdef	__cplusplus
}
#endif

#endif /* fnmatch.h */


/* Enable GNU extensions in fnmatch.h.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE	1
#endif

#include <errno.h>
#include <ctype.h>

#if defined STDC_HEADERS || !defined isascii
# define IN_CTYPE_DOMAIN(c) 1
#else
# define IN_CTYPE_DOMAIN(c) isascii (c)
#endif

#define ISUPPER(c) (IN_CTYPE_DOMAIN (c) && isupper (c))


#ifndef errno
extern int errno;
#endif

/* Match STRING against the filename pattern PATTERN, returning zero if
   it matches, nonzero if not.  */
int
fnmatch (const char *pattern, const char *string, int flags)
{
  register const char *p = pattern, *n = string;
  register char c;

/* Note that this evaluates C many times.  */
#define FOLD(c) ((flags & FNM_CASEFOLD) && ISUPPER ((unsigned char) (c)) \
                 ? tolower ((unsigned char) (c)) \
                 : (c))

  while ((c = *p++) != '\0')
    {
      c = FOLD (c);

      switch (c)
	{
	case '?':
	  if (*n == '\0')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_FILE_NAME) && *n == '/')
	    return FNM_NOMATCH;
	  else if ((flags & FNM_PERIOD) && *n == '.' &&
		   (n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	    return FNM_NOMATCH;
	  break;

	case '\\':
	  if (!(flags & FNM_NOESCAPE))
	    {
	      c = *p++;
	      if (c == '\0')
		/* Trailing \ loses.  */
		return FNM_NOMATCH;
	      c = FOLD (c);
	    }
	  if (FOLD (*n) != c)
	    return FNM_NOMATCH;
	  break;

	case '*':
	  if ((flags & FNM_PERIOD) && *n == '.' &&
	      (n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	    return FNM_NOMATCH;

	  for (c = *p++; c == '?' || c == '*'; c = *p++)
	    {
	      if (c == '?')
		{
		  /* A ? needs to match one character.  */
		  if (*n == '\0' || (*n == '/' && (flags & FNM_FILE_NAME)))
		    /* There isn't another character; no match.  */
		    return FNM_NOMATCH;
		  else
		    /* One character of the string is consumed in matching
		       this ? wildcard, so *??? won't match if there are
		       less than three characters.  */
		    ++n;
		}
	    }

	  if (c == '\0')
	    {
	      if ((flags & (FNM_FILE_NAME | FNM_LEADING_DIR)) == FNM_FILE_NAME)
		for (; *n != '\0'; n++)
		  if (*n == '/')
		    return FNM_NOMATCH;
	      return 0;
	    }

	  {
	    char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
	    c1 = FOLD (c1);
	    for (--p; *n != '\0'; ++n)
	      if ((c == '[' || FOLD (*n) == c1) &&
		  fnmatch (p, n, flags & ~FNM_PERIOD) == 0)
		return 0;
	      else if (*n == '/' && (flags & FNM_FILE_NAME))
		break;
	    return FNM_NOMATCH;
	  }

	case '[':
	  {
	    /* Nonzero if the sense of the character class is inverted.  */
	    register int _not;

	    if (*n == '\0')
	      return FNM_NOMATCH;

	    if ((flags & FNM_PERIOD) && *n == '.' &&
		(n == string || ((flags & FNM_FILE_NAME) && n[-1] == '/')))
	      return FNM_NOMATCH;

	    _not = (*p == '!' || *p == '^');
	    if (_not)
	      ++p;

	    c = *p++;
	    for (;;)
	      {
		register char cstart = c, cend = c;

		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  {
		    if (*p == '\0')
		      return FNM_NOMATCH;
		    cstart = cend = *p++;
		  }

		cstart = cend = FOLD (cstart);

		if (c == '\0')
		  /* [ (unterminated) loses.  */
		  return FNM_NOMATCH;

		c = *p++;
		c = FOLD (c);

		if ((flags & FNM_FILE_NAME) && c == '/')
		  /* [/] can never match.  */
		  return FNM_NOMATCH;

		if (c == '-' && *p != ']')
		  {
		    cend = *p++;
		    if (!(flags & FNM_NOESCAPE) && cend == '\\')
		      cend = *p++;
		    if (cend == '\0')
		      return FNM_NOMATCH;
		    cend = FOLD (cend);

		    c = *p++;
		  }

		if (FOLD (*n) >= cstart && FOLD (*n) <= cend)
		  goto matched;

		if (c == ']')
		  break;
	      }
	    if (!_not)
	      return FNM_NOMATCH;
	    break;

	  matched:;
	    /* Skip the rest of the [...] that already matched.  */
	    while (c != ']')
	      {
		if (c == '\0')
		  /* [... (unterminated) loses.  */
		  return FNM_NOMATCH;

		c = *p++;
		if (!(flags & FNM_NOESCAPE) && c == '\\')
		  {
		    if (*p == '\0')
		      return FNM_NOMATCH;
		    /* XXX 1003.2d11 is unclear if this is right.  */
		    ++p;
		  }
	      }
	    if (_not)
	      return FNM_NOMATCH;
	  }
	  break;

	default:
	  if (c != FOLD (*n))
	    return FNM_NOMATCH;
	}

      ++n;
    }

  if (*n == '\0')
    return 0;

  if ((flags & FNM_LEADING_DIR) && *n == '/')
    /* The FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
    return 0;

  return FNM_NOMATCH;

#undef FOLD
}


// end fnmatch.c////////////////////////



bool PatternMatcher::matches(const char *pattern, const char *str) 
{
	int flags =0 ;
	return (fnmatch(pattern,str,flags) == 0);
}
