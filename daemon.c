/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include "msg.h"
#include "prog.h"
#include "sig.h"
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
** int daemon_pidfile(const char *name)
**
** Creates a pid file for a daemon and locks it.
** The file has one line containing the process id of the daemon.
** The well-known location for the file is defined in PID_DIR
** ("/var/run" by default). The name of the file is the name of
** the daemon (giveb by the name argument) followed by ".pid".
**
** Returns 0 on success or -1 on failure (with errno set appropriately).
** Reasons for failure are: ENAMETOOLONG If the pid file's path is longer
** than the system path limit. ENOMEM If memory cannot be allocated to
** store the pid file's path. open(2). fcntl(2). write(2).
*/

static int daemon_pidfile(const char *name)
{
	mode_t mode;
	char pid[32];
	int pid_fd;
	long path_len;
	char *suffix = ".pid";

	path_len = limit_path();

	if (sizeof(PID_DIR) + sizeof(PATH_SEP) + strlen(name) + strlen(suffix) + 1 > path_len)
		return set_errno(ENAMETOOLONG);

	if (!g.lock && !(g.lock = mem_create(path_len, char)))
		return set_errno(ENOMEM);

	snprintf(g.lock, path_len, "%s%c%s%s", PID_DIR, PATH_SEP, name, suffix);

	/*
	** This is broken on NFS (Linux).
	*/

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

	/*
	** It wasn't locked. Now we have it locked, store our pid.
	*/

	snprintf(pid, 32, "%d\n", getpid());

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
** int daemon_started_by_init(void)
**
** Determines whether or not this process was started by init(8).
** If it was, we might be getting respawned so fork(2) and exit(2)
** would be a big mistake.
*/

int daemon_started_by_init(void)
{
	return (getppid() == 1);
}

/*
** int daemon_started_by_inetd(void)
**
** Determines whether or not this process was started by inetd(8).
** If it was, stdin, stdout and stderr would be opened to a socket.
** Closing them would be a big mistake. We also wouldn't need to
** fork(2) and exec(2) because there isn't a controlling terminal
** in sight.
*/

int daemon_started_by_inetd(void)
{
	socklen_t optlen = sizeof(int);
	int optval;

	return (getsockopt(STDIN_FILENO, SOL_SOCKET, SO_TYPE, &optval, &optlen) == 0);
}

/*
** int daemon_init(const char *name, sighandler_t *hup, sighandler_t *term)
**
** Initialises a daemon:
** Disables core files to prevent security holes.
** If the process wasn't started by init(8) or inetd(8):
**   Backgrounds the process to lose process group leadership.
**   Becomes a process session leader.
**   When SVR4 is defined:
**     Backgrounds the process again to lose process group leadership.
**     This prevents the process from gaining a controlling terminal.
**     Under BSD, you must still include O_NOCTTY when opening terminals
**     to prevent the process from gaining a controlling terminal.
** Changes directory to the root directory so as not to hamper umounts.
** Clears the umask to enable explicit file modes.
** If the process wasn't started by inetd(8):
**   Closes all files (as determined by sysconf(2)).
**   Opens stdin, stdout and stderr to /dev/null in case something needs them.
** If the hup parameter is non-null,
**   Registers a SIGHUP handler.
** If the term parameter is non-null,
**   Registers a SIGTERM handler.
** If the name parameter is non-null,
**   Places the process id in the file system and locks it.
** Returns 0 on success, -1 on error (with errno set appropriately).
** Reasons for failure are: getrlimit(2), setrlimit(2), fork(2), chdir(2),
** sysconf(2), open(2), dup2(2), daemon_pidfile(), sigemptyset(2),
** sigaddset(2), sigaction(2).
*/

int daemon_init(const char *name, sighandler_t *hup, sighandler_t *term)
{
	pid_t pid;
	int fd;
	long nopen;
	struct rlimit limit[1] = {{ 0, 0 }};

	/*
	** Disable core files to prevent security holes.
	*/

	if (getrlimit(RLIMIT_CORE, limit) == -1)
		return -1;

	limit->rlim_cur = 0;

	if (setrlimit(RLIMIT_CORE, limit) == -1)
		return -1;

	/*
	** Don't setup a daemon-friendly process context
	** if started by init(8) or inetd(8).
	*/

	if (!daemon_started_by_init() && !daemon_started_by_inetd())
	{
		/*
		** Background the process.
		** Lose process group leadership.
		*/

		if ((pid = fork()) == -1)
			return -1;

		if (pid)
			exit(0);

		/*
		** Become a process session leader.
		*/

		setsid();

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
	}

	/*
	** Enter the root directory to prevent hampering umounts.
	*/

	if (chdir(ROOT_DIR) == -1)
		return -1;

	/*
	** Clear umask to enable explicit file modes.
	*/

	umask(0);

	/*
	** Don't close all file descriptors if stdin, stdout and stderr
	** are the socket given to us by inetd.
	*/

	if (!daemon_started_by_inetd())
	{
		/*
		** Close all open files.
		** Don't forget to open any future
		** tty devices with O_NOCTTY so as
		** to prevent gaining a controlling
		** terminal (not necessary with SVR4).
		*/

		if ((nopen = limit_open()) == -1)
			return -1;

		for (fd = 0; fd < nopen; ++fd)
			close(fd);

		/*
		** Open stdin, stdout and stderr to /dev/null
		** just in case someone expects them to be open.
		*/

		if ((fd = open("/dev/null", O_RDWR)) == -1)
			return -1;

		/*
		** This is only needed for very strange
		** (hypothetical) posix implementations
		** where STDIN_FILENO != 0 or STDOUT_FILE != 1
		** or STERR_FILENO != 2 (yeah, right).
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

	/*
	** Register a SIGHUP handler, if required.
	*/

	if (hup && signal_set_handler(SIGHUP, 0, hup) == -1)
		return -1;

	/*
	** Register a SIGTERM handler, if required.
	*/

	if (term && signal_set_handler(SIGTERM, 0, term) == -1)
		return -1;

	/*
	** Place our process id in the file system and lock it.
	*/

	if (!name)
		return 0;

	return daemon_pidfile(name);
}

/*
** int daemon_close()
**
** Unlinks the daemon's (locked) process id file.
*/

int daemon_close()
{
	if (g.lock)
	{
		unlink(g.lock);
		mem_destroy(g.lock);
	}

	return 0;
}

#ifdef TEST

#include <syslog.h>
#include <string.h>

void term(int signo)
{
	daemon_close();
	exit(0);
}

int main(int ac, char **av)
{
	const char *name = geteuid() ? NULL : prog_basename(*av);
	int facility = LOG_DAEMON | LOG_DEBUG;

	printf("Testing: %s\n", *av);
	printf("Note: can't verify syslog daemon.debug.\n");
	printf("Check that it received: ");
	printf("\"%s succeeded\" (not \"%s failed\").\n", *av, *av);

	if (daemon_init(name, NULL, term) == -1)
	{
		syslog(facility, "%s failed: %s", *av, strerror(errno));
		return 1;
	}

	syslog(facility, "%s succeeded", *av);
	kill(getpid(), SIGTERM);
	signal_handle_all();
	return 0; // unreached
}

#endif

/* vi:set ts=4 sw=4: */
