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

I<libslack(msg)> - message module

=head1 SYNOPSIS

    #include <slack/msg.h>

    typedef struct Msg Msg;

    void msg_release(Msg *msg);
    #define msg_destroy(msg)
    void *msg_destroy_fn(Msg **msg);
    void vmsg_out(Msg *dst, const char *fmt, va_list args);
    void msg_out(Msg *dst, const char *fmt, ...);
    Msg *msg_create_fd(int fd);
    Msg *msg_create_stderr(void);
    Msg *msg_create_stdout(void);
    Msg *msg_create_file(const char *path);
    Msg *msg_create_syslog(const char *ident, int option, int facility);
    Msg *msg_create_plex(Msg *msg1, Msg *msg2);
    int msg_add_plex(Msg *msg, Msg *item);
    const char *msg_set_timestamp_format(const char *format);

=head1 DESCRIPTION

This module provides general messaging functions. Message channels can be
created that send messages to a file descriptor, a file, I<syslog> or
multiplex messages to any combination of the above. Messages sent to files
are timestamped using (by default) the I<strftime(3)> format: C<"%Y%m%d
%H:%M:%S">.

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

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

typedef void msg_out_t(void *data, const void *msg, size_t msglen);
typedef void msg_destroy_t(void *data);
typedef int MsgFDData;
typedef struct MsgFileData MsgFileData;
typedef struct MsgSysData MsgSysData;
typedef struct MsgPlexData MsgPlexData;

struct Msg
{
	msg_out_t *out;         /* message handling function */
	void *data;             /* sybtype specific data */
	msg_destroy_t *destroy; /* destructor function for data */
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

/*

C<Msg *msg_create(msg_out_t *out, void *data, msg_destroy_t *destroy)>

Creates a I<Msg> object initialised with C<out>, C<data> and C<destroy>.
On success, returns the new I<Msg> object. On error, returns C<NULL>.

*/

static Msg *msg_create(msg_out_t *out, void *data, msg_destroy_t *destroy)
{
	Msg *msg;

	if (!(msg = mem_new(Msg)))
		return NULL;

	msg->out = out;
	msg->data = data;
	msg->destroy = destroy;

	return msg;
}

/*

=item C<void msg_release(Msg *msg)>

Releases (deallocates) C<msg> and its internal data.

=cut

*/

void msg_release(Msg *msg)
{
	if (!msg)
		return;

	if (msg->destroy)
		msg->destroy(msg->data);

	mem_release(msg);
}

/*

=item C< #define msg_destroy(msg)>

Destroys (deallocates and sets to C<NULL>) C<msg>. Returns C<NULL>.

=item C<void *msg_destroy_fn(Msg **msg)>

Destroys (deallocates and sets to C<NULL>) the I<Msg> object pointer pointed
to by C<msg>. Returns C<NULL>. This function is exposed as an implementation
side effect. Don't call it directly. Call I<msg_destroy()> instead.

=cut

*/

void *msg_destroy_fn(Msg **msg)
{
	if (msg && *msg)
	{
		msg_release(*msg);
		*msg = NULL;
	}

	return NULL;
}

/*

=item C<void vmsg_out(Msg *dst, const char *fmt, va_list ap)>

Sends a message to C<dst>. C<fmt> is a I<printf>-like format string.
C<args> is processed as in I<vprintf(3)>.

=cut

*/

void vmsg_out(Msg *dst, const char *fmt, va_list args)
{
	if (dst && dst->out)
	{
		char msg[MSG_SIZE];
		vsnprintf(msg, MSG_SIZE, fmt, args);
		dst->out(dst->data, msg, strlen(msg));
	}
}

/*

=item C<void msg_out(Msg *dst, const char *fmt, ...)>

Sends a message to C<dst>. C<fmt> is a I<printf>-like format string. Any
remaining arguments are processed as in I<printf(3)>.

=cut

*/

void msg_out(Msg *dst, const char *fmt, ...)
{
	if (dst && dst->out)
	{
		va_list args;
		va_start(args, fmt);
		vmsg_out(dst, fmt, args);
		va_end(args);
	}
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
	MsgFDData *data;
	Msg *msg;

	if (!(data = msg_fddata_create(fd)))
		return NULL;

	if (!(msg = msg_create(msg_out_fd, data, (msg_destroy_t *)msg_fddata_release)))
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
	return msg_create_fd(STDERR_FILENO);
}

/*

=item C<Msg *msg_create_stdout(void)>

Creates a I<Msg> object that sends messages to standard output. On success,
returns the new I<Msg> object. On error, returns C<NULL>.

=cut

*/

Msg *msg_create_stdout(void)
{
	return msg_create_fd(STDOUT_FILENO);
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

=item C<const char *msg_set_timestamp_format(const char *format)>

Sets the I<strftime(3)> format string used when sending messages to a file.
By default, it is C<"%Y%m%d %H:%M:%S">. On success, returns the previous
format string. On error (i.e. C<format> is C<NULL>), returns C<NULL>.

=cut

*/

static const char *timestamp_format = "%Y%m%d %H:%M:%S";
const char *msg_set_timestamp_format(const char *format)
{
	if (format)
	{
		const char *save = timestamp_format;
		timestamp_format = format;
		return save;
	}

	return NULL;
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
	strftime(buf, MSG_SIZE, timestamp_format, localtime(&t));
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
	MsgFileData *data;
	Msg *msg;

	if (!(data = msg_filedata_create(path)))
		return NULL;

	if (!(msg = msg_create(msg_out_file, data, (msg_destroy_t *)msg_filedata_release)))
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
		syslog(dst->facility, "%*.*s", msglen, msglen, msg);
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
	MsgSysData *data;
	Msg *msg;

	if (!(data = msg_sysdata_create(ident, option, facility)))
		return NULL;

	if (!(msg = msg_create(msg_out_syslog, data, (msg_destroy_t *)msg_sysdata_release)))
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
		Msg **new_list = mem_resize(data->list, new_size);

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
		msg_destroy_fn(data->list + i);

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
	MsgPlexData *data;
	Msg *msg;

	if (!(data = msg_plexdata_create(msg1, msg2)))
		return NULL;

	if (!(msg = msg_create(msg_out_plex, data, (msg_destroy_t *)msg_plexdata_release)))
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
	return msg_plexdata_add((MsgPlexData *)msg->data, item);
}

/*

=back

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

	memset(buf, '\0', MSG_SIZE);
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
	const char *note =
		"\n    Note: can't verify syslog local0.debug. Look for:";

	Msg *msg_stdout = msg_create_stdout();
	Msg *msg_stderr = msg_create_stderr();
	Msg *msg_file = msg_create_file(msg_file_name);
	Msg *msg_syslog = msg_create_syslog(NULL, 0, LOG_LOCAL0 | LOG_DEBUG);
	Msg *msg_plex = msg_create_plex(msg_stdout, msg_stderr);
	int errors = 0;
	int out;

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
	msg_destroy_fn(&msg_plex);
	dup2(out, STDOUT_FILENO);
	close(out);

	errors += verify("Test8", msg_stdout_name, msg);
	errors += verify("Test9", msg_stderr_name, msg);
	errors += verify("Test10", msg_file_name, msg);

	if (errors)
		printf("%d/10 tests failed\n%s\n    %s", errors, note, msg);
	else
		printf("All tests passed\n%s\n    %s", note, msg);

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
