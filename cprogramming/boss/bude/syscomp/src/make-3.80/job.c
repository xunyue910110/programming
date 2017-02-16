/* Job execution and handling for GNU Make.
Copyright (C) 1988,89,90,91,92,93,94,95,96,97,99 Free Software Foundation, Inc.
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

#include "job.h"
#include "debug.h"
#include "filedef.h"
#include "commands.h"
#include "variable.h"
#include "debug.h"

#include <string.h>

/* Default shell to use.  */
#ifdef WINDOWS32
char *default_shell = "sh.exe";
int no_default_sh_exe = 1;
int batch_mode_shell = 1;
#else  /* WINDOWS32 */
# ifdef _AMIGA
char default_shell[] = "";
extern int MyExecute (char **);
# else /* _AMIGA */
#  ifdef __MSDOS__
/* The default shell is a pointer so we can change it if Makefile
   says so.  It is without an explicit path so we get a chance
   to search the $PATH for it (since MSDOS doesn't have standard
   directories we could trust).  */
char *default_shell = "command.com";
#  else  /* __MSDOS__ */
#   ifdef VMS
#    include <descrip.h>
char default_shell[] = "";
#   else
char default_shell[] = "/bin/sh";
#   endif /* VMS */
#  endif /* __MSDOS__ */
int batch_mode_shell = 0;
# endif /* _AMIGA */
#endif /* WINDOWS32 */

#ifdef __MSDOS__
# include <process.h>
static int execute_by_shell;
static int dos_pid = 123;
int dos_status;
int dos_command_running;
#endif /* __MSDOS__ */

#ifdef _AMIGA
# include <proto/dos.h>
static int amiga_pid = 123;
static int amiga_status;
static char amiga_bname[32];
static int amiga_batch_file;
#endif /* Amiga.  */

#ifdef VMS
# ifndef __GNUC__
#   include <processes.h>
# endif
# include <starlet.h>
# include <lib$routines.h>
#endif

#ifdef WINDOWS32
# include <windows.h>
# include <io.h>
# include <process.h>
# include "sub_proc.h"
# include "w32err.h"
# include "pathstuff.h"
#endif /* WINDOWS32 */

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

#if defined (HAVE_SYS_WAIT_H) || defined (HAVE_UNION_WAIT)
# include <sys/wait.h>
#endif

#ifdef HAVE_WAITPID
# define WAIT_NOHANG(status)	waitpid (-1, (status), WNOHANG)
#else	/* Don't have waitpid.  */
# ifdef HAVE_WAIT3
#  ifndef wait3
extern int wait3 ();
#  endif
#  define WAIT_NOHANG(status)	wait3 ((status), WNOHANG, (struct rusage *) 0)
# endif /* Have wait3.  */
#endif /* Have waitpid.  */

#if !defined (wait) && !defined (POSIX)
extern int wait ();
#endif

#ifndef	HAVE_UNION_WAIT

# define WAIT_T int

# ifndef WTERMSIG
#  define WTERMSIG(x) ((x) & 0x7f)
# endif
# ifndef WCOREDUMP
#  define WCOREDUMP(x) ((x) & 0x80)
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(x) (((x) >> 8) & 0xff)
# endif
# ifndef WIFSIGNALED
#  define WIFSIGNALED(x) (WTERMSIG (x) != 0)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(x) (WTERMSIG (x) == 0)
# endif

#else	/* Have `union wait'.  */

# define WAIT_T union wait
# ifndef WTERMSIG
#  define WTERMSIG(x) ((x).w_termsig)
# endif
# ifndef WCOREDUMP
#  define WCOREDUMP(x) ((x).w_coredump)
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(x) ((x).w_retcode)
# endif
# ifndef WIFSIGNALED
#  define WIFSIGNALED(x) (WTERMSIG(x) != 0)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(x) (WTERMSIG(x) == 0)
# endif

#endif	/* Don't have `union wait'.  */

/* How to set close-on-exec for a file descriptor.  */

#if !defined F_SETFD
# define CLOSE_ON_EXEC(_d)
#else
# ifndef FD_CLOEXEC
#  define FD_CLOEXEC 1
# endif
# define CLOSE_ON_EXEC(_d) (void) fcntl ((_d), F_SETFD, FD_CLOEXEC)
#endif

#ifdef VMS
static int vms_jobsefnmask = 0;
#endif /* !VMS */

#ifndef	HAVE_UNISTD_H
extern int dup2 ();
extern int execve ();
extern void _exit ();
# ifndef VMS
extern int geteuid ();
extern int getegid ();
extern int setgid ();
extern int getgid ();
# endif
#endif

extern char *allocated_variable_expand_for_file PARAMS ((char *line, struct file *file));

extern int getloadavg PARAMS ((double loadavg[], int nelem));
extern int start_remote_job PARAMS ((char **argv, char **envp, int stdin_fd,
		int *is_remote, int *id_ptr, int *used_stdin));
extern int start_remote_job_p PARAMS ((int));
extern int remote_status PARAMS ((int *exit_code_ptr, int *signal_ptr,
		int *coredump_ptr, int block));

RETSIGTYPE child_handler PARAMS ((int));
static void free_child PARAMS ((struct child *));
static void start_job_command PARAMS ((struct child *child));
static int load_too_high PARAMS ((void));
static int job_next_command PARAMS ((struct child *));
static int start_waiting_job PARAMS ((struct child *));
#ifdef VMS
static void vmsWaitForChildren PARAMS ((int *));
#endif

/* Chain of all live (or recently deceased) children.  */

struct child *children = 0;

/* Number of children currently running.  */

unsigned int job_slots_used = 0;

/* Nonzero if the `good' standard input is in use.  */

static int good_stdin_used = 0;

/* Chain of children waiting to run until the load average goes down.  */

static struct child *waiting_jobs = 0;

/* Non-zero if we use a *real* shell (always so on Unix).  */

int unixy_shell = 1;


#ifdef WINDOWS32
/*
 * The macro which references this function is defined in make.h.
 */
int w32_kill(int pid, int sig)
{
  return ((process_kill(pid, sig) == TRUE) ? 0 : -1);
}
#endif /* WINDOWS32 */

/* Write an error message describing the exit status given in
   EXIT_CODE, EXIT_SIG, and COREDUMP, for the target TARGET_NAME.
   Append "(ignored)" if IGNORED is nonzero.  */

static void
child_error (target_name, exit_code, exit_sig, coredump, ignored)
     char *target_name;
     int exit_code, exit_sig, coredump;
     int ignored;
{
  if (ignored && silent_flag)
    return;

#ifdef VMS
  if (!(exit_code & 1))
      error (NILF,
             (ignored ? _("*** [%s] Error 0x%x (ignored)")
              : _("*** [%s] Error 0x%x")),
             target_name, exit_code);
#else
  if (exit_sig == 0)
    error (NILF, ignored ? _("[%s] Error %d (ignored)") :
	   _("*** [%s] Error %d"),
	   target_name, exit_code);
  else
    error (NILF, "*** [%s] %s%s",
	   target_name, strsignal (exit_sig),
	   coredump ? _(" (core dumped)") : "");
#endif /* VMS */
}

#ifdef VMS
/* Wait for nchildren children to terminate */
static void
vmsWaitForChildren(int *status)
{
  while (1)
    {
      if (!vms_jobsefnmask)
	{
	  *status = 0;
	  return;
	}

      *status = sys$wflor (32, vms_jobsefnmask);
    }
  return;
}

/* Set up IO redirection.  */

char *
vms_redirect (desc, fname, ibuf)
    struct dsc$descriptor_s *desc;
    char *fname;
    char *ibuf;
{
  char *fptr;
  extern char *vmsify ();

  ibuf++;
  while (isspace ((unsigned char)*ibuf))
    ibuf++;
  fptr = ibuf;
  while (*ibuf && !isspace ((unsigned char)*ibuf))
    ibuf++;
  *ibuf = 0;
  if (strcmp (fptr, "/dev/null") != 0)
    {
      strcpy (fname, vmsify (fptr, 0));
      if (strchr (fname, '.') == 0)
	strcat (fname, ".");
    }
  desc->dsc$w_length = strlen(fname);
  desc->dsc$a_pointer = fname;
  desc->dsc$b_dtype = DSC$K_DTYPE_T;
  desc->dsc$b_class = DSC$K_CLASS_S;

  if (*fname == 0)
    printf (_("Warning: Empty redirection\n"));
  return ibuf;
}


/*
   found apostrophe at (p-1)

   inc p until after closing apostrophe.  */

static char *
handle_apos (char *p)
{
  int alast;
  int inside;

#define SEPCHARS ",/()= "

  inside = 0;

  while (*p != 0)
    {
      if (*p == '"')
	{
	  if (inside)
	    {
	      while ((alast > 0)
		    && (*p == '"'))
		{
		  p++;
		  alast--;
		}
	      if (alast == 0)
		inside = 0;
	      else
		{
		  fprintf (stderr, _("Syntax error, still inside '\"'\n"));
		  exit (3);
		}
	    }
	  else
	    {
	      p++;
	      if (strchr (SEPCHARS, *p))
		break;
	      inside = 1;
	      alast = 1;
	      while (*p == '"')
		{
		  alast++;
		  p++;
		}
	    }
	}
      else
	p++;
    }

  return p;
}

#endif


/* Handle a dead child.  This handler may or may not ever be installed.

   If we're using the jobserver feature, we need it.  First, installing it
   ensures the read will interrupt on SIGCHLD.  Second, we close the dup'd
   read FD to ensure we don't enter another blocking read without reaping all
   the dead children.  In this case we don't need the dead_children count.

   If we don't have either waitpid or wait3, then make is unreliable, but we
   use the dead_children count to reap children as best we can.  */

static unsigned int dead_children = 0;

RETSIGTYPE
child_handler (sig)
     int sig;
{
  ++dead_children;

  if (job_rfd >= 0)
    {
      close (job_rfd);
      job_rfd = -1;
    }

  DB (DB_JOBS, (_("Got a SIGCHLD; %u unreaped children.\n"), dead_children));
}


extern int shell_function_pid, shell_function_completed;

/* Reap all dead children, storing the returned status and the new command
   state (`cs_finished') in the `file' member of the `struct child' for the
   dead child, and removing the child from the chain.  In addition, if BLOCK
   nonzero, we block in this function until we've reaped at least one
   complete child, waiting for it to die if necessary.  If ERR is nonzero,
   print an error message first.  */

void
reap_children (block, err)
     int block, err;
{
  WAIT_T status;
  /* Initially, assume we have some.  */
  int reap_more = 1;

#ifdef WAIT_NOHANG
# define REAP_MORE reap_more
#else
# define REAP_MORE dead_children
#endif

  /* As long as:

       We have at least one child outstanding OR a shell function in progress,
         AND
       We're blocking for a complete child OR there are more children to reap

     we'll keep reaping children.  */

  while ((children != 0 || shell_function_pid != 0)
         && (block || REAP_MORE))
    {
      int remote = 0;
      register int pid;
      int exit_code, exit_sig, coredump;
      register struct child *lastc, *c;
      int child_failed;
      int any_remote, any_local;

      if (err && block)
	{
	  /* We might block for a while, so let the user know why.  */
	  fflush (stdout);
	  error (NILF, _("*** Waiting for unfinished jobs...."));
	}

      /* We have one less dead child to reap.  As noted in
	 child_handler() above, this count is completely unimportant for
	 all modern, POSIX-y systems that support wait3() or waitpid().
	 The rest of this comment below applies only to early, broken
	 pre-POSIX systems.  We keep the count only because... it's there...

	 The test and decrement are not atomic; if it is compiled into:
	 	register = dead_children - 1;
		dead_children = register;
	 a SIGCHLD could come between the two instructions.
	 child_handler increments dead_children.
	 The second instruction here would lose that increment.  But the
	 only effect of dead_children being wrong is that we might wait
	 longer than necessary to reap a child, and lose some parallelism;
	 and we might print the "Waiting for unfinished jobs" message above
	 when not necessary.  */

      if (dead_children > 0)
	--dead_children;

      any_remote = 0;
      any_local = shell_function_pid != 0;
      for (c = children; c != 0; c = c->next)
	{
	  any_remote |= c->remote;
	  any_local |= ! c->remote;
	  DB (DB_JOBS, (_("Live child 0x%08lx (%s) PID %ld %s\n"),
                        (unsigned long int) c, c->file->name,
                        (long) c->pid, c->remote ? _(" (remote)") : ""));
#ifdef VMS
	  break;
#endif
	}

      /* First, check for remote children.  */
      if (any_remote)
	pid = remote_status (&exit_code, &exit_sig, &coredump, 0);
      else
	pid = 0;

      if (pid > 0)
	/* We got a remote child.  */
	remote = 1;
      else if (pid < 0)
	{
          /* A remote status command failed miserably.  Punt.  */
	remote_status_lose:
	  pfatal_with_name ("remote_status");
	}
      else
	{
	  /* No remote children.  Check for local children.  */
#if !defined(__MSDOS__) && !defined(_AMIGA) && !defined(WINDOWS32)
	  if (any_local)
	    {
#ifdef VMS
	      vmsWaitForChildren (&status);
	      pid = c->pid;
#else
#ifdef WAIT_NOHANG
	      if (!block)
		pid = WAIT_NOHANG (&status);
	      else
#endif
		pid = wait (&status);
#endif /* !VMS */
	    }
	  else
	    pid = 0;

	  if (pid < 0)
	    {
              /* The wait*() failed miserably.  Punt.  */
	      pfatal_with_name ("wait");
	    }
	  else if (pid > 0)
	    {
	      /* We got a child exit; chop the status word up.  */
	      exit_code = WEXITSTATUS (status);
	      exit_sig = WIFSIGNALED (status) ? WTERMSIG (status) : 0;
	      coredump = WCOREDUMP (status);
	    }
	  else
	    {
	      /* No local children are dead.  */
              reap_more = 0;

	      if (!block || !any_remote)
                break;

              /* Now try a blocking wait for a remote child.  */
              pid = remote_status (&exit_code, &exit_sig, &coredump, 1);
              if (pid < 0)
                goto remote_status_lose;
              else if (pid == 0)
                /* No remote children either.  Finally give up.  */
                break;

              /* We got a remote child.  */
              remote = 1;
	    }
#endif /* !__MSDOS__, !Amiga, !WINDOWS32.  */

#ifdef __MSDOS__
	  /* Life is very different on MSDOS.  */
	  pid = dos_pid - 1;
	  status = dos_status;
	  exit_code = WEXITSTATUS (status);
	  if (exit_code == 0xff)
	    exit_code = -1;
	  exit_sig = WIFSIGNALED (status) ? WTERMSIG (status) : 0;
	  coredump = 0;
#endif /* __MSDOS__ */
#ifdef _AMIGA
	  /* Same on Amiga */
	  pid = amiga_pid - 1;
	  status = amiga_status;
	  exit_code = amiga_status;
	  exit_sig = 0;
	  coredump = 0;
#endif /* _AMIGA */
#ifdef WINDOWS32
          {
            HANDLE hPID;
            int err;

            /* wait for anything to finish */
            if (hPID = process_wait_for_any()) {

              /* was an error found on this process? */
              err = process_last_err(hPID);

              /* get exit data */
              exit_code = process_exit_code(hPID);

              if (err)
                fprintf(stderr, "make (e=%d): %s",
                  exit_code, map_windows32_error_to_string(exit_code));

              /* signal */
              exit_sig = process_signal(hPID);

              /* cleanup process */
              process_cleanup(hPID);

              coredump = 0;
            }
            pid = (int) hPID;
          }
#endif /* WINDOWS32 */
	}

      /* Check if this is the child of the `shell' function.  */
      if (!remote && pid == shell_function_pid)
	{
	  /* It is.  Leave an indicator for the `shell' function.  */
	  if (exit_sig == 0 && exit_code == 127)
	    shell_function_completed = -1;
	  else
	    shell_function_completed = 1;
	  break;
	}

      child_failed = exit_sig != 0 || exit_code != 0;

      /* Search for a child matching the deceased one.  */
      lastc = 0;
      for (c = children; c != 0; lastc = c, c = c->next)
	if (c->remote == remote && c->pid == pid)
	  break;

      if (c == 0)
        /* An unknown child died.
           Ignore it; it was inherited from our invoker.  */
        continue;

      DB (DB_JOBS, (child_failed
                    ? _("Reaping losing child 0x%08lx PID %ld %s\n")
                    : _("Reaping winning child 0x%08lx PID %ld %s\n"),
                    (unsigned long int) c, (long) c->pid,
                    c->remote ? _(" (remote)") : ""));

      if (c->sh_batch_file) {
        DB (DB_JOBS, (_("Cleaning up temp batch file %s\n"),
                      c->sh_batch_file));

        /* just try and remove, don't care if this fails */
        remove (c->sh_batch_file);

        /* all done with memory */
        free (c->sh_batch_file);
        c->sh_batch_file = NULL;
      }

      /* If this child had the good stdin, say it is now free.  */
      if (c->good_stdin)
        good_stdin_used = 0;

      if (child_failed && !c->noerror && !ignore_errors_flag)
        {
          /* The commands failed.  Write an error message,
             delete non-precious targets, and abort.  */
          static int delete_on_error = -1;
          child_error (c->file->name, exit_code, exit_sig, coredump, 0);
          c->file->update_status = 2;
          if (delete_on_error == -1)
            {
              struct file *f = lookup_file (".DELETE_ON_ERROR");
              delete_on_error = f != 0 && f->is_target;
            }
          if (exit_sig != 0 || delete_on_error)
            delete_child_targets (c);
        }
      else
        {
          if (child_failed)
            {
              /* The commands failed, but we don't care.  */
              child_error (c->file->name,
                           exit_code, exit_sig, coredump, 1);
              child_failed = 0;
            }

          /* If there are more commands to run, try to start them.  */
          if (job_next_command (c))
            {
              if (handling_fatal_signal)
                {
                  /* Never start new commands while we are dying.
                     Since there are more commands that wanted to be run,
                     the target was not completely remade.  So we treat
                     this as if a command had failed.  */
                  c->file->update_status = 2;
                }
              else
                {
                  /* Check again whether to start remotely.
                     Whether or not we want to changes over time.
                     Also, start_remote_job may need state set up
                     by start_remote_job_p.  */
                  c->remote = start_remote_job_p (0);
                  start_job_command (c);
                  /* Fatal signals are left blocked in case we were
                     about to put that child on the chain.  But it is
                     already there, so it is safe for a fatal signal to
                     arrive now; it will clean up this child's targets.  */
                  unblock_sigs ();
                  if (c->file->command_state == cs_running)
                    /* We successfully started the new command.
                       Loop to reap more children.  */
                    continue;
                }

              if (c->file->update_status != 0)
                /* We failed to start the commands.  */
                delete_child_targets (c);
            }
          else
            /* There are no more commands.  We got through them all
               without an unignored error.  Now the target has been
               successfully updated.  */
            c->file->update_status = 0;
        }

      /* When we get here, all the commands for C->file are finished
         (or aborted) and C->file->update_status contains 0 or 2.  But
         C->file->command_state is still cs_running if all the commands
         ran; notice_finish_file looks for cs_running to tell it that
         it's interesting to check the file's modtime again now.  */

      if (! handling_fatal_signal)
        /* Notice if the target of the commands has been changed.
           This also propagates its values for command_state and
           update_status to its also_make files.  */
        notice_finished_file (c->file);

      DB (DB_JOBS, (_("Removing child 0x%08lx PID %ld%s from chain.\n"),
                    (unsigned long int) c, (long) c->pid,
                    c->remote ? _(" (remote)") : ""));

      /* Block fatal signals while frobnicating the list, so that
         children and job_slots_used are always consistent.  Otherwise
         a fatal signal arriving after the child is off the chain and
         before job_slots_used is decremented would believe a child was
         live and call reap_children again.  */
      block_sigs ();

      /* There is now another slot open.  */
      if (job_slots_used > 0)
        --job_slots_used;

      /* Remove the child from the chain and free it.  */
      if (lastc == 0)
        children = c->next;
      else
        lastc->next = c->next;

      free_child (c);

      unblock_sigs ();

      /* If the job failed, and the -k flag was not given, die,
         unless we are already in the process of dying.  */
      if (!err && child_failed && !keep_going_flag &&
          /* fatal_error_signal will die with the right signal.  */
          !handling_fatal_signal)
        die (2);

      /* Only block for one child.  */
      block = 0;
    }

  return;
}

/* Free the storage allocated for CHILD.  */

static void
free_child (child)
     register struct child *child;
{
  /* If this child is the only one it was our "free" job, so don't put a
     token back for it.  This child has already been removed from the list,
     so if there any left this wasn't the last one.  */

  if (job_fds[1] >= 0 && children)
    {
      char token = '+';

      /* Write a job token back to the pipe.  */

      if (write (job_fds[1], &token, 1) != 1)
	pfatal_with_name (_("write jobserver"));

      DB (DB_JOBS, (_("Released token for child 0x%08lx (%s).\n"),
                    (unsigned long int) child, child->file->name));
    }

  if (handling_fatal_signal) /* Don't bother free'ing if about to die.  */
    return;

  if (child->command_lines != 0)
    {
      register unsigned int i;
      for (i = 0; i < child->file->cmds->ncommand_lines; ++i)
	free (child->command_lines[i]);
      free ((char *) child->command_lines);
    }

  if (child->environment != 0)
    {
      register char **ep = child->environment;
      while (*ep != 0)
	free (*ep++);
      free ((char *) child->environment);
    }

  free ((char *) child);
}

#ifdef POSIX
extern sigset_t fatal_signal_set;
#endif

void
block_sigs ()
{
#ifdef POSIX
  (void) sigprocmask (SIG_BLOCK, &fatal_signal_set, (sigset_t *) 0);
#else
# ifdef HAVE_SIGSETMASK
  (void) sigblock (fatal_signal_mask);
# endif
#endif
}

#ifdef	POSIX
void
unblock_sigs ()
{
  sigset_t empty;
  sigemptyset (&empty);
  sigprocmask (SIG_SETMASK, &empty, (sigset_t *) 0);
}
#endif

#ifdef MAKE_JOBSERVER
/* Set the child handler action flags to FLAGS.  */
static void
set_child_handler_action_flags (flags)
     int flags;
{
  struct sigaction sa;
  bzero ((char *) &sa, sizeof sa);
  sa.sa_handler = child_handler;
  sa.sa_flags = flags;
#if defined SIGCHLD
  sigaction (SIGCHLD, &sa, NULL);
#endif
#if defined SIGCLD && SIGCLD != SIGCHLD
  sigaction (SIGCLD, &sa, NULL);
#endif
}
#endif


/* Start a job to run the commands specified in CHILD.
   CHILD is updated to reflect the commands and ID of the child process.

   NOTE: On return fatal signals are blocked!  The caller is responsible
   for calling `unblock_sigs', once the new child is safely on the chain so
   it can be cleaned up in the event of a fatal signal.  */

static void
start_job_command (child)
     register struct child *child;
{
#ifndef _AMIGA
  static int bad_stdin = -1;
#endif
  register char *p;
  int flags;
#ifdef VMS
  char *argv;
#else
  char **argv;
#endif

  /* If we have a completely empty commandset, stop now.  */
  if (!child->command_ptr)
    goto next_command;

  /* Combine the flags parsed for the line itself with
     the flags specified globally for this target.  */
  flags = (child->file->command_flags
	   | child->file->cmds->lines_flags[child->command_line - 1]);

  p = child->command_ptr;
  child->noerror = flags & COMMANDS_NOERROR;

  while (*p != '\0')
    {
      if (*p == '@')
	flags |= COMMANDS_SILENT;
      else if (*p == '+')
	flags |= COMMANDS_RECURSE;
      else if (*p == '-')
	child->noerror = 1;
      else if (!isblank ((unsigned char)*p))
	break;
      ++p;
    }

  /* Update the file's command flags with any new ones we found.  We only
     keep the COMMANDS_RECURSE setting.  Even this isn't 100% correct; we are
     now marking more commands recursive than should be in the case of
     multiline define/endef scripts where only one line is marked "+".  In
     order to really fix this, we'll have to keep a lines_flags for every
     actual line, after expansion.  */
  child->file->cmds->lines_flags[child->command_line - 1]
    |= flags & COMMANDS_RECURSE;

  /* Figure out an argument list from this command line.  */

  {
    char *end = 0;
#ifdef VMS
    argv = p;
#else
    argv = construct_command_argv (p, &end, child->file, &child->sh_batch_file);
#endif
    if (end == NULL)
      child->command_ptr = NULL;
    else
      {
	*end++ = '\0';
	child->command_ptr = end;
      }
  }

  /* If -q was given, say that updating `failed' if there was any text on the
     command line, or `succeeded' otherwise.  The exit status of 1 tells the
     user that -q is saying `something to do'; the exit status for a random
     error is 2.  */
  if (argv != 0 && question_flag && !(flags & COMMANDS_RECURSE))
    {
#ifndef VMS
      free (argv[0]);
      free ((char *) argv);
#endif
      child->file->update_status = 1;
      notice_finished_file (child->file);
      return;
    }

  if (touch_flag && !(flags & COMMANDS_RECURSE))
    {
      /* Go on to the next command.  It might be the recursive one.
	 We construct ARGV only to find the end of the command line.  */
#ifndef VMS
      if (argv)
        {
          free (argv[0]);
          free ((char *) argv);
        }
#endif
      argv = 0;
    }

  if (argv == 0)
    {
    next_command:
#ifdef __MSDOS__
      execute_by_shell = 0;   /* in case construct_command_argv sets it */
#endif
      /* This line has no commands.  Go to the next.  */
      if (job_next_command (child))
	start_job_command (child);
      else
	{
	  /* No more commands.  Make sure we're "running"; we might not be if
             (e.g.) all commands were skipped due to -n.  */
          set_command_state (child->file, cs_running);
	  child->file->update_status = 0;
	  notice_finished_file (child->file);
	}
      return;
    }

  /* Print out the command.  If silent, we call `message' with null so it
     can log the working directory before the command's own error messages
     appear.  */

  message (0, (just_print_flag || (!(flags & COMMANDS_SILENT) && !silent_flag))
	   ? "%s" : (char *) 0, p);

  /* Tell update_goal_chain that a command has been started on behalf of
     this target.  It is important that this happens here and not in
     reap_children (where we used to do it), because reap_children might be
     reaping children from a different target.  We want this increment to
     guaranteedly indicate that a command was started for the dependency
     chain (i.e., update_file recursion chain) we are processing.  */

  ++commands_started;

  /* Optimize an empty command.  People use this for timestamp rules,
     so avoid forking a useless shell.  Do this after we increment
     commands_started so make still treats this special case as if it
     performed some action (makes a difference as to what messages are
     printed, etc.  */

#if !defined(VMS) && !defined(_AMIGA)
  if (
#ifdef __MSDOS__
      unixy_shell	/* the test is complicated and we already did it */
#else
      (argv[0] && !strcmp (argv[0], "/bin/sh"))
#endif
      && (argv[1]
          && argv[1][0] == '-' && argv[1][1] == 'c' && argv[1][2] == '\0')
      && (argv[2] && argv[2][0] == ':' && argv[2][1] == '\0')
      && argv[3] == NULL)
    {
      free (argv[0]);
      free ((char *) argv);
      goto next_command;
    }
#endif  /* !VMS && !_AMIGA */

  /* If -n was given, recurse to get the next line in the sequence.  */

  if (just_print_flag && !(flags & COMMANDS_RECURSE))
    {
#ifndef VMS
      free (argv[0]);
      free ((char *) argv);
#endif
      goto next_command;
    }

  /* Flush the output streams so they won't have things written twice.  */

  fflush (stdout);
  fflush (stderr);

#ifndef VMS
#if !defined(WINDOWS32) && !defined(_AMIGA) && !defined(__MSDOS__)

  /* Set up a bad standard input that reads from a broken pipe.  */

  if (bad_stdin == -1)
    {
      /* Make a file descriptor that is the read end of a broken pipe.
	 This will be used for some children's standard inputs.  */
      int pd[2];
      if (pipe (pd) == 0)
	{
	  /* Close the write side.  */
	  (void) close (pd[1]);
	  /* Save the read side.  */
	  bad_stdin = pd[0];

	  /* Set the descriptor to close on exec, so it does not litter any
	     child's descriptor table.  When it is dup2'd onto descriptor 0,
	     that descriptor will not close on exec.  */
	  CLOSE_ON_EXEC (bad_stdin);
	}
    }

#endif /* !WINDOWS32 && !_AMIGA && !__MSDOS__ */

  /* Decide whether to give this child the `good' standard input
     (one that points to the terminal or whatever), or the `bad' one
     that points to the read side of a broken pipe.  */

  child->good_stdin = !good_stdin_used;
  if (child->good_stdin)
    good_stdin_used = 1;

#endif /* !VMS */

  child->deleted = 0;

#ifndef _AMIGA
  /* Set up the environment for the child.  */
  if (child->environment == 0)
    child->environment = target_environment (child->file);
#endif

#if !defined(__MSDOS__) && !defined(_AMIGA) && !defined(WINDOWS32)

#ifndef VMS
  /* start_waiting_job has set CHILD->remote if we can start a remote job.  */
  if (child->remote)
    {
      int is_remote, id, used_stdin;
      if (start_remote_job (argv, child->environment,
			    child->good_stdin ? 0 : bad_stdin,
			    &is_remote, &id, &used_stdin))
        /* Don't give up; remote execution may fail for various reasons.  If
           so, simply run the job locally.  */
	goto run_local;
      else
	{
	  if (child->good_stdin && !used_stdin)
	    {
	      child->good_stdin = 0;
	      good_stdin_used = 0;
	    }
	  child->remote = is_remote;
	  child->pid = id;
	}
    }
  else
#endif /* !VMS */
    {
      /* Fork the child process.  */

      char **parent_environ;

    run_local:
      block_sigs ();

      child->remote = 0;

#ifdef VMS

      if (!child_execute_job (argv, child)) {
        /* Fork failed!  */
        perror_with_name ("vfork", "");
        goto error;
      }

#else

      parent_environ = environ;
      child->pid = vfork ();
      environ = parent_environ;	/* Restore value child may have clobbered.  */
      if (child->pid == 0)
	{
	  /* We are the child side.  */
	  unblock_sigs ();

          /* If we aren't running a recursive command and we have a jobserver
             pipe, close it before exec'ing.  */
          if (!(flags & COMMANDS_RECURSE) && job_fds[0] >= 0)
            {
              close (job_fds[0]);
              close (job_fds[1]);
            }
          if (job_rfd >= 0)
            close (job_rfd);

	  child_execute_job (child->good_stdin ? 0 : bad_stdin, 1,
                             argv, child->environment);
	}
      else if (child->pid < 0)
	{
	  /* Fork failed!  */
	  unblock_sigs ();
	  perror_with_name ("vfork", "");
	  goto error;
	}
#endif /* !VMS */
    }

#else	/* __MSDOS__ or Amiga or WINDOWS32 */
#ifdef __MSDOS__
  {
    int proc_return;

    block_sigs ();
    dos_status = 0;

    /* We call `system' to do the job of the SHELL, since stock DOS
       shell is too dumb.  Our `system' knows how to handle long
       command lines even if pipes/redirection is needed; it will only
       call COMMAND.COM when its internal commands are used.  */
    if (execute_by_shell)
      {
	char *cmdline = argv[0];
	/* We don't have a way to pass environment to `system',
	   so we need to save and restore ours, sigh...  */
	char **parent_environ = environ;

	environ = child->environment;

	/* If we have a *real* shell, tell `system' to call
	   it to do everything for us.  */
	if (unixy_shell)
	  {
	    /* A *real* shell on MSDOS may not support long
	       command lines the DJGPP way, so we must use `system'.  */
	    cmdline = argv[2];	/* get past "shell -c" */
	  }

	dos_command_running = 1;
	proc_return = system (cmdline);
	environ = parent_environ;
	execute_by_shell = 0;	/* for the next time */
      }
    else
      {
	dos_command_running = 1;
	proc_return = spawnvpe (P_WAIT, argv[0], argv, child->environment);
      }

    /* Need to unblock signals before turning off
       dos_command_running, so that child's signals
       will be treated as such (see fatal_error_signal).  */
    unblock_sigs ();
    dos_command_running = 0;

    /* If the child got a signal, dos_status has its
       high 8 bits set, so be careful not to alter them.  */
    if (proc_return == -1)
      dos_status |= 0xff;
    else
      dos_status |= (proc_return & 0xff);
    ++dead_children;
    child->pid = dos_pid++;
  }
#endif /* __MSDOS__ */
#ifdef _AMIGA
  amiga_status = MyExecute (argv