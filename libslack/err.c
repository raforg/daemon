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
* 20011109 raf <raf@raf.org>
*/

/*

=head1 NAME

I<libslack(err)> - message/error/debug/verbosity/alert messaging module

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
    void alert(int priority, const char *fmt, ...);
    void valert(int priority, const char *fmt, va_list args);
    void debugsysf(size_t level, const char *fmt, ...);
    void vdebugsysf(size_t level, const char *fmt, va_list args);
    int errorsys(const char *fmt, ...);
    int verrorsys(const char *fmt, va_list args);
    void fatalsys(const char *fmt, ...);
    void vfatalsys(const char *fmt, va_list args);
    void dumpsys(const char *fmt, ...);
    void vdumpsys(const char *fmt, va_list args);
    void alertsys(int priority, const char *fmt, ...);
    void valertsys(int priority, const char *fmt, va_list args);
    int set_errno(int errnum);
    void *set_errnull(int errnum);

    #define debug(args)
    #define vdebug(args)
    #define debugsys(args)
    #define vdebugsys(args)
    #define check(test, msg)

=head1 DESCRIPTION

This module works with the I<prog> and I<msg> modules to provide functions
for emitting various types of message with simple call syntax and flexible
behaviour. The message types catered for are: normal, verbose, debug, error,
fatal error, dump and alert messages. All messages are created and sent with
I<printf(3)>-like syntax. The destinations for these messages are
configurable by the client. Calling I<prog_init()> causes normal and verbose
messages to be sent to standard output; debug, error, fatal error, dump and
alert messages to be sent to standard error.

Calls to I<prog_set_out()>, I<prog_out_fd()>, I<prog_out_stdout()>,
I<prog_out_file()>, I<prog_out_syslog()> and I<prog_out_none()> cause normal
and verbose messages to be sent to the specified destination.

Calls to I<prog_set_err()>, I<prog_err_fd()>, I<prog_err_stderr()>,
I<prog_err_file()>, I<prog_err_syslog()> and I<prog_err_none()> cause error,
fatal error and dump messages to be sent to the specified destination.

Calls to I<prog_set_dbg()>, I<prog_dbg_fd()>, I<prog_dbg_stdout()>,
I<prog_dbg_stderr()>, I<prog_dbg_file()>, I<prog_dbg_syslog()>,
I<prog_dbg_none()> cause debug messages to be sent to the specified
destination.

Calls to I<prog_set_alert()>, I<prog_alert_fd()>, I<prog_alert_stdout()>,
I<prog_alert_stderr()>, I<prog_alert_file()>, I<prog_alert_syslog()>,
I<prog_alert_none()> cause alert messages to be sent to the specified
destination.

Calls to the generic functions I<prog_set_out()>, I<prog_set_err()>,
I<prog_set_dbg()> and I<prog_set_alert()> cause their respective message
types to be sent to the specified destination or destinations (multiplexing
messages is only possible via these functions). See I<msg(3)> for more
details.

=over 4

=cut

*/

#include "config.h"
#include "std.h"

#include "msg.h"
#include "prog.h"
#include "err.h"

#ifndef HAVE_SNPRINTF
#include "snprintf.h"
#endif

#ifndef TEST

/*

=item C<void msg(const char *fmt, ...)>

Outputs a message to the program's normal message destination. C<fmt> is a
I<printf(3)>-like format string and processes any remaining arguments in the
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
according to their verbosity. C<fmt> is a I<printf(3)>-like format string
and processes any remaining arguments in the same way as I<printf(3)>. The
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
C<level> satisfies the program's current debug level. The debug level is
broken into two components. The low byte specifies the level. The next three
bytes specify a section within which the level applies. Debug messages with
a section value whose bits overlap those of the program's current debug
section and with a level that is less than or equal to the program's current
debug level are emitted. As a convenience, if the program's current debug
section is zero, debug messages with a sufficiently small level are emitted
regardless of the message section. See I<prog_set_debug_level()> for
examples. If the program's name has been supplied using I<prog_set_name()>,
the message will be preceeded by the name, a colon and a space. The message
is also preceeded by as many spaces as the debug level. This indents debug
messages according to their debug level. C<fmt> is a I<printf(3)>-like
format string and processes any remaining arguments in the same way as
I<printf(3)>. The message is followed by a newline.

=cut

*/

static int debug_level_match(size_t level)
{
	size_t debug_level, debug_section, section;
	
	debug_level = prog_debug_level();
	debug_section = debug_level & 0xffffff00;
	debug_level &= 0x000000ff;
	section = level & 0xffffff00;
	level &= 0x000000ff;

	return (!debug_section || debug_section & section) && debug_level >= level;
}

void debugf(size_t level, const char *fmt, ...)
{
	if (debug_level_match(level))
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
	if (debug_level_match(level))
	{
		char msg[MSG_SIZE], prefix[32] = "";
		vsnprintf(msg, MSG_SIZE, fmt, args);

		if (level & 0xffffff00)
			snprintf(prefix, 32, " [%d]", (level & 0xffffff00) >> 8);

		if (prog_name())
			msg_out(prog_dbg(), "%s: debug:%s%*s%s\n", prog_name(), prefix, level & 0xff, "", msg);
		else
			msg_out(prog_dbg(), "debug:%s%*s%s\n", prefix, level & 0xff, "", msg);
	}
}

/*

=item C<int error(const char *fmt, ...)>

Outputs an error message to the program's error message destination. If the
program's name has been supplied using I<prog_set_name()>, the message will
be preceeded by the name, a colon and a space. C<fmt> is a I<printf(3)>-like
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
calls I<exit(3)> with a return code of C<EXIT_FAILURE>. If the program's
name was supplied using I<prog_set_name()>, the message will be preceeded by
the name, a colon and a space. This is followed by the string C<"fatal: ">.
C<fmt> is a I<printf(3)>-like format string and processes any remaining
arguments in the same way as I<printf(3)>. The message is followed by a
newline. B<Note:> Never use this in a library. Only an application can
decide which errors are fatal.

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
	exit(EXIT_FAILURE);
}

/*

=item C<void dump(const char *fmt, ...)>

Outputs an error message to the program's error message destination and then
calls I<abort(3)>. If the program's name was supplied using
I<prog_set_name()>, the message will be preceeded by the name, a colon and a
space. This is followed by the string C<"dump: ">. C<fmt> is a
I<printf(3)>-like format string and processes any remaining arguments in the
same way as I<printf(3)>. The message is followed by a newline. B<Note:>
Never use this in a library. Only an application can decide which errors are
fatal.

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

=item C<void alert(int priority, const char *fmt, ...)>

Outputs an alert message of the given C<priority> to the program's alert
message destination. If the program's name has been supplied using
I<prog_set_name()>, the message will be preceeded by the name, a colon and a
space. C<fmt> is a I<printf(3)>-like format string and processes any
remaining arguments in the same way as I<printf(3)>. The message is followed
by a newline. Note that this only works when the program's alert message
destination is a simple syslog destination. If the alert message destination
is anything else (including a multiplexing message destination containing
syslog destinations), C<priority> is ignored.

=cut

*/

void alert(int priority, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	valert(priority, fmt, args);
	va_end(args);
}

/*

=item C<void valert(int priority, const char *fmt, va_list args)>

Equivalent to I<alert()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void valert(int priority, const char *fmt, va_list args)
{
	Msg *alert;
	char msg[MSG_SIZE];
	int err;

	vsnprintf(msg, MSG_SIZE, fmt, args);

	alert = prog_alert();

	if ((err = msg_wrlock(alert)))
	{
		set_errno(err);
		return;
	}

	msg_syslog_set_priority_unlocked(alert, priority);

	if (prog_name())
		msg_out_unlocked(alert, "%s: %s\n", prog_name(), msg);
	else
		msg_out_unlocked(alert, "%s\n", msg);

	if ((err = msg_unlock(alert)))
		set_errno(err);
}

/*

=item C<void debugsysf(size_t level, const char *fmt, ...)>

Equivalent to I<debugf()> except that the message is followed by a colon, a
space, the string representation of C<errno> and a newline (rather than just
a newline).

=cut

*/

void debugsysf(size_t level, const char *fmt, ...)
{
	if (debug_level_match(level))
	{
		va_list args;
		va_start(args, fmt);
		vdebugsysf(level, fmt, args);
		va_end(args);
	}
}

/*

=item C<void vdebugsysf(size_t level, const char *fmt, va_list args)>

Equivalent to I<debugsysf()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void vdebugsysf(size_t level, const char *fmt, va_list args)
{
	if (debug_level_match(level))
	{
		char msg[MSG_SIZE];
		int errno_saved = errno;
		vsnprintf(msg, MSG_SIZE, fmt, args);
		debugf(level, "%s: %s", msg, strerror(errno_saved));
	}
}

/*

=item C<int errorsys(const char *fmt, ...)>

Equivalent to I<error()> except that the message is followed by a colon, a
space, the string representation of C<errno> and a newline (rather than just
a newline).

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

Equivalent to I<fatal()> except that the message is followed by a colon, a
space, the string representation of C<errno> and a newline (rather than just
a newline).

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
	int errno_saved = errno;
	vsnprintf(msg, MSG_SIZE, fmt, args);
	fatal("%s: %s", msg, strerror(errno_saved));
}

/*

=item C<void dumpsys(const char *fmt, ...)>

Equivalent to I<dump()> except that the message is followed by a colon, a
space, the string representation of C<errno> and a newline (rather than just
a newline).

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
	int errno_saved = errno;
	vsnprintf(msg, MSG_SIZE, fmt, args);
	dump("%s: %s", msg, strerror(errno_saved));
}

/*

=item C<void alertsys(int priority, const char *fmt, ...)>

Equivalent to I<alert()> except that the message is followed by a colon, a
space, the string representation of C<errno> and a newline (rather than just
a newline).

=cut

*/

void alertsys(int priority, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	valertsys(priority, fmt, args);
	va_end(args);
}

/*

=item C<void valertsys(int priority, const char *fmt, va_list args)>

Equivalent to I<alertsys()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

void valertsys(int priority, const char *fmt, va_list args)
{
	char msg[MSG_SIZE];
	int errno_saved = errno;
	vsnprintf(msg, MSG_SIZE, fmt, args);
	alert(priority, "%s: %s", msg, strerror(errno_saved));
}

/*

=item C<int set_errno(int errnum)>

Sets C<errno> to C<errnum> and returns -1.

=cut

*/

int (set_errno)(int errnum)
{
	errno = errnum;
	return -1;
}

/*

=item C<void *set_errnull(int errnum)>

Sets C<errno> to C<errnum> and returns C<null>.

=cut

*/

void *(set_errnull)(int errnum)
{
	errno = errnum;
	return NULL;
}

/*

=item C< #define debug(args)>

Calls I<debugf()> unless C<NDEBUG> is defined. C<args> must be supplied with
extra parentheses. e.g.

    debug((1, "rc=%d", rc))

=item C< #define vdebug(args)>

Calls I<vdebugf()> unless C<NDEBUG> is defined. C<args> must be supplied
with extra parentheses. e.g.

    vdebug((1, fmt, args))

=item C< #define debugsys(args)>

Calls I<debugsysf()> unless C<NDEBUG> is defined. C<args> must be supplied
with extra parentheses. e.g.

    debugsys((1, "fd=%d", fd))

=item C< #define vdebugsys(args)>

Calls I<vdebugsysf()> unless C<NDEBUG> is defined. C<args> must be supplied
with extra parentheses. e.g.

    vdebugsys((1, fmt, args))

=item C< #define check(cond, msg)>

Like I<assert(3)> but includes a string argument, C<msg>, for including an
explanation of the condition tested and then calls I<dump()> to terminate
the program. This means the message will be sent to the right place(s)
rather than just to C<stderr>. B<Note:> Like I<assert(3)>, this function is
largely useless. It should never be left in production code (because it's
rude) so you need to write code to handle error conditions properly anyway.
You might as well not bother using I<assert()> or I<check()> in the first
place.

=back

=head1 MT-Level

MT-Safe

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<msg(3)|msg(3)>,
L<prog(3)|prog(3)>,
L<printf(3)|printf(3)>

=head1 AUTHOR

20011109 raf <raf@raf.org>

=cut

*/

#endif

#ifdef TEST

#include <fcntl.h>

#include <sys/wait.h>
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

	memset(buf, 0, BUFSIZ);
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
	const char * const alertfile = "err.alert";
	const char * const core = "core";
	const char * const core2 = "err.core"; /* OpenBSD */
	char buf[BUFSIZ];
	int rci;
	void *rcp;

	const char *results[12] =
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
		"dump: dumpsys: %s\n",
		"debug: [1] lexer debug\n"
		"debug: [2] parser debug\n"
		"debug: [1] lexer debug\n"
		"debug: [2] parser debug\n"
		"debug: [4] interp debug\n"
		"debug: global debug\n",
		"alert\n"
		"alertsys: " /* followed by "Success" or "Error 0" */
	};

	pid_t pid;
	int errors = 0;

	if (ac == 2 && !strcmp(av[1], "help"))
	{
		printf("usage: %s\n", *av);
		return EXIT_SUCCESS;
	}

	printf("Testing: err\n");

	/* Test debug, verbose and error */

	prog_set_debug_level(1);
	prog_set_verbosity_level(1);

	prog_out_file(out);
	msg("msg\n");
	errors += verify(1, out, results[0]);

	prog_out_file(out);
	verbose(1, "verbose");
	errors += verify(2, out, results[1]);

	prog_dbg_file(dbg);
	debugf(1, "debug");
	errors += verify(3, dbg, results[2]);

	prog_err_file(err);
	error("error");
	errors += verify(4, err, results[3]);

	/* Test fatal */

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

			if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_FAILURE)
				++errors, printf("Test5: failed: %s %d\n",
					WIFSIGNALED(status) ? "received signal" : "exit code",
					WIFSIGNALED(status) ? WTERMSIG(status) : WEXITSTATUS(status));
		}
	}

	errors += verify(5, err, results[4]);

	/* Test dump */

	switch (pid = fork())
	{
		case 0:
		{
			prog_err_file(err);
			unlink(core);
			unlink(core2);
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

			if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGABRT)
				++errors, printf("Test5: failed: %s %d\n",
					WIFSIGNALED(status) ? "received signal" : "exit code",
					WIFSIGNALED(status) ? WTERMSIG(status) : WEXITSTATUS(status));

			if (stat(core, statbuf) == -1 && errno == ENOENT &&
				stat(core2, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test6: failed: no core file produced (ulimit?)\n");
			else
			{
				unlink(core);
				unlink(core2);
			}
		}
	}

	errors += verify(6, err, results[5]);

	/* Test debugsys, errorsys */

	prog_dbg_file(dbg);
	set_errno(EPERM);
	debugsysf(1, "debugsys");
	errors += verifysys(7, dbg, results[6], EPERM);

	prog_err_file(err);
	set_errno(ENOENT);
	errorsys("errorsys");
	errors += verifysys(8, err, results[7], ENOENT);

	/* Test fatalsys */

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

			if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_FAILURE)
				++errors, printf("Test5: failed: %s %d\n",
					WIFSIGNALED(status) ? "received signal" : "exit code",
					WIFSIGNALED(status) ? WTERMSIG(status) : WEXITSTATUS(status));
		}
	}

	errors += verifysys(9, err, results[8], EPERM);

	/* Test dumpsys */

	switch (pid = fork())
	{
		case 0:
		{
			prog_err_file(err);
			unlink(core);
			unlink(core2);
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

			if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGABRT)
				++errors, printf("Test5: failed: %s %d\n",
					WIFSIGNALED(status) ? "received signal" : "exit code",
					WIFSIGNALED(status) ? WTERMSIG(status) : WEXITSTATUS(status));

			if (stat(core, statbuf) == -1 && errno == ENOENT &&
				stat(core2, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test10: failed: no core file produced (ulimit?)\n");
			else
			{
				unlink(core);
				unlink(core2);
			}
		}
	}

	errors += verifysys(10, err, results[9], ENOENT);

	/* Test check true */

	switch (pid = fork())
	{
		case 0:
		{
			int i = 1;
			prog_err_file(err);
			unlink(core);
			unlink(core2);
			set_errno(ENOENT);
			check(i == 1, "check");
			_exit(EXIT_SUCCESS);
		}

		case -1:
		{
			++errors;
			printf("Test11: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status;

			if (waitpid(pid, &status, 0) == -1)
			{
				++errors;
				printf("Test11: failed to wait for test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
				++errors, printf("Test5: failed: %s %d\n",
					WIFSIGNALED(status) ? "received signal" : "exit code",
					WIFSIGNALED(status) ? WTERMSIG(status) : WEXITSTATUS(status));
		}
	}

	/* Test check false */

	switch (pid = fork())
	{
		case 0:
		{
			int i = 1;
			prog_err_file(err);
			unlink(core);
			unlink(core2);
			set_errno(ENOENT);
			check(i == 0, "check");
			_exit(EXIT_SUCCESS);
		}

		case -1:
		{
			++errors;
			printf("Test12: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			struct stat statbuf[1];
			int status;

			if (waitpid(pid, &status, 0) == -1)
			{
				++errors;
				printf("Test12: failed to wait for test - waitpid(%d) failed (%s)\n", (int)pid, strerror(errno));
				break;
			}

			if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGABRT)
				++errors, printf("Test5: failed: %s %d\n",
					WIFSIGNALED(status) ? "received signal" : "exit code",
					WIFSIGNALED(status) ? WTERMSIG(status) : WEXITSTATUS(status));

			if (stat(core, statbuf) == -1 && errno == ENOENT &&
				stat(core2, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test12: failed: no core file produced (ulimit?)\n");
			else
			{
				unlink(core);
				unlink(core2);
			}
		}
	}

	snprintf(buf, BUFSIZ, "dump: Internal Error: %s: %s [", "i == 0", "check");
	errors += verify(12, err, buf);

	/* Test debug sections */

#define LEXER_SECTION  (1 << 8)
#define PARSER_SECTION (2 << 8)
#define INTERP_SECTION (4 << 8)

    prog_set_debug_level(LEXER_SECTION | PARSER_SECTION | 1);
	prog_dbg_file(dbg);
	msg_set_timestamp_format("");

    debugf(LEXER_SECTION  | 1, "lexer debug");  /* yes */
    debugf(LEXER_SECTION  | 4, "lexer debug");  /* no (level too high) */
    debugf(PARSER_SECTION | 1, "parser debug"); /* yes */
    debugf(INTERP_SECTION | 1, "interp debug"); /* no (wrong section) */
    debugf(1, "global debug");                  /* no (no section to match) */

    prog_set_debug_level(1);
    debugf(LEXER_SECTION  | 1, "lexer debug");  /* yes */
    debugf(LEXER_SECTION  | 4, "lexer debug");  /* no (level too high) */
    debugf(PARSER_SECTION | 1, "parser debug"); /* yes */
    debugf(INTERP_SECTION | 1, "interp debug"); /* yes */
    debugf(1, "global debug");                  /* yes */
    debugf(4, "global debug");                  /* no (level too high) */

	errors += verify(13, dbg, results[10]);

#undef LEXER_SECTION
#undef PARSER_SECTION
#undef INTERP_SECTION

	/* Test alert() and alertsys() */

	prog_alert_file(alertfile);
	alert(LOG_INFO, "alert");
	errno = 0;
	alertsys(LOG_INFO, "alertsys");
	errors += verify(14, alertfile, results[11]);

	prog_out_none();
	prog_err_none();
	prog_dbg_none();
	prog_alert_none();

	/* Test set_errno() and set_errnull() */

	errno = 0;
	if ((rci = set_errno(EINVAL)) != -1)
		++errors, printf("Test15: set_errno(EINVAL) failed (returned %d, not %d)\n", rci, -1);
	else if (errno != EINVAL)
		++errors, printf("Test15: set_errno(EINVAL) failed (errno = %s, not %s)\n", strerror(errno), strerror(EINVAL));

	errno = 0;
	if ((rcp = set_errnull(EINVAL)) != NULL)
		++errors, printf("Test16: set_errnull(EINVAL) failed (returned %p, not %p)\n", rcp, NULL);
	else if (errno != EINVAL)
		++errors, printf("Test16: set_errnull(EINVAL) failed (errno = %s, not %s)\n", strerror(errno), strerror(EINVAL));

	if (errors)
		printf("%d/16 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return (errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif

/* vi:set ts=4 sw=4: */
