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

I<libprog(opt)> - command line option handling module

=head1 SYNOPSIS

    #include <prog/opt.h>

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
        size_t length;
        Option *options;
    };

    int opt_process(int argc, char **argv, Options *options);
    char *opt_usage(char *buf, size_t size, Options *options);

=head1 DESCRIPTION

The opt module is an alternate interface to I<GNU getopt_long_only(3)>. It
defines a way to specify command line option syntax, semantics and
explanations in multiple, discrete chunks. The I<getopt> functions require
that the client specify the syntax and partial semantics for all options in
the same place (if it is to be done statically). This can be annoying when
library modules require their own command line options. This module allows
various parts of a program to (statically) specify their own command line
options independently and link them together via I<parent> pointers.

Option syntax is specified in much the same way as for I<GNU
getopt_long_only(3)>. Option semantics are specified by an action
(C<OPT_NOTHING>, C<OPT_VARIABLE> or C<OPT_FUNCTION>), an argument type
(C<OPT_NONE>, C<OPT_INTEGER> or C<OPT_STRING>) and an object (C<int *>,
C<char **>, C<func()>, C<func(int)> or C<func(char *)>).

This module is used by the I<prog> module and needn't be used directly. Use
I<prog_opt_process(3)>, I<prog_usage_msg(3)> and I<prog_help_msg()> instead.

=over 4

=cut

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "opt.h"
#include "mem.h"

/*

C<opt_convert(Options *options)>

Creates and returns a flat table of option structs from C<options>. The
resulting array is for use with I<GNU getopt_long_only(3)>. The memory
returned must be I<free()>d by the caller.

*/

static option *opt_convert(Options *options)
{
	Options *opts;
	size_t size = 0;
	option *ret;
	int index = 0;

	for (opts = options; opts; opts = opts->parent)
		for (opts->length = 0; opts->options[opts->length].name; ++opts->length)
			++size;

	if (!(ret = mem_create(size + 1, option)))
		return NULL;

	for (opts = options; opts; opts = opts->parent)
	{
		int i;

		for (i = 0; i < opts->length; ++i, ++index)
		{
			ret[index].name = opts->options[i].name;
			ret[index].has_arg = opts->options[i].has_arg;
			ret[index].flag = NULL;
			ret[index].val = index << 8; // avoid ascii
		}
	}

	memset(ret + index, '\0', sizeof(option));

	return ret;
}

/*

C<void opt_action(Options *options, int index, const char *argument)>

Performs the action associated with the C<index>'th option in C<options> (as
supplied to I<getopt_long_only(3)>). C<argument> is a pointer to an C<int>
or C<char *>.

If C<argument> is non-C<NULL>:

    arg_action    arg_type     action
    ~~~~~~~~~~~~  ~~~~~~~~~~~  ~~~~~~
    OPT_VARIABLE  OPT_INTEGER  *object = atoi(argument)
    OPT_VARIABLE  OPT_STRING   *object = argument
    OPT_FUNCTION  OPT_INTEGER  object(atoi(argument))
    OPT_FUNCTION  OPT_STRING   object(argument)

Otherwise:

    arg_action    action
    ~~~~~~~~~~~~  ~~~~~~
    OPT_VARIABLE  ++*object
    OPT_FUNCTION  object()

*/

static void opt_action(Options *options, int index, const char *argument)
{
	Option *option;
	int i = 0;

	for (; index && options; options = options->parent)
	{
		if (index >= options->length)
		{
			index -= options->length;
			continue;
		}

		for (; index && options->options[i].name; ++i, --index)
		{}

		if (index == 0)
			break;
	}

	if (!options)
		return;

	option = options->options + i;

	if (argument && option->has_arg != no_argument)
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
						((opt_action_int_t)option->object)(atoi(argument));
						break;
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
				++*(int *)option->object;
				break;

			case OPT_FUNCTION:
				((opt_action_none_t)option->object)();
				break;
		}
	}
}

/*

=item C<int opt_process(int argc, char **argv, Options *options)>

Parses C<argv> for options specified in C<options>. Uses I<GNU
getopt_long_only(3)>. As each option is encountered, its corresponding
action is performed. Returns C<optind> on success, or -1 on error (invalid
option).

The following tables show the actions that are applied to an option's
C<object> based on its C<arg_action> and C<arg_type> attributes and whether
or not an argument is present.

If C<argument> is non-C<NULL>:

    arg_action    arg_type     action
    ============  ===========  ========================
    OPT_VARIABLE  OPT_INTEGER  *object = atoi(argument)
    OPT_VARIABLE  OPT_STRING   *object = argument
    OPT_FUNCTION  OPT_INTEGER  object(atoi(argument))
    OPT_FUNCTION  OPT_STRING   object(argument)

Otherwise:

    arg_action    action
    ============  =========
    OPT_VARIABLE  ++*object
    OPT_FUNCTION  object()

=cut

*/

int opt_process(int argc, char **argv, Options *options)
{
	option *long_options = opt_convert(options);
	if (!long_options)
		return optind;

	for (;;)
	{
		int val = getopt_long_only(argc, argv, "", long_options, NULL);
		if (val == EOF)
			break;

		if (val == '?' || val == ':')
		{
			mem_release(long_options);
			return -1;
		}

		opt_action(options, val >> 8, optarg);
	}

	mem_release(long_options);

	return optind;
}

/*

=item C<char *opt_usage(char *buf, size_t size, Options *options)>

Writes a usage message into C<buf> that displays the names, syntax and
explanations of all options in C<options>. C<options> is traversed depth
first so the chunk with the C<NULL> I<parent> appears first. Each chunk of
options is preceeded by a blank line. No more than C<size> bytes are
written, including the terminating C<nul> character. The string returned
will look like:

        -a       -- no-arg/var option
        -b       -- no-arg/func option
        -c=arg   -- int-arg/var option
        -d=arg   -- int-arg/func option
        -e=arg   -- str-arg/var option
        -f=arg   -- str-arg/func option
        -g[=arg] -- opt-int-arg/var option
        -h[=arg] -- opt-str-arg/func option with one of those really,
                    really, really, long descriptions that goes on and
                    on and even contains a really long url:
                    http://www.zip.com.au/~raf2/lib/software/daemon/index.html
                    would you believe? Here it is again!
                    http://www.zip.com.au/~raf2/lib/software/daemon/index.html#just_kidding

=cut

*/

char *opt_usage(char *buf, size_t size, Options *options)
{
	const int total_width = 72;
	const char * const indent = "        -";
	const size_t indent_width = strlen(indent);
	const char * const leader = " -- ";
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

		for (i = 0; i < opts->length; ++i)
		{
			Option *opt = opts->options + i;
			char help[BUFSIZ];
			const char *desc;
			const char *next = NULL;
			size_t desc_length;
			size_t help_length;

			/* Produce the left hand side: syntax */

			snprintf(help, BUFSIZ, "%s%s", indent, opt->name);
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

				for (; next > desc && !isspace((int)*next); --next)
				{}

				/* If none (word too long), look forward for end of word */

				if (next == desc)
				{
					while (isspace((int)*next))
						++next;

					next = strchr(desc, ' ');

					if (!next)
						next = desc + desc_length;
				}

				/* Ignore any extra whitespace to the left */

				while (next != desc && isspace((int)next[-1]))
					--next;

				/* Add one line of description */

				snprintf(help + help_length, BUFSIZ - help_length, "%*.*s\n", next - desc, next - desc, desc);
				help_length = strlen(help);

				/* Ignore any extra whitespace to the right */

				while (isspace((int)*next))
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

=head1 SEE ALSO

L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<getopt(3)|getopt(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<msg(3)|msg(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>

=head1 AUTHOR

raf <raf2@zip.com.au>

=cut

*/

#ifdef TEST

int intvar_a;
int intvar_b;
void nonefunc_b(void) { intvar_b = 1; }
int intvar_c;
int intvar_d;
void intfunc_d(int arg) { intvar_d = arg; }
const char *strvar_e;
const char *strvar_f;
void strfunc_f(const char *arg) { strvar_f = arg; }
int intvar_g;
const char *strvar_h;
void strfunc_h(const char *arg) { strvar_h = arg; }

static Option optab[] =
{
	{ "a", NULL,  "no-arg/var option", no_argument, OPT_NONE, OPT_VARIABLE, &intvar_a },
	{ "b", NULL,  "no-arg/func option", no_argument, OPT_NONE, OPT_FUNCTION, (void *)nonefunc_b },
	{ "c", "arg", "int-arg/var option", required_argument, OPT_INTEGER, OPT_VARIABLE, &intvar_c },
	{ "d", "arg", "int-arg/func option", required_argument, OPT_INTEGER, OPT_FUNCTION, (void *)intfunc_d },
	{ "e", "arg", "str-arg/var option", required_argument, OPT_STRING, OPT_VARIABLE, &strvar_e },
	{ "f", "arg", "str-arg/func option", required_argument, OPT_STRING, OPT_FUNCTION, (void *)strfunc_f },
	{ "g", "arg", "opt-int-arg/var option", optional_argument, OPT_INTEGER, OPT_VARIABLE, &intvar_g },
	{ "h", "arg", "opt-str-arg/func option with one of those really, really, really, long descriptions that goes on and on and even contains a really long url: http://www.zip.com.au/~raf2/lib/software/daemon/index.html would you believe? Here it is again! http://www.zip.com.au/~raf2/lib/software/daemon/index.html#just_kidding", optional_argument, OPT_STRING, OPT_FUNCTION, (void *)strfunc_h },
	{ NULL, NULL, NULL, 0, 0, 0, NULL }
};

static Options options[1] = {{ NULL, 0, optab }};

int main(int ac, char **av)
{
	int argc = 13;
	char *argv[] = { "test.opt", "-a", "-b", "-c", "42", "-d", "37", "-e", "eee", "-f", "fff", "remaining", "arguments", NULL };
	char buf[BUFSIZ];
	int rc;
	int errors = 0;
	const char * const usage =
		"\n"
		"        -a       -- no-arg/var option\n"
		"        -b       -- no-arg/func option\n"
		"        -c=arg   -- int-arg/var option\n"
		"        -d=arg   -- int-arg/func option\n"
		"        -e=arg   -- str-arg/var option\n"
		"        -f=arg   -- str-arg/func option\n"
		"        -g[=arg] -- opt-int-arg/var option\n"
		"        -h[=arg] -- opt-str-arg/func option with one of those really,\n"
		"                    really, really, long descriptions that goes on and\n"
		"                    on and even contains a really long url:\n"
		"                    http://www.zip.com.au/~raf2/lib/software/daemon/index.html\n"
		"                    would you believe? Here it is again!\n"
		"                    http://www.zip.com.au/~raf2/lib/software/daemon/index.html#just_kidding\n";

	printf("Testing: opt\n");

	rc = opt_process(argc, argv, options);
	if (rc != 11)
		++errors, printf("Test1: rc = %d (not 11, fail)\n", rc);
	if (intvar_a != 1)
		++errors, printf("Test2: intvar_a = %d (not 1, fail)\n", intvar_a);
	if (intvar_b != 1)
		++errors, printf("Test3: intvar_b = %d (not 1, fail)\n", intvar_b);
	if (intvar_c != 42)
		++errors, printf("Test4: intvar_c = %d (not 42, fail)\n", intvar_c);
	if (intvar_d != 37)
		++errors, printf("Test5: intvar_d = %d (not 37, fail)\n", intvar_d);
	if (strcmp(strvar_e, "eee"))
		++errors, printf("Test6: strvar_e = '%s' (not 'eee', fail)\n", strvar_e ? strvar_e : "null");
	if (strcmp(strvar_f, "fff"))
		++errors, printf("Test7: strvar_f = '%s' (not 'fff', fail)\n", strvar_f ? strvar_f : "null");

	opt_usage(buf, BUFSIZ, options);
	if (strcmp(buf, usage))
		++errors, printf("Test8: opt_usage() produced incorrect output:\nshould be:\n%s\nwas:\n%s\n", usage, buf);

	if (errors)
		printf("%d/8 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
