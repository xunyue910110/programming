/* Argument parsing and main program of GNU Make.
Copyright (C) 1988, 1989, 1990, 1991, 1994, 1995, 1996, 1997, 1998, 1999,
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
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.  */

#include "make.h"
#include "dep.h"
#include "filedef.h"
#include "variable.h"
#include "job.h"
#include "commands.h"
#include "rule.h"
#include "debug.h"
#include "getopt.h"

#include <assert.h>
#ifdef _AMIGA
# include <dos/dos.h>
# include <proto/dos.h>
#endif
#ifdef WINDOWS32
#include <windows.h>
#include "pathstuff.h"
#endif
#if defined(MAKE_JOBSERVER) && defined(HAVE_FCNTL_H)
# include <fcntl.h>
#endif

#ifdef _AMIGA
int __stack = 20000; /* Make sure we have 20K of stack space */
#endif

extern void init_dir PARAMS ((void));
extern void remote_setup PARAMS ((void));
extern void remote_cleanup PARAMS ((void));
extern RETSIGTYPE fatal_error_signal PARAMS ((int sig));

extern void print_variable_data_base PARAMS ((void));
extern void print_dir_data_base PARAMS ((void));
extern void print_rule_data_base PARAMS ((void));
extern void print_file_data_base PARAMS ((void));
extern void print_vpath_data_base PARAMS ((void));

#if defined HAVE_WAITPID || defined HAVE_WAIT3
# define HAVE_WAIT_NOHANG
#endif

#ifndef	HAVE_UNISTD_H
extern int chdir ();
#endif
#ifndef	STDC_HEADERS
# ifndef sun			/* Sun has an incorrect decl in a header.  */
extern void exit PARAMS ((int)) __attribute__ ((noreturn));
# endif
extern double atof ();
#endif

static void print_data_base PARAMS ((void));
static void print_version PARAMS ((void));
static void decode_switches PARAMS ((int argc, char **argv, int env));
static void decode_env_switches PARAMS ((char *envar, unsigned int len));
static void define_makeflags PARAMS ((int all, int makefile));
static char *quote_for_env PARAMS ((char *out, char *in));
static void initialize_global_hash_tables PARAMS ((void));


/* The structure that describes an accepted command switch.  */

struct command_switch
  {
    int c;			/* The switch character.  */

    enum			/* Type of the value.  */
      {
	flag,			/* Turn int flag on.  */
	flag_off,		/* Turn int flag off.  */
	string,			/* One string per switch.  */
	positive_int,		/* A positive integer.  */
	floating,		/* A floating-point number (double).  */
	ignore			/* Ignored.  */
      } type;

    char *value_ptr;	/* Pointer to the value-holding variable.  */

    unsigned int env:1;		/* Can come from MAKEFLAGS.  */
    unsigned int toenv:1;	/* Should be put in MAKEFLAGS.  */
    unsigned int no_makefile:1;	/* Don't propagate when remaking makefiles.  */

    char *noarg_value;	/* Pointer to value used if no argument is given.  */
    char *default_value;/* Pointer to default value.  */

    char *long_name;		/* Long option name.  */
  };

/* True if C is a switch value that corresponds to a short option.  */

#define short_option(c) ((c) <= CHAR_MAX)

/* The structure used to hold the list of strings given
   in command switches of a type that takes string arguments.  */

struct stringlist
  {
    char **list;	/* Nil-terminated list of strings.  */
    unsigned int idx;	/* Index into above.  */
    unsigned int max;	/* Number of pointers allocated.  */
  };


/* The recognized command switches.  */

/* Nonzero means do not print commands to be executed (-s).  */

int silent_flag;

/* Nonzero means just touch the files
   that would appear to need remaking (-t)  */

int touch_flag;

/* Nonzero means just print what commands would need to be executed,
   don't actually execute them (-n).  */

int just_print_flag;

/* Print debugging info (--debug).  */

static struct stringlist *db_flags;
static int debug_flag = 0;

int db_level = 0;

#ifdef WINDOWS32
/* Suspend make in main for a short time to allow debugger to attach */

int suspend_flag = 0;
#endif

/* Environment variables override makefile definitions.  */

int env_overrides = 0;

/* Nonzero means ignore status codes returned by commands
   executed to remake files.  Just treat them all as successful (-i).  */

int ignore_errors_flag = 0;

/* Nonzero means don't remake anything, just print the data base
   that results from reading the makefile (-p).  */

int print_data_base_flag = 0;

/* Nonzero means don't remake anything; just return a nonzero status
   if the specified targets are not up to date (-q).  */

int question_flag = 0;

/* Nonzero means do not use any of the builtin rules (-r) / variables (-R).  */

int no_builtin_rules_flag = 0;
int no_builtin_variables_flag = 0;

/* Nonzero means keep going even if remaking some file fails (-k).  */

int keep_going_flag;
int default_keep_going_flag = 0;

/* Nonzero means print directory before starting and when done (-w).  */

int print_directory_flag = 0;

/* Nonzero means ignore print_directory_flag and never print the directory.
   This is necessary because print_directory_flag is set implicitly.  */

int inhibit_print_directory_flag = 0;

/* Nonzero means print version information.  */

int print_version_flag = 0;

/* List of makefiles given with -f switches.  */

static struct stringlist *makefiles = 0;

/* Number of job slots (commands that can be run at once).  */

unsigned int job_slots = 1;
unsigned int default_job_slots = 1;

/* Value of job_slots that means no limit.  */

static unsigned int inf_jobs = 0;

/* File descriptors for the jobs pipe.  */

static struct stringlist *jobserver_fds = 0;

int job_fds[2] = { -1, -1 };
int job_rfd = -1;

/* Maximum load average at which multiple jobs will be run.
   Negative values mean unlimited, while zero means limit to
   zero load (which could be useful to start infinite jobs remotely
   but one at a time locally).  */
#ifndef NO_FLOAT
double max_load_average = -1.0;
double default_load_average = -1.0;
#else
int max_load_average = -1;
int default_load_average = -1;
#endif

/* List of directories given with -C switches.  */

static struct stringlist *directories = 0;

/* List of include directories given with -I switches.  */

static struct stringlist *include_directories = 0;

/* List of files given with -o switches.  */

static struct stringlist *old_files = 0;

/* List of files given with -W switches.  */

static struct stringlist *new_files = 0;

/* If nonzero, we should just print usage and exit.  */

static int print_usage_flag = 0;

/* If nonzero, we should print a warning message
   for each reference to an undefined variable.  */

int warn_undefined_variables_flag;

/* If nonzero, always build all targets, regardless of whether
   they appear out of date or not.  */

int always_make_flag = 0;

/* The usage output.  We write it this way to make life easier for the
   translators, especially those trying to translate to right-to-left
   languages like Hebrew.  */

static const char *const usage[] =
  {
    N_("Options:\n"),
    N_("\
  -b, -m                      Ignored for compatibility.\n"),
    N_("\
  -B, --always-make           Unconditionally make all targets.\n"),
    N_("\
  -C DIRECTORY, --directory=DIRECTORY\n\
                              Change to DIRECTORY before doing anything.\n"),
    N_("\
  -d                          Print lots of debugging information.\n"),
    N_("\
  --debug[=FLAGS]             Print various types of debugging information.\n"),
    N_("\
  -e, --environment-overrides\n\
                              Environment variables override makefiles.\n"),
    N_("\
  -f FILE, --file=FILE, --makefile=FILE\n\
                              Read FILE as a makefile.\n"),
    N_("\
  -h, --help                  Print this message and exit.\n"),
    N_("\
  -i, --ignore-errors         Ignore errors from commands.\n"),
    N_("\
  -I DIRECTORY, --include-dir=DIRECTORY\n\
                              Search DIRECTORY for included makefiles.\n"),
    N_("\
  -j [N], --jobs[=N]          Allow N jobs at once; infinite jobs with no arg.\n"),
    N_("\
  -k, --keep-going            Keep going when some targets can't be made.\n"),
    N_("\
  -l [N], --load-average[=N], --max-load[=N]\n\
                              Don't start multiple jobs unless load is below N.\n"),
    N_("\
  -n, --just-print, --dry-run, --recon\n\
                              Don't actually run any commands; just print them.\n"),
    N_("\
  -o FILE, --old-file=FILE, --assume-old=FILE\n\
                              Consider FILE to be very old and don't remake it.\n"),
    N_("\
  -p, --print-data-base       Print make's internal database.\n"),
    N_("\
  -q, --question              Run no commands; exit status says if up to date.\n"),
    N_("\
  -r, --no-builtin-rules      Disable the built-in implicit rules.\n"),
    N_("\
  -R, --no-builtin-variables  Disable the built-in variable settings.\n"),
    N_("\
  -s, --silent, --quiet       Don't echo commands.\n"),
    N_("\
  -S, --no-keep-going, --stop\n\
                              Turns off -k.\n"),
    N_("\
  -t, --touch                 Touch targets instead of remaking them.\n"),
    N_("\
  -v, --version               Print the version number of make and exit.\n"),
    N_("\
  -w, --print-directory       Print the current directory.\n"),
    N_("\
  --no-print-directory        Turn off -w, even if it was turned on implicitly.\n"),
    N_("\
  -W FILE, --what-if=FILE, --new-file=FILE, --assume-new=FILE\n\
                              Consider FILE to be infinitely new.\n"),
    N_("\
  --warn-undefined-variables  Warn when an undefined variable is referenced.\n"),
    NULL
  };

/* The table of command switches.  */

static const struct command_switch switches[] =
  {
    { 'b', ignore, 0, 0, 0, 0, 0, 0, 0 },
    { 'B', flag, (char *) &always_make_flag, 1, 1, 0, 0, 0, "always-make" },
    { 'C', string, (char *) &directories, 0, 0, 0, 0, 0, "directory" },
    { 'd', flag, (char *) &debug_flag, 1, 1, 0, 0, 0, 0 },
    { CHAR_MAX+1, string, (char *) &db_flags, 1, 1, 0, "basic", 0, "debug" },
#ifdef WINDOWS32
    { 'D', flag, (char *) &suspend_flag, 1, 1, 0, 0, 0, "suspend-for-debug" },
#endif
    { 'e', flag, (char *) &env_overrides, 1, 1, 0, 0, 0,
        "environment-overrides", },
    { 'f', string, (char *) &makefiles, 0, 0, 0, 0, 0, "file" },
    { 'h', flag, (char *) &print_usage_flag, 0, 0, 0, 0, 0, "help" },
    { 'i', flag, (char *) &ignore_errors_flag, 1, 1, 0, 0, 0,
        "ignore-errors" },
    { 'I', string, (char *) &include_directories, 1, 1, 0, 0, 0,
        "include-dir" },
    { 'j', positive_int, (char *) &job_slots, 1, 1, 0, (char *) &inf_jobs,
        (char *) &default_job_slots, "jobs" },
    { CHAR_MAX+2, string, (char *) &jobserver_fds, 1, 1, 0, 0, 0,
        "jobserver-fds" },
    { 'k', flag, (char *) &keep_going_flag, 1, 1, 0, 0,
        (char *) &default_keep_going_flag, "keep-going" },
#ifndef NO_FLOAT
    { 'l', floating, (char *) &max_load_average, 1, 1, 0,
	(char *) &default_load_average, (char *) &default_load_average,
	"load-average" },
#else
    { 'l', positive_int, (char *) &max_load_average, 1, 1, 0,
	(char *) &default_load_average, (char *) &default_load_average,
	"load-average" },
#endif
    { 'm', ignore, 0, 0, 0, 0, 0, 0, 0 },
    { 'n', flag, (char *) &just_print_flag, 1, 1, 1, 0, 0, "just-print" },
    { 'o', string, (char *) &old_files, 0, 0, 0, 0, 0, "old-file" },
    { 'p', flag, (char *) &print_data_base_flag, 1, 1, 0, 0, 0,
        "print-data-base" },
    { 'q', flag, (char *) &question_flag, 1, 1, 1, 0, 0, "question" },
    { 'r', flag, (char *) &no_builtin_rules_flag, 1, 1, 0, 0, 0,
      "no-builtin-rules" },
    { 'R', flag, (char *) &no_builtin_variables_flag, 1, 1, 0, 0, 0,
	"no-builtin-variables" },
    { 's', flag, (char *) &silent_flag, 1, 1, 0, 0, 0, "silent" },
    { 'S', flag_off, (char *) &keep_going_flag, 1, 1, 0, 0,
      (char *) &default_keep_going_flag, "no-keep-going" },
    { 't', flag, (char *) &touch_flag, 1, 1, 1, 0, 0, "touch" },
    { 'v', flag, (char *) &print_version_flag, 1, 1, 0, 0, 0, "version" },
    { 'w', flag, (char *) &print_directory_flag, 1, 1, 0, 0, 0,
        "print-directory" },
    { CHAR_MAX+3, flag, (char *) &inhibit_print_directory_flag, 1, 1, 0, 0, 0,
	"no-print-directory" },
    { 'W', string, (char *) &new_files, 0, 0, 0, 0, 0, "what-if" },
    { CHAR_MAX+4, flag, (char *) &warn_undefined_variables_flag, 1, 1, 0, 0, 0,
	"warn-undefined-variables" },
    { '\0', }
  };

/* Secondary long names for options.  */

static struct option long_option_aliases[] =
  {
    { "quiet",		no_argument,		0, 's' },
    { "stop",		no_argument,		0, 'S' },
    { "new-file",	required_argument,	0, 'W' },
    { "assume-new",	required_argument,	0, 'W' },
    { "assume-old",	required_argument,	0, 'o' },
    { "max-load",	optional_argument,	0, 'l' },
    { "dry-run",	no_argument,		0, 'n' },
    { "recon",		no_argument,		0, 'n' },
    { "makefile",	required_argument,	0, 'f' },
  };

/* List of goal targets.  */

static struct dep *goals, *lastgoal;

/* List of variables which were defined on the command line
   (or, equivalently, in MAKEFLAGS).  */

struct command_variable
  {
    struct command_variable *next;
    struct variable *variable;
  };
static struct command_variable *command_variables;

/* The name we were invoked with.  */

char *program;

/* Our current directory before processing any -C options.  */

char *directory_before_chdir;

/* Our current directory after processing all -C options.  */

char *starting_directory;

/* Value of the MAKELEVEL variable at startup (or 0).  */

unsigned int makelevel;

/* First file defined in the makefile whose name does not
   start with `.'.  This is the default to remake if the
   command line does not specify.  */

struct file *default_goal_file;

/* Pointer to structure for the file .DEFAULT
   whose commands are used for any file that has none of its own.
   This is zero if the makefiles do not define .DEFAULT.  */

struct file *default_file;

/* Nonzero if we have seen the magic `.POSIX' target.
   This turns on pedantic compliance with POSIX.2.  */

int posix_pedantic;

/* Nonzero if we have seen the `.NOTPARALLEL' target.
   This turns off parallel builds for this invocation of make.  */

int not_parallel;

/* Nonzero if some rule detected clock skew; we keep track so (a) we only
   print one warning about it during the run, and (b) we can print a final
   warning at the end of the run. */

int clock_skew_detected;

/* Mask of signals that are being caught with fatal_error_signal.  */

#ifdef	POSIX
sigset_t fatal_signal_set;
#else
# ifdef	HAVE_SIGSETMASK
int fatal_signal_mask;
# endif
#endif

#if !defined HAVE_BSD_SIGNAL && !defined bsd_signal
# if !defined HAVE_SIGACTION
#  define bsd_signal signal
# else
typedef RETSIGTYPE (*bsd_signal_ret_t) ();

static bsd_signal_ret_t
bsd_signal (sig, func)
     int sig;
     bsd_signal_ret_t func;
{
  struct sigaction act, oact;
  act.sa_handler = func;
  act.sa_flags = SA_RESTART;
  sigemptyset (&act.sa_mask);
  sigaddset (&act.sa_mask, sig);
  if (sigaction (sig, &act, &oact) != 0)
    return SIG_ERR;
  return oact.sa_handler;
}
# endif
#endif

static void
initialize_global_hash_tables ()
{
  init_hash_global_variable_set ();
  init_hash_files ();
  hash_init_directories ();
  hash_init_function_table ();
}

static struct file *
enter_command_line_file (name)
     char *name;
{
  if (name[0] == '\0')
    fatal (NILF, _("empty string invalid as file name"));

  if (name[0] == '~')
    {
      char *expanded = tilde_expand (name);
      if (expanded != 0)
	name = expanded;	/* Memory leak; I don't care.  */
    }

  /* This is also done in parse_file_seq, so this is redundant
     for names read from makefiles.  It is here for names passed
     on the command line.  */
  while (name[0] == '.' && name[1] == '/' && name[2] != '\0')
    {
      name += 2;
      while (*name == '/')
	/* Skip following slashes: ".//foo" is "foo", not "/foo".  */
	++name;
    }

  if (*name == '\0')
    {
      /* It was all slashes!  Move back to the dot and truncate
	 it after the first slash, so it becomes just "./".  */
      do
	--name;
      while (name[0] != '.');
      name[2] = '\0';
    }

  return enter_file (xstrdup (name));
}

/* Toggle -d on receipt of SIGUSR1.  */

static RETSIGTYPE
debug_signal_handler (sig)
     int sig;
{
  db_level = db_level ? DB_NONE : DB_BASIC;
}

static void
decode_debug_flags ()
{
  char **pp;

  if (debug_flag)
    db_level = DB_ALL;

  if (!db_flags)
    return;

  for (pp=db_flags->list; *pp; ++pp)
    {
      const char *p = *pp;

      while (1)
        {
          switch (tolower (p[0]))
            {
            case 'a':
              db_level |= DB_ALL;
              break;
            case 'b':
              db_level |= DB_BASIC;
              break;
            case 'i':
              db_level |= DB_BASIC | DB_IMPLICIT;
              break;
            case 'j':
              db_level |= DB_JOBS;
              break;
            case 'm':
              db_level |= DB_BASIC | DB_MAKEFILES;
              break;
            case 'v':
              db_level |= DB_BASIC | DB_VERBOSE;
              break;
            default:
              fatal (NILF, _("unknown debug level specification `%s'"), p);
            }

          while (*(++p) != '\0')
            if (*p == ',' || *p == ' ')
              break;

          if (*p == '\0')
            break;

          ++p;
        }
    }
}

#ifdef WINDOWS32
/*
 * HANDLE runtime exceptions by avoiding a requestor on the GUI. Capture
 * exception and print it to stderr instead.
 *
 * If ! DB_VERBOSE, just print a simple message and exit.
 * If DB_VERBOSE, print a more verbose message.
 * If compiled for DEBUG, let exception pass through to GUI so that
 *   debuggers can attach.
 */
LONG WINAPI
handle_runtime_exceptions( struct _EXCEPTION_POINTERS *exinfo )
{
  PEXCEPTION_RECORD exrec = exinfo->ExceptionRecord;
  LPSTR cmdline = GetCommandLine();
  LPSTR prg = strtok(cmdline, " ");
  CHAR errmsg[1024];
#ifdef USE_EVENT_LOG
  HANDLE hEventSource;
  LPTSTR lpszStrings[1];
#endif

  if (! ISDB (DB_VERBOSE))
    {
      sprintf(errmsg,
              _("%s: Interrupt/Exception caught (code = 0x%x, addr = 0x%x)\n"),
              prg, exrec->ExceptionCode, exrec->ExceptionAddress);
      fprintf(stderr, errmsg);
      exit(255);
    }

  sprintf(errmsg,
          _("\nUnhandled exception filter called from program %s\nExceptionCode = %x\nExceptionFlags = %x\nExceptionAddress = %x\n"),
          prg, exrec->ExceptionCode, exrec->ExceptionFlags,
          exrec->ExceptionAddress);

  if (exrec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
      && exrec->NumberParameters >= 2)
    sprintf(&errmsg[strlen(errmsg)],
            (exrec->ExceptionInformation[0]
             ? _("Access violation: write operation at address %x\n")
             : _("Access violation: read operation at address %x\n")),
            exrec->ExceptionInformation[1]);

  /* turn this on if we want to put stuff in the event log too */
#ifdef USE_EVENT_LOG
  hEventSource = RegisterEventSource(NULL, "GNU Make");
  lpszStrings[0] = errmsg;

  if (hEventSource != NULL)
    {
      ReportEvent(hEventSource,         /* handle of event source */
                  EVENTLOG_ERROR_TYPE,  /* event type */
                  0,                    /* event category */
                  0,                    /* event ID */
                  NULL,                 /* current user's SID */
                  1,                    /* strings in lpszStrings */
                  0,                    /* no bytes of raw data */
                  lpszStrings,          /* array of error strings */
                  NULL);                /* no raw data */

      (VOID) DeregisterEventSource(hEventSource);
    }
#endif

  /* Write the error to stderr too */
  fprintf(stderr, errmsg);

#ifdef DEBUG
  return EXCEPTION_CONTINUE_SEARCH;
#else
  exit(255);
  return (255); /* not reached */
#endif
}

/*
 * On WIN32 systems we don't have the luxury of a /bin directory that
 * is mapped globally to every drive mounted to the system. Since make could
 * be invoked from any drive, and we don't want to propogate /bin/sh
 * to every single drive. Allow ourselves a chance to search for
 * a value for default shell here (if the default path does not exist).
 */

int
find_and_set_default_shell(char *token)
{
  int sh_found = 0;
  char* search_token;
  PATH_VAR(sh_path);
  extern char *default_shell;

  if (!token)
    search_token = default_shell;
  else
    search_token = token;

  if (!no_default_sh_exe &&
      (token == NULL || !strcmp(search_token, default_shell))) {
    /* no new information, path already set or known */
    sh_found = 1;
  } else if (file_exists_p(search_token)) {
    /* search token path was found */
    sprintf(sh_path, "%s", search_token);
    default_shell = xstrdup(w32ify(sh_path,0));
    DB (DB_VERBOSE,
        (_("find_and_set_shell setting default_shell = %s\n"), default_shell));
    sh_found = 1;
  } else {
    char *p;
    struct variable *v = lookup_variable ("Path", 4);

    /*
     * Search Path for shell
     */
    if (v && v->value) {
      char *ep;

      p  = v->value;
      ep = strchr(p, PATH_SEPARATOR_CHAR);

      while (ep && *ep) {
        *ep = '\0';

        if (dir_file_exists_p(p, search_token)) {
          sprintf(sh_path, "%s/%s", p, search_token);
          default_shell = xstrdup(w32ify(sh_path,0));
          sh_found = 1;
          *ep = PATH_SEPARATOR_CHAR;

          /* terminate loop */
          p += strlen(p);
        } else {
          *ep = PATH_SEPARATOR_CHAR;
           p = ++ep;
        }

        ep = strchr(p, PATH_SEPARATOR_CHAR);
      }

      /* be sure to check last element of Path */
      if (p && *p && dir_file_exists_p(p, search_token)) {
          sprintf(sh_path, "%s/%s", p, search_token);
          default_shell = xstrdup(w32ify(sh_path,0));
          sh_found = 1;
      }

      if (sh_found)
        DB (DB_VERBOSE,
            (_("find_and_set_shell path search set default_shell = %s\n"),
             default_shell));
    }
  }

  /* naive test */
  if (!unixy_shell && sh_found &&
      (strstr(default_shell, "sh") || strstr(default_shell, "SH"))) {
    unixy_shell = 1;
    batch_mode_shell = 0;
  }

#ifdef BATCH_MODE_ONLY_SHELL
  batch_mode_shell = 1;
#endif

  return (sh_found);
}
#endif  /* WINDOWS32 */

#ifdef  __MSDOS__

static void
msdos_return_to_initial_directory ()
{
  if (directory_before_chdir)
    chdir (directory_before_chdir);
}
#endif

extern char *mktemp ();
extern int mkstemp ();

FILE *
open_tmpfile(name, template)
     char **name;
     const char *template;
{
  int fd;

#if defined HAVE_MKSTEMP || defined HAVE_MKTEMP
# define TEMPLATE_LEN   strlen (template)
#else
# define TEMPLATE_LEN   L_tmpnam
#endif
  *name = xmalloc (TEMPLATE_LEN + 1);
  strcpy (*name, template);

#if defined HAVE_MKSTEMP && defined HAVE_FDOPEN
  /* It's safest to use mkstemp(), if we can.  */
  fd = mkstemp (*name);
  if (fd == -1)
    return 0;
  return fdopen (fd, "w");
#else
# ifdef HAVE_MKTEMP
  (void) mktemp (*name);
# else
  (void) tmpnam (*name);
# endif

# ifdef HAVE_FDOPEN
  /* Can't use mkstemp(), but guard against a race condition.  */
  fd = open (*name, O_CREAT|O_EXCL|O_WRONLY, 0600);
  if (fd == -1)
    return 0;
  return fdopen (fd, "w");
# else
  /* Not secure, but what can we do?  */
  return fopen (*name, "w");
# endif
#endif
}


#ifndef _AMIGA
int
main (argc, argv, envp)
     int argc;
     char **argv;
     char **envp;
#else
int main (int argc, char ** argv)
#endif
{
  static char *stdin_nm = 0;
  register struct file *f;
  register unsigned int i;
  char **p;
  struct dep *read_makefiles;
  PATH_VAR (current_directory);
#ifdef WINDOWS32
  char *unix_path = NULL;
  char *windows32_path = NULL;

  SetUnhandledExceptionFilter(handle_runtime_exceptions);

  /* start off assuming we have no shell */
  unixy_shell = 0;
  no_default_sh_exe = 1;
#endif

  default_goal_file = 0;
  reading_file = 0;

#if defined (__MSDOS__) && !defined (_POSIX_SOURCE)
  /* Request the most powerful version of `system', to
     make up for the dumb default shell.  */
  __system_flags = (__system_redirect
		    | __system_use_shell
		    | __system_allow_multiple_cmds
		    | __system_allow_long_cmds
		    | __system_handle_null_commands
		    | __system_emulate_chdir);

#endif

  /* Set up gettext/internationalization support.  */
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

#ifdef	POSIX
  sigemptyset (&fatal_signal_set);
#define	ADD_SIG(sig)	sigaddset (&fatal_signal_set, sig)
#else
#ifdef	HAVE_SIGSETMASK
  fatal_signal_mask = 0;
#define	ADD_SIG(sig)	fatal_signal_mask |= sigmask (sig)
#else
#define	ADD_SIG(sig)
#endif
#endif

#define	FATAL_SIG(sig)							      \
  if (bsd_signal (sig, fatal_error_signal) == SIG_IGN)			      \
    bsd_signal (sig, SIG_IGN);						      \
  else									      \
    ADD_SIG (sig);

#ifdef SIGHUP
  FATAL_SIG (SIGHUP);
#endif
#ifdef SIGQUIT
  FATAL_SIG (SIGQUIT);
#endif
  FATAL_SIG (SIGINT);
  FATAL_SIG (SIGTERM);

#ifdef __MSDOS__
  /* Windows 9X delivers FP exceptions in child programs to their
     parent!  We don't want Make to die when a child divides by zero,
     so we work around that lossage by catching SIGFPE.  */
  FATAL_SIG (SIGFPE);
#endif

#ifdef	SIGDANGER
  FATAL_SIG (SIGDANGER);
#endif
#ifdef SIGXCPU
  FATAL_SIG (SIGXCPU);
#endif
#ifdef SIGXFSZ
  FATAL_SIG (SIGXFSZ);
#endif

#undef	FATAL_SIG

  /* Do not ignore the child-death signal.  This must be done before
     any children could possibly be created; otherwise, the wait
     functions won't work on systems with the SVR4 ECHILD brain
     damage, if our invoker is ignoring this signal.  */

#ifdef HAVE_WAIT_NOHANG
# if defined SIGCHLD
  (void) bsd_signal (SIGCHLD, SIG_DFL);
# endif
# if defined SIGCLD && SIGCLD != SIGCHLD
  (void) bsd_signal (SIGCLD, SIG_DFL);
# endif
#endif

  /* Make sure stdout is line-buffered.  */

#ifdef HAVE_SETVBUF
# ifdef SETVBUF_REVERSED
  setvbuf (stdout, _IOLBF, xmalloc (BUFSIZ), BUFSIZ);
# else	/* setvbuf not reversed.  */
  /* Some buggy systems lose if we pass 0 instead of allocating ourselves.  */
  setvbuf (stdout, (char *) 0, _IOLBF, BUFSIZ);
# endif	/* setvbuf reversed.  */
#elif HAVE_SETLINEBUF
  setlinebuf (stdout);
#endif	/* setlinebuf missing.  */

  /* Figure out where this program lives.  */

  if (argv[0] == 0)
    argv[0] = "";
  if (argv[0][0] == '\0')
    program = "make";
  else
    {
#ifdef VMS
      program = strrchr (argv[0], ']');
#else
      program = strrchr (argv[0], '/');
#endif
#ifdef __MSDOS__
      if (program == 0)
	program = strrchr (argv[0], '\\');
      else
	{
	  /* Some weird environments might pass us argv[0] with
	     both kinds of slashes; we must find the rightmost.  */
	  char *p = strrchr (argv[0], '\\');
	  if (p && p > program)
	    program = p;
	}
      if (program == 0 && argv[0][1] == ':')
	program = argv[0] + 1;
#endif
      if (program == 0)
	program = argv[0];
      else
	++program;
    }

  /* Set up to access user data (files).  */
  user_access ();

  initialize_global_hash_tables ();

  /* Figure out where we are.  */

#ifdef WINDOWS32
  if (getcwd_fs (current_directory, GET_PATH_MAX) == 0)
#else
  if (getcwd (current_directory, GET_PATH_MAX) == 0)
#endif
    {
#ifdef	HAVE_GETCWD
      perror_with_name ("getcwd: ", "");
#else
      error (NILF, "getwd: %s", current_directory);
#endif
      current_directory[0] = '\0';
      directory_before_chdir = 0;
    }
  else
    directory_before_chdir = xstrdup (current_directory);
#ifdef  __MSDOS__
  /* Make sure we will return to the initial directory, come what may.  */
  atexit (msdos_return_to_initial_directory);
#endif

  /* Initialize the special variables.  */
  define_variable (".VARIABLES", 10, "", o_default, 0)->special = 1;
  /* define_variable (".TARGETS", 8, "", o_default, 0); */

  /* Read in variables from the environment.  It is important that this be
     done before $(MAKE) is figured out so its definitions will not be
     from the environment.  */

#ifndef _AMIGA
  for (i = 0; envp[i] != 0; ++i)
    {
      int do_not_define;
      register char *ep = envp[i];

      /* by default, everything gets defined and exported */
      do_not_define = 0;

      while (*ep != '=')
        ++ep;
#ifdef WINDOWS32
      if (!unix_path && strneq(envp[i], "PATH=", 5))
        unix_path = ep+1;
      else if (!windows32_path && !strnicmp(envp[i], "Path=", 5)) {
        do_not_define = 1; /* it gets defined after loop exits */
        windows32_path = ep+1;
      }
#endif
      /* The result of pointer arithmetic is cast to unsigned int for
	 machines where ptrdiff_t is a different size that doesn't widen
	 the same.  */
      if (!do_not_define)
        define_variable (envp[i], (unsigned int) (ep - envp[i]),
                         ep + 1, o_env, 1)
	/* Force exportation of every variable culled from the environment.
	   We used to rely on target_environment's v_default code to do this.
	   But that does not work for the case where an environment variable
	   is redefined in a makefile with `override'; it should then still
	   be exported, because it was originally in the environment.  */
	->export = v_export;
    }
#ifdef WINDOWS32
    /*
     * Make sure that this particular spelling of 'Path' is available
     */
    if (windows32_path)
      define_variable("Path", 4, windows32_path, o_env, 1)->export = v_export;
    else if (unix_path)
      define_variable("Path", 4, unix_path, o_env, 1)->export = v_export;
    else
      define_variable("Path", 4, "", o_env, 1)->export = v_export;

    /*
     * PATH defaults to Path iff PATH not found and Path is found.
     */
    if (!unix_path && windows32_path)
      define_variable("PATH", 4, windows32_path, o_env, 1)->export = v_export;
#endif
#else /* For Amiga, read the ENV: device, ignoring all dirs */
    {
	BPTR env, file, old;
	char buffer[1024];
	int len;
	__aligned struct FileInfoBlock fib;

	env = Lock ("ENV:", ACCESS_READ);
	if (env)
	{
	    old = CurrentDir (DupLock(env));
	    Examine (env, &fib);

	    while (ExNext (env, &fib))
	    {
		if (fib.fib_DirEntryType < 0) /* File */
		{
		    /* Define an empty variable. It will be filled in
			variable_lookup(). Makes startup quite a bit
			faster. */
			define_variable (fib.fib_FileName,
			    strlen (fib.fib_FileName),
			"", o_env, 1)->export = v_export;
		}
	    }
	    UnLock (env);
	    UnLock(CurrentDir(old));
	}
    }
#endif

  /* Decode the switches.  */

  decode_env_switches ("MAKEFLAGS", 9);
#if 0
  /* People write things like:
     	MFLAGS="CC=gcc -pipe" "CFLAGS=-g"
     and we set the -p, -i and -e switches.  Doesn't seem quite right.  */
  decode_env_switches ("MFLAGS", 6);
#endif
  decode_switches (argc, argv, 0);
#ifdef WINDOWS32
  if (suspend_flag) {
        fprintf(stderr, "%s (pid = %d)\n", argv[0], GetCurrentProcessId());
        fprintf(stderr, _("%s is suspending for 30 seconds..."), argv[0]);
        Sleep(30 * 1000);
        fprintf(stderr, _("done sleep(30). Continuing.\n"));
  }
#endif

  decode_debug_flags ();

  /* Print version information.  */

  if (print_version_flag || print_data_base_flag || db_level)
    print_version ();

  /* `make --version' is supposed to just print the version and exit.  */
  if (print_version_flag)
    die (0);

#ifndef VMS
  /* Set the "MAKE_COMMAND" variable to the name we were invoked with.
     (If it is a relative pathname with a slash, prepend our directory name
     so the result will run the same program regardless of the current dir.
     If it is a name with no slash, we can only hope that PATH did not
     find it in the current directory.)  */
#ifdef WINDOWS32
  /*
   * Convert from backslashes to forward slashes for
   * programs like sh which don't like them. Shouldn't
   * matter if the path is one way or the other for
   * CreateProcess().
   */
  if (strpbrk(argv[0], "/:\\") ||
      strstr(argv[0], "..") ||
      strneq(argv[0], "//", 2))
    argv[0] = xstrdup(w32ify(argv[0],1));
#else /* WINDOWS32 */
#ifdef __MSDOS__
  if (strchr (argv[0], '\\'))
    {
      char *p;

      argv[0] = xstrdup (argv[0]);
      for (p = argv[0]; *p; p++