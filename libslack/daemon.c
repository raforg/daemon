/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999, 2000 raf <raf@raf.org>
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
* 20000902 raf <raf@raf.org>
*/

/*

=head1 NAME

I<libslack(daemon)> - daemon module

=head1 SYNOPSIS

    #include <slack/deamon.h>

    int daemon_started_by_init(void);
    int daemon_started_by_inetd(void);
    int daemon_prevent_core(void);
    int daemon_revoke_privileges(void);
    int daemon_file_is_safe(const char *path);
    int daemon_init(const char *name);
    int deamon_close(void);

=head1 DESCRIPTION

This module provides functions for writing daemons. There are many tasks
that need to be performed to correctly set up a daemon process. This can be
tedious. These functions perform these tasks for you.

=over 4

=cut

*/

#include "std.h"

#include <signal.h>
#include <fcntl.h>
#define __USE_BSD
#include <grp.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include "daemon.h"
#include "mem.h"
#include "err.h"
#include "lim.h"
#include "fifo.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

static struct
{
	char *lock; /* Name of the locked pid file */
}
g;

/*

C<int daemon_pidfile(const char *name)>

Creates a pid file for a daemon and locks it. The file has one line
containing the process id of the daemon. The well-known location for the
file is defined in C<PID_DIR> (C<"/var/run"> by default). The name of the
file is the name of the daemon (given by the name argument) followed by
C<".pid">. The presence of this file will prevent two deamons with the same
name from running at the same time. On success, returns 0. On error, returns
-1 with C<errno> set appropriately.

*/

static int daemon_pidfile(const char *name)
{
	mode_t mode;
	char pid[32];
	int pid_fd;
	long path_len;
	char *suffix = ".pid";
	int rc;

	path_len = limit_path();

	if (sizeof(PID_DIR) + sizeof(PATH_SEP) + strlen(name) + strlen(suffix) + 1 > path_len)
		return set_errno(ENAMETOOLONG);

	if (!g.lock && !(g.lock = mem_create(path_len, char)))
		return set_errno(ENOMEM);

	rc = snprintf(g.lock, path_len, "%s%c%s%s", PID_DIR, PATH_SEP, name, suffix);
	if (rc == -1 || rc >= path_len)
	{
		mem_destroy(g.lock);
		return set_errno(ENOSPC);
	}

	/* This is broken over NFS (Linux). */

	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if ((pid_fd = open(g.lock, O_RDWR | O_CREAT | O_EXCL, mode)) == -1)
	{
		if (errno != EEXIST)
		{
			mem_destroy(g.lock);
			return -1;
		}

		/*
		** The pidfile already exists. Is it locked?
		** If so, another invocation is still alive.
		** If not, the invocation that created it has died.
		** Open the pidfile to attempt a lock.
		*/

		if ((pid_fd = open(g.lock, O_RDWR)) == -1)
		{
			mem_destroy(g.lock);
			return -1;
		}
	}

	if (fcntl_lock(pid_fd, F_SETLK, F_WRLCK, SEEK_SET, 0, 0) == -1)
	{
		mem_destroy(g.lock);
		return -1;
	}

	/* It wasn't locked. Now we have it locked, store our pid. */

	rc = snprintf(pid, 32, "%d\n", getpid());
	if (rc == -1 || rc >= 32)
	{
		mem_destroy(g.lock);
		return set_errno(ENOSPC);
	}

	if (write(pid_fd, pid, strlen(pid)) != strlen(pid))
	{
		daemon_close();
		return -1;
	}

	/*
	** Flaw: If someone unceremoniously unlinks the pidfile,
	** we won't know about it and nothing will stop another
	** invocation from starting up.
	*/

	return 0;
}

/*

=item C<int daemon_started_by_init(void)>

If this process was started by I<init(8)>, returns 1. If not, returns 0. If
it was, we might be getting respawned so I<fork(2)> and I<exit(2)> would be
a big mistake (and unnecessary anyway since there is no controlling
terminal). The return value is cached so any subsequent calls are faster.

=cut

*/

int daemon_started_by_init(void)
{
	static int rc = -1;

	if (rc == -1)
		rc = (getppid() == 1);

	return rc;
}

/*

=item C<int daemon_started_by_inetd(void)>

If this process was started by I<inetd(8)>, returns 1. If not, returns 0. If
it was, C<stdin>, C<stdout> and C<stderr> would be opened to a socket.
Closing them would be a big mistake. We also would not need to I<fork(2)>
and I<exit(2)> because there is no controlling terminal. The return value is
cached so any subsequent calls are faster.

=cut

*/

int daemon_started_by_inetd(void)
{
	static int rc = -1;

	if (rc == -1)
	{
		size_t optlen = sizeof(int);
		int optval;

		rc = (getsockopt(STDIN_FILENO, SOL_SOCKET, SO_TYPE, &optval, &optlen) == 0);
	}

	return rc;
}

/*

=item C<int daemon_prevent_core(void)>

Prevents core files from being generated. This is used to prevent security
holes in daemons run by root. On success, returns 0. On error, returns -1
with C<errno> set appropriately.

=cut

*/

int daemon_prevent_core(void)
{
	struct rlimit limit[1] = {{ 0, 0 }};

	if (getrlimit(RLIMIT_CORE, limit) == -1)
		return -1;

	limit->rlim_cur = 0;

	return setrlimit(RLIMIT_CORE, limit);
}

/*

=item C<int daemon_revoke_privileges(void)>

Sets the effective gid to the real gid if they differ. Sets the effective
uid to the real uid if they differ. Checks that they no longer differ. On
success, returns 0. On error, returns -1.

=cut

*/

int daemon_revoke_privileges(void)
{
	uid_t uid = getuid();
	gid_t gid = getgid();
	uid_t euid = geteuid();
	gid_t egid = getegid();

#ifdef NGROUPS_MAX

	gid_t gidset[1];
	gidset[0] = gid;

	if (euid == 0 && euid != uid && (setgroups(1, gidset) == -1 || getgroups(1, gidset) != 1 || gidset[0] != gid))
		return -1;

#endif

	if (egid != gid && (setgid(gid) == -1 || getegid() != getgid()))
		return -1;

	if (uid != euid && (setuid(uid) == -1 || getuid() != geteuid()))
		return -1;

	return 0;
}

/*

=item C<int daemon_file_is_safe(const char *path)>

Checks that the file referred to by C<path> is not group or world writable.
Also checks that the containing directories are not group or world writable.
On success, returns 1 if C<path> is safe or 0 if it is not. On error,
returns -1.

=cut

*/

int daemon_file_is_safe(const char *path)
{
	struct stat status[1];
	char *tmp, *slash;

	if (!path)
		return set_errno(EINVAL);

	if (stat(path, status) == -1)
		return -1;

	if (status->st_mode & (S_IWGRP | S_IWOTH))
		return 0;

	tmp = mem_strdup(path);
	if (!tmp)
		return -1;

	for (slash = strrchr(tmp, PATH_SEP); slash; slash = strrchr(tmp, PATH_SEP))
	{
		slash[(slash == tmp) ? 1 : 0] = '\0';

		if (stat(tmp, status) == -1)
		{
			mem_release(tmp);
			return -1;
		}

		if (status->st_mode & (S_IWGRP | S_IWOTH))
		{
			mem_release(tmp);
			return 0;
		}

		if (slash == tmp)
			break;
	}

	mem_release(tmp);
	return 1;
}

/*

=item C<int daemon_init(const char *name)>

Initialises a daemon by performing the following tasks:

=over 4

=item *

If the process was not invoked by I<init(8)> or I<inetd(8)>:

=over 4

=item *

Background the process to lose process group leadership.

=item *

Start a new process session.

=item *

Under I<SVR4>, background the process again to lose process session
leadership. This prevents the process from ever gaining a controlling
terminal. This only happens when C<SVR4> is defined and
C<NO_EXTRA_SVR4_FORK> is not defined when I<libslack> is compiled.

=back

=item *

Change to the root directory so as not to hamper umounts.

=item *

Clear the umask to enable explicit file creation modes.

=item *

Close all open file descriptors. If the process was invoked by I<inetd(8)>,
C<stdin>, C<stdout> and C<stderr> are left open since they are open to a
socket.

=item *

Open C<stdin>, C<stdout> and C<stderr> to C</dev/null> in case something
requires them to be open. Of course, this is not done if the process was
invoked by I<inetd(8)>.

=item *

If C<name> is non-null, create and lock a file containing the process id of
the process. The presence of this locked file prevents two instances of a
daemon with the same name from running at the same time.

=back

On success, returns 0. On error, returns -1 with C<errno> set appropriately.

=cut

*/

int daemon_init(const char *name)
{
	pid_t pid;
	long nopen;
	int fd;

	/*
	** Don't setup a daemon-friendly process context
	** if started by init(8) or inetd(8).
	*/

	if (!(daemon_started_by_init() || daemon_started_by_inetd()))
	{
		/*
		** Background the process.
		** Lose process group leadership.
		*/

		if ((pid = fork()) == -1)
			return -1;

		if (pid)
			exit(0);

		/* Become a process session leader. */

		setsid();

#ifndef NO_EXTRA_SVR4_FORK
#ifdef SVR4
		/*
		** Lose process session leadership
		** to prevent gaining a controlling
		** terminal in SVR4.
		*/

		if ((pid = fork()) == -1)
			return -1;

		if (pid)
			exit(0);
#endif
#endif
	}

	/* Enter the root directory to prevent hampering umounts. */

	if (chdir(ROOT_DIR) == -1)
		return -1;

	/* Clear umask to enable explicit file modes. */

	umask(0);

	/*
	** We need to close all open file descriptors. Check how
	** many file descriptors we have (If indefinite, a usable
	** number (1024) will be returned).
	*/

	if ((nopen = limit_open()) == -1)
		return -1;

	/*
	** Close all open file descriptors. If started by inetd,
	** we don't close stdin, stdout and stderr.
	** Don't forget to open any future tty devices with O_NOCTTY
	** so as to prevent gaining a controlling terminal
	** (not necessary with SVR4).
	*/

	if (daemon_started_by_inetd())
	{
		for (fd = 0; fd < nopen; ++fd)
		{
			switch (fd)
			{
				case STDIN_FILENO:
				case STDOUT_FILENO:
				case STDERR_FILENO:
					break;
				default:
					close(fd);
			}
		}
	}
	else
	{
		for (fd = 0; fd < nopen; ++fd)
			close(fd);

		/*
		** Open stdin, stdout and stderr to /dev/null just in case
		** some code buried in a library somewhere expects them to be open.
		*/

		if ((fd = open("/dev/null", O_RDWR)) == -1)
			return -1;

		/*
		** This is only needed for very strange (hypothetical)
		** posix implementations where STDIN_FILENO != 0 or
		** STDOUT_FILE != 1 or STERR_FILENO != 2 (yeah, right).
		*/

		if (fd != STDIN_FILENO)
		{
			if (dup2(fd, STDIN_FILENO) == -1)
				return -1;

			close(fd);
		}

		if (dup2(STDIN_FILENO, STDOUT_FILENO) == -1)
			return -1;

		if (dup2(STDIN_FILENO, STDERR_FILENO) == -1)
			return -1;
	}

	/* Place our process id in the file system and lock it. */

	if (name)
		return daemon_pidfile(name);

	return 0;
}

/*

=item C<int daemon_close(void)>

Unlinks the locked pid file, if any. Returns 0.

=cut

*/

int daemon_close(void)
{
	if (g.lock)
	{
		unlink(g.lock);
		mem_destroy(g.lock);
	}

	return 0;
}

/*

=back

=head1 ERRORS


Additional errors may be generated and returned from the underlying system
calls. See their manual pages.

=over 4

=item ENAMETOOLONG

The C<name> passed to I<daemon_pidfile()> resulted in a path name that is
too long for the intended filesystem.

=item ENOMEM

I<daemon_pidfile()> failed to allocate memory for the the pid file's path.

=back

=head1 EXAMPLE

    #include <unistd.h>
    #include <syslog.h>
    #include <signal.h>

    #include <slack/daemon.h>
    #include <slack/prog.h>
    #include <slack/sig.h>

    void hup(int signo)
    {
        // reload config file...
    }

    void term(int signo)
    {
        daemon_close();
        exit(0);
    }

    void do_stuff()
    {
        // do stuff...
        kill(getpid(), SIGTERM);
        signal_handle_all();
    }

    int main(int ac, char **av)
    {
        if (daemon_prevent_core()                == -1 ||
            daemon_init(prog_basename(*av))      == -1 ||
            signal_set_handler(SIGHUP, 0, hup)   == -1 ||
            signal_set_handler(SIGTERM, 0, term) == -1)
            return 1;

        do_stuff();
        return 0; // unreached
    }

=head1 BUGS

It is possible to obtain a controlling terminal under I<BSD> (and even under
I<SVR4> if C<SVR4> was not defined or C<NO_EXTRA_SVR4_FORK> was defined when
I<libslack> is compiled). If anything calls I<open(2)> on a terminal device
without the C<O_NOCTTY> flag, the process doing so will obtain a controlling
terminal.

It's very likely that only I<root> can name daemon's clients since the pid
file is created in a directory that is probably only writable by I<root>. If
this is a problem, override the C<PID_DIR> macro to specify a directory that
is writable by everyone and then recompile I<libslack>.

=head1 SEE ALSO

L<daemon(1)|daemon(1)>,
L<init(8)|init(8)>,
L<inetd(8)|inetd(8)>,
L<conf(3)|conf(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<msg(3)|msg(3)>,
L<net(3)|net(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<str(3)|str(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

#include <syslog.h>

#include "msg.h"
#include "prog.h"
#include "sig.h"

void term(int signo)
{
	daemon_close();
	exit(0);
}

int main(int ac, char **av)
{
	const char *name = geteuid() ? NULL : prog_basename(*av);
	int facility = LOG_DAEMON | LOG_ERR;

	printf("Testing: daemon\n");
	printf("All tests passed\n");
	printf("\n");
	printf("    Note: can't verify syslog daemon.err output.\n");
	printf("    Look for: \"%s succeeded\" (not \"%s failed\").\n", *av, *av);

	if (daemon_prevent_core() == -1)
	{
		syslog(facility, "%s failed: daemon_prevent_core(): %s", *av, strerror(errno));
		return 1;
	}

	if (daemon_init(name) == -1)
	{
		syslog(facility, "%s failed: daemon_init(): %s", *av, strerror(errno));
		return 1;
	}

	if (signal_set_handler(SIGTERM, 0, term) == -1)
	{
		syslog(facility, "%s failed: signal_set_handler(): %s", *av, strerror(errno));
		return 1;
	}

	syslog(facility, "%s succeeded", *av);
	kill(getpid(), SIGTERM);
	signal_handle_all();
	return 0; /* unreached */
}

#endif

/* vi:set ts=4 sw=4: */
