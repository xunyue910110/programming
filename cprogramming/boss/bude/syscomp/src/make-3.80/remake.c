/* Basic dependency engine for GNU Make.
Copyright (C) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1999,
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
#include "filedef.h"
#include "job.h"
#include "commands.h"
#include "dep.h"
#include "variable.h"
#include "debug.h"

#include <assert.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#ifdef VMS
#include <starlet.h>
#endif
#ifdef WINDOWS32
#include <io.h>
#endif

extern int try_implicit_rule PARAMS ((struct file *file, unsigned int depth));


/* The test for circular dependencies is based on the 'updating' bit in
   `struct file'.  However, double colon targets have seperate `struct
   file's; make sure we always use the base of the double colon chain. */

#define start_updating(_f)  (((_f)->double_colon ? (_f)->double_colon : (_f))\
                             ->updating = 1)
#define finish_updating(_f) (((_f)->double_colon ? (_f)->double_colon : (_f))\
                             ->updating = 0)
#define is_updating(_f)     (((_f)->double_colon ? (_f)->double_colon : (_f))\
                             ->updating)


/* Incremented when a command is started (under -n, when one would be).  */
unsigned int commands_started = 0;

/* Current value for pruning the scan of the goal chain (toggle 0/1).  */
static unsigned int considered;

static int update_file PARAMS ((struct file *file, unsigned int depth));
static int update_file_1 PARAMS ((struct file *file, unsigned int depth));
static int check_dep PARAMS ((struct file *file, unsigned int depth, FILE_TIMESTAMP this_mtime, int *must_make_ptr));
static int touch_file PARAMS ((struct file *file));
static void remake_file PARAMS ((struct file *file));
static FILE_TIMESTAMP name_mtime PARAMS ((char *name));
static int library_search PARAMS ((char **lib, FILE_TIMESTAMP *mtime_ptr));


/* Remake all the goals in the `struct dep' chain GOALS.  Return -1 if nothing
   was done, 0 if all goals were updated successfully, or 1 if a goal failed.
   If MAKEFILES is nonzero, these goals are makefiles, so -t, -q, and -n should
   be disabled for them unless they were also command-line targets, and we
   should only make one goal at a time and return as soon as one goal whose
   `changed' member is nonzero is successfully made.  */

int
update_goal_chain (goals, makefiles)
     register struct dep *goals;
     int makefiles;
{
  int t = touch_flag, q = question_flag, n = just_print_flag;
  unsigned int j = job_slots;
  int status = -1;

#define	MTIME(file) (makefiles ? file_mtime_no_search (file) \
		     : file_mtime (file))

  /* Duplicate the chain so we can remove things from it.  */

  goals = copy_dep_chain (goals);

  {
    /* Clear the `changed' flag of each goal in the chain.
       We will use the flag below to notice when any commands
       have actually been run for a target.  When no commands
       have been run, we give an "up to date" diagnostic.  */

    struct dep *g;
    for (g = goals; g != 0; g = g->next)
      g->changed = 0;
  }

  /* All files start with the considered bit 0, so the global value is 1.  */
  considered = 1;

  /* Update all the goals until they are all finished.  */

  while (goals != 0)
    {
      register struct dep *g, *lastgoal;

      /* Start jobs that are waiting for the load to go down.  */

      start_waiting_jobs ();

      /* Wait for a child to die.  */

      reap_children (1, 0);

      lastgoal = 0;
      g = goals;
      while (g != 0)
	{
	  /* Iterate over all double-colon entries for this file.  */
	  struct file *file;
	  int stop = 0, any_not_updated = 0;

	  for (file = g->file->double_colon ? g->file->double_colon : g->file;
	       file != NULL;
	       file = file->prev)
	    {
	      unsigned int ocommands_started;
	      int x;
	      check_renamed (file);
	      if (makefiles)
		{
		  if (file->cmd_target)
		    {
		      touch_flag = t;
		      question_flag = q;
		      just_print_flag = n;
		    }
		  else
		    touch_flag = question_flag = just_print_flag = 0;
		}

	      /* Save the old value of `commands_started' so we can compare
		 later.  It will be incremented when any commands are
		 actually run.  */
	      ocommands_started = commands_started;

	      x = update_file (file, makefiles ? 1 : 0);
	      check_renamed (file);

	      /* Set the goal's `changed' flag if any commands were started
		 by calling update_file above.  We check this flag below to
		 decide when to give an "up to date" diagnostic.  */
	      g->changed += commands_started - ocommands_started;

              /* If we updated a file and STATUS was not already 1, set it to
                 1 if updating failed, or to 0 if updating succeeded.  Leave
                 STATUS as it is if no updating was done.  */

	      stop = 0;
	      if ((x != 0 || file->updated) && status < 1)
                {
                  if (file->update_status != 0)
                    {
                      /* Updating failed, or -q triggered.  The STATUS value
                         tells our caller which.  */
                      status = file->update_status;
                      /* If -q just triggered, stop immediately.  It doesn't
                         matter how much more we run, since we already know
                         the answer to return.  */
                      stop = (!keep_going_flag && !question_flag
                              && !makefiles);
                    }
                  else
                    {
                      FILE_TIMESTAMP mtime = MTIME (file);
                      check_renamed (file);

                      if (file->updated && g->changed &&
                           mtime != file->mtime_before_update)
                        {
                          /* Updating was done.  If this is a makefile and
                             just_print_flag or question_flag is set (meaning
                             -n or -q was given and this file was specified
                             as a command-line target), don't change STATUS.
                             If STATUS is changed, we will get re-exec'd, and
                             enter an infinite loop.  */
                          if (!makefiles
                              || (!just_print_flag && !question_flag))
                            status = 0;
                          if (makefiles && file->dontcare)
                            /* This is a default makefile; stop remaking.  */
                            stop = 1;
                        }
                    }
                }

	      /* Keep track if any double-colon entry is not finished.
                 When they are all finished, the goal is finished.  */
	      any_not_updated |= !file->updated;

	      if (stop)
		break;
	    }

	  /* Reset FILE since it is null at the end of the loop.  */
	  file = g->file;

	  if (stop || !any_not_updated)
	    {
	      /* If we have found nothing whatever to do for the goal,
		 print a message saying nothing needs doing.  */

	      if (!makefiles
		  /* If the update_status is zero, we updated successfully
		     or not at all.  G->changed will have been set above if
		     any commands were actually started for this goal.  */
		  && file->update_status == 0 && !g->changed
		  /* Never give a message under -s or -q.  */
		  && !silent_flag && !question_flag)
		message (1, ((file->phony || file->cmds == 0)
			     ? _("Nothing to be done for `%s'.")
			     : _("`%s' is up to date.")),
			 file->name);

	      /* This goal is finished.  Remove it from the chain.  */
	      if (lastgoal == 0)
		goals = g->next;
	      else
		lastgoal->next = g->next;

	      /* Free the storage.  */
	      free ((char *) g);

	      g = lastgoal == 0 ? goals : lastgoal->next;

	      if (stop)
		break;
	    }
	  else
	    {
	      lastgoal = g;
	      g = g->next;
	    }
	}

      /* If we reached the end of the dependency graph toggle the considered
         flag for the next pass.  */
      if (g == 0)
        considered = !considered;
    }

  if (makefiles)
    {
      touch_flag = t;
      question_flag = q;
      just_print_flag = n;
      job_slots = j;
    }
  return status;
}

/* If FILE is not up to date, execute the commands for it.
   Return 0 if successful, 1 if unsuccessful;
   but with some flag settings, just call `exit' if unsuccessful.

   DEPTH is the depth in recursions of this function.
   We increment it during the consideration of our dependencies,
   then decrement it again after finding out whether this file
   is out of date.

   If there are multiple double-colon entries for FILE,
   each is considered in turn.  */

static int
update_file (file, depth)
     struct file *file;
     unsigned int depth;
{
  register int status = 0;
  register struct file *f;

  f = file->double_colon ? file->double_colon : file;

  /* Prune the dependency graph: if we've already been here on _this_
     pass through the dependency graph, we don't have to go any further.
     We won't reap_children until we start the next pass, so no state
     change is possible below here until then.  */
  if (f->considered == considered)
    {
      DBF (DB_VERBOSE, _("Pruning file `%s'.\n"));
      return f->command_state == cs_finished ? f->update_status : 0;
    }

  /* This loop runs until we start commands for a double colon rule, or until
     the chain is exhausted. */
  for (; f != 0; f = f->prev)
    {
      f->considered = considered;

      status |= update_file_1 (f, depth);
      check_renamed (f);

      if (status != 0 && !keep_going_flag)
	break;

      if (f->command_state == cs_running
          || f->command_state == cs_deps_running)
        {
	  /* Don't run the other :: rules for this
	     file until this rule is finished.  */
          status = 0;
          break;
        }
    }

  /* Process the remaining rules in the double colon chain so they're marked
     considered.  Start their prerequisites, too.  */
  for (; f != 0 ; f = f->prev)
    {
      struct dep *d;

      f->considered = considered;

      for (d = f->deps; d != 0; d = d->next)
        status |= update_file (d->file, depth + 1);
    }

  return status;
}

/* Consider a single `struct file' and update it as appropriate.  */

static int
update_file_1 (file, depth)
     struct file *file;
     unsigned int depth;
{
  register FILE_TIMESTAMP this_mtime;
  int noexist, must_make, deps_changed;
  int dep_status = 0;
  register struct dep *d, *lastd;
  int running = 0;

  DBF (DB_VERBOSE, _("Considering target file `%s'.\n"));

  if (file->updated)
    {
      if (file->update_status > 0)
	{
	  DBF (DB_VERBOSE,
               _("Recently tried and failed to update file `%s'.\n"));
	  return file->update_status;
	}

      DBF (DB_VERBOSE, _("File `%s' was considered already.\n"));
      return 0;
    }

  switch (file->command_state)
    {
    case cs_not_started:
    case cs_deps_running:
      break;
    case cs_running:
      DBF (DB_VERBOSE, _("Still updating file `%s'.\n"));
      return 0;
    case cs_finished:
      DBF (DB_VERBOSE, _("Finished updating file `%s'.\n"));
      return file->update_status;
    default:
      abort ();
    }

  ++depth;

  /* Notice recursive update of the same file.  */
  start_updating (file);

  /* Looking at the file's modtime beforehand allows the possibility
     that its name may be changed by a VPATH search, and thus it may
     not need an implicit rule.  If this were not done, the file
     might get implicit commands that apply to its initial name, only
     to have that name replaced with another found by VPATH search.  */

  this_mtime = file_mtime (file);
  check_renamed (file);
  noexist = this_mtime == NONEXISTENT_MTIME;
  if (noexist)
    DBF (DB_BASIC, _("File `%s' does not exist.\n"));
  else if (ORDINARY_MTIME_MIN <= this_mtime && this_mtime <= ORDINARY_MTIME_MAX
	   && file->low_resolution_time)
    {
      /* Avoid spurious rebuilds due to low resolution time stamps.  */
      int ns = FILE_TIMESTAMP_NS (this_mtime);
      if (ns != 0)
	error (NILF, _("*** Warning: .LOW_RESOLUTION_TIME file `%s' has a high resolution time stamp"),
	       file->name);
      this_mtime += FILE_TIMESTAMPS_PER_S - 1 - ns;
    }

  must_make = noexist;

  /* If file was specified as a target with no commands,
     come up with some default commands.  */

  if (!file->phony && file->cmds == 0 && !file->tried_implicit)
    {
      if (try_implicit_rule (file, depth))
	DBF (DB_IMPLICIT, _("Found an implicit rule for `%s'.\n"));
      else
	DBF (DB_IMPLICIT, _("No implicit rule found for `%s'.\n"));
      file->tried_implicit = 1;
    }
  if (file->cmds == 0 && !file->is_target
      && default_file != 0 && default_file->cmds != 0)
    {
      DBF (DB_IMPLICIT, _("Using default commands for `%s'.\n"));
      file->cmds = default_file->cmds;
    }

  /* Update all non-intermediate files we depend on, if necessary,
     and see whether any of them is more recent than this file.  */

  lastd = 0;
  d = file->deps;
  while (d != 0)
    {
      FILE_TIMESTAMP mtime;
      int maybe_make;

      check_renamed (d->file);

      mtime = file_mtime (d->file);
      check_renamed (d->file);

      if (is_updating (d->file))
	{
	  error (NILF, _("Circular %s <- %s dependency dropped."),
		 file->name, d->file->name);
	  /* We cannot free D here because our the caller will still have
	     a reference to it when we were called recursively via
	     check_dep below.  */
	  if (lastd == 0)
	    file->deps = d->next;
	  else
	    lastd->next = d->next;
	  d = d->next;
	  continue;
	}

      d->file->parent = file;
      maybe_make = must_make;
      dep_status |= check_dep (d->file, depth, this_mtime, &maybe_make);
      if (! d->ignore_mtime)
        must_make = maybe_make;

      check_renamed (d->file);

      {
	register struct file *f = d->file;
	if (f->double_colon)
	  f = f->double_colon;
	do
	  {
	    running |= (f->command_state == cs_running
			|| f->command_state == cs_deps_running);
	    f = f->prev;
	  }
	while (f != 0);
      }

      if (dep_status != 0 && !keep_going_flag)
	break;

      if (!running)
	d->changed = file_mtime (d->file) != mtime;

      lastd = d;
      d = d->next;
    }

  /* Now we know whether this target needs updating.
     If it does, update all the intermediate files we depend on.  */

  if (must_make || always_make_flag)
    {
      for (d = file->deps; d != 0; d = d->next)
	if (d->file->intermediate)
	  {
	    FILE_TIMESTAMP mtime = file_mtime (d->file);
	    check_renamed (d->file);
	    d->file->parent = file;
	    dep_status |= update_file (d->file, depth);
	    check_renamed (d->file);

	    {
	      register struct file *f = d->file;
	      if (f->double_colon)
		f = f->double_colon;
	      do
		{
		  running |= (f->command_state == cs_running
			      || f->command_state == cs_deps_running);
		  f = f->prev;
		}
	      while (f != 0);
	    }

	    if (dep_status != 0 && !keep_going_flag)
	      break;

	    if (!running)
	      d->changed = ((file->phony && file->cmds != 0)
			    || file_mtime (d->file) != mtime);
	  }
    }

  finish_updating (file);

  DBF (DB_VERBOSE, _("Finished prerequisites of target file `%s'.\n"));

  if (running)
    {
      set_command_state (file, cs_deps_running);
      --depth;
      DBF (DB_VERBOSE, _("The prerequisites of `%s' are being made.\n"));
      return 0;
    }

  /* If any dependency failed, give up now.  */

  if (dep_status != 0)
    {
      file->update_status = dep_status;
      notice_finished_file (file);

      --depth;

      DBF (DB_VERBOSE, _("Giving up on target file `%s'.\n"));

      if (depth == 0 && keep_going_flag
	  && !just_print_flag && !question_flag)
	error (NILF,
               _("Target `%s' not remade because of errors."), file->name);

      return dep_status;
    }

  if (file->command_state == cs_deps_running)
    /* The commands for some deps were running on the last iteration, but
       they have finished now.  Reset the command_state to not_started to
       simplify later bookkeeping.  It is important that we do this only
       when the prior state was cs_deps_running, because that prior state
       was definitely propagated to FILE's also_make's by set_command_state
       (called above), but in another state an also_make may have
       independently changed to finished state, and we would confuse that
       file's bookkeeping (updated, but not_started is bogus state).  */
    set_command_state (file, cs_not_started);

  /* Now record which prerequisites are more
     recent than this file, so we can define $?.  */

  deps_changed = 0;
  for (d = file->deps; d != 0; d = d->next)
    {
      FILE_TIMESTAMP d_mtime = file_mtime (d->file);
      check_renamed (d->file);

      if (! d->ignore_mtime)
        {
#if 1
          /* %%% In version 4, remove this code completely to
	   implement not remaking deps if their deps are newer
	   than their parents.  */
          if (d_mtime == NONEXISTENT_MTIME && !d->file->intermediate)
            /* We must remake if this dep does not
               exist and is not intermediate.  */
            must_make = 1;
#endif

          /* Set DEPS_CHANGED if this dep actually changed.  */
          deps_changed |= d->changed;
        }

      /* Set D->changed if either this dep actually changed,
	 or its dependent, FILE, is older or does not exist.  */
      d->changed |= noexist || d_mtime > this_mtime;

      if (!noexist && ISDB (DB_BASIC|DB_VERBOSE))
	{
          const char *fmt = 0;

          if (d->ignore_mtime)
            {
              if (ISDB (DB_VERBOSE))
                fmt = _("Prerequisite `%s' is order-only for target `%s'.\n");
            }
          else if (d_mtime == NONEXISTENT_MTIME)
            {
              if (ISDB (DB_BASIC))
                fmt = _("Prerequisite `%s' of target `%s' does not exist.\n");
            }
	  else if (d->changed)
            {
              if (ISDB (DB_BASIC))
                fmt = _("Prerequisite `%s' is newer than target `%s'.\n");
            }
          else if (ISDB (DB_VERBOSE))
            fmt = _("Prerequisite `%s' is older than target `%s'.\n");

          if (fmt)
            {
              print_spaces (depth);
              printf (fmt, dep_name (d), file->name);
              fflush (stdout);
            }
	}
    }

  /* Here depth returns to the value it had when we were called.  */
  depth--;

  if (file->double_colon && file->deps == 0)
    {
      must_make = 1;
      DBF (DB_BASIC,
           _("Target `%s' is double-colon and has no prerequisites.\n"));
    }
  else if (!noexist && file->is_target && !deps_changed && file->cmds == 0
           && !always_make_flag)
    {
      must_make = 0;
      DBF (DB_VERBOSE,
           _("No commands for `%s' and no prerequisites actually changed.\n"));
    }
  else if (!must_make && file->cmds != 0 && always_make_flag)
    {
      must_make = 1;
      DBF (DB_VERBOSE, _("Making `%s' due to always-make flag.\n"));
    }

  if (!must_make)
    {
      if (ISDB (DB_VERBOSE))
        {
          print_spaces (depth);
          printf (_("No need to remake target `%s'"), file->name);
          if (!streq (file->name, file->hname))
              printf (_("; using VPATH name `%s'"), file->hname);
          puts (".");
          fflush (stdout);
        }

      notice_finished_file (file);

      /* Since we don't need to remake the file, convert it to use the
         VPATH filename if we found one.  hfile will be either the
         local name if no VPATH or the VPATH name if one was found.  */

      while (file)
        {
          file->name = file->hname;
          file = file->prev;
        }

      return 0;
    }

  DBF (DB_BASIC, _("Must remake target `%s'.\n"));

  /* It needs to be remade.  If it's VPATH and not reset via GPATH, toss the
     VPATH.  */
  if (!streq(file->name, file->hname))
    {
      DB (DB_BASIC, (_("  Ignoring VPATH name `%s'.\n"), file->hname));
      file->ignore_vpath = 1;
    }

  /* Now, take appropriate actions to remake the file.  */
  remake_file (file);

  if (file->command_state != cs_finished)
    {
      DBF (DB_VERBOSE, _("Commands of `%s' are being run.\n"));
      return 0;
    }

  switch (file->update_status)
    {
    case 2:
      DBF (DB_BASIC, _("Failed to remake target file `%s'.\n"));
      break;
    case 0:
      DBF (DB_BASIC, _("Successfully remade target file `%s'.\n"));
      break;
    case 1:
      DBF (DB_BASIC, _("Target file `%s' needs remade under -q.\n"));
      break;
    default:
      assert (file->update_status >= 0 && file->update_status <= 2);
      break;
    }

  file->updated = 1;
  return file->update_status;
}

/* Set FILE's `updated' flag and re-check its mtime and the mtime's of all
   files listed in its `also_make' member.  Under -t, this function also
   touches FILE.

   On return, FILE->update_status will no longer be -1 if it was.  */

void
notice_finished_file (file)
     register struct file *file;
{
  struct dep *d;
  int ran = file->command_state == cs_running;
  int touched = 0;

  file->command_state = cs_finished;
  file->updated = 1;

  if (touch_flag
      /* The update status will be:
	 	-1	if this target was not remade;
		0	if 0 or more commands (+ or ${MAKE}) were run and won;
		1	if some commands were run and lost.
	 We touch the target if it has commands which either were not run
	 or won when they ran (i.e. status is 0).  */
      && file->update_status == 0)
    {
      if (file->cmds != 0 && file->cmds->any_recurse)
	{
	  /* If all the command lines were recursive,
	     we don't want to do the touching.  */
	  unsigned int i;
	  for (i = 0; i < file->cmds->ncommand_lines; ++i)
	    if (!(file->cmds->lines_flags[i] & COMMANDS_RECURSE))
	      goto have_nonrecursing;
	}
      else
	{
	have_nonrecursing:
	  if (file->phony)
	    file->update_status = 0;
	  else
            {
              /* Should set file's modification date and do nothing else.  */
              file->update_status = touch_file (file);

              /* Pretend we ran a real touch command, to suppress the
                 "`foo' is up to date" message.  */
              commands_started++;

              /* Request for the timestamp to be updated (and distributed
                 to the double-colon entries). Simply setting ran=1 would
                 almost have done the trick, but messes up with the also_make
                 updating logic below.  */
              touched = 1;
            }
	}
    }

  if (file->mtime_before_update == UNKNOWN_MTIME)
    file->mtime_before_update = file->last_mtime;

  if ((ran && !file->phony) || touched)
    {
      struct file *f;
      int i = 0;

      /* If -n, -t, or -q and all the commands are recursive, we ran them so
         really check the target's mtime again.  Otherwise, assume the target
         would have been updated. */

      if (question_flag || just_print_flag || touch_flag)
        {
          for (i = file->cmds->ncommand_lines; i > 0; --i)
            if (! (file->cmds->lines_flags[i-1] & COMMANDS_RECURSE))
              break;
        }

      /* If there were no commands at all, it's always new. */

      else if (file->is_target && file->cmds == 0)
	i = 1;

      file->last_mtime = i == 0 ? UNKNOWN_MTIME : NEW_MTIME;

      /* Propagate the change of modification time to all the double-colon
	 entries for this file.  */
      for (f = file->double_colon; f != 0; f = f->prev)
	f->last_mtime = file->last_mtime;
    }

  if (ran && file->update_status != -1)
    /* We actually tried to update FILE, which has
       updated its also_make's as well (if it worked).
       If it didn't work, it wouldn't work again for them.
       So mark them as updated with the same status.  */
    for (d = file->also_make; d != 0; d = d->next)
      {
	d->file->command_state = cs_finished;
	d->file->updated = 1;
	d->file->update_status = file->update_status;

	if (ran && !d->file->phony)
	  /* Fetch the new modification time.
	     We do this instead of just invalidating the cached time
	     so that a vpath_search can happen.  Otherwise, it would
	     never be done because the target is already updated.  */
	  (void) f_mtime (d->file, 0);
      }
  else if (file->update_status == -1)
    /* Nothing was done for FILE, but it needed nothing done.
       So mark it now as "succeeded".  */
    file->update_status = 0;
}

/* Check whether another file (whose mtime is THIS_MTIME)
   needs updating on account of a dependency which is file FILE.
   If it does, store 1 in *MUST_MAKE_PTR.
   In the process, update any non-intermediate files
   that FILE depends on (including FILE itself).
   Return nonzero if any updating failed.  */

static int
check_dep (file, depth, this_mtime, must_make_ptr)
     struct file *file;
     unsigned int depth;
     FILE_TIMESTAMP this_mtime;
     int *must_make_ptr;
{
  struct dep *d;
  int dep_status = 0;

  ++depth;
  start_updating (file);

  if (!file->intermediate)
    /* If this is a non-intermediate file, update it and record
       whether it is newer than THIS_MTIME.  */
    {
      FILE_TIMESTAMP mtime;
      dep_status = update_file (file, depth);
      check_renamed (file);
      mtime = file_mtime (file);
      check_renamed (file);
      if (mtime == NONEXISTENT_MTIME || mtime > this_mtime)
	*must_make_ptr = 1;
    }
  else
    {
      /* FILE is an intermediate file.  */
      FILE_TIMESTAMP mtime;

      if (!file->phony && file->cmds == 0 && !file->tried_implicit)
	{
	  if (try_implicit_rule (file, depth))
	    DBF (DB_IMPLICIT, _("Found an implicit rule for `%s'.\n"));
	  else
	    DBF (DB_IMPLICIT, _("No implicit rule found for `%s'.\n"));
	  file->tried_implicit = 1;
	}
      if (file->cmds == 0 && !file->is_target
	  && default_file != 0 && default_file->cmds != 0)
	{
	  DBF (DB_IMPLICIT, _("Using default commands for `%s'.\n"));
	  file->cmds = default_file->cmds;
	}

      /* If the intermediate file actually exists
	 and is newer, then we should remake from it.  */
      check_renamed (file);
      mtime = file_mtime (file);
      check_renamed (file);
      if (mtime != NONEXISTENT_MTIME && mtime > this_mtime)
	*must_make_ptr = 1;
	  /* Otherwise, update all non-intermediate files we depend on,
	     if necessary, and see whether any of them is more
	     recent than the file on whose behalf we are checking.  */
      else
	{
	  struct dep *lastd;

	  lastd = 0;
	  d = file->deps;
	  while (d != 0)
	    {
              int maybe_make;

	      if (is_updating (d->file))
		{
		  error (NILF, _("Circular %s <- %s dependency dropped."),
			 file->name, d->file->name);
		  if (lastd == 0)
		    {
		      file->deps = d->next;
		      free ((char *) d);
		      d = file->deps;
		    }
		  else
		    {
		      lastd->next = d->next;
		      free ((char *) d);
		      d = lastd->next;
		    }
		  continue;
		}

	      d->file->parent = file;
              maybe_make = *must_make_ptr;
	      dep_status |= check_dep (d->file, depth, this_mtime,
                                       &maybe_make);
              if (! d->ignore_mtime)
                *must_make_ptr = maybe_make;
	      check_renamed (d->file);
	      if (dep_status != 0 && !keep_going_flag)
		break;

	      if (d->file->command_state == cs_running
		  || d->file->command_state == cs_deps_running)
		/* Record that some of FILE's deps are still being made.
		   This tells the upper levels to wait on processing it until
		   the commands are finished.  */
		set_command_state (file, cs_deps_running);

	      lastd = d;
	      d = d->next;
	    }
	}
    }

  finish_updating (file);
  return dep_status;
}

/* Touch FILE.  Return zero if successful, one if not.  */

#define TOUCH_ERROR(call) return (perror_with_name (call, file->name), 1)

static int
touch_file (file)
     register struct file *file;
{
  if (!silent_flag)
    message (0, "touch %s", file->name);

#ifndef	NO_ARCHIVES
  if (ar_name (file->name))
    return ar_touch (file->name);
  else
#endif
    {
      int fd = open (file->name, O_RDWR | O_CREAT, 0666);

      if (fd < 0)
	TOUCH_ERROR ("touch: open: ");
      else
	{
	  struct stat statbuf;
	  char buf;

	  if (fstat (fd, &statbuf) < 0)
	    TOUCH_ERROR ("touch: fstat: ");
	  /* Rewrite character 0 same as it already is.  */
	  if (read (fd, &buf, 1) < 0)
	    TOUCH_ERROR ("touch: read: ");
	  if (lseek (fd, 0L, 0) < 0L)
	    TOUCH_ERROR ("touch: lseek: ");
	  if (write (fd, &buf, 1) < 0)
	    TOUCH_ERROR ("touch: write: ");
	  /* If file length was 0, we just
	     changed it, so change it back.  */
	  if (statbuf.st_size == 0)
	    {
	      (void) close (fd);
	      fd = open (file->name, O_RDWR | O_TRUNC, 0666);
	      if (fd < 0)
		TOUCH_ERROR ("touch: open: ");
	    }
	  (void) close (fd);
	}
    }

  return 0;
}

/* Having checked and updated the dependencies of FILE,
   do whatever is appropriate to remake FILE itself.
   Return the status from executing FILE's commands.  */

static void
remake_file (file)
     struct file *file;
{
  if (file->cmds == 0)
    {
      if (file->phony)
	/* Phony target.  Pretend it succeeded.  */
	file->update_status = 0;
      else if (file->is_target)
	/* This is a nonexistent target file we cannot make.
	   Pretend it was successfully remade.  */
	file->update_status = 0;
      else
        {
          const char *msg_noparent
            = _("%sNo rule to make target `%s'%s");
          const char *msg_parent
            = _("%sNo rule to make target `%s', needed by `%s'%s");

          /* This is a dependency file we cannot remake.  Fail.  */
          if (!keep_going_flag && !file->dontcare)
            {
              if (file->parent == 0)
                fatal (NILF, msg_noparent, "", file->name, "");

              fatal (NILF, msg_parent, "", file->name, file->parent->name, "");
            }

          if (!file->dontcare)
            {
              if (file->parent == 0)
                error (NILF, msg_noparent, "*** ", file->name, ".");
              else
                error (NILF, msg_parent, "*** ",
                       file->name, file->parent->name, ".");
            }
          file->update_status = 2;
        }
    }
  else
    {
      chop_commands (file->cmds);

      /* The normal case: start some commands.  */
      if (!touch_flag || file->cmds->any_recurse)
	{
	  execute_file_commands (file);
	  return;
	}

      /* This tells notice_finished_file it is ok to touch the file.  */
      file->update_status = 0;
    }

  /* This does the touching under -t.  */
  notice_finished_file (file);
}

/* Return the mtime of a file, given a `struct file'.
   Caches the time in the struct file to avoid excess stat calls.

   If the file is not found, and SEARCH is nonzero, VPATH searching and
   replacement is done.  If that fails, a library (-lLIBNAME) is tried and
   the library's actual name (/lib/libLIBNAME.a, etc.) is substituted into
   FILE.  */

FILE_TIMESTAMP
f_mtime (file, search)
     register struct file *file;
     int search;
{
  FILE_TIMESTAMP mtime;

  /* File's mtime is not known; must get it from the system.  */

#ifndef	NO_ARCHIVES
  if (ar_name (file->name))
    {
      /* This file is an archive-member reference.  */

      char *arname, *memname;
      struct file *arfile;
      int arname_used = 0;
      time_t member_date;

      /* Find the archive's name.  */
      ar_parse_name (file->name, &arname, &memname);

      /* Find the modification time of the archive itself.
	 Also allow for its name to be changed via VPATH search.  */
      arfile = lookup_file (arname);
      if (arfile == 0)
	{
	  arfile = enter_file (arname);
	  arname_used = 1;
	}
      mtime = f_mtime (arfile, search);
      check_renamed (arfile);
      if (search && strcmp (arfile->hname, arname))
	{
	  /* The archive's name has changed.
	     Change the archive-member reference accordingly.  */

          char *name;
	  unsigned int arlen, memlen;

	  if (!arname_used)
	    {
	      free (arname);
	      arname_used 