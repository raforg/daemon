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

#ifndef LIBPROG_LIST_H
#define LIBPROG_LIST_H

#include <sys/types.h>

#include <prog/hdr.h>

typedef struct List List;
typedef struct Lister Lister;
typedef void list_destroy_t(void *);
typedef void *list_copy_t(void *);
typedef int list_cmp_t(const void *, const void *);
typedef void list_action_t(void *, size_t *);
typedef int list_query_t(void *, size_t *);

#define list_destroy(list) list_destroy_func(&(list))
#define lister_destroy(lister) lister_destroy_func(&(lister))

__BEGIN_DECLS
List *list_create __PROTO ((list_destroy_t *destroy));
List *list_make __PROTO ((list_destroy_t *destroy, ...));
List *list_copy __PROTO ((List *src, list_copy_t *copy));
void list_release __PROTO ((List *list));
void *list_destroy_func __PROTO ((List **list));
void *list_item __PROTO ((List *list, size_t index));
int list_item_int __PROTO ((List *list, size_t index));
int list_empty __PROTO ((List *list));
size_t list_length __PROTO ((List *list));
ssize_t list_last __PROTO ((List *list));
List *list_remove __PROTO ((List *list, size_t index));
List *list_remove_range __PROTO ((List *list, size_t index, size_t range));
List *list_insert __PROTO ((List *list, size_t index, void *item));
List *list_insert_list __PROTO ((List *dst, size_t index, List *src, list_copy_t *copy));
List *list_append __PROTO ((List *list, void *item));
List *list_append_list __PROTO ((List *dst, List *src, list_copy_t *copy));
List *list_prepend __PROTO ((List *list, void *item));
List *list_prepend_list __PROTO ((List *dst, List *src, list_copy_t *copy));
List *list_replace __PROTO ((List *list, size_t index, size_t range, void *item));
List *list_replace_list __PROTO ((List *dst, size_t index, size_t range, List *src, list_copy_t *copy));
List *list_extract __PROTO ((List *orig, size_t index, size_t range, list_copy_t *copy));
List *list_sort __PROTO ((List *list, list_cmp_t *cmp));
void list_apply __PROTO ((List *list, list_action_t *action));
ssize_t list_ask __PROTO ((List *list, ssize_t *index, list_query_t *query));
Lister *lister_create __PROTO ((List *list));
void lister_release __PROTO ((Lister * lister));
void *lister_destroy_func __PROTO ((Lister **lister));
int list_has_next __PROTO ((Lister *lister));
void *list_next __PROTO ((Lister *lister));
void lister_remove __PROTO ((Lister *lister));
__END_DECLS

#endif

/* vi:set ts=4 sw=4: */
