/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdio.h>

#include <signal.h>
#include <errno.h>

#include "sig.h"
#include "err.h"

typedef struct signal_handler_t signal_handler_t;

struct signal_handler_t
{
	struct sigaction action[1];
	sighandler_t *handler;
};

static struct
{
	int init;
	sigset_t set[1];
	signal_handler_t handler[NSIG];
}
g;

static volatile sig_atomic_t g_received[NSIG];

/*
** static int signal_add(int signo)
**
** Adds a signal number to the program's signal set.
*/

static int signal_add(int signo)
{
	if (!g.init)
	{
		if (sigemptyset(g.set) == -1)
			return -1;

		++g.init;
	}

	return sigaddset(g.set, signo);
}

/*
** int signal_in(int signo)
**
** Returns whether or not signo is in the program's signal set.
*/

int signal_in(int signo)
{
	return sigismember(g.set, signo);
}

/*
** void signal_catcher(int signo)
**
** This is an ANSI C compliant signal handler function.
** It is used to catch all signals. It records that the
** signal signo was received.
*/

static void signal_catcher(int signo)
{
	++g_received[signo];
}

/*
** int signal_set_handler(int signo, int flags, sighandler_t *handler)
**
** Installs handler as the signal handler for the signal signo. flags
** is used as the sa_flags field of the signal_handler_t argument to
** sigaction(2). The actual finction set as the signal handler is not
** handler. It is an ANSI C (much more restrictive than POSIX.1 in this
** regard) compliant function that just records the fact that a signal
** was reeived. handler will only be invoked when the user invokes
** signal_handle() or signal_handle_all() from the normal thread of
** execution. So there are no restrictions on handler.
** Returns -1 on error.
*/

int signal_set_handler(int signo, int flags, sighandler_t *handler)
{
	signal_handler_t *h = &g.handler[signo];

	sigemptyset(&h->action->sa_mask);
	sigaddset(&h->action->sa_mask, signo);
	h->action->sa_flags = flags;
	h->action->sa_handler = signal_catcher;
	h->handler = handler;
	g_received[signo] = 0;

	if (signal_add(signo) == -1)
		return -1;

	return sigaction(signo, h->action, NULL);
}

/*
** int signal_received(int signo)
**
** Returns the number of times that the signal signo has been received
** since the last call to signal_handle(signo) or signal_handle_all().
** Returns -1 if signo is out of range with errno set to EINVAL.
*/

int signal_received(int signo)
{
	if (signo < 0 || signo >= NSIG)
		return set_errno(EINVAL);

	return g_received[signo];
}

/*
** int signal_handle(int signo)
**
** Executes the installed signal handler for the signal signo.
** The signo signal blocked during the execution of the signal handler.
** Clears the received status of the signo signal. Returns -1 on error.
*/

int signal_handle(int signo)
{
	signal_handler_t *h = &g.handler[signo];
	sigset_t origmask[1];

	g_received[signo] = 0;

	if (sigprocmask(SIG_BLOCK, &h->action->sa_mask, origmask) == -1)
		return -1;

	g.handler[signo].handler(signo);

	return sigprocmask(SIG_SETMASK, origmask, NULL);
}

/*
** void signal_handle_all(void)
**
** Executes the installed signal handlers for all signals that have been
** reveived and not yet handled during a call to signal_handle() or
** signal_handle_all(). During the execution of each signal handler,
** the corresponding signal will be blocked. Clears the received status
** of all signals handled. Returns -1 on error.
*/

void signal_handle_all(void)
{
	int signo;

	for (signo = 0; signo < NSIG; ++signo)
		if (signal_in(signo) && signal_received(signo))
			signal_handle(signo);
}

#ifdef TEST

#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <wait.h>

#include <sys/stat.h>

const char * const results[2] =
{
	"Received SIGHUP\n",
	"Received SIGTERM\n",
};

static void hup(int signo)
{
	printf(results[0]);
}

static void term(int signo)
{
	printf(results[1]);
	exit(0);
}

static void child(const char *out)
{
	char msg[BUFSIZ];
	ssize_t n;

	freopen(out, "w", stdout);

	for (;;)
	{
		signal_handle_all();

		/*
		** Signals arriving here are lost.
		*/

		while ((n = read(STDIN_FILENO, msg, BUFSIZ)) > 0)
			fprintf(stderr, "%*.*s", n, n, msg);

		switch (n)
		{
			case -1:
			{
				if (errno != EINTR)
				{
					fprintf(stderr, "read error\n");
					exit(1);
				}

				signal_handle_all();
				break;
			}

			default:
			{
				fprintf(stderr, "read = %d\n", n);
				exit(1);
			}
		}
	}
}

static int verify(int test, const char *name, const char *msg1, const char *msg2)
{
	char buf[BUFSIZ];
	int fd;
	ssize_t bytes;
	size_t msg1_length;

	if ((fd = open(name, O_RDONLY)) == -1)
	{
		printf("Test %d: failed to create sig file: %s (%s)\n", test, name, strerror(errno));
		return 1;
	}

	memset(buf, '\0', BUFSIZ);
	bytes = read(fd, buf, BUFSIZ);
	close(fd);
	unlink(name);

	if (bytes == -1)
	{
		printf("Test %d: failed to read sig file: %s (%s)\n", test, name, strerror(errno));
		return 1;
	}

	msg1_length = strlen(msg1);

	if (strncmp(buf, msg1, msg1_length))
	{
		printf("Test %d: msg file produced incorrect input:\nshould be\n%s\nwas:\n%*.*s\n", test, msg1, msg1_length, (int)msg1_length, buf);
		return 1;
	}

	if (strcmp(buf + msg1_length, msg2))
	{
		printf("Test %d: msg file produced incorrect input:\nshould be:\n%s\nwas:\n%s\n", test + 1, msg2, buf + msg1_length);
		return 1;
	}

	return 0;
}

int main(int ac, char **av)
{
	const char *out = "sig.out";
	pid_t pid;
	int errors = 0;

	printf("Testing: %s\n", *av);

	if (signal_set_handler(SIGHUP, 0, hup) == -1)
	{
		fprintf(stderr, "Failed to set SIGHUP handler\n");
		return 1;
	}

	if (signal_set_handler(SIGTERM, 0, term) == -1)
	{
		fprintf(stderr, "Failed to set SIGTERM handler\n");
		return 1;
	}

	switch (pid = fork())
	{
		case 0:
		{
			child(out);
			break; /* unreached */
		}

		case -1:
		{
			errors += 3;
			printf("Test1: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status[1];

			if (kill(pid, SIGHUP) == -1)
				++errors, printf("Test1: failed to perform test - kill(%d, HUP) failed (%s)\n", pid, strerror(errno));

			if (kill(pid, SIGTERM) == -1)
			{
				++errors, printf("Test2: failed to perform test - kill(%d, TERM) failed (%s)\n", pid, strerror(errno));
				break;
			}

			if (waitpid(pid, status, 0) == -1)
			{
				++errors;
				printf("Test2: could not perform test - waitpid(%d) failed (%s)\n", pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(*status) && WTERMSIG(*status) != SIGABRT)
				++errors, printf("Test2: failed: received signal %d\n", WTERMSIG(*status));

			if (WIFEXITED(*status) && WEXITSTATUS(*status) != 0)
				++errors, printf("Test2: failed: exit status %d\n", WEXITSTATUS(*status));

			errors += verify(1, out, results[0], results[1]);

			break;
		}
	}

	if (errors)
		printf("%d/2 test failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
