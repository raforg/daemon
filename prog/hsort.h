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

#ifndef LIBPROG_HSORT_H
#define LIBPROG_HSORT_H

#include <stdlib.h>

#include <prog/hdr.h>

typedef int hsort_cmp_t(const void *, const void *);

__BEGIN_DECLS
void hsort __PROTO ((void *base, size_t n, size_t size, hsort_cmp_t *cmp));
__END_DECLS

#endif

/* vi:set ts=4 sw=4: */
