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

I<libslack(msg)> - message module

=head1 SYNOPSIS

    #include <slack/msg.h>

    typedef struct Msg Msg;

    void msg_release(Msg *msg);
    void *msg_destroy(Msg **msg);
    void msg_out(Msg *dst, const char *fmt, ...);
    void vmsg_out(Msg *dst, const char *fmt, va_list args);
    Msg *msg_create_fd(int fd);
    Msg *msg_create_fd_locked(Locker *locker, int fd);
    Msg *msg_create_stderr(void);
    Msg *msg_create_stderr_locked(Locker *locker);
    Msg *msg_create_stdout(void);
    Msg *msg_create_stdout_locked(Locker *locker);
    Msg *msg_create_file(const char *path);
    Msg *msg_create_file_locked(Locker *locker, const char *path);
    Msg *msg_create_syslog(const char *ident, int option, int facility);
    Msg *msg_create_syslog_locked(Locker *locker, const char *ident, int option, int facility);
    Msg *msg_create_plex(Msg *msg1, Msg *msg2);
    Msg *msg_create_plex_locked(Locker *locker, Msg *msg1, Msg *msg2);
    int msg_add_plex(Msg *msg, Msg *item);
    const char *msg_set_timestamp_format(const char *format);
    int msg_set_timestamp_format_locker(Locker *locker);
    int syslog_lookup_facility(const char *facility);
    int syslog_lookup_priority(const char *priority);
    const char *syslog_facility_str(int spec);
    const char *syslog_priority_str(int spec);
    int syslog_parse(const char *spec, int *facility, int *priority);

=head1 DESCRIPTION

This module provides general messaging functions. Message channels can be
created that send messages to a file descriptor, a file, I<syslog> or
multiplex messages to any combination of the above. Messages sent to files
are timestamped using (by default) the I<strftime(3)> format: C<"%Y%m%d
%H:%M:%S">.

It also provides functions for parsing I<syslog> targets, converting between
I<syslog> facility names and codes, and converting between I<syslog>
priority names and codes.

=over 4

=cut

*/

#include "std.h"

#include <syslog.h>
#include <fcntl.h>
#include <time.h>

#include <sys/stat.h>

#include "msg.h"
#include "mem.h"
#include "err.h"
#include "str.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

typedef void msg_out_t(void *data, const void *msg, size_t msglen);
typedef void msg_release_t(void *data);
typedef int MsgFDData;
typedef struct MsgFileData MsgFileData;
typedef struct MsgSysData MsgSysData;
typedef struct MsgPlexData MsgPlexData;

struct Msg
{
	msg_out_t *out;         /* message handling function */
	void *data;             /* sybtype specific data */
	msg_release_t *destroy; /* destructor function for data */
	Locker *locker;         /* locking strategy for this structure */
};

struct MsgFileData
{
	char *path; /* file path */
	int fd;     /* file descriptor (-1 when closed) */
};

struct MsgSysData
{
	char *ident;  /* syslog(3) ident */
	int option;   /* sysglog(3) option */
	int facility; /* syslog(3) facility | priority */
};

struct MsgPlexData
{
	size_t size;   /* elements allocated */
	size_t length; /* length of Msg list */
	Msg **list;    /* list of Msg objects */
};

static const char *timestamp_format = "%Y%m%d %H:%M:%S";
static Locker *timestamp_format_locker = NULL;

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

static const syslog_map_t syslog_facility_map[] =
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

static const syslog_map_t syslog_priority_map[] =
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

C<Msg *msg_create(Locker *locker, msg_out_t *out, void *data, msg_release_t *destroy)>

Creates a I<Msg> object initialised with C<out>, C<data> and C<destroy>.
On success, returns the new I<Msg> object. On error, returns C<NULL>.

*/

static Msg *msg_create(Locker *locker, msg_out_t *out, void *data, msg_release_t *destroy)
{
	Msg *msg;

	if (!(msg = mem_new(Msg)))
		return NULL;

	msg->out = out;
	msg->data = data;
	msg->destroy = destroy;
	msg->locker = locker;

	return msg;
}

/*

=item C<void msg_release(Msg *msg)>

Releases (deallocates) C<msg> and its internal data.

=cut

*/

void msg_release(Msg *msg)
{
	Locker *locker;

	if (!msg)
		return;

	locker = msg->locker;

	if (locker_wrlock(locker) == -1)
		return;

	if (msg->destroy)
		msg->destroy(msg->data);

	mem_release(msg);
	locker_unlock(locker);
}

/*

=item C<void *msg_destroy(Msg **msg)>

Destroys (deallocates and sets to C<NULL>) C<*msg>. Returns C<NULL>.

=cut

*/

void *msg_destroy(Msg **msg)
{
	if (msg && *msg)
	{
		msg_release(*msg);
		*msg = NULL;
	}

	return NULL;
}

/*

=item C<void msg_out(Msg *dst, const char *fmt, ...)>

Sends a message to C<dst>. C<fmt> is a I<printf>-like format string. Any
remaining arguments are processed as in I<printf(3)>.

B<Warning: Do not under any circumstances ever pass a non-literal string as
the fmt argument unless you know exactly how many conversions will take
place. Being careless with this is a very good way to build potential
security holes into your programs. The same is true for all functions that
take a printf()-like format string as an argument.>

    msg_out(dst, buf);       // EVIL
    msg_out(dst, "%s", buf); // GOOD

=cut

*/

void msg_out(Msg *dst, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vmsg_out(dst, fmt, args);
	va_end(args);
}

/*

=item C<void vmsg_out(Msg *dst, const char *fmt, va_list ap)>

Sends a message to C<dst>. C<fmt> is a I<printf>-like format string.
C<args> is processed as in I<vprintf(3)>.

=cut

*/

void vmsg_out(Msg *dst, const char *fmt, va_list args)
{
	if (!dst)
		return;

	if (locker_rdlock(dst->locker) == -1)
		return;

	if (dst->out)
	{
		char msg[MSG_SIZE];
		vsnprintf(msg, MSG_SIZE, fmt, args);
		dst->out(dst->data, msg, strlen(msg));
	}

	locker_unlock(dst->locker);
}

/*

C<MsgFDData *msg_fddata_create(int fd)>

Creates and initialises the internal data needed by a I<Msg> object that
sends messages to file descriptor C<fd>. On success, returns the data. On
error, returns C<NULL>.

*/

static MsgFDData *msg_fddata_create(int fd)
{
	MsgFDData *data;

	if (!(data = mem_new(MsgFDData)))
		return NULL;

	*data = fd;

	return data;
}

/*

C<void msg_fddata_release(MsgFDData *data)>

Releases (deallocates) the internal data needed by a I<Msg> object that
sends messages to a file descriptor. The file descriptor is not closed.

*/

static void msg_fddata_release(MsgFDData *data)
{
	mem_release(data);
}

/*

C<void msg_out_fd(void *data, const void *msg, size_t msglen)>

Sends a message to a file descriptor. C<data> is a pointer to the file
descriptor. C<msg> is the message. C<msglen> is it's length.

*/

static void msg_out_fd(void *data, const void *msg, size_t msglen)
{
	if (data && msg)
		write(*(MsgFDData *)data, msg, msglen);
}

/*

=item C<Msg *msg_create_fd(int fd)>

Creates a I<Msg> object that sends messages to file descriptor C<fd>.
On success, returns the new I<Msg> object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_fd(int fd)
{
	return msg_create_fd_locked(NULL, fd);
}

/*

=item C<Msg *msg_create_fd_locked(Locker *locker, int fd)>

Creates a I<Msg> object that sends messages to file descriptor C<fd>.
Multiple threads accessing this I<Msg> object will be synchronised by
C<locker>. On success, returns the new I<Msg> object. On error, returns
C<NULL>.

=cut

*/

Msg *msg_create_fd_locked(Locker *locker, int fd)
{
	MsgFDData *data;
	Msg *msg;

	if (!(data = msg_fddata_create(fd)))
		return NULL;

	if (!(msg = msg_create(locker, msg_out_fd, data, (msg_release_t *)msg_fddata_release)))
	{
		msg_fddata_release(data);
		return NULL;
	}

	return msg;
}

/*

=item C<Msg *msg_create_stderr(void)>

Creates a I<Msg> object that sends messages to standard error. On success,
returns the new I<Msg> object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_stderr(void)
{
	return msg_create_fd_locked(NULL, STDERR_FILENO);
}

/*

=item C<Msg *msg_create_stderr_locked(Locker *locker)>

Creates a I<Msg> object that sends messages to standard error. Multiple
threads accessing this I<Msg> object will be synchronised by C<locker>. On
success, returns the new I<Msg> object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_stderr_locked(Locker *locker)
{
	return msg_create_fd_locked(locker, STDERR_FILENO);
}

/*

=item C<Msg *msg_create_stdout(void)>

Creates a I<Msg> object that sends messages to standard output. On success,
returns the new I<Msg> object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_stdout(void)
{
	return msg_create_fd_locked(NULL, STDOUT_FILENO);
}

/*

=item C<Msg *msg_create_stdout_locked(Locker *locker)>

Creates a I<Msg> object that sends messages to standard output. Multiple
threads accessing this I<Msg> object will be synchronised by C<locker>. On
success, returns the new I<Msg> object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_stdout_locked(Locker *locker)
{
	return msg_create_fd_locked(locker, STDOUT_FILENO);
}

/*

C<int msg_filedata_init(MsgFileData *data, const char *path)>

Initialises the internal data needed by a I<Msg> object that sends messages
to the file specified by C<path>. This data consists of a copy of C<path>
and an open file descriptor to the file. The file descriptor is opened with
the C<O_WRONLY>, C<O_CREAT> and C<O_APPEND> flags. On success, returns 0. On
error, returns -1.

*/

static int msg_filedata_init(MsgFileData *data, const char *path)
{
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	if (!data || !path)
		return -1;

	if (!(data->path = mem_strdup(path)))
		return -1;

	if ((data->fd = open(data->path, O_WRONLY | O_CREAT | O_APPEND, mode)) == -1)
		return -1;

	return 0;
}

/*

C<MsgFileData *msg_filedata_create(const char *path)>

Creates the internal data needed by a I<Msg> object that sends messages to
the file specified by C<path>. On success, returns the data. On error,
returns C<NULL>.

*/

static MsgFileData *msg_filedata_create(const char *path)
{
	MsgFileData *data;

	if (!(data = mem_new(MsgFileData)))
		return NULL;

	if (msg_filedata_init(data, path) == -1)
	{
		mem_release(data);
		return NULL;
	}

	return data;
}

/*

C<void msg_filedata_release(MsgFileData *data)>

Releases (deallocates) the internal data needed by a I<Msg> object that
sends messages to a file. The file descriptor is closed first.

*/

static void msg_filedata_release(MsgFileData *data)
{
	if (!data)
		return;

	if (data->fd != -1)
		close(data->fd);

	mem_release(data->path);
	mem_release(data);
}

/*

C<void msg_out_file(void *data, const void *msg, size_t msglen)>

Sends a message to a file. C<data> contains the file descriptor. C<msg> is
the message. C<msglen> is it's length.

*/

static void msg_out_file(void *data, const void *msg, size_t msglen)
{
	MsgFileData *dst = data;
	char buf[MSG_SIZE];
	size_t buflen;

	time_t t = time(NULL);

	if (locker_rdlock(timestamp_format_locker) == -1)
		return;

	strftime(buf, MSG_SIZE, timestamp_format, localtime(&t));

	if (locker_unlock(timestamp_format_locker) == -1)
		return;

	buflen = strlen(buf);
	buf[buflen++] = ' ';
	if (buflen + msglen >= MSG_SIZE)
		msglen -= MSG_SIZE - buflen;
	memmove(buf + buflen, msg, msglen);

	if (msg && dst && dst->fd != -1)
		write(dst->fd, buf, buflen + msglen);
}

/*

=item C<Msg *msg_create_file(const char *path)>

Creates a I<Msg> object that sends messages to the file specified by
C<path>. On success, returns the new I<Msg> object. On error, returns
C<NULL>.

=cut

*/

Msg *msg_create_file(const char *path)
{
	return msg_create_file_locked(NULL, path);
}

/*

=item C<Msg *msg_create_file_locked(Locker *locker, const char *path)>

Creates a I<Msg> object that sends messages to the file specified by
C<path>. Multiple threads accessing this I<Msg> object will be synchronised
by C<locker>. On success, returns the new I<Msg> object. On error, returns
C<NULL>.

=cut

*/

Msg *msg_create_file_locked(Locker *locker, const char *path)
{
	MsgFileData *data;
	Msg *msg;

	if (!(data = msg_filedata_create(path)))
		return NULL;

	if (!(msg = msg_create(locker, msg_out_file, data, (msg_release_t *)msg_filedata_release)))
	{
		msg_filedata_release(data);
		return NULL;
	}

	return msg;
}

/*

C<int msg_sysdata_init(MsgSysData *data, const char *ident, int option, int facility)>

Initialises the internal data needed by a I<Msg> object that sends messages
to I<syslog>. I<openlog(3)> is called with C<ident> and C<option>.
C<facility> is stored to be used when sending messages. On success, returns
0. On error, returns -1.

*/

static int msg_sysdata_init(MsgSysData *data, const char *ident, int option, int facility)
{
	if (!data || facility == -1)
		return -1;

	data->ident = NULL;
	data->option = option;
	data->facility = facility;

	if (ident && !(data->ident = mem_strdup(ident)))
		return -1;

	if (data->ident || data->option)
		openlog(data->ident, data->option, 0);

	return 0;
}

/*

C<MsgSysData *msg_sysdata_create(const char *ident, int option, int facility)>

Creates the internal data needed by a I<Msg> object that sends messages to
I<syslog>. C<ident>, C<option> and C<facility> are used to initialise the
connection to I<syslog>. On success, returns the data. On error, returns
C<NULL>.

*/

static MsgSysData *msg_sysdata_create(const char *ident, int option, int facility)
{
	MsgSysData *data;

	if (!(data = mem_new(MsgSysData)))
		return NULL;

	if (msg_sysdata_init(data, ident, option, facility) == -1)
	{
		mem_release(data);
		return NULL;
	}

	return data;
}

/*

C<void msg_sysdata_release(MsgSysData *data)>

Releases (deallocates) the internal data needed by a I<Msg> object that
sends messages to I<syslog>. Calls I<closelog(3)>.

*/

static void msg_sysdata_release(MsgSysData *data)
{
	if (!data)
		return;

	mem_release(data->ident);
	mem_release(data);
	closelog();
}

/*

C<void msg_out_syslog(void *data, const void *msg, size_t msglen)>

Sends a message to I<syslog>. C<data> contains the facility to use. C<msg>
is the message. C<msglen> is it's length.

*/

static void msg_out_syslog(void *data, const void *msg, size_t msglen)
{
	MsgSysData *dst = data;

	if (msg && dst && dst->facility != -1)
		syslog(dst->facility, "%*.*s", (int)msglen, (int)msglen, (char *)msg);
}

/*

=item C<Msg *msg_create_syslog(const char *ident, int option, int facility)>

Creates a I<Msg> object that sends messages to I<syslog> initialised with
C<ident>, C<option> and C<facility>. On success, returns the new I<Msg>
object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_syslog(const char *ident, int option, int facility)
{
	return msg_create_syslog_locked(NULL, ident, option, facility);
}

/*

=item C<Msg *msg_create_syslog_locked(Locker *locker, const char *ident, int option, int facility)>

Creates a I<Msg> object that sends messages to I<syslog> initialised with
C<ident>, C<option> and C<facility>. Multiple threads accessing this I<Msg>
object will be synchronised by C<locker>. On success, returns the new I<Msg>
object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_syslog_locked(Locker *locker, const char *ident, int option, int facility)
{
	MsgSysData *data;
	Msg *msg;

	if (!(data = msg_sysdata_create(ident, option, facility)))
		return NULL;

	if (!(msg = msg_create(locker, msg_out_syslog, data, (msg_release_t *)msg_sysdata_release)))
	{
		msg_sysdata_release(data);
		return NULL;
	}

	return msg;
}

/*

C<int msg_plexdata_init(Msg *msg1, Msg *msg2)>

Initialises the internal data needed by a I<Msg> object that multiplexes
messages to several I<Msg> objects. On success, returns 0. On error, returns
-1.

*/

static int msg_plexdata_init(MsgPlexData *data, Msg *msg1, Msg *msg2)
{
	data->length = data->size = 2;

	if (!(data->list = mem_create(data->size, Msg *)))
		return -1;

	data->list[0] = msg1;
	data->list[1] = msg2;

	return 0;
}

/*

C<int msg_plexdata_add(MsgPlexData *data, Msg *msg)>

Adds C<msg> to a list of multiplexed I<Msg> objects. On success, returns 0.
On error, returns -1.

*/

static int msg_plexdata_add(MsgPlexData *data, Msg *msg)
{
	if (data->length == data->size)
	{
		size_t new_size = data->size << 1;
		Msg **new_list = mem_resize(&data->list, new_size);

		if (!new_list)
			return -1;

		data->size = new_size;
		data->list = new_list;
	}

	data->list[data->length++] = msg;

	return 0;
}

/*

C<MsgPlexData *msg_plexdata_create(Msg *msg1, Msg * msg2)>

Creates the internal data needed by a I<Msg> object that multiplexes
messages to several I<Msg> objects. Further I<Msg> objects can be added to
the list with I<msg_plexdata_add(Msg *msg)>. On success, returns the data.
On error, returns C<NULL>.

*/

static MsgPlexData *msg_plexdata_create(Msg *msg1, Msg *msg2)
{
	MsgPlexData *data;

	if (!(data = mem_new(MsgPlexData)))
		return NULL;

	if (msg_plexdata_init(data, msg1, msg2) == -1)
	{
		mem_release(data);
		return NULL;
	}

	return data;
}

/*

C<void msg_plexdata_release(MsgPlexData *data)>

Releases (deallocates) the internal data needed by a I<Msg> object that
multiplexes messages to several I<Msg> objects.

*/

static void msg_plexdata_release(MsgPlexData *data)
{
	size_t i;

	if (!data)
		return;

	for (i = 0; i < data->length; ++i)
		msg_destroy(data->list + i);

	mem_release(data->list);
	mem_release(data);
}

/*

C<void msg_out_plex(void *data, const void *msg, size_t msglen)>

Multiplexes a message to several I<Msg> objects. C<data> contains the list
of I<Msg> objects. C<msg> is the message. C<msglen> is it's length.

*/

static void msg_out_plex(void *data, const void *msg, size_t msglen)
{
	MsgPlexData *dst = data;
	size_t i;

	if (msg && dst)
	{
		for (i = 0; i < dst->length; ++i)
		{
			Msg *out = dst->list[i];
			if (out && out->out)
				out->out(out->data, msg, msglen);
		}
	}
}

/*

=item C<Msg *msg_create_plex(Msg *msg1, Msg *msg2)>

Creates a I<Msg> object that multiplexes messages to C<msg1> and C<msg2>.
Further I<Msg> objects may be added to its list using I<msg_add_plex(Msg
*msg)>. On success, returns the new I<Msg> object. On error, returns
C<NULL>.

=cut

*/

Msg *msg_create_plex(Msg *msg1, Msg *msg2)
{
	return msg_create_plex_locked(NULL, msg1, msg2);
}

/*

=item C<Msg *msg_create_plex_locked(Locker *locker, Msg *msg1, Msg *msg2)>

Creates a I<Msg> object that multiplexes messages to C<msg1> and C<msg2>.
Further I<Msg> objects may be added to its list using I<msg_add_plex(Msg
*msg)>. Multiple threads accessing this I<Msg> object will be synchronised
by C<locker>. On success, returns the new I<Msg> object. On error, returns
C<NULL>.

=cut

*/

Msg *msg_create_plex_locked(Locker *locker, Msg *msg1, Msg *msg2)
{
	MsgPlexData *data;
	Msg *msg;

	if (!(data = msg_plexdata_create(msg1, msg2)))
		return NULL;

	if (!(msg = msg_create(locker, msg_out_plex, data, (msg_release_t *)msg_plexdata_release)))
	{
		msg_plexdata_release(data);
		return NULL;
	}

	return msg;
}

/*

=item C<int msg_add_plex(Msg *msg, Msg *item)>

Adds C<item> to the list of I<Msg> objects multiplexed by C<msg>. On
success, returns 0. On error, returns -1.

=cut

*/

int msg_add_plex(Msg *msg, Msg *item)
{
	int rc;

	if (!msg)
		return set_errno(EINVAL);

	if (locker_wrlock(msg->locker) == -1)
		return -1;

	rc = msg_plexdata_add((MsgPlexData *)msg->data, item);

	if (locker_unlock(msg->locker) == -1)
		return -1;

	return rc;
}

/*

=item C<const char *msg_set_timestamp_format(const char *format)>

Sets the I<strftime(3)> format string used when sending messages to a file.
By default, it is C<"%Y%m%d %H:%M:%S">. On success, returns the previous
format string. On error (i.e. C<format> is C<NULL>), returns C<NULL>.

=cut

*/

const char *msg_set_timestamp_format(const char *format)
{
	const char *save;

	if (!format)
		return NULL;

	if (locker_wrlock(timestamp_format_locker) == -1)
		return NULL;

	save = timestamp_format;
	timestamp_format = format;

	if (locker_unlock(timestamp_format_locker) == -1)
		return NULL;

	return save;
}

/*

=item C<int msg_set_timestamp_format_locker(Locker *locker)>

Sets the locking strategy for changing the timestamp format used when
sending messages to a file. This is only needed if the timestamp format will
be modified in multiple threads. On success, returns C<0>. On error, returns
C<-1>.

=cut

*/

int msg_set_timestamp_format_locker(Locker *locker)
{
	if (timestamp_format_locker)
		return set_errno(EINVAL);

	timestamp_format_locker = locker;

	return 0;
}

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

Returns the code corresponding to C<facility>. If not found, returns -1.

=cut

*/

int syslog_lookup_facility(const char *facility)
{
	return syslog_lookup(syslog_facility_map, facility);
}

/*

=item C<int syslog_lookup_priority(const char *priority)>

Returns the code corresponding to C<priority>. If not found, returns -1.

=cut

*/

int syslog_lookup_priority(const char *priority)
{
	return syslog_lookup(syslog_priority_map, priority);
}

/*

=item C<const char *syslog_facility_str(int spec)>

Returns the name corresponding to the facility part of C<spec>.
If not found, returns C<NULL>.

=cut

*/

const char *syslog_facility_str(int spec)
{
	return syslog_lookup_str(syslog_facility_map, spec, LOG_FACMASK);
}

/*

=item C<const char *syslog_priority_str(int spec)>

Returns the name corresponding to the priority part of C<spec>.
If not found, returns C<NULL>.

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

    syslog facilities          syslog priorities
    ----------------------     -----------------------
    kern       LOG_KERN        emerg       LOG_EMERG
    user       LOG_USER        alert       LOG_ALERT
    mail       LOG_MAIL        crit        LOG_CRIT
    daemon     LOG_DAEMON      err         LOG_ERR
    auth       LOG_AUTH        warning     LOG_WARNING
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

=cut

*/

int syslog_parse(const char *spec, int *facility, int *priority)
{
	char fac[64], *pri;
	int f, p;

	if (!spec)
		return -1;

	strlcpy(fac, spec, 64);

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
    #include <slack/msg.h>

    int main(int ac, char **av)
    {
        int facility, priority;
        if (syslog_parse(av[1], &facility, &priority) != -1)
            syslog(facility | priority, "syslog(%s)", av[1]);
        return 0;
    }

=head1 MT-Level

MT-Disciplined - msg functions - man I<thread(3)> for details

MT-Safe - syslog functions

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<err(3)|err(3)>,
L<prog(3)|prog(3)>,
L<syslog(3)|syslog(3)>,
L<thread(3)|thread(3)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

=cut

*/

#ifdef TEST

static int verify(const char *prefix, const char *name, const char *msg)
{
	char buf[MSG_SIZE];
	int fd;
	ssize_t bytes;

	if ((fd = open(name, O_RDONLY)) == -1)
	{
		printf("%s: failed to create msg file: %s (%s)\n", prefix, name, strerror(errno));
		return 1;
	}

	memset(buf, nul, MSG_SIZE);
	bytes = read(fd, buf, MSG_SIZE);
	close(fd);
	unlink(name);

	if (bytes == -1)
	{
		printf("%s: failed to read msg file: %s (%s)\n", prefix, name, strerror(errno));
		return 1;
	}

	if (!strstr(buf, msg))
	{
		printf("%s: msg file produced incorrect input:\nshould contain:\n%s\nwas:\n%s\n", prefix, msg, buf);
		return 1;
	}

	return 0;
}

int main(int ac, char **av)
{
	const char *msg_file_name = "./msg.file";
	const char *msg_stdout_name = "./msg.stdout";
	const char *msg_stderr_name = "./msg.stderr";
	const char *msg = "msg multiplexed to stdout, stderr, ./msg.file and syslog local0.debug\n";
	const char *note = "\n    Note: Can't verify syslog local0.debug. Look for:";

	Msg *msg_stdout = msg_create_stdout();
	Msg *msg_stderr = msg_create_stderr();
	Msg *msg_file = msg_create_file(msg_file_name);
	Msg *msg_syslog = msg_create_syslog(NULL, 0, LOG_LOCAL0 | LOG_DEBUG);
	Msg *msg_plex = msg_create_plex(msg_stdout, msg_stderr);
	int errors = 0;
	int tests = 0;
	int out, i, j, rc;

	printf("Testing: msg\n");

	if (!msg_stdout)
		++errors, printf("Test1: failed to create msg_stdout");
	if (!msg_stderr)
		++errors, printf("Test2: failed to create msg_stder");
	if (!msg_file)
		++errors, printf("Test3: failed to create msg_file");
	if (!msg_syslog)
		++errors, printf("Test4: failed to create msg_syslog");
	if (!msg_plex)
		++errors, printf("Test5: failed to create msg_plex");
	if (msg_add_plex(msg_plex, msg_file) == -1)
		++errors, printf("Test6: failed to add msg_file to msg_plex\n");
	if (msg_add_plex(msg_plex, msg_syslog) == -1)
		++errors, printf("Test7: failed to add msg_syslog to msg_plex\n");

	out = dup(STDOUT_FILENO);
	freopen(msg_stdout_name, "w", stdout);
	freopen(msg_stderr_name, "w", stderr);
	msg_out(msg_plex, msg);
	msg_destroy(&msg_plex);
	dup2(out, STDOUT_FILENO);
	close(out);

	errors += verify("Test8", msg_stdout_name, msg);
	errors += verify("Test9", msg_stderr_name, msg);
	errors += verify("Test10", msg_file_name, msg);

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
		++errors, printf("Test%d: syslog_parse(NULL) failed\n", tests);

	++tests;
	if (syslog_parse("gibberish", NULL, NULL) != -1)
		++errors, printf("Test%d: syslog_parse(\"gibberish\") failed\n", tests);

	if (errors)
		printf("%d/%d tests failed\n%s\n    %s", errors, 10 + tests, note, msg);
	else
		printf("All tests passed\n%s\n    %s", note, msg);

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
