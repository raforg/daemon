/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "msg.h"
#include "prog.h"
#include "err.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

/*
** void msg(const char *fmt, ...)
**
** Outputs a message. fmt is a printf-like format string and processes any
** remaining arguments in the same way as printf.
*/

void msg(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vmsg(fmt, args);
	va_end(args);
}

/*
** void vmsg(const char *fmt, va_list args)
**
** Outputs a message. fmt is a printf-like format string and processes
** args in the same way as vprintf.
*/

void vmsg(const char *fmt, va_list args)
{
	vmsg_out(prog_out(), fmt, args);
}

/*
** void debug(size_t level, const char *fmt, ...)
**
** Outputs a debug message if level is less than or equal to the program's
** current debug level. fmt is a printf-like format string and processes any
** remaining arguments in the same way as printf.
*/

void debug(size_t level, const char *fmt, ...)
{
	if (prog_debug_level() >= level)
	{
		va_list args;
		va_start(args, fmt);
		vdebug(level, fmt, args);
		va_end(args);
	}
}

/*
** void vdebug(size_t level, const char *fmt, va_list args)
**
** Outputs a debug message if level is less than or equal to the program's
** current debug level. fmt is a printf-like format string and processes
** args in the same way as vprintf.
*/

void vdebug(size_t level, const char *fmt, va_list args)
{
	if (prog_debug_level() >= level)
	{
		char msg[BUFSIZ];
		vsnprintf(msg, BUFSIZ, fmt, args);

		if (prog_name())
			msg_out(prog_dbg(), "%s: debug: %*s%s\n", prog_name(), level, "", msg);
		else
			msg_out(prog_dbg(), "debug: %*s%s\n", level, "", msg);
	}
}

/*
** int error(const char *fmt, ...)
**
** Outputs an error message. If the program's name has been supplied using
** prog_set_name(), the error message will be preceeded by the name, a colon
** and a space. fmt is a printf-like format string and processes any remaining
** arguments in the same way as printf. Returns -1.
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
** int verror(const char *fmt, va_list args)
**
** Outputs an error message. If the program's name has been supplied using
** prog_set_name(), the error message will be preceeded by the name, a colon
** and a space. fmt is a printf-like format string and processes argts in the
** same way as vprintf. Returns -1.
*/

int verror(const char *fmt, va_list args)
{
	char msg[BUFSIZ];
	vsnprintf(msg, BUFSIZ, fmt, args);

	if (prog_name())
		msg_out(prog_err(), "%s: %s\n", prog_name(), msg);
	else
		msg_out(prog_err(), "%s\n", msg);

	return -1;
}

/*
** void fatal(const char *fmt, ...)
**
** Outputs an error message and exits with a return code of 1. The error
** message, will contain the string "fatal: ". fmt is a printf-like format
** string and processes any remaining arguments in the same way as printf.
*/

void fatal(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfatal(fmt, args);
	va_end(args); // unreached
}

/*
** void vfatal(const char *fmt, va_list args)
**
** Outputs an error message and exits with a return code of 1. The error
** message, will contain the string "fatal: ". fmt is a printf-like format
** string and processes args in the same way as vprintf.
*/

void vfatal(const char *fmt, va_list args)
{
	char msg[BUFSIZ];
	vsnprintf(msg, BUFSIZ, fmt, args);
	error("fatal: %s", msg);
	exit(1);
}

/*
** void dump(const char *fmt, ...)
**
** Outputs an error message and then abort()s the program. The error message,
** will contain the string "dump: ". fmt is a printf-like format string and
** processes any remaining arguments in the same way as printf.
*/

void dump(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vdump(fmt, args);
	va_end(args); // unreached
}

/*
** void vdump(const char *fmt, va_list args)
**
** Outputs an error message and then abort()s the program. The error message,
** will contain the string "dump: ". fmt is a printf-like format string and
** processes args in the same way as vprintf.
*/

void vdump(const char *fmt, va_list args)
{
	char msg[BUFSIZ];
	vsnprintf(msg, BUFSIZ, fmt, args);
	error("dump: %s", msg);
	abort();
}

/*
** void debugsys(size_t level, const char *fmt, ...)
**
** Outputs a debug message if level is less than or equal to the program's
** current debug level. fmt is a printf-like format string and processes any
** remaining arguments in the same way as printf. After the message is a colon,
** a space and the string version of the current value of errno.
*/

void debugsys(size_t level, const char *fmt, ...)
{
	if (prog_debug_level() >= level)
	{
		va_list args;
		va_start(args, fmt);
		vdebugsys(level, fmt, args);
		va_end(args);
	}
}

/*
** void vdebugsys(size_t level, const char *fmt, value args)
**
** Outputs a debug message if level is less than or equal to the program's
** current debug level. fmt is a printf-like format string and processes args
** in the same way as vprintf. After the message is a colon, a space and the
** string version of the current value of errno.
*/

void vdebugsys(size_t level, const char *fmt, va_list args)
{
	if (prog_debug_level() >= level)
	{
		char msg[BUFSIZ];
		vsnprintf(msg, BUFSIZ, fmt, args);
		debug(level, "%s: %s", msg, strerror(errno));
	}
}

/*
** int errorsys(const char *fmt, ...)
**
** Outputs an error message. If the program's name has been supplied using
** prog_set_name(), the error message will be preceeded by the name, a colon
** and a space. fmt is a printf-like format string and processes any remaining
** arguments in the same way as printf. After the message is a colon, a space
** and the string version of the current value of errno. Returns -1.
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
** int verrorsys(const char *fmt, va_list args)
**
** Outputs an error message. If the program's name has been supplied using
** prog_set_name(), the error message will be preceeded by the name, a colon
** and a space. fmt is a printf-like format string and processes args in the
** same way as vprintf. After the message is a colon, a space and the string
** version of the current value of errno. Returns -1.
*/

int verrorsys(const char *fmt, va_list args)
{
	char msg[BUFSIZ];
	vsnprintf(msg, BUFSIZ, fmt, args);
	return error("%s: %s", msg, strerror(errno));
}

/*
** void fatalsys(const char *fmt, ...)
**
** Outputs an error message and exits with a return code of 1. The error
** message, will contain the string "fatal: ". fmt is a printf-like format
** string and processes any remaining arguments in the same way as printf.
** After the message is a colon, a space and the string version of the current
** value of errno.
*/

void fatalsys(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfatalsys(fmt, args);
	va_end(args); // unreached
}

/*
** void vfatalsys(const char *fmt, va_list args)
**
** Outputs an error message and exits with a return code of 1. The error
** message, will contain the string "fatal: ". fmt is a printf-like format
** string and processes args in the same way as vprintf. After the message
** is a colon, a space and the string version of the current value of errno.
*/

void vfatalsys(const char *fmt, va_list args)
{
	char msg[BUFSIZ];
	vsnprintf(msg, BUFSIZ, fmt, args);
	fatal("%s: %s", msg, strerror(errno));
}

/*
** void dumpsys(const char *fmt, ...)
**
** Outputs an error message and then abort()s the program. The error message,
** will contain the string "dump: ". fmt is a printf-like format string and
** processes any remaining arguments in the same way as printf. After the
** message is a colon, a space and the string version of the current value of
** errno.
*/

void dumpsys(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vdumpsys(fmt, args);
	va_end(args); // unreached
}

/*
** void vdumpsys(const char *fmt, va_list args)
**
** Outputs an error message and then abort()s the program. The error message,
** will contain the string "dump: ". fmt is a printf-like format string and
** processes args in the same way as vprintf. After the message is a colon,
** a space and the string version of the current value of errno.
*/

void vdumpsys(const char *fmt, va_list args)
{
	char msg[BUFSIZ];
	vsnprintf(msg, BUFSIZ, fmt, args);
	dump("%s: %s", msg, strerror(errno));
}

/*
** int set_errno(int errnum)
**
** Sets errno to errnum and returns -1.
*/

int set_errno(int errnum)
{
	errno = errnum;
	return -1;
}

#ifdef TEST

#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

static int verify(int test, const char *name, const char *result)
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

	if (strcmp(buf, result))
	{
		printf("Test%d: err file produced incorrect input:\nshould be:\n%s\nwas:\n%s\n", test, result, buf);
		return 1;
	}

	return 0;
}

static int verifysys(int test, const char *name, const char *result, int err)
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

	const char *results[9] =
	{
		"msg\n",
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

	printf("Testing: %s\n", *av);

	prog_set_debug_level(1);

	prog_out_file(out);
	msg("msg\n");
	errors += verify(1, out, results[0]);

	prog_dbg_file(dbg);
	debug(0, "debug");
	errors += verify(2, dbg, results[1]);

	prog_err_file(err);
	error("error");
	errors += verify(3, err, results[2]);

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
			printf("Test4: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status[1];

			if (waitpid(pid, status, 0) == -1)
			{
				++errors, printf("Test4: failed to wait for test - waitpid(%d) failed (%s)\n", pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(*status) && WTERMSIG(*status) != SIGABRT)
				++errors, printf("Test4: failed: received signal %d\n", WTERMSIG(*status));

			if (WIFEXITED(*status) && WEXITSTATUS(*status) != 1)
				++errors, printf("Test4: failed: exit status %d\n", WEXITSTATUS(*status));
		}
	}

	errors += verify(4, err, results[3]);

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
			printf("Test5: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			struct stat statbuf[1];
			int status[1];

			if (waitpid(pid, status, 0) == -1)
			{
				++errors, printf("Test5: failed to wait for test - waitpid(%d) failed (%s)\n", pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(*status) && WTERMSIG(*status) != SIGABRT)
				++errors, printf("Test5: failed: received signal %d\n", WTERMSIG(*status));

			if (WIFEXITED(*status) && WEXITSTATUS(*status) != 1)
				++errors, printf("Test5: failed: exit status %d\n", WEXITSTATUS(*status));

			if (stat(core, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test5: failed: no core file produced\n");
			else
				unlink(core);
		}
	}

	errors += verify(5, err, results[4]);

	prog_dbg_file(dbg);
	set_errno(EPERM);
	debugsys(0, "debugsys");
	errors += verifysys(6, dbg, results[5], EPERM);

	prog_err_file(err);
	set_errno(ENOENT);
	errorsys("errorsys");
	errors += verifysys(7, err, results[6], ENOENT);

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
			printf("Test8: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			int status[1];

			if (waitpid(pid, status, 0) == -1)
			{
				++errors;
				printf("Test8: failed to wait for test - waitpid(%d) failed (%s)\n", pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(*status) && WTERMSIG(*status) != SIGABRT)
				++errors, printf("Test8: failed: received signal %d\n", WTERMSIG(*status));

			if (WIFEXITED(*status) && WEXITSTATUS(*status) != 1)
				++errors, printf("Test8: failed: exit status %d\n", WEXITSTATUS(*status));
		}
	}

	errors += verifysys(8, err, results[7], EPERM);

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
			printf("Test9: failed to perform test - fork() failed (%s)\n", strerror(errno));
			break;
		}

		default:
		{
			struct stat statbuf[1];
			int status[1];

			if (waitpid(pid, status, 0) == -1)
			{
				++errors;
				printf("Test9: failed to wait for test - waitpid(%d) failed (%s)\n", pid, strerror(errno));
				break;
			}

			if (WIFSIGNALED(*status) && WTERMSIG(*status) != SIGABRT)
				++errors, printf("Test9: failed: received signal %d\n", WTERMSIG(*status));

			if (WIFEXITED(*status) && WEXITSTATUS(*status) != 1)
				++errors, printf("Test9: failed: exit status %d\n", WEXITSTATUS(*status));

			if (stat(core, statbuf) == -1 && errno == ENOENT)
				++errors, printf("Test9: failed: no core file produced\n");
			else
				unlink(core);
		}
	}

	errors += verifysys(9, err, results[8], ENOENT);

	if (errors)
		printf("%d/9 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
