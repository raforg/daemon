/*
* libslack - http://libslack.org/
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
* 20010215 raf <raf@raf.org>
*/

/*

=head1 NAME

I<libslack(err)> - message/error/debug/verbosity messaging module

=head1 SYNOPSIS

    #include <slack/err.h>

    void msg(const char *fmt, ...);
    void vmsg(const char *fmt, va_list args);
    void verbose(size_t level, const char *fmt, ...);
    void vverbose(size_t level, const char *fmt, va_list args);
    void debugf(size_t level, const char *fmt, ...);
    void vdebugf(size_t level, const char *fmt, va_list args);
    int error(const char *fmt, ...);
    int verror(const char *fmt, va_list args);
    void fatal(const char *fmt, ...);
    void vfatal(const char *fmt, va_list args);
    void dump(const char *fmt, ...);
    void vdump(const char *fmt, va_list args);
    void debugsysf(size_t level, const char *fmt, ...);
    void vdebugsysf(size_t level, const char *fmt, va_list args);
    int errorsys(const char *fmt, ...);
    int verrorsys(const char *fmt, va_list args);
    void fatalsys(const char *fmt, ...);
    void vfatalsys(const char *fmt, va_list args);
    void dumpsys(const char *fmt, ...);
    void vdumpsys(const char *fmt, va_list args);
    int set_errno(int errnum);

    #define debug(args)
    #define vdebug(args)
    #define debugsys(args)
    #define vdebugsys(args)
    #define check(test, msg)

=head1 DESCRIPTION

This module works with the I<prog> and I<msg> modules to provide functions
for emitting various types of message with simple call syntax and flexible
behaviour. The message types catered for are: normal, verbose, debug, error,
fatal error and dump messages. All messages are created and sent with
I<printf>-like syntax. The destinations for these messages are configurable
by the client. Calling I<prog_init()> causes normal and verbose messages to
be sent to standard output; debug, error, fatal error and dump messages to
be sent to standard error.

Calls to I<prog_out_fd()>, I<prog_out_stdout()>, I<prog_out_file()>,
I<prog_out_syslog()>, I<prog_out_none()> cause normal and verbose messages
to be sent to the specified destination. Calls to I<prog_err_fd()>,
I<prog_err_stderr()>, I<prog_err_file()>, I<prog_err_syslog()>,
I<prog_err_none()> cause error, fatal error and dump messages to be sent to
the specified destination. Calls to I<prog_dbg_fd()>, I<prog_dbg_stdout()>,
I<prog_dbg_stderr()>, I<prog_dbg_file()>, I<prog_dbg_syslog()>,
I<prog_dbg_none()> cause debug messages to be sent to the specified
destination. Calls to the generic functions I<prog_set_out()>,
I<prog_set_err()> and I<prog_set_dbg()> cause their respective message types
to be sent to the specified destination or destinations (multiplexing
messages is only possible via these functions). See I<msg(3)> for more
details.

=over 4

=cut

*/

#include "std.h"

#include "msg.h"
#include "prog.h"
#include "err.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

/*

=item C<void msg(const char *fmt, ...)>

Outputs a message to the program's normal message destination. C<fmt> is a
I<printf>-like format string and processes any remaining arguments in the
same way as I<printf(3)>.

B<Warning: Do not under any circumstances ever pass a non-literal string as
the fmt argument unless you know exactly how many conversions will take
place. Being careless with this is a very good way to build potential
security holes into your programs. The same is true for all functions that
take a printf()-like format string as an argument.>

    msg(buf);       // EVIL
    msg("%s", buf); // GOOD

=cut

*/

void msg(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vmsg(fmt, args);
	va_end(args);
}

/*

=item C<void vmsg(const char *fmt, va_list args)>

Equivalent to I<msg()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vmsg(const char *fmt, va_list args)
{
	vmsg_out(prog_out(), fmt, args);
}

/*

=item C<void verbose(size_t level, const char *fmt, ...)>

Outputs a verbose message to the program's normal message destination if
C<level> is less than or equal to the program's current verbosity level. If
the program's name has been supplied using I<prog_set_name()>, the message
will be preceeded by the name, a colon and a space. The message is also
preceeded by as many spaces as the message level. This indents messages
according to their verbosity. C<fmt> is a I<printf>-like format string and
processes any remaining arguments in the same way as I<printf(3)>. The
message is followed by a newline.

=cut

*/

void verbose(size_t level, const char *fmt, ...)
{
	if (prog_verbosity_level() >= level)
	{
		va_list args;
		va_start(args, fmt);
		vverbose(level, fmt, args);
		va_end(args);
	}
}

/*

=item C<void vverbose(size_t level, const char *fmt, va_list args)>

Equivalent to I<verbose()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vverbose(size_t level, const char *fmt, va_list args)
{
	if (prog_verbosity_level() >= level)
	{
		char msg[MSG_SIZE];
		vsnprintf(msg, MSG_SIZE, fmt, args);

		if (prog_name())
			msg_out(prog_out(), "%s: %*s%s\n", prog_name(), level, "", msg);
		else
			msg_out(prog_out(), "%*s%s\n", level, "", msg);
	}
}

/*

=item C<void debugf(size_t level, const char *fmt, ...)>

Outputs a debug message to the program's debug message destination if
C<level> is less than or equal to the current program debug level. If the
program's name has been supplied using I<prog_set_name()>, the message will
be preceeded by the name, a colon and a space. The message is also preceeded
by as many spaces as the message level. This indents debug messages
according to their debug level. C<fmt> is a I<printf>-like format string
and processes any remaining arguments in the same way as I<printf(3)>. The
message is followed by a newline.

=cut

*/

void debugf(size_t level, const char *fmt, ...)
{
	if (prog_debug_level() >= level)
	{
		va_list args;
		va_start(args, fmt);
		vdebugf(level, fmt, args);
		va_end(args);
	}
}

/*

=item C<void vdebugf(size_t level, const char *fmt, va_list args)>

Equivalent to I<debugf()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vdebugf(size_t level, const char *fmt, va_list args)
{
	if (prog_debug_level() >= level)
	{
		char msg[MSG_SIZE];
		vsnprintf(msg, MSG_SIZE, fmt, args);

		if (prog_name())
			msg_out(prog_dbg(), "%s: debug: %*s%s\n", prog_name(), level, "", msg);
		else
			msg_out(prog_dbg(), "debug: %*s%s\n", level, "", msg);
	}
}

/*

=item C<int error(const char *fmt, ...)>

Outputs an error message to the program's error message destination. If the
program's name has been supplied using I<prog_set_name()>, the message will
be preceeded by the name, a colon and a space. C<fmt> is a I<printf>-like
format string and processes any remaining arguments in the same way as
I<printf(3)>. The message is followed by a newline. Returns -1.

=cut

*/

int error(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	verror(fmt, args);
	va_end(args);

	return -1;
}

/*

=item C<int verror(const char *fmt, va_list args)>

Equivalent to I<error()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

int verror(const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	vsnprintf(msg, MSG_SIZE, fmt, args);

	if (prog_name())
		msg_out(prog_err(), "%s: %s\n", prog_name(), msg);
	else
		msg_out(prog_err(), "%s\n", msg);

	return -1;
}

/*

=item C<void fatal(const char *fmt, ...)>

Outputs an error message to the program's error message destination and then
calls I<exit(3)> with a return code of 1. If the program's name was supplied
using I<prog_set_name()>, the message will be preceeded by the name, a colon
and a space. This is followed by the string C<"fatal: ">. C<fmt> is a
I<printf>-like format string and processes any remaining arguments in the
same way as I<printf(3)>. The message is followed by a newline.

=cut

*/

void fatal(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfatal(fmt, args);
	va_end(args); /* unreached */
}

/*

=item C<void vfatal(const char *fmt, va_list args)>

Equivalent to I<fatal()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vfatal(const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	vsnprintf(msg, MSG_SIZE, fmt, args);
	error("fatal: %s", msg);
	exit(1);
}

/*

=item C<void dump(const char *fmt, ...)>

Outputs an error message to the program's error message destination and then
calls I<abort(3)>. If the program's name was supplied using
I<prog_set_name()>, the message will be preceeded by the name, a colon and a
space. This is followed by the string C<"dump: ">. C<fmt> is a
I<printf>-like format string and processes any remaining arguments in the
same way as I<printf(3)>. The message is followed by a newline.

=cut

*/

void dump(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vdump(fmt, args);
	va_end(args); /* unreached */
}

/*

=item C<void vdump(const char *fmt, va_list args)>

Equivalent to I<dump()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vdump(const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	vsnprintf(msg, MSG_SIZE, fmt, args);
	error("dump: %s", msg);
	abort();
}

/*

=item C<void debugsysf(size_t level, const char *fmt, ...)>

Outputs a debug message to the program's debug message destination if
C<level> is less than or equal to the current program debug level. If the
program's name has been supplied using I<prog_set_name()>, the message will
be preceeded by the name, a colon and a space. The message is also preceeded
by as many spaces as the message level. This indents debug messages
according to their debug level. C<fmt> is a I<printf>-like format string
and processes any remaining arguments in the same way as I<printf(3)>. The
message is followed by a colon, a space, the string representation of
C<errno> and a newline.

=cut

*/

void debugsysf(size_t level, const char *fmt, ...)
{
	if (prog_debug_level() >= level)
	{
		va_list args;
		va_start(args, fmt);
		vdebugsysf(level, fmt, args);
		va_end(args);
	}
}

/*

=item C<void vdebugsysf(size_t level, const char *fmt, value args)>

Equivalent to I<debugsysf()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vdebugsysf(size_t level, const char *fmt, va_list args)
{
	if (prog_debug_level() >= level)
	{
		char msg[MSG_SIZE];
		int errno_saved = errno;
		vsnprintf(msg, MSG_SIZE, fmt, args);
		debugf(level, "%s: %s", msg, strerror(errno_saved));
	}
}

/*

=item C<int errorsys(const char *fmt, ...)>

Outputs an error message to the program's error message destination. If the
program's name has been supplied using I<prog_set_name()>, the message will
be preceeded by the name, a colon and a space. C<fmt> is a I<printf>-like
format string and processes any remaining arguments in the same way as
I<printf(3)>. The message is followed by a colon, a space, the string
representation of C<errno> and a newline. Returns -1.

=cut

*/

int errorsys(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	verrorsys(fmt, args);
	va_end(args);
	return -1;
}

/*

=item C<int verrorsys(const char *fmt, va_list args)>

Equivalent to I<errorsys()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

int verrorsys(const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	int errno_saved = errno;
	vsnprintf(msg, MSG_SIZE, fmt, args);
	return error("%s: %s", msg, strerror(errno_saved));
}

/*

=item C<void fatalsys(const char *fmt, ...)>

Outputs an error message to the program's error message destination and then
calls I<exit(3)> with a return code of 1. If the program's name was supplied
using I<prog_set_name()>, the message will be preceeded by the name, a colon
and a space. This is followed by the string C<"fatal: ">. C<fmt> is a
I<printf>-like format string and processes any remaining arguments in the
same way as I<printf(3)>. The message is followed by a colon, a space, the
string representation of C<errno> and a newline.

=cut

*/

void fatalsys(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfatalsys(fmt, args);
	va_end(args); /* unreached */
}

/*

=item C<void vfatalsys(const char *fmt, va_list args)>

Equivalent to I<fatalsys()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vfatalsys(const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	vsnprintf(msg, MSG_SIZE, fmt, args);
	fatal("%s: %s", msg, strerror(errno));
}

/*

=item C<void dumpsys(const char *fmt, ...)>

Outputs an error message to the program's error message destination and then
calls I<abort(3)>. If the program's name was supplied using
I<prog_set_name()>, the message will be preceeded by the name, a colon and a
space. This is followed by the string C<"dump: ">. C<fmt> is a
I<printf>-like format string and processes any remaining arguments in the
same way as I<printf(3)>. The message is followed by a colon, a space, the
string representation of C<errno> and a newline.

=cut

*/

void dumpsys(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vdumpsys(fmt, args);
	va_end(args); /* unreached */
}

/*

=item C<void vdumpsys(const char *fmt, va_list args)>

Equivalent to I<dumpsys()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vdumpsys(const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	vsnprintf(msg, MSG_SIZE, fmt, args);
	dump("%s: %s", msg, strerror(errno));
}

/*

=item C<int set_errno(int errnum)>

Sets C<errno> to C<errnum> and returns -1.

=cut

*/

int set_errno(int errnum)
{
	errno = errnum;
	return -1;
}

/*

=item C< #define debug(args)>

Calls I<debugf()> unless C<NDEBUG> is defined. C<args> must be supplied with
extra parentheses. (e.g. C<debug((1, "rc=%d", rc))>).

=item C< #define vdebug(args)>

Calls I<vdebugf()> unless C<NDEBUG> is defined. C<args> must be supplied
with extra parentheses. (e.g. C<vdebug((1, fmt, args))>).

=item C< #define debugsys(args)>

Calls I<debugsysf()> unless C<NDEBUG> is defined. C<args> must be supplied
with extra parentheses. (e.g. C<debugsys((1, "fd=%d", fd))>).

=item C< #define vdebugsys(args)>

Calls I<vdebugsysf()> unless C<NDEBUG> is defined. C<args> must be supplied
with extra parentheses. (e.g. C<vdebugsys((1, fmt, args))>).

=item C< #define check(test, msg)>

Like I<assert()> but includes a C<msg> argument for including an explanation
of the test and it calls C<dump()> to terminate the program so that the
output of the assertion failure message will be sent to the right place.

=back

=head1 MT-Level

MT-Safe

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<msg(3)|msg(3)>,
L<prog(3)|prog(3)>,
L<printf(3)|printf(3)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

=cut

*/

#ifdef TEST

#include <signal.h>
#include <wait.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

int verify(int test, const char *name, const char *result)
{
	char buf[BUFSIZ];
	int fd;
	ssize_t bytes;

	if ((fd = open(name, O_RDONLY)) == -1)
	{
		printf("Test%d: failed to create err file: %s (%s)\n", test, name, strerror(errno));
		return 1;
	}

	memset(buf, '\0', BUFSIZ);
	bytes = read(fd, buf, BUFSIZ);
	close(fd);
	unlink(name);

	if (bytes == -1)
	{
		printf("Test%d: failed to read err file: %s (%s)\n", test, name, strerror(errno));
		return 1;
	}

	if (!strstr(buf, result))
	{
		printf("Test%d: err file produced incorrect input:\nshould contain:\n%s\nwas:\n%s\n", test, result, buf);
		return 1;
	}

	return 0;
}

int verifysys(int test, const char *name, const char *result, int err)
{
	char buf[BUFSIZ];

	snprintf(buf, BUFSIZ, result, strerror(err));

	return verify(test, name, buf);
}

int main(int ac, char **av)
{
	const char * const out = "err.out";
	const char * const err = "err.err";
	const char * const dbg = "err.dbg";
	const char * const core = "core";

	const char *results[10] =
	{
		"msg\n",
		"verbose\n",
		"debug: debug\n",
		"error\n",
		"fatal: fatal\n",
		"dump: dump\n",
		"debug: debugsys: %s\n",
		"errorsys: %s\n",
		"fatal: fatalsys: %s\n",
		"dump: dumpsys: %s\n"
	};

	pid_t pid;
	int errors = 0;

	printf("Testing: err\n");

	prog_set_debug_level(1);
	prog_set_verbosity_level(1);

	prog_out_file(out);
	msg("msg\n");
	errors += verify(1, out, results[0]);

	prog_out_file(out);
	verbose(0, "verbose");
	errors += verify(2, out, results[1]);

	prog_dbg_file(dbg);
	debugf(0, "debug");
	errors += verify(3, dbg, results[2]);

	prog_err_file(err);
	error("error");
	errors += verify(4, err, results[3]);

	switch (pid = fork())
	{
		case 0:
		{
			prog_err_file(err);
			fatal("fatal");
		}

		case -1:
		{
			++errors;
			printf("Test5: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status;

			if (waitpid(pid, &status, 0) == -1)
			{
				++errors, printf("Test5: failed to wait for test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(status) && WTERMSIG(status) != SIGABRT)
				++errors, printf("Test5: failed: received signal %d\n", WTERMSIG(status));

			if (WIFEXITED(status) && WEXITSTATUS(status) != 1)
				++errors, printf("Test5: failed: exit status %d\n", WEXITSTATUS(status));
		}
	}

	errors += verify(5, err, results[4]);

	switch (pid = fork())
	{
		case 0:
		{
			prog_err_file(err);
			unlink(core);
			dump("dump");
		}

		case -1:
		{
			++errors;
			printf("Test6: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			struct stat statbuf[1];
			int status;

			if (waitpid(pid, &status, 0) == -1)
			{
				++errors, printf("Test6: failed to wait for test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(status) && WTERMSIG(status) != SIGABRT)
				++errors, printf("Test6: failed: received signal %d\n", WTERMSIG(status));

			if (WIFEXITED(status) && WEXITSTATUS(status) != 1)
				++errors, printf("Test6: failed: exit status %d\n", WEXITSTATUS(status));

			if (stat(core, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test6: failed: no core file produced\n");
			else
				unlink(core);
		}
	}

	errors += verify(6, err, results[5]);

	prog_dbg_file(dbg);
	set_errno(EPERM);
	debugsysf(0, "debugsys");
	errors += verifysys(7, dbg, results[6], EPERM);

	prog_err_file(err);
	set_errno(ENOENT);
	errorsys("errorsys");
	errors += verifysys(8, err, results[7], ENOENT);

	switch (pid = fork())
	{
		case 0:
		{
			prog_err_file(err);
			set_errno(EPERM);
			fatalsys("fatalsys");
		}

		case -1:
		{
			++errors;
			printf("Test9: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status;

			if (waitpid(pid, &status, 0) == -1)
			{
				++errors;
				printf("Test9: failed to wait for test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(status) && WTERMSIG(status) != SIGABRT)
				++errors, printf("Test9: failed: received signal %d\n", WTERMSIG(status));

			if (WIFEXITED(status) && WEXITSTATUS(status) != 1)
				++errors, printf("Test9: failed: exit status %d\n", WEXITSTATUS(status));
		}
	}

	errors += verifysys(9, err, results[8], EPERM);

	switch (pid = fork())
	{
		case 0:
		{
			prog_err_file(err);
			unlink(core);
			set_errno(ENOENT);
			dumpsys("dumpsys");
		}

		case -1:
		{
			++errors;
			printf("Test10: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			struct stat statbuf[1];
			int status;

			if (waitpid(pid, &status, 0) == -1)
			{
				++errors;
				printf("Test10: failed to wait for test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(status) && WTERMSIG(status) != SIGABRT)
				++errors, printf("Test10: failed: received signal %d\n", WTERMSIG(status));

			if (WIFEXITED(status) && WEXITSTATUS(status) != 1)
				++errors, printf("Test10: failed: exit status %d\n", WEXITSTATUS(status));

			if (stat(core, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test10: failed: no core file produced\n");
			else
				unlink(core);
		}
	}

	errors += verifysys(10, err, results[9], ENOENT);

	prog_out_none();
	prog_err_none();
	prog_dbg_none();

	if (errors)
		printf("%d/10 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
