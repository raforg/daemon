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

#ifndef LIBSLACK_PROG_H
#define LIBSLACK_PROG_H

#include <stdlib.h>

#include <slack/hdr.h>
#include <slack/msg.h>

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif

#ifndef HAVE_GETOPT_LONG
#include <slack/getopt.h>
#else
#include <getopt.h>
#endif

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

_begin_decls
void prog_init _args ((void));
const char *prog_set_name _args ((const char *name));
Options *prog_set_options _args ((Options *options));
const char *prog_set_syntax _args ((const char *syntax));
const char *prog_set_desc _args ((const char *desc));
const char *prog_set_version _args ((const char *version));
const char *prog_set_date _args ((const char *date));
const char *prog_set_author _args ((const char *author));
const char *prog_set_contact _args ((const char *contact));
const char *prog_set_vendor _args ((const char *vendor));
const char *prog_set_url _args ((const char *url));
const char *prog_set_legal _args ((const char *legal));
Msg *prog_set_out _args ((Msg *out));
Msg *prog_set_err _args ((Msg *err));
Msg *prog_set_dbg _args ((Msg *dbg));
Msg *prog_set_alert _args ((Msg *alert));
ssize_t prog_set_debug_level _args ((size_t debug_level));
ssize_t prog_set_verbosity_level _args ((size_t verbosity_level));
int prog_set_locker _args ((Locker *locker));
const char *prog_name _args ((void));
const Options *prog_options _args ((void));
const char *prog_syntax _args ((void));
const char *prog_desc _args ((void));
const char *prog_version _args ((void));
const char *prog_date _args ((void));
const char *prog_author _args ((void));
const char *prog_contact _args ((void));
const char *prog_vendor _args ((void));
const char *prog_url _args ((void));
const char *prog_legal _args ((void));
Msg *prog_out _args ((void));
Msg *prog_err _args ((void));
Msg *prog_dbg _args ((void));
Msg *prog_alert _args ((void));
size_t prog_debug_level _args ((void));
size_t prog_verbosity_level _args ((void));
int prog_out_fd _args ((int fd));
int prog_out_stdout _args ((void));
int prog_out_file _args ((const char *path));
int prog_out_syslog _args ((const char *ident, int option, int facility, int priority));
int prog_out_none _args ((void));
int prog_err_fd _args ((int fd));
int prog_err_stderr _args ((void));
int prog_err_file _args ((const char *path));
int prog_err_syslog _args ((const char *ident, int option, int facility, int priority));
int prog_err_none _args ((void));
int prog_dbg_fd _args ((int fd));
int prog_dbg_stdout _args ((void));
int prog_dbg_stderr _args ((void));
int prog_dbg_file _args ((const char *path));
int prog_dbg_syslog _args ((const char *id, int option, int facility, int priority));
int prog_dbg_none _args ((void));
int prog_alert_fd _args ((int fd));
int prog_alert_stdout _args ((void));
int prog_alert_stderr _args ((void));
int prog_alert_file _args ((const char *path));
int prog_alert_syslog _args ((const char *id, int option, int facility, int priority));
int prog_alert_none _args ((void));
int prog_opt_process _args ((int ac, char **av));
void prog_usage_msg _args ((const char *fmt, ...));
void prog_help_msg _args ((void));
void prog_version_msg _args ((void));
const char *prog_basename _args ((const char *path));
extern Options prog_options_table[1];
int opt_process _args ((int argc, char **argv, Options *options, char *msgbuf, size_t bufsize));
char *opt_usage _args ((char *buf, size_t size, Options *options));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
