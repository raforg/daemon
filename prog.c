/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>

#include "msg.h"
#include "err.h"
#include "prog.h"
#include "opt.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

typedef struct Prog Prog;

struct Prog
{
	const char *name;
	Options *options;
	const char *syntax;
	const char *desc;
	const char *version;
	const char *date;
	const char *author;
	const char *contact;
	const char *vendor;
	const char *url;
	const char *legal;
	Msg *out;
	Msg *err;
	Msg *dbg;
	size_t level;
};

static Prog g =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, 0
};

/*
** void prog_init(void)
**
** Initialises the message, error and debug destinations.
*/

void prog_init(void)
{
	prog_out_stdout();
	prog_err_stderr();
	prog_dbg_stderr();
}

/*
** const char *prog_set_name(const char *name)
**
** Sets the program's name to the string name.
*/

const char *prog_set_name(const char *name)
{
	return g.name = name;
}

/*
** Options *prog_set_options(Options *options)
**
** Sets the program's options to usage.
*/

Options *prog_set_options(Options *options)
{
	return g.options = options;
}

/*
** const char *prog_set_syntax(const char *syntax)
**
** Sets the program's syntax description.
*/

const char *prog_set_syntax(const char *syntax)
{
	return g.syntax = syntax;
}

/*
** const char *prog_set_desc(const char *desc)
**
** Sets the program's description to the string desc.
*/

const char *prog_set_desc(const char *desc)
{
	return g.desc = desc;
}

/*
** const char *prog_set_version(const char *version)
**
** Sets the program's version to the string version.
*/

const char *prog_set_version(const char *version)
{
	return g.version = version;
}

/*
** const char *prog_set_date(const char *date)
**
** Sets the program's release date to the string date.
*/

const char *prog_set_date(const char *date)
{
	return g.date = date;
}

/*
** const char *prog_set_author(const char *author)
**
** Sets the program's author to the string author.
*/

const char *prog_set_author(const char *author)
{
	return g.author = author;
}

/*
** const char *prog_set_contact(const char *contact)
**
** Sets the program's contact address to the string contact.
*/

const char *prog_set_contact(const char *contact)
{
	return g.contact = contact;
}

/*
** const char *prog_set_vendor(const char *vendor)
**
** Sets the program's vendor to the string vendor.
*/

const char *prog_set_vendor(const char *vendor)
{
	return g.vendor = vendor;
}

/*
** const char *prog_set_url(const char *url)
**
** Sets the program's URL to the string url.
*/

const char *prog_set_url(const char *url)
{
	return g.url = url;
}

/*
** const char *prog_set_legal(const char *legal)
**
** Sets the program's legal notice to the string legal.
*/

const char *prog_set_legal(const char *legal)
{
	return g.legal = legal;
}

/*
** Msg *prog_set_out(Msg *output)
**
** Sets the program's message destination to out.
*/

Msg *prog_set_out(Msg *out)
{
	if (g.out && g.out != out)
		msg_release(g.out);

	return g.out = out;
}

/*
** Msg *prog_set_err(Msg *err)
**
** Sets the program's error message destination to msg.
*/

Msg *prog_set_err(Msg *err)
{
	if (g.err && g.err != err)
		msg_release(g.err);

	return g.err = err;
}

/*
** Msg *prog_set_dbg(Msg *dbg)
**
** Sets the program's debug message destination to dbg.
*/

Msg *prog_set_dbg(Msg *dbg)
{
	if (g.dbg && g.dbg != dbg)
		msg_release(g.dbg);

	return g.dbg = dbg;
}

/*
** size_t prog_set_debug_level(size_t level)
**
** Sets the program's debug level to level. Returns the previous debug level.
*/

size_t prog_set_debug_level(size_t level)
{
	size_t prev = g.level;
	g.level = level;
	return prev;
}

/*
** const char *prog_name(void)
**
** Returns the program's name.
*/

const char *prog_name(void)
{
	return g.name;
}

/*
** const Options *prog_options(void)
**
** Returns the program's options.
*/

const Options *prog_options(void)
{
	return g.options;
}

/*
** const char *prog_syntax(void)
**
** Returns the program's syntax description.
*/

const char *prog_syntax(void)
{
	return g.syntax;
}

/*
** const char *prog_desc(void)
**
** Returns the program's description.
*/

const char *prog_desc(void)
{
	return g.desc;
}

/*
** const char *prog_version(void)
**
** Returns the program's version string.
*/

const char *prog_version(void)
{
	return g.version;
}

/*
** const char *prog_date(void)
**
** Returns the program's release date.
*/

const char *prog_date(void)
{
	return g.date;
}

/*
** const char *prog_author(void)
**
** Returns the program's author.
*/

const char *prog_author(void)
{
	return g.author;
}

/*
** const char *prog_contact(void)
**
** Returns the program's contact address.
*/

const char *prog_contact(void)
{
	return g.contact;
}

/*
** const char *prog_vendor(void)
**
** Returns the program's vendor.
*/

const char *prog_vendor(void)
{
	return g.vendor;
}

/*
** const char *prog_url(void)
**
** Returns the program's URl.
*/

const char *prog_url(void)
{
	return g.url;
}

/*
** const char *prog_legal(void)
**
** Returns the program's legal notice.
*/

const char *prog_legal(void)
{
	return g.legal;
}

/*
** Msg *prog_out(void)
**
** Returns the program's message destination.
*/

Msg *prog_out(void)
{
	return g.out;
}

/*
** Msg *prog_err(void)
**
** Returns the program's error message destination.
*/

Msg *prog_err(void)
{
	return g.err;
}

/*
** Msg *prog_dbg(void)
**
** Returns the program's debug message destination.
*/

Msg *prog_dbg(void)
{
	return g.dbg;
}

/*
** size_t prog_debug_level(void)
**
** Returns the program's debug level.
*/

size_t prog_debug_level(void)
{
	return g.level;
}

/*
** int prog_out_fd(int fd)
**
** Sets the program's message destination to be file descriptor
** specified by fd. Returns 0 on success, -1 on error.
*/

int prog_out_fd(int fd)
{
	Msg *msg;

	if (!(msg = msg_create_fd(fd)))
		return -1;

	prog_set_out(msg);

	return 0;
}

/*
** int prog_out_stdout(void)
**
** Sets the program's message destination to be standard output.
** Returns 0 on success, -1 on error.
*/

int prog_out_stdout(void)
{
	return prog_out_fd(STDOUT_FILENO);
}

/*
** int prog_out_file(const char *path)
**
** Sets the program's message destination to be the file
** specified by path. Returns 0 on success, -1 on error.
*/

int prog_out_file(const char *path)
{
	Msg *msg;

	if (!(msg = msg_create_file(path)))
		return -1;

	prog_set_out(msg);

	return 0;
}

/*
** int prog_out_syslog(const char *ident, int option, int facility)
**
** Sets the program's message destination to be the system logger
** initialised with ident, option and facility.
** Returns 0 on success, -1 on error.
*/

int prog_out_syslog(const char *ident, int option, int facility)
{
	Msg *msg;

	if (!(msg = msg_create_syslog(ident, option, facility)))
		return -1;

	prog_set_out(msg);

	return 0;
}

/*
** int prog_out_none(void)
**
** Sets the program's message destination to be NULL. This disables
** all error messages. Returns 0.
*/

int prog_out_none(void)
{
	prog_set_out(NULL);

	return 0;
}

/*
** int prog_err_fd(int fd)
**
** Sets the program's error message destination to be file descriptor
** specified by fd. Returns 0 on success, -1 on error.
*/

int prog_err_fd(int fd)
{
	Msg *msg;

	if (!(msg = msg_create_fd(fd)))
		return -1;

	prog_set_err(msg);

	return 0;
}

/*
** int prog_err_stderr(void)
**
** Sets the program's error message destination to be standard error.
** Returns 0 on success, -1 on error.
*/

int prog_err_stderr(void)
{
	return prog_err_fd(STDERR_FILENO);
}

/*
** int prog_err_file(const char *path)
**
** Sets the program's error message destination to be the file
** specified by path. Returns 0 on success, -1 on error.
*/

int prog_err_file(const char *path)
{
	Msg *msg;

	if (!(msg = msg_create_file(path)))
		return -1;

	prog_set_err(msg);

	return 0;
}

/*
** int prog_err_syslog(const char *ident, int option, int facility)
**
** Sets the program's error message destination to be the system logger
** initialised with ident, option and facility.
** Returns 0 on success, -1 on error.
*/

int prog_err_syslog(const char *ident, int option, int facility)
{
	Msg *msg;

	if (!(msg = msg_create_syslog(ident, option, facility)))
		return -1;

	prog_set_err(msg);

	return 0;
}

/*
** int prog_err_none(void)
**
** Sets the program's error message destination to be NULL. This disables
** all error messages. Returns 0.
*/

int prog_err_none(void)
{
	prog_set_err(NULL);

	return 0;
}

/*
** int prog_dbg_fd(int fd)
**
** Sets the program's debug message destination to be file descriptor
** specified by fd. Returns 0 on success, -1 on error.
*/

int prog_dbg_fd(int fd)
{
	Msg *msg;

	if (!(msg = msg_create_fd(fd)))
		return -1;

	prog_set_dbg(msg);

	return 0;
}

/*
** int prog_dbg_stdout(void)
**
** Sets the program's debug message destination to be standard output.
** Returns 0 on success, -1 on error.
*/

int prog_dbg_stdout(void)
{
	return prog_dbg_fd(STDOUT_FILENO);
}

/*
** int prog_dbg_stderr(void)
**
** Sets the program's debug message destination to be standard error.
** Returns 0 on success, -1 on error.
*/

int prog_dbg_stderr(void)
{
	return prog_dbg_fd(STDERR_FILENO);
}

/*
** int prog_dbg_file(const char *path)
**
** Sets the program's debug message destination to be the file
** specified by path.
*/

int prog_dbg_file(const char *path)
{
	Msg *dbg;

	if (!(dbg = msg_create_file(path)))
		return -1;

	prog_set_dbg(dbg);

	return 0;
}

/*
** int prog_dbg_syslog(const char *id, int option, int facility)
**
** Sets the program's debug message destination to be the system logger
** initialised with ident, option and facility.
*/

int prog_dbg_syslog(const char *id, int option, int facility)
{
	Msg *dbg;

	if (!(dbg = msg_create_syslog(id, option, facility)))
		return -1;

	prog_set_dbg(dbg);

	return 0;
}

/*
** int prog_dbg_none(void)
**
** Sets the program's debug message destination to be NULL. This disables
** all debug messages. Returns 0.
*/

int prog_dbg_none(void)
{
	prog_set_dbg(NULL);

	return 0;
}

/*
** int prog_opt_process(int ac, const char **av)
**
** Parses the command line arguments in argv to process options.
*/

int prog_opt_process(int ac, char **av)
{
	int rc = opt_process(ac, av, g.options);
	if (rc == -1)
		prog_usage_msg("");

	return rc;
}

/*
** void prog_usage_msg(const char *fmt, ...)
**
** Emits a program usage error message. fmt is a printf-like format string.
** Any remaining arguments are processed as in printf(2).
*/

void prog_usage_msg(const char *fmt, ...)
{
	char msg_buf[BUFSIZ];
	char opt_buf[BUFSIZ];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg_buf, BUFSIZ, fmt, args);
	va_end(args);
	opt_usage(opt_buf, BUFSIZ, g.options);

	fflush(stderr); /* I thought stderr wasn't supposed to be buffered? */

	msg_out(g.err, "%s%susage: %s %s\n%s",
		msg_buf,
		strlen(msg_buf) ? "\n" : "",
		(g.name) ? g.name : "",
		(g.syntax) ? g.syntax : "",
		opt_buf
	);

	exit(1);
}

/*
** void prog_help_msg(void)
**
** Emits a program help message. This message consists of the program's usage
** message, description, version, release date, author, URL and legal notice.
*/

static void prog_help_msg(void)
{
#define safe(str) ((str) ? (str) : "")
	char buf[BUFSIZ];
	size_t length = 0;

	snprintf(buf, BUFSIZ, "usage: %s %s\n", safe(g.name), safe(g.syntax));

	if (g.options)
	{
		length = strlen(buf);
		opt_usage(buf + length, BUFSIZ - length, g.options);
	}

	if (g.desc)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "%s\n", g.desc);
	 }

	if (g.version)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "Version: %s\n", g.version);
	}

	if (g.date)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "Date: %s\n", g.date);
	}

	if (g.author)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "Author: %s\n", g.author);
	}

	if (g.contact)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "Contact: %s\n", g.contact);
	}

	if (g.vendor)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "Vendor: %s\n", g.vendor);
	}

	if (g.url)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "URL: %s\n", g.url);
	}

	if (g.legal)
	{
		length = strlen(buf);
		snprintf(buf + length, BUFSIZ - length, "\n%s\n", g.legal);
	}

	msg("%s", buf);
	exit(0);
}

/*
** void prog_version_msg(void)
**
** Emits a program version message. This message consists of the program's
** name and version.
*/

static void prog_version_msg(void)
{
	char buf[BUFSIZ];

	if (g.name && g.version)
		snprintf(buf, BUFSIZ, "%s-%s\n", g.name, g.version);
	else if (g.name)
		snprintf(buf, BUFSIZ, "%s\n", g.name);
	else if (g.version)
		snprintf(buf, BUFSIZ, "%s\n", g.version);
	else
		*buf = '\0';

	msg("%s", buf);
	exit(0);
}

/*
** const char *prog_basename(const char *path)
**
** Returns the filename part of path.
*/

const char *prog_basename(const char *path)
{
	const char *name = strrchr(path, PATH_SEP);

	return (name) ? (name + 1) : (char*)path;
}

static Option prog_optab[] =
{
	{ "help", NULL, "Prints a help message then exits", no_argument, OPT_NONE, OPT_FUNCTION, (void*)prog_help_msg },
	{ "version", NULL, "Prints a version message then exits", no_argument, OPT_NONE, OPT_FUNCTION, (void*)prog_version_msg },
	{ "debug", "level", "Sets the debug level", optional_argument, OPT_NONE, OPT_FUNCTION, (void*)prog_set_debug_level },
	{ NULL, NULL, NULL, 0, 0, 0, NULL }
};

Options prog_options_table[1] = {{ NULL, 0, prog_optab }};

#ifdef TEST

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

int verify(int i, const char *name, const char *result, const char *prog_name, const char *type)
{
	char buf[BUFSIZ];
	char result_buf[BUFSIZ];
	int fd;
	ssize_t bytes;

	if ((fd = open(name, O_RDONLY)) == -1)
	{
		printf("Test%d: failed to output message\n", i);
		return 1;
	}

	memset(buf, '\0', BUFSIZ);
	bytes = read(fd, buf, BUFSIZ);
	close(fd);
	unlink(name);

	if (bytes == -1)
	{
		printf("Test%d: failed to read output (%s)\n", i, strerror(errno));
		return 1;
	}

	snprintf(result_buf, BUFSIZ, result, prog_name, prog_name);

	if (strcmp(buf, result_buf))
	{
		printf("Test%d: incorrect output:\nshould be:\n%s\nwas:\n%s\n", i, result_buf, buf);
		return 1;
	}

	return 0;
}

int main(int ac, char **av)
{
	const char *prog_name;
	char out_name[16];
	char err_name[16];
	int out, err;
	int tests = 3;
	int argc = 3;
	char *argv[3][4] =
	{
		{ NULL, "-debug=4", "-help", NULL },
		{ NULL, "-debug", "-version", NULL },
		{ NULL, "-debug", "-invalid", NULL }
	};
	char *results[3][2] =
	{
		/* -help output */
		{
			/* stdout */
			"usage: %s [options]\n"
			"        -help          -- Prints a help message then exits\n"
			"        -version       -- Prints a version message then exits\n"
			"        -debug [level] -- Sets the debug level\n"
			"This program tests the prog module.\n"
			"Version: 0.1\n"
			"Date: 13/10/1999\n"
			"Author: raf <raf2@zip.com.au>\n"
			"Contact: raf <raf2@zip.com.au>\n"
			"Vendor: Twisted Systems\n"
			"URL: http://www.zip.com.au/~raf2/lib/software/daemon\n"
			"\n"
			"This software is free. Use it at your own risk.\n",

			/* stderr */
			""
		},

		/* -version output */

		{
			/* stdout */
			"%s-0.1\n",

			/* stderr */
			""
		},

		/* -invalid output */
		{
			/* stdout */
			"",

			/* stderr */
			"%s: unrecognized option `-invalid'\n"
			"usage: %s [options]\n"
			"        -help          -- Prints a help message then exits\n"
			"        -version       -- Prints a version message then exits\n"
			"        -debug [level] -- Sets the debug level\n"
		}
	};

	int errors = 0;
	int i;

	printf("Testing: %s\n", *av);

	prog_name = prog_basename(*av);
	argv[0][0] = argv[1][0] = argv[2][0] = (char*)prog_name;

	prog_init();
	prog_set_name(prog_name);
	prog_set_syntax("[options]");
	prog_set_options(prog_options_table);
	prog_set_version("0.1");
	prog_set_date("13/10/1999");
	prog_set_author("raf <raf2@zip.com.au>");
	prog_set_contact("raf <raf2@zip.com.au>");
	prog_set_vendor("Twisted Systems");
	prog_set_url("http://www.zip.com.au/~raf2/lib/software/daemon");
	prog_set_legal("This software is free. Use it at your own risk.");
	prog_set_desc("This program tests the prog module.");

	out = dup(STDOUT_FILENO);
	err = dup(STDERR_FILENO);

	for (i = 0; i < tests; ++i)
	{
		pid_t pid = fork();

		snprintf(out_name, 16, "./prog.out.%d", i);
		snprintf(err_name, 16, "./prog.err.%d", i);
		freopen(out_name, "a", stdout);
		freopen(err_name, "a", stderr);

		switch (pid)
		{
			case 0:
			{
				return (prog_opt_process(argc, argv[i]) != argc);
			}

			case -1:
			{
				dup2(out, STDOUT_FILENO);
				dup2(err, STDERR_FILENO);
				++errors, printf("Test%d: failed to perform test - fork() failed (%s)\n", i + 1, strerror(errno));
				continue;
			}

			default:
			{
				int status[1];

				dup2(out, STDOUT_FILENO);
				dup2(err, STDERR_FILENO);

				if (waitpid(pid, status, 0) == -1)
				{
					++errors, printf("Test%d: failed to wait for test - waitpid(%d) failed (%s)\n", i + 1, pid, strerror(errno));
					continue;
				}

				if (WIFSIGNALED(*status))
					++errors, printf("Test%d: failed: received signal %d\n", i + 1, WTERMSIG(*status));

				if (i != 2 && WIFEXITED(*status) && WEXITSTATUS(*status))
					++errors, printf("Test%d: failed: exit status %d\n", i + 1, WEXITSTATUS(*status));
			}
		}

		errors += verify(i, out_name, results[i][0], prog_name, "stdout");
		errors += verify(i, err_name, results[i][1], prog_name, "stderr");
	}

	if (errors)
		printf("%d/%d tests failed\n", errors, tests);
	else
		printf("All tests passed\n");

	exit(0);
}

#endif

/* vi:set ts=4 sw=4: */
