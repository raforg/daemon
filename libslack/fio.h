/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999-2002 raf <raf@raf.org>
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
* 20020916 raf <raf@raf.org>
*/

#ifndef LIBSLACK_FIO_H
#define LIBSLACK_FIO_H

#include <sys/types.h>

#include <slack/hdr.h>

_begin_decls
char *fgetline _args ((char *line, size_t size, FILE *stream));
char *fgetline_unlocked _args ((char *line, size_t size, FILE *stream));
int read_timeout _args ((int fd, long sec, long usec));
int write_timeout _args ((int fd, long sec, long usec));
int rw_timeout _args ((int fd, long sec, long usec));
int nap _args ((long sec, long usec));
int fcntl_set_flag _args ((int fd, int flag));
int fcntl_clear_flag _args ((int fd, int flag));
int fcntl_lock _args ((int fd, int cmd, int type, int whence, int start, int len));
int nonblock_set _args ((int fd, int arg));
int nonblock_on _args ((int fd));
int nonblock_off _args ((int fd));
int fifo_exists _args ((const char *path, int prepare));
int fifo_has_reader _args ((const char *path, int prepare));
int fifo_open _args ((const char *path, mode_t mode, int lock, int *writefd));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
