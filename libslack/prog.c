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

I<libslack(prog)> - program framework module

=head1 SYNOPSIS

    #include <slack/prog.h>

    void prog_init(void);
    const char *prog_set_name(const char *name);
    Options *prog_set_options(Options *options);
    const char *prog_set_syntax(const char *syntax);
    const char *prog_set_desc(const char *desc);
    const char *prog_set_version(const char *version);
    const char *prog_set_date(const char *date);
    const char *prog_set_author(const char *author);
    const char *prog_set_contact(const char *contact);
    const char *prog_set_vendor(const char *vendor);
    const char *prog_set_url(const char *url);
    const char *prog_set_legal(const char *legal);
    Msg *prog_set_out(Msg *out);
    Msg *prog_set_err(Msg *err);
    Msg *prog_set_dbg(Msg *dbg);
    size_t prog_set_debug_level(size_t debug_level);
    size_t prog_set_verbosity_level(size_t verbosity_level);
    const char *prog_name(void);
    const Options *prog_options(void);
    const char *prog_syntax(void);
    const char *prog_desc(void);
    const char *prog_version(void);
    const char *prog_date(void);
    const char *prog_author(void);
    const char *prog_contact(void);
    const char *prog_vendor(void);
    const char *prog_url(void);
    const char *prog_legal(void);
    Msg *prog_out(void);
    Msg *prog_err(void);
    Msg *prog_dbg(void);
    size_t prog_debug_level(void);
    size_t prog_verbosity_level(void);
    int prog_out_fd(int fd);
    int prog_out_stdout(void);
    int prog_out_file(const char *path);
    int prog_out_syslog(const char *ident, int option, int facility);
    int prog_out_none(void);
    int prog_err_fd(int fd);
    int prog_err_stderr(void);
    int prog_err_file(const char *path);
    int prog_err_syslog(const char *ident, int option, int facility);
    int prog_err_none(void);
    int prog_dbg_fd(int fd);
    int prog_dbg_stdout(void);
    int prog_dbg_stderr(void);
    int prog_dbg_file(const char *path);
    int prog_dbg_syslog(const char *id, int option, int facility);
    int prog_dbg_none(void);
    int prog_opt_process(int ac, char **av);
    void prog_usage_msg(const char *fmt, ...);
    void prog_help_msg(void);
    void prog_version_msg(void);
    const char *prog_basename(const char *path);
    extern Options prog_options_table[1];

=head1 DESCRIPTION

This module provides functions for arbitrary programs. The services include
program identification; flexible command line option processing; help, usage
and version messages; flexible debug, verbose, error and normal messaging
(simple call syntax with arbitrary message destinations including
multiplexing).

=over 4

=cut

*/

#include "std.h"

#include "msg.h"
#include "err.h"
#include "mem.h"
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
	size_t debug_level;
	size_t verbosity_level;
};

static Prog g =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, 0, 0
};

/*

=item C<void prog_init(void)>

Initialises the message, error and debug destinations to C<stdout>,
C<stderr> and C<stderr>, respectively. This function must be called before
any other functions in this module or the I<err> module.

=cut

*/

void prog_init(void)
{
	prog_out_stdout();
	prog_err_stderr();
	prog_dbg_stderr();
}

/*

=item C<const char *prog_set_name(const char *name)>

Sets the program's name to C<name>. This is used when composing usage, help,
version and error messages. Returns C<name>.

=cut

*/

const char *prog_set_name(const char *name)
{
	return g.name = name;
}

/*

=item C<Options *prog_set_options(Options *options)>

Sets the program's options to C<options>. This is used when processing the
command line options with I<prog_opt_process()>. Returns C<options>.

=cut

*/

Options *prog_set_options(Options *options)
{
	return g.options = options;
}

/*

=item C<const char *prog_set_syntax(const char *syntax)>

Sets the program's syntax description to C<syntax>. This is used when
composing usage and help messages. It must contain a description of the
command line arguments, excluding any options. Returns C<syntax>.

=cut

*/

const char *prog_set_syntax(const char *syntax)
{
	return g.syntax = syntax;
}

/*

=item C<const char *prog_set_desc(const char *desc)>

Sets the program's description to C<desc>. This is used when composing help
messages. Returns C<desc>.

=cut

*/

const char *prog_set_desc(const char *desc)
{
	return g.desc = desc;
}

/*

=item C<const char *prog_set_version(const char *version)>

Sets the program's version to C<version>. This is used when composing help
and version messages. Returns C<version>.

=cut

*/

const char *prog_set_version(const char *version)
{
	return g.version = version;
}

/*

=item C<const char *prog_set_date(const char *date)>

Sets the program's release date to C<date>. This is used when composing help
messages. Returns C<date>.

=cut

*/

const char *prog_set_date(const char *date)
{
	return g.date = date;
}

/*

=item C<const char *prog_set_author(const char *author)>

Sets the program's author to C<author>. This is used when composing help
messages. It must contain the (free format) name of the author. Returns
C<author>.

=cut

*/

const char *prog_set_author(const char *author)
{
	return g.author = author;
}

/*

=item C<const char *prog_set_contact(const char *contact)>

Sets the program's contact address to C<contact>. This is used when
composing help messages. It must contain the email address to which bug
reports should be sent. Returns C<contact>.

=cut

*/

const char *prog_set_contact(const char *contact)
{
	return g.contact = contact;
}

/*

=item C<const char *prog_set_vendor(const char *vendor)>

Sets the program's vendor to C<vendor>. This is used when composing help
messages. It must contain the (free format) name of the vendor. Returns
C<vendor>.

=cut

*/

const char *prog_set_vendor(const char *vendor)
{
	return g.vendor = vendor;
}

/*

=item C<const char *prog_set_url(const char *url)>

Sets the program's URL to C<url>. This is used when composing help messages.
It must contain the URL where the program can be downloaded. Returns C<url>.

=cut

*/

const char *prog_set_url(const char *url)
{
	return g.url = url;
}

/*

=item C<const char *prog_set_legal(const char *legal)>

Sets the program's legal notice to C<legal>. This is used when composing
help messages. It is assumed that the legal notice may contain multiple
lines and so must contain its own newline characters. Returns C<legal>.

=cut

*/

const char *prog_set_legal(const char *legal)
{
	return g.legal = legal;
}

/*

=item C<Msg *prog_set_out(Msg *out)>

Sets the program's message destination to C<out>. This is used by I<msg()>
and I<vmsg()> which are, in turn, used to emit usage, version and help
messages. The program message destination is set to standard output by
I<prog_init()> but it can be anything. However, it is probably best to
leave it as standard output until after command line option processing is
complete. See I<msg()> for details. Returns C<out>.

=cut

*/

Msg *prog_set_out(Msg *out)
{
	if (g.out && g.out != out)
		msg_release(g.out);

	return g.out = out;
}

/*

=item C<Msg *prog_set_err(Msg *err)>

Sets the program's error message destination to C<err>. This is used by
I<error(3)>, I<errorsys(3)>, I<fatal(3)>, I<fatalsys(3)>, I<dump(3)> and
I<dumpsys(3)>. The program error message destination is set to standard
error by I<prog_init(3)> but it can be anything. See I<msg(3)> for details.
Returns C<err>.

=cut

*/

Msg *prog_set_err(Msg *err)
{
	if (g.err && g.err != err)
		msg_release(g.err);

	return g.err = err;
}

/*

=item C<Msg *prog_set_dbg(Msg *dbg)>

Sets the program's debug message destination to C<dbg>. This is set to
standard error by I<prog_init(3)> but it can be set to anything. See
I<msg(3)> for details. Returns C<dbg>.

=cut

*/

Msg *prog_set_dbg(Msg *dbg)
{
	if (g.dbg && g.dbg != dbg)
		msg_release(g.dbg);

	return g.dbg = dbg;
}

/*

=item C<size_t prog_set_debug_level(size_t debug_level)>

Sets the program's debug level to C<debug_level>. This is used when
determining whether or not to emit a debug message. Debug messages with a
level that is lower than the program debug level are emited. Returns the
previous debug level.

=cut

*/

size_t prog_set_debug_level(size_t debug_level)
{
	size_t prev = g.debug_level;
	g.debug_level = debug_level;
	return prev;
}

/*

=item C<size_t prog_set_verbosity_level(size_t verbosity_level)>

Sets the program's verbosity level to C<verbosity_level>. This is used to
determine whether or not to emit verbose messages. Verbose messages with a
level that is lower than the program verbosity level are emitted. Returns
the previous verbosity level.

=cut

*/

size_t prog_set_verbosity_level(size_t verbosity_level)
{
	size_t prev = g.verbosity_level;
	g.verbosity_level = verbosity_level;
	return prev;
}

/*

=item C<const char *prog_name(void)>

Returns the program's name.

=cut

*/

const char *prog_name(void)
{
	return g.name;
}

/*

=item C<const Options *prog_options(void)>

Returns the program's options.

=cut

*/

const Options *prog_options(void)
{
	return g.options;
}

/*

=item C<const char *prog_syntax(void)>

Returns the program's syntax description.

=cut

*/

const char *prog_syntax(void)
{
	return g.syntax;
}

/*

=item C<const char *prog_desc(void)>

Returns the program's description.

=cut

*/

const char *prog_desc(void)
{
	return g.desc;
}

/*

=item C<const char *prog_version(void)>

Returns the program's version string.

=cut

*/

const char *prog_version(void)
{
	return g.version;
}

/*

=item C<const char *prog_date(void)>

Returns the program's release date.

=cut

*/

const char *prog_date(void)
{
	return g.date;
}

/*

=item C<const char *prog_author(void)>

Returns the program's author.

=cut

*/

const char *prog_author(void)
{
	return g.author;
}

/*

=item C<const char *prog_contact(void)>

Returns the program's contact address.

=cut

*/

const char *prog_contact(void)
{
	return g.contact;
}

/*

=item C<const char *prog_vendor(void)>

Returns the program's vendor.

=cut

*/

const char *prog_vendor(void)
{
	return g.vendor;
}

/*

=item C<const char *prog_url(void)>

Returns the program's URL.

=cut

*/

const char *prog_url(void)
{
	return g.url;
}

/*

=item C<const char *prog_legal(void)>

Returns the program's legal notice.

=cut

*/

const char *prog_legal(void)
{
	return g.legal;
}

/*

=item C<Msg *prog_out(void)>

Returns the program's message destination.

=cut

*/

Msg *prog_out(void)
{
	return g.out;
}

/*

=item C<Msg *prog_err(void)>

Returns the program's error message destination.

=cut

*/

Msg *prog_err(void)
{
	return g.err;
}

/*

=item C<Msg *prog_dbg(void)>

Returns the program's debug message destination.

=cut

*/

Msg *prog_dbg(void)
{
	return g.dbg;
}

/*

=item C<size_t prog_debug_level(void)>

Returns the program's debug level.

=cut

*/

size_t prog_debug_level(void)
{
	return g.debug_level;
}

/*

=item C<size_t prog_verbosity_level(void)>

Returns the program's verbosity level.

=cut

*/

size_t prog_verbosity_level(void)
{
	return g.verbosity_level;
}

/*

=item C<int prog_out_fd(int fd)>

Sets the program's message destination to be the file descriptor specified
by C<fd>. On success, returns 0. On error, returns -1.

=cut

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

=item C<int prog_out_stdout(void)>

Sets the program's message destination to be standard output. On success,
returns 0. On error, returns -1.

=cut

*/

int prog_out_stdout(void)
{
	return prog_out_fd(STDOUT_FILENO);
}

/*

=item C<int prog_out_file(const char *path)>

Sets the program's message destination to be the file specified by C<path>.
On success, returns 0. On error, returns -1.

=cut

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

=item C<int prog_out_syslog(const char *ident, int option, int facility)>

Sets the program's message destination to be I<syslog> initialised with
C<ident>, C<option> and C<facility>. On success, returns 0. On error,
returns -1.

=cut

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

=item C<int prog_out_none(void)>

Sets the program's message destination to C<NULL>. This disables all normal
messages. Returns 0.

=cut

*/

int prog_out_none(void)
{
	prog_set_out(NULL);

	return 0;
}

/*

=item C<int prog_err_fd(int fd)>

Sets the program's error message destination to be the file descriptor
specified by C<fd>. On success, returns 0. On error, returns -1.

=cut

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

=item C<int prog_err_stderr(void)>

Sets the program's error message destination to be standard error. On success,
returns 0. On error, returns -1.

=cut

*/

int prog_err_stderr(void)
{
	return prog_err_fd(STDERR_FILENO);
}

/*

=item C<int prog_err_file(const char *path)>

Sets the program's error message destination to be the file specified by
C<path>. On success, returns 0. On error, returns -1.

=cut

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

=item C<int prog_err_syslog(const char *ident, int option, int facility)>

Sets the program's error message destination to be I<syslog> initialised
with C<ident>, C<option> and C<facility>. On success, returns 0. On error,
returns -1.

=cut

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

=item C<int prog_err_none(void)>

Sets the program's error message destination to C<NULL>. This disables all
error messages. Returns 0.

=cut

*/

int prog_err_none(void)
{
	prog_set_err(NULL);

	return 0;
}

/*

=item C<int prog_dbg_fd(int fd)>

Sets the program's debug message destination to be the file descriptor
specified by C<fd>. On success, returns 0. On error, returns -1.

=cut

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

=item C<int prog_dbg_stdout(void)>

Sets the program's debug message destination to be standard output. On
success, returns 0. On error, returns -1.

=cut

*/

int prog_dbg_stdout(void)
{
	return prog_dbg_fd(STDOUT_FILENO);
}

/*

=item C<int prog_dbg_stderr(void)>

Sets the program's debug message destination to be standard error. On
success, returns 0. On error, returns -1.

=cut

*/

int prog_dbg_stderr(void)
{
	return prog_dbg_fd(STDERR_FILENO);
}

/*

=item C<int prog_dbg_file(const char *path)>

Sets the program's debug message destination to be the file specified by
C<path>. On success, returns 0. On error, returns -1.

=cut

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

=item C<int prog_dbg_syslog(const char *id, int option, int facility)>

Sets the program's debug message destination to be I<syslog> initialised
with C<ident>, C<option> and C<facility>. On success, returns 0. On error,
returns -1.

=cut

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

=item C<int prog_dbg_none(void)>

Sets the program's debug message destination to C<NULL>. This disables all
debug messages. Returns 0.

=cut

*/

int prog_dbg_none(void)
{
	prog_set_dbg(NULL);

	return 0;
}

/*

=item C<int prog_opt_process(int ac, const char **av)>

Parses and processes the command line options in C<av>. If there is an
error, a usage message is emitted and the program terminates. This function
is just an interface to I<GNU getopt_long(3)> that provides easier, more
flexible, and more complete option handling. As well as supplying the syntax
for options, this function requires their semantics and descriptions. The
descriptions allow usage and help messages to be automatically composed by
I<prog_usage_msg(3)> and I<prog_help_msg(3)>. The semantics (which may be
either a variable assignment or a function invocation) allow complete
command line option processing to be performed with a single call to this
function. On success, returns C<optind>. On error (i.e. invalid option),
calls I<prog_usage_msg()> which terminates the program with a return code
of -1. See I<opt(3)> for details on specifying option data.

=cut

*/

int prog_opt_process(int ac, char **av)
{
	int rc = opt_process(ac, av, g.options);
	if (rc == -1)
		prog_usage_msg("");

	return rc;
}

/*

=item C<void prog_usage_msg(const char *fmt, ...)>

Emits a program usage error message then terminates the program with a
return code of 1. The usage message consists of the program's name, syntax,
options descriptions and the given message. C<fmt> is a I<printf>-like
format string. Any remaining arguments are processed as in I<printf(3)>.

=cut

*/

void prog_usage_msg(const char *fmt, ...)
{
	char msg_buf[MSG_SIZE];
	char opt_buf[MSG_SIZE];
	int msg_length;
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg_buf, MSG_SIZE, fmt, args);
	va_end(args);
	opt_usage(opt_buf, MSG_SIZE, g.options);

	fflush(stderr);

	msg_length = strlen(msg_buf);
	msg_out(g.err, "%s%susage: %s %s\noptions:\n%s",
		msg_buf,
		(msg_length && msg_buf[msg_length - 1] != '\n') ? "\n" : "",
		(g.name) ? g.name : "",
		(g.syntax) ? g.syntax : "",
		opt_buf
	);

	exit(1);
}

/*

=item C<void prog_help_msg(void)>

Emits a program help message then terminates the program with a return code
of 0. This message consists of the program's usage message, description,
name, version, release date, author, vendor, URL, legal notice and contact
address.

=cut

*/

void prog_help_msg(void)
{
	char buf[MSG_SIZE];
	size_t length = 0;

	snprintf(buf, MSG_SIZE, "usage: %s %s\n",
		g.name ? g.name : "",
		g.syntax ? g.syntax : ""
	);

	if (g.options)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "options:\n");
		length = strlen(buf);
		opt_usage(buf + length, MSG_SIZE - length, g.options);
	}

	if (g.desc)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "\n%s\n", g.desc);
	 }

	if (g.name)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "Name: %s\n", g.name);
	}

	if (g.version)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "Version: %s\n", g.version);
	}

	if (g.date)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "Date: %s\n", g.date);
	}

	if (g.author)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "Author: %s\n", g.author);
	}

	if (g.vendor)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "Vendor: %s\n", g.vendor);
	}

	if (g.url)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "URL: %s\n", g.url);
	}

	if (g.legal)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "\n%s\n", g.legal);
	}

	if (g.contact)
	{
		length = strlen(buf);
		snprintf(buf + length, MSG_SIZE - length, "Report bugs to %s\n", g.contact);
	}

	msg("%s", buf);
	exit(0);
}

/*

=item C<void prog_version_msg(void)>

Emits a program version message then terminates the program with a return
code of 0. This message consists of the program's name and version.

=cut

*/

void prog_version_msg(void)
{
	char buf[MSG_SIZE];

	if (g.name && g.version)
		snprintf(buf, MSG_SIZE, "%s-%s\n", g.name, g.version);
	else if (g.name)
		snprintf(buf, MSG_SIZE, "%s\n", g.name);
	else if (g.version)
		snprintf(buf, MSG_SIZE, "%s\n", g.version);
	else
		*buf = '\0';

	msg("%s", buf);
	exit(0);
}

/*

=item C<const char *prog_basename(const char *path)>

Returns the filename part of C<path>.

=cut

*/

const char *prog_basename(const char *path)
{
	const char *name = strrchr(path, PATH_SEP);

	return (name) ? (name + 1) : (char *)path;
}

/*

=item C<extern Options prog_options_table[1];>

Contains the syntax, semantics and descriptions of some options that are
available to all programs that use I<libslack>. These options are:

=over 4

=item C<help>

Print a help message then exit

=item C<version>

Print a version message then exit

=item C<verbose=>I<level>

Set the verbosity level

=item C<debug=>I<level>

Set the debug level

=back

If your program supports no other options than these, I<prog_options_table>
can be passed directly to I<prog_set_options(3)>. Otherwise,
I<prog_options_table> should be assigned to the C<parent> field of the
C<Options> structure that will be passed to I<prog_set_options(3)>.

=cut

*/

static Option prog_optab[] =
{
	{
		"help", 'h', NULL, "Print a help message then exit",
		no_argument, OPT_NONE, OPT_FUNCTION, (void *)prog_help_msg
	},
	{
		"version", 'V', NULL, "Print a version message then exit",
		no_argument, OPT_NONE, OPT_FUNCTION, (void *)prog_version_msg
	},
	{
		"verbose", 'v', "level", "Set the verbosity level",
		required_argument, OPT_INTEGER, OPT_FUNCTION, (void *)prog_set_verbosity_level
	},
#ifndef NDEBUG
	{
		"debug", 'd', "level", "Set the debug level",
		required_argument, OPT_INTEGER, OPT_FUNCTION, (void *)prog_set_debug_level
	},
#endif
	{
		NULL, '\0', NULL, NULL, 0, 0, 0, NULL
	}
};

Options prog_options_table[1] = {{ NULL, prog_optab }};

/*

=back

=head1 EXAMPLES

    #include <stdio.h>
    #include <slack/prog.h>

    int main(int ac, char **av)
    {
        int a;
        prog_init();
        prog_set_name(prog_basename(*av));
        prog_set_syntax("[options] arg...");
        prog_set_options(prog_options_table);
        prog_set_version("1.0");
        prog_set_date("20000902");
        prog_set_author("raf <raf@raf.org>");
        prog_set_contact(prog_author());
        prog_set_vendor("ACME Software Company");
        prog_set_url("http://libslack.org/");
        prog_set_legal("This software is released under the terms of the GPL.\n");
        prog_set_desc("This program is an example of the prog module.\n");

        for (a = prog_opt_process(ac, av); a < ac; ++a)
            printf("av[%d] = \"%s\"\n", a, av[a]);

        return 0;
    }

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
L<net(3)|net(3)>,
L<opt(3)|opt(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<str(3)|str(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

#include <fcntl.h>
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
		printf("Test%d: incorrect %s:\nshould be:\n%s\nwas:\n%s\n", i, type, result_buf, buf);
		return 1;
	}

	return 0;
}

int main(int ac, char **av)
{
	const char *prog_name;
	char out_name[32];
	char err_name[32];
	int tests = 3;
	int argc = 3;
	char *argv[3][4] =
	{
		{ "prog.test", "--debug=4", "--help", NULL },
		{ "prog.test", "--debug=1", "--version", NULL },
		{ "prog.test", "--debug=1", "--invalid", NULL }
	};
	char *results[3][2] =
	{
		/* -help output */
		{
			/* stdout */
			"usage: %s [options]\n"
			"options:\n"
			"\n"
			"      -h, --help          - Print a help message then exit\n"
			"      -V, --version       - Print a version message then exit\n"
			"      -v, --verbose=level - Set the verbosity level\n"
			"      -d, --debug=level   - Set the debug level\n"
			"\n"
			"This program tests the prog module.\n"
			"\n"
			"Name: %s\n"
			"Version: 1.0\n"
			"Date: 20000902\n"
			"Author: raf <raf@raf.org>\n"
			"Vendor: A Software Company\n"
			"URL: http://libslack.org/test/\n"
			"\n"
			"This software is released under the terms of the GPL.\n"
			"\n"
			"Report bugs to raf <raf@raf.org>\n",

			/* stderr */
			""
		},

		/* -version output */

		{
			/* stdout */
			"%s-1.0\n",

			/* stderr */
			""
		},

		/* -invalid output */
		{
			/* stdout */
			"",

			/* stderr */
			"%s: unrecognized option `--invalid'\n"
			"usage: %s [options]\n"
			"options:\n"
			"\n"
			"      -h, --help          - Print a help message then exit\n"
			"      -V, --version       - Print a version message then exit\n"
			"      -v, --verbose=level - Set the verbosity level\n"
			"      -d, --debug=level   - Set the debug level\n"
		}
	};

	int errors = 0;
	int i;

	printf("Testing: prog\n");

	prog_name = prog_basename(*av);
	argv[0][0] = argv[1][0] = argv[2][0] = (char *)prog_name;

	prog_init();
	prog_set_name(prog_name);
	prog_set_syntax("[options]");
	prog_set_options(prog_options_table);
	prog_set_version("1.0");
	prog_set_date("20000902");
	prog_set_author("raf <raf@raf.org>");
	prog_set_contact("raf <raf@raf.org>");
	prog_set_vendor("A Software Company");
	prog_set_url("http://libslack.org/test/");
	prog_set_legal("This software is released under the terms of the GPL.\n");
	prog_set_desc("This program tests the prog module.\n");

	for (i = 0; i < tests; ++i)
	{
		pid_t pid;

		snprintf(out_name, 32, "prog.out.%d", i);
		snprintf(err_name, 32, "prog.err.%d", i);

		switch (pid = fork())
		{
			case 0:
			{
				freopen(out_name, "a", stdout);
				freopen(err_name, "a", stderr);

				return (prog_opt_process(argc, argv[i]) != argc);
			}

			case -1:
			{
				++errors, printf("Test%d: failed to perform test - fork() failed (%s)\n", i + 1, strerror(errno));
				continue;
			}

			default:
			{
				int status[1];

				if (waitpid(pid, status, 0) == -1)
				{
					++errors, printf("Test%d: failed to wait for test - waitpid(%d) failed (%s)\n", i + 1, (int)pid, strerror(errno));
					continue;
				}

				if (WIFSIGNALED(*status))
					++errors, printf("Test%d: failed: received signal %d\n", i + 1, WTERMSIG(*status));

				if (i != 2 && WIFEXITED(*status) && WEXITSTATUS(*status))
					++errors, printf("Test%d: failed: exit status %d\n", i + 1, WEXITSTATUS(*status));
			}
		}

		errors += verify(i + 1, out_name, results[i][0], prog_name, "stdout");
		errors += verify(i + 1, err_name, results[i][1], prog_name, "stderr");
	}

	if (errors)
		printf("%d/%d tests failed\n", errors, tests * 2);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
