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

#ifndef LIBSLACK_FIFO_H
#define LIBSLACK_FIFO_H

#include <sys/types.h>

#include <slack/hdr.h>

__START_DECLS
int fcntl_set_flag __PROTO ((int fd, int flag));
int fcntl_clear_flag __PROTO ((int fd, int flag));
int fcntl_lock __PROTO ((int fd, int cmd, int type, int whence, int start, int len));
int nonblock_set __PROTO ((int fd, int arg));
int nonblock_on __PROTO ((int fd));
int nonblock_off __PROTO ((int fd));
int fifo_exists __PROTO ((const char *path, int prepare));
int fifo_has_reader __PROTO ((const char *path, int prepare));
int fifo_open __PROTO ((const char *path, mode_t mode, int lock));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
