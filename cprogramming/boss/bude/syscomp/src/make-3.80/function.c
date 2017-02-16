/* Builtin function expansion for GNU Make.
Copyright (C) 1988, 1989, 1991-1997, 1999, 2002 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "make.h"
#include "filedef.h"
#include "variable.h"
#include "dep.h"
#include "job.h"
#include "commands.h"
#include "debug.h"

#ifdef _AMIGA
#include "amiga.h"
#endif


struct function_table_entry
  {
    const char *name;
    unsigned char len;
    unsigned char minimum_args;
    unsigned char maximum_args;
    char expand_args;
    char *(*func_ptr) PARAMS ((char *output, char **argv, const char *fname));
  };

static unsigned long
function_table_entry_hash_1 (keyv)
    const void *keyv;
{
  struct function_table_entry const *key = (struct function_table_entry const *) keyv;
  return_STRING_N_HASH_1 (key->name, key->len);
}

static unsigned long
function_table_entry_hash_2 (keyv)
    const void *keyv;
{
  struct function_table_entry const *key = (struct function_table_entry const *) keyv;
  return_STRING_N_HASH_2 (key->name, key->len);
}

static int
function_table_entry_hash_cmp (xv, yv)
    const void *xv;
    const void *yv;
{
  struct function_table_entry const *x = (struct function_table_entry const *) xv;
  struct function_table_entry const *y = (struct function_table_entry const *) yv;
  int result = x->len - y->len;
  if (result)
    return result;
  return_STRING_N_COMPARE (x->name, y->name, x->len);
}

static struct hash_table function_table;


/* Store into VARIABLE_BUFFER at O the result of scanning TEXT and replacing
   each occurrence of SUBST with REPLACE. TEXT is null-terminated.  SLEN is
   the length of SUBST and RLEN is the length of REPLACE.  If BY_WORD is
   nonzero, substitutions are done only on matches which are complete
   whitespace-delimited words.  If SUFFIX_ONLY is nonzero, substitutions are
   done only at the ends of whitespace-delimited words.  */

char *
subst_expand (o, text, subst, replace, slen, rlen, by_word, suffix_only)
     char *o;
     char *text;
     char *subst, *replace;
     unsigned int slen, rlen;
     int by_word, suffix_only;
{
  register char *t = text;
  register char *p;

  if (slen == 0 && !by_word && !suffix_only)
    {
      /* The first occurrence of "" in any string is its end.  */
      o = variable_buffer_output (o, t, strlen (t));
      if (rlen > 0)
	o = variable_buffer_output (o, replace, rlen);
      return o;
    }

  do
    {
      if ((by_word | suffix_only) && slen == 0)
	/* When matching by words, the empty string should match
	   the end of each word, rather than the end of the whole text.  */
	p = end_of_token (next_token (t));
      else
	{
	  p = sindex (t, 0, subst, slen);
	  if (p == 0)
	    {
	      /* No more matches.  Output everything left on the end.  */
	      o = variable_buffer_output (o, t, strlen (t));
	      return o;
	    }
	}

      /* Output everything before this occurrence of the string to replace.  */
      if (p > t)
	o = variable_buffer_output (o, t, p - t);

      /* If we're substituting only by fully matched words,
	 or only at the ends of words, check that this case qualifies.  */
      if ((by_word
	   && ((p > t && !isblank ((unsigned char)p[-1]))
	       || (p[slen] != '\0' && !isblank ((unsigned char)p[slen]))))
	  || (suffix_only
	      && (p[slen] != '\0' && !isblank ((unsigned char)p[slen]))))
	/* Struck out.  Output the rest of the string that is
	   no longer to be replaced.  */
	o = variable_buffer_output (o, subst, slen);
      else if (rlen > 0)
	/* Output the replacement string.  */
	o = variable_buffer_output (o, replace, rlen);

      /* Advance T past the string to be replaced.  */
      t = p + slen;
    } while (*t != '\0');

  return o;
}


/* Store into VARIABLE_BUFFER at O the result of scanning TEXT
   and replacing strings matching PATTERN with REPLACE.
   If PATTERN_PERCENT is not nil, PATTERN has already been
   run through find_percent, and PATTERN_PERCENT is the result.
   If REPLACE_PERCENT is not nil, REPLACE has already been
   run through find_percent, and REPLACE_PERCENT is the result.  */

char *
patsubst_expand (o, text, pattern, replace, pattern_percent, replace_percent)
     char *o;
     char *text;
     register char *pattern, *replace;
     register char *pattern_percent, *replace_percent;
{
  unsigned int pattern_prepercent_len, pattern_postpercent_len;
  unsigned int replace_prepercent_len, replace_postpercent_len = 0;
  char *t;
  unsigned int len;
  int doneany = 0;

  /* We call find_percent on REPLACE before checking PATTERN so that REPLACE
     will be collapsed before we call subst_expand if PATTERN has no %.  */
  if (replace_percent == 0)
    replace_percent = find_percent (replace);
  if (replace_percent != 0)
    {
      /* Record the length of REPLACE before and after the % so
	 we don't have to compute these lengths more than once.  */
      replace_prepercent_len = replace_percent - replace;
      replace_postpercent_len = strlen (replace_percent + 1);
    }
  else
    /* We store the length of the replacement
       so we only need to compute it once.  */
    replace_prepercent_len = strlen (replace);

  if (pattern_percent == 0)
    pattern_percent = find_percent (pattern);
  if (pattern_percent == 0)
    /* With no % in the pattern, this is just a simple substitution.  */
    return subst_expand (o, text, pattern, replace,
			 strlen (pattern), strlen (replace), 1, 0);

  /* Record the length of PATTERN before and after the %
     so we don't have to compute it more than once.  */
  pattern_prepercent_len = pattern_percent - pattern;
  pattern_postpercent_len = strlen (pattern_percent + 1);

  while ((t = find_next_token (&text, &len)) != 0)
    {
      int fail = 0;

      /* Is it big enough to match?  */
      if (len < pattern_prepercent_len + pattern_postpercent_len)
	fail = 1;

      /* Does the prefix match? */
      if (!fail && pattern_prepercent_len > 0
	  && (*t != *pattern
	      || t[pattern_prepercent_len - 1] != pattern_percent[-1]
	      || !strneq (t + 1, pattern + 1, pattern_prepercent_len - 1)))
	fail = 1;

      /* Does the suffix match? */
      if (!fail && pattern_postpercent_len > 0
	  && (t[len - 1] != pattern_percent[pattern_postpercent_len]
	      || t[len - pattern_postpercent_len] != pattern_percent[1]
	      || !strneq (&t[len - pattern_postpercent_len],
			  &pattern_percent[1], pattern_postpercent_len - 1)))
	fail = 1;

      if (fail)
	/* It didn't match.  Output the string.  */
	o = variable_buffer_output (o, t, len);
      else
	{
	  /* It matched.  Output the replacement.  */

	  /* Output the part of the replacement before the %.  */
	  o = variable_buffer_output (o, replace, replace_prepercent_len);

	  if (replace_percent != 0)
	    {
	      /* Output the part of the matched string that
		 matched the % in the pattern.  */
	      o = variable_buffer_output (o, t + pattern_prepercent_len,
					  len - (pattern_prepercent_len
						 + pattern_postpercent_len));
	      /* Output the part of the replacement after the %.  */
	      o = variable_buffer_output (o, replace_percent + 1,
					  replace_postpercent_len);
	    }
	}

      /* Output a space, but not if the replacement is "".  */
      if (fail || replace_prepercent_len > 0
	  || (replace_percent != 0 && len + replace_postpercent_len > 0))
	{
	  o = variable_buffer_output (o, " ", 1);
	  doneany = 1;
	}
    }
  if (doneany)
    /* Kill the last space.  */
    --o;

  return o;
}


/* Look up a function by name.  */

static const struct function_table_entry *
lookup_function (s)
     const char *s;
{
  const char *e = s;

  while (*e && ( (*e >= 'a' && *e <= 'z') || *e == '-'))
    e++;
  if (*e == '\0' || isblank ((unsigned char) *e))
    {
      struct function_table_entry function_table_entry_key;
      function_table_entry_key.name = s;
      function_table_entry_key.len = e - s;

      return hash_find_item (&function_table, &function_table_entry_key);
    }
  return 0;
}


/* Return 1 if PATTERN matches STR, 0 if not.  */

int
pattern_matches (pattern, percent, str)
     register char *pattern, *percent, *str;
{
  unsigned int sfxlen, strlength;

  if (percent == 0)
    {
      unsigned int len = strlen (pattern) + 1;
      char *new_chars = (char *) alloca (len);
      bcopy (pattern, new_chars, len);
      pattern = new_chars;
      percent = find_percent (pattern);
      if (percent == 0)
	return streq (pattern, str);
    }

  sfxlen = strlen (percent + 1);
  strlength = strlen (str);

  if (strlength < (percent - pattern) + sfxlen
      || !strneq (pattern, str, percent - pattern))
    return 0;

  return !strcmp (percent + 1, str + (strlength - sfxlen));
}


/* Find the next comma or ENDPAREN (counting nested STARTPAREN and
   ENDPARENtheses), starting at PTR before END.  Return a pointer to
   next character.

   If no next argument is found, return NULL.
*/

static char *
find_next_argument (startparen, endparen, ptr, end)
     char startparen;
     char endparen;
     const char *ptr;
     const char *end;
{
  int count = 0;

  for (; ptr < end; ++ptr)
    if (*ptr == startparen)
      ++count;

    else if (*ptr == endparen)
      {
	--count;
	if (count < 0)
	  return NULL;
      }

    else if (*ptr == ',' && !count)
      return (char *)ptr;

  /* We didn't find anything.  */
  return NULL;
}


/* Glob-expand LINE.  The returned pointer is
   only good until the next call to string_glob.  */

static char *
string_glob (line)
     char *line;
{
  static char *result = 0;
  static unsigned int length;
  register struct nameseq *chain;
  register unsigned int idx;

  chain = multi_glob (parse_file_seq
		      (&line, '\0', sizeof (struct nameseq),
		       /* We do not want parse_file_seq to strip `./'s.
			  That would break examples like:
			  $(patsubst ./%.c,obj/%.o,$(wildcard ./?*.c)).  */
		       0),
		      sizeof (struct nameseq));

  if (result == 0)
    {
      length = 100;
      result = (char *) xmalloc (100);
    }

  idx = 0;
  while (chain != 0)
    {
      register char *name = chain->name;
      unsigned int len = strlen (name);

      struct nameseq *next = chain->next;
      free ((char *) chain);
      chain = next;

      /* multi_glob will pass names without globbing metacharacters
	 through as is, but we want only files that actually exist.  */
      if (file_exists_p (name))
	{
	  if (idx + len + 1 > length)
	    {
	      length += (len + 1) * 2;
	      result = (char *) xrealloc (result, length);
	    }
	  bcopy (name, &result[idx], len);
	  idx += len;
	  result[idx++] = ' ';
	}

      free (name);
    }

  /* Kill the last space and terminate the string.  */
  if (idx == 0)
    result[0] = '\0';
  else
    result[idx - 1] = '\0';

  return result;
}

/*
  Builtin functions
 */

static char *
func_patsubst (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  o = patsubst_expand (o, argv[2], argv[0], argv[1], (char *) 0, (char *) 0);
  return o;
}


static char *
func_join (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  int doneany = 0;

  /* Write each word of the first argument directly followed
     by the corresponding word of the second argument.
     If the two arguments have a different number of words,
     the excess words are just output separated by blanks.  */
  register char *tp;
  register char *pp;
  char *list1_iterator = argv[0];
  char *list2_iterator = argv[1];
  do
    {
      unsigned int len1, len2;

      tp = find_next_token (&list1_iterator, &len1);
      if (tp != 0)
	o = variable_buffer_output (o, tp, len1);

      pp = find_next_token (&list2_iterator, &len2);
      if (pp != 0)
	o = variable_buffer_output (o, pp, len2);

      if (tp != 0 || pp != 0)
	{
	  o = variable_buffer_output (o, " ", 1);
	  doneany = 1;
	}
    }
  while (tp != 0 || pp != 0);
  if (doneany)
    /* Kill the last blank.  */
    --o;

  return o;
}


static char *
func_origin (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  /* Expand the argument.  */
  register struct variable *v = lookup_variable (argv[0], strlen (argv[0]));
  if (v == 0)
    o = variable_buffer_output (o, "undefined", 9);
  else
    switch (v->origin)
      {
      default:
      case o_invalid:
	abort ();
	break;
      case o_default:
	o = variable_buffer_output (o, "default", 7);
	break;
      case o_env:
	o = variable_buffer_output (o, "environment", 11);
	break;
      case o_file:
	o = variable_buffer_output (o, "file", 4);
	break;
      case o_env_override:
	o = variable_buffer_output (o, "environment override", 20);
	break;
      case o_command:
	o = variable_buffer_output (o, "command line", 12);
	break;
      case o_override:
	o = variable_buffer_output (o, "override", 8);
	break;
      case o_automatic:
	o = variable_buffer_output (o, "automatic", 9);
	break;
      }

  return o;
}

#ifdef VMS
# define IS_PATHSEP(c) ((c) == ']')
#else
# ifdef HAVE_DOS_PATHS
#  define IS_PATHSEP(c) ((c) == '/' || (c) == '\\')
# else
#  define IS_PATHSEP(c) ((c) == '/')
# endif
#endif


static char *
func_notdir_suffix (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  /* Expand the argument.  */
  char *list_iterator = argv[0];
  char *p2 =0;
  int doneany =0;
  unsigned int len=0;

  int is_suffix = streq (funcname, "suffix");
  int is_notdir = !is_suffix;
  while ((p2 = find_next_token (&list_iterator, &len)) != 0)
    {
      char *p = p2 + len;


      while (p >= p2 && (!is_suffix || *p != '.'))
	{
	  if (IS_PATHSEP (*p))
	    break;
	  --p;
	}

      if (p >= p2)
	{
	  if (is_notdir)
	    ++p;
	  else if (*p != '.')
	    continue;
	  o = variable_buffer_output (o, p, len - (p - p2));
	}
#ifdef HAVE_DOS_PATHS
      /* Handle the case of "d:foo/bar".  */
      else if (streq (funcname, "notdir") && p2[0] && p2[1] == ':')
	{
	  p = p2 + 2;
	  o = variable_buffer_output (o, p, len - (p - p2));
	}
#endif
      else if (is_notdir)
	o = variable_buffer_output (o, p2, len);

      if (is_notdir || p >= p2)
	{
	  o = variable_buffer_output (o, " ", 1);
	  doneany = 1;
	}
    }
  if (doneany)
    /* Kill last space.  */
    --o;


  return o;

}


static char *
func_basename_dir (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  /* Expand the argument.  */
  char *p3 = argv[0];
  char *p2=0;
  int doneany=0;
  unsigned int len=0;
  char *p=0;
  int is_basename= streq (funcname, "basename");
  int is_dir= !is_basename;

  while ((p2 = find_next_token (&p3, &len)) != 0)
	{
	  p = p2 + len;
	  while (p >= p2 && (!is_basename  || *p != '.'))
	    {
	      if (IS_PATHSEP (*p))
		break;
	      	    --p;
	    }

	  if (p >= p2 && (is_dir))
	    o = variable_buffer_output (o, p2, ++p - p2);
	  else if (p >= p2 && (*p == '.'))
	    o = variable_buffer_output (o, p2, p - p2);
#ifdef HAVE_DOS_PATHS
	/* Handle the "d:foobar" case */
	  else if (p2[0] && p2[1] == ':' && is_dir)
	    o = variable_buffer_output (o, p2, 2);
#endif
	  else if (is_dir)
#ifdef VMS
	    o = variable_buffer_output (o, "[]", 2);
#else
#ifndef _AMIGA
	    o = variable_buffer_output (o, "./", 2);
#else
	    ; /* Just a nop...  */
#endif /* AMIGA */
#endif /* !VMS */
	  else
	    /* The entire name is the basename.  */
	    o = variable_buffer_output (o, p2, len);

	  o = variable_buffer_output (o, " ", 1);
	  doneany = 1;
	}
      if (doneany)
	/* Kill last space.  */
	--o;


 return o;
}

static char *
func_addsuffix_addprefix (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  int fixlen = strlen (argv[0]);
  char *list_iterator = argv[1];
  int is_addprefix = streq (funcname, "addprefix");
  int is_addsuffix = !is_addprefix;

  int doneany = 0;
  char *p;
  unsigned int len;

  while ((p = find_next_token (&list_iterator, &len)) != 0)
    {
      if (is_addprefix)
	o = variable_buffer_output (o, argv[0], fixlen);
      o = variable_buffer_output (o, p, len);
      if (is_addsuffix)
	o = variable_buffer_output (o, argv[0], fixlen);
      o = variable_buffer_output (o, " ", 1);
      doneany = 1;
    }

  if (doneany)
    /* Kill last space.  */
    --o;

  return o;
}

static char *
func_subst (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  o = subst_expand (o, argv[2], argv[0], argv[1], strlen (argv[0]),
		    strlen (argv[1]), 0, 0);

  return o;
}


static char *
func_firstword (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  unsigned int i;
  char *words = argv[0];    /* Use a temp variable for find_next_token */
  char *p = find_next_token (&words, &i);

  if (p != 0)
    o = variable_buffer_output (o, p, i);

  return o;
}


static char *
func_words (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  int i = 0;
  char *word_iterator = argv[0];
  char buf[20];

  while (find_next_token (&word_iterator, (unsigned int *) 0) != 0)
    ++i;

  sprintf (buf, "%d", i);
  o = variable_buffer_output (o, buf, strlen (buf));


  return o;
}

char *
strip_whitespace (begpp, endpp)
     char **begpp;
     char **endpp;
{
  while (isspace ((unsigned char)**begpp) && *begpp <= *endpp)
    (*begpp) ++;
  while (isspace ((unsigned char)**endpp) && *endpp >= *begpp)
    (*endpp) --;
  return *begpp;
}

int
is_numeric (p)
     char *p;
{
  char *end = p + strlen (p) - 1;
  char *beg = p;
  strip_whitespace (&p, &end);

  while (p <= end)
    if (!ISDIGIT (*(p++)))  /* ISDIGIT only evals its arg once: see make.h.  */
      return 0;

  return (end - beg >= 0);
}

void
check_numeric (s, message)
     char *s;
     char *message;
{
  if (!is_numeric (s))
    fatal (reading_file, message);
}



static char *
func_word (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  char *end_p=0;
  int i=0;
  char *p=0;

  /* Check the first argument.  */
  check_numeric (argv[0], _("non-numeric first argument to `word' function"));
  i =  atoi (argv[0]);

  if (i == 0)
    fatal (reading_file, _("first argument to `word' function must be greater than 0"));


  end_p = argv[1];
  while ((p = find_next_token (&end_p, 0)) != 0)
    if (--i == 0)
      break;

  if (i == 0)
    o = variable_buffer_output (o, p, end_p - p);

  return o;
}

static char *
func_wordlist (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  int start, count;

  /* Check the arguments.  */
  check_numeric (argv[0],
		 _("non-numeric first argument to `wordlist' function"));
  check_numeric (argv[1],
		 _("non-numeric second argument to `wordlist' function"));

  start = atoi (argv[0]);
  count = atoi (argv[1]) - start + 1;

  if (count > 0)
    {
      char *p;
      char *end_p = argv[2];

      /* Find the beginning of the "start"th word.  */
      while (((p = find_next_token (&end_p, 0)) != 0) && --start)
        ;

      if (p)
        {
          /* Find the end of the "count"th word from start.  */
          while (--count && (find_next_token (&end_p, 0) != 0))
            ;

          /* Return the stuff in the middle.  */
          o = variable_buffer_output (o, p, end_p - p);
        }
    }

  return o;
}

static char*
func_findstring (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  /* Find the first occurrence of the first string in the second.  */
  int i = strlen (argv[0]);
  if (sindex (argv[1], 0, argv[0], i) != 0)
    o = variable_buffer_output (o, argv[0], i);

  return o;
}

static char *
func_foreach (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  /* expand only the first two.  */
  char *varname = expand_argument (argv[0], NULL);
  char *list = expand_argument (argv[1], NULL);
  char *body = argv[2];

  int doneany = 0;
  char *list_iterator = list;
  char *p;
  unsigned int len;
  register struct variable *var;

  push_new_variable_scope ();
  var = define_variable (varname, strlen (varname), "", o_automatic, 0);

  /* loop through LIST,  put the value in VAR and expand BODY */
  while ((p = find_next_token (&list_iterator, &len)) != 0)
    {
      char *result = 0;

      {
	char save = p[len];

	p[len] = '\0';
	free (var->value);
	var->value = (char *) xstrdup ((char*) p);
	p[len] = save;
      }

      result = allocated_variable_expand (body);

      o = variable_buffer_output (o, result, strlen (result));
      o = variable_buffer_output (o, " ", 1);
      doneany = 1;
      free (result);
    }

  if (doneany)
    /* Kill the last space.  */
    --o;

  pop_variable_scope ();
  free (varname);
  free (list);

  return o;
}

struct a_word
{
  struct a_word *next;
  struct a_word *chain;
  char *str;
  int length;
  int matched;
};

static unsigned long
a_word_hash_1 (key)
    const void *key;
{
  return_STRING_HASH_1 (((struct a_word const *) key)->str);
}

static unsigned long
a_word_hash_2 (key)
    const void *key;
{
  return_STRING_HASH_2 (((struct a_word const *) key)->str);
}

static int
a_word_hash_cmp (x, y)
    const void *x;
    const void *y;
{
  int result = ((struct a_word const *) x)->length - ((struct a_word const *) y)->length;
  if (result)
    return result;
  return_STRING_COMPARE (((struct a_word const *) x)->str,
			 ((struct a_word const *) y)->str);
}

struct a_pattern
{
  struct a_pattern *next;
  char *str;
  char *percent;
  int length;
  int save_c;
};

static char *
func_filter_filterout (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  struct a_word *wordhead;
  struct a_word **wordtail;
  struct a_word *wp;
  struct a_pattern *pathead;
  struct a_pattern **pattail;
  struct a_pattern *pp;

  struct hash_table a_word_table;
  int is_filter = streq (funcname, "filter");
  char *pat_iterator = argv[0];
  char *word_iterator = argv[1];
  int literals = 0;
  int words = 0;
  int hashing = 0;
  char *p;
  unsigned int len;

  /* Chop ARGV[0] up into patterns to match against the words.  */

  pattail = &pathead;
  while ((p = find_next_token (&pat_iterator, &len)) != 0)
    {
      struct a_pattern *pat = (struct a_pattern *) alloca (sizeof (struct a_pattern));

      *pattail = pat;
      pattail = &pat->next;

      if (*pat_iterator != '\0')
	++pat_iterator;

      pat->str = p;
      pat->length = len;
      pat->save_c = p[len];
      p[len] = '\0';
      pat->percent = find_percent (p);
      if (pat->percent == 0)
	literals++;
    }
  *pattail = 0;

  /* Chop ARGV[1] up into words to match against the patterns.  */

  wordtail = &wordhead;
  while ((p = find_next_token (&word_iterator, &len)) != 0)
    {
      struct a_word *word = (struct a_word *) alloca (sizeof (struct a_word));

      *wordtail = word;
      wordtail = &word->next;

      if (*word_iterator != '\0')
	++word_iterator;

      p[len] = '\0';
      word->str = p;
      word->length = len;
      word->matched = 0;
      word->chain = 0;
      words++;
    }
  *wordtail = 0;

  /* Only use a hash table if arg list lengths justifies the cost.  */
  hashing = (literals >= 2 && (literals * words) >= 10);
  if (hashing)
    {
      hash_init (&a_word_table, words, a_word_hash_1, a_word_hash_2, a_word_hash_cmp);
      for (wp = wordhead; wp != 0; wp = wp->next)
	{
	  struct a_word *owp = hash_insert (&a_word_table, wp);
	  if (owp)
	    wp->chain = owp;
	}
    }

  if (words)
    {
      int doneany = 0;

      /* Run each pattern through the words, killing words.  */
      for (pp = pathead; pp != 0; pp = pp->next)
	{
	  if (pp->percent)
	    for (wp = wordhead; wp != 0; wp = wp->next)
	      wp->matched |= pattern_matches (pp->str, pp->percent, wp->str);
	  else if (hashing)
	    {
	      struct a_word a_word_key;
	      a_word_key.str = pp->str;
	      a_word_key.length = pp->length;
	      wp = (struct a_word *) hash_find_item (&a_word_table, &a_word_key);
	      while (wp)
		{
		  wp->matched |= 1;
		  wp = wp->chain;
		}
	    }
	  else
	    for (wp = wordhead; wp != 0; wp = wp->next)
	      wp->matched |= (wp->length == pp->length
			      && strneq (pp->str, wp->str, wp->length));
	}

      /* Output the words that matched (or didn't, for filter-out).  */
      for (wp = wordhead; wp != 0; wp = wp->next)
	if (is_filter ? wp->matched : !wp->matched)
	  {
	    o = variable_buffer_output (o, wp->str, strlen (wp->str));
	    o = variable_buffer_output (o, " ", 1);
	    doneany = 1;
	  }

      if (doneany)
	/* Kill the last space.  */
	--o;
    }

  for (pp = pathead; pp != 0; pp = pp->next)
    pp->str[pp->length] = pp->save_c;

  if (hashing)
    hash_free (&a_word_table, 0);

  return o;
}


static char *
func_strip (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  char *p = argv[0];
  int doneany =0;

  while (*p != '\0')
    {
      int i=0;
      char *word_start=0;

      while (isspace ((unsigned char)*p))
	++p;
      word_start = p;
      for (i=0; *p != '\0' && !isspace ((unsigned char)*p); ++p, ++i)
	{}
      if (!i)
	break;
      o = variable_buffer_output (o, word_start, i);
      o = variable_buffer_output (o, " ", 1);
      doneany = 1;
    }

  if (doneany)
    /* Kill the last space.  */
    --o;
  return o;
}

/*
  Print a warning or fatal message.
*/
static char *
func_error (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  char **argvp;
  char *msg, *p;
  int len;

  /* The arguments will be broken on commas.  Rather than create yet
     another special case where function arguments aren't broken up,
     just create a format string that puts them back together.  */
  for (len=0, argvp=argv; *argvp != 0; ++argvp)
    len += strlen (*argvp) + 2;

  p = msg = (char *) alloca (len + 1);

  for (argvp=argv; argvp[1] != 0; ++argvp)
    {
      strcpy (p, *argvp);
      p += strlen (*argvp);
      *(p++) = ',';
      *(p++) = ' ';
    }
  strcpy (p, *argvp);

  if (*funcname == 'e')
    fatal (reading_file, "%s", msg);

  /* The warning function expands to the empty string.  */
  error (reading_file, "%s", msg);

  return o;
}


/*
  chop argv[0] into words, and sort them.
 */
static char *
func_sort (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  char **words = 0;
  int nwords = 0;
  register int wordi = 0;

  /* Chop ARGV[0] into words and put them in WORDS.  */
  char *t = argv[0];
  char *p;
  unsigned int len;
  int i;

  while ((p = find_next_token (&t, &len)) != 0)
    {
      if (wordi >= nwords - 1)
	{
	  nwords = (2 * nwords) + 5;
	  words = (char **) xrealloc ((char *) words,
				      nwords * sizeof (char *));
	}
      words[wordi++] = savestring (p, len);
    }

  if (!wordi)
    return o;

  /* Now sort the list of words.  */
  qsort ((char *) words, wordi, sizeof (char *), alpha_compare);

  /* Now write the sorted list.  */
  for (i = 0; i < wordi; ++i)
    {
      len = strlen (words[i]);
      if (i == wordi - 1 || strlen (words[i + 1]) != len
          || strcmp (words[i], words[i + 1]))
        {
          o = variable_buffer_output (o, words[i], len);
          o = variable_buffer_output (o, " ", 1);
        }
      free (words[i]);
    }
  /* Kill the last space.  */
  --o;

  free (words);

  return o;
}

/*
  $(if condition,true-part[,false-part])

  CONDITION is false iff it evaluates to an empty string.  White
  space before and after condition are stripped before evaluation.

  If CONDITION is true, then TRUE-PART is evaluated, otherwise FALSE-PART is
  evaluated (if it exists).  Because only one of the two PARTs is evaluated,
  you can use $(if ...) to create side-effects (with $(shell ...), for
  example).
*/

static char *
func_if (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  char *begp = argv[0];
  char *endp = begp + strlen (argv[0]);
  int result = 0;

  /* Find the result of the condition: if we have a value, and it's not
     empty, the condition is true.  If we don't have a value, or it's the
     empty string, then it's false.  */

  strip_whitespace (&begp, &endp);

  if (begp < endp)
    {
      char *expansion = expand_argument (begp, NULL);

      result = strlen (expansion);
      free (expansion);
    }

  /* If the result is true (1) we want to eval the first argument, and if
     it's false (0) we want to eval the second.  If the argument doesn't
     exist we do nothing, otherwise expand it and add to the buffer.  */

  argv += 1 + !result;

  if (argv[0])
    {
      char *expansion;

      expansion = expand_argument (argv[0], NULL);

      o = variable_buffer_output (o, expansion, strlen (expansion));

      free (expansion);
    }

  return o;
}

static char *
func_wildcard (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{

#ifdef _AMIGA
   o = wildcard_expansion (argv[0], o);
#else
   char *p = string_glob (argv[0]);
   o = variable_buffer_output (o, p, strlen (p));
#endif
   return o;
}

/*
  $(eval <makefile string>)

  Always resolves to the empty string.

  Treat the arguments as a segment of makefile, and parse them.
*/

static char *
func_eval (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  eval_buffer (argv[0]);

  return o;
}


static char *
func_value (o, argv, funcname)
     char *o;
     char **argv;
     const char *funcname;
{
  /* Look up the variable.  */
  struct variable *v = lookup_variable (argv[0], strlen (argv[0]));

  /* Copy its value into the output buffer without expanding it.  */
  if (v)
    o = variable_buffer_output (o, v->value, strlen(v->value));

  return o;
}

/*
  \r  is replaced on UNIX as well. Is this desirable?
 */
void
fold_newlines (buffer, length)
     char *buffer;
     int *length;
{
  char *dst = buffer;
  char *src = buffer;
  char *last_nonnl = buffer -1;
  src[*length] = 0;
  for (; *src != '\0'; ++src)
    {
      if (src[0] == '\r' && src[1] == '\n')
	continue;
      if (*src == '\n')
	{
	  *dst++ = ' ';
	}
      else
	{
	  last_nonnl = dst;
	  *dst++ = *src;
	}
    }
  *(++last_nonnl) = '\0';
  *length = last_nonnl - buffer;
}



int shell_function_pid = 0, shell_function_completed;


#ifdef WINDOWS32
/*untested*/

#include <windows.h>
#include <io.h>
#include "sub_proc.h"


void
windows32_openpipe (int *pipedes, int *pid_p, char **command_argv, char **envp)
{
  SECURITY_ATTRIBUTES saAttr;
  HANDLE hIn;
  HANDLE hErr;
  HANDLE hChildOutRd;
  HANDLE hChildOutWr;
  HANDLE hProcess;


  saAttr.nLength = sizeof (SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  if (DuplicateHandle (GetCurrentProcess(),
		      GetStdHandle(STD_INPUT_HANDLE),
		      GetCurrentProcess(),
		      &hIn,
		      0,
		      TRUE,
		      DUPLICATE_SAME_ACCESS) == FALSE) {
    fatal (NILF, _("create_child_process: DuplicateHandle(In) failed (e=%d)\n"),
	   GetLastError());

  }
  if (DuplicateHandle(GetCurrentProcess(),
		      GetStdHandle(STD_ERROR_HANDLE),
		      GetCurrentProcess(),
		      &hErr,
		      0,
		      TRUE,
		      DUPLICATE_SAME_ACCESS) == FALSE) {
    fatal (NILF, _("create_child_process: DuplicateHandle(Err) failed (e=%d)\n"),
	   GetLastError());
  }

  if (!CreatePipe(&hChildOutRd, &hChildOutWr, &saAttr, 0))
    fatal (NILF, _("CreatePipe() failed (e=%d)\n"), GetLastError());

  hProcess = process_init_fd(hIn, hChildOutWr, hErr);

  if (!hProcess)
    fatal (NILF, _("windows32_openpipe (): process_init_fd() failed\n"));

  /* make sure that CreateProcess() has Path it needs */
  sync_Path_environment();

  if (!process_begin(hProcess, command_argv, envp, command_argv[0], NULL)) {
    /* register process for wait */
    process_register(hProcess);

    /* set the pid for returning to caller */
    *pid_p = (int) hProcess;

  /* set up to read data from child */
  pipedes[0] = _open_osfhandle((long) hChildOutRd, O_RDONLY);

  /* this will be closed almost right away */
  pipedes[1] = _open_osfhandle((long) hChildOutWr, O_APPEND);
  } else {
    /* reap/cleanup the failed process */
	process_cleanup(hProcess);

    /* close handles which were duplicated, they weren't used */
	CloseHandle(hIn);
	CloseHandle(hErr);

	/* close pipe ha