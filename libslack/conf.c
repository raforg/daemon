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

I<libslack(conf)> - simple configuration file parsing module

=head1 SYNOPSIS

    #include <slack/conf.h>

    typedef void conf_parse_t(void *obj, const char *conf_path, char *line, size_t lineno);

    void conf_skip_spaces(char **in);
    void conf_skip_spaces_backwards(char **in, const char *start);
    int conf_expect_word(char **in, const char *expect);
    int conf_get_word(char **in, char *word, int length);
    void *conf_parse(const char *conf_path, void *obj, conf_parse_t *parse_line);

=head1 DESCRIPTION

This module provides functions for parsing simple configuration files like
the ones you find in C</etc>. The client need only write a function that
parses a single line. This function is passed to I<conf_parse()> which will
call it for every line in the configuration file after stripping out blank
lines, leading and trailing spaces and comments. Comments run from a C<`#'>
character to the end of the line. Line continuation is performed when a line
ends with a C<`\'> character (there may also be whitespace and a comment
after the C<`\'>).

=over 4

=cut

*/

#include "std.h"

#include "conf.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

/*

=item C<void conf_skip_spaces(char **in)>

Moves C<*in> forwards past whitespace.

=cut

*/

void conf_skip_spaces(char **in)
{
	while (isspace((int)**in))
		++*in;
}

/*

=item C<void conf_skip_spaces_backwards(char **in, const char *start)>

Moves C<*in> backwards past whitespace but not past C<start>.

=cut

*/

void conf_skip_spaces_backwards(char **in, const char *start)
{
	while (*in > start && isspace((int)(*in)[-1]))
		--*in;
}

/*

=item C<int conf_expect_word(char **in, const char *expect)>

Moves C<*in> forwards past C<expect> if C<expect> is found at C<*in> (after
skipping whitespace). If C<expect> is found, returns 0. Otherwise returns -1.

=cut

*/

int conf_expect_word(char **in, const char *expect)
{
	size_t length = strlen(expect);

	conf_skip_spaces(in);

	if (strncmp(*in, expect, length))
		return -1;

	*in += length;

	return 0;
}

/*

=item C<int conf_get_word(char **in, char *word, int length)>

Copies the word at C<*in> (after skipping whitespace) into C<word>. No more
than C<length> bytes are copied including the C<nul> byte. If a word was
found that fit into C<word>, returns 0. Otherwise returns -1.

=cut

*/

int conf_get_word(char **in, char *word, int length)
{
	char *initial = word;

	conf_skip_spaces(in);

	while (**in && !isspace((int)**in) && --length)
		*word++ = *(*in)++;

	if ((word == initial) || (!length && in[1] && !isspace((int)in[1])))
		return -1;

	*word = '\0';

	return 0;
}

/*

=item C<void *conf_parse(const char *conf_path, void *obj, conf_parse_t *parse_line)>

Parses the text configuration file named C<conf_path>. Blank lines are
ignored. Comments (C<`#'> to end of line) are ignored. Lines that end with
C<`\'> are joined with the following line. There may be whitespace and even
a comment after the C<`\'> character but nothing else. The C<parse_line>
function is called with the client supplied C<obj>, the file name, the line
and the line number as arguments. On success, returns C<obj>. On errors,
returns C<NULL> (i.e. if the configuration file could not be read).

=cut

*/

void *conf_parse(const char *conf_path, void *obj, conf_parse_t *parse_line)
{
	FILE *conf;
	char line[BUFSIZ];
	char buf[BUFSIZ];
	int lineno;
	int rc;

	if (!(conf = fopen(conf_path, "r")))
		return NULL;

	line[0] = '\0';
	for (lineno = 1; fgets(buf, BUFSIZ, conf); ++lineno)
	{
		char *start = buf;
		char *end;
		size_t length;
		int cont_line;

		/* Skip leading spaces */

		if (line[0] == '\0')
			conf_skip_spaces(&start);

		/* Strip trailing comments */

		end = strchr(start, '#');
		if (end)
			*end = '\0';
		else
			end = start + strlen(start);

		/* Skip trailing spaces (allows comments after line continuation) */

		conf_skip_spaces_backwards(&end, start);

		/* Skip empty lines */

		if (*start == '\0' || start == end)
			continue;

		/* Perform line continuation */

		cont_line = (end[-1] == '\\');
		if (cont_line)
			--end;

		length = strlen(line);
		rc = snprintf(line + length, BUFSIZ - length, "%*.*s", end - start, end - start, start);
		if (rc == -1 || rc >= BUFSIZ - length)
			return NULL;

		if (cont_line)
			continue;

		/* Parse the resulting line */

		parse_line(obj, conf_path, line, lineno);
		line[0] = '\0';
	}

	fclose(conf);

	return obj;
}

/*

=back

=head1 EXAMPLE

    #include <stdlib.h>
    #include <stdio.h>
    #include <slack/conf.h>

    void fstab_parser(void *obj, const char *conf_path, char *line, size_t lineno)
    {
        char device[64], mount[64], fstype[64], opts[64];
        int freq, passno;

        if (sscanf(line, "%s %s %s %s %d %d", device, mount, fstype, opts, &freq, &passno) != 6)
            fprintf(stderr, "Syntax error in %s (line %d): %s\n", conf_path, lineno, line);
        else
            printf("%s %s %s %s %d %d\n", device, mount, fstype, opts, freq, passno);
    }

    int main(int ac, char **av)
    {
        conf_parse("/etc/fstab", NULL, fstab_parser);
        return 0;
    }

=head1 BUGS

This module is crap and may eventually be replaced by a real lexical analyser.

=head1 SEE ALSO

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
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<str(3)|str(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

typedef struct Data1 Data1;
typedef struct Pair1 Pair1;

struct Data1
{
	int i;
	Pair1 *pair;
};

struct Pair1
{
	const char *service;
	const char *port;
};

static Pair1 pairs[] =
{
	{ "echo", "7/tcp" },
	{ "echo", "7/udp" },
	{ "ftp", "21/tcp" },
	{ "ssh", "22/tcp" },
	{ "smtp", "25/tcp" },
	{ NULL, NULL }
};

static const int final_pair = 5;

static Data1 data1[1] = {{ 0, pairs }};

typedef struct Data2 Data2;

struct Data2
{
	int i;
	int j;
	const char *text;
	const char *results[3][8];
};

static Data2 data2[1] =
{
	{
		0,
		0,
		"\n"
		"# This is a comment\n"
		"\n"
		"line1 = word1 word2\n"
		"line2 = word3 \\\n"
		"\tword4 word5 \\ # a comment in a funny place\n"
		"\tword6 word7\n"
		"\n"
		"line3 = \\\n"
		"\tword8\n"
		"\n",
		{
			{ "line1", "=", "word1", "word2", NULL, NULL, NULL, NULL },
			{ "line2", "=", "word3", "word4", "word5", "word6", "word7", NULL },
			{ "line3", "=", "word8", NULL, NULL, NULL, NULL, NULL }
		}
	}
};

static const int final_line = 3;
static const int final_word = 3;

static int errors = 0;

static int create_test1(const char *name)
{
	FILE *out = fopen(name, "w");
	int i;

	if (!out)
	{
		++errors, printf("Test1: failed to create file: '%s'\n", name);
		return 0;
	}

	for (i = 0; data1->pair[i].service; ++i)
		fprintf(out, "%s %s\n", data1->pair[i].service, data1->pair[i].port);

	fclose(out);
	return 1;
}

static void parse_test1(void *obj, const char *conf_path, char *line, size_t lineno)
{
	Data1 *data1 = (Data1 *)obj;
	char *p = line;
	char service[BUFSIZ];
	char port[BUFSIZ];

	if (conf_get_word(&p, service, BUFSIZ) == -1)
		++errors, printf("Test1: failed to get a word: '%s' (file %s line %d)\n", p, conf_path, lineno);
	else if (conf_get_word(&p, port, BUFSIZ) == -1)
		++errors, printf("Test1: failed to get a word: '%s' (files %s line %d)\n", p, conf_path, lineno);
	else if (strcmp(service, data1->pair[data1->i].service))
		++errors, printf("Test1: expected service '%s', received '%s' (file %s line %d)\n", data1->pair[data1->i].service, service, conf_path, lineno);
	else if (strcmp(port, data1->pair[data1->i].port))
		++errors, printf("Test1: expected port '%s', received '%s' (file %s line %d)\n", data1->pair[data1->i].port, port, conf_path, lineno);
	++data1->i;
}

static int create_test2(const char *name)
{
	FILE *out = fopen(name, "w");

	if (!out)
	{
		++errors, printf("Test2: failed to create file: '%s'\n", name);
		return 0;
	}

	fprintf(out, "%s", data2->text);
	fclose(out);
	return 1;
}

static void parse_test2(void *obj, const char *conf_path, char *line, size_t lineno)
{
	Data2 *data2 = (Data2 *)obj;
	char *p = line;
	char word[BUFSIZ];

	for (data2->j = 0; conf_get_word(&p, word, BUFSIZ) != -1; ++data2->j)
	{
		if (data2->results[data2->i][data2->j] && strcmp(word, data2->results[data2->i][data2->j]))
		{
			++errors;
			printf("Test2: expected '%s', received '%s' (file %s line %d)\n", data2->results[data2->i][data2->j], word, conf_path, lineno);
			break;
		}
	}

	++data2->i;
}

int main(int ac, char **av)
{
	const char * const name = "conf.testfile";

	int errors_save;

	printf("Testing: conf\n");

	if (create_test1(name))
	{
		errors_save = errors;
		conf_parse(name, data1, parse_test1);
		if (errors == errors_save && data1->i != final_pair)
			++errors, printf("Test1: failed to parse entire conf file\n");
		unlink(name);
	}

	if (create_test2(name))
	{
		errors_save = errors;
		conf_parse(name, data2, parse_test2);
		if (errors == errors_save && (data2->i != final_line || data2->j != final_word))
			++errors, printf("Test2: failed to parse entire conf file\n");
		unlink(name);
	}

	if (errors)
		printf("%d/2 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
