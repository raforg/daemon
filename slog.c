/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdio.h>
#include <string.h>

#include <syslog.h>

typedef struct syslog_map_t syslog_map_t;

struct syslog_map_t
{
	char *name;
	int val;
};

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
** int syslog_lookup(const syslog_map_t *map, const char *name)
**
** Looks for name in map. Returns the index into map where name is found.
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
** const char *syslog_lookup_str(const syslog_map_t *map, int spec)
**
** Looks for spec (a facility or priority) in map. Returns the name that
** matches spec.
*/

static const char *syslog_lookup_str(const syslog_map_t *map, int spec)
{
	int i;

	for (i = 0; map[i].name; ++i)
		if ((spec & map[i].val) == map[i].val)
			break;

	return map[i].name;
}

/*
** int syslog_lookup_facility(const char *facility)
**
** Returns the constant corresponding to the string facility.
*/

int syslog_lookup_facility(const char *facility)
{
	return syslog_lookup(syslog_facility_map, facility);
}

/*
** int syslog_lookup_priority(const char *priority)
**
** Returns the constant corresponding to the string priority.
*/

int syslog_lookup_priority(const char *priority)
{
	return syslog_lookup(syslog_priority_map, priority);
}

/*
** const char *syslog_facility_str(int spec)
**
** Returns the name corresponding to the facility spec.
*/

const char *syslog_facility_str(int spec)
{
	return syslog_lookup_str(syslog_facility_map, spec);
}

/*
** const char *syslog_priority_str(int spec)
**
** Returns the name corresponding to the priority spec.
*/

const char *syslog_priority_str(int spec)
{
	return syslog_lookup_str(syslog_priority_map, spec);
}

/*
** int syslog_parse(const char *spec, int *facility, int *priority)
**
** Parses spec as a "facility.priority" string. If facility is non-null,
** the parsed facility is stored in the location stored there. If priority
** is non-null the parsed priority is stored in the location stored there.
** Returns 0 on success, -1 on error.
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

#ifdef TEST

int main(int ac, char **av)
{
	int i;
	int j;
	int errors = 0;
	int tests = 0;
	int rc;

	printf("Testing: %s\n", *av);

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
		++tests;

		if (!syslog_facility_str(syslog_facility_map[i].val))
			++errors, printf("Test%d: syslog_facility_str(%d) failed\n", tests, syslog_facility_map[i].val);
	}

	for (i = 0; syslog_priority_map[i].name; ++i)
	{
		++tests;

		if (!syslog_priority_str(syslog_priority_map[i].val))
			++errors, printf("Test%d: syslog_priority_str(%d) failed\n", tests, syslog_priority_map[i].val);
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

	exit(0);
}

#endif

/* vi:set ts=4 sw=4: */
