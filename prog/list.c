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

/*

=head1 NAME

I<libprog(list)> - list module

=head1 SYNOPSIS

    #include <prog/list.h>

    typedef struct List List;
    typedef struct Lister Lister;
    typedef void list_destroy_t(void *);
    typedef void *list_copy_t(void *);
    typedef int list_cmp_t(const void *, const void *);
    typedef void list_action_t(void *, size_t *);
    typedef int list_query_t(void *, size_t *);

    #define list_destroy(list)
    #define lister_destroy(lister)

    List *list_create(list_destroy_t *destroy);
    List *list_make(list_destroy_t *destroy, ...);
    List *list_copy(List *src, list_copy_t *copy);
    void list_release(List *list);
    void *list_destroy_func(List **list);
    void *list_item(List *list, size_t index);
    int list_item_int(List *list, size_t index);
    int list_empty(List *list);
    size_t list_length(List *list);
    ssize_t list_last(List *list);
    List *list_remove(List *list, size_t index);
    List *list_remove_range(List *list, size_t index, size_t range);
    List *list_insert(List *list, size_t index, void *item);
    List *list_insert_list(List *dst, size_t index, List *src, list_copy_t *copy);
    List *list_append(List *list, void *item);
    List *list_append_list(List *dst, List *src, list_copy_t *copy);
    List *list_prepend(List *list, void *item);
    List *list_prepend_list(List *dst, List *src, list_copy_t *copy);
    List *list_replace(List *list, size_t index, size_t range, void *item);
    List *list_replace_list(List *dst, size_t index, size_t range, List *src, list_copy_t *copy);
    List *list_extract(List *orig, size_t index, size_t range, list_copy_t *copy);
    List *list_sort(List *list, list_cmp_t *cmp);
    void list_apply(List *list, list_action_t *action);
    ssize_t list_ask(List *list, ssize_t *index, list_query_t *query);
    Lister *lister_create(List *list);
    void lister_release(Lister * lister);
    void *lister_destroy_func(Lister **lister);
    int list_has_next(Lister *lister);
    void *list_next(Lister *lister);
    void lister_remove(Lister *lister);

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

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "list.h"
#include "mem.h"
#include "hsort.h"

#define xor(a, b) (!(a) ^ !(b))
#define iff(a, b) !xor(a, b)
#define implies(a, b) (!(a) || (b))

static const size_t chunk = 25;

struct List
{
	size_t size;             /* number of item slots allocated */
	size_t length;           /* number of items used */
	void **list;             /* vector of items (void *) */
	list_destroy_t *destroy; /* item destructor, if any */
};

struct Lister
{
	List *list;    /* the list being iterated over */
	ssize_t index; /* the index of the current item */
};

/*

C<int grow(List *list, size_t items)>

Allocates enough memory to add C<item> extra items to C<list> if necessary.
Returns 0 on success, or -1 on failure.

*/

static int grow(List *list, size_t items)
{
	int grown = 0;

	while (list->length + items > list->size)
	{
		list->size += chunk;
		grown = 1;
	}

	if (grown)
		return mem_resize(list->list, list->size) ? 0 : -1;

	return 0;
}

/*

C<int shrink(List *list, size_t items)>

Allocates less memory for removing C<items> items from C<list> if necessary.
Returns 0 on success, or -1 on failure.

*/

static int shrink(List *list, size_t items)
{
	int shrunk = 0;

	while (list->length - items < list->size - chunk)
	{
		list->size -= chunk;
		shrunk = 1;
	}

	if (shrunk)
		return mem_resize(list->list, list->size) ? 0 : -1;

	return 0;
}

/*

C<int expand(List *list, ssize_t index, size_t range)>

Slides C<list>'s items, starting at C<index>, C<range> slots to the right to
make room for more. Returns 0 on success, or -1 on error.

*/

static int expand(List *list, ssize_t index, size_t range)
{
#if 0
	ssize_t i;
#endif

	if (grow(list, range) == -1)
		return -1;

	memmove(list->list + index + range, list->list + index, (list->length - index) * sizeof(*list->list));
#if 0
	for (i = list->length - 1; i >= index; --i)
		list->list[i + range] = list->list[i];
#endif

	list->length += range;

	return 0;
}

/*

C<int contract(List *list, ssize_t index, size_t range)>

Slides C<list>'s items, starting at C<index>, C<range> slots to the left to
close a gap. Returns 0 on success, or -1 on error.

*/

static int contract(List *list, ssize_t index, size_t range)
{
#if 0
	ssize_t i;
#endif

	memmove(list->list + index, list->list + index + range, (list->length - index) * sizeof(*list->list));
#if 0
	for (i = index; i < list->length - range; ++i)
		list->list[i] = list->list[i + range];
#endif

	if (shrink(list, range) == -1)
		return -1;

	list->length -= range;

	return 0;
}

/*

C<int adjust(List *list, ssize_t index, size_t range, size_t length)>

Expands or contracts C<list> as required so that I<list[index + range ..]>
occupies I<list[index + length ..]>. Returns 0 on success, or -1 on error.

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

Creates a I<List> with C<destroy> as its item destructor. Returns the the
new list on success, or C<NULL> on error.

=cut

*/

List *list_create(list_destroy_t *destroy)
{
	List *list;

	if (!(list = mem_create(1, List)))
		return NULL;

	list->size = list->length = 0;
	list->list = NULL;
	list->destroy = destroy;

	return list;
}

/*

=item C<List *list_make(list_destroy_t *destroy, ...)>

Creates a I<List> with C<destroy> as its item destructor and the remaining
arguments as its initial items. Returns the new list on success, or C<NULL>
on error.

=cut

*/

List *list_make(list_destroy_t *destroy, ...)
{
	List *list;
	void *item;
	va_list ap;

	if (!(list = list_create(destroy)))
		return NULL;

	va_start(ap, destroy);

	while ((item = va_arg(ap, void *)) != NULL)
		list_append(list, item);

	va_end(ap);

	return list;
}

/*

=item C<List *list_copy(List *src, list_copy_t *copy)>

Creates a clone of C<src> using C<copy> as the copy constructor (if not
C<NULL>). Returns the clone on success, or C<NULL> on error.

=cut

*/

List *list_copy(List *src, list_copy_t *copy)
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

	mem_release(list);
}

/*

=item C< #define list_destroy(list)>

Destroys (deallocate and set to C<NULL>) C<list>. Returns C<NULL>.

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

=item C<void *list_item(List *list, size_t index)>

Returns the C<index>'th item in C<list>, or C<NULL> on error.

=cut

*/

void *list_item(List *list, size_t index)
{
	if (!list)
		return NULL;

	if (index >= list->length)
		return NULL;

	return list->list[index];
}

/*

=item C<int list_item_int(List *list, size_t index)>

Returns the C<index>'th item in C<list> as an integer, or 0 on error.

=cut

*/

int list_item_int(List *list, size_t index)
{
	if (!list)
		return 0;

	if (index >= list->length)
		return 0;

	return (int)(list->list[index]);
}

/*

=item C<int list_empty(List *list)>

Returns whether or not C<list> is empty.

=cut

*/

int list_empty(List *list)
{
	return !list || !list->length;
}

/*

=item C<size_t list_length(List *list)>

Returns the length of C<list>.

=cut

*/

size_t list_length(List *list)
{
	return (list) ? list->length : 0;
}

/*

=item C<ssize_t list_last(List *list)>

Returns the index of the last item in C<list>, or -1 if there are no items.

=cut

*/

ssize_t list_last(List *list)
{
	return (list) ? list->length - 1 : -1;
}

/*

=item C<List *list_remove(List *list, size_t index)>

Removes the C<index>'th item from C<list>. Returns C<list> on success, or
C<NULL> on error.

=cut

*/

List *list_remove(List *list, size_t index)
{
	return list_remove_range(list, index, 1);
}

/*

=item C<List *list_remove_range(List *list, size_t index, size_t range)>

Removes C<range> items from C<list> starting at C<index>. Returns C<list> on
success, or C<NULL> on error.

=cut

*/

List *list_remove_range(List *list, size_t index, size_t range)
{
	if (!list || list->length < index + range)
		return NULL;

	killitems(list, index, range);
	contract(list, index, range);

	return list;
}

/*

=item C<List *list_insert(List *list, size_t index, void *item)>

Adds C<item> to C<list> at position C<index>. Returns C<list> on success, or
C<NULL> on error.

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

#define enlist(item, copy) ((copy) ? (copy)(item) : (item))

/*

=item C<List *list_insert_list(List *dst, size_t index, List *src, list_copy_t *copy)>

Inserts the items from C<src> into C<dst>, starting at position C<index>
using C<copy> as the copy constructor (if not C<NULL>). Returns C<dst> on
success, or C<NULL> on error.

=cut

*/

List *list_insert_list(List *dst, size_t index, List *src, list_copy_t *copy)
{
	size_t i;

	if (!src || !dst || dst->length < index || xor(dst->destroy, copy))
		return NULL;

	if (expand(dst, index, src->length) == -1)
		return NULL;

	for (i = 0; i < src->length; ++i)
		dst->list[index + i] = enlist(src->list[i], copy);

	return dst;
}

/*

=item C<List *list_append(List *list, void *item)>

Appends C<item> to C<list>. Returns C<list> on success, or C<NULL> on error.

=cut

*/

List *list_append(List *list, void *item)
{
	return list_insert(list, list_length(list), item);
}

/*

=item C<List *list_append_list(List *dst, List *src, list_copy_t *copy)>

Appends the items in C<src> to C<dst> using C<copy> as the copy constructor
(if not C<NULL>). Returns C<list> on success, or C<NULL> on error.

=cut

*/

List *list_append_list(List *dst, List *src, list_copy_t *copy)
{
	return list_insert_list(dst, list_length(dst), src, copy);
}

/*

=item C<List *list_prepend(List *list, void *item)>

Prepends C<item> to C<list>. Returns C<list> on success, or C<NULL> on
error.

=cut

*/

List *list_prepend(List *list, void *item)
{
	return list_insert(list, 0, item);
}

/*

=item C<List *list_prepend_list(List *dst, List *src, list_copy_t *copy)>

Prepends the items in C<src> to C<dst> using C<copy> as the copy constructor
(if not C<NULL>). Returns C<list> on success, or C<NULL> on error.

=cut

*/

List *list_prepend_list(List *dst, List *src, list_copy_t *copy)
{
	return list_insert_list(dst, 0, src, copy);
}

/*

=item C<List *list_replace(List *list, size_t index, size_t range, void *item)>

Replaces C<range> items in C<list>, starting at C<index>, with C<item>.
Returns C<list> on success, or C<NULL> on error.

=cut

*/

List *list_replace(List *list, size_t index, size_t range, void *item)
{
	if (!list || list->length < index + range)
		return NULL;

	killitems(list, index, range);
	adjust(list, index, range, 1);
	list->list[index] = item;

	return list;
}

/*

=item C<List *list_replace_list(List *dst, size_t index, size_t range, List *src, list_copy_t *copy)>

Replaces C<range> items in C<dst>, starting at C<index>, with the items in
C<src> using C<copy> as the copy constructor (if not C<NULL>). Returns
C<dst> on success, or C<NULL> on error.

=cut

*/

List *list_replace_list(List *dst, size_t index, size_t range, List *src, list_copy_t *copy)
{
	size_t length;

	if (!src || !dst || dst->length < index + range || xor(dst->destroy, copy))
		return NULL;

	killitems(dst, index, range);
	adjust(dst, index, range, length = list_length(src));

	while (length--)
		dst->list[index + length] = enlist(src->list[length], copy);

	return dst;
}

/*

=item C<List *list_extract(List *orig, size_t index, size_t range, list_copy_t *copy)>

Creates a new list consisting of C<range> items from C<orig>, starting at
C<index>, using C<copy> as the copy constructor (if not C<NULL>). Returns
the new list on success, or C<NULL> on error.

=cut

*/

List *list_extract(List *orig, size_t index, size_t range, list_copy_t *copy)
{
	List *list;

	if (!orig || orig->length < index + range || xor(orig->destroy, copy))
		return NULL;

	if (!(list = list_create(copy ? orig->destroy : NULL)))
		return NULL;

	while (range--)
		list_append(list, enlist(orig->list[index++], copy));

	return list;
}

#undef enlist

/*

=item C<List *list_sort(List *list, list_cmp_t *cmp)>

Sorts the items in C<list> using the item comparison function C<cmp> and
I<qsort(3)> for lists of fewer than 1000 items and I<hsort(3)> for larger
lists. Returns C<list> on success, or C<NULL> on error.

=cut

*/

List *list_sort(List *list, list_cmp_t *cmp)
{
	if (!list || !list->list || !list->length)
		return NULL;

	((list->length >= 1000) ? hsort : qsort)(list->list, list->length, sizeof list->list[0], cmp);

	return list;
}

/*

=item C<void list_apply(List *list, list_action_t *action)>

Invokes C<action> for each of C<list>'s items. The item and its position
within C<list> are passed to C<action> for each invocation.

=cut

*/

void list_apply(List *list, list_action_t *action)
{
	size_t index;

	if (!list || !action)
		return;

	for (index = 0; index < list->length; ++index)
		action(list->list[index], &index);
}

/*

=item C<ssize_t list_ask(List *list, ssize_t *index, list_query_t *query)>

Asks C<query> of each of C<list>'s items, starting at C<index>, until an
item satisfies C<query>. Returns the index of the item that satisfied query,
or -1 when query is not satisfied by any remaining items.

=cut

*/

ssize_t list_ask(List *list, ssize_t *index, list_query_t *query)
{
	size_t i;

	if (!list || !index || *index >= list->length || !query)
		return -1;

	for (i = *index; i < list->length; ++i)
		if (query(list->list[i], (size_t *)index))
			return *index = i;

	return *index = -1;
}

/*

=item C<Lister *lister_create(List *list)>

Creates an iterator for C<list>. Returns the iterator on success, or C<NULL>
on error.

=cut

*/

Lister *lister_create(List *list)
{
	Lister *lister;

	if (!list)
		return NULL;

	if (!(lister = mem_create(1, Lister)))
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

=item C<int list_has_next(Lister *lister)>

Returns whether or not there is another item in the list being iterated over
by C<lister>.

=cut

*/

int list_has_next(Lister *lister)
{
	return lister && lister->index + 1 < lister->list->length;
}

/*

=item C<void *list_next(Lister *lister)>

Returns the next item in the iteration C<lister>.

=cut

*/

void *list_next(Lister *lister)
{
	if (!lister)
		return NULL;

	return list_item(lister->list, (size_t)++lister->index);
}

/*

=item C<void lister_remove(Lister *lister)>

Removes the current item in the iteration C<lister>. The next item in the
iteration is the item following the removed item, if any.

=cut

*/

void lister_remove(Lister *lister)
{
	if (!lister || lister->index == -1)
		return;

	list_remove(lister->list, (size_t)lister->index--);
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
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>

=head1 AUTHOR

raf <raf2@zip.com.au>

=cut

*/

#ifdef TEST

#include <stdio.h>

#if 0
static void list_print(const char *name, List *list)
{
	int i;

	if (!list)
	{
		printf("%s = nil\n", name);
		return;
	}

	//printf("%s = { destroy = %p, size = %d, length = %d, list = { ", name, list->destroy, list->size, list->length);
	printf("%s = { ", name);

	for (i = 0; i < list_length(list); ++i)
	{
		if (i)
			printf(", ");

		//printf("\"%s\"(%p)", (char *)list_item(list, i), list_item(list, i));
		printf("\"%s\"", (char *)list_item(list, i));
	}

	printf(" }\n");
}
#endif

static char action_data[1024];

static void action(void *item, size_t *index)
{
	strcat(action_data, item);
}

static int query_data[] = { 2, 6, 8 , -1 };

static int query(void *item, size_t *index)
{
	return !strcmp((const char *)item, "def");
}

static int sort_cmp(const char **a, const char **b)
{
	return strcmp(*a, *b);
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
		list_append(c, mem_strdup("abc"));
		list_append(c, mem_strdup("def"));
		list_append(c, mem_strdup("ghi"));
		list_append(c, mem_strdup("jkl"));

		d = list_copy(c, NULL);
		if (d)
			++errors, printf("Test13: list_copy() with destroy() but no copy() failed\n");

		d = list_copy(c, (list_copy_t *)mem_strdup);
		if (!d)
			++errors, printf("Test14: list_copy() with copy() and destroy() failed\n");
	}

	if (!list_remove(a, 3))
		++errors, printf("Test15: list_remove() failed\n");
	else if (list_length(a) != 3)
		++errors, printf("Test15: list_remove() from end of list failed: has %d items (not 3)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test15: list_remove(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "def"))
		++errors, printf("Test15: list_remove(): 2nd item is '%s' not 'def'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "ghi"))
		++errors, printf("Test15: list_remove(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(a, 2));

	if (!list_remove(a, 0))
		++errors, printf("Test16: list_remove() failed\n");
	else if (list_length(a) != 2)
		++errors, printf("Test16: list_remove() from beginning of list failed: has %d items (not 2)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "def"))
		++errors, printf("Test16: list_remove(): 1st item is '%s' not 'def'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "ghi"))
		++errors, printf("Test16: list_remove(): 2nd item is '%s' not 'ghi'\n", (char *)list_item(a, 1));

	if (!list_replace(a, 1, 1, "123"))
		++errors, printf("Test17: list_replace() failed\n");
	else if (list_length(a) != 2)
		++errors, printf("Test17: list_replace() failed: has %d items (not 2)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "def"))
		++errors, printf("Test17: list_replace(): 1st item is '%s' not 'def'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "123"))
		++errors, printf("Test17: list_replace(): 2nd item is '%s' not '123'\n", (char *)list_item(a, 1));

	if (!list_append_list(a, b, NULL))
		++errors, printf("Test18: list_append_list() failed\n");
	else if (list_length(a) != 5)
		++errors, printf("Test18: list_append_list() failed: has %d items (not 5)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "def"))
		++errors, printf("Test18: list_append_list(): 1st item is '%s' not 'def'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "123"))
		++errors, printf("Test18: list_append_list(): 2nd item is '%s' not '123'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "def"))
		++errors, printf("Test18: list_append_list(): 3rd item is '%s' not 'def'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "ghi"))
		++errors, printf("Test18: list_append_list(): 4th item is '%s' not 'ghi'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "abc"))
		++errors, printf("Test18: list_append_list(): 5th item is '%s' not 'abc'\n", (char *)list_item(a, 4));

	if (!list_prepend_list(a, c, NULL))
		++errors, printf("Test19: list_prepend_list() failed\n");
	else if (list_length(a) != 9)
		++errors, printf("Test19: list_prepend_list() failed: has %d items (not 9)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test19: list_prepend_list(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "def"))
		++errors, printf("Test19: list_prepend_list(): 2nd item is '%s' not 'def'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "ghi"))
		++errors, printf("Test19: list_prepend_list(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "jkl"))
		++errors, printf("Test19: list_prepend_list(): 4th item is '%s' not 'jkl'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test19: list_prepend_list(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "123"))
		++errors, printf("Test19: list_prepend_list(): 6th item is '%s' not '123'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "def"))
		++errors, printf("Test19: list_prepend_list(): 7th item is '%s' not 'def'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test19: list_prepend_list(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "abc"))
		++errors, printf("Test19: list_prepend_list(): 9th item is '%s' not 'abc'\n", (char *)list_item(a, 8));

	if (!list_insert_list(b, 1, c, NULL))
		++errors, printf("Test20: list_insert_list() failed\n");
	else if (list_length(b) != 7)
		++errors, printf("Test20: list_insert_list() failed: has %d items (not 7)\n", list_length(b));
	else if (!list_item(b, 0) || strcmp(list_item(b, 0), "def"))
		++errors, printf("Test20: list_insert_list(): 1st item is '%s' not 'def'\n", (char *)list_item(b, 0));
	else if (!list_item(b, 1) || strcmp(list_item(b, 1), "abc"))
		++errors, printf("Test20: list_insert_list(): 2nd item is '%s' not 'abc'\n", (char *)list_item(b, 1));
	else if (!list_item(b, 2) || strcmp(list_item(b, 2), "def"))
		++errors, printf("Test20: list_insert_list(): 3rd item is '%s' not 'def'\n", (char *)list_item(b, 2));
	else if (!list_item(b, 3) || strcmp(list_item(b, 3), "ghi"))
		++errors, printf("Test20: list_insert_list(): 4th item is '%s' not 'ghi'\n", (char *)list_item(b, 3));
	else if (!list_item(b, 4) || strcmp(list_item(b, 4), "jkl"))
		++errors, printf("Test20: list_insert_list(): 5th item is '%s' not 'jkl'\n", (char *)list_item(b, 4));
	else if (!list_item(b, 5) || strcmp(list_item(b, 5), "ghi"))
		++errors, printf("Test20: list_insert_list(): 6th item is '%s' not 'ghi'\n", (char *)list_item(b, 5));
	else if (!list_item(b, 6) || strcmp(list_item(b, 6), "abc"))
		++errors, printf("Test20: list_insert_list(): 7th item is '%s' not 'abc'\n", (char *)list_item(b, 6));

	if (list_replace_list(c, 1, 2, d, NULL))
		++errors, printf("Test21: list_replace_list() with destroy() but not copy() failed\n");

	if (list_replace_list(a, 1, 2, d, (list_copy_t *)mem_strdup))
		++errors, printf("Test22: list_replace_list() with copy() but not destroy() failed\n");

	if (!list_replace_list(a, 1, 2, d, NULL))
		++errors, printf("Test23: list_replace_list() failed\n");
	else if (list_length(a) != 11)
		++errors, printf("Test23: list_insert_list() failed: has %d items (not 11)\n", list_length(a));
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "abc"))
		++errors, printf("Test23: list_insert_list(): 1st item is '%s' not 'abc'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test23: list_insert_list(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "def"))
		++errors, printf("Test23: list_insert_list(): 3rd item is '%s' not 'def'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "ghi"))
		++errors, printf("Test23: list_insert_list(): 4th item is '%s' not 'ghi'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "jkl"))
		++errors, printf("Test23: list_insert_list(): 5th item is '%s' not 'jkl'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "jkl"))
		++errors, printf("Test23: list_insert_list(): 6th item is '%s' not 'jkl'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "def"))
		++errors, printf("Test23: list_insert_list(): 7th item is '%s' not 'def'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "123"))
		++errors, printf("Test23: list_insert_list(): 8th item is '%s' not '123'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "def"))
		++errors, printf("Test23: list_insert_list(): 9th item is '%s' not 'def'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "ghi"))
		++errors, printf("Test23: list_insert_list(): 10th item is '%s' not 'ghi'\n", (char *)list_item(a, 9));
	else if (!list_item(a, 10) || strcmp(list_item(a, 10), "abc"))
		++errors, printf("Test23: list_insert_list(): 11th item is '%s' not 'abc'\n", (char *)list_item(a, 10));

	if (!list_remove_range(b, 1, 3))
		++errors, printf("Test24: list_remove_range() failed\n");
	else if (list_length(b) != 4)
		++errors, printf("Test24: list_insert_list() failed: has %d items (not 4)\n", list_length(b));
	else if (!list_item(b, 0) || strcmp(list_item(b, 0), "def"))
		++errors, printf("Test24: list_insert_list(): 1st item is '%s' not 'def'\n", (char *)list_item(b, 0));
	else if (!list_item(b, 1) || strcmp(list_item(b, 1), "jkl"))
		++errors, printf("Test24: list_insert_list(): 2nd item is '%s' not 'jkl'\n", (char *)list_item(b, 1));
	else if (!list_item(b, 2) || strcmp(list_item(b, 2), "ghi"))
		++errors, printf("Test24: list_insert_list(): 3rd item is '%s' not 'ghi'\n", (char *)list_item(b, 2));
	else if (!list_item(b, 3) || strcmp(list_item(b, 3), "abc"))
		++errors, printf("Test24: list_insert_list(): 4th item is '%s' not 'abc'\n", (char *)list_item(b, 3));

	list_apply(a, (list_action_t *)action);
	if (strcmp(action_data, "abcabcdefghijkljkldef123defghiabc"))
		++errors, printf("Test25: list_apply() failed\n");

	for (i = 0; list_ask(a, &index, (list_query_t *)query) != -1; ++i, ++index)
	{
		if (index != query_data[i])
		{
			++errors, printf("Test26: list_ask returned %d (not %d)\n", index, query_data[i]);
			break;
		}
	}

	if (!list_sort(a, (list_cmp_t *)sort_cmp))
		++errors, printf("Test27: list_sort() failed\n");
	else if (!list_item(a, 0) || strcmp(list_item(a, 0), "123"))
		++errors, printf("Test27: list_sort(): 1st item is '%s' not '123'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test27: list_sort(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "abc"))
		++errors, printf("Test27: list_sort(): 3rd item is '%s' not 'abc'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "abc"))
		++errors, printf("Test27: list_sort(): 4th item is '%s' not 'abc'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test27: list_sort(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "def"))
		++errors, printf("Test27: list_sort(): 6th item is '%s' not 'def'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "def"))
		++errors, printf("Test27: list_sort(): 7th item is '%s' not 'def'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test27: list_sort(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "ghi"))
		++errors, printf("Test27: list_sort(): 9th item is '%s' not 'ghi'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "jkl"))
		++errors, printf("Test27: list_sort(): 10th item is '%s' not 'jkl'\n", (char *)list_item(a, 9));
	else if (!list_item(a, 10) || strcmp(list_item(a, 10), "jkl"))
		++errors, printf("Test27: list_sort(): 11th item is '%s' not 'jkl'\n", (char *)list_item(a, 10));

	if (!(lister = lister_create(a)))
		++errors, printf("Test28: lister_create() failed\n");

	for (i = 0; list_has_next(lister); ++i)
	{
		void *item = list_next(lister);

		if (item != list_item(a, i))
		{
			++errors, printf("Test29: iteration %d is '%s' not '%s'\n", i, (char *)item, (char *)list_item(a, i));
			break;
		}
	}

	lister_destroy(lister);
	if (lister)
		++errors, printf("Test30: lister_destroy(lister) failed, lister is %p not null\n", lister);

	if (!(lister = lister_create(a)))
		++errors, printf("Test31: lister_create() failed\n");

	for (i = 0; list_has_next(lister); ++i)
	{
		list_next(lister);

		if (i == 4)
			lister_remove(lister);
	}

	if (!list_item(a, 0) || strcmp(list_item(a, 0), "123"))
		++errors, printf("Test32: list_sort(): 1st item is '%s' not '123'\n", (char *)list_item(a, 0));
	else if (!list_item(a, 1) || strcmp(list_item(a, 1), "abc"))
		++errors, printf("Test32: list_sort(): 2nd item is '%s' not 'abc'\n", (char *)list_item(a, 1));
	else if (!list_item(a, 2) || strcmp(list_item(a, 2), "abc"))
		++errors, printf("Test32: list_sort(): 3rd item is '%s' not 'abc'\n", (char *)list_item(a, 2));
	else if (!list_item(a, 3) || strcmp(list_item(a, 3), "abc"))
		++errors, printf("Test32: list_sort(): 4th item is '%s' not 'abc'\n", (char *)list_item(a, 3));
	else if (!list_item(a, 4) || strcmp(list_item(a, 4), "def"))
		++errors, printf("Test32: list_sort(): 5th item is '%s' not 'def'\n", (char *)list_item(a, 4));
	else if (!list_item(a, 5) || strcmp(list_item(a, 5), "def"))
		++errors, printf("Test32: list_sort(): 6th item is '%s' not 'def'\n", (char *)list_item(a, 5));
	else if (!list_item(a, 6) || strcmp(list_item(a, 6), "ghi"))
		++errors, printf("Test32: list_sort(): 7th item is '%s' not 'ghi'\n", (char *)list_item(a, 6));
	else if (!list_item(a, 7) || strcmp(list_item(a, 7), "ghi"))
		++errors, printf("Test32: list_sort(): 8th item is '%s' not 'ghi'\n", (char *)list_item(a, 7));
	else if (!list_item(a, 8) || strcmp(list_item(a, 8), "jkl"))
		++errors, printf("Test32: list_sort(): 9th item is '%s' not 'jkl'\n", (char *)list_item(a, 8));
	else if (!list_item(a, 9) || strcmp(list_item(a, 9), "jkl"))
		++errors, printf("Test32: list_sort(): 10th item is '%s' not 'jkl'\n", (char *)list_item(a, 9));

	lister_destroy(lister);
	if (lister)
		++errors, printf("Test33: lister_destroy(lister) failed, lister is %p not null\n", lister);

	list_destroy(a);
	if (lister)
		++errors, printf("Test34: list_destroy(a) failed, a is %p not null\n", a);

	list_destroy(b);
	if (lister)
		++errors, printf("Test35: list_destroy(b) failed, b is %p not null\n", b);

	list_destroy(c);
	if (lister)
		++errors, printf("Test36: list_destroy(c) failed, c is %p not null\n", c);

	list_destroy(d);
	if (lister)
		++errors, printf("Test37: list_destroy(d) failed, d is %p not null\n", d);

	if (errors)
		printf("%d/37 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
