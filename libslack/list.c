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

/*

=head1 NAME

I<libslack(list)> - list module

=head1 SYNOPSIS

    #include <slack/list.h>

    typedef struct List List;
    typedef struct Lister Lister;
    typedef void list_destroy_t(void *item);
    typedef void *list_copy_t(const void *item);
    typedef int list_cmp_t(const void *a, const void *b);
    typedef void list_action_t(void *item, size_t *index, void *data);
    typedef void *list_map_t(void *item, size_t *index, void *data);
    typedef int list_query_t(void *item, size_t *index, void *data);

    List *list_create(list_destroy_t *destroy);
    List *list_make(list_destroy_t *destroy, ...);
    List *list_vmake(list_destroy_t *destroy, va_list args);
    List *list_copy(const List *src, list_copy_t *copy);
    void list_release(List *list);
    #define list_destroy(list)
    void *list_destroy_func(List **list);
    int list_own(List *list, list_destroy_t *destroy);
    list_destroy_t *list_disown(List *list);
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
    List *list_map(List *list, list_destroy_t *destroy, list_map_t *map, void *data);
    List *list_grep(List *list, list_query_t *grep, void *data);
    ssize_t list_ask(List *list, ssize_t *index, list_query_t *query, void *data);
    Lister *lister_create(List *list);
    void lister_release(Lister * lister);
    #define lister_destroy(lister)
    void *lister_destroy_func(Lister **lister);
    int lister_has_next(Lister *lister);
    void *lister_next(Lister *lister);
    int lister_next_int(Lister *lister);
    void lister_remove(Lister *lister);
    int list_has_next(List *list);
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
#include "hsort.h"

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
	list_destroy_t *destroy; /* item destructor, if any */
	Lister *lister;          /* built-in iterator */
};

struct Lister
{
	List *list;    /* the list being iterated over */
	ssize_t index; /* the index of the current item */
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
		return mem_resize(list->list, list->size) ? 0 : -1;

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
		return mem_resize(list->list, list->size) ? 0 : -1;

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

Slides C<list>'s items, starting at C<index>, C<range> slots to the left to
close a gap. On success, returns 0. On error, returns -1.

*/

static int contract(List *list, ssize_t index, size_t range)
{
	memmove(list->list + index, list->list + index + range, (list->length - index) * sizeof(*list->list));

	if (shrink(list, range) == -1)
		return -1;

	list->length -= range;

	return 0;
}

/*

C<int adjust(List *list, ssize_t index, size_t range, size_t length)>

Expands or contracts C<list> as required so that I<list[index + range ..]>
occupies I<list[index + length ..]>. On success, returns 0. On error,
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

Destroys the items in I<list> ranging from C<index> to C<range>.

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

/*

=item C<List *list_create(list_destroy_t *destroy)>

Creates a I<List> with C<destroy> as its item destructor. On success,
returns the new list. On error, returns C<NULL>.

=cut

*/

List *list_create(list_destroy_t *destroy)
{
	List *list;

	if (!(list = mem_new(List)))
		return NULL;

	list->size = list->length = 0;
	list->list = NULL;
	list->destroy = destroy;
	list->lister = NULL;

	return list;
}

/*

=item C<List *list_make(list_destroy_t *destroy, ...)>

Creates a I<List> with C<destroy> as its item destructor and the remaining
arguments as its initial items. On success, returns the new list. On error,
return C<NULL>.

=cut

*/

List *list_make(list_destroy_t *destroy, ...)
{
	List *list;
	va_list args;
	va_start(args, destroy);
	list = list_vmake(destroy, args);
	va_end(args);
	return list;
}

/*

=item C<List *list_vmake(list_destroy_t *destroy, va_list args)>

Equivalent to I<list_make()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

List *list_vmake(list_destroy_t *destroy, va_list args)
{
	List *list;
	void *item;

	if (!(list = list_create(destroy)))
		return NULL;

	while ((item = va_arg(args, void *)) != NULL)
		list_append(list, item);

	return list;
}

/*

=item C<List *list_copy(const List *src, list_copy_t *copy)>

Creates a clone of C<src> using C<copy> as the copy constructor (if not
C<NULL>). On success, returns the clone. On error, returns C<NULL>.

=cut

*/

List *list_copy(const List *src, list_copy_t *copy)
{
	if (!src)
		return NULL;

	return list_extract(src, 0, src->length, copy);
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

	if (list->list)
	{
		killitems(list, 0, list->length);
		mem_release(list->list);
	}

	lister_release(list->lister);
	mem_release(list);
}

/*

=item C< #define list_destroy(list)>

Destroys (deallocates and sets to C<NULL>) C<list>. Returns C<NULL>.

=item C<void *list_destroy_func(List **list)>

Destroys (deallocates and sets to C<NULL>) C<list>. Returns C<NULL>. This
function is exposed as an implementation side effect. Don't call it
directly. Call I<list_destroy()> instead.

=cut

*/

void *list_destroy_func(List **list)
{
	if (list && *list)
	{
		list_release(*list);
		*list = NULL;
	}

	return NULL;
}

/*

=item C<int list_own(List *list, list_destroy_t *destroy)>

Causes C<list> to take ownership of its items. The items will be destroyed
using C<destroy> when they are removed or when C<list> is destroyed.
On success, returns 0. On error, returns -1.

=cut

*/

int list_own(List *list, list_destroy_t *destroy)
{
	if (!list || !destroy)
		return -1;

	list->destroy = destroy;
	return 0;
}

/*

=item C<list_destroy_t *list_disown(List *list)>

Causes C<list> to relinquish ownership of its items. The items will not be
destroyed when they are removed from C<list> or when C<list> is destroyed.
On success, returns the previous destroy function, if any. On error, returns
C<NULL>.

=cut

*/

list_destroy_t *list_disown(List *list)
{
	list_destroy_t *destroy;

	if (!list)
		return NULL;

	destroy = list->destroy;
	list->destroy = NULL;
	return destroy;
}

/*

=item C<void *list_item(const List *list, size_t index)>

Returns the C<index>'th item in C<list>. On error, returns C<NULL>.

=cut

*/

void *list_item(const List *list, size_t index)
{
	if (!list)
		return NULL;

	if (index >= list->length)
		return NULL;

	return list->list[index];
}

/*

=item C<int list_item_int(const List *list, size_t index)>

Returns the C<index>'th item in C<list> as an integer. On error, returns 0.

=cut

*/

int list_item_int(const List *list, size_t index)
{
	if (!list)
		return 0;

	if (index >= list->length)
		return 0;

	return (int)(list->list[index]);
}

/*

=item C<int list_empty(const List *list)>

Returns whether or not C<list> is empty.

=cut

*/

int list_empty(const List *list)
{
	return !list || !list->length;
}

/*

=item C<size_t list_length(const List *list)>

Returns the length of C<list>.

=cut

*/

size_t list_length(const List *list)
{
	return (list) ? list->length : 0;
}

/*

=item C<ssize_t list_last(const List *list)>

Returns the index of the last item in C<list>, or -1 if there are no items.

=cut

*/

ssize_t list_last(const List *list)
{
	return (list) ? list->length - 1 : -1;
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

=item C<List *list_remove_range(List *list, size_t index, size_t range)>

Removes C<range> items from C<list> starting at C<index>. On success, returns
C<list>. On error, returns C<NULL>.

=cut

*/

List *list_remove_range(List *list, size_t index, size_t range)
{
	if (!list || list->length < index + range)
		return NULL;

	killitems(list, index, range);

	if (contract(list, index, range) == -1)
		return NULL;

	return list;
}

/*

=item C<List *list_insert(List *list, size_t index, void *item)>

Adds C<item> to C<list> at position C<index>. On success, returns C<list>.
On error, returns C<NULL>.

=cut

*/

List *list_insert(List *list, size_t index, void *item)
{
	if (!list || list->length < index)
		return NULL;

	if (expand(list, index, 1) == -1)
		return NULL;

	list->list[index] = item;

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

	if (!src || !list || list->length < index || xor(list->destroy, copy))
		return NULL;

	if (expand(list, index, src->length) == -1)
		return NULL;

	for (i = 0; i < src->length; ++i)
		list->list[index + i] = enlist(src->list[i], copy);

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
	if (!list || list->length < index + range)
		return NULL;

	killitems(list, index, range);

	if (adjust(list, index, range, 1) == -1)
		return NULL;

	list->list[index] = item;

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

	if (!src || !list || list->length < index + range || xor(list->destroy, copy))
		return NULL;

	killitems(list, index, range);

	if (adjust(list, index, range, length = list_length(src)) == -1)
		return NULL;

	while (length--)
		list->list[index + length] = enlist(src->list[length], copy);

	return list;
}

/*

=item C<List *list_extract(const List *list, size_t index, size_t range, list_copy_t *copy)>

Creates a new list consisting of C<range> items from C<list>, starting at
C<index>, using C<copy> as the copy constructor (if not C<NULL>). On success,
returns the new list. On error, returns C<NULL>.

=cut

*/

List *list_extract(const List *list, size_t index, size_t range, list_copy_t *copy)
{
	List *ret;

	if (!list || list->length < index + range || xor(list->destroy, copy))
		return NULL;

	if (!(ret = list_create(copy ? list->destroy : NULL)))
		return NULL;

	while (range--)
		if (list_append(ret, enlist(list->list[index++], copy)) == NULL)
			return list_destroy(ret);

	return ret;
}

#undef enlist

/*

=item C<List *list_push(List *list, void *item)>

Pushes C<item> onto the end of C<list>. On success, returns I<list>. Om
error, returns C<NULL>.

=cut

*/

List *list_push(List *list, void *item)
{
	return list_append(list, item);
}

/*

=item C<List *list_push_int(List *list, int item)>

Pushes C<item> onto the end of C<list>. On success, returns I<list>. Om
error, returns C<NULL>.

=cut

*/

List *list_push_int(List *list, int item)
{
	return list_append_int(list, item);
}

/*

=item C<void *list_pop(List *list)>

Pops the last item off I<list>. On success, returns the item popped. On
error, returns C<NULL>.

=cut

*/

void *list_pop(List *list)
{
	void *item;

	if (!list || !list->length)
		return NULL;

	item = list->list[list->length - 1];
	list->list[list->length - 1] = NULL;

	if (!list_remove(list, list->length - 1))
		return list->list[list->length - 1] = item, NULL;

	return item;
}

/*

=item C<int list_pop_int(List *list)>

Pops the last item off I<list>. On success, returns the item popped. On
error, returns C<0>.

=cut

*/

int list_pop_int(List *list)
{
	return (int)list_pop(list);
}

/*

=item C<void *list_shift(List *list)>

Removes and returns the first item in I<list>. On success, returns the item
shifted. On error, returns C<NULL>.

=cut

*/

void *list_shift(List *list)
{
	void *item;

	if (!list || !list->length)
		return NULL;

	item = list->list[0];
	list->list[0] = NULL;

	if (!list_remove(list, 0))
		return list->list[0] = item, NULL;

	return item;
}

/*

=item C<int list_shift_int(List *list)>

Removes and returns the first item in I<list>. On success, returns the item
shifted. On error, returns C<0>.

=cut

*/

int list_shift_int(List *list)
{
	return (int)list_shift(list);
}

/*

=item C<List *list_unshift(List *list, void *item)>

Inserts I<item> at the start of I<list>. On success, returns I<list>. On
error, returns C<NULL>.

=cut

*/

List *list_unshift(List *list, void *item)
{
	return list_prepend(list, item);
}

/*

=item C<List *list_unshift_int(List *list, int item)>

Inserts I<item> at the start of I<list>. On success, returns I<list>. On
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

	if (!list || list->length < index + range)
		return NULL;

	ret = list_extract(list, index, range, copy);
	if (!ret)
		return NULL;

	if (!list_remove_range(list, index, range))
	{
		list_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<List *list_sort(List *list, list_cmp_t *cmp)>

Sorts the items in C<list> using the item comparison function C<cmp> and
I<qsort(3)> for lists of fewer than 1000 items and I<hsort(3)> for larger
lists. On success, returns C<list>. On error, returns C<NULL>.

=cut

*/

List *list_sort(List *list, list_cmp_t *cmp)
{
	if (!list || !list->list || !list->length)
		return NULL;

	((list->length >= 10000) ? hsort : qsort)(list->list, list->length, sizeof list->list[0], cmp);

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

	for (i = 0; i < list->length; ++i)
		action(list->list[i], &i, data);
}

/*

=item C<List *list_map(List *list, list_destroy *destroy, list_map_t *map, void *data)>

Returns a new list containing the return values of C<map>, invoked once for
each item in C<list>. The arguments passed to C<map> are the item, a pointer
to the loop variable containing the item's position within C<list> and
C<data>. C<destroy> will be the destroy function for the returned list.
The user is responsible for deallocating the returned list with
I<list_destroy()> or I<list_release()>. On success, returns the new list. On
error, returns C<NULL>.

=cut

*/

List *list_map(List *list, list_destroy_t *destroy, list_map_t *map, void *data)
{
	List *mapping;
	size_t i;

	if (!list || !map)
		return NULL;

	if (!(mapping = list_create(destroy)))
		return NULL;

	for (i = 0; i < list->length; ++i)
	{
		if (!list_append(mapping, map(list->list[i], &i, data)))
		{
			list_release(mapping);
			return NULL;
		}
	}

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

	for (i = 0; i < list->length; ++i)
	{
		if (grep(list->list[i], &i, data))
			if (!list_append(grepping, list->list[i]))
			{
				list_release(grepping);
				return NULL;
			}
	}

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
	size_t i;

	if (!list || !index || *index >= list->length || !query)
		return -1;

	for (i = *index; i < list->length; ++i)
		if (query(list->list[i], (size_t *)index, data))
			return *index = i;

	return *index = -1;
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
	mem_release(lister);
}

/*

=item C< #define lister_destroy(lister)>

Destroys (deallocates and sets to C<NULL>) C<lister>.

=item C<void *lister_destroy_func(Lister **lister)>

Destroys (deallocates and sets to C<NULL>) C<lister>. Returns C<NULL>. This
function is exposed as an implementation side effect. Don't call it
directly. Call I<lister_destroy()> instead.

=cut

*/

void *lister_destroy_func(Lister **lister)
{
	if (lister && *lister)
	{
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
	return lister && lister->index + 1 < lister->list->length;
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

	return list_item(lister->list, (size_t)++lister->index);
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

	return list_item_int(lister->list, (size_t)++lister->index);
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
	if (!lister || lister->index == -1)
		return;

	list_remove(lister->list, (size_t)lister->index--);
}

/*

=item C<int list_has_next(List *list)>

Returns whether or not there is another item in C<list>. The first time this
is called, a new, internal I<Lister> will be created (Note: there can be
only one). When there are no more items, returns zero and destroys the
internal iterator. When it returns a non-zero value, use I<list_next()> to
retrieve the next item.

=cut

*/

int list_has_next(List *list)
{
	int ret;

	if (!list)
		return 0;

	if (!list->lister)
		list->lister = lister_create(list);

	ret = lister_has_next(list->lister);
	if (!ret)
		lister_destroy(list->lister);

	return ret;
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

=head1 BUGS

Little attempt is made to protect the client from sharing items between
lists with differing ownership policies and getting it wrong. When copying
items from any list to an owning list, a copy function must be supplied.
When adding a single item to an owning list, it is assumed that the list may
take over ownership of the item. When an owning list is destroyed, all of
its items are destroyed. If any of these items had been shared with a
non-owning list that outlived the owning list, then the non-owning list will
contain items that point to deallocated memory.

=head1 SEE ALSO

L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<msg(3)|msg(3)>,
L<net(3)|net(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<str(3)|str(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

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
	strcat(action_data, item);
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

int main(int ac, char **av)
{
	int errors = 0;
	List *a, *b, *c, *d;
	Lister *lister;
	ssize_t index = 0;
	int i;

	printf("Testing: list\n");

	a = list_make(NULL, "abc", "def", "ghi", "jkl", NULL);
	if (!a)
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

	list_destroy(c);
	if (c)
		++errors, printf("Test 11: list_destroy() failed\n");

	if (!(c = list_create((list_destroy_t *)free)))
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

	if (!(lister = lister_create(a)))
		++errors, printf("Test32: lister_create() failed\n");

	for (i = 0; lister_has_next(lister); ++i)
	{
		void *item = lister_next(lister);

		if (item != list_item(a, i))
		{
			++errors, printf("Test33: iteration %d is '%s' not '%s'\n", i, (char *)item, (char *)list_item(a, i));
			break;
		}
	}

	lister_destroy(lister);
	if (lister)
		++errors, printf("Test34: lister_destroy(lister) failed, lister is %p not null\n", lister);

	for (i = 0; list_has_next(a); ++i)
	{
		void *item = list_next(a);

		if (item != list_item(a, i))
		{
			++errors, printf("Test35: internal iteration %d is '%s' not '%s'\n", i, (char *)item, (char *)list_item(a, i));
			break;
		}
	}

	if (!(lister = lister_create(a)))
		++errors, printf("Test36: lister_create() failed\n");

	for (i = 0; lister_has_next(lister); ++i)
	{
		lister_next(lister);

		if (i == 4)
			lister_remove(lister);
	}

	if (!list_item(a, 0) || strcmp(list_item(a, 0), "123"))
		++errors, printf("Test37: list_sort(): 1st item is '%s' not '123'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test37: list_sort(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "abc"))
		++errors, printf("Test37: list_sort(): 3rd item is '%s' not 'abc'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "abc"))
		++errors, printf("Test37: list_sort(): 4th item is '%s' not 'abc'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test37: list_sort(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "def"))
		++errors, printf("Test37: list_sort(): 6th item is '%s' not 'def'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "ghi"))
		++errors, printf("Test37: list_sort(): 7th item is '%s' not 'ghi'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test37: list_sort(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "jkl"))
		++errors, printf("Test37: list_sort(): 9th item is '%s' not 'jkl'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "jkl"))
		++errors, printf("Test37: list_sort(): 10th item is '%s' not 'jkl'\n", (char *)list_item(a, 9));

	lister_destroy(lister);
	if (lister)
		++errors, printf("Test38: lister_destroy(lister) failed, lister is %p not null\n", lister);

	list_destroy(a);
	if (a)
		++errors, printf("Test39: list_destroy(a) failed, a is %p not null\n", a);

	list_destroy(b);
	if (b)
		++errors, printf("Test40: list_destroy(b) failed, b is %p not null\n", b);

	list_destroy(c);
	if (c)
		++errors, printf("Test41: list_destroy(c) failed, c is %p not null\n", c);

	list_destroy(d);
	if (d)
		++errors, printf("Test42: list_destroy(d) failed, d is %p not null\n", d);

	a = list_create(NULL);
	if (!a)
		++errors, printf("Test43: list_create(NULL) failed\n");

	if (!list_append_int(a, 2))
		++errors, printf("Test44: list_append_int(a, 2) failed\n");
	if (list_item_int(a, 0) != 2)
		++errors, printf("Test45: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 2);

	if (!list_prepend_int(a, 0))
		++errors, printf("Test46: list_prepend_int(a, 0) failed\n");
	if (list_item_int(a, 0) != 0)
		++errors, printf("Test47: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 0);
	if (list_item_int(a, 1) != 2)
		++errors, printf("Test48: list_item_int(a, 1) failed (%d, not %d)\n", list_item_int(a, 1), 2);

	if (!list_insert_int(a, 1, 1))
		++errors, printf("Test49: list_insert_int(a, 1, 1) failed\n");
	if (list_item_int(a, 0) != 0)
		++errors, printf("Test50: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 0);
	if (list_item_int(a, 1) != 1)
		++errors, printf("Test51: list_item_int(a, 1) failed (%d, not %d)\n", list_item_int(a, 1), 1);
	if (list_item_int(a, 2) != 2)
		++errors, printf("Test52: list_item_int(a, 2) failed (%d, not %d)\n", list_item_int(a, 2), 2);

	for (i = 0; list_has_next(a); ++i)
	{
		int item = list_next_int(a);
		if (item != list_item_int(a, i))
			++errors, printf("Test53: int list test failed (item %d = %d, not %d)\n", i, item, list_item_int(a, i));
	}

	if (i != 3)
		++errors, printf("Test54: list_has_next() failed (only %d items, not %d)\n", i, 3);

	if (!(lister = lister_create(a)))
		++errors, printf("Test55: lister_create(a) failed\n");
	else
	{
		for (i = 0; lister_has_next(lister); ++i)
		{
			int item = lister_next_int(lister);
			if (item != list_item_int(a, i))
				++errors, printf("Test56: int list test failed (item %d = %d, not %d)\n", i, item, i);
		}

		if (i != 3)
			++errors, printf("Test57: lister_has_next() failed (only %d items, not %d)\n", i, 3);
	}

	if (!list_replace_int(a, 2, 1, 4))
		++errors, printf("Test58: list_replace_int(a, 2, 1, 4) failed\n");
	if (list_item_int(a, 0) != 0)
		++errors, printf("Test59: list_item_int(a, 0) failed (%d, not %d)\n", list_item_int(a, 0), 0);
	if (list_item_int(a, 1) != 1)
		++errors, printf("Test60: list_item_int(a, 1) failed (%d, not %d)\n", list_item_int(a, 1), 1);
	if (list_item_int(a, 2) != 4)
		++errors, printf("Test61: list_item_int(a, 2) failed (%d, not %d)\n", list_item_int(a, 2), 4);

	i = 0;
	if (!(b = list_map(a, NULL, (list_map_t *)mapf, &i)))
		++errors, printf("Test62: list_map() failed\n");
	else
	{
		if (list_length(b) != 3)
			++errors, printf("Test63: list_map() failed (length %d, not %d)\n", list_length(b), 3);
		else
		{
			if (list_item_int(b, 0) != 0)
				++errors, printf("Test64: list_map() failed (item %d = %d, not %d)\n", 0, list_item_int(b, 0), 0);
			if (list_item_int(b, 1) != 1)
				++errors, printf("Test65: list_map() failed (item %d = %d, not %d)\n", 1, list_item_int(b, 1), 1);
			if (list_item_int(b, 2) != 5)
				++errors, printf("Test66: list_map() failed (item %d = %d, not %d)\n", 2, list_item_int(b, 2), 5);
		}

		list_destroy(b);
		if (b)
			++errors, printf("Test67: list_destroy(b) failed\n");
	}

	if (!(b = list_grep(a, (list_query_t *)grepf, NULL)))
		++errors, printf("Test68: list_grep() failed\n");
	else
	{
		if (list_length(b) != 2)
			++errors, printf("Test69: list_grep() failed (length %d, not %d)\n", list_length(b), 2);
		else
		{
			if (list_item_int(b, 0) != 0)
				++errors, printf("Test70: list_map() failed (item %d = %d, not %d)\n", 0, list_item_int(b, 0), 0);
			if (list_item_int(b, 1) != 4)
				++errors, printf("Test71: list_map() failed (item %d = %d, not %d)\n", 1, list_item_int(b, 1), 4);
		}

		list_destroy(b);
		if (b)
			++errors, printf("Test72: list_destroy(b) failed\n");
	}

	if (!(a = list_create(NULL)))
		++errors, printf("Test73: a = list_create(NULL) failed\n");
	else
	{
		int item;

		if (!list_push_int(a, 1))
			++errors, printf("Test74: list_push_int(a, %d) failed\n", 1);
		if (!list_push_int(a, 2))
			++errors, printf("Test75: list_push_int(a, %d) failed\n", 2);
		if (!list_push_int(a, 3))
			++errors, printf("Test76: list_push_int(a, %d) failed\n", 3);
		if (!list_push_int(a, 0))
			++errors, printf("Test77: list_push_int(a, %d) failed\n", 0);
		if (!list_push_int(a, 5))
			++errors, printf("Test78: list_push_int(a, %d) failed\n", 5);
		if (!list_push_int(a, 6))
			++errors, printf("Test79: list_push_int(a, %d) failed\n", 6);
		if (!list_push_int(a, 7))
			++errors, printf("Test80: list_push_int(a, %d) failed\n", 7);
		if ((item = list_pop_int(a)) != 7)
			++errors, printf("Test81: list_pop_int(a) failed (%d, not %d)\n", item, 7);
		if ((item = list_pop_int(a)) != 6)
			++errors, printf("Test82: list_pop_int(a) failed (%d, not %d)\n", item, 6);
		if ((item = list_pop_int(a)) != 5)
			++errors, printf("Test83: list_pop_int(a) failed (%d, not %d)\n", item, 5);
		if ((item = list_pop_int(a)) != 0)
			++errors, printf("Test84: list_pop_int(a) failed (%d, not %d)\n", item, 0);
		if ((item = list_pop_int(a)) != 3)
			++errors, printf("Test85: list_pop_int(a) failed (%d, not %d)\n", item, 3);
		if ((item = list_pop_int(a)) != 2)
			++errors, printf("Test86: list_pop_int(a) failed (%d, not %d)\n", item, 2);
		if ((item = list_pop_int(a)) != 1)
			++errors, printf("Test87: list_pop_int(a) failed (%d, not %d)\n", item, 1);
		if ((item = list_pop_int(a)) != 0)
			++errors, printf("Test88: list_pop_int(a) failed (%d, not %d)\n", item, 0);

		if (!list_unshift_int(a, 1))
			++errors, printf("Test89: list_unshift_int(a, %d) failed\n", 1);
		if (!list_unshift_int(a, 2))
			++errors, printf("Test90: list_unshift_int(a, %d) failed\n", 2);
		if (!list_unshift_int(a, 3))
			++errors, printf("Test91: list_unshift_int(a, %d) failed\n", 3);
		if (!list_unshift_int(a, 0))
			++errors, printf("Test92: list_unshift_int(a, %d) failed\n", 0);
		if (!list_unshift_int(a, 5))
			++errors, printf("Test93: list_unshift_int(a, %d) failed\n", 5);
		if (!list_unshift_int(a, 6))
			++errors, printf("Test94: list_unshift_int(a, %d) failed\n", 6);
		if (!list_unshift_int(a, 7))
			++errors, printf("Test95: list_unshift_int(a, %d) failed\n", 7);
		if ((item = list_shift_int(a)) != 7)
			++errors, printf("Test96: list_shift_int(a) failed (%d, not %d)\n", item, 7);
		if ((item = list_shift_int(a)) != 6)
			++errors, printf("Test97: list_shift_int(a) failed (%d, not %d)\n", item, 6);
		if ((item = list_shift_int(a)) != 5)
			++errors, printf("Test98: list_shift_int(a) failed (%d, not %d)\n", item, 5);
		if ((item = list_shift_int(a)) != 0)
			++errors, printf("Test99: list_shift_int(a) failed (%d, not %d)\n", item, 0);
		if ((item = list_shift_int(a)) != 3)
			++errors, printf("Test100: list_shift_int(a) failed (%d, not %d)\n", item, 3);
		if ((item = list_shift_int(a)) != 2)
			++errors, printf("Test101: list_shift_int(a) failed (%d, not %d)\n", item, 2);
		if ((item = list_shift_int(a)) != 1)
			++errors, printf("Test102: list_shift_int(a) failed (%d, not %d)\n", item, 1);
		if ((item = list_shift_int(a)) != 0)
			++errors, printf("Test103: list_shift_int(a) failed (%d, not %d)\n", item, 0);

		list_destroy(a);
		if (a)
			++errors, printf("Test104: list_destroy(a) failed\n");
	}

	if (!(a = list_create(free)))
		++errors, printf("Test105: a = list_create(free) failed\n");
	else
	{
		char *item;

		if (!list_push(a, mem_strdup("1")))
			++errors, printf("Test106: list_push(a, \"%s\") failed\n", "1");
		if (!list_push(a, mem_strdup("2")))
			++errors, printf("Test107: list_push(a, \"%s\") failed\n", "2");
		if (!list_push(a, mem_strdup("3")))
			++errors, printf("Test108: list_push(a, \"%s\") failed\n", "3");
		if (!list_push(a, mem_strdup("4")))
			++errors, printf("Test109: list_push(a, \"%s\") failed\n", "4");
		if (!list_push(a, mem_strdup("5")))
			++errors, printf("Test110: list_push(a, \"%s\") failed\n", "5");
		if (!list_push(a, mem_strdup("6")))
			++errors, printf("Test111: list_push(a, \"%s\") failed\n", "6");
		if (!list_push(a, mem_strdup("7")))
			++errors, printf("Test112: list_push(a, \"%s\") failed\n", "7");
		if (!(item = list_pop(a)) || strcmp(item, "7"))
			++errors, printf("Test113: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "7");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "6"))
			++errors, printf("Test114: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "6");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "5"))
			++errors, printf("Test115: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "5");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "4"))
			++errors, printf("Test116: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "4");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "3"))
			++errors, printf("Test117: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "3");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "2"))
			++errors, printf("Test118: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "2");
		free(item);
		if (!(item = list_pop(a)) || strcmp(item, "1"))
			++errors, printf("Test119: list_pop(a) failed (\"%s\", not \"%s\")\n", item, "1");
		free(item);
		if ((item = list_pop(a)))
			++errors, printf("Test120: list_pop(empty a) failed (%p, not %p)\n", item, NULL);

		if (!list_unshift(a, mem_strdup("1")))
			++errors, printf("Test121: list_unshift(a, \"%s\") failed\n", "1");
		if (!list_unshift(a, mem_strdup("2")))
			++errors, printf("Test122: list_unshift(a, \"%s\") failed\n", "2");
		if (!list_unshift(a, mem_strdup("3")))
			++errors, printf("Test123: list_unshift(a, \"%s\") failed\n", "3");
		if (!list_unshift(a, mem_strdup("4")))
			++errors, printf("Test124: list_unshift(a, \"%s\") failed\n", "4");
		if (!list_unshift(a, mem_strdup("5")))
			++errors, printf("Test125: list_unshift(a, \"%s\") failed\n", "5");
		if (!list_unshift(a, mem_strdup("6")))
			++errors, printf("Test126: list_unshift(a, \"%s\") failed\n", "6");
		if (!list_unshift(a, mem_strdup("7")))
			++errors, printf("Test127: list_unshift(a, \"%s\") failed\n", "7");

		if (!(item = list_shift(a)) || strcmp(item, "7"))
			++errors, printf("Test128: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "7");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "6"))
			++errors, printf("Test129: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "6");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "5"))
			++errors, printf("Test130: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "5");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "4"))
			++errors, printf("Test131: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "4");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "3"))
			++errors, printf("Test132: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "3");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "2"))
			++errors, printf("Test133: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "2");
		free(item);
		if (!(item = list_shift(a)) || strcmp(item, "1"))
			++errors, printf("Test134: list_shift(a) failed (\"%s\", not \"%s\")\n", item, "1");
		free(item);
		if ((item = list_shift(a)))
			++errors, printf("Test135: list_shift(empty a) failed (%p, not %p)\n", item, NULL);

		list_destroy(a);
		if (a)
			++errors, printf("Test136: list_destroy(a) failed\n");
	}

	if (!(a = list_make(NULL, "a", "b", "c", "d", "e", "f", NULL)))
		++errors, printf("Test137: a = list_make(NULL, \"a\", \"b\", \"c\", \"d\", \"e\", \"f\") failed\n");
	else
	{
		List *splice;

		if (!(splice = list_splice(a, 0, 1, NULL)))
			++errors, printf("Test138: list_splice(a, 0, 1) failed\n");
		else
		{
			if (list_length(splice) != 1)
				++errors, printf("Test139: list_splice(a, 0, 1) failed (splice length is %d, not %d)\n", list_length(splice), 1);
			if (strcmp(list_item(splice, 0), "a"))
				++errors, printf("Test140: list_splice(a, 0, 1) failed (splice item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(splice, 0), "a");

			if (strcmp(list_item(a, 0), "b"))
				++errors, printf("Test141: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(a, 0), "b");
			if (strcmp(list_item(a, 1), "c"))
				++errors, printf("Test142: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(a, 1), "c");
			if (strcmp(list_item(a, 2), "d"))
				++errors, printf("Test143: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 2, (char *)list_item(a, 2), "d");
			if (strcmp(list_item(a, 3), "e"))
				++errors, printf("Test144: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 3, (char *)list_item(a, 3), "e");
			if (strcmp(list_item(a, 4), "f"))
				++errors, printf("Test145: list_splice(a, 0, 1) failed (item %d is \"%s\", not \"%s\")\n", 4, (char *)list_item(a, 4), "f");

			list_destroy(splice);
			if (splice)
				++errors, printf("Test146: list_destroy(splice) failed\n");
		}

		if (!(splice = list_splice(a, 4, 1, NULL)))
			++errors, printf("Test147: list_splice(a, 4, 1) failed\n");
		else
		{
			if (list_length(splice) != 1)
				++errors, printf("Test148: list_splice(a, 4, 1) failed (splice length is %d, not %d)\n", list_length(splice), 1);
			if (strcmp(list_item(splice, 0), "f"))
				++errors, printf("Test149: list_splice(a, 4, 1) failed (splice item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(splice, 0), "f");

			if (strcmp(list_item(a, 0), "b"))
				++errors, printf("Test150: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(a, 0), "b");
			if (strcmp(list_item(a, 1), "c"))
				++errors, printf("Test151: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(a, 1), "c");
			if (strcmp(list_item(a, 2), "d"))
				++errors, printf("Test152: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 2, (char *)list_item(a, 2), "d");
			if (strcmp(list_item(a, 3), "e"))
				++errors, printf("Test153: list_splice(a, 4, 1) failed (item %d is \"%s\", not \"%s\")\n", 3, (char *)list_item(a, 3), "e");

			list_destroy(splice);
			if (splice)
				++errors, printf("Test154: list_destroy(splice) failed\n");
		}

		if (!(splice = list_splice(a, 1, 2, NULL)))
			++errors, printf("Test155: list_splice(a, 1, 2) failed\n");
		else
		{
			if (list_length(splice) != 2)
				++errors, printf("Test156: list_splice(a, 1, 2) failed (splice length is %d, not %d)\n", list_length(splice), 2);
			if (strcmp(list_item(splice, 0), "c"))
				++errors, printf("Test157: list_splice(a, 1, 2) failed (splice item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(splice, 0), "c");
			if (strcmp(list_item(splice, 1), "d"))
				++errors, printf("Test158: list_splice(a, 1, 2) failed (splice item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(splice, 1), "d");

			if (strcmp(list_item(a, 0), "b"))
				++errors, printf("Test159: list_splice(a, 1, 2) failed (item %d is \"%s\", not \"%s\")\n", 0, (char *)list_item(a, 0), "b");
			if (strcmp(list_item(a, 1), "e"))
				++errors, printf("Test160: list_splice(a, 1, 2) failed (item %d is \"%s\", not \"%s\")\n", 1, (char *)list_item(a, 1), "e");

			list_destroy(splice);
			if (splice)
				++errors, printf("Test161: list_destroy(splice) failed\n");
		}

		list_destroy(a);
		if (a)
			++errors, printf("Test162: list_destroy(a) failed\n");
	}

	if (errors)
		printf("%d/162 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
