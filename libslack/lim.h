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

#ifndef LIBSLACK_LIM_H
#define LIBSLACK_LIM_H

#include <slack/hdr.h>

_begin_decls
long limit_arg _args ((void));
long limit_child _args ((void));
long limit_tick _args ((void));
long limit_group _args ((void));
long limit_open _args ((void));
long limit_stream _args ((void));
long limit_tzname _args ((void));
long limit_job _args ((void));
long limit_save_ids _args ((void));
long limit_version _args ((void));
long limit_pcanon _args ((const char *path));
long limit_fcanon _args ((int fd));
long limit_canon _args ((void));
long limit_pinput _args ((const char *path));
long limit_finput _args ((int fd));
long limit_input _args ((void));
long limit_pvdisable _args ((const char *path));
long limit_fvdisable _args ((int fd));
long limit_vdisable _args ((void));
long limit_plink _args ((const char *path));
long limit_flink _args ((int fd));
long limit_link _args ((void));
long limit_pname _args ((const char *path));
long limit_fname _args ((int fd));
long limit_name _args ((void));
long limit_ppath _args ((const char *path));
long limit_fpath _args ((int fd));
long limit_path _args ((void));
long limit_ppipe _args ((const char *path));
long limit_fpipe _args ((int fd));
long limit_pnotrunc _args ((const char *path));
long limit_fnotrunc _args ((int fd));
long limit_notrunc _args ((void));
long limit_pchown _args ((const char *path));
long limit_fchown _args ((int fd));
long limit_chown _args ((void));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
