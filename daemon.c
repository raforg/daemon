/*
* daemon - http://libslack.org/daemon/
*
* Copyright (C) 1999-2001 raf <raf@raf.org>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
* or visit http://www.gnu.org/copyleft/gpl.html
*
* 20011109 raf <raf@raf.org>
*/

/*

=head1 NAME

I<daemon> - turns other processes into daemons

=head1 SYNOPSIS

 usage: daemon [options] cmd arg...
 options:

   -h, --help                     - Print a help message then exit
   -V, --version                  - Print a version message then exit
   -v, --verbose[=level]          - Set the verbosity level
   -d, --debug[=level]            - Set the debug level

   -n, --name=name                - Name the client exclusively
   -u, --user=user[.group]        - Run the client as user[.group]
   -R, --chroot=path              - Run the client with path as root
   -D, --chdir=path               - Run the client in directory path
   -m, --umask=umask              - Run the client with the given umask
   -r, --respawn                  - Respawn the client when it terminates
   -f, --force                    - Respawn the client even when it crashes
   -c, --core                     - Allow core file generation
   -s, --syslog=facility.priority - Send client's stdout/stderr to syslog
   -o, --outlog=facility.priority - Send client's stdout to syslog
   -e, --errlog=facility.priority - Send client's stderr to syslog
   -l, --log=facility.priority    - Send daemon's output to syslog
   -C, --config=path              - Specify the configuration file

=head1 DESCRIPTION

I<daemon(1)> turns other processes into daemons. There are many tasks that
need to be performed to correctly set up a daemon process. This can be
tedious. I<daemon> performs these tasks for other processes.

The preparatory tasks that I<daemon> performs for other processes are:

=over 4

=item *

First revoke any setuid or setgid privileges that I<daemon> may have been
installed with (by system administrators who laugh in the face of danger).

=item *

Process command line options.

=item *

Change the root directory if the C<--chroot> option was supplied.

=item *

Change the process uid and gid if the C<--user> option was supplied. Only
root can use this option. Note that the uid of I<daemon> itself is changed,
rather than just changing the uid of the client process.

=item *

Read the configuration file (C</etc/daemon.conf> by default, or specified
by the C<--config> option). Note: The root directory and the user must be
set before access to the configuration file can be attempted so neither
C<--chroot> nor C<--user> options may appear in the configuration file.

=item *

Disable core file generation to prevent leaking sensitive information in
daemons run by root (unless the C<--core> option was supplied).

=item *

Become a daemon process:

=over 4

=item *

If I<daemon> was not invoked by I<init(8)> or I<inetd(8)>:

=over 4

=item *

Background the process to lose process group leadership.

=item *

Start a new process session.

=item *

Under I<SVR4>, background the process again to lose process session
leadership. This prevents the process from ever gaining a controlling
terminal. This only happens when C<SVR4> is defined and
C<NO_EXTRA_SVR4_FORK> is not defined when I<libslack(3)> is compiled. Before
doing this, ignore C<SIGHUP> because when the session leader terminates, all
processes in the foreground process group are sent a C<SIGHUP> signal
(apparently). Note that this code may not execute (e.g. when started by
I<init(8)> or I<inetd(8)> or when either C<SVR4> was not defined or
C<NO_EXTRA_SVR4_FORK> was defined when I<libslack(3)> was compiled). This
means that the client can't make any assumptions about the C<SIGHUP>
handler.

=back

=item *

Change directory to the root directory so as not to hamper umounts.

=item *

Clear the umask to enable explicit file creation modes.

=item *

Close all open file descriptors. If I<daemon> was invoked by I<inetd(8)>,
C<stdin>, C<stdout> and C<stderr> are left open since they are open to a
socket.

=item *

Open C<stdin>, C<stdout> and C<stderr> to C</dev/null> in case something
requires them to be open. Of course, this is not done if I<daemon> was
invoked by I<inetd(8)>.

=item *

If the C<--name> option was supplied, create and lock a file containing the
process id of the I<daemon> process. The presence of this locked file
prevents two instances of a daemon with the same name from running at the
same time. The default location of the pidfile is C</var/run> on Linux and
C</etc> on Solaris for root or C</tmp> for ordinary users.

=back

=item *

If the C<--umask> option was supplied, set the umask to its argument.
Otherwise, set the umask to C<022> to prevent clients from accidentally
creating group or world writable files.

=item *

Set the current directory if the C<--chdir> option was supplied.

=item *

Spawn the client command specified on the command line and wait for it
to terminate.

=item *

If the C<--syslog>, C<--outlog> and/or C<--errlog> options were supplied,
the client's standard output and/or standard error are captured by I<daemon>
and sent to the respective I<syslog> destinations.

=item *

When the client terminates, I<daemon> respawns it if the C<--respawn> option
was supplied and the client terminated successfully after at least 600
seconds. If the C<--force> option was also supplied, the client will be
respawned even if it crashed or was killed by a signal after at least 600
seconds. Otherwise I<daemon> terminates.

=item *

If I<daemon> receives a C<SIGTERM> signal, it propagates the signal to the
client and then terminates.

=back

=head1 OPTIONS

=over 4

=item C<-h>, C<--help>

Display a help message and exit.

=item C<-V>, C<--version>

Display a version message and exit.

=item C<-v>I<[level]>, C<--verbose>I<[=level]>

Set the message verbosity level to I<level> (or 1 if I<level> is not
supplied). I<daemon> does not have any verbose messages so this has no
effect.

=item C<-d>I<[level]>, C<--debug>I<[=level]>

Set the debug message level to I<level> (or 1 if I<level> is not supplied).
Set to level 1 for a trace of all functions called. Set to level 2 for more
detail. Debug messages are sent to the I<syslog(3)> facility,
C<daemon.debug>.

=item C<-n=>I<name>, C<--name=>I<name>

Create and lock a pid file (C</var/run/>I<name>C<.pid>), ensuring that only
one daemon with the given I<name> is active at the same time.

=item C<-u=>I<user[.group]>, C<--user=>I<user[.group]>

Run the client as a different user (and group). This only works for root. If
the argument includes a C<.group> specifier, I<daemon> will assume the
specified group and no other. Otherwise, I<daemon> will assume all groups
that the specified user is in.

=item C<-R=>I<path>, C<--chroot=>I<path>

Change the root directory to I<path> before running the client. On some
systems, only root can do this. Note that the path to the client program and
to the configuration file (if any) must be relative to the new root path.

=item C<-D=>I<path>, C<--chdir=>I<path>

Change the directory to I<path> before running the client.

=item C<-m=>I<umask>, C<--umask=>I<umask>

Change the umask to I<umask> before running the client. I<umask> must
be a valid octal mode. The default umask is C<022>.

=item C<-r>, C<--respawn>

Respawn the client when it terminates successfully after at least 600
seconds.

=item C<-f>, C<--force>

Respawn the client even if it crashed or was killed by a signal after at
least 600 seconds. Note that this option has no effect unless the
C<--respawn> option is also supplied.

=item C<-c>, C<--core>

Allow the client to create a core file. This should only be used for
debugging as it could lead to security holes in daemons run by root.

=item C<-s=>I<facility.priority> C<--syslog=>I<facility.priority>

Capture the client's standard output and error and send it to the syslog
destination specified by I<facility.priority>.

=item C<-o=>I<facility.priority> C<--outlog=>I<facility.priority>

Capture the client's standard output and send it to the syslog destination
specified by I<facility.priority>.

=item C<-e=>I<facility.priority> C<--errlog=>I<facility.priority>

Capture the client's standard error and send it to the syslog destination
specified by I<facility.priority>.

=item C<-l=>I<facility.priority> C<--log=>I<facility.priority>

Send I<daemon>'s standard output and error to the syslog destination
specified by I<facility.priority>. By default, they are sent to
C<daemon.err>.

=item C<-C=>I<path>, C<--config=>I<path>

Specify the configuration file to use. By default, C</etc/daemon.conf> is
the configuration file if it exists and is not group or world writable and
does not exist in a group or world writable directory. The configuration
file lets you predefine options that apply to all clients and to
specifically named clients.

=back

=head1 FILES

C</etc/daemon.conf> - define default options

Each line of the configuration file consists of a client name or C<'*'>,
followed by whitespace, followed by a comma separated list of options. Blank
lines and comments (C<'#'> to end of the line) are ignored. Lines may be
continued with a C<'\'> character at the end of the line.

For example:

    *       log=daemon.err,syslog=local0.err,respawn
    test1   syslog=local0.debug,debug=9,verbose=9,core
    test2   syslog=local0.debug,debug=9,verbose=9,core

The command line options are processed first to look for a C<--config>
option. If no C<--config> option was supplied, the default file,
C</etc/daemon.conf>, is used. If the configuration file contains any generic
(C<'*'>) entries, their options are applied in order of appearance. If the
C<--name> option was supplied and the configuration file contains any entries
with the given name, their options are then applied in order of appearance.
Finally, the command line options are applied again. This ensures that any
generic options apply to all clients by default. Client specific options
override generic options. Command line options override both.

Note that the configuration file is not opened and read until after any
C<--chroot> and/or C<--user> command line options are processed. This means
that the configuration file path and the client's file path must be relative
to the C<--chroot> argument. It also means that the configuration file and
the client executable must be readable/executable by the user specified by
the C<--user> argument. It also means that neither the C<--chroot> nor the
C<--user> options will have any effect if the appear in the configuration
file. The command line options are parsed mainly to find C<--chroot>,
C<--user> and C<--config>. The C<--chroot> and C<--user> then take effect if
they are present. Then the configuration file is parsed looking for extra
options. Generic options are processed. Then named options are processed.
Finally, the command line options are processed again to make sure that they
override any generic or named options.

=head1 BUGS

If you specify (in the configuration file) that all clients allow core file
generation, there is no way to countermand that for any client (without
using an alternative configuration file). So don't do that. The same applies
to respawning.

It is possible for the client process to obtain a controlling terminal under
I<BSD> (and even under I<SVR4> if C<SVR4> was not defined or
C<NO_EXTRA_SVR4_FORK> was defined when I<libslack(3)> is compiled). If
anything calls I<open(2)> on a terminal device without the C<O_NOCTTY> flag,
the process doing so will obtain a controlling terminal and then be
susceptible to unintended termination by a C<SIGHUP>.

=head1 MAILING LISTS

The following mailing lists exist for daemon related discussion:

=over 4

 daemon-announce@libslack.org - Announcements
 daemon-users@libslack.org    - User forum
 daemon-dev@libslack.org      - Development forum

=back

To subscribe to any of these mailing lists, send a mail message to
I<listname>C<-request@libslack.org> with C<subscribe> as the message body.
e.g.

 $ echo subscribe | mail daemon-announce-request@libslack.org
 $ echo subscribe | mail daemon-users-request@libslack.org
 $ echo subscribe | mail daemon-dev-request@libslack.org

Or you can send a mail message to C<majordomo@libslack.org> with
C<subscribe> I<listname> in the message body. This way, you can
subscribe to multiple lists at the same time.
e.g.

 $ mail majordomo@libslack.org
 subscribe daemon-announce
 subscribe daemon-users
 subscribe daemon-dev
 .

A digest version of each mailing list is also available. Subscribe to
digests as above but append C<-digest> to the listname.

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<daemon(3)|daemon(3)>,
L<init(8)|init(8)>,
L<inetd(8)|inetd(8)>,
L<fork(2)|fork(2)>,
L<umask(2)|umask(2)>,
L<setsid(2)|setsid(2)>,
L<chdir(2)|chdir(2)>,
L<chroot(2)|chroot(2)>,
L<setrlimit(2)|setrlimit(2)>,
L<setgid(2)|setgid(2)>,
L<setuid(2)|setuid(2)>,
L<setgroups(2)|setgroups(2)>,
L<initgroups(3)|initgroups(3)>,
L<syslog(3)|syslog(3)>,
L<kill(2)|kill(2)>

=head1 AUTHOR

20011109 raf <raf@raf.org>

=cut

*/

#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* For setgroups(2) on Linux */
#endif

#include <slack/std.h>

#include <pwd.h>
#include <grp.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/param.h> /* For setgroups(2) on Mac OS X */

#include <slack/prog.h>
#include <slack/daemon.h>
#include <slack/sig.h>
#include <slack/err.h>
#include <slack/mem.h>
#include <slack/msg.h>
#include <slack/list.h>
#include <slack/str.h>

/* Configuration file entries */

typedef struct Config Config;

struct Config
{
	char *name;
	List *options;
};

#ifndef RESPAWN_THRESHOLD
#define RESPAWN_THRESHOLD 600
#endif

#ifndef CONFIG_PATH
#define CONFIG_PATH "/etc/daemon.conf"
#endif

/* Global variables */

static struct
{
	int ac;            /* number of command line arguments */
	char **av;         /* the command line arguments */
	char **cmd;        /* command vector to execute */
	char *name;        /* the daemon's name to use for the locked pid file */
	char *user;        /* name of user to run as */
	char *group;       /* name of group to run as */
	char *chroot;      /* name of root directory to run under */
	char *chdir;       /* name of directory to change to */
	mode_t umask;      /* set umask to this */
	int init_groups;   /* initgroups(3) if group not specified */
	uid_t uid;         /* run the client as this user */
	gid_t gid;         /* run the client as this group */
	int respawn;       /* respawn the client process when it terminates? */
	int force;         /* respawn the client process when it crashes? */
	int core;          /* do we allow core file generation? */
	char *client_out;  /* syslog spec for client stdout */
	char *client_err;  /* syslog spec for client stderr */
	char *daemon_log;  /* syslog spec for daemon output */
	int client_outlog; /* syslog facility for client stdout */
	int client_errlog; /* syslog facility for client stderr */
	int daemon_syslog; /* syslog facility for daemon output */
	int daemon_debug;  /* syslog facility for daemon debug output */
	char *config;      /* name of the config file to use - /etc/daemon.conf */
	pid_t pid;         /* the pid of the client process to run as a daemon */
	int outpipe[2];    /* pipe fds for client stdout */
	int errpipe[2];    /* pipe fds for client stderr */
	time_t spawn_time; /* when did we last spawn the client? */
}
g =
{
	0,                      /* ac */
	null,                   /* av */
	null,                   /* cmd */
	null,                   /* name */
	null,                   /* user */
	null,                   /* group */
	null,                   /* chroot */
	null,                   /* chdir */
	S_IWGRP | S_IWOTH,      /* umask */
	0,                      /* init_groups */
	0,                      /* uid */
	0,                      /* gid */
	0,                      /* respawn */
	0,                      /* force */
	0,                      /* core */
	null,                   /* client_out */
	null,                   /* client_err */
	null,                   /* daemon_log */
	0,                      /* client_outlog */
	0,                      /* client_errlog */
	LOG_DAEMON | LOG_ERR,   /* daemon_syslog */
	LOG_DAEMON | LOG_DEBUG, /* daemon_debug */
	CONFIG_PATH,            /* config */
	(pid_t)0,               /* pid */
	{ -1, -1 },             /* outpipe */
	{ -1, -1 },             /* errpipe */
	(time_t)0               /* spawn_time */
};

enum { RD = 0, WR = 1 };    /* pipe(2) fd array indexes */

/*

C<void term(int signo)>

Registered as the C<SIGTERM> handler. Propagates the C<SIGTERM> signal to
the client process and calls I<exit(3)>. I<daemon_close()> will be called by
I<atexit(3)> to unlink the locked pid file (if any).

*/

static void term(int signo)
{
	debug((1, "term(signo = %d)", signo))

	if (g.pid != 0 && g.pid != -1 && g.pid != getpid())
	{
		debug((2, "kill(term) process %d", g.pid))

		if (kill(g.pid, SIGTERM) == -1)
			errorsys("failed to kill the client process (%d)", g.pid);
	}

	exit(EXIT_SUCCESS);
}

/*

C<void chld(int signo)>

Registered as the C<SIGCHLD> handler. Does nothing.

*/

static void chld(int signo)
{
	debug((1, "chld(signo = %d)", signo))
}

/*

C<void prepare_parent(void)>

Before forking, set the term and chld signal handlers, revert to the user's
real uid and gid (if different to their effective uid and gid) and then set
the desired uid and gid from the --user option if required.

*/

static void prepare_parent(void)
{
	debug((1, "prepare_parent()"))

	debug((2, "setting sigterm action"))

	if (signal_set_handler(SIGTERM, 0, term) == -1)
		fatalsys("failed to set sigterm action");

	debug((2, "setting sigchld action"))

	if (signal_set_handler(SIGCHLD, 0, chld) == -1)
		fatalsys("failed to set sigchld action");
}

/*

C<void spawn_child(void)>

Create pipe (if syslogging) and fork. Child restores default term and chld
signal handling. If syslogging, close outpipe[read] and dup outpipe[write]
to stdout and close errpipe[read] and dup errpipe[write] to stderr. Finally,
child execs g.cmd. If syslogging, parent closes pipe[read].

*/

static void spawn_child(void)
{
	time_t spawn_time;

	debug((1, "spawn_child()"))

	if ((spawn_time = time(0)) == -1)
		fatalsys("failed to get the time");

	if (g.spawn_time)
	{
		debug((2, "checking if enough time has passed to allow respawning"))

		if (spawn_time < g.spawn_time)
			fatal("refusing to respawn: the clock's gone backwards");

		if (spawn_time - g.spawn_time <= RESPAWN_THRESHOLD)
			fatal("refusing to respawn too quickly");
	}

	g.spawn_time = spawn_time;

	if (g.client_out || g.client_err)
	{
		debug((2, "creating pipes"))

		if (g.outpipe[RD] != -1)
			close(g.outpipe[RD]), g.outpipe[RD] = -1;

		if (g.outpipe[WR] != -1)
			close(g.outpipe[WR]), g.outpipe[WR] = -1;

		if (pipe(g.outpipe) == -1)
			errorsys("failed to create pipe for --outlog");

		if (g.errpipe[RD] != -1)
			close(g.errpipe[RD]), g.errpipe[RD] = -1;

		if (g.errpipe[WR] != -1)
			close(g.errpipe[WR]), g.errpipe[WR] = -1;

		if (pipe(g.errpipe) == -1)
			errorsys("failed to create pipe for --errlog");
	}

	debug((2, "forking"))

	switch (g.pid = fork())
	{
		case -1:
			fatalsys("failed to fork");

		case 0:
		{
			debug((2, "child pid = %d", getpid()))

			debug((2, "child restoring sigterm/sigchld actions"))

			if (signal_set_handler(SIGTERM, 0, SIG_DFL) == -1)
				fatalsys("child failed to restore sigterm action, exiting");

			if (signal_set_handler(SIGCHLD, 0, SIG_DFL) == -1)
				fatalsys("child failed to restore sigchld action, exiting");

			if (g.client_out && g.outpipe[WR] != -1)
			{
				debug((2, "child close(outpipe[read]) and dup2(outpipe[write] to stdout)"))

				if (close(g.outpipe[RD]) == -1)
					fatalsys("child failed to close outpipe[read]");

				g.outpipe[RD] = -1;

				if (g.outpipe[WR] != STDOUT_FILENO)
				{
					if (dup2(g.outpipe[WR], STDOUT_FILENO) == -1)
						fatalsys("child failed to dup2(outpipe[write], stdout)");

					if (close(g.outpipe[WR]) == -1)
						fatalsys("child failed to close(outpipe[write]) after dup2()");
				}
			}

			if (g.client_err && g.errpipe[WR] != -1)
			{
				debug((2, "child close(errpipe[read]) and dup2(errpipe[write] to stderr)"))

				if (close(g.errpipe[RD]) == -1)
					fatalsys("child failed to close errpipe[read]");

				g.errpipe[RD] = -1;

				if (g.errpipe[WR] != STDERR_FILENO)
				{
					if (dup2(g.errpipe[WR], STDERR_FILENO) == -1)
						fatalsys("child failed to dup2(errpipe[write], stderr)");

					if (close(g.errpipe[WR]) == -1)
						fatalsys("child failed to close(errpipe[write]) after dup2()");
				}
			}

			debug((1, "child execing %s", *g.cmd))
			execvp(*g.cmd, g.cmd);
			fatalsys("child failed to exec %s", *g.cmd);
		}
	}

	debug((2, "parent pid = %d\n", getpid()))

	if (g.client_out && g.outpipe[WR] != -1)
	{
		debug((2, "parent close(outpipe[write])"))

		if (close(g.outpipe[WR]) == -1)
			fatalsys("parent failed to close(outpipe[write])");

		g.outpipe[WR] = -1;
	}

	if (g.client_err && g.errpipe[WR] != -1)
	{
		debug((2, "parent close(errpipe[write])"))

		if (close(g.errpipe[WR]) == -1)
			errorsys("parent failed to close(errpipe[write])");

		g.errpipe[WR] = -1;
	}
}

/*

C<void examine_child(pid_t pid)>

Wait for the child process specified by C<pid>. If I<waitpid(2)> is
interrupted by a signal, handle the signal and call I<waitpid(2)> again. If
I<waitpid(2)> fails, die. If the child was killed by a signal, die. Iff the
child terminated of its own accord, we proceed. If we need to respawn the
client, do so. Otherwise, we exit. So, if this function returns at all, a
new child will have been spawned.

*/

static void examine_child(pid_t pid)
{
	int status;
	pid_t wpid;

	debug((1, "examine_child(pid = %d)", pid))
	debug((2, "waitpid(pid = %d)", g.pid))

	while ((wpid = waitpid(g.pid, &status, 0)) == -1 && errno == EINTR)
	{
		debug((2, "waitpid(pid = %d) interrupted - handling signals and trying again", g.pid))
		signal_handle_all();
	}

	if (wpid == -1 && errno != EINTR)
		fatalsys("waitpid(%d) failed", g.pid);

	debug((2, "pid %d received sigchld for pid %d", getpid(), wpid))

	if (WIFEXITED(status))
		debug((2, "child terminated with status %d", WEXITSTATUS(status)))
	else if (WIFSIGNALED(status))
	{
		if (g.respawn && g.force)
			error("child %d killed by signal %d, force respawn", (int)pid, WTERMSIG(status));
		else
			fatal("child %d killed by signal %d, exiting", (int)pid, WTERMSIG(status));
	}
	else if (WIFSTOPPED(status)) /* can't happen - we didn't set WUNTRACED */
		fatal("child stopped by signal %d, exiting", WSTOPSIG(status));
	else /* can't happen - there are no other options */
		fatal("child died under mysterious circumstances, exiting");

	if (g.respawn)
	{
		debug((2, "about to respawn"))
		spawn_child();
	}
	else
	{
		debug((2, "child terminated, exiting"))
		exit(EXIT_SUCCESS);
	}
}

/*

C<void run(void)>

The main run loop. Calls I<prepare_parent()> and I<spawn_child()>. If we're
sending the client's stdout and/or stderr to syslog, read from the pipes and
send anything read to I<syslog(3)>. Handle any signals that arrive in the
meantime. When there is no more to read from the pipes (either the client has
died or it has closed stdout and stderr), just I<waitpid(2)> for the client
to terminate. If we're not sending the client's stdout and stderr to syslog,
just <waitpid(2)> for the client to terminate.

*/

static void run(void)
{
	debug((1, "run()"))
	debug((2, "config: name %s, uid %d, gid %d, init_groups %d, respawn %s, client stdout %s%s%s, client stderr %s%s%s, daemon syslog %s%s%s, daemon debug %s%s%s, core %s, config %s, verbose %d, debug %d",
		g.name ? g.name : "<unnamed>",
		g.uid, g.gid, g.init_groups,
		g.respawn ? "yes" : "no",
		g.client_outlog ? syslog_facility_str(g.client_outlog) : "",
		g.client_outlog ? "." : "None",
		g.client_outlog ? syslog_priority_str(g.client_outlog) : "",
		g.client_errlog ? syslog_facility_str(g.client_errlog) : "",
		g.client_errlog ? "." : "None",
		g.client_errlog ? syslog_priority_str(g.client_errlog) : "",
		g.daemon_syslog ? syslog_facility_str(g.daemon_syslog) : "",
		g.daemon_syslog ? "." : "None",
		g.daemon_syslog ? syslog_priority_str(g.daemon_syslog) : "",
		g.daemon_debug ? syslog_facility_str(g.daemon_debug) : "",
		g.daemon_debug ? "." : "None",
		g.daemon_debug ? syslog_priority_str(g.daemon_debug) : "",
		g.core ? "yes" : "no",
		g.config,
		prog_verbosity_level(),
		prog_debug_level()
	))

	prepare_parent();
	spawn_child();

	for (;;)
	{
		debug((2, "run loop - handle any signals"))

		signal_handle_all();

		/* Signals arriving here are lost */

		if (g.outpipe[RD] != -1 || g.errpipe[RD] != -1)
		{
			char buf[BUFSIZ + 1];
			fd_set readfds[1];
			int maxfd = -1;
			int n;

			debug((2, "syslogging, select() pipes, read() and syslog()"))

			FD_ZERO(readfds);

			if (g.outpipe[RD] != -1)
			{
				FD_SET(g.outpipe[RD], readfds);
				maxfd = g.outpipe[RD];
			}

			if (g.errpipe[RD] != -1)
			{
				FD_SET(g.errpipe[RD], readfds);
				if (g.errpipe[RD] > maxfd)
					maxfd = g.errpipe[RD];
			}

			if ((n = select(maxfd + 1, readfds, null, null, null)) == -1 && errno != EINTR)
			{
				errorsys("select(pipes) failed, refusing to handle client output anymore");

				if (g.outpipe[RD] != -1 && close(g.outpipe[RD]) == -1)
					errorsys("failed to close(outpipe[read])");

				if (g.errpipe[RD] != -1 && close(g.errpipe[RD]) == -1)
					errorsys("failed to close(errpipe[read])");

				g.outpipe[RD] = -1;
				g.errpipe[RD] = -1;

				continue;
			}

			if (n == -1 || errno == EINTR)
			{
				debug((9, "select(pipes) was interrupted by a signal"));
				continue;
			}

			debug((9, "select(pipes) returned %d", n))

			if (g.outpipe[RD] != -1 && FD_ISSET(g.outpipe[RD], readfds))
			{
				if ((n = read(g.outpipe[RD], buf, BUFSIZ)) > 0)
				{
					char *p, *q;

					debug((2, "read(outpipe[read]) returned %d", n))
					buf[n] = '\0';

					for (p = buf; (q = strchr(p, '\n')); p = q + 1)
					{
						debug((2, "stdout syslog(%s, %*.*s)", g.client_out, q - p, q - p, p))
						syslog(g.client_outlog, "%*.*s", q - p, q - p, p);
					}

					if (*p && (*p != '\n' || p[1] != '\0'))
					{
						debug((2, "stdout syslog(%s, %s)", g.client_out, p))
						syslog(g.client_outlog, "%s", p);
					}
				}
				else if (n == -1 && errno == EINTR)
				{
					debug((2, "read(outpipe[read]) was interrupted by a signal\n"))
					continue;
				}
				else if (n == -1)
				{
					errorsys("read(outpipe[read]) failed, refusing to handle client stdout anymore");

					if (close(g.outpipe[RD]) == -1)
						errorsys("failed to close(outpipe[read])");

					g.outpipe[RD] = -1;
				}
				else /* eof */
				{
					debug((2, "read(outpipe[read]) returned %d, closing outpipe[read]", n))

					if (close(g.outpipe[RD]) == -1)
						errorsys("failed to close(outpipe[read])");

					g.outpipe[RD] = -1;
				}
			}

			if (g.errpipe[RD] != -1 && FD_ISSET(g.errpipe[RD], readfds))
			{
				if ((n = read(g.errpipe[RD], buf, BUFSIZ)) > 0)
				{
					char *p, *q;

					debug((2, "read(errpipe[read]) returned %d", n))
					buf[n] = '\0';

					for (p = buf; (q = strchr(p, '\n')); p = q + 1)
					{
						debug((2, "stderr syslog(%s, %*.*s)", g.client_err, q - p, q - p, p))
						syslog(g.client_errlog, "%*.*s", q - p, q - p, p);
					}

					if (*p && (*p != '\n' || p[1] != '\0'))
					{
						debug((2, "stderr syslog(%s, %s)", g.client_err, p))
						syslog(g.client_errlog, "%s", p);
					}
				}
				else if (n == -1 && errno == EINTR)
				{
					debug((2, "read(errpipe[read]) was interrupted by a signal\n"))
					continue;
				}
				else if (n == -1)
				{
					errorsys("read(errpipe[read]) failed");

					if (close(g.errpipe[RD]) == -1)
						errorsys("failed to close(errpipe[read])");

					g.errpipe[RD] = -1;
				}
				else /* eof */
				{
					debug((2, "read(errpipe[RD]) returned %d, closing errpipe[read]", n))

					if (close(g.errpipe[RD]) == -1)
						errorsys("failed to close(errpipe[read])");

					g.errpipe[RD] = -1;
				}
			}
		}
		else
		{
			debug((2, "not syslogging, just waitpid() for child"))

			examine_child(g.pid);
		}
	}
}

/*

C<void store_syslog(const char *spec, char **str, int *var)>

Parse the syslog target, C<spec>. Store C<spec> in C<*str> and store
the parsed facility and priority in C<*var>.

*/

static void store_syslog(const char *spec, char **str, int *var)
{
	int facility;
	int priority;

	debug((1, "store_syslog(spec = %s)", spec))

	if (syslog_parse(spec, &facility, &priority) == -1)
		prog_usage_msg("Invalid syslog argument: failed to parse '%s'", spec);

	*str = (char *)spec;
	*var = facility | priority;
}

/*

C<void handle_syslog_option(const char *spec)>

Parse and store the client syslog option argument, C<spec>.

*/

static void handle_syslog_option(const char *spec)
{
	debug((1, "handle_syslog_option(spec = %s)", spec))

	store_syslog(spec, &g.client_out, &g.client_outlog);
	store_syslog(spec, &g.client_err, &g.client_errlog);
}

/*

C<void handle_outlog_option(const char *spec)>

Parse and store the client outlog option argument, C<spec>.

*/

static void handle_outlog_option(const char *spec)
{
	debug((1, "handle_outlog_option(spec = %s)", spec))

	store_syslog(spec, &g.client_out, &g.client_outlog);
}

/*

C<void handle_errlog_option(const char *spec)>

Parse and store the client errlog option argument, C<spec>.

*/

static void handle_errlog_option(const char *spec)
{
	debug((1, "handle_errlog_option(spec = %s)", spec))

	store_syslog(spec, &g.client_err, &g.client_errlog);
}

/*

C<void handle_log_option(const char *spec)>

Parse and store the daemon syslog option argument, C<spec>.

*/

static void handle_log_option(const char *spec)
{
	debug((1, "handle_log_option(spec = %s)", spec))

	store_syslog(spec, &g.daemon_log, &g.daemon_syslog);
}

/*

C<void handle_user_option(char *spec)>

Parse and store the client user[.group] option argument, C<spec>.

*/

static void handle_user_option(char *spec)
{
	struct passwd *pwd;
	struct group *grp;
	char **member;

	debug((1, "handle_user_option(spec = %s)", spec))

	if (getuid() || geteuid())
		prog_usage_msg("The --user option only works for root");

	g.user = spec;
	g.group = strchr(spec, '.');

	if (g.group)
		*g.group++ = '\0';

	g.init_groups = (g.group == null);

	if (!(pwd = getpwnam(g.user)))
		prog_usage_msg("Unknown user: '%s'", g.user);

	g.uid = pwd->pw_uid;
	g.gid = pwd->pw_gid;

	if (g.group)
	{
		if (!(grp = getgrnam(g.group)))
			prog_usage_msg("Unknown group: '%s'", g.group);

		if (grp->gr_gid != pwd->pw_gid)
		{
			for (member = grp->gr_mem; *member; ++member)
				if (!strcmp(*member, g.user))
					break;

			if (!*member)
				prog_usage_msg("User %s is not in group %s", g.user, g.group);
		}

		g.gid = grp->gr_gid;
	}
}

/*

C<static void handle_umask_option(const char *spec)>

Parse and store the umask option argument, C<spec>.

=cut

*/

static void handle_umask_option(const char *spec)
{
	char *end;
	long val;

	val = strtol(spec, &end, 8);

	if (end == spec || *end || val < 0 || val > 0777)
		prog_usage_msg("Argument to --umask must be a valid octal mode");

	g.umask = val;
}

/*

C<Config *config_create(char *name, char *options)>

Create a I<Config> object from a name and a comma separated list of C<options>.

*/

static Config *config_create(char *name, char *options)
{
	Config *config;
	char *tok;

	debug((1, "config_create(name = \"%s\", options = \"%s\")", name, options))

	if (!(config = mem_new(Config)))
		return null;

	if (!(config->name = mem_strdup(name)))
	{
		mem_release(config);
		return null;
	}

	if (!(config->options = list_create(free)))
	{
		mem_release(config->name);
		mem_release(config);
		return null;
	}

	for (tok = strtok(options, ","); tok; tok = strtok(tok + strlen(tok) + 1, ","))
	{
		size_t size = strlen(tok) + 3;
		char *option;

		option = mem_create(size, char);
		if (!option)
		{
			list_release(config->options);
			mem_release(config->name);
			mem_release(config);
			return null;
		}

		strlcpy(option, "--", size);
		strlcat(option, tok, size);

		if (!list_append(config->options, option))
		{
			mem_release(option);
			list_release(config->options);
			mem_release(config->name);
			mem_release(config);
			return null;
		}
	}

	return config;
}

/*

C<void config_release(Config *config)>

Release all memory associated with C<config>.

*/

static void config_release(Config *config)
{
	mem_release(config->name);
	list_release(config->options);
	mem_release(config);
}

/*

C<void config_parse(void *obj, const char *path, char *line, size_t lineno)>

Parse a C<line> of the configuration file, storing results in C<obj> which
is a list of lists of strings. C<lineno> is the current line number within
the configuration file.

*/

#define is_space(c) isspace((int)(unsigned char)(c))

static void config_parse(void *obj, const char *path, char *line, size_t lineno)
{
	List *list = (List *)obj;
	Config *config;
	char name[128], *n = name;
	char options[1024], *o = options;
	char *s = line;

	debug((1, "config_parse(obj = %p, path = %s, line = \"%s\", lineno = %d)", obj, path, line, lineno))

	while (*s && is_space(*s))
		++s;

	while ((n - name) < 128 && *s && !is_space(*s))
	{
		if (*s == '\\')
			*n++ = *s++;
		*n++ = *s++;
	}

	*n = '\0';

	while (*s && is_space(*s))
		++s;

	while ((o - options) < 1024 && *s && !is_space(*s))
	{
		if (*s == '\\')
			*o++ = *s++;
		*o++ = *s++;
	}

	*o = '\0';

	if (!*name || !*options)
		prog_usage_msg("config: syntax error in %s, line %d\n", path, lineno);

	if (!(config = config_create(name, options)) || !list_append(list, config))
		prog_usage_msg("config: out of memory");
}

/*

C<void config_process(List *conf, char *target)>

Searches for C<target> in C<conf> and processes the all configuration lines
that match C<target>.

*/

static void config_process(List *conf, char *target)
{
	int ac;
	char **av;
	Config *config;
	int j;

	while (list_has_next(conf) == 1)
	{
		config = (Config *)list_next(conf);

		if (!strcmp(config->name, target))
		{
			if (!(av = mem_create(list_length(config->options) + 2, char *)))
				prog_usage_msg("config: out of memory");

			av[0] = (char *)prog_name();

			for (j = 1; list_has_next(config->options) == 1; ++j)
				if (!(av[j] = mem_strdup(list_next(config->options))))
					prog_usage_msg("config: out of memory");

			av[ac = j] = null;
			optind = 0;
			prog_opt_process(ac, av);
			mem_release(av); /* Leak av elements since g might refer to them now */
		}
	}
}

/*

C<void config(void)>

Parse the configuration file, if any, and process the contents as command
line options. Generic options are applied to all clients. Options specific
to a particular named client override the generic options. Command line options
override both specific and generic options.

*/

static void config(void)
{
	List *conf;

	debug((1, "config()"))

	/* Check the the config file is safe */

	switch (daemon_path_is_safe(g.config))
	{
		case  0: error("Ignoring unsafe %s", g.config);
		case -1: return;
	}

	/* Parse the config file */

	if (!(conf = list_create((list_release_t *)config_release)))
		return;

	if (!daemon_parse_config(g.config, conf, config_parse))
	{
		list_release(conf);
		return;
	}

	/* Apply generic options */

	config_process(conf, "*");

	/* Override with specific options */

	if (g.name)
		config_process(conf, g.name);

	/* Override with command line options */

	optind = 0;
	prog_opt_process(g.ac, g.av);

	list_release(conf);
}

/*

C<Option daemon_optab[];>

Application specific command line options.

*/

static Option daemon_optab[] =
{
	{
		"name", 'n', "name", "Prevent multiple named instances",
		required_argument, OPT_STRING, OPT_VARIABLE, &g.name
	},
	{
		"user", 'u', "user[.group]", "Run the client as user[.group]",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)handle_user_option
	},
	{
		"chroot", 'R', "path", "Run the client with path as root",
		required_argument, OPT_STRING, OPT_VARIABLE, &g.chroot
	},
	{
		"chdir", 'D', "path", "Run the client in directory path",
		required_argument, OPT_STRING, OPT_VARIABLE, &g.chdir
	},
	{
		"umask", 'm', "umask", "Run the client with the given umask",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)handle_umask_option
	},
	{
		"respawn", 'r', null, "Respawn the client when it terminates",
		no_argument, OPT_NONE, OPT_VARIABLE, &g.respawn
	},
	{
		"force", 'f', null, "Respawn the client even when it crashes",
		no_argument, OPT_NONE, OPT_VARIABLE, &g.force
	},
	{
		"core", 'c', null, "Allow core file generation",
		no_argument, OPT_NONE, OPT_VARIABLE, &g.core
	},
	{
		"syslog", 's', "facility.priority", "Send client's stdout/stderr to syslog",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)handle_syslog_option
	},
	{
		"outlog", 'o', "facility.priority", "Send client's stdout to syslog",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)handle_outlog_option
	},
	{
		"errlog", 'e', "facility.priority", "Send client's stderr to syslog",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)handle_errlog_option
	},
	{
		"log", 'l', "facility.priority", "Send daemon's output to syslog",
		required_argument, OPT_STRING, OPT_FUNCTION, (void *)handle_log_option
	},
	{
		"config", 'C', "path", "Specify the configuration file",
		required_argument, OPT_STRING, OPT_VARIABLE, &g.config
	},
	{
		null, '\0', null, null, 0, 0, 0, null
	}
};

static Options options[1] = {{ prog_options_table, daemon_optab }};

/*

C<void id(void)>

Supplies program identification for use in help, version and error messages.

*/

static void id(void)
{
	prog_set_name(DAEMON_NAME);
	prog_set_version(DAEMON_VERSION);
	prog_set_date(DAEMON_DATE);
	prog_set_syntax("[options] cmd arg...");
	prog_set_options(options);
	prog_set_author("raf <raf@raf.org>");
	prog_set_contact(prog_author());
	prog_set_url(DAEMON_URL);
	prog_set_legal
	(
		"Copyright (C) 1999-2001 raf <raf@raf.org>\n"
		"\n"
		"This is free software released under the terms of the GPL:\n"
		"\n"
		"    http://www.gnu.org/copyleft/gpl.html\n"
		"\n"
		"There is no warranty; not even for merchantability or fitness\n"
		"for a particular purpose.\n"
#ifndef HAVE_GETOPT_LONG
		"\n"
		"Includes the GNU getopt functions:\n"
		"    Copyright (C) 1997, 1998 Free Software Foundation, Inc.\n"
#endif
	);

	prog_set_desc
	(
		"Daemon turns other processes into daemons.\n"
		"See the daemon(1) manpage for more information.\n"
	);
}

/*

C<void init(int ac, char **av)>

Initialises the program. Revokes any setuid/setgid privileges. Processes
command line options. Processes the configuration file. Calls
I<daemon_prevent_core()> unless the C<--core> option was supplied. Calls
I<daemon_init()> with the C<--name> option's argument, if any. Arranges to
have C<SIGTERM> signals propagated to the client process. And stores the
remaining command line arguments to be I<execvp()>d later.

*/

static void init(int ac, char **av)
{
	int a;

	prog_init();
	id();

	if (daemon_revoke_privileges() == -1)
		fatalsys("failed to revoke uid/gid privileges: uid/gid = %d/%d euid/egid = %d/%d", getuid(), getgid(), geteuid(), getegid());

	if ((a = prog_opt_process(g.ac = ac, g.av = av)) == ac)
		prog_usage_msg("Invalid arguments: no command supplied");

	if (g.chroot && chroot(g.chroot) == -1)
		fatalsys("failed to change root directory to %s", g.chroot);

	if (g.uid && daemon_become_user(g.uid, g.gid, (g.init_groups) ? g.user : null) == -1)
	{
		struct group *grp = getgrgid(g.gid);
		struct passwd *pwd = getpwuid(g.uid);
		fatalsys("failed to set user/group to %s/%s (%d/%d): uid/gid = %d/%d euid/egid = %d/%d", (pwd) ? pwd->pw_name : "<noname>", (grp) ? grp->gr_name : "<noname>", g.uid, g.pid, getuid(), getgid(), geteuid(), getegid());
	}

	config();

	if (!g.core && daemon_prevent_core() == -1)
		fatalsys("failed to prevent core file generation");

	if (!(g.cmd = mem_create(ac - a + 1, char *)))
		prog_usage_msg("out of memory");

	memmove(g.cmd, av + a, (ac - a) * sizeof(char *));
	g.cmd[ac - a] = null;

	if (daemon_init(g.name) == -1)
		fatalsys("failed to become a daemon");

	umask(g.umask);

	if (g.name && atexit((void (*)(void))daemon_close) == -1)
		fatalsys("failed to atexit(daemon_close)");

	if (prog_dbg_syslog(null, 0, g.daemon_debug & LOG_FACMASK, g.daemon_debug & LOG_PRIMASK) == -1)
		fatalsys("failed to start debug delivery to %s.%s", syslog_facility_str(g.daemon_debug), syslog_priority_str(g.daemon_debug));

	if (prog_err_syslog(null, 0, g.daemon_syslog & LOG_FACMASK, g.daemon_syslog & LOG_PRIMASK) == -1)
		fatalsys("failed to start error delivery to %s.%s", syslog_facility_str(g.daemon_syslog), syslog_priority_str(g.daemon_syslog));

	if (g.chdir && chdir(g.chdir) == -1)
		fatalsys("failed to change directory to %s", g.chdir);
}

/*

C<int main(int ac, char **av)>

Initialise the program with the command line arguments specified in
C<ac> and C<av> then run it.

*/

int main(int ac, char **av)
{
	setlocale(LC_ALL, "");
	init(ac, av);
	run();
	return EXIT_SUCCESS; /* unreached */
}

/* vi:set ts=4 sw=4: */
