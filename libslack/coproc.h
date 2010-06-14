/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999-2010 raf <raf@raf.org>
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
* 20100612 raf <raf@raf.org>
*/

#ifndef H_COPROC_H
#define H_COPROC_H

#include <termios.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <slack/hdr.h>

_begin_decls
pid_t coproc_open(int *to, int *from, int *err, const char *cmd, char * const *argv, char * const *envv, void (*action)(void *data), void *data);
int coproc_close(pid_t pid, int *to, int *from, int *err);
pid_t coproc_pty_open(int *masterfd, char *slavename, size_t slavenamesize, const struct termios *slave_termios, const struct winsize *slave_winsize, const char *cmd, char * const *argv, char * const *envv, void (*action)(void *data), void *data);
int coproc_pty_close(pid_t pid, int *masterfd, const char *slavename);
_end_decls

#endif

/* vi:set ts=4 sw=4: */
