/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/stat.h>
#define BSD_COMP /* for Solaris::FIONBIO */
#include <sys/ioctl.h>
#undef BSD_COMP

#include "err.h"

/*
** int fcntl_set_flag(int fd, int flag)
**
** Shorthand for seting a fcntl flag on a file descriptor.
** All other flags are unaffected.
** Returns the same as fcntl(2).
*/

int fcntl_set_flag(int fd, int flag)
{
	int flags;

	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		return -1;

	return fcntl(fd, F_SETFL, flags | flag);
}

/*
** int fcntl_clear_flag(int fd, int flag)
**
** Shorthand for clearing a fcntl flag from a file descriptor.
** All other flags are unaffected.
** Returns the same as fcntl(2).
*/

int fcntl_clear_flag(int fd, int flag)
{
	int flags;

	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		return -1;

	return fcntl(fd, F_SETFL, flags & ~flag);
}

/*
** int fcntl_lock(int fd, int cmd, int type, int whence, int start, int len)
**
** Shorthand for performing discretionary file locking operations on a
** file descriptor. Returns the same as fcntl(2).
*/

int fcntl_lock(int fd, int cmd, int type, int whence, int start, int len)
{
	struct flock lock[1];

	lock->l_type = type;
	lock->l_whence = whence;
	lock->l_start = start;
	lock->l_len = len;

	return fcntl(fd, cmd, lock);
}

/*
** int nonblock_set(int fd, int arg)
**
** Sets non-blocking mode if arg is non-zero, clears it if arg is zero.
** Returns the same as ioctl(2).
*/

int nonblock_set(int fd, int arg)
{
	return ioctl(fd, FIONBIO, &arg);
}

/*
** int nonblock_on(int fd)
**
** Sets non-blocking mode for the the file descriptor.
** Returns the same as ioctl(2).
*/

int nonblock_on(int fd)
{
	return nonblock_set(fd, 1);
}

/*
** int nonblock_off(int fd)
**
** Clears non-blocking mode for the the file descriptor.
** Returns the same as ioctl(2).
*/

int nonblock_off(int fd)
{
	return nonblock_set(fd, 0);
}

/*
** int fifo_exists(const char *path, int prepare)
**
** Determines whether or not path refers to a fifo. Returns 0 if path
** doesn't exist or doesn't refer to a fifo. Returns 1 if path is a fifo.
** If prepare is non-zero, and path refers to a non-fifo, it will be unlinked.
** Returns -1 on error with errno set by stat(2).
*/

int fifo_exists(const char *path, int prepare)
{
	struct stat status[1];

	if (stat(path, status) == -1)
		return (errno == ENOENT) ? 0 : -1;

	if (S_ISFIFO(status->st_mode) == 0)
	{
		if (prepare)
			unlink(path);

		return 0;
	}

	return 1;
}

/*
** int fifo_has_reader(const char *path, int prepare)
**
** Determines whether or not the given path refers to a fifo that is
** being read by another process. Returns 0 if the path does not exist
** or does not refer to a fifo or if the fifo can't be opened for
** non-blocking write(2). If prepare is non-zero, and path refers to a
** non-fifo, it will be unlinked. Returns -1 on error with errno set by
** stat(2), open(2).
*/

int fifo_has_reader(const char *path, int prepare)
{
	int fd;

	/*
	** Check that fifo exists and is a fifo.
	** If not, there can be no reader process.
	*/

	switch (fifo_exists(path, prepare))
	{
		case  0: return 0;
		case -1: return -1;
	}

	/*
	** Open the fifo for non-blocking write.
	** If there is no reader process, open()
	** will fail with errno == ENXIO.
	*/

	if ((fd = open(path, O_WRONLY | O_NONBLOCK)) == -1)
		return (errno == ENXIO) ? 0 : -1;

	close(fd);

	return 1;
}

/*
** int fifo_open(char *path, mode_t mode, int lock)
**
** Creates a fifo for reading. If path already exists, is a fifo and has
** a reader process, returns -1 with errno set to EADDRINUSE. If the fifo
** is created (or an existing one can be reused), two file descriptors are
** opened to the fifo. A read descriptor and a write descriptor. The read
** descriptor is returned by this function on success. The write descriptor
** only exists to ensure that there is always at least one writer process
** for the fifo. This allows a read(2) on the read descriptor to block until
** another process writes to the fifo rather than returning an EOF condition.
** If lock is non-zero, the fifo is exclusively locked.
** Returns -1 on error with errno set by stat(2), open(2), mkfifo(2),
** fstat(2), fcntl(2), ioctl(2).
*/

int fifo_open(const char *path, mode_t mode, int lock)
{
	struct stat status[1];
	int rfd, wfd;

	/*
	** Don't open the fifo for reading twice.
	*/

	switch (fifo_has_reader(path, 1))
	{
		case  1: return set_errno(EADDRINUSE);
		case -1: return -1;
	}

	/*
	** Create the fifo.
	*/

	if (mkfifo(path, S_IFIFO | mode) == -1 && errno != EEXIST)
		return -1;

	/*
	** Open the fifo for non-blocking read only.
	** This prevents blocking while waiting for a
	** writer process. We are about to supply our
	** own writer.
	*/

	if ((rfd = open(path, O_RDONLY | O_NONBLOCK)) == -1)
		return -1;

	/*
	** A sanity check to make sure that what we have just
	** opened is really a fifo. Someone may have just replacd
	** the fifo with a file between fifo_has_reader and here.
	*/

	if (fstat(rfd, status) == -1 || S_ISFIFO(status->st_mode) == 0)
	{
		close(rfd);
		return -1;
	}

	/*
	** Open the fifo for write only and leave this fd open.
	** This guarantees that there is always at least one
	** writer process. This prevents EOF indications being
	** returned from read() when there are no other writer
	** processes.
	**
	** Just opening the fifo "rw" works but it's undefined
	** by posix.
	*/

	if ((wfd = open(path, O_WRONLY)) == -1)
	{
		close(rfd);
		return -1;
	}

	/*
	** A sanity test on what we have just opened.
	*/

	if (fstat(wfd, status) == -1 || S_ISFIFO(status->st_mode) == 0)
	{
		close(rfd);
		close(wfd);
		return -1;
	}

	/*
	** Exclusively lock the fifo to prevent two invocations
	** deciding that there's no reader and opening this fifo
	** at the same time.
	*/

	if (lock && fcntl_lock(wfd, F_SETLK, F_WRLCK, SEEK_SET, 0, 0) == -1)
	{
		close(rfd);
		close(wfd);
		return -1;
	}

	/*
	** Now put the reader into blocking mode.
	*/

	if (nonblock_off(rfd) == -1)
	{
		close(rfd);
		close(wfd);
		return -1;
	}

	/*
	** Flaw: If someone unceremoniously unlinks our fifo, we won't know about
	** it and nothing will stop another invocation from creating a new fifo and
	** handling it. This process would sleep forever in select().
	*/

	return rfd;
}

#ifdef TEST

#include <stdio.h>

int main(int ac, char **av)
{
	printf("Testing: %s\n", *av);
	printf("Test not yet implemented\n");
	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
