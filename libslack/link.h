/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999-2002 raf <raf@raf.org>
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
* 20020916 raf <raf@raf.org>
*/

#ifndef LIBSLACK_LINK_H
#define LIBSLACK_LINK_H

#include <slack/hdr.h>

typedef struct slink_t slink_t;
typedef struct dlink_t dlink_t;

struct slink_t
{
	void *next;
};

struct dlink_t
{
	void *next;
	void *prev;
};

_begin_decls
int slink_has_next _args ((void *link));
void *slink_next _args ((void *link));
int dlink_has_next _args ((void *link));
void *dlink_next _args ((void *link));
int dlink_has_prev _args ((void *link));
void *dlink_prev _args ((void *link));
void *slink_insert _args ((void *link, void *item));
void *dlink_insert _args ((void *link, void *item));
void *slink_remove _args ((void *link));
void *dlink_remove _args ((void *link));
void *slink_freelist_init _args ((void *freelist, size_t nelem, size_t size));
void *dlink_freelist_init _args ((void *freelist, size_t nelem, size_t size));
void *slink_freelist_attach _args ((void *freelist1, void *freelist2));
void *dlink_freelist_attach _args ((void *freelist1, void *freelist2));
void *slink_alloc _args ((void **freelist));
void *dlink_alloc _args ((void **freelist));
void *slink_free _args ((void **freelist, void *item));
void *dlink_free _args ((void **freelist, void *item));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
