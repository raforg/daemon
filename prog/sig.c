/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (C) 1999 raf <raf2@zip.com.au>
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
*/

/*

=head1 NAME

I<libprog(sig)> - ANSI C compliant signal handler module

=head1 SYNOPSIS

    #include <prog/sig.h>

    typedef void sighandler_t(int signo);

    int signal_set_handler(int signo, int flags, sighandler_t *handler);
    int signal_addset(int signo_handled, int signo_blocked);
    int signal_received(int signo);
    int signal_raise(int signo);
    int signal_handle(int signo);
    void signal_handle_all(void);

=head1 DESCRIPTION

This module provides functions for ANSI C compliant signal handling. ANSI C
compliant signal handlers may only set a single value of type
C<sig_atomic_t>. This is a very restrictive requirement. This module allows
you to specify unrestricted signal handlers while (almost) transparently
enforcing ANSI C compliance.

When a handled signal arrives, an ANSI C compliant signal handler is invoked
to merely record the fact that the signal was received. Then, in the main
thread of execution, when I<signal_handle()> or I<signal_handle_all()> is
invoked, the client supplied signal handlers for all signals received since
the last invocation of I<signal_handle()> or I<signal_handle_all()> are
invoked.

Since the user supplied signal handlers execute in the main thread on
execution, they are not subject to the normal restrictions on signal
handlers. Also, they will execute with the same signals blocked as the real
signal handler.

=over 4

=cut

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
	signal_handler_t handler[NSIG];
}
g;

static volatile sig_atomic_t g_received[NSIG];

/*

C<void signal_catcher(int signo)>

This is an ANSI C compliant signal handler function. It is used to catch all
signals. It records that the signal C<signo> was received.

*/

static void signal_catcher(int signo)
{
	++g_received[signo];
}

/*

=item C<int signal_set_handler(int signo, int flags, sighandler_t *handler)>

Installs C<handler> as the signal handler for the signal C<signo>. C<flags>
is used as the I<sa_flags> field of the C<signal_handler_t> argument to
I<sigaction(2)>. The actual function set as the signal handler is not
C<handler>. It is an ANSI C (much more restrictive than POSIX in this
regard) compliant function that just records the fact that a signal was
received. C<handler> will only be invoked when the client invokes
I<signal_handle()> or I<signal_handle_all()> from the main thread of
execution. So there are no restrictions on C<handler>. When C<handler> is
invoked, the C<signo> signal will be blocked. Other signals can also be
blocked when C<handler> is invoked using I<signal_addset()>. Returns 0 on
success, or -1 on error with C<errno> set to indicate the reason.

=cut

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

	return sigaction(signo, h->action, NULL);
}

/*

=item C<int signal_addset(int signo_handled, int signo_blocked)>

Adds C<signo_blocked> to the set of signals that will be blocked when the
handler for signal C<signo_handled> is invoked. This must not be called
before the call to I<signal_set_handler()> for C<signo_handled> which
initialises the signal set to include C<signo_handled>. Return 0 on success,
or -1 on error with C<errno> set to indicate the reason.

=cut

*/

int signal_addset(int signo_handled, int signo_blocked)
{
	signal_handler_t *h = &g.handler[signo_handled];

	return sigaddset(&h->action->sa_mask, signo_blocked);
}

/*

=item C<int signal_received(int signo)>

Returns the number of times that the signal C<signo> has been received since
the last call to I<signal_handle(signo)> or I<signal_handle_all()>. Returns
-1 if C<signo> is out of range with C<errno> set to C<EINVAL>.

=cut

*/

int signal_received(int signo)
{
	if (signo < 0 || signo >= NSIG)
		return set_errno(EINVAL);

	return g_received[signo];
}

/*

=item C<void signal_raise(int signo)>

Simulates the receipt of the signal specified by C<signo>. Returns the
number of unhandled C<signo> signals (including this one) on success, or -1
if C<signo> is out of range with C<errno> set to C<EINVAL>.

=cut

*/

int signal_raise(int signo)
{
	if (signo < 0 || signo >= NSIG)
		return set_errno(EINVAL);

	return ++g_received[signo];
}

/*

=item C<int signal_handle(int signo)>

Executes the installed signal handler for the signal C<signo>. The C<signo>
signal (and any others added with I<signal_addset()>) is blocked during the
execution of the signal handler. Clears the received status of the C<signo>
signal. Returns 0 on success, or -1 on error with C<errno> set to indicate
the reason.

=cut

*/

int signal_handle(int signo)
{
	signal_handler_t *h = &g.handler[signo];
	sigset_t origmask[1];

	if (sigprocmask(SIG_BLOCK, &h->action->sa_mask, origmask) == -1)
		return -1;

	g_received[signo] = 0;
	g.handler[signo].handler(signo);

	return sigprocmask(SIG_SETMASK, origmask, NULL);
}

/*

=item C<void signal_handle_all(void)>

Executes the installed signal handlers for all signals that have been
received since the last call to I<signal_handle()> or
I<signal_handle_all()>. During the execution of each signal handler, the
corresponding signal (and possibly others) will be blocked. Clears the
received status of all signals handled.

=cut

*/

void signal_handle_all(void)
{
	int signo;

	for (signo = 0; signo < NSIG; ++signo)
		if (signal_received(signo))
			signal_handle(signo);
}

/*

=back

=head1 SEE ALSO

L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<msg(3)|msg(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>

=head1 AUTHOR

raf <raf2@zip.com.au>

=cut

*/

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

static void child(void)
{
	char msg[BUFSIZ];
	ssize_t n;

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
	int sync[2];
	pid_t pid;
	int errors = 0;

	printf("Testing: sig\n");

	if (signal_set_handler(SIGHUP, 0, hup) == -1)
	{
		fprintf(stderr, "Failed to set the SIGHUP handler\n");
		return 1;
	}

	if (signal_set_handler(SIGTERM, 0, term) == -1)
	{
		fprintf(stderr, "Failed to set the SIGTERM handler\n");
		return 1;
	}

	if (pipe(sync) == -1)
	{
		fprintf(stderr, "Failed to create a pipe\n");
		return 1;
	}

	switch (pid = fork())
	{
		case 0:
		{
			if (!freopen(out, "w", stdout))
			{
				fprintf(stderr, "Failed to freopen stdout to %s (%s)\n", out, strerror(errno));
				exit(1);
			}

/* fprintf(stderr, "ready to die\n"); */
/* fprintf(stderr, "about to sync\n"); */
			close(sync[0]);
			write(sync[1], "1", 1);
			close(sync[1]);

			child();
			break; /* unreached */
		}

		case -1:
		{
			errors += 3, printf("Test1: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status[1];
			char ack;

/* fprintf(stderr, "about to sync\n"); */
			close(sync[1]);
			read(sync[0], &ack, 1);
			close(sync[0]);

/* fprintf(stderr, "ready to kill\n"); */
			if (kill(pid, SIGHUP) == -1)
				++errors, printf("Test1: failed to perform test - kill(%d, HUP) failed (%s)\n", (int)pid, strerror(errno));

			if (kill(pid, SIGTERM) == -1)
			{
				++errors, printf("Test2: failed to perform test - kill(%d, TERM) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (waitpid(pid, status, 0) == -1)
			{
				++errors;
				printf("Test2: could not perform test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
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
