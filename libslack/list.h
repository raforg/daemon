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
* 20010215 raf <raf@raf.org>
*/

#ifndef LIBSLACK_LIST_H
#define LIBSLACK_LIST_H

#include <stdarg.h>

#include <sys/types.h>

#include <slack/hdr.h>
#include <slack/thread.h>

typedef struct List List;
typedef struct Lister Lister;
typedef void list_release_t(void *item);
typedef void *list_copy_t(const void *item);
typedef int list_cmp_t(const void *a, const void *b);
typedef void list_action_t(void *item, size_t *index, void *data);
typedef void *list_map_t(void *item, size_t *index, void *data);
typedef int list_query_t(void *item, size_t *index, void *data);

_start_decls
List *list_create _args ((list_release_t *destroy));
List *list_make _args ((list_release_t *destroy, ...));
List *list_vmake _args ((list_release_t *destroy, va_list args));
List *list_copy _args ((const List *src, list_copy_t *copy));
List *list_create_locked _args ((Locker *locker, list_release_t *destroy));
List *list_make_locked _args ((Locker *locker, list_release_t *destroy, ...));
List *list_vmake_locked _args ((Locker *locker, list_release_t *destroy, va_list args));
List *list_copy_locked _args ((Locker *locker, const List *src, list_copy_t *copy));
void list_release _args ((List *list));
void *list_destroy _args ((List **list));
int list_own _args ((List *list, list_release_t *destroy));
list_release_t *list_disown _args ((List *list));
void *list_item _args ((const List *list, size_t index));
int list_item_int _args ((const List *list, size_t index));
int list_empty _args ((const List *list));
size_t list_length _args ((const List *list));
ssize_t list_last _args ((const List *list));
List *list_remove _args ((List *list, size_t index));
List *list_remove_range _args ((List *list, size_t index, size_t range));
List *list_insert _args ((List *list, size_t index, void *item));
List *list_insert_int _args ((List *list, size_t index, int item));
List *list_insert_list _args ((List *list, size_t index, const List *src, list_copy_t *copy));
List *list_append _args ((List *list, void *item));
List *list_append_int _args ((List *list, int item));
List *list_append_list _args ((List *list, const List *src, list_copy_t *copy));
List *list_prepend _args ((List *list, void *item));
List *list_prepend_int _args ((List *list, int item));
List *list_prepend_list _args ((List *list, const List *src, list_copy_t *copy));
List *list_replace _args ((List *list, size_t index, size_t range, void *item));
List *list_replace_int _args ((List *list, size_t index, size_t range, int item));
List *list_replace_list _args ((List *list, size_t index, size_t range, const List *src, list_copy_t *copy));
List *list_extract _args ((const List *list, size_t index, size_t range, list_copy_t *copy));
List *list_push _args ((List *list, void *item));
List *list_push_int _args ((List *list, int item));
void *list_pop _args ((List *list));
int list_pop_int _args ((List *list));
void *list_shift _args ((List *list));
int list_shift_int _args ((List *list));
List *list_unshift _args ((List *list, void *item));
List *list_unshift_int _args ((List *list, int item));
List *list_splice _args ((List *list, size_t index, size_t range, list_copy_t *copy));
List *list_sort _args ((List *list, list_cmp_t *cmp));
void list_apply _args ((List *list, list_action_t *action, void *data));
List *list_map _args ((List *list, list_release_t *destroy, list_map_t *map, void *data));
List *list_grep _args ((List *list, list_query_t *grep, void *data));
ssize_t list_ask _args ((List *list, ssize_t *index, list_query_t *query, void *data));
Lister *lister_create _args ((List *list));
void lister_release _args ((Lister * lister));
void *lister_destroy _args ((Lister **lister));
int lister_has_next _args ((Lister *lister));
void *lister_next _args ((Lister *lister));
int lister_next_int _args ((Lister *lister));
void lister_remove _args ((Lister *lister));
int list_has_next _args ((List *list));
void list_break _args ((List *list));
void *list_next _args ((List *list));
int list_next_int _args ((List *list));
void list_remove_current _args ((List *list));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
