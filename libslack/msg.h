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

#ifndef LIBSLACK_MSG_H
#define LIBSLACK_MSG_H

#include <stdarg.h>

#include <slack/hdr.h>

#ifndef MSG_SIZE
#define MSG_SIZE 8192
#endif

typedef struct Msg Msg;

#undef msg_destroy

__START_DECLS
void msg_release __PROTO ((Msg *msg));
#define msg_destroy(msg) msg_destroy_fn(&(msg))
void *msg_destroy_fn __PROTO ((Msg **msg));
void vmsg_out __PROTO ((Msg *dst, const char *fmt, va_list args));
void msg_out __PROTO ((Msg *dst, const char *fmt, ...));
Msg *msg_create_fd __PROTO ((int fd));
Msg *msg_create_stderr __PROTO ((void));
Msg *msg_create_stdout __PROTO ((void));
Msg *msg_create_file __PROTO ((const char *path));
Msg *msg_create_syslog __PROTO ((const char *ident, int option, int facility));
Msg *msg_create_plex __PROTO ((Msg *msg1, Msg *msg2));
int msg_add_plex __PROTO ((Msg *msg, Msg *item));
const char *msg_set_timestamp_format __PROTO ((const char *format));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
