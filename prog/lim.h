/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (C) 1999 raf <raf2@zip.com.au>
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
*/

#ifndef LIBPROG_LIM_H
#define LIBPROG_LIM_H

#include <prog/hdr.h>

__BEGIN_DECLS
long limit_arg __PROTO ((void));
long limit_child __PROTO ((void));
long limit_tick __PROTO ((void));
long limit_group __PROTO ((void));
long limit_open __PROTO ((void));
long limit_stream __PROTO ((void));
long limit_tzname __PROTO ((void));
long limit_job __PROTO ((void));
long limit_save_ids __PROTO ((void));
long limit_version __PROTO ((void));
long limit_pcanon __PROTO ((const char *path));
long limit_fcanon __PROTO ((int fd));
long limit_canon __PROTO ((void));
long limit_pinput __PROTO ((const char *path));
long limit_finput __PROTO ((int fd));
long limit_input __PROTO ((void));
long limit_pvdisable __PROTO ((const char *path));
long limit_fvdisable __PROTO ((int fd));
long limit_vdisable __PROTO ((void));
long limit_plink __PROTO ((const char *path));
long limit_flink __PROTO ((int fd));
long limit_link __PROTO ((void));
long limit_pname __PROTO ((const char *path));
long limit_fname __PROTO ((int fd));
long limit_name __PROTO ((void));
long limit_ppath __PROTO ((const char *path));
long limit_fpath __PROTO ((int fd));
long limit_path __PROTO ((void));
long limit_ppipe __PROTO ((const char *path));
long limit_fpipe __PROTO ((int fd));
long limit_pnotrunc __PROTO ((const char *path));
long limit_fnotrunc __PROTO ((int fd));
long limit_notrunc __PROTO ((void));
long limit_pchown __PROTO ((const char *path));
long limit_fchown __PROTO ((int fd));
long limit_chown __PROTO ((void));
__END_DECLS

#endif

/* vi:set ts=4 sw=4: */
