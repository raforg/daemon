/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef FIFO_H
#define FIFO_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int fcntl_set_flag(int fd, int flag);
int fcntl_clear_flag(int fd, int flag);
int fcntl_lock(int fd, int cmd, int type, int whence, int start, int len);
int nonblock_set(int fd, int arg);
int nonblock_on(int fd);
int nonblock_off(int fd);
int fifo_exists(const char *path, int prepare);
int fifo_has_reader(const char *path, int prepare);
int fifo_open(const char *path, mode_t mode, int lock);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
