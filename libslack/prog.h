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

#ifndef LIBSLACK_PROG_H
#define LIBSLACK_PROG_H

#include <slack/hdr.h>
#include <slack/opt.h>
#include <slack/msg.h>

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif

__START_DECLS
void prog_init __PROTO ((void));
const char *prog_set_name __PROTO ((const char *name));
Options *prog_set_options __PROTO ((Options *options));
const char *prog_set_syntax __PROTO ((const char *syntax));
const char *prog_set_desc __PROTO ((const char *desc));
const char *prog_set_version __PROTO ((const char *version));
const char *prog_set_date __PROTO ((const char *date));
const char *prog_set_author __PROTO ((const char *author));
const char *prog_set_contact __PROTO ((const char *contact));
const char *prog_set_vendor __PROTO ((const char *vendor));
const char *prog_set_url __PROTO ((const char *url));
const char *prog_set_legal __PROTO ((const char *legal));
Msg *prog_set_out __PROTO ((Msg *out));
Msg *prog_set_err __PROTO ((Msg *err));
Msg *prog_set_dbg __PROTO ((Msg *dbg));
size_t prog_set_debug_level __PROTO ((size_t level));
size_t prog_set_verbosity_level __PROTO ((size_t level));
const char *prog_name __PROTO ((void));
const Options *prog_options __PROTO ((void));
const char *prog_syntax __PROTO ((void));
const char *prog_desc __PROTO ((void));
const char *prog_version __PROTO ((void));
const char *prog_date __PROTO ((void));
const char *prog_author __PROTO ((void));
const char *prog_contact __PROTO ((void));
const char *prog_vendor __PROTO ((void));
const char *prog_url __PROTO ((void));
const char *prog_legal __PROTO ((void));
Msg *prog_out __PROTO ((void));
Msg *prog_err __PROTO ((void));
Msg *prog_dbg __PROTO ((void));
size_t prog_debug_level __PROTO ((void));
size_t prog_verbosity_level __PROTO ((void));
int prog_out_fd __PROTO ((int fd));
int prog_out_stdout __PROTO ((void));
int prog_out_file __PROTO ((const char *path));
int prog_out_syslog __PROTO ((const char *ident, int option, int facility));
int prog_out_none __PROTO ((void));
int prog_err_fd __PROTO ((int fd));
int prog_err_stderr __PROTO ((void));
int prog_err_file __PROTO ((const char *path));
int prog_err_syslog __PROTO ((const char *ident, int option, int facility));
int prog_err_none __PROTO ((void));
int prog_dbg_fd __PROTO ((int fd));
int prog_dbg_stdout __PROTO ((void));
int prog_dbg_stderr __PROTO ((void));
int prog_dbg_file __PROTO ((const char *path));
int prog_dbg_syslog __PROTO ((const char *id, int option, int facility));
int prog_dbg_none __PROTO ((void));
int prog_opt_process __PROTO ((int ac, char **av));
void prog_usage_msg __PROTO ((const char *fmt, ...));
void prog_help_message __PROTO ((void));
void prog_version_message __PROTO ((void));
const char *prog_basename __PROTO ((const char *path));
extern Options prog_options_table[1];
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
