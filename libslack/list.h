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

#ifndef LIBSLACK_LIST_H
#define LIBSLACK_LIST_H

#include <stdarg.h>

#include <sys/types.h>

#include <slack/hdr.h>

typedef struct List List;
typedef struct Lister Lister;
typedef void list_destroy_t(void *item);
typedef void *list_copy_t(const void *item);
typedef int list_cmp_t(const void *a, const void *b);
typedef void list_action_t(void *item, size_t *index, void *data);
typedef void *list_map_t(void *item, size_t *index, void *data);
typedef int list_query_t(void *item, size_t *index, void *data);

#undef list_destroy
#undef lister_destroy

__START_DECLS
List *list_create __PROTO ((list_destroy_t *destroy));
List *list_make __PROTO ((list_destroy_t *destroy, ...));
List *list_vmake __PROTO ((list_destroy_t *destroy, va_list args));
List *list_copy __PROTO ((const List *src, list_copy_t *copy));
void list_release __PROTO ((List *list));
#define list_destroy(list) list_destroy_func(&(list))
void *list_destroy_func __PROTO ((List **list));
int list_own __PROTO ((List *list, list_destroy_t *destroy));
list_destroy_t *list_disown __PROTO ((List *list));
void *list_item __PROTO ((const List *list, size_t index));
int list_item_int __PROTO ((const List *list, size_t index));
int list_empty __PROTO ((const List *list));
size_t list_length __PROTO ((const List *list));
ssize_t list_last __PROTO ((const List *list));
List *list_remove __PROTO ((List *list, size_t index));
List *list_remove_range __PROTO ((List *list, size_t index, size_t range));
List *list_insert __PROTO ((List *list, size_t index, void *item));
List *list_insert_int __PROTO ((List *list, size_t index, int item));
List *list_insert_list __PROTO ((List *list, size_t index, const List *src, list_copy_t *copy));
List *list_append __PROTO ((List *list, void *item));
List *list_append_int __PROTO ((List *list, int item));
List *list_append_list __PROTO ((List *list, const List *src, list_copy_t *copy));
List *list_prepend __PROTO ((List *list, void *item));
List *list_prepend_int __PROTO ((List *list, int item));
List *list_prepend_list __PROTO ((List *list, const List *src, list_copy_t *copy));
List *list_replace __PROTO ((List *list, size_t index, size_t range, void *item));
List *list_replace_int __PROTO ((List *list, size_t index, size_t range, int item));
List *list_replace_list __PROTO ((List *list, size_t index, size_t range, const List *src, list_copy_t *copy));
List *list_extract __PROTO ((const List *list, size_t index, size_t range, list_copy_t *copy));
List *list_push __PROTO ((List *list, void *item));
List *list_push_int __PROTO ((List *list, int item));
void *list_pop __PROTO ((List *list));
int list_pop_int __PROTO ((List *list));
void *list_shift __PROTO ((List *list));
int list_shift_int __PROTO ((List *list));
List *list_unshift __PROTO ((List *list, void *item));
List *list_unshift_int __PROTO ((List *list, int item));
List *list_splice __PROTO ((List *list, size_t index, size_t range, list_copy_t *copy));
List *list_sort __PROTO ((List *list, list_cmp_t *cmp));
void list_apply __PROTO ((List *list, list_action_t *action, void *data));
List *list_map __PROTO ((List *list, list_destroy_t *destroy, list_map_t *map, void *data));
List *list_grep __PROTO ((List *list, list_query_t *grep, void *data));
ssize_t list_ask __PROTO ((List *list, ssize_t *index, list_query_t *query, void *data));
Lister *lister_create __PROTO ((List *list));
void lister_release __PROTO ((Lister * lister));
#define lister_destroy(lister) lister_destroy_func(&(lister))
void *lister_destroy_func __PROTO ((Lister **lister));
int lister_has_next __PROTO ((Lister *lister));
void *lister_next __PROTO ((Lister *lister));
int lister_next_int __PROTO ((Lister *lister));
void lister_remove __PROTO ((Lister *lister));
int list_has_next __PROTO ((List *list));
void *list_next __PROTO ((List *list));
int list_next_int __PROTO ((List *list));
void list_remove_current __PROTO ((List *list));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
