/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdio.h>

#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include <sys/resource.h>

enum name_t
{
	LIMIT_ARG,
	LIMIT_CHILD,
	LIMIT_TICK,
	LIMIT_GROUP,
	LIMIT_OPEN,
	LIMIT_STREAM,
	LIMIT_TZNAME,
	LIMIT_JOB,
	LIMIT_SAVE_IDS,
	LIMIT_VERSION,
	LIMIT_CANON,
	LIMIT_INPUT,
	LIMIT_VDISABLE,
	LIMIT_LINK,
	LIMIT_NAME,
	LIMIT_PATH,
	LIMIT_PIPE,
	LIMIT_NOTRUNC,
	LIMIT_CHOWN,
	LIMIT_COUNT
};

typedef enum name_t name_t;
typedef struct conf_t conf_t;

struct conf_t
{
	int init;      /* has this value been initialised? */
	int idem;      /* is this function idempotent? */
	int name;      /* name argument for sysconf, [f]pathconf */
	long value;    /* limit value */
	long guess;    /* limit to use when indeterminate */
	off_t offset;  /* offset to apply to value when not indeterminate */
};

#ifndef STREAM_MAX
#define STREAM_MAX 0
#endif

#ifndef TZNAME_MAX
#define TZNAME_MAX 0
#endif

#ifndef NAME_MAX
#define NAME_MAX 0
#endif

static conf_t g_limit[] =
{
	{ 0, 1, _SC_ARG_MAX,          ARG_MAX,    1048576, 0 },
	{ 0, 1, _SC_CHILD_MAX,        CHILD_MAX,    32768, 0 },
	{ 0, 1, _SC_CLK_TCK,          0,               -1, 0 },
	{ 0, 1, _SC_NGROUPS_MAX,      NGROUPS_MAX,   1024, 0 },
	{ 0, 1, _SC_OPEN_MAX,         OPEN_MAX,      1024, 0 },
	{ 0, 1, _SC_STREAM_MAX,       STREAM_MAX,    1024, 0 },
	{ 0, 1, _SC_TZNAME_MAX,       TZNAME_MAX,    1024, 0 },
	{ 0, 1, _SC_JOB_CONTROL,      0,               -1, 0 },
	{ 0, 1, _SC_SAVED_IDS,        0,               -1, 0 },
	{ 0, 1, _SC_VERSION,          0,               -1, 0 },
	{ 0, 0, _PC_MAX_CANON,        MAX_CANON,     4096, 0 },
	{ 0, 0, _PC_MAX_INPUT,        MAX_INPUT,     4096, 0 },
	{ 0, 0, _PC_VDISABLE,         0,               -1, 0 },
	{ 0, 0, _PC_LINK_MAX,         LINK_MAX,      1024, 0 },
	{ 0, 0, _PC_NAME_MAX,         NAME_MAX,      1024, 0 },
	{ 0, 0, _PC_PATH_MAX,         PATH_MAX,      4096, 2 },
	{ 0, 0, _PC_PIPE_BUF,         PIPE_BUF,      4096, 0 },
	{ 0, 0, _PC_NO_TRUNC,         0,               -1, 0 },
	{ 0, 0, _PC_CHOWN_RESTRICTED, 0,               -1, 0 }
};

/*
** int limit_needed(int lim)
**
** Determines whether the given limit is known or it needs to be obtained.
** lim is one of name_t and represents the name argument to sysconf(3),
** pathconf(3) or fpathconf(3). Returns 1 if the given limit has been
** initialised and is idempotent. Returns 0 otherwise.
** If LIMIT_USE_DEFINED_VALUES is defined, then limit values that are
** defined in header files are taken into account. They shouldn't be
** used, however, since the values defined in header files can be the
** absolute minima allowed by POSIX.1 which bears little resemblance to
** an actual system.
*/

static int limit_needed(int lim)
{
	return
#ifdef LIMIT_USE_DEFINED_VALUES
		!g_limit[lim].value &&
#endif
		!g_limit[lim].init || !g_limit[lim].idem;
}

/*
** long limit_sysconf(int limit)
**
** Returns system limits using sysconf(3). If the limit has been obtained
** in the past and is idempotent, a cached value is returned rather than
** invoking sysconf(3) again. If the limit is indeterminate, a predetermined
** guess is returned. Whatever happens, a usable value will be returned.
*/

static long limit_sysconf(int limit)
{
	if (limit_needed(limit))
	{
		errno = 0;

		if ((g_limit[limit].value = sysconf(g_limit[limit].name)) == -1)
		{
			if (errno)
				return -1;

			g_limit[limit].value = g_limit[limit].guess;
		}
		else
			g_limit[limit].value += g_limit[limit].offset;

		g_limit[limit].init = 1;
	}

	return g_limit[limit].value;
}

/*
** long limit_pathconf(int limit, const char *path)
**
** Returns system limits using pathconf(3). If the limit has been obtained
** in the past and is idempotent, a cached value is returned rather than
** invoking pathconf(3) again. If the limit is indeterminate, a predetermined
** guess is returned. If the limit is determinate, a predetermined amount may
** be added to its value. This is only needed for _PC_PATH_MAX which is the
** maximum length of a relative path. To be more useful, 2 is added to this
** limit to account for the '/' and '\0' that will be needed to form an
** absolute path. Whatever happens, a usable value will be returned.
*/

static long limit_pathconf(int limit, const char *path)
{
	if (limit_needed(limit))
	{
		errno = 0;

		if ((g_limit[limit].value = pathconf(path, g_limit[limit].name)) == -1)
		{
			if (errno)
				return -1;

			g_limit[limit].value = g_limit[limit].guess;
		}
		else
			g_limit[limit].value += g_limit[limit].offset;

		g_limit[limit].init = 1;
	}

	return g_limit[limit].value;
}

/*
** long limit_fpathconf(int limit, int fd)
**
** Returns system limits using pathconf(3). If the limit has been obtained
** in the past and is idempotent, a cached value is returned rather than
** invoking pathconf(3) again. If the limit is indeterminate, a predetermined
** guess is returned. If the limit is determinate, a predetermined amount may
** be added to its value. This is only needed for _PC_PATH_MAX which is the
** maximum length of a relative path. To be more useful, 2 is added to this
** limit to account for the '/' and '\0' that will be needed to form an
** absolute path. Whatever happens, a usable value will be returned.
*/

static long limit_fpathconf(int limit, int fd)
{
	if (limit_needed(limit))
	{
		errno = 0;

		if ((g_limit[limit].value = fpathconf(fd, g_limit[limit].name)) == -1)
		{
			if (errno)
				return -1;

			g_limit[limit].value = g_limit[limit].guess;
		}
		else
			g_limit[limit].value += g_limit[limit].offset;

		g_limit[limit].init = 1;
	}

	return g_limit[limit].value;
}

/*
** long limit_arg(void)
**
** Returns the (possibly cached) maximum length of arguments to the exec()
** family of functions. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_arg(void)
{
	return limit_sysconf(LIMIT_ARG);
}

/*
** long limit_child(void)
**
** Returns the (possibly cached) maximum number of simulatenous processes
** per user id. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_child(void)
{
	return limit_sysconf(LIMIT_CHILD);
}

/*
** long limit_tick(void)
**
** Returns the (possibly cached) number of clock ticks per second.
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_tick(void)
{
	return limit_sysconf(LIMIT_TICK);
}

/*
** long limit_group(void)
**
** Returns the (possibly cached) maximum number of groups that a user may
** belong to. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_group(void)
{
	return limit_sysconf(LIMIT_GROUP);
}

/*
** long limit_open(void)
**
** Returns the (possibly cached) maximum number of files that a process
** can have open at any time. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_open(void)
{
	return limit_sysconf(LIMIT_OPEN);
}

/*
** long limit_stream(void)
**
** Returns the (possibly cached) maximum number of streams that a process
** can have open at any time. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_stream(void)
{
	return limit_sysconf(LIMIT_STREAM);
}

/*
** long limit_tzname(void)
**
** Returns the (possibly cached) maximum number of bytes in a timezone name.
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_tzname(void)
{
	return limit_sysconf(LIMIT_TZNAME);
}

/*
** long limit_job(void)
**
** Returns whether or not (possibly cached) job control is supported.
** Returns -1 on error.
*/

long limit_job(void)
{
	return limit_sysconf(LIMIT_JOB);
}

/*
** long limit_save_ids(void)
**
** Returns whether or not (possibly cached) a process has a saved set-user-id
** and a saved set-group-id. Returns -1 on error.
*/

long limit_save_ids(void)
{
	return limit_sysconf(LIMIT_SAVE_IDS);
}

/*
** long limit_version(void)
**
** Returns the (possibly cached) year and month the POSIX.1 standard was
** approved in the format YYYYMML. Returns -1 on error.
*/

long limit_version(void)
{
	return limit_sysconf(LIMIT_VERSION);
}

/*
** long limit_pcanon(const char *path)
**
** Returns the maximum length of a formatted input line for the terminal
** referred to by path. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_pcanon(const char *path)
{
	return limit_pathconf(LIMIT_CANON, path);
}

/*
** long limit_fcanon(int fd)
**
** Returns the maximum length of a formatted input line for the terminal
** referred to by fd. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_fcanon(int fd)
{
	return limit_fpathconf(LIMIT_CANON, fd);
}

/*
** long limit_canon(void)
**
** Returns the maximum length of a formatted input line for the terminal
** referred to by "/dev/tty". If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_canon(void)
{
	return limit_pcanon("/dev/tty");
}

/*
** long limit_pinput(const char *path)
**
** Returns the maximum length of an input line for the terminal referred
** to by path. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_pinput(const char *path)
{
	return limit_pathconf(LIMIT_INPUT, path);
}

/*
** long limit_finput(int fd)
**
** Returns the maximum length of an input line for the terminal referred
** to by fd. If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_finput(int fd)
{
	return limit_fpathconf(LIMIT_INPUT, fd);
}

/*
** long limit_input(void)
**
** Returns the maximum length of an input line for the terminal referred
** to by "/dev/tty". If indeterminate, a usable guess is returned.
** Returns -1 on error.
*/

long limit_input(void)
{
	return limit_pinput("/dev/tty");
}

/*
** long limit_pvdisable(const char *path)
**
** Returns whether or not special character processing can be disabled
** for the terminal referred to by path. Returns -1 on error.
*/

long limit_pvdisable(const char *path)
{
	return limit_pathconf(LIMIT_VDISABLE, path);
}

/*
** long limit_fvdisable(int fd)
**
** Returns whether special character processing can be disabled for the
** terminal referred to by fd. Returns -1 on error.
*/

long limit_fvdisable(int fd)
{
	return limit_fpathconf(LIMIT_VDISABLE, fd);
}

/*
** long limit_vdisable(void)
**
** Returns whether special character processing can be disabled for the
** terminal referred to by "/dev/tty". Returns -1 on error.
*/

long limit_vdisable(void)
{
	return limit_pvdisable("/dev/tty");
}

/*
** long limit_plink(const char *path)
**
** Returns the maximum number of links to the file represented by path.
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_plink(const char *path)
{
	return limit_pathconf(LIMIT_LINK, path);
}

/*
** long limit_flink(char *path)
**
** Returns the maximum number of links to the file represented by fd.
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_flink(int fd)
{
	return limit_fpathconf(LIMIT_LINK, fd);
}

/*
** long limit_link(void)
**
** Returns the maximum number of links to the file represented by "/".
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_link(void)
{
	return limit_plink("/");
}

/*
** long limit_pname(const char *path)
**
** Returns the maximum length of a filename in the directory referred to by
** path that the process can create. If indeterminate, a usable guess is
** returned. Returns -1 on error.
*/

long limit_pname(const char *path)
{
	return limit_pathconf(LIMIT_NAME, path);
}

/*
** long limit_fname(int fd)
**
** Returns the maximum length of a filename in the directory referred to by
** fd that the process can create. If indeterminate, a usable guess is
** returned. Returns -1 on error.
*/

long limit_fname(int fd)
{
	return limit_fpathconf(LIMIT_NAME, fd);
}

/*
** long limit_name(void)
**
** Returns the maximum length of a filename in the directory referred to by
** "/" that the process can create. If indeterminate, a usable guess is
** returned. Returns -1 on error.
*/

long limit_name(void)
{
	return limit_pname("/");
}

/*
** long limit_ppath(const char *path)
**
** Returns the maximum length of a relative pathname when path is the current
** directory. If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_ppath(const char *path)
{
	return limit_pathconf(LIMIT_PATH, path);
}

/*
** long limit_fpath(int fd)
**
** Returns the maximum length of a relative pathname when fd is the current
** directory. If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_fpath(int fd)
{
	return limit_fpathconf(LIMIT_PATH, fd);
}

/*
** long limit_path(int fd)
**
** Returns the maximum length of an absolute pathname (including the nul
** character). If indeterminate, a usable guess is returned. Returns -1 on
** error.
*/

long limit_path(void)
{
	return limit_ppath("/");
}

/*
** long limit_ppipe(const char *path)
**
** Returns the size of the pipe buffer for the fifo referred to by path.
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_ppipe(const char *path)
{
	return limit_pathconf(LIMIT_PIPE, path);
}

/*
** long limit_ppipe(int fd)
**
** Returns the size of the pipe buffer for the pipe or fifo referred to by fd.
** If indeterminate, a usable guess is returned. Returns -1 on error.
*/

long limit_fpipe(int fd)
{
	return limit_fpathconf(LIMIT_PIPE, fd);
}

/*
** long limit_pnotrunc(const char *path)
**
** Returns whether or not an error is generated when accessing filenames
** longer than the maximum filename length for the filesystem referred
** to by path. Returns -1 on error.
*/

long limit_pnotrunc(const char *path)
{
	return limit_pathconf(LIMIT_NOTRUNC, path);
}

/*
** long limit_fnotrunc(int fd)
**
** Returns whether or not an error is generated when accessing filenames
** longer than the maximum filename length for the filesystem referred
** to by fd. Returns -1 on error.
*/

long limit_fnotrunc(int fd)
{
	return limit_fpathconf(LIMIT_NOTRUNC, fd);
}

/*
** long limit_notrunc(void)
**
** Returns whether or not an error is generated when accessing filenames
** longer than the maximum filename length for the root filesystem.
** Returns -1 on error.
*/

long limit_notrunc(void)
{
	return limit_pnotrunc("/");
}

/*
** long limit_pchown(const char *path)
**
** Returns whether or not chown(2) may be called on the file referred to by
** path or the files contained in the directory referred to by path.
** Returns -1 on error.
*/

long limit_pchown(const char *path)
{
	return limit_pathconf(LIMIT_CHOWN, path);
}

/*
** long limit_fchown(int fd)
**
** Returns whether or not chown(2) may be called on the file referred to by
** fd or the files contained in the directory referred to by fd.
** Returns -1 on error.
*/

long limit_fchown(int fd)
{
	return limit_fpathconf(LIMIT_CHOWN, fd);
}

/*
** long limit_chown(void)
**
** Returns whether or not chown(2) may be called on the files contained in
** the root directory. Returns -1 on error.
*/

long limit_chown(void)
{
	return limit_pchown("/");
}

#ifdef TEST

#include <string.h>

int main(int ac, char **av)
{
	int fds[2];
	long limit;
	int errors = 0;
	int verbose = (ac >= 2 && !strcmp(av[1], "-v"));

	printf("Testing: %s\n", *av);

	if ((limit = limit_arg()) == -1)
		++errors, printf("Test1: limit_arg() failed\n");
	else if (verbose)
		printf("arg = %ld\n", limit);

	if ((limit = limit_child()) == -1)
		++errors, printf("Test2: limit_child() failed\n");
	else if (verbose)
		printf("child = %ld\n", limit);

	if ((limit = limit_tick()) == -1)
		++errors, printf("Test3: limit_tick() failed\n");
	else if (verbose)
		printf("tick = %ld\n", limit);

	if ((limit = limit_group()) == -1)
		++errors, printf("Test4: limit_group() failed\n");
	else if (verbose)
		printf("group = %ld\n", limit);

	if ((limit = limit_open()) == -1)
		++errors, printf("Test5: limit_open() failed\n");
	else if (verbose)
		printf("open = %ld\n", limit);

	if ((limit = limit_stream()) == -1)
		++errors, printf("Test6: limit_stream() failed\n");
	else if (verbose)
		printf("stream = %ld\n", limit);

	if ((limit = limit_tzname()) == -1)
		++errors, printf("Test7: limit_tzname() failed\n");
	else if (verbose)
		printf("tzname = %ld\n", limit);

	if ((limit = limit_job()) == -1)
		++errors, printf("Test8: limit_job() failed\n");
	else if (verbose)
		printf("job = %ld\n", limit);

	if ((limit = limit_save_ids()) == -1)
		++errors, printf("Test9: limit_save_ids() failed\n");
	else if (verbose)
		printf("save_ids = %ld\n", limit);

	if ((limit = limit_version()) == -1)
		++errors, printf("Test10: limit_version() failed\n");
	else if (verbose)
		printf("version = %ld\n", limit);

	if ((limit = limit_canon()) == -1)
		++errors, printf("Test11: limit_canon() failed\n");
	else if (verbose)
		printf("canon = %ld\n", limit);

	if ((limit = limit_input()) == -1)
		++errors, printf("Test12: limit_input() failed\n");
	else if (verbose)
		printf("input = %ld\n", limit);

	if ((limit = limit_vdisable()) == -1)
		++errors, printf("Test13: limit_vdisable() failed\n");
	else if (verbose)
		printf("vdisable = %ld\n", limit);

	if ((limit = limit_link()) == -1)
		++errors, printf("Test14: limit_link() failed\n");
	else if (verbose)
		printf("link = %ld\n", limit);

	if ((limit = limit_name()) == -1)
		++errors, printf("Test15: limit_name() failed\n");
	else if (verbose)
		printf("name = %ld\n", limit);

	if ((limit = limit_path()) == -1)
		++errors, printf("Test16: limit_path() failed\n");
	else if (verbose)
		printf("path = %ld\n", limit);

	if (pipe(fds) == -1)
	{
		++errors, printf("Test17: failed to test limit_fpipe() - pipe() failed (%s)\n", strerror(errno));
	}
	else
	{
		if ((limit = limit_fpipe(fds[0])) == -1)
			++errors, printf("Test17: limit_fpipe() failed\n");
		else if (verbose)
			printf("pipe = %ld\n", limit);
	}

	if ((limit = limit_notrunc()) == -1)
		++errors, printf("Test18: limit_notrunc() failed\n");
	else if (verbose)
		printf("notrunc = %ld\n", limit);

	if ((limit = limit_chown()) == -1)
		++errors, printf("Test19: limit_chown() failed\n");
	else if (verbose)
		printf("chown = %ld\n", limit);

	if (errors)
		printf("%d/19 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
