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

#ifndef LIBPROG_MEM_H
#define LIBPROG_MEM_H

#include <stdlib.h>

#include <prog/hdr.h>

#define mem_create(size, type) malloc((size) * sizeof(type))
#define mem_resize(mem, size) mem_resize_fn((void *)&(mem), (size) * sizeof(*(mem)))
#define mem_release(mem) free(mem)
#define mem_destroy(mem) mem_destroy_fn((void **)&(mem))

__BEGIN_DECLS
void *mem_resize_fn __PROTO ((void **mem, size_t size));
void *mem_destroy_fn __PROTO ((void **mem));
char *mem_strdup __PROTO ((const char *str));
__END_DECLS

#endif

/* vi:set ts=4 sw=4: */
