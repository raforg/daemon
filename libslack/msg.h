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
* 20011109 raf <raf@raf.org>
*/

#ifndef LIBSLACK_MSG_H
#define LIBSLACK_MSG_H

#include <stdarg.h>

#include <sys/syslog.h>

#include <slack/hdr.h>
#include <slack/locker.h>

#ifndef MSG_SIZE
#define MSG_SIZE 8192
#endif

typedef struct Msg Msg;
typedef void msg_out_t(void *data, const void *msg, size_t msglen);
typedef void msg_release_t(void *data);

_begin_decls
Msg *msg_create _args ((int type, msg_out_t *out, void *data, msg_release_t *destroy));
Msg *msg_create_with_locker _args ((Locker *locker, int type, msg_out_t *out, void *data, msg_release_t *destroy));
int msg_rdlock _args ((Msg *msg));
int msg_wrlock _args ((Msg *msg));
int msg_unlock _args ((Msg *msg));
void msg_release _args ((Msg *msg));
void *msg_destroy _args ((Msg **msg));
void msg_out _args ((Msg *dst, const char *fmt, ...));
void msg_out_unlocked _args ((Msg *dst, const char *fmt, ...));
void vmsg_out _args ((Msg *dst, const char *fmt, va_list args));
void vmsg_out_unlocked _args ((Msg *dst, const char *fmt, va_list args));
Msg *msg_create_fd _args ((int fd));
Msg *msg_create_fd_with_locker _args ((Locker *locker, int fd));
Msg *msg_create_stderr _args ((void));
Msg *msg_create_stderr_with_locker _args ((Locker *locker));
Msg *msg_create_stdout _args ((void));
Msg *msg_create_stdout_with_locker _args ((Locker *locker));
Msg *msg_create_file _args ((const char *path));
Msg *msg_create_file_with_locker _args ((Locker *locker, const char *path));
Msg *msg_create_syslog _args ((const char *ident, int option, int facility, int priority));
Msg *msg_create_syslog_with_locker _args ((Locker *locker, const char *ident, int option, int facility, int priority));
Msg *msg_syslog_set_facility _args ((Msg *msg, int facility));
Msg *msg_syslog_set_facility_unlocked _args ((Msg *msg, int facility));
Msg *msg_syslog_set_priority _args ((Msg *msg, int priority));
Msg *msg_syslog_set_priority_unlocked _args ((Msg *msg, int priority));
Msg *msg_create_plex _args ((Msg *msg1, Msg *msg2));
Msg *msg_create_plex_with_locker _args ((Locker *locker, Msg *msg1, Msg *msg2));
int msg_add_plex _args ((Msg *msg, Msg *item));
int msg_add_plex_unlocked _args ((Msg *msg, Msg *item));
const char *msg_set_timestamp_format _args ((const char *format));
int msg_set_timestamp_format_locker _args ((Locker *locker));
int syslog_lookup_facility _args ((const char *facility));
int syslog_lookup_priority _args ((const char *priority));
const char *syslog_facility_str _args ((int spec));
const char *syslog_priority_str _args ((int spec));
int syslog_parse _args ((const char *spec, int *facility, int *priority));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
