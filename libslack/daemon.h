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

#ifndef LIBSLACK_DAEMON_H
#define LIBSLACK_DAEMON_H

#include <slack/hdr.h>

#ifndef ROOT_PID_DIR
#define ROOT_PID_DIR "/var/run"
#endif

#ifndef USER_PID_DIR
#define USER_PID_DIR "/tmp"
#endif

#ifndef ROOT_DIR
#define ROOT_DIR "/"
#endif

#ifndef ETC_DIR
#define ETC_DIR "/etc"
#endif

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif

#ifndef PATH_SEP_STR
#define PATH_SEP_STR "/"
#endif

#ifndef PATH_LIST_SEP
#define PATH_LIST_SEP ':'
#endif

typedef void daemon_config_parser_t(void *obj, const char *path, char *line, size_t lineno);

_begin_decls
int daemon_started_by_init _args ((void));
int daemon_started_by_inetd _args ((void));
int daemon_prevent_core _args ((void));
int daemon_revoke_privileges _args ((void));
int daemon_become_user _args ((uid_t uid, gid_t gid, char *user));
char *daemon_absolute_path _args ((const char *path));
int daemon_path_is_safe _args ((const char *path));
void *daemon_parse_config _args ((const char *path, void *obj, daemon_config_parser_t *parser));
int daemon_init _args ((const char *name));
int daemon_close _args ((void));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
