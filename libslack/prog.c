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
    int prog_set_locker(Locker *locker);
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

    typedef struct option option;
    typedef struct Option Option;
    typedef struct Options Options;

    typedef void (*opt_action_int_t)(int);
    typedef void (*opt_action_optional_int_t)(int *);
    typedef void (*opt_action_string_t)(const char *);
    typedef void (*opt_action_optional_string_t)(const char *);
    typedef void (*opt_action_none_t)(void);

    enum OptionArgument
    {
        OPT_NONE,
        OPT_INTEGER,
        OPT_STRING
    };

    enum OptionAction
    {
        OPT_NOTHING,
        OPT_VARIABLE,
        OPT_FUNCTION
    };

    typedef enum OptionArgument OptionArgument;
    typedef enum OptionAction OptionAction;

    struct Option
    {
        const char *name;
        char short_name;
        const char *argname;
        const char *desc;
        int has_arg;
        OptionArgument arg_type;
        OptionAction action;
        void *object;
    };

    struct Options
    {
        Options *parent;
        Option *options;
    };

    int opt_process(int argc, char **argv, Options *options);
    char *opt_usage(char *buf, size_t size, Options *options);

=head1 DESCRIPTION

This module provides functions for arbitrary programs. The services include
program identification; flexible, complete command line option processing;
help, usage and version messages; flexible debug, verbose, error and normal
messaging (simple call syntax with arbitrary message destinations including
multiplexing).

This module exposes an alternate interface to I<GNU getopt_long(3)>. It
defines a way to specify command line option syntax, semantics and
descriptions in multiple, discrete chunks. The I<getopt> functions require
that the client specify the syntax and partial semantics for all options in
the same place (if it is to be done statically). This can be annoying when
library modules require their own command line options. This module allows
various parts of a program to (statically) specify their own command line
options independently and link them together via I<parent> pointers.

Option syntax is specified in much the same way as for I<GNU
getopt_long(3)>. Option semantics are specified by an action
(C<OPT_NOTHING>, C<OPT_VARIABLE> or C<OPT_FUNCTION>), an argument type
(C<OPT_NONE>, C<OPT_INTEGER> or C<OPT_STRING>) and an object (C<int *>,
C<char **>, C<func()>, C<func(int)> or C<func(char *)>).

The I<opt_process()> and I<opt_usage()> functions are used by the I<prog>
functions and needn't be used directly. Instead, use I<prog_opt_process(3)>
to execute options and I<prog_usage_msg(3)> and I<prog_help_msg()> to
construct usage and help message directly from the supplied option data.
They are exposed in case you don't want to use any other part of this module.

=over 4

=cut

*/

#include "std.h"

#include "msg.h"
#include "err.h"
#include "mem.h"
#include "prog.h"

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
	Locker *locker;
};

static Prog g =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, 0, 0, NULL
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.name = name;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return name;
}

/*

=item C<Options *prog_set_options(Options *options)>

Sets the program's options to C<options>. This is used when processing the
command line options with I<prog_opt_process()>. Returns C<options>.

=cut

*/

Options *prog_set_options(Options *options)
{
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.options = options;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return options;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.syntax = syntax;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return syntax;
}

/*

=item C<const char *prog_set_desc(const char *desc)>

Sets the program's description to C<desc>. This is used when composing help
messages. Returns C<desc>.

=cut

*/

const char *prog_set_desc(const char *desc)
{
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.desc = desc;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return desc;
}

/*

=item C<const char *prog_set_version(const char *version)>

Sets the program's version to C<version>. This is used when composing help
and version messages. Returns C<version>.

=cut

*/

const char *prog_set_version(const char *version)
{
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.version = version;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return version;
}

/*

=item C<const char *prog_set_date(const char *date)>

Sets the program's release date to C<date>. This is used when composing help
messages. Returns C<date>.

=cut

*/

const char *prog_set_date(const char *date)
{
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.date = date;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return date;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.author = author;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return author;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.contact = contact;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return contact;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.vendor = vendor;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return vendor;
}

/*

=item C<const char *prog_set_url(const char *url)>

Sets the program's URL to C<url>. This is used when composing help messages.
It must contain the URL where the program can be downloaded. Returns C<url>.

=cut

*/

const char *prog_set_url(const char *url)
{
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.url = url;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return url;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	g.legal = legal;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return legal;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	if (g.out && g.out != out)
		msg_release(g.out);

	g.out = out;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return out;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	if (g.err && g.err != err)
		msg_release(g.err);

	g.err = err;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return err;
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
	if (locker_wrlock(g.locker) == -1)
		return NULL;

	if (g.dbg && g.dbg != dbg)
		msg_release(g.dbg);

	g.dbg = dbg;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return dbg;
}

/*

=item C<ssize_t prog_set_debug_level(size_t debug_level)>

Sets the program's debug level to C<debug_level>. This is used when
determining whether or not to emit a debug message. Debug messages with a
level that is lower than the program debug level are emited. On success,
returns the previous debug level. On error, returns C<-1>.

=cut

*/

ssize_t prog_set_debug_level(size_t debug_level)
{
	size_t prev;

	if (locker_wrlock(g.locker) == -1)
		return -1;

	prev = g.debug_level;
	g.debug_level = debug_level;

	if (locker_unlock(g.locker) == -1)
		return -1;

	return prev;
}

/*

=item C<ssize_t prog_set_verbosity_level(size_t verbosity_level)>

Sets the program's verbosity level to C<verbosity_level>. This is used to
determine whether or not to emit verbose messages. Verbose messages with a
level that is lower than the program verbosity level are emitted. On
success, returns the previous verbosity level. On error, returns C<-1>.

=cut

*/

ssize_t prog_set_verbosity_level(size_t verbosity_level)
{
	size_t prev;

	if (locker_wrlock(g.locker) == -1)
		return -1;

	prev = g.verbosity_level;
	g.verbosity_level = verbosity_level;

	if (locker_unlock(g.locker) == -1)
		return -1;

	return prev;
}

/*

=item C<int prog_set_locker(Locker *locker)>

Sets the locker (multiple thread synchronisation strategy) for this module.
This is only needed in multi-threaded programs. See I<thread(3)|thread(3)>
for details. On success, returns C<0>. On error, returns C<-1>.

=cut

*/

int prog_set_locker(Locker *locker)
{
	if (g.locker)
		return -1;

	g.locker = locker;

	return 0;
}

/*

=item C<const char *prog_name(void)>

Returns the program's name.

=cut

*/

const char *prog_name(void)
{
	const char *name;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	name = g.name;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return name;
}

/*

=item C<const Options *prog_options(void)>

Returns the program's options.

=cut

*/

const Options *prog_options(void)
{
	const Options *options;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	options = g.options;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return options;
}

/*

=item C<const char *prog_syntax(void)>

Returns the program's syntax description.

=cut

*/

const char *prog_syntax(void)
{
	const char *syntax;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	syntax = g.syntax;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return syntax;
}

/*

=item C<const char *prog_desc(void)>

Returns the program's description.

=cut

*/

const char *prog_desc(void)
{
	const char *desc;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	desc = g.desc;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return desc;
}

/*

=item C<const char *prog_version(void)>

Returns the program's version string.

=cut

*/

const char *prog_version(void)
{
	const char *version;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	version = g.version;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return version;
}

/*

=item C<const char *prog_date(void)>

Returns the program's release date.

=cut

*/

const char *prog_date(void)
{
	const char *date;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	date = g.date;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return date;
}

/*

=item C<const char *prog_author(void)>

Returns the program's author.

=cut

*/

const char *prog_author(void)
{
	const char *author;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	author = g.author;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return author;
}

/*

=item C<const char *prog_contact(void)>

Returns the program's contact address.

=cut

*/

const char *prog_contact(void)
{
	const char *contact;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	contact = g.contact;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return contact;
}

/*

=item C<const char *prog_vendor(void)>

Returns the program's vendor.

=cut

*/

const char *prog_vendor(void)
{
	const char *vendor;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	vendor = g.vendor;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return vendor;
}

/*

=item C<const char *prog_url(void)>

Returns the program's URL.

=cut

*/

const char *prog_url(void)
{
	const char *url;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	url = g.url;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return url;
}

/*

=item C<const char *prog_legal(void)>

Returns the program's legal notice.

=cut

*/

const char *prog_legal(void)
{
	const char *legal;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	legal = g.legal;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return legal;
}

/*

=item C<Msg *prog_out(void)>

Returns the program's message destination.

=cut

*/

Msg *prog_out(void)
{
	Msg *out;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	out = g.out;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return out;
}

/*

=item C<Msg *prog_err(void)>

Returns the program's error message destination.

=cut

*/

Msg *prog_err(void)
{
	Msg *err;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	err = g.err;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return err;
}

/*

=item C<Msg *prog_dbg(void)>

Returns the program's debug message destination.

=cut

*/

Msg *prog_dbg(void)
{
	Msg *dbg;

	if (locker_rdlock(g.locker) == -1)
		return NULL;

	dbg = g.dbg;

	if (locker_unlock(g.locker) == -1)
		return NULL;

	return dbg;
}

/*

=item C<size_t prog_debug_level(void)>

Returns the program's debug level.

=cut

*/

size_t prog_debug_level(void)
{
	size_t debug_level;

	if (locker_rdlock(g.locker) == -1)
		return 0;

	debug_level = g.debug_level;

	if (locker_unlock(g.locker) == -1)
		return 0;

	return debug_level;
}

/*

=item C<size_t prog_verbosity_level(void)>

Returns the program's verbosity level.

=cut

*/

size_t prog_verbosity_level(void)
{
	size_t verbosity_level;

	if (locker_rdlock(g.locker) == -1)
		return 0;

	verbosity_level = g.verbosity_level;

	if (locker_unlock(g.locker) == -1)
		return 0;

	return verbosity_level;
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

	if (!(msg = msg_create_fd_locked(g.locker, fd)))
		return -1;

	if (!prog_set_out(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(msg = msg_create_file_locked(g.locker, path)))
		return -1;

	if (!prog_set_out(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(msg = msg_create_syslog_locked(g.locker, ident, option, facility)))
		return -1;

	if (!prog_set_out(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(msg = msg_create_fd_locked(g.locker, fd)))
		return -1;

	if (!prog_set_err(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(msg = msg_create_file_locked(g.locker, path)))
		return -1;

	if (!prog_set_err(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(msg = msg_create_syslog_locked(g.locker, ident, option, facility)))
		return -1;

	if (!prog_set_err(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(msg = msg_create_fd_locked(g.locker, fd)))
		return -1;

	if (!prog_set_dbg(msg))
	{
		msg_release(msg);
		return -1;
	}

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

	if (!(dbg = msg_create_file_locked(g.locker,path)))
		return -1;

	if (!prog_set_dbg(dbg))
	{
		msg_release(dbg);
		return -1;
	}

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

	if (!(dbg = msg_create_syslog_locked(g.locker, id, option, facility)))
		return -1;

	if (!prog_set_dbg(dbg))
	{
		msg_release(dbg);
		return -1;
	}

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
calls I<prog_usage_msg()> which terminates the program with a return code of
-1. See below (I<opt_usage()>) for details on specifying option data. On
error (other), returns C<-1>.

=cut

*/

int prog_opt_process(int ac, char **av)
{
	int rc;

	if (locker_rdlock(g.locker) == -1)
		return -1;

	rc = opt_process(ac, av, g.options);

	if (locker_unlock(g.locker) == -1)
		return -1;

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

B<Warning: Do not under any circumstances ever pass a non-literal string as
the fmt argument unless you know exactly how many conversions will take
place. Being careless with this is a very good way to build potential
security holes into your programs. The same is true for all functions that
take a printf()-like format string as an argument.>

    prog_usage_msg(buf);       // EVIL
    prog_usage_msg("%s", buf); // GOOD

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

	locker_rdlock(g.locker);
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

	locker_unlock(g.locker);

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

	locker_rdlock(g.locker);

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

	locker_unlock(g.locker);
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

	locker_rdlock(g.locker);

	if (g.name && g.version)
		snprintf(buf, MSG_SIZE, "%s-%s\n", g.name, g.version);
	else if (g.name)
		snprintf(buf, MSG_SIZE, "%s\n", g.name);
	else if (g.version)
		snprintf(buf, MSG_SIZE, "%s\n", g.version);
	else
		*buf = nul;

	locker_unlock(g.locker);

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

	return (name) ? (name + 1) : path;
}

/*

=item C<extern Options prog_options_table[1]>

Contains the syntax, semantics and descriptions of some options that are
available to all programs that use I<libslack>. These options are:

=over 4

=item C<help>

Print a help message then exit

=item C<version>

Print a version message then exit

=item C<verbose>I<[=level]>

Set the verbosity level (Defaults to 1 if I<level> not supplied)

=item C<debug>I<[=level]>

Set the debug level (Defaults to 1 if I<level> not supplied)

=back

If your program supports no other options than these, I<prog_options_table>
can be passed directly to I<prog_set_options(3)>. Otherwise,
I<prog_options_table> should be assigned to the C<parent> field of the
C<Options> structure that will be passed to I<prog_set_options(3)>.

=cut

*/

static void handle_verbose_option(int *arg)
{
	prog_set_verbosity_level(arg ? *arg : 1);
}

static void handle_debug_option(int *arg)
{
	prog_set_debug_level(arg ? *arg : 1);
}

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
		optional_argument, OPT_INTEGER, OPT_FUNCTION, (void *)handle_verbose_option
	},
#ifndef NDEBUG
	{
		"debug", 'd', "level", "Set the debug level",
		optional_argument, OPT_INTEGER, OPT_FUNCTION, (void *)handle_debug_option
	},
#endif
	{
		NULL, nul, NULL, NULL, 0, 0, 0, NULL
	}
};

Options prog_options_table[1] = {{ NULL, prog_optab }};

/*

C<opt_convert(Options *options)>

Creates and returns a flat table of option structs from C<options>. The
resulting array is for use with I<GNU getopt_long(3)>. The memory
returned must be I<free(3)>d by the caller.

*/

static option *opt_convert(Options *options)
{
	Options *opts;
	size_t size = 0;
	size_t i;
	option *ret;
	int index = 0;

	for (opts = options; opts; opts = opts->parent)
		for (i = 0; opts->options[i].name; ++i)
			++size;

	if (!(ret = mem_create(size + 1, option)))
		return NULL;

	for (opts = options; opts; opts = opts->parent)
	{
		int i;

		for (i = 0; opts->options[i].name; ++i, ++index)
		{
			ret[index].name = (char *)opts->options[i].name;
			ret[index].has_arg = opts->options[i].has_arg;
			ret[index].flag = NULL;
			ret[index].val = 0;
		}
	}

	memset(ret + index, nul, sizeof(option));

	return ret;
}

/*

C<opt_optstring(Options *options)>

Creates and returns a string containing all of the short option names from
C<options>. The resulting string is for use with I<GNU getopt_long(3)>. The
memory returned must be I<free(3)>d by the caller.

*/

static char *opt_optstring(Options *options)
{
	Options *opts;
	size_t size = 0;
	size_t i;
	char *optstring;
	char *p;

	for (opts = options; opts; opts = opts->parent)
		for (i = 0; opts->options[i].name; ++i)
			if (opts->options[i].short_name)
				++size;

	if (!(p = optstring = mem_create((size * 3) + 1, char)))
		return NULL;

	for (opts = options; opts; opts = opts->parent)
	{
		int i;

		for (i = 0; opts->options[i].name; ++i)
		{
			if (opts->options[i].short_name)
			{
				*p++ = opts->options[i].short_name;

				switch (opts->options[i].has_arg)
				{
					case optional_argument: *p++ = ':';
					case required_argument: *p++ = ':';
				}
			}
		}
	}

	*p = nul;

	return optstring;
}

/*

C<void opt_action(Options *options, int rc, int longindex, const char *argument)>

Performs the action associated with the option in C<options> when I<GNU
getopt_long(3)> returned C<rc> or C<longindex>. C<argument> is a pointer to
an C<int> or C<char *>.

 has_arg           arg_type    arg_action   optarg action
 ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~ ~~~~~~
 required_argument OPT_INTEGER OPT_VARIABLE yes    *object = atoi(argument)
 required_argument OPT_STRING  OPT_VARIABLE yes    *object = argument
 required_argument OPT_INTEGER OPT_FUNCTION yes    object(atoi(argument))
 required_argument OPT_STRING  OPT_FUNCTION yes    object(argument)

 optional_argument OPT_INTEGER OPT_VARIABLE yes    *object = atoi(argument)
 optional_argument OPT_STRING  OPT_VARIABLE yes    *object = argument
 optional_argument OPT_INTEGER OPT_FUNCTION yes    object(&atoi(argument))
 optional_argument OPT_STRING  OPT_FUNCTION yes    object(argument)

 optional_argument OPT_INTEGER OPT_VARIABLE no     ++*object
 optional_argument OPT_STRING  OPT_VARIABLE no     nothing
 optional_argument OPT_INTEGER OPT_FUNCTION no     object(NULL)
 optional_argument OPT_STRING  OPT_FUNCTION no     object(NULL)

 no_argument       OPT_NONE    OPT_VARIABLE no     ++*object
 no_argument       OPT_NONE    OPT_FUNCTION no     object()

*/

static void opt_action(Options *options, int rc, int longindex, const char *argument)
{
	Option *option;
	int i = -1;

	if (rc != 0 && longindex == -1) /* Short option */
	{
		for (; options; options = options->parent)
		{
			for (i = 0; options->options[i].name; ++i)
				if (options->options[i].short_name == rc)
					break;

			if (options->options[i].short_name == rc)
				break;
		}
	}
	else if (rc == 0 && longindex != -1) /* Long option */
	{
		for (i = 0; options; options = options->parent)
		{
			for (i = 0; longindex && options->options[i].name; ++i)
				--longindex;

			if (!options->options[i].name)
				continue;

			if (longindex == 0)
				break;
		}
	}
	else
		return;

	if (!options || i == -1)
		return;

	option = options->options + i;

	if (option->has_arg == required_argument && !argument)
		return;

	if (option->has_arg == no_argument && argument)
		return;

	if (argument)
	{
		switch (option->arg_type)
		{
			case OPT_NONE:
				break;

			case OPT_INTEGER:
			{
				switch (option->action)
				{
					case OPT_NOTHING:
						break;

					case OPT_VARIABLE:
						*(int *)option->object = atoi(argument);
						break;

					case OPT_FUNCTION:
					{
						if (option->has_arg == required_argument)
							((opt_action_int_t)option->object)(atoi(argument));
						else
						{
							int arg = atoi(argument);
							((opt_action_optional_int_t)option->object)(&arg);
						}
						break;
					}
				}

				break;
			}

			case OPT_STRING:
			{
				switch (option->action)
				{
					case OPT_NOTHING:
						break;

					case OPT_VARIABLE:
						*(const char **)option->object = argument;
						break;

					case OPT_FUNCTION:
						((opt_action_string_t)option->object)(argument);
						break;
				}

				break;
			}
		}
	}
	else
	{
		switch (option->action)
		{
			case OPT_NOTHING:
				break;

			case OPT_VARIABLE:
				if (option->arg_type != OPT_STRING)
					++*(int *)option->object;
				break;

			case OPT_FUNCTION:
				if (option->action == optional_argument)
					((opt_action_optional_int_t)option->object)(NULL);
				else
					((opt_action_none_t)option->object)();
				break;
		}
	}
}

/*

=item C<int opt_process(int argc, char **argv, Options *options)>

Parses C<argv> for options specified in C<options>. Uses I<GNU
getopt_long(3)>. As each option is encountered, its corresponding action is
performed. On success, returns C<optind>. On error (i.e. invalid option),
returns -1.

The following table shows the actions that are applied to an option's
C<object> based on its C<has_arg>, C<arg_type> and C<arg_action> attributes
and whether or not an argument is present.

 has_arg           arg_type    arg_action   optarg action
 ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~ ~~~~~~
 required_argument OPT_INTEGER OPT_VARIABLE yes    *object = atoi(argument)
 required_argument OPT_STRING  OPT_VARIABLE yes    *object = argument
 required_argument OPT_INTEGER OPT_FUNCTION yes    object(atoi(argument))
 required_argument OPT_STRING  OPT_FUNCTION yes    object(argument)

 optional_argument OPT_INTEGER OPT_VARIABLE yes    *object = atoi(argument)
 optional_argument OPT_STRING  OPT_VARIABLE yes    *object = argument
 optional_argument OPT_INTEGER OPT_FUNCTION yes    object(&atoi(argument))
 optional_argument OPT_STRING  OPT_FUNCTION yes    object(argument)

 optional_argument OPT_INTEGER OPT_VARIABLE no     ++*object
 optional_argument OPT_STRING  OPT_VARIABLE no     nothing
 optional_argument OPT_INTEGER OPT_FUNCTION no     object(NULL)
 optional_argument OPT_STRING  OPT_FUNCTION no     object(NULL)

 no_argument       OPT_NONE    OPT_VARIABLE no     ++*object
 no_argument       OPT_NONE    OPT_FUNCTION no     object()

=cut

*/

int opt_process(int argc, char **argv, Options *options)
{
	option *long_options;
	char *optstring;

	if (!(long_options = opt_convert(options)))
		return optind;

	if (!(optstring = opt_optstring(options)))
	{
		mem_release(long_options);
		return optind;
	}

	for (;;)
	{
		int longindex = -1;
		int rc = getopt_long(argc, argv, optstring, long_options, &longindex);

		if (rc == EOF)
			break;

		if (rc == '?' || rc == ':')
		{
			mem_release(long_options);
			mem_release(optstring);
			return -1;
		}

		opt_action(options, rc, longindex, optarg);
	}

	mem_release(long_options);
	mem_release(optstring);
	return optind;
}

/*

=item C<char *opt_usage(char *buf, size_t size, Options *options)>

Writes a usage message into C<buf> that displays the names, syntax and
descriptions of all options in C<options>. C<options> is traversed depth
first so the chunk with the C<NULL> I<parent> appears first. Each chunk of
options is preceeded by a blank line. No more than C<size> bytes are
written, including the terminating C<nul> character. The string returned
will look like:

      -a, --aaa       -- no-arg/var option
      -b, --bbb       -- no-arg/func option
      -c, --ccc=arg   -- int-arg/var option
      -d, --ddd=arg   -- int-arg/func option
      -e, --eee=arg   -- str-arg/var option
      -f, --fff=arg   -- str-arg/func option
      -g, --ggg[=arg] -- opt-int-arg/var option
      -h, --hhh[=arg] -- opt-str-arg/func option with one of those really,
                         really, really, long descriptions that goes on and on
                         and even contains a really long url:
                         http://www.zip.com.au/~joe/fairly/long/url/index.html
                         would you believe? Here it is again!
                         http://www.zip.com.au/~joe/fairly/long/url/index.html#just_kidding

=cut

*/

char *opt_usage(char *buf, size_t size, Options *options)
{
	const int total_width = 80;
	const char * const indent = "      ";
	const size_t indent_width = strlen(indent);
	const char * const leader = " - ";
	const size_t leader_width = strlen(leader);

	Options *opts;
	Options **stack;
	size_t max_width = 0;
	size_t depth = 0;
	size_t length = 0;
	size_t remainder = 0;
	int i;

	/* Determine the room needed by the longest option */

	for (opts = options; opts; opts = opts->parent, ++depth)
	{
		for (i = 0; opts->options[i].name; ++i)
		{
			Option *opt = opts->options + i;
			size_t width = strlen(opt->name);

			if (opt->argname)
			{
				width += 1 + strlen(opt->argname);

				if (opt->has_arg == optional_argument)
					width += 2;
			}

			if (width > max_width)
				max_width = width;
		}
	}

	/* Include room for "-o, --" */

	max_width += 6;

	/* Remember all options for reverse traversal */

	if (!(stack = mem_create(depth, Options *)))
		return NULL;

	for (opts = options, i = 0; opts; opts = opts->parent)
		stack[i++] = opts;

	/* Process options parent first */

	while (depth--)
	{
		opts = stack[depth];

		snprintf(buf + length, size - length, "\n");
		length = strlen(buf);

		for (i = 0; opts->options[i].name; ++i)
		{
			Option *opt = opts->options + i;
			char help[BUFSIZ];
			const char *desc;
			const char *next = NULL;
			size_t desc_length;
			size_t help_length;

			/* Produce the left hand side: syntax */

			snprintf(help, BUFSIZ, "%s%c%c%c --%s",
				indent,
				opt->short_name ? '-' : ' ',
				opt->short_name ? opt->short_name : ' ',
				opt->short_name ? ',' : ' ',
				opt->name
			);

			help_length = strlen(help);

			if (opt->argname)
			{
				int optional = (opt->has_arg == optional_argument);

				snprintf(help + help_length, BUFSIZ - help_length, "%s%s%s",
					optional ? "[=" : "=",
					opt->argname,
					optional ? "]" : ""
				);

				help_length = strlen(help);
			}

			snprintf(help + help_length, BUFSIZ - help_length, "%*s%s", max_width - help_length + indent_width, "", leader);
			help_length = strlen(help);
			remainder = total_width - help_length;

			/* Produce the right hand side: descriptions */

			for (desc = opt->desc; (desc_length = strlen(desc)) > remainder; desc = next)
			{
				/* Indent subsequent description lines */
				if (desc != opt->desc)
				{
					snprintf(help + help_length, BUFSIZ - help_length, "%*s%*.*s", indent_width + max_width, "", leader_width, (int)leader_width, "");
					help_length = strlen(help);
				}

				/* Look for last space that will fit on this line */

				next = desc + remainder;

				for (; next > desc && !isspace((int)(unsigned int)*next); --next)
				{}

				/* If none (word too long), look forward for end of word */

				if (next == desc)
				{
					while (isspace((int)(unsigned int)*next))
						++next;

					next = strchr(desc, ' ');

					if (!next)
						next = desc + desc_length;
				}

				/* Ignore any extra whitespace to the left */

				while (next != desc && isspace((int)(unsigned int)next[-1]))
					--next;

				/* Add one line of description */

				snprintf(help + help_length, BUFSIZ - help_length, "%*.*s\n", next - desc, next - desc, desc);
				help_length = strlen(help);

				/* Ignore any extra whitespace to the right */

				while (isspace((int)(unsigned int)*next))
					++next;
			}

			/* Add the last line of description */

			if (desc_length)
			{
				/* Indent the last line if it's not also the first line */

				if (desc != opt->desc)
				{
					snprintf(help + help_length, BUFSIZ - help_length, "%*s%*.*s", indent_width + max_width, "", leader_width, (int)leader_width, "");
					help_length = strlen(help);
				}

				snprintf(help + help_length, BUFSIZ - help_length, "%s\n", desc);
			}

			/* Add this option's help to the whole usage message */

			snprintf(buf + length, size - length, "%s", help);
			length = strlen(buf);
		}
	}

	mem_release(stack);

	return buf;
}

/*

=back

=head1 MT-Level

MT-Disciplined - prog functions

By default, this module is not thread safe because most programs are single
threaded and synchronisation doesn't come for free. For multi threaded
programs, use I<prog_set_locker()> to synchronise access to this module's
data before creating the threads that will access it.

Unsafe - opt functions

I<opt_process()> and I<opt_usage()> must only be used in the main thread.
They should not be needed anywhere else.

=head1 EXAMPLE

The following program:

 #include <stdio.h>
 #include <slack/prog.h>

 char *name = NULL;
 int minimum = 0;
 int reverse = 0;

 void setup_syslog(char *facility) { ... }
 void parse_config(char *path) { ... }

 Option example_optab[] =
 {
     {
         "name", 'n', "name", "Provide a name",
         required_argument, OPT_STRING, OPT_VARIABLE, &name
     },
     {
         "minimum", 'm', "minval", "Ignore everything below minimum",
         required_argument, OPT_INTEGER, OPT_VARIABLE, &minimum
     },
     {
         "syslog", 's', "facility.priority", "Send client's output to syslog (defaults to local0.debug)",
         optional_argument, OPT_STRING, OPT_FUNCTION, (void *)setup_syslog
     },
     {
         "reverse", 'r', NULL, "Reverse direction",
         no_argument, OPT_NONE, OPT_VARIABLE, &reverse
     },
     {
         "config", 'c', "path", "Specify the configuration file",
         required_argument, OPT_STRING, OPT_FUNCTION, (void *)parse_config
     },
     {
         NULL, '\0', NULL, NULL, 0, 0, 0, NULL
     }
 };

 Options options[1] = {{ prog_options_table, example_optab }};

 int main(int ac, char **av)
 {
     int a;
     prog_init();
     prog_set_name("example");
     prog_set_syntax("[options] arg...");
     prog_set_options(options);
     prog_set_version("1.0");
     prog_set_date("20010215");
     prog_set_author("raf <raf@raf.org>");
     prog_set_contact(prog_author());
     prog_set_url("http://libslack.org/");
     prog_set_legal("This software is released under the terms of the GPL.\n");
     prog_set_desc("This program is an example of the prog module.\n");

     for (a = prog_opt_process(ac, av); a < ac; ++a)
         msg("av[%d] = \"%s\"\n", a, av[a]);

     return 0;
 }

will behave like:

 $ example --version # to stdout
 example-1.0

 $ example --help # to stdout
 usage: example [options] arg...
 options:

       -h, --help                       - Print a help message then exit
       -V, --version                    - Print a version message then exit
       -v, --verbose[=level]            - Set the verbosity level
       -d, --debug[=level]              - Set the debug level

       -n, --name=name                  - Provide a name
       -m, --minimum=minval             - Ignore everything below minimum
       -s, --syslog[=facility.priority] - Send client's output to syslog
                                          (defaults to local0.debug)
       -r, --reverse                    - Reverse direction
       -c, --config=path                - Specify the configuration file

 This program is an example of the prog module.

 Name: example
 Version: 1.0
 Date: 20010215
 Author: raf <raf@raf.org>
 URL: http://libslack.org/

 This software is released under the terms of the GPL.

 Report bugs to raf <raf@raf.org>

 $ example -x # to stderr
 ./example: invalid option -- x
 usage: example [options] arg...
 options:
 
       -h, --help                       - Print a help message then exit
       -V, --version                    - Print a version message then exit
       -v, --verbose[=level]            - Set the verbosity level
       -d, --debug[=level]              - Set the debug level
 
       -n, --name=name                  - Provide a name
       -m, --minimum=minval             - Ignore everything below minimum
       -s, --syslog[=facility.priority] - Send client's output to syslog
                                          (defaults to local0.debug)
       -r, --reverse                    - Reverse direction
       -c, --config=path                - Specify the configuration file

 $ example a b c # to stdout
 av[1] = "a"
 av[2] = "b"
 av[3] = "c"

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<getopt_long(3)|getopt_long(3)>,
L<err(3)|err(3)>,
L<msg(3)|msg(3)>,
L<opt(3)|opt(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<thread(3)|thread(3)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

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

	memset(buf, nul, BUFSIZ);
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

int intvar_a;
int intvar_b;
void nonefunc_b(void) { ++intvar_b; }
int intvar_c;
int intvar_d;
void intfunc_d(int arg) { intvar_d = arg; }
const char *strvar_e;
const char *strvar_f;
void strfunc_f(const char *arg) { strvar_f = arg; }
int optintvar_g;
int optintvar_h;
void optintfunc_h(int *arg) { if (arg) optintvar_h = *arg; else ++optintvar_h; }
const char *optstrvar_i;
const char *optstrvar_j;
void optstrfunc_j(const char *arg) { if (arg) optstrvar_j = arg; }

static Option optab[] =
{
	{ "aaa", 'a', NULL,  "no-arg/var option", no_argument, OPT_NONE, OPT_VARIABLE, &intvar_a },
	{ "bbb", 'b', NULL,  "no-arg/func option", no_argument, OPT_NONE, OPT_FUNCTION, (void *)nonefunc_b },
	{ "ccc", nul, "int", "int-arg/var option", required_argument, OPT_INTEGER, OPT_VARIABLE, &intvar_c },
	{ "ddd", 'd', "int", "int-arg/func option", required_argument, OPT_INTEGER, OPT_FUNCTION, (void *)intfunc_d },
	{ "eee", 'e', "str", "str-arg/var option", required_argument, OPT_STRING, OPT_VARIABLE, &strvar_e },
	{ "fff", nul, "str", "str-arg/func option", required_argument, OPT_STRING, OPT_FUNCTION, (void *)strfunc_f },
	{ "ggg", 'g', "int", "opt-int-arg/var option", optional_argument, OPT_INTEGER, OPT_VARIABLE, &optintvar_g },
	{ "hhh", 'h', "int", "opt-int-arg/func option", optional_argument, OPT_INTEGER, OPT_FUNCTION, (void *)optintfunc_h },
	{ "iii", 'i', "str", "opt-str-arg/var option", optional_argument, OPT_STRING, OPT_VARIABLE, &optstrvar_i },
	{ "jjj", 'j', "str", "opt-str-arg/func option with one of those really, really, really, long descriptions that goes on and on and even contains a really long url: http://www.zip.com.au/~joe/fairly/long/url/index.html would you believe? Here it is again! http://www.zip.com.au/~joe/fairly/long/url/index.html#just_kidding", optional_argument, OPT_STRING, OPT_FUNCTION, (void *)optstrfunc_j },
	{ NULL, nul, NULL, NULL, 0, 0, 0, NULL }
};

static Options options[1] = {{ NULL, optab }};

int main(int ac, char **av)
{
	int oargc = 26;
	char *oargv[] = /* Note: optstrvar with no argument (i.e. -i) does nothing  */
	{
		"test.opt", "-ab", "--aaa", "--bbb", "--ccc", "42",
		"-d", "37", "--ddd=51", "-e", "eee", "--eee", "123",
		"--fff", "fff", "--ggg=4", "-g8", "-h3", "-h", "--hhh",
		"-ifish", "--iii=carp", "-i", "--jjj=jjj",
		"remaining", "arguments", NULL
	};
	char buf[BUFSIZ];
	int rc;
	const char * const usage =
		"\n"
		"      -a, --aaa       - no-arg/var option\n"
		"      -b, --bbb       - no-arg/func option\n"
		"          --ccc=int   - int-arg/var option\n"
		"      -d, --ddd=int   - int-arg/func option\n"
		"      -e, --eee=str   - str-arg/var option\n"
		"          --fff=str   - str-arg/func option\n"
		"      -g, --ggg[=int] - opt-int-arg/var option\n"
		"      -h, --hhh[=int] - opt-int-arg/func option\n"
		"      -i, --iii[=str] - opt-str-arg/var option\n"
		"      -j, --jjj[=str] - opt-str-arg/func option with one of those really,\n"
		"                        really, really, long descriptions that goes on and on\n"
		"                        and even contains a really long url:\n"
		"                        http://www.zip.com.au/~joe/fairly/long/url/index.html\n"
		"                        would you believe? Here it is again!\n"
		"                        http://www.zip.com.au/~joe/fairly/long/url/index.html#just_kidding\n";

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
			"      -h, --help            - Print a help message then exit\n"
			"      -V, --version         - Print a version message then exit\n"
			"      -v, --verbose[=level] - Set the verbosity level\n"
			"      -d, --debug[=level]   - Set the debug level\n"
			"\n"
			"This program tests the prog module.\n"
			"\n"
			"Name: %s\n"
			"Version: 1.0\n"
			"Date: 20010215\n"
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
			"      -h, --help            - Print a help message then exit\n"
			"      -V, --version         - Print a version message then exit\n"
			"      -v, --verbose[=level] - Set the verbosity level\n"
			"      -d, --debug[=level]   - Set the debug level\n"
		}
	};

	int errors = 0;
	int i;

	printf("Testing: prog\n");

	rc = opt_process(oargc, oargv, options);
	if (rc != 24)
		++errors, printf("Test1: rc = %d (not 24, fail)\n", rc);
	if (intvar_a != 2)
		++errors, printf("Test2: intvar_a = %d (not 2, fail)\n", intvar_a);
	if (intvar_b != 2)
		++errors, printf("Test3: intvar_b = %d (not 2, fail)\n", intvar_b);
	if (intvar_c != 42)
		++errors, printf("Test4: intvar_c = %d (not 42, fail)\n", intvar_c);
	if (intvar_d != 51)
		++errors, printf("Test5: intvar_d = %d (not 51, fail)\n", intvar_d);
	if (strcmp(strvar_e ? strvar_e : "NULL", "123"))
		++errors, printf("Test6: strvar_e = '%s' (not '123', fail)\n", strvar_e ? strvar_e : "NULL");
	if (strcmp(strvar_f ? strvar_f : "NULL", "fff"))
		++errors, printf("Test7: strvar_f = '%s' (not 'fff', fail)\n", strvar_f ? strvar_f : "NULL");
	if (optintvar_g != 8)
		++errors, printf("Test8: optintvar_g = %d (not 8, fail)\n", optintvar_g);
	if (optintvar_h != 5)
		++errors, printf("Test9: optintvar_h = %d (not 5, fail)\n", optintvar_h);
	if (!optstrvar_i)
		optstrvar_i = "NULL";
	if (strcmp(optstrvar_i, "carp"))
		++errors, printf("Test10: optstrvar_i = '%s' (not 'carp', fail)\n", optstrvar_i);
	if (!optstrvar_j)
		optstrvar_j = "NULL";
	if (strcmp(optstrvar_j, "jjj"))
		++errors, printf("Test11: optstrvar_j = '%s' (not 'jjj', fail)\n", optstrvar_j);

	opt_usage(buf, BUFSIZ, options);
	if (strcmp(buf, usage))
		++errors, printf("Test12: opt_usage() produced incorrect output:\nshould be:\n%s\nwas:\n%s\n", usage, buf);

	optind = 0;

	prog_name = prog_basename(*av);
	argv[0][0] = argv[1][0] = argv[2][0] = (char *)prog_name;

	prog_init();
	prog_set_name(prog_name);
	prog_set_syntax("[options]");
	prog_set_options(prog_options_table);
	prog_set_version("1.0");
	prog_set_date("20010215");
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
				++errors, printf("Test%d: failed to perform test - fork() failed (%s)\n", 12 + i + 1, strerror(errno));
				continue;
			}

			default:
			{
				int status;

				if (waitpid(pid, &status, 0) == -1)
				{
					++errors, printf("Test%d: failed to wait for test - waitpid(%d) failed (%s)\n", 12 + i + 1, (int)pid, strerror(errno));
					continue;
				}

				if (WIFSIGNALED(status))
					++errors, printf("Test%d: failed: received signal %d\n", 12 + i + 1, WTERMSIG(status));

				if (i != 2 && WIFEXITED(status) && WEXITSTATUS(status))
					++errors, printf("Test%d: failed: exit status %d\n", 12 + i + 1, WEXITSTATUS(status));
			}
		}

		errors += verify(12 + i + 1, out_name, results[i][0], prog_name, "stdout");
		errors += verify(12 + i + 1, err_name, results[i][1], prog_name, "stderr");
	}

	if (errors)
		printf("%d/%d tests failed\n", errors, 12 + tests * 2);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
