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

/*

=head1 NAME

I<libslack(list)> - list module

=head1 SYNOPSIS

    #include <slack/list.h>

    typedef struct List List;
    typedef struct Lister Lister;
    typedef void list_release_t(void *item);
    typedef void *list_copy_t(const void *item);
    typedef int list_cmp_t(const void *a, const void *b);
    typedef void list_action_t(void *item, size_t *index, void *data);
    typedef void *list_map_t(void *item, size_t *index, void *data);
    typedef int list_query_t(void *item, size_t *index, void *data);

    List *list_create(list_release_t *destroy);
    List *list_make(list_release_t *destroy, ...);
    List *list_vmake(list_release_t *destroy, va_list args);
    List *list_copy(const List *src, list_copy_t *copy);
    List *list_create_locked(Locker *locker, list_release_t *destroy);
    List *list_make_locked(Locker *locker, list_release_t *destroy, ...);
    List *list_vmake_locked(Locker *locker, list_release_t *destroy, va_list args);
    List *list_copy_locked(Locker *locker, const List *src, list_copy_t *copy);
    void list_release(List *list);
    void *list_destroy(List **list);
    int list_own(List *list, list_release_t *destroy);
    list_release_t *list_disown(List *list);
    void *list_item(const List *list, size_t index);
    int list_item_int(const List *list, size_t index);
    int list_empty(const List *list);
    size_t list_length(const List *list);
    ssize_t list_last(const List *list);
    List *list_remove(List *list, size_t index);
    List *list_remove_range(List *list, size_t index, size_t range);
    List *list_insert(List *list, size_t index, void *item);
    List *list_insert_int(List *list, size_t index, int item);
    List *list_insert_list(List *list, size_t index, const List *src, list_copy_t *copy);
    List *list_append(List *list, void *item);
    List *list_append_int(List *list, int item);
    List *list_append_list(List *list, const List *src, list_copy_t *copy);
    List *list_prepend(List *list, void *item);
    List *list_prepend_int(List *list, int item);
    List *list_prepend_list(List *list, const List *src, list_copy_t *copy);
    List *list_replace(List *list, size_t index, size_t range, void *item);
    List *list_replace_int(List *list, size_t index, size_t range, int item);
    List *list_replace_list(List *list, size_t index, size_t range, const List *src, list_copy_t *copy);
    List *list_extract(const List *list, size_t index, size_t range, list_copy_t *copy);
    List *list_push(List *list, void *item);
    List *list_push_int(List *list, int item);
    void *list_pop(List *list);
    int list_pop_int(List *list);
    void *list_shift(List *list);
    int list_shift_int(List *list);
    List *list_unshift(List *list, void *item);
    List *list_unshift_int(List *list, int item);
    List *list_splice(List *list, size_t index, size_t range, list_copy_t *copy);
    List *list_sort(List *list, list_cmp_t *cmp);
    void list_apply(List *list, list_action_t *action, void *data);
    List *list_map(List *list, list_release_t *destroy, list_map_t *map, void *data);
    List *list_grep(List *list, list_query_t *grep, void *data);
    ssize_t list_ask(List *list, ssize_t *index, list_query_t *query, void *data);
    Lister *lister_create(List *list);
    void lister_release(Lister * lister);
    void *lister_destroy(Lister **lister);
    int lister_has_next(Lister *lister);
    void *lister_next(Lister *lister);
    int lister_next_int(Lister *lister);
    void lister_remove(Lister *lister);
    int list_has_next(List *list);
    void list_break(List *list);
    void *list_next(List *list);
    int list_next_int(List *list);
    void list_remove_current(List *list);

=head1 DESCRIPTION

This module provides functions for manipulating and iterating over lists of
homogeneous data (or heterogeneous data if it's polymorphic). I<Lists> may
own their items. I<Lists> created with a non-C<NULL> destroy function use
that function to destroy an item when it is removed from the list and to
destroy each item when the list itself is destroyed. Be careful not to
insert items owned by one list into a list that doesn't own its own items
unless you know that the source list (and all of the shared items) will
outlive the destination list.

=over 4

=cut

*/

#include "std.h"

#include "list.h"
#include "mem.h"
#include "err.h"
#include "hsort.h"
#include "thread.h"

#define xor(a, b) (!(a) ^ !(b))
#define iff(a, b) !xor(a, b)
#define implies(a, b) (!(a) || (b))

/* Minimum list length: must be a power of 2 */

static const size_t MIN_LIST_LENGTH = 4;

struct List
{
	size_t size;             /* number of item slots allocated */
	size_t length;           /* number of items used */
	void **list;             /* vector of items (void *) */
	list_release_t *destroy; /* item destructor, if any */
	Lister *lister;          /* built-in iterator */
	Locker *locker;          /* locking strategy for this object */
};

struct Lister
{
	pthread_mutex_t lock;    /* lock for this object */
	pthread_t owner;         /* the thread that owns the lock */
	List *list;              /* the list being iterated over */
	ssize_t index;           /* the index of the current item */
};

/*

C<int grow(List *list, size_t items)>

Allocates enough memory to add C<item> extra items to C<list> if necessary.
On success, returns 0. On error, returns -1.

*/

static int grow(List *list, size_t items)
{
	int grown = 0;

	while (list->length + items > list->size)
	{
		if (list->size)
			list->size <<= 1;
		else
			list->size = MIN_LIST_LENGTH;

		grown = 1;
	}

	if (grown)
		return mem_resize(&list->list, list->size) ? 0 : -1;

	return 0;
}

/*

C<int shrink(List *list, size_t items)>

Allocates less memory for removing C<items> items from C<list> if necessary.
On success, returns 0. On error, returns -1.

*/

static int shrink(List *list, size_t items)
{
	int shrunk = 0;

	while (list->length - items < list->size >> 1)
	{
		if (list->size == MIN_LIST_LENGTH)
			break;

		list->size >>= 1;
		shrunk = 1;
	}

	if (shrunk)
		return mem_resize(&list->list, list->size) ? 0 : -1;

	return 0;
}

/*

C<int expand(List *list, ssize_t index, size_t range)>

Slides C<list>'s items, starting at C<index>, C<range> slots to the right to
make room for more. On success, returns 0. On error, returns -1.

*/

static int expand(List *list, ssize_t index, size_t range)
{
	if (grow(list, range) == -1)
		return -1;

	memmove(list->list + index + range, list->list + index, (list->length - index) * sizeof(*list->list));
	list->length += range;

	return 0;
}

/*

C<int contract(List *list, ssize_t index, size_t range)>

Slides C<list>'s items, starting at C<index> + C<range>, C<range> positions
to the left to close a gap starting at C<index>. On success, returns 0. On
error, returns -1.

*/

static int contract(List *list, ssize_t index, size_t range)
{
	memmove(list->list + index, list->list + index + range, (list->length - index - range) * sizeof(*list->list));

	if (shrink(list, range) == -1)
		return -1;

	list->length -= range;

	return 0;
}

/*

C<int adjust(List *list, ssize_t index, size_t range, size_t length)>

Expands or contracts C<list> as required so that C<list[index + range ..]>
occupies C<list[index + length ..]>. On success, returns 0. On error,
returns -1.

*/

static int adjust(List *list, ssize_t index, size_t range, size_t length)
{
	if (range < length)
		return expand(list, index + range, length - range);

	if (range > length)
		return contract(list, index + length, range - length);

	return 0;
}

/*

C<void killitems(List *list, size_t index, size_t range)>

Destroys the items in C<list> ranging from C<index> to C<range>.

*/

static void killitems(List *list, size_t index, size_t range)
{
	while (range--)
	{
		if (list->destroy)
			list->destroy(list->list[index]);
		list->list[index++] = NULL;
	}
}

#define ptry(action, ret) { if (action) return ret; }

/*

C<int list_rdlock(List *list)>

Read locks C<list>. On success, returns 0. On error, return -1 with C<errno>
set appropriately.

*/

static int list_rdlock(List *list)
{
	if (!list || !list->locker)
		return 0;

	return locker_rdlock(list->locker);
}

/*

C<int list_wrlock(List *list)>

Write locks C<list>. On success, returns 0. On error, return -1 with
C<errno> set appropriately.

*/

static int list_wrlock(List *list)
{
	if (!list || !list->locker)
		return 0;

	return locker_wrlock(list->locker);
}

/*

C<int list_unlock(List *list)>

Unlocks a read or write lock obtained with I<list_rdlock()> or
I<list_wrlock()>. On success, returns 0. On error, returns -1 with C<errno>
set appropriately.

*/

static int list_unlock(List *list)
{
	if (!list || !list->locker)
		return 0;

	return locker_unlock(list->locker);
}

/*

=item C<List *list_create(list_release_t *destroy)>

Creates a I<List> with C<destroy> as its item destructor. On success,
returns the new list. On error, returns C<NULL>.

=cut

*/

List *list_create(list_release_t *destroy)
{
	return list_create_locked(NULL, destroy);
}

/*

=item C<List *list_make(list_release_t *destroy, ...)>

Creates a I<List> with C<destroy> as its item destructor and the remaining
arguments as its initial items. Multiple threads accessing this map will be
synchronised by C<locker>. On success, returns the new list. On error,
return C<NULL>.

=cut

*/

List *list_make(list_release_t *destroy, ...)
{
	List *list;
	va_list args;
	va_start(args, destroy);
	list = list_vmake_locked(NULL, destroy, args);
	va_end(args);
	return list;
}

/*

=item C<List *list_vmake(list_release_t *destroy, va_list args)>

Equivalent to I<list_make()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

List *list_vmake(list_release_t *destroy, va_list args)
{
	return list_vmake_locked(NULL, destroy, args);
}

/*

=item C<List *list_copy(const List *src, list_copy_t *copy)>

Creates a clone of C<src> using C<copy> as the copy constructor (if not
C<NULL>). On success, returns the clone. On error, returns C<NULL>.

=cut

*/

List *list_copy(const List *src, list_copy_t *copy)
{
	return list_copy_locked(NULL, src, copy);
}

/*

=item C<List *list_create_locked(Locker locker, list_release_t *destroy)>

Creates a I<List> with C<destroy> as its item destructor. Multiple threads
accessing this list will be synchronised by C<locker>. On success, returns
the new list. On error, returns C<NULL>.

=cut

*/

List *list_create_locked(Locker *locker, list_release_t *destroy)
{
	List *list;

	if (!(list = mem_new(List)))
		return NULL;

	list->size = list->length = 0;
	list->list = NULL;
	list->destroy = destroy;
	list->lister = NULL;
	list->locker = locker;

	return list;
}

/*

=item C<List *list_make_locked(Locker *locker, list_release_t *destroy, ...)>

Creates a I<List> with C<destroy> as its item destructor and the remaining
arguments as its initial items. Multiple threads accessing this list will be
synchronised by C<locker>. On success, returns the new list. On error,
return C<NULL>.

=cut

*/

List *list_make_locked(Locker *locker, list_release_t *destroy, ...)
{
	List *list;
	va_list args;
	va_start(args, destroy);
	list = list_vmake_locked(locker, destroy, args);
	va_end(args);
	return list;
}

/*

=item C<List *list_vmake_locked(Locker *locker, list_release_t *destroy, va_list args)>

Equivalent to I<list_make_locked()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

List *list_vmake_locked(Locker *locker, list_release_t *destroy, va_list args)
{
	List *list;
	void *item;

	if (!(list = list_create_locked(locker, destroy)))
		return NULL;

	while ((item = va_arg(args, void *)) != NULL)
		list_append(list, item);

	return list;
}

/*

=item C<List *list_copy_locked(Locker *locker, const List *src, list_copy_t *copy)>

Creates a clone of C<src> using C<copy> as the copy constructor (if not
C<NULL>). Multiple threads accessing this list will be synchronised by
C<locker>. On success, returns the clone. On error, returns C<NULL>.

=cut

*/

List *list_copy_locked(Locker *locker, const List *src, list_copy_t *copy)
{
	List *list;

	if (!src)
		return NULL;

	list = list_extract(src, 0, src->length, copy);
	if (!list)
		return NULL;

	list->locker = locker;

	return list;
}

/*

=item C<void list_release(List *list)>

Releases (deallocates) C<list>, destroying its items if necessary.

=cut

*/

void list_release(List *list)
{
	if (!list)
		return;

	if (list_wrlock(list) == -1)
		return;

	if (list->list)
	{
		killitems(list, 0, list->length);
		mem_release(list->list);
	}

	lister_release(list->lister);
	list_unlock(list);
	mem_release(list);
}

/*

=item C<void *list_destroy(List **list)>

Destroys (deallocates and sets to C<NULL>) C<*list>. Returns C<NULL>.
B<Note:> lists shared by multiple threads must not be destroyed until after
the threads have finished with it.

=cut

*/

void *list_destroy(List **list)
{
	if (list && *list)
	{
		list_release(*list);
		*list = NULL;
	}

	return NULL;
}

/*

=item C<int list_own(List *list, list_release_t *destroy)>

Causes C<list> to take ownership of its items. The items will be destroyed
using C<destroy> when they are removed or when C<list> is destroyed.
On success, returns 0. On error, returns -1.

=cut

*/

int list_own(List *list, list_release_t *destroy)
{
	if (!list || !destroy)
		return -1;

	if (list_wrlock(list) == -1)
		return -1;

	list->destroy = destroy;

	if (list_unlock(list) == -1)
		return -1;

	return 0;
}

/*

=item C<list_release_t *list_disown(List *list)>

Causes C<list> to relinquish ownership of its items. The items will not be
destroyed when they are removed from C<list> or when C<list> is destroyed.
On success, returns the previous destroy function, if any. On error, returns
C<NULL>.

=cut

*/

list_release_t *list_disown(List *list)
{
	list_release_t *destroy;

	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	destroy = list->destroy;
	list->destroy = NULL;

	if (list_unlock(list) == -1)
		return NULL;

	return destroy;
}

/*

C<void *list_item_locked(const List *list, size_t index, int lock_list)>

Returns the C<index>'th item in C<list>. If C<lock_list> is non-zero,
C<list> is read locked. On error, returns C<NULL>.

*/

static void *list_item_locked(const List *list, size_t index, int lock_list)
{
	void *item;

	if (!list)
		return NULL;

	if (lock_list && list_rdlock((List *)list) == -1)
		return NULL;

	if (index >= list->length)
	{
		if (lock_list)
			list_unlock((List *)list);
		return NULL;
	}

	item = list->list[index];

	if (lock_list && list_unlock((List *)list) == -1)
		return NULL;

	return item;
}

/*

=item C<void *list_item(const List *list, size_t index)>

Returns the C<index>'th item in C<list>. On error, returns C<NULL>.

=cut

*/

void *list_item(const List *list, size_t index)
{
	return list_item_locked(list, index, 1);
}

/*

C<void *list_item_unlocked(const List *list, size_t index)>

Returns the C<index>'th item in C<list> without locking C<list>. On error,
returns C<NULL>.

*/

static void *list_item_unlocked(const List *list, size_t index)
{
	return list_item_locked(list, index, 0);
}

/*

C<int list_item_int_locked(const List *list, size_t index, int lock_list)>

Returns the C<index>'th item in C<list> as an integer. If C<lock_list> is
non-zero, C<list> is read locked. C<list> is not On error, returns 0.

*/

static int list_item_int_locked(const List *list, size_t index, int lock_list)
{
	int item;

	if (!list)
		return 0;

	if (lock_list && list_rdlock((List *)list) == -1)
		return 0;

	if (index >= list->length)
		return 0;

	item = (int)(list->list[index]);

	if (lock_list && list_unlock((List *)list) == -1)
		return 0;

	return item;
}

/*

=item C<int list_item_int(const List *list, size_t index)>

Returns the C<index>'th item in C<list> as an integer. On error, returns 0.

=cut

*/

int list_item_int(const List *list, size_t index)
{
	return list_item_int_locked(list, index, 1);
}

/*

C<int list_item_int_unlocked(const List *list, size_t index)>

Returns the C<index>'th item in C<list> as an integer without locking
C<list>. On error, returns 0.

*/

static int list_item_int_unlocked(const List *list, size_t index)
{
	return list_item_int_locked(list, index, 0);
}

/*

=item C<int list_empty(const List *list)>

Returns whether or not C<list> is empty.

=cut

*/

int list_empty(const List *list)
{
	int empty;

	if (list_rdlock((List *)list) == -1)
		return 0;

	empty = !list || !list->length;

	if (list_unlock((List *)list) == -1)
		return 0;

	return empty;
}

/*

=item C<size_t list_length(const List *list)>

Returns the length of C<list>.

=cut

*/

size_t list_length(const List *list)
{
	size_t length;

	if (!list)
		return 0;

	if (list_rdlock((List *)list) == -1)
		return 0;

	length = list->length;

	if (list_unlock((List *)list) == -1)
		return 0;

	return length;
}

/*

=item C<ssize_t list_last(const List *list)>

Returns the index of the last item in C<list>, or -1 if there are no items.

=cut

*/

ssize_t list_last(const List *list)
{
	ssize_t last;

	if (list_rdlock((List *)list) == -1)
		return 0;

	last = (list) ? list->length - 1 : -1;

	if (list_unlock((List *)list) == -1)
		return 0;

	return last;
}

/*

=item C<List *list_remove(List *list, size_t index)>

Removes the C<index>'th item from C<list>. On success, returns C<list>. On
error, returns C<NULL>.

=cut

*/

List *list_remove(List *list, size_t index)
{
	return list_remove_range(list, index, 1);
}

/*

C<List *list_remove_unlocked(List *list, size_t index)>

Removes the C<index>'th item from C<list> without locking C<list>. On
success, returns C<list>. On error, returns C<NULL>.

*/

static List *list_remove_range_unlocked(List *list, size_t index, size_t range);

static List *list_remove_unlocked(List *list, size_t index)
{
	return list_remove_range_unlocked(list, index, 1);
}

/*

C<List *list_remove_range_locked(List *list, size_t index, size_t range, int lock_list)>

Removes C<range> items from C<list> starting at C<index>. On success, returns
C<list>. On error, returns C<NULL>.

*/

static List *list_remove_range_locked(List *list, size_t index, size_t range, int lock_list)
{
	if (!list)
	  return NULL;

	if (lock_list && list_wrlock(list) == -1)
		return NULL;

	if (list->length < index + range)
	{
		if (lock_list)
			list_unlock(list);
		return NULL;
	}

	killitems(list, index, range);

	if (contract(list, index, range) == -1)
	{
		if (lock_list)
			list_unlock(list);
		return NULL;
	}

	if (lock_list && list_unlock(list) == -1)
		return NULL;

	return list;
}

/*

=item C<List *list_remove_range(List *list, size_t index, size_t range)>

Removes C<range> items from C<list> starting at C<index>. On success, returns
C<list>. On error, returns C<NULL>.

=cut

*/

List *list_remove_range(List *list, size_t index, size_t range)
{
	return list_remove_range_locked(list, index, range, 1);
}

/*

C<List *list_remove_range_unlocked(List *list, size_t index, size_t range)>

Removes C<range> items from C<list> starting at C<index> without locking
C<list>. On success, returns C<list>. On error, returns C<NULL>.

*/

static List *list_remove_range_unlocked(List *list, size_t index, size_t range)
{
	return list_remove_range_locked(list, index, range, 0);
}

/*

=item C<List *list_insert(List *list, size_t index, void *item)>

Adds C<item> to C<list> at position C<index>. On success, returns C<list>.
On error, returns C<NULL>.

=cut

*/

List *list_insert(List *list, size_t index, void *item)
{
	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (list->length < index)
	{
		list_unlock(list);
		return NULL;
	}

	if (expand(list, index, 1) == -1)
	{
		list_unlock(list);
		return NULL;
	}

	list->list[index] = item;
	list_unlock(list);

	return list;
}

/*

=item C<List *list_insert_int(List *list, size_t index, int item)>

Adds C<item> to C<list> at position C<index>. On success, returns C<list>.
On error, returns C<NULL>.

=cut

*/

List *list_insert_int(List *list, size_t index, int item)
{
	return list_insert(list, index, (void *)item);
}

/*

=item C<List *list_insert_list(List *list, size_t index, const List *src, list_copy_t *copy)>

Inserts the items from C<src> into C<list>, starting at position C<index>
using C<copy> as the copy constructor (if not C<NULL>). On success, returns
C<list>. On error, returns C<NULL>.

=cut

*/

#define enlist(item, copy) ((copy) ? (copy)(item) : (item))

List *list_insert_list(List *list, size_t index, const List *src, list_copy_t *copy)
{
	size_t i;

	if (!src || !list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (list_rdlock((List *)src) == -1)
	{
		list_unlock(list);
		return NULL;
	}

	if (list->length < index || xor(list->destroy, copy))
	{
		list_unlock((List *)src);
		list_unlock(list);
		return NULL;
	}

	if (expand(list, index, src->length) == -1)
	{
		list_unlock((List *)src);
		list_unlock(list);
		return NULL;
	}

	for (i = 0; i < src->length; ++i)
		list->list[index + i] = enlist(src->list[i], copy);

	list_unlock((List *)src);
	list_unlock(list);

	return list;
}

/*

=item C<List *list_append(List *list, void *item)>

Appends C<item> to C<list>. On success, returns C<list>. On error, returns
C<NULL>.

=cut

*/

List *list_append(List *list, void *item)
{
	return list_insert(list, list_length(list), item);
}

/*

=item C<List *list_append_int(List *list, int item)>

Appends C<item> to C<list>. On success, returns C<list>. On error, returns
C<NULL>.

=cut

*/

List *list_append_int(List *list, int item)
{
	return list_insert_int(list, list_length(list), item);
}

/*

=item C<List *list_append_list(List *list, const List *src, list_copy_t *copy)>

Appends the items in C<src> to C<list> using C<copy> as the copy constructor
(if not C<NULL>). On success, returns C<list>. On error, returns C<NULL>.

=cut

*/

List *list_append_list(List *list, const List *src, list_copy_t *copy)
{
	return list_insert_list(list, list_length(list), src, copy);
}

/*

=item C<List *list_prepend(List *list, void *item)>

Prepends C<item> to C<list>. On success, returns C<list>. On error, returns
C<NULL>.

=cut

*/

List *list_prepend(List *list, void *item)
{
	return list_insert(list, 0, item);
}

/*

=item C<List *list_prepend_int(List *list, int item)>

Prepends C<item> to C<list>. On success, returns C<list>. On error, returns
C<NULL>.

=cut

*/

List *list_prepend_int(List *list, int item)
{
	return list_insert_int(list, 0, item);
}

/*

=item C<List *list_prepend_list(List *list, const List *src, list_copy_t *copy)>

Prepends the items in C<src> to C<list> using C<copy> as the copy constructor
(if not C<NULL>). On success, returns C<list>. On error, returns C<NULL>.

=cut

*/

List *list_prepend_list(List *list, const List *src, list_copy_t *copy)
{
	return list_insert_list(list, 0, src, copy);
}

/*

=item C<List *list_replace(List *list, size_t index, size_t range, void *item)>

Replaces C<range> items in C<list>, starting at C<index>, with C<item>.
On success, returns C<list>. On error, returns C<NULL>.

=cut

*/

List *list_replace(List *list, size_t index, size_t range, void *item)
{
	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (list->length < index + range)
	{
		list_unlock(list);
		return NULL;
	}

	killitems(list, index, range);

	if (adjust(list, index, range, 1) == -1)
	{
		list_unlock(list);
		return NULL;
	}

	list->list[index] = item;

	if (list_unlock(list) == -1)
		return NULL;

	return list;
}

/*

=item C<List *list_replace_int(List *list, size_t index, size_t range, int item)>

Replaces C<range> items in C<list>, starting at C<index>, with C<item>.
On success, returns C<list>. On error, returns C<NULL>.

=cut

*/

List *list_replace_int(List *list, size_t index, size_t range, int item)
{
	return list_replace(list, index, range, (void *)item);
}

/*

=item C<List *list_replace_list(List *list, size_t index, size_t range, const List *src, list_copy_t *copy)>

Replaces C<range> items in C<list>, starting at C<index>, with the items in
C<src> using C<copy> as the copy constructor (if not C<NULL>). On success,
return C<list>. On error, returns C<NULL>.

=cut

*/

List *list_replace_list(List *list, size_t index, size_t range, const List *src, list_copy_t *copy)
{
	size_t length;

	if (!src || !list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (list_rdlock((List *)src))
	{
		list_unlock(list);
		return NULL;
	}

	if (list->length < index + range || xor(list->destroy, copy))
	{
		list_unlock((List *)src);
		list_unlock(list);
		return NULL;
	}

	killitems(list, index, range);

	if (adjust(list, index, range, length = list_length(src)) == -1)
	{
		list_unlock((List *)src);
		list_unlock(list);
		return NULL;
	}

	while (length--)
		list->list[index + length] = enlist(src->list[length], copy);

	list_unlock((List *)src);
	list_unlock(list);

	return list;
}

/*

C<List *list_extract_locked(const List *list, size_t index, size_t range, list_copy_t *copy, int lock_list)>

Creates a new list consisting of C<range> items from C<list>, starting at
C<index>, using C<copy> as the copy constructor (if not C<NULL>). If
C<lock_list> is non-zero, C<list> is read locked. On success, returns the
new list. On error, returns C<NULL>.

*/

static List *list_extract_locked(const List *list, size_t index, size_t range, list_copy_t *copy, int lock_list)
{
	List *ret;

	if (!list)
		return NULL;

	if (lock_list && list_rdlock((List *)list) == -1)
		return NULL;

	if (list->length < index + range || xor(list->destroy, copy))
	{
		if (lock_list)
			list_unlock((List *)list);
		return NULL;
	}

	if (!(ret = list_create(copy ? list->destroy : NULL)))
	{
		if (lock_list)
			list_unlock((List *)list);
		return NULL;
	}

	while (range--)
	{
		if (list_append(ret, enlist(list->list[index++], copy)) == NULL)
		{
			if (lock_list)
				list_unlock((List *)list);
			list_release(ret);
			return NULL;
		}
	}

	if (lock_list)
		list_unlock((List *)list);

	return ret;
}

#undef enlist

/*

=item C<List *list_extract(const List *list, size_t index, size_t range, list_copy_t *copy)>

Creates a new list consisting of C<range> items from C<list>, starting at
C<index>, using C<copy> as the copy constructor (if not C<NULL>). On success,
returns the new list. On error, returns C<NULL>.

=cut

*/

List *list_extract(const List *list, size_t index, size_t range, list_copy_t *copy)
{
	return list_extract_locked(list, index, range, copy, 1);
}

/*

C<List *list_extract_unlocked(const List *list, size_t index, size_t range, list_copy_t *copy)>

Creates a new list consisting of C<range> items from C<list>, starting at
C<index>, using C<copy> as the copy constructor (if not C<NULL>). Does not
read lock C<list>. On success, returns the new list. On error, returns
C<NULL>.

*/

static List *list_extract_unlocked(const List *list, size_t index, size_t range, list_copy_t *copy)
{
	return list_extract_locked(list, index, range, copy, 0);
}

/*

=item C<List *list_push(List *list, void *item)>

Pushes C<item> onto the end of C<list>. On success, returns C<list>. Om
error, returns C<NULL>.

=cut

*/

List *list_push(List *list, void *item)
{
	return list_append(list, item);
}

/*

=item C<List *list_push_int(List *list, int item)>

Pushes C<item> onto the end of C<list>. On success, returns C<list>. Om
error, returns C<NULL>.

=cut

*/

List *list_push_int(List *list, int item)
{
	return list_append_int(list, item);
}

/*

=item C<void *list_pop(List *list)>

Pops the last item off C<list>. On success, returns the item popped. On
error, returns C<NULL>.

=cut

*/

void *list_pop(List *list)
{
	void *item;

	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (!list || !list->length)
	{
		list_unlock(list);
		return NULL;
	}

	item = list->list[list->length - 1];
	list->list[list->length - 1] = NULL;

	if (!list_remove_unlocked(list, list->length - 1))
	{
		list->list[list->length - 1] = item;
		list_unlock(list);
		return NULL;
	}

	list_unlock(list);

	return item;
}

/*

=item C<int list_pop_int(List *list)>

Pops the last item off C<list>. On success, returns the item popped. On
error, returns C<0>.

=cut

*/

int list_pop_int(List *list)
{
	return (int)list_pop(list);
}

/*

=item C<void *list_shift(List *list)>

Removes and returns the first item in C<list>. On success, returns the item
shifted. On error, returns C<NULL>.

=cut

*/

void *list_shift(List *list)
{
	void *item;

	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (!list->length)
	{
		list_unlock(list);
		return NULL;
	}

	item = list->list[0];
	list->list[0] = NULL;

	if (!list_remove_unlocked(list, 0))
	{
		list->list[0] = item;
		list_unlock(list);
		return NULL;
	}

	list_unlock(list);

	return item;
}

/*

=item C<int list_shift_int(List *list)>

Removes and returns the first item in C<list>. On success, returns the item
shifted. On error, returns C<0>.

=cut

*/

int list_shift_int(List *list)
{
	return (int)list_shift(list);
}

/*

=item C<List *list_unshift(List *list, void *item)>

Inserts C<item> at the start of C<list>. On success, returns C<list>. On
error, returns C<NULL>.

=cut

*/

List *list_unshift(List *list, void *item)
{
	return list_prepend(list, item);
}

/*

=item C<List *list_unshift_int(List *list, int item)>

Inserts C<item> at the start of C<list>. On success, returns C<list>. On
error, returns C<NULL>.

=cut

*/

List *list_unshift_int(List *list, int item)
{
	return list_prepend_int(list, item);
}

/*

=item C<List *list_splice(List *list, size_t index, size_t range)>

Removes a sublist from C<list> starting at C<index> of length C<range> items.
On success, returns the sublist. It is the caller's responsibility to
deallocate the new sublist with I<list_release()> or I<list_destroy()>. On
error, returns C<NULL>.

=cut

*/

List *list_splice(List *list, size_t index, size_t range, list_copy_t *copy)
{
	List *ret;

	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (list->length < index + range)
	{
		list_unlock(list);
		return NULL;
	}

	if (!(ret = list_extract_unlocked(list, index, range, copy)))
	{
		list_unlock(list);
		return NULL;
	}

	if (!list_remove_range_unlocked(list, index, range))
	{
		list_unlock(list);
		list_release(ret);
		return NULL;
	}

	list_unlock(list);

	return ret;
}

/*

=item C<List *list_sort(List *list, list_cmp_t *cmp)>

Sorts the items in C<list> using the item comparison function C<cmp> and
I<qsort(3)> for lists of fewer than 10000 items and I<hsort(3)> for larger
lists. On success, returns C<list>. On error, returns C<NULL>.

=cut

*/

List *list_sort(List *list, list_cmp_t *cmp)
{
	if (!list)
		return NULL;

	if (list_wrlock(list) == -1)
		return NULL;

	if (!list->list || !list->length)
	{
		list_unlock(list);
		return NULL;
	}

	((list->length >= 10000) ? hsort : qsort)(list->list, list->length, sizeof list->list[0], cmp);
	list_unlock(list);

	return list;
}

/*

=item C<void list_apply(List *list, list_action_t *action)>

Invokes C<action> for each of C<list>'s items. The arguments passed to
C<action> are the item, a pointer to the loop variable containing the item's
position within C<list> and C<data>.

=cut

*/

void list_apply(List *list, list_action_t *action, void *data)
{
	size_t i;

	if (!list || !action)
		return;

	if (list_rdlock(list) == -1)
		return;

	for (i = 0; i < list->length; ++i)
		action(list->list[i], &i, data);

	list_unlock(list);
}

/*

=item C<List *list_map(List *list, list_release_t *destroy, list_map_t *map, void *data)>

Returns a new list containing the return values of C<map>, invoked once for
each item in C<list>. The arguments passed to C<map> are the item, a pointer
to the loop variable containing the item's position within C<list> and
C<data>. C<destroy> will be the destroy function for the returned list.
The user is responsible for deallocating the returned list with
I<list_destroy()> or I<list_release()>. On success, returns the new list. On
error, returns C<NULL>.

=cut

*/

List *list_map(List *list, list_release_t *destroy, list_map_t *map, void *data)
{
	List *mapping;
	size_t i;

	if (!list || !map)
		return NULL;

	if (!(mapping = list_create(destroy)))
		return NULL;

	if (list_rdlock(list) == -1)
	{
		list_release(mapping);
		return NULL;
	}

	for (i = 0; i < list->length; ++i)
	{
		if (!list_append(mapping, map(list->list[i], &i, data)))
		{
			list_unlock(list);
			list_release(mapping);
			return NULL;
		}
	}

	list_unlock(list);

	return mapping;
}

/*

=item C<List *list_grep(List *list, list_query_t *grep, void *data)>

Invokes C<grep> once for each item in C<list>, building a new list
containing the items that resulted in C<grep> returning a non-zero value.
Returns this new list. The arguments passed to C<grep> are the item, a
pointer to the loop variable containing the item's position within C<list>
and C<data>. The user is responsible for deallocating the returned list with
I<list_destroy()> or I<list_release()>. Note that the list returned does not
own its items since it does not copy the items. On success, returns the new
list. On error, returns C<NULL>.

=cut

*/

List *list_grep(List *list, list_query_t *grep, void *data)
{
	List *grepping;
	size_t i;

	if (!list || !list)
		return NULL;

	if (!(grepping = list_create(NULL)))
		return NULL;

	if (list_rdlock(list) == -1)
	{
		list_release(grepping);
		return NULL;
	}

	for (i = 0; i < list->length; ++i)
	{
		if (grep(list->list[i], &i, data))
		{
			if (!list_append(grepping, list->list[i]))
			{
				list_release(grepping);
				list_unlock(list);
				return NULL;
			}
		}
	}

	list_unlock(list);

	return grepping;
}

/*

=item C<ssize_t list_ask(List *list, ssize_t *index, list_query_t *query, void *data)>

Invokes C<query> on each item of C<list>, starting at C<index> until
C<query> returns a non-zero value. The arguments passed to C<query> are the
item, C<index> and C<data>. Returns the index of the item that satisfied
query, or -1 when query is not satisfied by any remaining items. The value
pointed to by C<index> is set to the return value.

=cut

*/

ssize_t list_ask(List *list, ssize_t *index, list_query_t *query, void *data)
{
	ssize_t ret;
	size_t i;

	if (!list || !index || !query)
		return -1;

	if (list_rdlock(list) == -1)
		return -1;

	if (*index >= list->length)
	{
		list_unlock(list);
		return -1;
	}

	for (i = *index; i < list->length; ++i)
	{
		if (query(list->list[i], (size_t *)index, data))
		{
			ret = *index = i;
			list_unlock(list);
			return ret;
		}
	}

	ret = *index = -1;
	list_unlock(list);
	return ret;
}

/*

=item C<Lister *lister_create(List *list)>

Creates an iterator for C<list>. On success, returns the iterator. On error,
returns C<NULL>.

=cut

*/

Lister *lister_create(List *list)
{
	Lister *lister;

	if (!list)
		return NULL;

	if (!(lister = mem_new(Lister)))
		return NULL;

	if (pthread_mutex_init(&lister->lock, NULL) != 0)
	{
		mem_release(lister);
		return NULL;
	}

	if (pthread_mutex_lock(&lister->lock) != 0)
	{
		pthread_mutex_destroy(&lister->lock);
		mem_release(lister);
		return NULL;
	}

	if (list_wrlock(list) == -1)
	{
		pthread_mutex_unlock(&lister->lock);
		pthread_mutex_destroy(&lister->lock);
		mem_release(lister);
		return NULL;
	}

	lister->owner = pthread_self();
	lister->list = list;
	lister->index = -1;

	return lister;
}

/*

=item C<void lister_release(Lister *lister)>

Releases (deallocates) C<lister>.

=cut

*/

void lister_release(Lister *lister)
{
	if (!lister)
		return;

	if (!pthread_equal(lister->owner, pthread_self()))
		return;

	list_unlock(lister->list);
	pthread_mutex_unlock(&lister->lock);
	pthread_mutex_destroy(&lister->lock);
	mem_release(lister);
}

/*

=item C<void *lister_destroy(Lister **lister)>

Destroys (deallocates and sets to C<NULL>) C<*lister>. Returns C<NULL>.
B<Note:> listers shared by multiple threads must not be destroyed until
after the threads have finished with it.

=cut

*/

void *lister_destroy(Lister **lister)
{
	if (lister && *lister)
	{
		if (!pthread_equal((*lister)->owner, pthread_self()))
			return NULL;

		lister_release(*lister);
		*lister = NULL;
	}

	return NULL;
}

/*

=item C<int lister_has_next(Lister *lister)>

Returns whether or not there is another item in the list being iterated over
by C<lister>.

=cut

*/

int lister_has_next(Lister *lister)
{
	if (!lister)
		return 0;

	if (!pthread_equal(lister->owner, pthread_self()))
		return 0;

	return lister->index + 1 < lister->list->length;
}

/*

=item C<void *lister_next(Lister *lister)>

Returns the next item in the iteration C<lister>. On error, returns C<NULL>.

=cut

*/

void *lister_next(Lister *lister)
{
	if (!lister)
		return NULL;

	if (!pthread_equal(lister->owner, pthread_self()))
		return NULL;

	return list_item_unlocked(lister->list, (size_t)++lister->index);
}

/*

=item C<int lister_next_int(Lister *lister)>

Returns the next item in the iteration C<lister> as an integer. On error,
returns C<-1>.

=cut

*/

int lister_next_int(Lister *lister)
{
	if (!lister)
		return -1;

	if (!pthread_equal(lister->owner, pthread_self()))
		return -1;

	return list_item_int_unlocked(lister->list, (size_t)++lister->index);
}

/*

=item C<void lister_remove(Lister *lister)>

Removes the current item in the iteration C<lister>. The next item in the
iteration is the item following the removed item, if any. This must be called
after I<lister_next()> or I<lister_next_int()>.

=cut

*/

void lister_remove(Lister *lister)
{
	if (!lister)
		return;

	if (!pthread_equal(lister->owner, pthread_self()))
		return;

	if (lister->index == -1)
		return;

	list_remove_unlocked(lister->list, (size_t)lister->index--);
}

/*

=item C<int list_has_next(List *list)>

Returns whether or not there is another item in C<list>. The first time this
is called, a new, internal I<Lister> will be created (Note: There can be
only one). When there are no more items, returns zero and destroys the
internal iterator. When it returns a non-zero value, use I<list_next()> to
retrieve the next item.

Note: If an iteration using an internal iterator terminates before the end
of the list, it is the caller's responsibility to call I<list_break()>.
Failure to do so will cause the internal iterator to leak which will leave
the list write locked. The next use of I<list_has_next()> for the same list
will not do what you expect. In fact, the next attempt to use the list would
deadlock the prgoram.

=cut

*/

int list_has_next(List *list)
{
	int has;

	if (!list)
		return 0;

	if (!list->lister || !pthread_equal(pthread_self(), list->lister->owner))
		list->lister = lister_create(list);

	if (!(has = lister_has_next(list->lister)))
	  list_break(list);

	return has;
}

/*

=item C<void list_break(List *list)>

Unlocks C<list> and destroys its internal iterator. Must be used only when
an iteration using an internal iterator has terminated before reaching the
end of C<list>.

=cut

*/

void list_break(List *list)
{
	if (list)
	{
		Lister *lister = list->lister;
		list->lister = NULL;
		lister_release(lister);
	}
}

/*

=item C<void *list_next(List *list)>

Returns the next item in C<list> using it's internal iterator. On error,
returns C<NULL>.

=cut

*/

void *list_next(List *list)
{
	if (!list || !list->lister)
		return NULL;

	return lister_next(list->lister);
}

/*

=item C<int list_next_int(List *list)>

Returns the next item in C<list> as an integer using it's internal iterator.
On error, returns C<-1>.

=cut

*/

int list_next_int(List *list)
{
	if (!list || !list->lister)
		return -1;

	return lister_next_int(list->lister);
}

/*

=item C<void list_remove_current(List *list)>

Removes the current item in C<list> using it's internal iterator. The next
item in the iteration is the item following the removed item, if any. This
must be called after I<list_next()>.

=cut

*/

void list_remove_current(List *list)
{
	if (!list || !list->lister)
		return;

	lister_remove(list->lister);
}

/*

=back

=head1 MT-Level

MT-Disciplined

By default, I<List>s are not MT-Safe because most programs are single
threaded and synchronisation doesn't come for free. Even in multi threaded
programs, not all I<List>s are necessarily shared between multiple threads.

When a I<List> is shared between multiple threads which need to be
synchronised, the method of synchronisation must be carefully selected by the
client code. There are tradeoffs between concurrency and overhead. The greater
the concurrency, the greater the overhead. More locks give greater concurrency
but have greater overhead. Readers/Writer locks can give greater concurrency
than Mutex locks but have greater overhead. One lock for each I<List> may be
required, or one lock for all (or a set of) I<List>s may be more appropriate.

Generally, the best synchronisation strategy for a given application can only
be determined by testing/benchmarking the written application. It is important
to be able to experiment with the synchronisation strategy at this stage of
development without pain.

To facilitate this, I<List>s can be created with I<list_create_locked()> which
takes a I<Locker> argument. The I<Locker> specifies a lock and a set of
functions for manipulating the lock. Each I<List> can have it's own lock by
creating a separate I<Locker> for each I<List>. Multiple I<List>s can share the
same lock by sharing the same I<Locker>. Only the application developer can
determine what is appropriate for each application on a case by case basis.

MT-Disciplined means that the application developer has a mechanism for
specifying the synchronisation requirements to be applied to library code.

=head1 BUGS

Little attempt is made to protect the client from sharing items between lists
with differing ownership policies and getting it wrong. When copying items from
any list to an owning list, a copy function must be supplied. When adding a
single item to an owning list, it is assumed that the list may take over
ownership of the item. When an owning list is destroyed, all of its items are
destroyed. If any of these items had been shared with a non-owning list that
outlived the owning list, then the non-owning list will contain items that
point to deallocated memory.

If you use an internal iterator in a loop that terminates before the end of the
list, and fail to call I<list_break()>, the internal iterator will leak and the
list will remain write locked, deadlocking the program the time you attempt to
access the list.

Uses I<malloc(3)>. Need to decouple memory type and allocation strategy from
this code.

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<hsort(3)|hsort(3)>,
L<qsort(3)|qsort(3)>,
L<thread(3)|thread(3)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

=cut

*/

#ifdef TEST

#include <semaphore.h>

#include <sys/time.h>

#include <slack/str.h>

#if 0
static void list_print(const char *name, List *list)
{
	int i;

	if (!list)
	{
		printf("%s = nil\n", name);
		return;
	}

	/* printf("%s = { destroy = %p, size = %d, length = %d, list = { ", name, list->destroy, list->size, list->length); */
	printf("%s = { ", name);

	for (i = 0; i < list_length(list); ++i)
	{
		if (i)
			printf(", ");

		/* printf("\"%s\"(%p)", (char *)list_item(list, i), list_item(list, i)); */
		printf("\"%s\"", (char *)list_item(list, i));
	}

	printf(" }\n");
}
#endif

char action_data[1024];

void action(void *item, size_t *index)
{
	strlcat(action_data, item, 1024);
}

int query_data[] = { 2, 6, 8 , -1 };

int query(void *item, size_t *index)
{
	return !strcmp((const char *)item, "def");
}

int sort_cmp(const char **a, const char **b)
{
	return strcmp(*a, *b);
}

int mapf(int item, size_t *index, int *sum)
{
	return (sum) ? *sum += item : item;
}

int grepf(int item, size_t *index, int *data)
{
	return !(item & 1);
}

List *mtlist = NULL;
Locker *locker = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
sem_t barrier;
sem_t length;
const int lim = 1000;
int debug = 0;
int errors = 0;

void *produce(void *arg)
{
	int i;
	int test = *(int *)arg;

	for (i = 0; i <= lim; ++i)
	{
		if (debug)
			printf("p: prepend %d\n", i);

		if (!list_prepend_int(mtlist, i))
			++errors, printf("Test%d: list_prepend_int(mtlist, %d), failed\n", test, i);

		sem_post(&length);
	}

	sem_post(&barrier);
	return NULL;
}

void *consume(void *arg)
{
	int i, v;
	int test = *(int *)arg;

	for (i = 0; i < lim * 2; ++i)
	{
		if (debug)
			printf("c: pop\n");

		while (sem_wait(&length) != 0)
		{}

		v = list_pop_int(mtlist);

		if (debug)
			printf("c: pop %d\n", v);

		if (v == lim)
			break;
	}

	if (i != lim)
		++errors, printf("Test%d: consumer read %d items, not %d\n", test, i, lim);

	sem_post(&barrier);
	return NULL;
}

void *iterate(void *arg)
{
	int i;
	int t = *(int *)arg;
	int broken = 0;

	if (debug)
		printf("i%d: loop\n", t);

	for (i = 0; i < lim / 10; ++i)
	{
		while (list_has_next(mtlist))
		{
			int val = list_next_int(mtlist);

			if (debug)
				printf("i%d: loop %d/%d val %d\n", t, i, lim / 10, val);

			if (!broken)
			{
				list_break(mtlist);
				broken = 1;
				break;
			}
		}
	}

	sem_post(&barrier);
	return NULL;
}

void *iterate2(void *arg)
{
	int i;
	int t = *(int *)arg;

	if (debug)
		printf("j%d: loop\n", t);

	for (i = 0; i < lim / 10; ++i)
	{
		Lister *lister = lister_create(mtlist);

		while (lister_has_next(lister))
		{
			int val = (int)lister_next(lister);

			if (debug)
				printf("j%d: loop %d/%d val %d\n", t, i, lim / 10, val);
		}

		lister_release(lister);
	}

	sem_post(&barrier);
	return NULL;
}

void mt_test(int test, Locker *locker)
{
	mtlist = list_create_locked(locker, NULL);
	if (!mtlist)
		++errors, printf("Test%d: list_create_locked(NULL) failed\n", test);
	else
	{
		static int iid[7] = { 0, 1, 2, 3, 4, 5, 6 };
		pthread_attr_t attr;
		pthread_t id;
		int i;

		sem_init(&barrier, 0, 0);
		sem_init(&length, 0, 0);
		thread_attr_init(&attr);
		pthread_create(&id, &attr, produce, &test);
		pthread_create(&id, &attr, consume, &test);
		pthread_create(&id, &attr, iterate, iid + 0);
		pthread_create(&id, &attr, iterate, iid + 1);
		pthread_create(&id, &attr, iterate, iid + 2);
		pthread_create(&id, &attr, iterate, iid + 3);
		pthread_create(&id, &attr, iterate2, iid + 4);
		pthread_create(&id, &attr, iterate2, iid + 5);
		pthread_create(&id, &attr, iterate2, iid + 6);
		pthread_attr_destroy(&attr);

		for (i = 0; i < 9; ++i)
			while (sem_wait(&barrier) != 0)
			{}

		list_destroy(&mtlist);
		if (mtlist)
			++errors, printf("Test%d: list_destroy(&mtlist) failed\n", test);
	}
}

int main(int ac, char **av)
{
	int errors = 0;
	List *a, *b, *c, *d;
	Lister *lister;
	ssize_t index = 0;
	int i;

	printf("Testing: list\n");

	/* Test list_make, list_length, list_item */

	if (!(a = list_make(NULL, "abc", "def", "ghi", "jkl", NULL)))
		++errors, printf("Test1: list_make() failed\n");
	else if (list_length(a) != 4)
		++errors, printf("Test1: list_make() created a list with %d items (not 4) or list_length() failed\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test1: list_make(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "def"))
		++errors, printf("Test1: list_make(): 2nd item is '%s' not 'def'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "ghi"))
		++errors, printf("Test1: list_make(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "jkl"))
		++errors, printf("Test1: list_make(): 4th item is '%s' not 'jkl'\n", (char *)list_item(a, 3));

	/* Test list_create(NULL), list_empty, list_append, list_prepend, list_insert, list_last */

	if (!(b = list_create(NULL)))
		++errors, printf("Test2: list_create(NULL) failed\n");

	if (!list_empty(b))
		++errors, printf("Test3: list_empty() on empty list failed\n");

	if (!list_append(b, "abc") || list_length(b) != 1 || strcmp(list_item(b, 0), "abc"))
		++errors, printf("Test4: list_append() failed\n");

	if (list_empty(b))
		++errors, printf("Test5: list_empty() on non-empty list failed\n");

	if (!list_prepend(b, "def") || list_length(b) != 2 || strcmp(list_item(b, 0), "def") || strcmp(list_item(b, 1), "abc"))
		++errors, printf("Test6: list_prepend() failed\n");

	if (!list_insert(b, 1, "ghi") || list_length(b) != 3 || strcmp(list_item(b, 0), "def") || strcmp(list_item(b, 1), "ghi") || strcmp(list_item(b, 2), "abc"))
		++errors, printf("Test7: list_insert() failed\n");

	if (list_last(b) != 2)
		++errors, printf("Test8: list_last() failed (returned %d, not 2)\n", list_last(b));

	/* Test list_copy, list_destroy */

	c = list_copy(a, (list_copy_t *)free);
	if (c)
		++errors, printf("Test9: list_copy() with copy() but no destroy() failed\n");

	c = list_copy(a, NULL);
	if (!c)
		++errors, printf("Test10: list_copy() without copy() or destroy() failed\n");
	else if (list_length(c) != 4)
		++errors, printf("Test10: list_copy() created a list with %d items (not 4) or list_length() failed\n", list_length(c));
	else if (!list_item(c, 0) || strcmp(list_item(c, 0), "abc"))
		++errors, printf("Test10: list_copy(): 1st item is '%s' not 'abc'\n", (char *)list_item(c, 0));
	else if (!list_item(c, 0) || strcmp(list_item(c, 1), "def"))
		++errors, printf("Test10: list_copy(): 2nd item is '%s' not 'def'\n", (char *)list_item(c, 1));
	else if (!list_item(c, 0) || strcmp(list_item(c, 2), "ghi"))
		++errors, printf("Test10: list_copy(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(c, 2));
	else if (!list_item(c, 0) || strcmp(list_item(c, 3), "jkl"))
		++errors, printf("Test10: list_copy(): 4th item is '%s' not 'jkl'\n", (char *)list_item(c, 3));

	list_destroy(&c);
	if (c)
		++errors, printf("Test 11: list_destroy(&c) failed\n");

	/* Test list_create(free), list_append, list_copy */

	if (!(c = list_create((list_release_t *)free)))
		++errors, printf("Test12: list_create(free) failed\n");
	else
	{
		if (list_append(c, mem_strdup("abc")) == NULL)
			++errors, printf("Test13: list_append(\"abc\") failed\n");
		if (list_append(c, mem_strdup("def")) == NULL)
			++errors, printf("Test14: list_append(\"def\") failed\n");
		if (list_append(c, mem_strdup("ghi")) == NULL)
			++errors, printf("Test15: list_append(\"ghi\") failed\n");
		if (list_append(c, mem_strdup("jkl")) == NULL)
			++errors, printf("Test16: list_append(\"jkl\") failed\n");

		d = list_copy(c, NULL);
		if (d)
			++errors, printf("Test17: list_copy() with destroy() but no copy() failed\n");

		d = list_copy(c, (list_copy_t *)mem_strdup);
		if (!d)
			++errors, printf("Test18: list_copy() with copy() and destroy() failed\n");
	}

	/* Test list_remove, list_replace */

	if (!list_remove(a, 3))
		++errors, printf("Test19: list_remove() failed\n");
	else if (list_length(a) != 3)
		++errors, printf("Test19: list_remove() from end of list failed: has %d items (not 3)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test19: list_remove(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "def"))
		++errors, printf("Test19: list_remove(): 2nd item is '%s' not 'def'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "ghi"))
		++errors, printf("Test19: list_remove(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(a, 2));

	if (!list_remove(a, 0))
		++errors, printf("Test20: list_remove() failed\n");
	else if (list_length(a) != 2)
		++errors, printf("Test20: list_remove() from beginning of list failed: has %d items (not 2)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "def"))
		++errors, printf("Test20: list_remove(): 1st item is '%s' not 'def'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "ghi"))
		++errors, printf("Test20: list_remove(): 2nd item is '%s' not 'ghi'\n", (char *)list_item(a, 1));

	if (!list_replace(a, 1, 1, "123"))
		++errors, printf("Test21: list_replace() failed\n");
	else if (list_length(a) != 2)
		++errors, printf("Test21: list_replace() failed: has %d items (not 2)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "def"))
		++errors, printf("Test21: list_replace(): 1st item is '%s' not 'def'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "123"))
		++errors, printf("Test21: list_replace(): 2nd item is '%s' not '123'\n", (char *)list_item(a, 1));

	/* Test list_append_list, list_prepend_list, list_insert_list */

	if (!list_append_list(a, b, NULL))
		++errors, printf("Test22: list_append_list() failed\n");
	else if (list_length(a) != 5)
		++errors, printf("Test22: list_append_list() failed: has %d items (not 5)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "def"))
		++errors, printf("Test22: list_append_list(): 1st item is '%s' not 'def'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "123"))
		++errors, printf("Test22: list_append_list(): 2nd item is '%s' not '123'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "def"))
		++errors, printf("Test22: list_append_list(): 3rd item is '%s' not 'def'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "ghi"))
		++errors, printf("Test22: list_append_list(): 4th item is '%s' not 'ghi'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "abc"))
		++errors, printf("Test22: list_append_list(): 5th item is '%s' not 'abc'\n", (char *)list_item(a, 4));

	if (!list_prepend_list(a, c, NULL))
		++errors, printf("Test23: list_prepend_list() failed\n");
	else if (list_length(a) != 9)
		++errors, printf("Test23: list_prepend_list() failed: has %d items (not 9)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test23: list_prepend_list(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "def"))
		++errors, printf("Test23: list_prepend_list(): 2nd item is '%s' not 'def'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "ghi"))
		++errors, printf("Test23: list_prepend_list(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "jkl"))
		++errors, printf("Test23: list_prepend_list(): 4th item is '%s' not 'jkl'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test23: list_prepend_list(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "123"))
		++errors, printf("Test23: list_prepend_list(): 6th item is '%s' not '123'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "def"))
		++errors, printf("Test23: list_prepend_list(): 7th item is '%s' not 'def'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test23: list_prepend_list(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "abc"))
		++errors, printf("Test23: list_prepend_list(): 9th item is '%s' not 'abc'\n", (char *)list_item(a, 8));

	if (!list_insert_list(b, 1, c, NULL))
		++errors, printf("Test24: list_insert_list() failed\n");
	else if (list_length(b) != 7)
		++errors, printf("Test24: list_insert_list() failed: has %d items (not 7)\n", list_length(b));
	else if (!list_item(b, 0) || strcmp(list_item(b, 0), "def"))
		++errors, printf("Test24: list_insert_list(): 1st item is '%s' not 'def'\n", (char *)list_item(b, 0));
	else if (!list_item(b, 1) || strcmp(list_item(b, 1), "abc"))
		++errors, printf("Test24: list_insert_list(): 2nd item is '%s' not 'abc'\n", (char *)list_item(b, 1));
	else if (!list_item(b, 2) || strcmp(list_item(b, 2), "def"))
		++errors, printf("Test24: list_insert_list(): 3rd item is '%s' not 'def'\n", (char *)list_item(b, 2));
	else if (!list_item(b, 3) || strcmp(list_item(b, 3), "ghi"))
		++errors, printf("Test24: list_insert_list(): 4th item is '%s' not 'ghi'\n", (char *)list_item(b, 3));
	else if (!list_item(b, 4) || strcmp(list_item(b, 4), "jkl"))
		++errors, printf("Test24: list_insert_list(): 5th item is '%s' not 'jkl'\n", (char *)list_item(b, 4));
	else if (!list_item(b, 5) || strcmp(list_item(b, 5), "ghi"))
		++errors, printf("Test24: list_insert_list(): 6th item is '%s' not 'ghi'\n", (char *)list_item(b, 5));
	else if (!list_item(b, 6) || strcmp(list_item(b, 6), "abc"))
		++errors, printf("Test24: list_insert_list(): 7th item is '%s' not 'abc'\n", (char *)list_item(b, 6));

	/* Test list_replace_list, list_remove_range */

	if (list_replace_list(c, 1, 2, d, NULL))
		++errors, printf("Test25: list_replace_list() with destroy() but not copy() failed\n");

	if (list_replace_list(a, 1, 2, d, (list_copy_t *)mem_strdup))
		++errors, printf("Test26: list_replace_list() with copy() but not destroy() failed\n");

	if (!list_replace_list(a, 1, 2, d, NULL))
		++errors, printf("Test27: list_replace_list() failed\n");
	else if (list_length(a) != 11)
		++errors, printf("Test27: list_insert_list() failed: has %d items (not 11)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test27: list_insert_list(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test27: list_insert_list(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "def"))
		++errors, printf("Test27: list_insert_list(): 3rd item is '%s' not 'def'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "ghi"))
		++errors, printf("Test27: list_insert_list(): 4th item is '%s' not 'ghi'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "jkl"))
		++errors, printf("Test27: list_insert_list(): 5th item is '%s' not 'jkl'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "jkl"))
		++errors, printf("Test27: list_insert_list(): 6th item is '%s' not 'jkl'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "def"))
		++errors, printf("Test27: list_insert_list(): 7th item is '%s' not 'def'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "123"))
		++errors, printf("Test27: list_insert_list(): 8th item is '%s' not '123'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "def"))
		++errors, printf("Test27: list_insert_list(): 9th item is '%s' not 'def'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "ghi"))
		++errors, printf("Test27: list_insert_list(): 10th item is '%s' not 'ghi'\n", (char *)list_item(a, 9));
	else if (!list_item(a, 10) || strcmp(list_item(a, 10), "abc"))
		++errors, printf("Test27: list_insert_list(): 11th item is '%s' not 'abc'\n", (char *)list_item(a, 10));

	if (!list_remove_range(b, 1, 3))
		++errors, printf("Test28: list_remove_range() failed\n");
	else if (list_length(b) != 4)
		++errors, printf("Test28: list_insert_list() failed: has %d items (not 4)\n", list_length(b));
	else if (!list_item(b, 0) || strcmp(list_item(b, 0), "def"))
		++errors, printf("Test28: list_insert_list(): 1st item is '%s' not 'def'\n", (char *)list_item(b, 0));
	else if (!list_item(b, 1) || strcmp(list_item(b, 1), "jkl"))
		++errors, printf("Test28: list_insert_list(): 2nd item is '%s' not 'jkl'\n", (char *)list_item(b, 1));
	else if (!list_item(b, 2) || strcmp(list_item(b, 2), "ghi"))
		++errors, printf("Test28: list_insert_list(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(b, 2));
	else if (!list_item(b, 3) || strcmp(list_item(b, 3), "abc"))
		++errors, printf("Test28: list_insert_list(): 4th item is '%s' not 'abc'\n", (char *)list_item(b, 3));

	/* Test list_apply, list_ask, list_sort */

	list_apply(a, (list_action_t *)action, NULL);
	if (strcmp(action_data, "abcabcdefghijkljkldef123defghiabc"))
		++errors, printf("Test29: list_apply() failed\n");

	for (i = 0; list_ask(a, &index, (list_query_t *)query, NULL) != -1; ++i, ++index)
	{
		if (index != query_data[i])
		{
			++errors, printf("Test30: list_ask returned %d (not %d)\n", index, query_data[i]);
			break;
		}
	}

	if (!list_sort(a, (list_cmp_t *)sort_cmp))
		++errors, printf("Test31: list_sort() failed\n");
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "123"))
		++errors, printf("Test31: list_sort(): 1st item is '%s' not '123'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test31: list_sort(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "abc"))
		++errors, printf("Test31: list_sort(): 3rd item is '%s' not 'abc'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "abc"))
		++errors, printf("Test31: list_sort(): 4th item is '%s' not 'abc'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test31: list_sort(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "def"))
		++errors, printf("Test31: list_sort(): 6th item is '%s' not 'def'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "def"))
		++errors, printf("Test31: list_sort(): 7th item is '%s' not 'def'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test31: list_sort(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "ghi"))
		++errors, printf("Test31: list_sort(): 9th item is '%s' not 'ghi'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "jkl"))
		++errors, printf("Test31: list_sort(): 10th item is '%s' not 'jkl'\n", (char *)list_item(a, 9));
	else if (!list_item(a, 10) || strcmp(list_item(a, 10), "jkl"))
		++errors, printf("Test31: list_sort(): 11th item is '%s' not 'jkl'\n", (char *)list_item(a, 10));

	/* Test lister_create, lister_has_next, lister_next, lister_destroy */

	if (!(lister = lister_create(a)))
		++errors, printf("Test32: lister_create() failed\n");

	for (i = 0; lister_has_next(lister); ++i)
	{
		void *item = lister_next(lister);

		if (item != list_item_unlocked(a, i)) /* white box */
		{
			++errors, printf("Test33: iteration %d is '%s' not '%s'\n", i, (char *)item, (char *)list_item(a, i));
			break;
		}
	}

	lister_destroy(&lister);
	if (lister)
		++errors, printf("Test34: lister_destroy(&lister) failed, lister is %p not NULL\n", (void *)lister);

	/* Test list_has_next, list_next, list_break */

	for (i = 0; list_has_next(a); ++i)
	{
		void *item = list_next(a);

		if (item != list_item_unlocked(a, i)) /* white box */
		{
			++errors, printf("Test35: internal iteration %d is '%s' not '%s'\n", i, (char *)item, (char *)list_item(a, i));
			list_break(a);
			break;
		}
	}

	for (i = 0; list_has_next(a); ++i)
	{
		void *item = list_next(a);

		if (item != list_item_unlocked(a, i)) /* white box */
		{
			++errors, printf("Test36: internal iteration %d is '%s' not '%s'\n", i, (char *)item, (char *)list_item(a, i));
			list_break(a);
			break;
		}

		if (i == 2)
		{
			list_break(a);
			break;
		}
	}

	if (a->lister)
		++errors, printf("Test36: list_break() failed\n");

	/* Test lister_remove */

	if (!(lister = lister_create(a)))
		++errors, printf("Test37: lister_create() failed\n");

	for (i = 0; lister_has_next(lister); ++i)
	{
		lister_next(lister);

		if (i == 4)
			lister_remove(lister);
	}

	lister_destroy(&lister);
	if (lister)
		++errors, printf("Test38: lister_destroy(&lister) failed, lister is %p not NULL\n", (void *)lister);

	if (!list_item(a, 0) || strcmp(list_item(a, 0), "123"))
		++errors, printf("Test39: list_sort(): 1st item is '%s' not '123'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test39: list_sort(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "abc"))
		++errors, printf("Test39: list_sort(): 3rd item is '%s' not 'abc'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "abc"))
		++errors, printf("Test39: list_sort(): 4th item is '%s' not 'abc'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test39: list_sort(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "def"))
		++errors, printf("Test39: list_sort(): 6th item is '%s' not 'def'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "ghi"))
		++errors, printf("Test39: list_sort(): 7th item is '%s' not 'ghi'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test39: list_sort(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "jkl"))
		++errors, printf("Test39: list_sort(): 9th item is '%s' not 'jkl'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "jkl"))
		++errors, printf("Test39: list_sort(): 10th item is '%s' not 'jkl'\n", (char *)list_item(a, 9));

	list_destroy(&a);
	if (a)
		++errors, printf("Test40: list_destroy(&a) failed, a is %p not NULL\n", (void *)a);

	list_destroy(&b);
	if (b)
		++errors, printf("Test41: list_destroy(&b) failed, b is %p not NULL\n", (void *)b);

	list_destroy(&c);
	if (c)
		++errors, printf("Test42: list_destroy(&c) failed, c is %p not NULL\n", (void *)c);

	list_destroy(&d);
	if (d)
		++errors, printf("Test43: list_destroy(&d) failed, d is %p not NULL\n", (void *)d);

	/* Test lists with int items, list_map, list_grep */

	a = list_create(NULL);
	if (!a)
		++errors, printf("Test44: list_create(NULL) failed\n");
	else
	{
		if (!list_append_int(a, 2))
			++errors, printf("Test45: list_append_int(a, 2) failed\n");
		if (list_item_int(a, 0) != 2)
			++errors, printf("Test46: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 2);

		if (!list_prepend_int(a, 0))
			++errors, printf("Test47: list_prepend_int(a, 0) failed\n");
		if (list_item_int(a, 0) != 0)
			++errors, printf("Test48: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 0);
		if (list_item_int(a, 1) != 2)
			++errors, printf("Test49: list_item_int(a, 1) failed (%d, not %d)\n", list_item_int(a, 1), 2);

		if (!list_insert_int(a, 1, 1))
			++errors, printf("Test50: list_insert_int(a, 1, 1) failed\n");
		if (list_item_int(a, 0) != 0)
			++errors, printf("Test51: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 0);
		if (list_item_int(a, 1) != 1)
			++errors, printf("Test52: list_item_int(a, 1) failed (%d, not %d)\n", list_item_int(a, 1), 1);
		if (list_item_int(a, 2) != 2)
			++errors, printf("Test53: list_item_int(a, 2) failed (%d, not %d)\n", list_item_int(a, 2), 2);

		for (i = 0; list_has_next(a); ++i)
		{
			int item = list_next_int(a);
			if (item != list_item_int_unlocked(a, i)) /* white box */
				++errors, printf("Test54: int list test failed (item %d = %d, not %d)\n", i, item, list_item_int(a, i));
		}

		if (i != 3)
			++errors, printf("Test55: list_has_next() failed (only %d items, not %d)\n", i, 3);

		if (!(lister = lister_create(a)))
			++errors, printf("Test56: lister_create(a) failed\n");
		else
		{
			for (i = 0; lister_has_next(lister); ++i)
			{
				int item = lister_next_int(lister);

				if (item != list_item_int_unlocked(a, i)) /* white box */
					++errors, printf("Test57: int list test failed (item %d = %d, not %d)\n", i, item, i);
			}

			if (i != 3)
				++errors, printf("Test58: lister_has_next() failed (only %d items, not %d)\n", i, 3);

			lister_destroy(&lister);
			if (lister)
				++errors, printf("Test59: lister_destroy(&lister) failed\n");
		}

		if (!list_replace_int(a, 2, 1, 4))
			++errors, printf("Test60: list_replace_int(a, 2, 1, 4) failed\n");
		if (list_item_int(a, 0) != 0)
			++errors, printf("Test61: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 0);
		if (list_item_int(a, 1) != 1)
			++errors, printf("Test62: list_item_int(a, 1) failed (%d, not %d)\n", list_item_int(a, 1), 1);
		if (list_item_int(a, 2) != 4)
			++errors, printf("Test63: list_item_int(a, 2) failed (%d, not %d)\n", list_item_int(a, 2), 4);

		i = 0;
		if (!(b = list_map(a, NULL, (list_map_t *)mapf, &i)))
			++errors, printf("Test64: list_map() failed\n");
		else
		{
			if (list_length(b) != 3)
				++errors, printf("Test65: list_map() failed (length %d, not %d)\n", list_length(b), 3);
			else
			{
				if (list_item_int(b, 0) != 0)
					++errors, printf("Test66: list_map() failed (item %d = %d, not %d)\n", 0, list_item_int(b, 0), 0);
				if (list_item_int(b, 1) != 1)
					++errors, printf("Test67: list_map() failed (item %d = %d, not %d)\n", 1, list_item_int(b, 1), 1);
				if (list_item_int(b, 2) != 5)
					++errors, printf("Test68: list_map() failed (item %d = %d, not %d)\n", 2, list_item_int(b, 2), 5);
			}

			list_destroy(&b);
			if (b)
				++errors, printf("Test69: list_destroy(&b) failed\n");
		}

		if (!(b = list_grep(a, (list_query_t *)grepf, NULL)))
			++errors, printf("Test70: list_grep() failed\n");
		else
		{
			if (list_length(b) != 2)
				++errors, printf("Test71: list_grep() failed (length %d, not %d)\n", list_length(b), 2);
			else
			{
				if (list_item_int(b, 0) != 0)
					++errors, printf("Test72: list_map() failed (item %d = %d, not %d)\n", 0, list_item_int(b, 0), 0);
				if (list_item_int(b, 1) != 4)
					++errors, printf("Test73: list_map() failed (item %d = %d, not %d)\n", 1, list_item_int(b, 1), 4);
			}

			list_destroy(&b);
			if (b)
				++errors, printf("Test74: list_destroy(&b) failed\n");
		}

		list_destroy(&a);
		if (a)
			++errors, printf("Test75: list_destroy(&a) failed\n");
	}

	/* Test list_push_int, list_pop_int, list_unshift_int, list_shift_int */

	if (!(a = list_create(NULL)))
		++errors, printf("Test76: a = list_create(NULL) failed\n");
	else
	{
		int item;

		if (!list_push_int(a, 1))
			++errors, printf("Test77: list_push_int(a, %d) failed\n", 1);
		if (!list_push_int(a, 2))
			++errors, printf("Test78: list_push_int(a, %d) failed\n", 2);
		if (!list_push_int(a, 3))
			++errors, printf("Test79: list_push_int(a, %d) failed\n", 3);
		if (!list_push_int(a, 0))
			++errors, printf("Test80: list_push_int(a, %d) failed\n", 0);
		if (!list_push_int(a, 5))
			++errors, printf("Test81: list_push_int(a, %d) failed\n", 5);
		if (!list_push_int(a, 6))
			++errors, printf("Test82: list_push_int(a, %d) failed\n", 6);
		if (!list_push_int(a, 7))
			++errors, printf("Test83: list_push_int(a, %d) failed\n", 7);
		if ((item = list_pop_int(a)) != 7)
			++errors, printf("Test84: list_pop_int(a) failed (%d, not %d)\n", item, 7);
		if ((item = list_pop_int(a)) != 6)
			++errors, printf("Test85: list_pop_int(a) failed (%d, not %d)\n", item, 6);
		if ((item = list_pop_int(a)) != 5)
			++errors, printf("Test86: list_pop_int(a) failed (%d, not %d)\n", item, 5);
		if ((item = list_pop_int(a)) != 0)
			++errors, printf("Test87: list_pop_int(a) failed (%d, not %d)\n", item, 0);
		if ((item = list_pop_int(a)) != 3)
			++errors, printf("Test88: list_pop_int(a) failed (%d, not %d)\n", item, 3);
		if ((item = list_pop_int(a)) != 2)
			++errors, printf("Test89: list_pop_int(a) failed (%d, not %d)\n", item, 2);
		if ((item = list_pop_int(a)) != 1)
			++errors, printf("Test90: list_pop_int(a) failed (%d, not %d)\n", item, 1);
		if ((item = list_pop_int(a)) != 0)
			++errors, printf("Test91: list_pop_int(a) failed (%d, not %d)\n", item, 0);

		if (!list_unshift_int(a, 1))
			++errors, printf("Test92: list_unshift_int(a, %d) failed\n", 1);
		if (!list_unshift_int(a, 2))
			++errors, printf("Test93: list_unshift_int(a, %d) failed\n", 2);
		if (!list_unshift_int(a, 3))
			++errors, printf("Test94: list_unshift_int(a, %d) failed\n", 3);
		if (!list_unshift_int(a, 0))
			++errors, printf("Test95: list_unshift_int(a, %d) failed\n", 0);
		if (!list_unshift_int(a, 5))
			++errors, printf("Test96: list_unshift_int(a, %d) failed\n", 5);
		if (!list_unshift_int(a, 6))
			++errors, printf("Test97: list_unshift_int(a, %d) failed\n", 6);
		if (!list_unshift_int(a, 7))
			++errors, printf("Test98: list_unshift_int(a, %d) failed\n", 7);
		if ((item = list_shift_int(a)) != 7)
			++errors, printf("Test99: list_shift_int(a) failed (%d, not %d)\n", item, 7);
		if ((item = list_shift_int(a)) != 6)
			++errors, printf("Test100: list_shift_int(a) failed (%d, not %d)\n", item, 6);
		if ((item = list_shift_int(a)) != 5)
			++errors, printf("Test101: list_shift_int(a) failed (%d, not %d)\n", item, 5);
		if ((item = list_shift_int(a)) != 0)
			++errors, printf("Test102: list_shift_int(a) failed (%d, not %d)\n", item, 0);
		if ((item = list_shift_int(a)) != 3)
			++errors, printf("Test103: list_shift_int(a) failed (%d, not %d)\n", item, 3);
		if ((item = list_shift_int(a)) != 2)
			++errors, printf("Test104: list_shift_int(a) failed (%d, not %d)\n", item, 2);
		if ((item = list_shift_int(a)) != 1)
			++errors, printf("Test105: list_shift_int(a) failed (%d, not %d)\n", item, 1);
		if ((item = list_shift_int(a)) != 0)
			++errors, printf("Test106: list_shift_int(a) failed (%d, not %d)\n", item, 0);

		list_destroy(&a);
		if (a)
			++errors, printf("Test107: list_destroy(&a) failed\n");
	}

	/* Test list_push, list_pop, list_unshift, list_shift */

	if (!(a = list_create(free)))
		++errors, printf("Test108: a = list_create(free) failed\n");
	else
	{
		char *item;

		if (!list_push(a, mem_strdup("1")))
			++errors, printf("Test109: list_push(a, \"%s\") failed\n", "1");
		if (!list_push(a, mem_strdup("2")))
			++errors, printf("Test110: list_push(a, \"%s\") failed\n", "2");
		if (!list_push(a, mem_strdup("3")))
			++errors, printf("Test111: list_push(a, \"%s\") failed\n", "3");
		if (!list_push(a, mem_strdup("4")))
			++errors, printf("Test112: list_push(a, \"%s\") failed\n", "4");
		if (!list_push(a, mem_strdup("5")))
			++errors, printf("Test113: list_push(a, \"%s\") failed\n", "5");
		if (!list_push(a, mem_strdup("6")))
			++errors, printf("Test114: list_push(a, \"%s\") failed\n", "6");
		if (!list_push(a, mem_strdup("7")))
			++errors, printf("Test115: list_push(a, \"%s\") failed\n", "7");
		if (!(item = list_pop(a)) || strcmp(item, "7"))
			++errors, printf("Test116: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "7");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "6"))
			++errors, printf("Test117: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "6");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "5"))
			++errors, printf("Test118: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "5");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "4"))
			++errors, printf("Test119: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "4");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "3"))
			++errors, printf("Test120: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "3");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "2"))
			++errors, printf("Test121: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "2");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "1"))
			++errors, printf("Test122: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "1");
		free(item);
		if ((item = list_pop(a)))
			++errors, printf("Test123: list_pop(empty a) failed (%p, not %p)\n", (void *)item, NULL);

		if (!list_unshift(a, mem_strdup("1")))
			++errors, printf("Test124: list_unshift(a, \"%s\") failed\n", "1");
		if (!list_unshift(a, mem_strdup("2")))
			++errors, printf("Test125: list_unshift(a, \"%s\") failed\n", "2");
		if (!list_unshift(a, mem_strdup("3")))
			++errors, printf("Test126: list_unshift(a, \"%s\") failed\n", "3");
		if (!list_unshift(a, mem_strdup("4")))
			++errors, printf("Test127: list_unshift(a, \"%s\") failed\n", "4");
		if (!list_unshift(a, mem_strdup("5")))
			++errors, printf("Test128: list_unshift(a, \"%s\") failed\n", "5");
		if (!list_unshift(a, mem_strdup("6")))
			++errors, printf("Test129: list_unshift(a, \"%s\") failed\n", "6");
		if (!list_unshift(a, mem_strdup("7")))
			++errors, printf("Test130: list_unshift(a, \"%s\") failed\n", "7");

		if (!(item = list_shift(a)) || strcmp(item, "7"))
			++errors, printf("Test131: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "7");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "6"))
			++errors, printf("Test132: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "6");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "5"))
			++errors, printf("Test133: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "5");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "4"))
			++errors, printf("Test134: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "4");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "3"))
			++errors, printf("Test135: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "3");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "2"))
			++errors, printf("Test136: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "2");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "1"))
			++errors, printf("Test137: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "1");
		free(item);
		if ((item = list_shift(a)))
			++errors, printf("Test138: list_shift(empty a) failed (%p, not %p)\n", (void *)item, NULL);

		list_destroy(&a);
		if (a)
			++errors, printf("Test139: list_destroy(&a) failed\n");
	}

	/* Test list_make, list_splice */

	if (!(a = list_make(NULL, "a", "b", "c", "d", "e", "f", NULL)))
		++errors, printf("Test140: a = list_make(NULL, \"a\", \"b\", \"c\", \"d\", \"e\", \"f\") failed\n");
	else
	{
		List *splice;

		if (!(splice = list_splice(a, 0, 1, NULL)))
			++errors, printf("Test141: list_splice(a, 0, 1) failed\n");
		else
		{
			if (list_length(splice) != 1)
				++errors, printf("Test142: list_splice(a, 0, 1) failed (splice length is %d, not %d)\n", list_length(splice), 1);
			if (strcmp(list_item(splice, 0), "a"))
				++errors, printf("Test143: list_splice(a, 0, 1) failed (splice item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(splice, 0), "a");

			if (strcmp(list_item(a, 0), "b"))
				++errors, printf("Test144: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(a, 0), "b");
			if (strcmp(list_item(a, 1), "c"))
				++errors, printf("Test145: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(a, 1), "c");
			if (strcmp(list_item(a, 2), "d"))
				++errors, printf("Test146: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 2, (char *)list_item(a, 2), "d");
			if (strcmp(list_item(a, 3), "e"))
				++errors, printf("Test147: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 3, (char *)list_item(a, 3), "e");
			if (strcmp(list_item(a, 4), "f"))
				++errors, printf("Test148: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 4, (char *)list_item(a, 4), "f");

			list_destroy(&splice);
			if (splice)
				++errors, printf("Test149: list_destroy(&splice) failed\n");
		}

		if (!(splice = list_splice(a, 4, 1, NULL)))
			++errors, printf("Test150: list_splice(a, 4, 1) failed\n");
		else
		{
			if (list_length(splice) != 1)
				++errors, printf("Test151: list_splice(a, 4, 1) failed (splice length is %d, not %d)\n", list_length(splice), 1);
			if (strcmp(list_item(splice, 0), "f"))
				++errors, printf("Test152: list_splice(a, 4, 1) failed (splice item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(splice, 0), "f");

			if (strcmp(list_item(a, 0), "b"))
				++errors, printf("Test153: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(a, 0), "b");
			if (strcmp(list_item(a, 1), "c"))
				++errors, printf("Test154: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(a, 1), "c");
			if (strcmp(list_item(a, 2), "d"))
				++errors, printf("Test155: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 2, (char *)list_item(a, 2), "d");
			if (strcmp(list_item(a, 3), "e"))
				++errors, printf("Test156: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 3, (char *)list_item(a, 3), "e");

			list_destroy(&splice);
			if (splice)
				++errors, printf("Test157: list_destroy(&splice) failed\n");
		}

		if (!(splice = list_splice(a, 1, 2, NULL)))
			++errors, printf("Test158: list_splice(a, 1, 2) failed\n");
		else
		{
			if (list_length(splice) != 2)
				++errors, printf("Test159: list_splice(a, 1, 2) failed (splice length is %d, not %d)\n", list_length(splice), 2);
			if (strcmp(list_item(splice, 0), "c"))
				++errors, printf("Test160: list_splice(a, 1, 2) failed (splice item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(splice, 0), "c");
			if (strcmp(list_item(splice, 1), "d"))
				++errors, printf("Test161: list_splice(a, 1, 2) failed (splice item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(splice, 1), "d");

			if (strcmp(list_item(a, 0), "b"))
				++errors, printf("Test162: list_splice(a, 1, 2) failed (item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(a, 0), "b");
			if (strcmp(list_item(a, 1), "e"))
				++errors, printf("Test163: list_splice(a, 1, 2) failed (item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(a, 1), "e");

			list_destroy(&splice);
			if (splice)
				++errors, printf("Test164: list_destroy(&splice) failed\n");
		}

		list_destroy(&a);
		if (a)
			++errors, printf("Test165: list_destroy(&a) failed\n");
	}

	/* Test MT Safety */

	debug = (ac != 1);

	if (debug)
		setbuf(stdout, NULL);

	if (debug)
		locker = locker_create_debug_rwlock(&rwlock);
	else
		locker = locker_create_rwlock(&rwlock);

	if (!locker)
		++errors, printf("Test166: locker_create_rwlock() failed\n");
	else
	{
		mt_test(166, locker);
		locker_destroy(&locker);
	}

	if (debug)
		locker = locker_create_debug_mutex(&mutex);
	else
		locker = locker_create_mutex(&mutex);

	if (!locker)
		++errors, printf("Test167: locker_create_mutex() failed\n");
	else
	{
		mt_test(167, locker);
		locker_destroy(&locker);
	}

	if (errors)
		printf("%d/167 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
