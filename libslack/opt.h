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

#ifndef LIBSLACK_OPT_H
#define LIBSLACK_OPT_H

#include <stdlib.h>

#include <slack/hdr.h>

#ifdef NEEDS_GETOPT
#include "getopt.h"
#else
#include <getopt.h>
#endif

typedef struct option option;
typedef struct Option Option;
typedef struct Options Options;

typedef void (*opt_action_int_t)(int);
typedef void (*opt_action_string_t)(const char *);
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

__START_DECLS
int opt_process __PROTO ((int argc, char **argv, Options *options));
char *opt_usage __PROTO ((char *buf, size_t size, Options *options));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
