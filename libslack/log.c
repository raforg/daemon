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

I<libslack(log)> - syslog helper module

=head1 SYNOPSIS

    #include <slack/log.h>

    int syslog_lookup_facility(const char *facility);
    int syslog_lookup_priority(const char *priority);
    const char *syslog_facility_str(int spec);
    const char *syslog_priority_str(int spec);
    int syslog_parse(const char *spec, int *facility, int *priority);

=head1 DESCRIPTION

This module provides functions for parsing I<syslog> targets, converting
between I<syslog> facility names and codes, and converting between I<syslog>
priority names and codes.

    syslog facilities          syslog priorities
    ----------------------     -----------------------
    kern       LOG_KERN        emerg       LOG_EMERG
    user       LOG_USER        alert       LOG_ALERT
    mail       LOG_MAIL        crit        LOG_CRIT
    daemon     LOG_DAEMON      err         LOG_ERR
    auth       LOG_AUTH        warning     LOG_WARNIN
    syslog     LOG_SYSLOG      info        LOG_INFO
    lpr        LOG_LPR         debug       LOG_DEBUG
    news       LOG_NEWS
    uucp       LOG_UUCP
    cron       LOG_CRON
    local0     LOG_LOCAL0
    local1     LOG_LOCAL1
    local2     LOG_LOCAL2
    local3     LOG_LOCAL3
    local4     LOG_LOCAL4
    local5     LOG_LOCAL5
    local6     LOG_LOCAL6
    local7     LOG_LOCAL7

=over 4

=cut

*/

#include "std.h"

#include <syslog.h>

typedef struct syslog_map_t syslog_map_t;

struct syslog_map_t
{
	char *name;
	int val;
};

/*
** The following masks may be wrong on some systems.
*/

#ifndef LOG_PRIMASK
#define LOG_PRIMASK 0x0007
#endif

#ifndef LOG_FACMASK
#define LOG_FACMASK 0x03f8
#endif

static syslog_map_t syslog_facility_map[] =
{
	{ "kern",   LOG_KERN },
	{ "user",   LOG_USER },
	{ "mail",   LOG_MAIL },
	{ "daemon", LOG_DAEMON },
	{ "auth",   LOG_AUTH },
	{ "syslog", LOG_SYSLOG },
	{ "lpr",    LOG_LPR },
	{ "news",   LOG_NEWS },
	{ "uucp",   LOG_UUCP },
	{ "cron",   LOG_CRON },
	{ "local0", LOG_LOCAL0 },
	{ "local1", LOG_LOCAL1 },
	{ "local2", LOG_LOCAL2 },
	{ "local3", LOG_LOCAL3 },
	{ "local4", LOG_LOCAL4 },
	{ "local5", LOG_LOCAL5 },
	{ "local6", LOG_LOCAL6 },
	{ "local7", LOG_LOCAL7 },
	{ NULL,     -1 }
};

static syslog_map_t syslog_priority_map[] =
{
	{ "emerg",   LOG_EMERG },
	{ "alert",   LOG_ALERT },
	{ "crit",    LOG_CRIT },
	{ "err",     LOG_ERR },
	{ "warning", LOG_WARNING },
	{ "info",    LOG_INFO },
	{ "debug",   LOG_DEBUG },
	{ NULL,      -1 }
};

/*

C<int syslog_lookup(const syslog_map_t *map, const char *name)>

Looks for C<name> (a facility or priority name) in C<map>. If found, returns
its corresponding code. If not found, returns -1.

*/

static int syslog_lookup(const syslog_map_t *map, const char *name)
{
	int i;

	for (i = 0; map[i].name; ++i)
		if (!strcmp(name, map[i].name))
			break;

	return map[i].val;
}

/*

C<const char *syslog_lookup_str(const syslog_map_t *map, int spec)>

Looks for C<spec> (a facility or priority code) in C<map>. If found, returns
its corresponding name. If not found, returns C<NULL>.

*/

static const char *syslog_lookup_str(const syslog_map_t *map, int spec, int mask)
{
	int i;

	for (i = 0; map[i].name; ++i)
		if ((spec & mask) == map[i].val)
			break;

	return map[i].name;
}

/*

=item C<int syslog_lookup_facility(const char *facility)>

Returns the code corresponding to C<facility>.

=cut

*/

int syslog_lookup_facility(const char *facility)
{
	return syslog_lookup(syslog_facility_map, facility);
}

/*

=item C<int syslog_lookup_priority(const char *priority)>

Returns the code corresponding to C<priority>.

=cut

*/

int syslog_lookup_priority(const char *priority)
{
	return syslog_lookup(syslog_priority_map, priority);
}

/*

=item C<const char *syslog_facility_str(int spec)>

Returns the name corresponding to the facility part of C<spec>.

=cut

*/

const char *syslog_facility_str(int spec)
{
	return syslog_lookup_str(syslog_facility_map, spec, LOG_FACMASK);
}

/*

=item C<const char *syslog_priority_str(int spec)>

Returns the name corresponding to the priority part of C<spec>.

=cut

*/

const char *syslog_priority_str(int spec)
{
	return syslog_lookup_str(syslog_priority_map, spec, LOG_PRIMASK);
}

/*

=item C<int syslog_parse(const char *spec, int *facility, int *priority)>

Parses C<spec> as a I<facility.priority> string. If C<facility> is
non-C<NULL>, the parsed facility is stored in the location pointed to by
C<facility>. If C<priority> is non-C<NULL> the parsed priority is stored in
the location pointed to by C<priority>. On success, returns 0. On error,
returns -1.

=cut

*/

int syslog_parse(const char *spec, int *facility, int *priority)
{
	char fac[256], *pri;
	int f, p;

	if (!spec)
		return -1;

	strncpy(fac, spec, 255);
	fac[255] = '\0';

	if (!(pri = strchr(fac, '.')))
		return -1;

	*pri++ = '\0';

	if ((f = syslog_lookup_facility(fac)) == -1)
		return -1;

	if ((p = syslog_lookup_priority(pri)) == -1)
		return -1;

	if (facility)
		*facility = f;

	if (priority)
		*priority = p;

	return 0;
}

/*

=back

=head1 EXAMPLE

    #include <syslog.h>
    #include <slack/log.h>

    int main(int ac, char **av)
    {
        int facility, priority;
        if (syslog_parse(av[1], &facility, &priority) != -1)
            syslog(facility | priority, "syslog(%s)", av[1]);
        return 0;
    }

=head1 SEE ALSO

L<syslog(3)|syslog(3)>,
L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
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

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

int main(int ac, char **av)
{
	int i;
	int j;
	int errors = 0;
	int tests = 0;
	int rc;

	printf("Testing: log\n");

	for (i = 0; syslog_facility_map[i].name; ++i)
	{
		for (j = 0; syslog_priority_map[j].name; ++j)
		{
			char buf[64];
			int facility = 0;
			int priority = 0;

			snprintf(buf, 64, "%s.%s", syslog_facility_map[i].name, syslog_priority_map[j].name);
			++tests;

			rc = syslog_parse(buf, &facility, &priority);
			if (rc == -1)
				++errors, printf("Test%d: syslog_parse(%s) failed\n", tests, buf);
			else if (facility != syslog_facility_map[i].val)
				++errors, printf("Test%d: syslog_parse(%s) failed: facility = %d (not %d)\n", tests, buf, facility, syslog_facility_map[i].val);
			else if (priority != syslog_priority_map[j].val)
				++errors, printf("Test%d: syslog_parse(%s) failed: priority = %d (not %d)\n", tests, buf, priority, syslog_priority_map[j].val);
		}
	}

	for (i = 0; syslog_facility_map[i].name; ++i)
	{
		const char *fac = syslog_facility_str(syslog_facility_map[i].val);

		++tests;
		if (fac != syslog_facility_map[i].name)
			++errors, printf("Test%d: syslog_facility_str(%d) failed: %s (not %s)\n", tests, syslog_facility_map[i].val, fac, syslog_facility_map[i].name);
	}

	for (i = 0; syslog_priority_map[i].name; ++i)
	{
		const char *pri = syslog_priority_str(syslog_priority_map[i].val);

		++tests;
		if (pri != syslog_priority_map[i].name)
			++errors, printf("Test%d: syslog_priority_str(%d) failed: %s (not %s)\n", tests, syslog_priority_map[i].val, pri, syslog_priority_map[i].name);
	}

	++tests;
	if (syslog_parse(NULL, NULL, NULL) != -1)
		++errors, printf("Test%d: syslog_parse(null) failed\n", tests);

	++tests;
	if (syslog_parse("gibberish", NULL, NULL) != -1)
		++errors, printf("Test%d: syslog_parse(\"gibberish\") failed\n", tests);

	if (errors)
		printf("%d/%d tests failed\n", errors, tests);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
