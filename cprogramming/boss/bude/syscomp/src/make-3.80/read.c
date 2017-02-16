/* Reading and parsing of makefiles for GNU Make.
Copyright (C) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997,
2002 Free Software Foundation, Inc.
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

#include <assert.h>

#include <glob.h>

#include "dep.h"
#include "filedef.h"
#include "job.h"
#include "commands.h"
#include "variable.h"
#include "rule.h"
#include "debug.h"
#include "hash.h"


#ifndef WINDOWS32
#ifndef _AMIGA
#ifndef VMS
#include <pwd.h>
#else
struct passwd *getpwnam PARAMS ((char *name));
#endif
#endif
#endif /* !WINDOWS32 */

/* A 'struct ebuffer' controls the origin of the makefile we are currently
   eval'ing.
*/

struct ebuffer
  {
    char *buffer;       /* Start of the current line in the buffer.  */
    char *bufnext;      /* Start of the next line in the buffer.  */
    char *bufstart;     /* Start of the entire buffer.  */
    unsigned int size;  /* Malloc'd size of buffer. */
    FILE *fp;           /* File, or NULL if this is an internal buffer.  */
    struct floc floc;   /* Info on the file in fp (if any).  */
  };

/* Types of "words" that can be read in a makefile.  */
enum make_word_type
  {
     w_bogus, w_eol, w_static, w_variable, w_colon, w_dcolon, w_semicolon,
     w_varassign
  };


/* A `struct conditionals' contains the information describing
   all the active conditionals in a makefile.

   The global variable `conditionals' contains the conditionals
   information for the current makefile.  It is initialized from
   the static structure `toplevel_conditionals' and is later changed
   to new structures for included makefiles.  */

struct conditionals
  {
    unsigned int if_cmds;	/* Depth of conditional nesting.  */
    unsigned int allocated;	/* Elts allocated in following arrays.  */
    char *ignoring;		/* Are we ignoring or interepreting?  */
    char *seen_else;		/* Have we already seen an `else'?  */
  };

static struct conditionals toplevel_conditionals;
static struct conditionals *conditionals = &toplevel_conditionals;


/* Default directories to search for include files in  */

static char *default_include_directories[] =
  {
#if defined(WINDOWS32) && !defined(INCLUDEDIR)
/*
 * This completely up to the user when they install MSVC or other packages.
 * This is defined as a placeholder.
 */
#define INCLUDEDIR "."
#endif
    INCLUDEDIR,
#ifndef _AMIGA
    "/usr/gnu/include",
    "/usr/local/include",
    "/usr/include",
#endif
    0
  };

/* List of directories to search for include files in  */

static char **include_directories;

/* Maximum length of an element of the above.  */

static unsigned int max_incl_len;

/* The filename and pointer to line number of the
   makefile currently being read in.  */

const struct floc *reading_file = 0;

/* The chain of makefiles read by read_makefile.  */

static struct dep *read_makefiles = 0;

static int eval_makefile PARAMS ((char *filename, int flags));
static int eval PARAMS ((struct ebuffer *buffer, int flags));

static long readline PARAMS ((struct ebuffer *ebuf));
static void do_define PARAMS ((char *name, unsigned int namelen,
                               enum variable_origin origin,
                               struct ebuffer *ebuf));
static int conditional_line PARAMS ((char *line, const struct floc *flocp));
static void record_files PARAMS ((struct nameseq *filenames, char *pattern, char *pattern_percent,
			struct dep *deps, unsigned int cmds_started, char *commands,
			unsigned int commands_idx, int two_colon,
			int have_sysv_atvar,
                        const struct floc *flocp, int set_default));
static void record_target_var PARAMS ((struct nameseq *filenames, char *defn,
                                       int two_colon,
                                       enum variable_origin origin,
                                       const struct floc *flocp));
static enum make_word_type get_next_mword PARAMS ((char *buffer, char *delim,
                        char **startp, unsigned int *length));

/* Read in all the makefiles and return the chain of their names.  */

struct dep *
read_all_makefiles (makefiles)
     char **makefiles;
{
  unsigned int num_makefiles = 0;

  /* Create *_LIST variables, to hold the makefiles, targets, and variables
     we will be reading. */

  define_variable ("MAKEFILE_LIST", sizeof ("MAKEFILE_LIST")-1, "", o_file, 0);

  DB (DB_BASIC, (_("Reading makefiles...\n")));

  /* If there's a non-null variable MAKEFILES, its value is a list of
     files to read first thing.  But don't let it prevent reading the
     default makefiles and don't let the default goal come from there.  */

  {
    char *value;
    char *name, *p;
    unsigned int length;

    {
      /* Turn off --warn-undefined-variables while we expand MAKEFILES.  */
      int save = warn_undefined_variables_flag;
      warn_undefined_variables_flag = 0;

      value = allocated_variable_expand ("$(MAKEFILES)");

      warn_undefined_variables_flag = save;
    }

    /* Set NAME to the start of next token and LENGTH to its length.
       MAKEFILES is updated for finding remaining tokens.  */
    p = value;

    while ((name = find_next_token (&p, &length)) != 0)
      {
	if (*p != '\0')
	  *p++ = '\0';
        name = xstrdup (name);
	if (eval_makefile (name,
                           RM_NO_DEFAULT_GOAL|RM_INCLUDED|RM_DONTCARE) < 2)
          free (name);
      }

    free (value);
  }

  /* Read makefiles specified with -f switches.  */

  if (makefiles != 0)
    while (*makefiles != 0)
      {
	struct dep *tail = read_makefiles;
	register struct dep *d;

	if (! eval_makefile (*makefiles, 0))
	  perror_with_name ("", *makefiles);

	/* Find the right element of read_makefiles.  */
	d = read_makefiles;
	while (d->next != tail)
	  d = d->next;

	/* Use the storage read_makefile allocates.  */
	*makefiles = dep_name (d);
	++num_makefiles;
	++makefiles;
      }

  /* If there were no -f switches, try the default names.  */

  if (num_makefiles == 0)
    {
      static char *default_makefiles[] =
#ifdef VMS
	/* all lower case since readdir() (the vms version) 'lowercasifies' */
	{ "makefile.vms", "gnumakefile.", "makefile.", 0 };
#else
#ifdef _AMIGA
	{ "GNUmakefile", "Makefile", "SMakefile", 0 };
#else /* !Amiga && !VMS */
	{ "GNUmakefile", "makefile", "Makefile", 0 };
#endif /* AMIGA */
#endif /* VMS */
      register char **p = default_makefiles;
      while (*p != 0 && !file_exists_p (*p))
	++p;

      if (*p != 0)
	{
	  if (! eval_makefile (*p, 0))
	    perror_with_name ("", *p);
	}
      else
	{
	  /* No default makefile was found.  Add the default makefiles to the
	     `read_makefiles' chain so they will be updated if possible.  */
	  struct dep *tail = read_makefiles;
	  /* Add them to the tail, after any MAKEFILES variable makefiles.  */
	  while (tail != 0 && tail->next != 0)
	    tail = tail->next;
	  for (p = default_makefiles; *p != 0; ++p)
	    {
	      struct dep *d = (struct dep *) xmalloc (sizeof (struct dep));
	      d->name = 0;
	      d->file = enter_file (*p);
	      d->file->dontcare = 1;
              d->ignore_mtime = 0;
	      /* Tell update_goal_chain to bail out as soon as this file is
		 made, and main not to die if we can't make this file.  */
	      d->changed = RM_DONTCARE;
	      if (tail == 0)
		read_makefiles = d;
	      else
		tail->next = d;
	      tail = d;
	    }
	  if (tail != 0)
	    tail->next = 0;
	}
    }

  return read_makefiles;
}

static int
eval_makefile (filename, flags)
     char *filename;
     int flags;
{
  struct dep *deps;
  struct ebuffer ebuf;
  const struct floc *curfile;
  int makefile_errno;
  int r;

  ebuf.floc.filenm = filename;
  ebuf.floc.lineno = 1;

  if (ISDB (DB_VERBOSE))
    {
      printf (_("Reading makefile `%s'"), filename);
      if (flags & RM_NO_DEFAULT_GOAL)
	printf (_(" (no default goal)"));
      if (flags & RM_INCLUDED)
	printf (_(" (search path)"));
      if (flags & RM_DONTCARE)
	printf (_(" (don't care)"));
      if (flags & RM_NO_TILDE)
	printf (_(" (no ~ expansion)"));
      puts ("...");
    }

  /* First, get a stream to read.  */

  /* Expand ~ in FILENAME unless it came from `include',
     in which case it was already done.  */
  if (!(flags & RM_NO_TILDE) && filename[0] == '~')
    {
      char *expanded = tilde_expand (filename);
      if (expanded != 0)
	filename = expanded;
    }

  ebuf.fp = fopen (filename, "r");
  /* Save the error code so we print the right message later.  */
  makefile_errno = errno;

  /* If the makefile wasn't found and it's either a makefile from
     the `MAKEFILES' variable or an included makefile,
     search the included makefile search path for this makefile.  */
  if (ebuf.fp == 0 && (flags & RM_INCLUDED) && *filename != '/')
    {
      register unsigned int i;
      for (i = 0; include_directories[i] != 0; ++i)
	{
	  char *name = concat (include_directories[i], "/", filename);
	  ebuf.fp = fopen (name, "r");
	  if (ebuf.fp == 0)
	    free (name);
	  else
	    {
	      filename = name;
	      break;
	    }
	}
    }

  /* Add FILENAME to the chain of read makefiles.  */
  deps = (struct dep *) xmalloc (sizeof (struct dep));
  deps->next = read_makefiles;
  read_makefiles = deps;
  deps->name = 0;
  deps->file = lookup_file (filename);
  if (deps->file == 0)
    {
      deps->file = enter_file (xstrdup (filename));
      if (flags & RM_DONTCARE)
	deps->file->dontcare = 1;
    }
  if (filename != ebuf.floc.filenm)
    free (filename);
  filename = deps->file->name;
  deps->changed = flags;
  deps->ignore_mtime = 0;

  /* If the makefile can't be found at all, give up entirely.  */

  if (ebuf.fp == 0)
    {
      /* If we did some searching, errno has the error from the last
	 attempt, rather from FILENAME itself.  Restore it in case the
	 caller wants to use it in a message.  */
      errno = makefile_errno;
      return 0;
    }

  /* Add this makefile to the list. */
  do_variable_definition (&ebuf.floc, "MAKEFILE_LIST", filename, o_file,
                          f_append, 0);

  /* Evaluate the makefile */

  ebuf.size = 200;
  ebuf.buffer = ebuf.bufnext = ebuf.bufstart = xmalloc (ebuf.size);

  curfile = reading_file;
  reading_file = &ebuf.floc;

  r = eval (&ebuf, !(flags & RM_NO_DEFAULT_GOAL));

  reading_file = curfile;

  fclose (ebuf.fp);

  free (ebuf.bufstart);
  return r;
}

int
eval_buffer (buffer)
     char *buffer;
{
  struct ebuffer ebuf;
  const struct floc *curfile;
  int r;

  /* Evaluate the buffer */

  ebuf.size = strlen (buffer);
  ebuf.buffer = ebuf.bufnext = ebuf.bufstart = buffer;
  ebuf.fp = NULL;

  ebuf.floc = *reading_file;

  curfile = reading_file;
  reading_file = &ebuf.floc;

  r = eval (&ebuf, 1);

  reading_file = curfile;

  return r;
}


/* Read file FILENAME as a makefile and add its contents to the data base.

   SET_DEFAULT is true if we are allowed to set the default goal.

   FILENAME is added to the `read_makefiles' chain.

   Returns 0 if a file was not found or not read.
   Returns 1 if FILENAME was found and read.
   Returns 2 if FILENAME was read, and we kept a reference (don't free it).  */

static int
eval (ebuf, set_default)
     struct ebuffer *ebuf;
     int set_default;
{
  static char *collapsed = 0;
  static unsigned int collapsed_length = 0;
  unsigned int commands_len = 200;
  char *commands;
  unsigned int commands_idx = 0;
  unsigned int cmds_started, tgts_started;
  int ignoring = 0, in_ignored_define = 0;
  int no_targets = 0;		/* Set when reading a rule without targets.  */
  int have_sysv_atvar = 0;
  struct nameseq *filenames = 0;
  struct dep *deps = 0;
  long nlines = 0;
  int two_colon = 0;
  char *pattern = 0, *pattern_percent;
  struct floc *fstart;
  struct floc fi;

#define record_waiting_files()						      \
  do									      \
    { 									      \
      if (filenames != 0)						      \
        {                                                                     \
	  fi.lineno = tgts_started;                                           \
	  record_files (filenames, pattern, pattern_percent, deps,            \
                        cmds_started, commands, commands_idx, two_colon,      \
                        have_sysv_atvar, &fi, set_default);                   \
        }                                                                     \
      filenames = 0;							      \
      commands_idx = 0;							      \
      no_targets = 0;                                                         \
      if (pattern) { free(pattern); pattern = 0; }                            \
    } while (0)

  pattern_percent = 0;
  cmds_started = tgts_started = 1;

  fstart = &ebuf->floc;
  fi.filenm = ebuf->floc.filenm;

  /* Loop over lines in the file.
     The strategy is to accumulate target names in FILENAMES, dependencies
     in DEPS and commands in COMMANDS.  These are used to define a rule
     when the start of the next rule (or eof) is encountered.

     When you see a "continue" in the loop below, that means we are moving on
     to the next line _without_ ending any rule that we happen to be working
     with at the moment.  If you see a "goto rule_complete", then the
     statement we just parsed also finishes the previous rule.  */

  commands = xmalloc (200);

  while (1)
    {
      int linelen;
      char *line;
      int len;
      char *p;
      char *p2;

      /* Grab the next line to be evaluated */
      ebuf->floc.lineno += nlines;
      nlines = readline (ebuf);

      /* If there is nothing left to eval, we're done.  */
      if (nlines < 0)
        break;

      /* If this line is empty, skip it.  */
      line = ebuf->buffer;
      if (line[0] == '\0')
        continue;

      linelen = strlen (line);

      /* Check for a shell command line first.
	 If it is not one, we can stop treating tab specially.  */
      if (line[0] == '\t')
	{
	  if (no_targets)
	    /* Ignore the commands in a rule with no targets.  */
	    continue;

	  /* If there is no preceding rule line, don't treat this line
	     as a command, even though it begins with a tab character.
	     SunOS 4 make appears to behave this way.  */

	  if (filenames != 0)
	    {
	      if (ignoring)
		/* Yep, this is a shell command, and we don't care.  */
		continue;

	      /* Append this command line to the line being accumulated.  */
	      if (commands_idx == 0)
		cmds_started = ebuf->floc.lineno;

	      if (linelen + 1 + commands_idx > commands_len)
		{
		  commands_len = (linelen + 1 + commands_idx) * 2;
		  commands = xrealloc (commands, commands_len);
		}
	      bcopy (line, &commands[commands_idx], linelen);
	      commands_idx += linelen;
	      commands[commands_idx++] = '\n';

	      continue;
	    }
	}

      /* This line is not a shell command line.  Don't worry about tabs.  */

      if (collapsed_length < linelen+1)
	{
	  collapsed_length = linelen+1;
	  if (collapsed != 0)
	    free (collapsed);
	  collapsed = (char *) xmalloc (collapsed_length);
	}
      strcpy (collapsed, line);
      /* Collapse continuation lines.  */
      collapse_continuations (collapsed);
      remove_comments (collapsed);

      /* Compare a word, both length and contents. */
#define	word1eq(s) 	(len == sizeof(s)-1 && strneq (s, p, sizeof(s)-1))
      p = collapsed;
      while (isspace ((unsigned char)*p))
	++p;

      if (*p == '\0')
	/* This line is completely empty--ignore it.  */
	continue;

      /* Find the end of the first token.  Note we don't need to worry about
       * ":" here since we compare tokens by length (so "export" will never
       * be equal to "export:").
       */
      for (p2 = p+1; *p2 != '\0' && !isspace ((unsigned char)*p2); ++p2)
        ;
      len = p2 - p;

      /* Find the start of the second token.  If it looks like a target or
         variable definition it can't be a preprocessor token so skip
         them--this allows variables/targets named `ifdef', `export', etc. */
      while (isspace ((unsigned char)*p2))
        ++p2;

      if ((p2[0] == ':' || p2[0] == '+' || p2[0] == '=') && p2[1] == '\0')
        {
          /* It can't be a preprocessor token so skip it if we're ignoring */
          if (ignoring)
            continue;

          goto skip_conditionals;
        }

      /* We must first check for conditional and `define' directives before
	 ignoring anything, since they control what we will do with
	 following lines.  */

      if (!in_ignored_define
	  && (word1eq ("ifdef") || word1eq ("ifndef")
	      || word1eq ("ifeq") || word1eq ("ifneq")
	      || word1eq ("else") || word1eq ("endif")))
	{
 	  int i = conditional_line (p, fstart);
	  if (i < 0)
	    fatal (fstart, _("invalid syntax in conditional"));

          ignoring = i;
	  continue;
	}

      if (word1eq ("endef"))
	{
	  if (!in_ignored_define)
	    fatal (fstart, _("extraneous `endef'"));
          in_ignored_define = 0;
	  continue;
	}

      if (word1eq ("define"))
	{
	  if (ignoring)
	    in_ignored_define = 1;
	  else
	    {
              if (*p2 == '\0')
                fatal (fstart, _("empty variable name"));

	      /* Let the variable name be the whole rest of the line,
		 with trailing blanks stripped (comments have already been
		 removed), so it could be a complex variable/function
		 reference that might contain blanks.  */
	      p = strchr (p2, '\0');
	      while (isblank ((unsigned char)p[-1]))
		--p;
	      do_define (p2, p - p2, o_file, ebuf);
	    }
	  continue;
	}

      if (word1eq ("override"))
        {
	  if (*p2 == '\0')
	    error (fstart, _("empty `override' directive"));

	  if (strneq (p2, "define", 6)
	      && (isblank ((unsigned char)p2[6]) || p2[6] == '\0'))
	    {
	      if (ignoring)
		in_ignored_define = 1;
	      else
		{
		  p2 = next_token (p2 + 6);
                  if (*p2 == '\0')
                    fatal (fstart, _("empty variable name"));

		  /* Let the variable name be the whole rest of the line,
		     with trailing blanks stripped (comments have already been
		     removed), so it could be a complex variable/function
		     reference that might contain blanks.  */
		  p = strchr (p2, '\0');
		  while (isblank ((unsigned char)p[-1]))
		    --p;
		  do_define (p2, p - p2, o_override, ebuf);
		}
	    }
	  else if (!ignoring
		   && !try_variable_definition (fstart, p2, o_override, 0))
	    error (fstart, _("invalid `override' directive"));

	  continue;
	}

      if (ignoring)
	/* Ignore the line.  We continue here so conditionals
	   can appear in the middle of a rule.  */
	continue;

      if (word1eq ("export"))
	{
          /* 'export' by itself causes everything to be exported. */
	  if (*p2 == '\0')
            export_all_variables = 1;
          else
            {
              struct variable *v;

              v = try_variable_definition (fstart, p2, o_file, 0);
              if (v != 0)
                v->export = v_export;
              else
                {
                  unsigned int len;
                  char *ap;

                  /* Expand the line so we can use indirect and constructed
                     variable names in an export command.  */
                  p2 = ap = allocated_variable_expand (p2);

                  for (p = find_next_token (&p2, &len); p != 0;
                       p = find_next_token (&p2, &len))
                    {
                      v = lookup_variable (p, len);
                      if (v == 0)
                        v = define_variable_loc (p, len, "", o_file, 0,
                                                 fstart);
                      v->export = v_export;
                    }

                  free (ap);
                }
            }
          goto rule_complete;
	}

      if (word1eq ("unexport"))
	{
	  if (*p2 == '\0')
	    export_all_variables = 0;
          else
            {
              unsigned int len;
              struct variable *v;
              char *ap;

              /* Expand the line so we can use indirect and constructed
                 variable names in an unexport command.  */
              p2 = ap = allocated_variable_expand (p2);

              for (p = find_next_token (&p2, &len); p != 0;
                   p = find_next_token (&p2, &len))
                {
                  v = lookup_variable (p, len);
                  if (v == 0)
                    v = define_variable_loc (p, len, "", o_file, 0, fstart);

                  v->export = v_noexport;
                }

              free (ap);
            }
          goto rule_complete;
	}

 skip_conditionals:
      if (word1eq ("vpath"))
	{
	  char *pattern;
	  unsigned int len;
	  p2 = variable_expand (p2);
	  p = find_next_token (&p2, &len);
	  if (p != 0)
	    {
	      pattern = savestring (p, len);
	      p = find_next_token (&p2, &len);
	      /* No searchpath means remove all previous
		 selective VPATH's with the same pattern.  */
	    }
	  else
	    /* No pattern means remove all previous selective VPATH's.  */
	    pattern = 0;
	  construct_vpath_list (pattern, p);
	  if (pattern != 0)
	    free (pattern);

          goto rule_complete;
	}

      if (word1eq ("include") || word1eq ("-include") || word1eq ("sinclude"))
	{
	  /* We have found an `include' line specifying a nested
	     makefile to be read at this point.  */
	  struct conditionals *save;
          struct conditionals new_conditionals;
	  struct nameseq *files;
	  /* "-include" (vs "include") says no error if the file does not
	     exist.  "sinclude" is an alias for this from SGI.  */
	  int noerror = (p[0] != 'i');

	  p = allocated_variable_expand (p2);
	  if (*p == '\0')
	    {
	      error (fstart,
                     _("no file name for `%sinclude'"), noerror ? "-" : "");
	      continue;
	    }

	  /* Parse the list of file names.  */
	  p2 = p;
	  files = multi_glob (parse_file_seq (&p2, '\0',
					      sizeof (struct nameseq),
					      1),
			      sizeof (struct nameseq));
	  free (p);

	  /* Save the state of conditionals and start
	     the included makefile with a clean slate.  */
	  save = conditionals;
	  bzero ((char *) &new_conditionals, sizeof new_conditionals);
	  conditionals = &new_conditionals;

	  /* Record the rules that are waiting so they will determine
	     the default goal before those in the included makefile.  */
	  record_waiting_files ();

	  /* Read each included makefile.  */
	  while (files != 0)
	    {
	      struct nameseq *next = files->next;
	      char *name = files->name;
              int r;

	      free ((char *)files);
	      files = next;

              r = eval_makefile (name, (RM_INCLUDED | RM_NO_TILDE
                                        | (noerror ? RM_DONTCARE : 0)));
	      if (!r)
                {
                  if (!noerror)
                    error (fstart, "%s: %s", name, strerror (errno));
                  free (name);
                }
	    }

	  /* Free any space allocated by conditional_line.  */
	  if (conditionals->ignoring)
	    free (conditionals->ignoring);
	  if (conditionals->seen_else)
	    free (conditionals->seen_else);

	  /* Restore state.  */
	  conditionals = save;

          goto rule_complete;
	}

      if (try_variable_definition (fstart, p, o_file, 0))
	/* This line has been dealt with.  */
	goto rule_complete;

      if (line[0] == '\t')
	{
	  p = collapsed;	/* Ignore comments, etc.  */
	  while (isblank ((unsigned char)*p))
	    ++p;
	  if (*p == '\0')
	    /* The line is completely blank; that is harmless.  */
	    continue;

	  /* This line starts with a tab but was not caught above
	     because there was no preceding target, and the line
	     might have been usable as a variable definition.
	     But now we know it is definitely lossage.  */
	  fatal(fstart, _("commands commence before first target"));
	}

      /* This line describes some target files.  This is complicated by
         the existence of target-specific variables, because we can't
         expand the entire line until we know if we have one or not.  So
         we expand the line word by word until we find the first `:',
         then check to see if it's a target-specific variable.

         In this algorithm, `lb_next' will point to the beginning of the
         unexpanded parts of the input buffer, while `p2' points to the
         parts of the expanded buffer we haven't searched yet. */

      {
        enum make_word_type wtype;
        enum variable_origin v_origin;
        char *cmdleft, *semip, *lb_next;
        unsigned int len, plen = 0;
        char *colonp;

        /* Record the previous rule.  */

        record_waiting_files ();
        tgts_started = fstart->lineno;

        /* Search the line for an unquoted ; that is not after an
           unquoted #.  */
        cmdleft = find_char_unquote (line, ';', '#', 0);
        if (cmdleft != 0 && *cmdleft == '#')
          {
            /* We found a comment before a semicolon.  */
            *cmdleft = '\0';
            cmdleft = 0;
          }
        else if (cmdleft != 0)
          /* Found one.  Cut the line short there before expanding it.  */
          *(cmdleft++) = '\0';
        semip = cmdleft;

        collapse_continuations (line);

        /* We can't expand the entire line, since if it's a per-target
           variable we don't want to expand it.  So, walk from the
           beginning, expanding as we go, and looking for "interesting"
           chars.  The first word is always expandable.  */
        wtype = get_next_mword(line, NULL, &lb_next, &len);
        switch (wtype)
          {
          case w_eol:
            if (cmdleft != 0)
              fatal(fstart, _("missing rule before commands"));
            /* This line contained something but turned out to be nothing
               but whitespace (a comment?).  */
            continue;

          case w_colon:
          case w_dcolon:
            /* We accept and ignore rules without targets for
               compatibility with SunOS 4 make.  */
            no_targets = 1;
            continue;

          default:
            break;
          }

        p2 = variable_expand_string(NULL, lb_next, len);
        while (1)
          {
            lb_next += len;
            if (cmdleft == 0)
              {
                /* Look for a semicolon in the expanded line.  */
                cmdleft = find_char_unquote (p2, ';', 0, 0);

                if (cmdleft != 0)
                  {
                    unsigned long p2_off = p2 - variable_buffer;
                    unsigned long cmd_off = cmdleft - variable_buffer;
                    char *pend = p2 + strlen(p2);

                    /* Append any remnants of lb, then cut the line short
                       at the semicolon.  */
                    *cmdleft = '\0';

                    /* One school of thought says that you shouldn't expand
                       here, but merely copy, since now you're beyond a ";"
                       and into a command script.  However, the old parser
                       expanded the whole line, so we continue that for
                       backwards-compatiblity.  Also, it wouldn't be
                       entirely consistent, since we do an unconditional
                       expand below once we know we don't have a
                       target-specific variable. */
                    (void)variable_expand_string(pend, lb_next, (long)-1);
                    lb_next += strlen(lb_next);
                    p2 = variable_buffer + p2_off;
                    cmdleft = variable_buffer + cmd_off + 1;
                  }
              }

            colonp = find_char_unquote(p2, ':', 0, 0);
#ifdef HAVE_DOS_PATHS
            /* The drive spec brain-damage strikes again...  */
            /* Note that the only separators of targets in this context
               are whitespace and a left paren.  If others are possible,
               they should be added to the string in the call to index.  */
            while (colonp && (colonp[1] == '/' || colonp[1] == '\\') &&
                   colonp > p2 && isalpha ((unsigned char)colonp[-1]) &&
                   (colonp == p2 + 1 || strchr (" \t(", colonp[-2]) != 0))
              colonp = find_char_unquote(colonp + 1, ':', 0, 0);
#endif
            if (colonp != 0)
              break;

            wtype = get_next_mword(lb_next, NULL, &lb_next, &len);
            if (wtype == w_eol)
              break;

            p2 += strlen(p2);
            *(p2++) = ' ';
            p2 = variable_expand_string(p2, lb_next, len);
            /* We don't need to worry about cmdleft here, because if it was
               found in the variable_buffer the entire buffer has already
               been expanded... we'll never get here.  */
          }

        p2 = next_token (variable_buffer);

        /* If the word we're looking at is EOL, see if there's _anything_
           on the line.  If not, a variable expanded to nothing, so ignore
           it.  If so, we can't parse this line so punt.  */
        if (wtype == w_eol)
          {
            if (*p2 != '\0')
              /* There's no need to be ivory-tower about this: check for
                 one of the most common bugs found in makefiles...  */
              fatal (fstart, _("missing separator%s"),
                     !strneq(line, "        ", 8) ? ""
                     : _(" (did you mean TAB instead of 8 spaces?)"));
            continue;
          }

        /* Make the colon the end-of-string so we know where to stop
           looking for targets.  */
        *colonp = '\0';
        filenames = multi_glob (parse_file_seq (&p2, '\0',
                                                sizeof (struct nameseq),
                                                1),
                                sizeof (struct nameseq));
        *p2 = ':';

        if (!filenames)
          {
            /* We accept and ignore rules without targets for
               compatibility with SunOS 4 make.  */
            no_targets = 1;
            continue;
          }
        /* This should never be possible; we handled it above.  */
        assert (*p2 != '\0');
        ++p2;

        /* Is this a one-colon or two-colon entry?  */
        two_colon = *p2 == ':';
        if (two_colon)
          p2++;

        /* Test to see if it's a target-specific variable.  Copy the rest
           of the buffer over, possibly temporarily (we'll expand it later
           if it's not a target-specific variable).  PLEN saves the length
           of the unparsed section of p2, for later.  */
        if (*lb_next != '\0')
          {
            unsigned int l = p2 - variable_buffer;
            plen = strlen (p2);
            (void) variable_buffer_output (p2+plen,
                                           lb_next, strlen (lb_next)+1);
            p2 = variable_buffer + l;
          }

        /* See if it's an "override" keyword; if so see if what comes after
           it looks like a variable definition.  */

        wtype = get_next_mword (p2, NULL, &p, &len);

        v_origin = o_file;
        if (wtype == w_static && word1eq ("override"))
          {
            v_origin = o_override;
            wtype = get_next_mword (p+len, NULL, &p, &len);
          }

        if (wtype != w_eol)
          wtype = get_next_mword (p+len, NULL, NULL, NULL);

        if (wtype == w_varassign)
          {
            /* If there was a semicolon found, add it back, plus anything
               after it.  */
            if (semip)
              {
                *(--semip) = ';';
                variable_buffer_output (p2 + strlen (p2),
                                        semip, strlen (semip)+1);
              }
            record_target_var (filenames, p, two_colon, v_origin, fstart);
            filenames = 0;
            continue;
          }

        /* This is a normal target, _not_ a target-specific variable.
           Unquote any = in the dependency list.  */
        find_char_unquote (lb_next, '=', 0, 0);

        /* We have some targets, so don't ignore the following commands.  */
        no_targets = 0;

        /* Expand the dependencies, etc.  */
        if (*lb_n