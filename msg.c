/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>

#include <sys/stat.h>

#include "mem.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

typedef struct Msg Msg;
typedef void msg_out_t(void *data, const void *msg, size_t msglen);
typedef void msg_destroy_t(void *data);
typedef int MsgFDData;
typedef struct MsgFileData MsgFileData;
typedef struct MsgSysData MsgSysData;
typedef struct MsgPlexData MsgPlexData;

struct Msg
{
	msg_out_t *out;         // message handling function
	void *data;             // sybtype specific data
	msg_destroy_t *destroy; // destructor function for data
};

struct MsgFileData
{
	char *path; // file path
	int fd;     // file descriptor (-1 when closed)
};

struct MsgSysData
{
	char *ident;  // syslog(3) ident
	int option;   // sysglog(3) option
	int facility; // syslog(3) facility | priority
};

struct MsgPlexData
{
	size_t size;   // elements allocated
	size_t length; // length of Msg list
	Msg **list;    // list of Msg objects
};

#define strdup(s) strcpy(malloc(strlen(s) + 1), (s))

/*
** Msg *msg_create(msg_out_t *out, void *data, msg_destroy_t *destroy)
**
** Creates a Msg object initialised with out, data and destroy.
*/

static Msg *msg_create(msg_out_t *out, void *data, msg_destroy_t *destroy)
{
	Msg *msg;

	if (!(msg = mem_create(1, Msg)))
		return NULL;

	msg->out = out;
	msg->data = data;
	msg->destroy = destroy;

	return msg;
}

/*
** void msg_release(Msg *msg)
**
** Deallocates msg and its internal data.
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
** void *msg_destroy_fn(Msg **msg)
**
** Deallocates the Msg object pointer pointed to by msg and assigns null
** to that pointer.
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
** void vmsg_out(Msg *dst, const char *fmt, va_list ap)
**
** Emits a message to dst. fmt is a printf-like format string. Any remaining
** arguments are processed as in vprintf(3).
*/

void vmsg_out(Msg *dst, const char *fmt, va_list args)
{
	if (dst && dst->out)
	{
		char msg[BUFSIZ];
		vsnprintf(msg, BUFSIZ, fmt, args);
		dst->out(dst->data, msg, strlen(msg));
	}
}

/*
** void msg_out(Msg *dst, const char *fmt, ...)
**
** Emits a message to dst. fmt is a printf-like format string. Any remaining
** arguments are processed as in printf(3).
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
** MsgFDData *msg_fddata_create(int fd)
**
** Creates and initialises the internal data needed by a Msg object that
** emits messages to the file descriptor specified by fd.
** Returns NULL on error.
*/

static MsgFDData *msg_fddata_create(int fd)
{
	MsgFDData *data = mem_create(1, MsgFDData);

	if (!data)
		return NULL;

	*data = fd;

	return data;
}

/*
** void msg_fddata_release(MsgFDData *data)
**
** Deallocates the internal data needed by a Msg object that emits messages
** to a file descriptor. The file descriptor is not closed.
*/

static void msg_fddata_release(MsgFDData *data)
{
	mem_release(data);
}

/*
** void msg_out_fd(void *data, const void *msg, size_t msglen)
**
** Emits a message to a file descriptor. data is a pointer to the file
** descriptor. msg is the message. msglen is it's length.
*/

static void msg_out_fd(void *data, const void *msg, size_t msglen)
{
	if (data && msg)
		write(*(MsgFDData*)data, msg, msglen);
}

/*
** Msg *msg_create_fd(int fd)
**
** Creates a Msg object that emits messages to the file descriptor specified
** by fd.
*/

Msg *msg_create_fd(int fd)
{
	MsgFDData *data;
	Msg *msg;

	data = msg_fddata_create(fd);

	if (!data)
		return NULL;

	msg = msg_create(msg_out_fd, data, (msg_destroy_t*)msg_fddata_release);

	if (!msg)
	{
		msg_fddata_release(data);
		return NULL;
	}

	return msg;
}

/*
** Msg *msg_create_stderr(void)
**
** Creates a Msg object that emits messages to standard error.
*/

Msg *msg_create_stderr(void)
{
	return msg_create_fd(STDERR_FILENO);
}

/*
** Msg *msg_create_stdout(void)
**
** Creates a Msg object that emits messages to standard output.
*/

Msg *msg_create_stdout(void)
{
	return msg_create_fd(STDOUT_FILENO);
}

/*
** int msg_filedata_init(MsgFileData *data, const char *path)
**
** Initialises the internal data needed by a Msg object that emits messages
** to the file specified by path. This data consists of a copy of the path
** and an open file descriptor to the file. The file descriptor is opened
** with the O_WRONLY, O_CREAT and O_APPEND flags. Returns 0 on success, -1 on
** error.
*/

static int msg_filedata_init(MsgFileData *data, const char *path)
{
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	if (!data || !path)
		return -1;

	if (!(data->path = strdup(path)))
		return -1;

	data->fd = open(data->path, O_WRONLY | O_CREAT | O_APPEND, mode);
	if (data->fd == -1)
		return -1;

	return 0;
}

/*
** MsgFileData *msg_filedata_create(const char *path)
**
** Creates the internal data needed by a Msg object that emits messages
** to the file specified by path. Returns NULL on error.
*/

static MsgFileData *msg_filedata_create(const char *path)
{
	MsgFileData *data;

	if (!(data = mem_create(1, MsgFileData)))
		return NULL;

	if (msg_filedata_init(data, path) == -1)
	{
		mem_release(data);
		return NULL;
	}

	return data;
}

/*
** void msg_filedata_release(MsgFileData *data)
**
** Deallocates the internal data needed by a Msg object that emits messages
** to a file. The file descriptor is closed first.
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
** void msg_out_file(void *data, const void *msg, size_t msglen)
**
** Emits a message to a file. data contains the file descriptor.
** msg is the message. msglen is it's length.
*/

static void msg_out_file(void *data, const void *msg, size_t msglen)
{
	MsgFileData *dst = data;

	if (msg && dst && dst->fd != -1)
		write(dst->fd, msg, msglen);
}

/*
** Msg *msg_create_file(const char *path)
**
** Creates a Msg object that emits messages to the file specified by path.
*/

Msg *msg_create_file(const char *path)
{
	MsgFileData *data;
	Msg *msg;

	if (!(data = msg_filedata_create(path)))
		return NULL;

	if (!(msg = msg_create(msg_out_file, data, (msg_destroy_t*)msg_filedata_release)))
	{
		msg_filedata_release(data);
		return NULL;
	}

	return msg;
}

/*
** int msg_sysdata_init(MsgSysData *data, const char *ident, int option, int facility)
**
** Initialises the internal data needed by a Msg object that emits messages to
** the system logger. openlog(3) is called with ident and option. facility is
** stored to be used when emitting messages. Returns 0 on success, -1 on error.
*/

static int msg_sysdata_init(MsgSysData *data, const char *ident, int option, int facility)
{
	if (!data || facility == -1)
		return -1;

	data->ident = NULL;
	data->option = option;
	data->facility = facility;

	if (ident && !(data->ident = strdup(ident)))
		return -1;

	if (data->ident || data->option)
		openlog(data->ident, data->option, 0);

	return 0;
}

/*
** MsgSysData *msg_sysdata_create(const char *ident, int option, int facility)
**
** Creates the internal data needed by a Msg object that emits messages to
** the system logger. ident, option and facility are used to initialise
** the connection to the system logger. Returns NULL on error.
*/

static MsgSysData *msg_sysdata_create(const char *ident, int option, int facility)
{
	MsgSysData *data;

	if (!(data = mem_create(1, MsgSysData)))
		return NULL;

	if (msg_sysdata_init(data, ident, option, facility) == -1)
	{
		mem_release(data);
		return NULL;
	}

	return data;
}

/*
** void msg_sysdata_release(MsgSysData *data)
**
** Deallocates the internal data needed by a Msg object that emits messages to
** the system logger. Calls closelog(3).
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
** void msg_out_syslog(void *data, const void *msg, size_t msglen)
**
** Emits a message to the system logger. data contains the facility to use.
** msg is the message. msglen is it's length.
*/

static void msg_out_syslog(void *data, const void *msg, size_t msglen)
{
	MsgSysData *dst = data;

	if (msg && dst && dst->facility != -1)
		syslog(dst->facility, "%*.*s", msglen, msglen, msg);
}

/*
** Msg *msg_create_syslog(const char *ident, int option, int facility)
**
** Creates a Msg object that emits messages to the system logger
** initialised with ident, option and facility.
*/

Msg *msg_create_syslog(const char *ident, int option, int facility)
{
	MsgSysData *data;
	Msg *msg;

	if (!(data = msg_sysdata_create(ident, option, facility)))
		return NULL;

	if (!(msg = msg_create(msg_out_syslog, data, (msg_destroy_t*)msg_sysdata_release)))
	{
		msg_sysdata_release(data);
		return NULL;
	}

	return msg;
}

/*
** int msg_plexdata_init(Msg *msg1, Msg *msg2)
**
** Initialises the internal data needed by a Msg object that multiplexes
** messages to several Msg objects.
** Returns 0 on success, -1 on error.
*/

static int msg_plexdata_init(MsgPlexData *data, Msg *msg1, Msg *msg2)
{
	data->length = data->size = 2;

	if (!(data->list = mem_create(data->size, Msg*)))
		return -1;

	data->list[0] = msg1;
	data->list[1] = msg2;

	return 0;
}

/*
** int msg_plexdata_add(MsgPlexData *data, Msg *msg)
**
** Adds msg to a list of multiplexed Msg objects.
** Returns 0 on success, -1 on error.
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
** MsgPlexData *msg_plexdata_create(Msg *msg1, Msg * msg2)
**
** Creates the internal data needed by a Msg object that multiplexes messages
** to several Msg objects. More Msg objects can be added to the list with
** msg_plexdata_add(Msg *msg). Returns NULL on error.
*/

static MsgPlexData *msg_plexdata_create(Msg *msg1, Msg *msg2)
{
	MsgPlexData *data;

	if (!(data = mem_create(1, MsgPlexData)))
		return NULL;

	if (msg_plexdata_init(data, msg1, msg2) == -1)
	{
		mem_release(data);
		return NULL;
	}

	return data;
}

/*
** void msg_plexdata_release(MsgPlexData *data)
**
** Deallocates the internal data needed by a Msg object that multiplexes
** messages to several Msg objects.
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
** void msg_out_plex(void *data, const void *msg, size_t msglen)
**
** Multiplexes a message to several Msg objects. data contains the list of
** Msg objects. msg is the message. msglen is it's length.
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
** Msg *msg_create_plex(Msg *msg1, Msg *msg2)
**
** Creates a Msg object that multiplexes messages to msg1 and msg2.
** Further Msg objects may be added to its list using
** msg_add_plex(Msg *msg).
*/

Msg *msg_create_plex(Msg *msg1, Msg *msg2)
{
	MsgPlexData *data;
	Msg *msg;

	if (!(data = msg_plexdata_create(msg1, msg2)))
		return NULL;

	if (!(msg = msg_create(msg_out_plex, data, (msg_destroy_t*)msg_plexdata_release)))
	{
		msg_plexdata_release(data);
		return NULL;
	}

	return msg;
}

/*
** int msg_add_plex(Msg *msg, Msg *item)
**
** Adds item to the list of Msg objects multiplexed by msg.
*/

int msg_add_plex(Msg *msg, Msg *item)
{
	return msg_plexdata_add((MsgPlexData*)msg->data, item);
}

#ifdef TEST

#undef strdup
#include <string.h>
#include <errno.h>

static int verify(const char *prefix, const char *name, const char *msg)
{
	char buf[BUFSIZ];
	int fd;
	ssize_t bytes;

	if ((fd = open(name, O_RDONLY)) == -1)
	{
		printf("%s: failed to create msg file: %s (%s)\n", prefix, name, strerror(errno));
		return 1;
	}

	memset(buf, '\0', BUFSIZ);
	bytes = read(fd, buf, BUFSIZ);
	close(fd);
	unlink(name);

	if (bytes == -1)
	{
		printf("%s: failed to read msg file: %s (%s)\n", prefix, name, strerror(errno));
		return 1;
	}

	if (strcmp(buf, msg))
	{
		printf("%s: msg file produced incorrect input:\nshould be:\n%s\nwas:\n%s\n", prefix, msg, buf);
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
		"Note: can't verify syslog local0.debug. Check that it received:";

	Msg *msg_stdout = msg_create_stdout();
	Msg *msg_stderr = msg_create_stderr();
	Msg *msg_file = msg_create_file(msg_file_name);
	Msg *msg_syslog = msg_create_syslog(NULL, 0, LOG_LOCAL0 | LOG_DEBUG);
	Msg *msg_plex = msg_create_plex(msg_stdout, msg_stderr);
	int errors = 0;
	int out;

	printf("Testing: %s\n", *av);

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
		printf("%d/10 tests failed.\n%s\n%s", errors, note, msg);
	else
		printf("All tests passed.\n%s\n%s", note, msg);

	exit(0);
}

#endif

/* vi:set ts=4 sw=4: */
