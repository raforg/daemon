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

I<libslack(map)> - map module

=head1 SYNOPSIS

    #include <slack/map.h>

    typedef struct Map Map;
    typedef struct Mapper Mapper;
    typedef struct Mapping Mapping;
    typedef list_destroy_t map_destroy_t;
    typedef list_copy_t map_copy_t;
    typedef list_cmp_t map_cmp_t;
    typedef size_t map_hash_t(size_t table_size, const void *key);
    typedef void map_action_t(void *key, void *item, void *data);

    Map *map_create(map_destroy_t *destroy);
    Map *map_create_sized(size_t size, map_destroy_t *destroy);
    Map *map_create_with_hash(map_hash_t *hash, map_destroy_t *destroy);
    Map *map_create_with_hash_sized(size_t size, map_hash_t *hash, map_destroy_t *destroy);
    Map *map_create_generic(map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy);
    Map *map_create_generic_sized(size_t size, map_copy *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy);
    void map_release(Map *map);
    #define map_destroy(map)
    void *map_destroy_func(Map **map);
    int map_own(Map *map, map_destroy_t *destroy);
    map_destroy_t *map_disown(Map *map);
    int map_add(Map *map, const void *key, void *value);
    int map_put(Map *map, const void *key, void *value);
    int map_insert(Map *map, const void *key, void *value, int replace);
    int map_remove(Map *map, const void *key);
    void *map_get(const Map *map, const void *key);
    Mapper *mapper_create(Map *map);
    void mapper_release(Mapper *mapper);
    #define mapper_destroy(mapper)
    void *mapper_destroy_func(Mapper **mapper);
    int mapper_has_next(Mapper *mapper);
    void *mapper_next(Mapper *mapper);
    const Mapping *mapper_next_mapping(Mapper *mapper);
    void mapper_remove(Mapper *mapper);
    int map_has_next(Map *map);
    void *map_next(Map *map);
    const Mapping *map_next_mapping(Map *map);
    void map_remove_current(Map *map);
    const void *mapping_key(const Mapping *mapping);
    const void *mapping_value(const Mapping *mapping);
    List *map_keys(Map *map);
    List *map_values(Map *map);
    void map_apply(Map *, map_action_t *action, void *data);
    ssize_t map_size(const Map *map);

=head1 DESCRIPTION

This module provides functions for manipulating and iterating over a set of
mappings from strings to homogeneous data (or heterogeneous data if it's
polymorphic), also known as associative arrays. I<Maps> may own their items.
I<Maps> created with a non-C<NULL> destroy function use that function to
destroy an item when it is removed from the map and to destroy each item
when the map itself it destroyed. I<Maps> are hash tables with 11 buckets
by default. They grow when necessary, approximately doubling in size each
time up to a maximum size of 26,214,401 buckets.

=over 4

=cut

*/

#include "std.h"

#include "map.h"
#include "mem.h"
#include "err.h"

struct Map
{
	size_t size;                  /* number of buckets */
	size_t items;                 /* number of items */
	List **chain;                 /* array of hash buckets */
	map_hash_t *hash;             /* hash function */
	map_copy_t *copy;             /* key copy function */
	map_cmp_t *cmp;               /* key comparison function */
	map_destroy_t *key_destroy;   /* destructor function for keys */
	map_destroy_t *value_destroy; /* destructor function for items */
	Mapper *mapper;               /* built-in iterator */
};

struct Mapping
{
	void *key;                    /* a map key */
	void *value;                  /* a map value */
	map_destroy_t *key_destroy;   /* destructor function for key */
	map_destroy_t *value_destroy; /* destructor function for value */
};

struct Mapper
{
	Map *map;                 /* the map being iterated over */
	ssize_t chain_index;      /* the index of the chain of the current item */
	ssize_t item_index;       /* the index of the current item */
	ssize_t next_chain_index; /* the index of the chain of the next item */
	ssize_t next_item_index;  /* the index of the next item */
};

/* Increasing sequence of valid (i.e. prime) table sizes to choose from. */

static const size_t table_sizes[] =
{
	11, 23, 47, 101, 199, 401, 797, 1601, 3203, 6397, 12799, 25601,
	51199, 102397, 204803, 409597, 819187, 1638431, 3276799, 6553621,
	13107197, 26214401
};

static const size_t num_table_sizes = sizeof(table_sizes) / sizeof(table_sizes[0]);

/* Average bucket length threshold that must be reached before a map grows */

static const double table_resize_factor = 2.0;

#if 0
/*

C<size_t hash(size_t size, const void *key)>

JPW hash function. Returns a hash value (in the range 0..size-1) for C<key>.

*/

static size_t hash(size_t size, const void *key)
{
	unsigned char *k = key;
	size_t g, h = 0;

	while (*k)
		if ((g = (h <<= 4, h += *k++) & 0xf0000000))
			h ^= (g >> 24) ^ g;

	return h % size;
}
#endif

/*

C<size_t hash(size_t size, const void *key)>

Hash function from The Practice of Programming by Kernighan and Pike (p57).
Returns a hash value (in the range 0..size-1) for C<key>.

*/

static size_t hash(size_t size, const void *key)
{
	const unsigned char *k = key;
	size_t h = 0;
	const int multiplier = 31;
	/* const int multiplier = 37; */

	while (*k)
		h *= multiplier, h += *k++;

	return h % size;
}

/*

C<Mapping *mapping_create(const void *key, void *value, map_destroy_t *destroy)>

Creates a new mapping from C<key> to C<value>. C<destroy> is the destructor
function for C<value>. On success, returns the new mapping. On error, returns
C<NULL>.

*/

static Mapping *mapping_create(void *key, void *value, map_destroy_t *key_destroy, map_destroy_t *value_destroy)
{
	Mapping *mapping;

	if (!(mapping = mem_new(Mapping)))
		return NULL;

	mapping->key = key;
	mapping->value = value;
	mapping->key_destroy = key_destroy;
	mapping->value_destroy = value_destroy;

	return mapping;
}

/*

C<void mapping_release(Mapping *mapping)>

Releases (deallocates) C<mapping>, destroying its value if necessary.

*/

static void mapping_release(Mapping *mapping)
{
	if (!mapping)
		return;

	if (mapping->key_destroy)
		mapping->key_destroy(mapping->key);

	if (mapping->value_destroy)
		mapping->value_destroy(mapping->value);

	mem_release(mapping);
}

/*

=item C<Map *map_create(map_destroy_t *destroy)>

Creates a I<Map> with strings keys, 11 buckets and C<destroy> as its item
destructor. On success, returns the new map. On error, returns C<NULL>.

=cut

*/

Map *map_create(map_destroy_t *destroy)
{
	return map_create_with_hash_sized(table_sizes[0], (map_hash_t *)hash, destroy);
}

/*

=item C<Map *map_create_sized(size_t size, map_destroy_t *destroy)>

Creates a I<Map> with string keys, approximately C<size> buckets and
C<destroy> as its item destructor. The actual size will be the first prime
greater than or equal to C<size> in a prebuilt sequence of primes between 11
and 26,214,401 that double at each step. On success, returns the new map. On
error, returns C<NULL>.

=cut

*/

Map *map_create_sized(size_t size, map_destroy_t *destroy)
{
	return map_create_with_hash_sized(size, (map_hash_t *)hash, destroy);
}

/*

=item C<Map *map_create_with_hash(map_hash_t *hash, map_destroy_t *destroy)>

Creates a I<Map> with strings keys, 11 buckets, C<hash> as the hash function
and C<destroy> as its item destructor. On success, returns the new map. On
error, returns C<NULL>. The arguments to C<hash> are a C<size_t> specifying
the number of buckets and a C<const void *> specifying the key to hash. It
returns a C<size_t> between zero and the table size - 1.

=cut

*/

Map *map_create_with_hash(map_hash_t *hash, map_destroy_t *destroy)
{
	return map_create_with_hash_sized(table_sizes[0], hash, destroy);
}

/*

=item C<Map *map_create_with_hash_sized(size_t size, map_hash_t *hash, map_destroy_t *destroy)>

Creates a I<Map> with string keys, approximately C<size> buckets, C<hash> as
its hash function and C<destroy> as its item destructor. The actual size
will be the first prime greater than or equal to C<size> in a built in
sequence of primes between 11 and 26,214,401 that double at each step. On
success, returns the new map. On error, returns C<NULL>. The arguments to
C<hash> are a C<size_t> specifying the number of buckets and a C<const void
*> specifying the key to hash. It must return a C<size_t> between zero and
the table size - 1.

=cut

*/

Map *map_create_with_hash_sized(size_t size, map_hash_t *hash, map_destroy_t *destroy)
{
	return map_create_generic_sized(size, (map_copy_t *)mem_strdup, (map_cmp_t *)strcmp, hash, (map_destroy_t *)free, destroy);
}

/*

=item C<Map *map_create_generic(map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy;)>

Creates a I<Map> with arbitrary keys, 11 buckets, C<copy> as its key copy
function, C<cmp> as its key comparison function, C<hash> as its hash
function, C<key_destroy> as its key destructor and C<value_destroy> as its
item destructor. On success, returns the new map. On error, returns C<NULL>.
The argument to C<copy> is the key to be copied. It returns the copy. The
arguments to C<cmp> are two keys to be compared. It returns < 0 if the first
compares less than the second, 0 if they compare equal and > 0 if the first
compares greater than the second. The arguments to C<hash> are a C<size_t>
specifying the number of buckets and a C<const void *> specifying the key to
hash. It must return a C<size_t> between zero and the table size - 1.

=cut

*/

Map *map_create_generic(map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy)
{
	return map_create_generic_sized(table_sizes[0], copy, cmp, hash, key_destroy, value_destroy);
}

/*

=item C<Map *map_create_generic(map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy)>

Creates a I<Map> with arbitrary keys, approximately C<size> buckets, C<copy>
as its key copy function, C<cmp> as its key comparison function, C<hash> as
its hash function, C<key_destroy> as its key destrouctor and
C<value_destroy> as its item destructor. The actual size will be the first
prime greater than or equal to C<size> in a built in sequence of primes
between 11 and 26,214,401 that double at each step. On success, returns the
new map. On error, returns C<NULL>. The argument to C<copy> is the key to be
copied. It returns the copy. The arguments to C<cmp> are two keys to be
compared. It returns < 0 if the first compares less than the second, 0 if
they compare equal and > 0 if the first compares greater than the second.
The arguments to C<hash> are a C<size_t> specifying the number of buckets
and a C<const void *> specifying the key to hash. It must return a C<size_t>
between zero and the table size - 1.

=cut

*/

Map *map_create_generic_sized(size_t size, map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy)
{
	Map *map;
	size_t i;

	for (i = 0; i < num_table_sizes; ++i)
	{
		if (table_sizes[i] >= size)
		{
			size = table_sizes[i];
			break;
		}
	}

	if (i == num_table_sizes)
		return NULL;

	if (!(map = mem_new(Map)))
		return NULL;

	if (!(map->chain = mem_create(size, List *)))
	{
		mem_release(map);
		return NULL;
	}

	map->size = size;
	map->items = 0;
	memset(map->chain, '\0', map->size * sizeof(List *));
	map->hash = hash;
	map->copy = copy;
	map->cmp = cmp;
	map->key_destroy = key_destroy;
	map->value_destroy = value_destroy;
	map->mapper = NULL;

	return map;
}

/*

=item C<void map_release(Map *map)>

Releases (deallocates) C<map>, destroying its items if necessary.

=cut

*/

void map_release(Map *map)
{
	size_t i;

	if (!map)
		return;

	for (i = 0; i < map->size; ++i)
		list_release(map->chain[i]);

	mem_release(map->chain);
	mapper_release(map->mapper);
	mem_release(map);
}

/*

=item C< #define map_destroy(map)>

Destroys (deallocates and sets to C<NULL>) C<map>. Returns C<NULL>.

=item C<void *map_destroy_func(Map **map)>

Destroys (deallocates and sets to C<NULL>) C<map>. Returns C<NULL>. This
function is exposed as an implementation side effect. Don't call it
directly. Call I<map_destroy()> instead.

=cut

*/

void *map_destroy_func(Map **map)
{
	if (map && *map)
	{
		map_release(*map);
		*map = NULL;
	}

	return NULL;
}

/*

=item C<int map_own(Map *map, map_destroy_t *key_destroy, map_destroy_t *value_destroy)>

Causes C<map> to take ownership of its items. The keys will be destroyed
using C<key_destroy>. The items will be destroyed using C<value_destroy>
when their mappings are removed from C<map> or when C<map> is destroyed. On
success, returns 0. On error, returns -1.

=cut

*/

int map_own(Map *map, map_destroy_t *destroy)
{
	size_t c, i, length;

	if (!map || !destroy)
		return -1;

	if (destroy == map->value_destroy)
		return 0;

	map->value_destroy = destroy;

	for (c = 0; c < map->size; ++c)
	{
		List *chain = map->chain[c];

		if (!chain)
			continue;

		length = list_length(chain);

		for (i = 0; i < length; ++i)
		{
			Mapping *mapping = (Mapping *)list_item(chain, i);
			mapping->value_destroy = destroy;
		}
	}

	return 0;
}

/*

=item C<map_destroy_t *map_disown(Map *map)>

Causes C<map> to relinquish ownership of its items. The items will not be
destroyed when their mappings are removed from C<map> or when C<map> is
destroyed. On success, returns the previous destroy function, if any.
On error, returns C<NULL>.

=cut

*/

map_destroy_t *map_disown(Map *map)
{
	size_t c, i, length;
	map_destroy_t *destroy;

	if (!map || !map->value_destroy)
		return NULL;

	destroy = map->value_destroy;
	map->value_destroy = NULL;

	for (c = 0; c < map->size; ++c)
	{
		List *chain = map->chain[c];

		if (!chain)
			continue;

		length = list_length(chain);

		for (i = 0; i < length; ++i)
		{
			Mapping *mapping = (Mapping *)list_item(chain, i);
			mapping->value_destroy = NULL;
		}
	}

	return destroy;
}

/*

C<static int map_resize(Map *map)>

Resizes C<map> to use the next prime in a built in sequence of primes between
11 and 26,214,401 that is greater than the current size.

*/

static int map_resize(Map *map)
{
	size_t size = 0;
	size_t i;
	Mapper *mapper;
	Map *new_map;

	if (!map)
		return -1;

	for (i = 1; i < num_table_sizes; ++i)
		if (table_sizes[i] > map->size)
		{
			size = table_sizes[i];
			break;
		}

	if (i == num_table_sizes || size == 0)
		return -1;

	if (!(mapper = mapper_create(map)))
		return -1;

	if (!(new_map = map_create_generic_sized(size, map->copy, map->cmp, map->hash, map->key_destroy, NULL)))
	{
		mapper_release(mapper);
		return -1;
	}

	while (mapper_has_next(mapper))
	{
		const Mapping *mapping = mapper_next_mapping(mapper);

		if (map_add(new_map, mapping->key, mapping->value) == -1)
		{
			mapper_destroy(mapper);
			map_destroy(new_map);
			return -1;
		}
	}

	mapper_destroy(mapper);
	if (map->value_destroy)
		map_own(new_map, map_disown(map));

	for (i = 0; i < map->size; ++i)
		list_release(map->chain[i]);
	mem_release(map->chain);

	*map = *new_map;
	mem_release(new_map);

	return 0;
}

/*

=item C<item map_add(Map *map, const void *key, void *value)>

Adds the C<(key, value)> mapping to C<map> if C<key> is not already present.
On success, returns 0. On error, returns -1.

=cut

*/

int map_add(Map *map, const void *key, void *value)
{
	return map_insert(map, key, value, 0);
}

/*

=item C<item map_put(Map *map, const void *key, void *value)>

Adds the C<(key, value)> mapping to C<map>, replacing any existing
C<(key, value)> mapping. On success, returns 0. On error, returns -1.

=cut

*/

int map_put(Map *map, const void *key, void *value)
{
	return map_insert(map, key, value, 1);
}

/*

=item C<int map_insert(Map *map, const void *key, void *value, int replace)>

Adds the C<(key, value)> mapping to C<map>, replacing any existing C<(key,
value)> mapping if C<replace> is non-zero. On success, returns 0. on error,
or if C<key> is already present and C<replace> is zero, returns -1.

=cut

*/

int map_insert(Map *map, const void *key, void *value, int replace)
{
	Mapping *mapping;
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return -1;

	if ((double)map->items / (double)map->size >= table_resize_factor)
		map_resize(map);

	h = map->hash(map->size, key);

	if (!map->chain[h])
		map->chain[h] = list_create((map_destroy_t *)mapping_release);

	if (!map->chain[h])
		return -1;

	chain = map->chain[h];
	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		mapping = (Mapping *)list_item(chain, c);

		if (!map->cmp(mapping->key, key))
		{
			if (!replace)
				return -1;

			list_remove(chain, c);
			break;
		}
	}

	if (!(mapping = mapping_create(map->copy(key), value, map->key_destroy, map->value_destroy)))
		return -1;

	if (!list_append(chain, mapping))
	{
		mapping_release(mapping);
		return -1;
	}

	++map->items;
	return 0;
}

/*

=item C<int map_remove(Map *map, const void *key)>

Removes C<(key, value)> mapping from C<map> if it is present. If C<map> was
created with a destroy function, then the value will be destroyed. On
success, returns 0. On error, returns -1.

=cut

*/

int map_remove(Map *map, const void *key)
{
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return -1;

	h = map->hash(map->size, key);
	chain = map->chain[h];

	if (!chain)
		return -1;

	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		Mapping *mapping = (Mapping *)list_item(chain, c);

		if (!map->cmp(mapping->key, key))
		{
			if (!list_remove(chain, c))
				return -1;

			--map->items;
			return 0;
		}
	}

	return -1;
}

/*

=item C<void *map_get(const Map *map, const void *key)>

Returns the value associated with C<key> in C<map>, or C<NULL> if there is
none.

=cut

*/

void *map_get(const Map *map, const void *key)
{
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return NULL;

	h = map->hash(map->size, key);
	chain = map->chain[h];

	if (!chain)
		return NULL;

	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		Mapping *mapping = (Mapping *)list_item(chain, c);

		if (!map->cmp(mapping->key, key))
			return mapping->value;
	}

	return NULL;
}

/*

=item C<Mapper *mapper_create(Map *map)>

Creates an iterator for C<map>. On success, returns the iterator. On error,
returns C<NULL>.

=cut

*/

Mapper *mapper_create(Map *map)
{
	Mapper *mapper;

	if (!map)
		return NULL;

	if (!(mapper = mem_new(Mapper)))
		return NULL;

	mapper->map = map;
	mapper->chain_index = -1;
	mapper->item_index = -1;
	mapper->next_chain_index = -1;
	mapper->next_item_index = -1;

	return mapper;
}

/*

=item C<void mapper_release(Mapper *mapper)>

Releases (deallocates) C<mapper>.

=cut

*/

void mapper_release(Mapper *mapper)
{
	mem_release(mapper);
}

/*

=item C< #define mapper_destroy(mapper)>

Destroys (deallocates and sets to C<NULL>) C<mapper>.

=item C<void *mapper_destroy_func(Mapper **mapper)>

Destroys (deallocates and sets to C<NULL>) C<mapper>. Returns C<NULL>. This
function is exposed as an implementation side effect. Don't call it
directly. Call I<mapper_destroy()> instead.

=cut

*/

void *mapper_destroy_func(Mapper **mapper)
{
	if (mapper && *mapper)
	{
		mapper_release(*mapper);
		*mapper = NULL;
	}

	return NULL;
}

/*

=item C<int mapper_has_next(Mapper *mapper)>

Returns whether or not there is another item in the map that C<mapper> is
iterating over.

=cut

*/

int mapper_has_next(Mapper *mapper)
{
	List *chain;

	if (!mapper)
		return 0;

	/* Find the current/first chain */

	mapper->next_chain_index = mapper->chain_index;
	mapper->next_item_index = mapper->item_index;

	if (mapper->next_chain_index == -1)
		++mapper->next_chain_index;

	while (mapper->next_chain_index < mapper->map->size && !mapper->map->chain[mapper->next_chain_index])
		++mapper->next_chain_index;

	if (mapper->next_chain_index == mapper->map->size)
		return 0;

	chain = mapper->map->chain[mapper->next_chain_index];

	/* Find the next item */

	if (++mapper->next_item_index < list_length(chain))
		return 1;

	do
	{
		++mapper->next_chain_index;

		while (mapper->next_chain_index < mapper->map->size && !mapper->map->chain[mapper->next_chain_index])
			++mapper->next_chain_index;

		if (mapper->next_chain_index == mapper->map->size)
			return 0;

		chain = mapper->map->chain[mapper->next_chain_index];
	}
	while (!list_length(chain));

	mapper->next_item_index = 0;

	return 1;
}

/*

=item C<void *mapper_next(Mapper *Mapper)>

Returns the next item in the map that C<mapper> is iterating over.

=cut

*/

void *mapper_next(Mapper *mapper)
{
	if (!mapper)
		return NULL;

	return mapper_next_mapping(mapper)->value;
}

/*

=item C<const Mapping *mapper_next_mapping(Mapper *Mapper)>

Returns the next mapping (key, value pair) in the map over which C<mapper>
is iterating.

=cut

*/

const Mapping *mapper_next_mapping(Mapper *mapper)
{
	mapper->chain_index = mapper->next_chain_index;
	mapper->item_index = mapper->next_item_index;

	return (Mapping *)list_item(mapper->map->chain[mapper->chain_index], mapper->item_index);
}

/*

=item C<void mapper_remove(Mapper *mapper)>

Removes the current item in the iteration C<mapper>. The next item in the
iteration is the item following the removed item, if any. This must be
called after I<mapper_next()>.

=cut

*/

void mapper_remove(Mapper *mapper)
{
	if (!mapper)
		return;

	list_remove(mapper->map->chain[mapper->chain_index], (size_t)mapper->item_index--);
	--mapper->map->items;
}

/*

=item C<int map_has_next(Map *map)>

Returns whether or not there is another item in C<map>. The first time this
is called, a new, internal I<Mapper> will be created (Note: there can be
only one). When there are no more items, returns zero and destroys the
internal iterator. When it returns a non-zero value, use I<map_next()> to
retrieve the next item.

=cut

*/

int map_has_next(Map *map)
{
	int ret;

	if (!map)
		return 0;

	if (!map->mapper)
		map->mapper = mapper_create(map);

	ret = mapper_has_next(map->mapper);
	if (!ret)
		mapper_destroy(map->mapper);

	return ret;
}

/*

=item C<void *map_next(Map *map)>

Returns the next item in C<map> using it's internal iterator.

=cut

*/

void *map_next(Map *map)
{
	if (!map || !map->mapper)
		return NULL;

	return mapper_next(map->mapper);
}

/*

=item C<const Mapping *map_next_mapping(Map *map)>

Returns the next mapping (key, value pair) in C<map> using it's internal
iterator.

=cut

*/

const Mapping *map_next_mapping(Map *map)
{
	if (!map || !map->mapper)
		return NULL;

	return mapper_next_mapping(map->mapper);
}

/*

=item C<void map_remove_current(Map *map)>

Removes the current item in C<map> using it's internal iterator. The next
item in the iteration is the item following the removed item, if any. This
must be called after I<map_next()>.

=cut

*/

void map_remove_current(Map *map)
{
	mapper_remove(map->mapper);
}

/*

=item C<const void *mapping_key(const Mapping *mapping)>

Returns the key in C<mapping>. On error, returns C<NULL>.

=cut

*/

const void *mapping_key(const Mapping *mapping)
{
	if (!mapping)
		return NULL;

	return mapping->key;
}

/*

=item C<const void *mapping_value(const Mapping *mapping)>

Returns the value in C<mapping>. On error, returns C<NULL>.

=cut

*/

const void *mapping_value(const Mapping *mapping)
{
	if (!mapping)
		return NULL;

	return mapping->value;
}

/*

=item C<List *map_keys(Map *map)>

Creates and returns a list of all of the keys contained in C<map>. The
caller is required to destroy the list. Also, the keys in the list are owned
by C<map> so the list returned must not outlive the map. On error, returns
C<NULL>.

=cut

*/

List *map_keys(Map *map)
{
	Mapper *mapper;
	List *keys;

	if (!map)
		return NULL;

	if (!(mapper = mapper_create(map)))
		return NULL;

	if (!(keys = list_create(NULL)))
	{
		mapper_destroy(mapper);
		return NULL;
	}

	while (mapper_has_next(mapper))
	{
		const Mapping *mapping = mapper_next_mapping(mapper);

		if (!list_append(keys, mapping->key))
		{
			mapper_destroy(mapper);
			list_destroy(keys);
			return NULL;
		}
	}

	mapper_destroy(mapper);

	return keys;
}

/*

=item C<List *map_values(Map *map)>

Creates and returns a list of all of the values contained in C<map>. The
caller is required to destroy the list. Also, the values in the list are not
owned by the list so it must not outlive C<map> if C<map> owns them. On
error, returns C<NULL>.

=cut

*/

List *map_values(Map *map)
{
	Mapper *mapper;
	List *values;

	if (!map)
		return NULL;

	if (!(mapper = mapper_create(map)))
		return NULL;

	if (!(values = list_create(NULL)))
	{
		mapper_destroy(mapper);
		return NULL;
	}

	while (mapper_has_next(mapper))
	{
		const Mapping *mapping = mapper_next_mapping(mapper);

		if (!list_append(values, mapping->value))
		{
			mapper_destroy(mapper);
			list_destroy(values);
			return NULL;
		}
	}

	mapper_destroy(mapper);

	return values;
}


/*

=item C<void map_apply(const Map *map, map_action_t *action, void *data)>

Invokes C<action> for each of C<map>'s items. The arguments passed to
C<action> are the key, the item and C<data>.

=cut

*/

void map_apply(Map *map, map_action_t *action, void *data)
{
	Mapper *mapper;

	if (!map || !action)
		return;

	if (!(mapper = mapper_create(map)))
		return;

	while (mapper_has_next(mapper))
	{
		const Mapping *mapping = mapper_next_mapping(mapper);
		action(mapping->key, mapping->value, data);
	}

	mapper_destroy(mapper);
}

/*

=item C<ssize_t map_size(const Map *map)>

Returns the number of mappings in C<map>. On error, returns -1.

=cut

*/

ssize_t map_size(const Map *map)
{
	if (!map)
		return -1;

	return map->items;
}

/*

=back

=head1 BUGS

C<NULL> can't be a key. Neither can zero when using integers as keys.

=head1 SEE ALSO

L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
L<log(3)|log(3)>,
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

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

#if 0
static void map_print(const char *name, Map *map)
{
	size_t i, j;

	if (!map)
	{
		printf("%s = nil\n", name);
		return;
	}

	printf("%s =\n{\n", name);

	for (i = 0; i < map->size; ++i)
	{
		if (map->chain[i])
		{
			List *chain = map->chain[i];

			for (j = 0; j < list_length(chain); ++j)
			{
				Mapping *mapping = (Mapping *)list_item(chain, j);

				printf("    [%d/%d] \"%s\" -> \"%s\"\n", i, j, (char *)mapping->key, (char *)mapping->value);
			}
		}
	}

	printf("}\n");
}
#endif

static void map_histogram(const char *name, Map *map)
{
	size_t i;
	int *histogram;

	if (!map)
	{
		printf("%s = nil\n", name);
		return;
	}

	if (!(histogram = mem_create(map->items, int)))
	{
		printf("Failed to allocate histogram for map %s\n", name);
		return;
	}

	memset(histogram, '\0', map->items * sizeof(int));

	for (i = 0; i < map->size; ++i)
	{
		size_t length = list_length(map->chain[i]);
		++histogram[length];
	}

	printf("\nhistogram %s =\n{\n", name);

	for (i = 0; i < map->items; ++i)
		if (histogram[i])
			printf("    %d chain%s of length %d\n", histogram[i], (histogram[i] == 1) ? "" : "s", i);

	printf("}\n");
}

static void test_hash(void)
{
	FILE *words = fopen("/usr/dict/words", "r");
	char word[BUFSIZ];
	Map *map;
	size_t c;
	size_t min = 0xffffffff;
	size_t max = 0x00000000;
	int sum = 0;

	if (!words)
	{
		printf("Failed to open /usr/dict/words\n");
		exit(1);
	}

	if (!(map = map_create(free)))
	{
		printf("Failed to create map\n");
		exit(1);
	}

	while (fgets(word, BUFSIZ, words))
	{
		char *eow = strchr(word, '\n');
		if (eow)
			*eow = '\0';

		map_add(map, word, mem_strdup(word));
	}

	fclose(words);

	printf("%d entries into %d buckets:\n\n", map->items, map->size);

	for (c = 0; c < map->size; ++c)
	{
		size_t length;

		if (!map->chain[c])
			continue;

		length = list_length(map->chain[c]);

		if (length > max)
			max = length;
		if (length < min)
			min = length;
		sum += length;
	}

	printf("avg = %g\n", (double)sum / (double)map->size);
	printf("min = %d\n", min);
	printf("max = %d\n", max);
	map_histogram("dict", map);
	map_destroy(map);

	exit(0);
}

static int sort_cmp(const char **a, const char **b)
{
	return strcmp(*a, *b);
}

static void test_action(char *key, char *value, char *cat)
{
	size_t len = strlen(cat);
	snprintf(cat + len, BUFSIZ, "%s%s=%s", (len) ? ", " : "", key, value);
}

typedef struct Point Point;
struct Point
{
	int x;
	int y;
};

static Point *point_create(int x, int y)
{
	Point *point = mem_create(1, Point);

	if (!point)
		return NULL;

	point->x = x;
	point->y = y;

	return point;
}

static Point *point_copy(Point *point)
{
	return point_create(point->x, point->y);
}

static int point_cmp(Point *a, Point *b)
{
	if (a->x > b->x)
		return 1;
	if (a->y > b->y)
		return 1;
	if (a->x == b->x && a->y == b->y)
		return 0;
	return -1;
}

static size_t point_hash(size_t size, Point *point)
{
	return (point->x * 31 + point->y * 37) % size;
}

static void point_release(Point *point)
{
	mem_release(point);
}

static int direct_copy(int key)
{
	return key;
}

static int direct_cmp(int a, int b)
{
	return a - b;
}

static size_t direct_hash(size_t size, int key)
{
	return key % size;
}

int main(int ac, char **av)
{
	int errors = 0;
	Map *map;
	Mapper *mapper;
	List *keys, *values;
	const void *ckey;
	const char *cvalue;
	char *value;

	char cat[BUFSIZ];

	if (ac == 2 && !strcmp(av[1], "hash"))
		test_hash();

	printf("Testing: map\n");

	/* Test map_create, map_add, map_get */

	if (!(map = map_create(NULL)))
		++errors, printf("Test1: map_create(NULL) failed\n");
	else
	{
		if (map_add(map, "abc", "abc") == -1)
			++errors, printf("Test2: map_add(\"abc\") failed\n");
		if (map_add(map, "def", "def") == -1)
			++errors, printf("Test3: map_add(\"def\") failed\n");
		if (map_add(map, "ghi", "ghi") == -1)
			++errors, printf("Test4: map_add(\"ghi\") failed\n");
		if (map_add(map, "jkl", "jkl") == -1)
			++errors, printf("Test5: map_add(\"jkl\") failed\n");

		value = (char *)map_get(map, "abc");
		if (!value || strcmp(value, "abc"))
			++errors, printf("Test6: map_get(\"abc\") failed\n");
		value = (char *)map_get(map, "def");
		if (!value || strcmp(value, "def"))
			++errors, printf("Test7: map_get(\"def\") failed\n");
		value = (char *)map_get(map, "ghi");
		if (!value || strcmp(value, "ghi"))
			++errors, printf("Test8: map_get(\"ghi\") failed\n");
		value = (char *)map_get(map, "jkl");
		if (!value || strcmp(value, "jkl"))
			++errors, printf("Test9: map_get(\"jkl\") failed\n");
		value = (char *)map_get(map, "zzz");
		if (value)
			++errors, printf("Test10: map_get(\"zzz\") failed\n");

		/* Test mapper_create, mapper_has_next, mapper_next, mapp_destroy */

		if (!(mapper = mapper_create(map)))
			++errors, printf("Test11: mapper_create() failed\n");
		else
		{
			if (!(keys = list_create(NULL)))
				printf("Test12: failed to create keys list\n");

			while (mapper_has_next(mapper))
			{
				void *item = mapper_next(mapper);

				if (!item)
					++errors, printf("Test13: mapper_next() failed\n");
				else
					list_append(keys, item);
			}

			mapper_destroy(mapper);
			if (mapper)
				++errors, printf("Test14: mapper_destroy() failed (%p, not null)", mapper);

			if (list_length(keys) != 4)
				++errors, printf("Test15: mapper failed (%d iterations not 4)\n", list_length(keys));
			else
			{
				list_sort(keys, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(keys, 0), "abc"))
					++errors, printf("Test16: mapper failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
				if (strcmp((char *)list_item(keys, 1), "def"))
					++errors, printf("Test17: mapper failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
				if (strcmp((char *)list_item(keys, 2), "ghi"))
					++errors, printf("Test18: mapper failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
				if (strcmp((char *)list_item(keys, 3), "jkl"))
					++errors, printf("Test19: mapper failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
			}

			list_destroy(keys);
			if (keys)
				++errors, printf("Test20: list_destroy() failed (%p, not null)\n", keys);
		}

		/* Test mapper_next_mapping, mapping_key, mapping_value */

		if (!(mapper = mapper_create(map)))
			++errors, printf("Test21: mapper_create() failed\n");
		else
		{
			if (!(keys = list_create(NULL)))
				++errors, printf("Test22: failed to create keys list\n");

			if (!(values = list_create(NULL)))
				++errors, printf("Test23: failed to create values list\n");

			while (mapper_has_next(mapper))
			{
				const Mapping *mapping = mapper_next_mapping(mapper);

				if (!mapping)
					++errors, printf("Test24: mapper_next_mapping() failed\n");
				else
				{
					ckey = mapping_key((void *)mapping);
					if (!ckey)
						++errors, printf("Test25: mapping_key() failed\n");
					else
						list_append(keys, (void *)ckey);

					cvalue = mapping_value(mapping);
					if (!cvalue)
						++errors, printf("Test26: mapping_value() failed\n");
					else
						list_append(values, (void *)cvalue);
				}
			}

			mapper_destroy(mapper);
			if (mapper)
				++errors, printf("Test27: mapper_destroy() failed (%p, not null)", mapper);

			if (list_length(keys) != 4)
				++errors, printf("Test28: mapper failed (%d key iterations not 4)\n", list_length(keys));
			else
			{
				list_sort(keys, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(keys, 0), "abc"))
					++errors, printf("Test29: mapper failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
				if (strcmp((char *)list_item(keys, 1), "def"))
					++errors, printf("Test30: mapper failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
				if (strcmp((char *)list_item(keys, 2), "ghi"))
					++errors, printf("Test31: mapper failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
				if (strcmp((char *)list_item(keys, 3), "jkl"))
					++errors, printf("Test32: mapper failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
			}

			list_destroy(keys);
			if (keys)
				++errors, printf("Test33: list_destroy() failed (%p, not null)\n", keys);

			if (list_length(values) != 4)
				++errors, printf("Test34: mapper failed (%d value iterations not 4)\n", list_length(values));
			else
			{
				list_sort(values, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(values, 0), "abc"))
					++errors, printf("Test35: mapper failed (\"%s\", not \"abc\")\n", (char *)list_item(values, 0));
				if (strcmp((char *)list_item(values, 1), "def"))
					++errors, printf("Test36: mapper failed (\"%s\", not \"def\")\n", (char *)list_item(values, 1));
				if (strcmp((char *)list_item(values, 2), "ghi"))
					++errors, printf("Test37: mapper failed (\"%s\", not \"ghi\")\n", (char *)list_item(values, 2));
				if (strcmp((char *)list_item(values, 3), "jkl"))
					++errors, printf("Test38: mapper failed (\"%s\", not \"jkl\")\n", (char *)list_item(values, 3));
			}

			list_destroy(values);
			if (values)
				++errors, printf("Test39: list_destroy() failed (%p, not null)\n", values);
		}

		/* Test map_has_next, map_next */

		if (!(keys = list_create(NULL)))
			printf("Test40: failed to create keys list\n");

		while (map_has_next(map))
		{
			void *item = map_next(map);

			if (!item)
				++errors, printf("Test41: map_next() failed\n");
			else
				list_append(keys, item);
		}

		if (list_length(keys) != 4)
			++errors, printf("Test42: map_has_next() failed (%d iterations not 4)\n", list_length(keys));
		else
		{
			list_sort(keys, (list_cmp_t *)sort_cmp);

			if (strcmp((char *)list_item(keys, 0), "abc"))
				++errors, printf("Test43: map_has_next() failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
			if (strcmp((char *)list_item(keys, 1), "def"))
				++errors, printf("Test44: map_has_next() failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
			if (strcmp((char *)list_item(keys, 2), "ghi"))
				++errors, printf("Test45: map_has_next() failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
			if (strcmp((char *)list_item(keys, 3), "jkl"))
				++errors, printf("Test46: map_has_next() failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
		}

		list_destroy(keys);
		if (keys)
			++errors, printf("Test47: list_destroy() failed (%p, not null)\n", keys);

		/* Test map_next_mapping */

		if (!(keys = list_create(NULL)))
			printf("Test48: failed to create keys list\n");

		if (!(values = list_create(NULL)))
			printf("Test49: failed to create values list\n");

		while (map_has_next(map))
		{
			const Mapping *mapping = map_next_mapping(map);

			if (!mapping)
				++errors, printf("Test50: map_next_mapping() failed\n");
			else
			{
				ckey = mapping_key(mapping);
				if (!ckey)
					++errors, printf("Test51: mapping_key() failed\n");
				else
					list_append(keys, (void *)ckey);

				cvalue = mapping_value(mapping);
				if (!cvalue)
					++errors, printf("Test52: mapping_value() failed\n");
				else
					list_append(values, (void *)cvalue);
			}
		}

		if (list_length(keys) != 4)
			++errors, printf("Test53: map_has_next() failed (%d iterations not 4)\n", list_length(keys));
		else
		{
			list_sort(keys, (list_cmp_t *)sort_cmp);

			if (strcmp((char *)list_item(keys, 0), "abc"))
				++errors, printf("Test54: map_has_next() failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
			if (strcmp((char *)list_item(keys, 1), "def"))
				++errors, printf("Test55: map_has_next() failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
			if (strcmp((char *)list_item(keys, 2), "ghi"))
				++errors, printf("Test56: map_has_next() failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
			if (strcmp((char *)list_item(keys, 3), "jkl"))
				++errors, printf("Test57: map_has_next() failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
		}

		list_destroy(keys);
		if (keys)
			++errors, printf("Test58: list_destroy() failed (%p, not null)\n", keys);

		if (list_length(values) != 4)
			++errors, printf("Test59: map_has_next() failed (%d iterations not 4)\n", list_length(values));
		else
		{
			list_sort(values, (list_cmp_t *)sort_cmp);

			if (strcmp((char *)list_item(values, 0), "abc"))
				++errors, printf("Test60: map_has_next() failed (\"%s\", not \"abc\")\n", (char *)list_item(values, 0));
			if (strcmp((char *)list_item(values, 1), "def"))
				++errors, printf("Test61: map_has_next() failed (\"%s\", not \"def\")\n", (char *)list_item(values, 1));
			if (strcmp((char *)list_item(values, 2), "ghi"))
				++errors, printf("Test62: map_has_next() failed (\"%s\", not \"ghi\")\n", (char *)list_item(values, 2));
			if (strcmp((char *)list_item(values, 3), "jkl"))
				++errors, printf("Test63: map_has_next() failed (\"%s\", not \"jkl\")\n", (char *)list_item(values, 3));
		}

		list_destroy(values);
		if (values)
			++errors, printf("Test64: list_destroy() failed (%p, not null)\n", values);

		/* Test map_keys */

		if (!(keys = map_keys(map)))
			++errors, printf("Test65: map_keys() failed\n");
		else
		{
			if (list_length(keys) != 4)
				++errors, printf("Test66: map_keys failed (%d keys, not 4)\n", list_length(keys));
			else
			{
				list_sort(keys, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(keys, 0), "abc"))
					++errors, printf("Test67: map_keys failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
				if (strcmp((char *)list_item(keys, 1), "def"))
					++errors, printf("Test68: map_keys failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
				if (strcmp((char *)list_item(keys, 2), "ghi"))
					++errors, printf("Test69: map_keys failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
				if (strcmp((char *)list_item(keys, 3), "jkl"))
					++errors, printf("Test70: map_keys failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
			}

			list_destroy(keys);
			if (keys)
				++errors, printf("Test71: list_destroy() failed (%p, not null)\n", keys);
		}

		/* Test map_values */

		if (!(values = map_values(map)))
			++errors, printf("Test72: map_values() failed\n");
		else
		{
			if (list_length(values) != 4)
				++errors, printf("Test73: map_values failed (%d values not 4)\n", list_length(values));
			else
			{
				list_sort(values, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(values, 0), "abc"))
					++errors, printf("Test74: map_values failed (\"%s\", not \"abc\")\n", (char *)list_item(values, 0));
				if (strcmp((char *)list_item(values, 1), "def"))
					++errors, printf("Test75: map_values failed (\"%s\", not \"def\")\n", (char *)list_item(values, 1));
				if (strcmp((char *)list_item(values, 2), "ghi"))
					++errors, printf("Test76: map_values failed (\"%s\", not \"ghi\")\n", (char *)list_item(values, 2));
				if (strcmp((char *)list_item(values, 3), "jkl"))
					++errors, printf("Test77: map_values failed (\"%s\", not \"jkl\")\n", (char *)list_item(values, 3));
			}

			list_destroy(values);
			if (values)
				++errors, printf("Test78: list_destroy() failed (%p, not null)\n", values);
		}

		/* Test map_remove */

		if (map_remove(map, "zzz") != -1)
			++errors, printf("Test79: map_remove(\"zzz\") failed\n");
		if (map_remove(map, "abc") == -1)
			++errors, printf("Test80: map_remove(\"abc\") failed\n");
		if (map_remove(map, "def") == -1)
			++errors, printf("Test81: map_remove(\"def\") failed\n");
		if (map_remove(map, "ghi") == -1)
			++errors, printf("Test82: map_remove(\"ghi\") failed\n");
		if (map_remove(map, "jkl") == -1)
			++errors, printf("Test83: map_remove(\"jkl\") failed\n");

		map_destroy(map);
		if (map)
			++errors, printf("Test84: map_destroy() failed (%p, not null)\n", map);
	}

	/* Test map_apply */

	if (!(map = map_create(NULL)))
		++errors, printf("Test 85: map_create(NULL) failed\n");
	else
	{
		if (map_add(map, "1", "7") == -1)
			++errors, printf("Test86: map_add(1, 7) failed\n");
		if (map_add(map, "2", "6") == -1)
			++errors, printf("Test87: map_add(2, 6) failed\n");
		if (map_add(map, "3", "5") == -1)
			++errors, printf("Test88: map_add(3, 5) failed\n");
		if (map_add(map, "4", "4") == -1)
			++errors, printf("Test89: map_add(4, 4) failed\n");
		if (map_add(map, "5", "3") == -1)
			++errors, printf("Test90: map_add(5, 3) failed\n");
		if (map_add(map, "6", "2") == -1)
			++errors, printf("Test91: map_add(6, 2) failed\n");
		if (map_add(map, "7", "1") == -1)
			++errors, printf("Test92: map_add(7, 1) failed\n");

		value = map_get(map, "1");
		if (strcmp(value, "7"))
			++errors, printf("Test93: map_get(1) failed (%s, not %s)\n", value, "7");

		value = map_get(map, "2");
		if (strcmp(value, "6"))
			++errors, printf("Test94: map_get(2) failed (%s, not %s)\n", value, "6");

		value = map_get(map, "3");
		if (strcmp(value, "5"))
			++errors, printf("Test95: map_get(3) failed (%s, not %s)\n", value, "5");

		value = map_get(map, "4");
		if (strcmp(value, "4"))
			++errors, printf("Test96: map_get(4) failed (%s, not %s)\n", value, "4");

		value = map_get(map, "5");
		if (strcmp(value, "3"))
			++errors, printf("Test97: map_get(5) failed (%s, not %s)\n", value, "3");

		value = map_get(map, "6");
		if (strcmp(value, "2"))
			++errors, printf("Test98: map_get(6) failed (%s, not %s)\n", value, "2");

		value = map_get(map, "7");
		if (strcmp(value, "1"))
			++errors, printf("Test99: map_get(7) failed (%s, not %s)\n", value, "1");

		cat[0] = '\0';
		map_apply(map, (map_action_t *)test_action, cat);
		if (strcmp(cat, "7=1, 1=7, 2=6, 3=5, 4=4, 5=3, 6=2"))
			++errors, printf("Test100: map_apply(cat) failed (cat = \"%s\", not \"%s\")\n", cat, "7=1, 1=7, 2=6, 3=5, 4=4, 5=3, 6=2");

		map_destroy(map);
		if (map)
			++errors, printf("Test101: map_destroy() failed (%p, not null)\n", map);
	}

	/* Test mapper_remove, map_size */

	if (!(map = map_create(NULL)))
		++errors, printf("Test 102: map_create(NULL) failed\n");
	else
	{
		if (map_add(map, "1", "7") == -1)
			++errors, printf("Test103: map_add(1, 7) failed\n");
		if (map_add(map, "2", "6") == -1)
			++errors, printf("Test104: map_add(2, 6) failed\n");
		if (map_add(map, "3", "5") == -1)
			++errors, printf("Test105: map_add(3, 5) failed\n");
		if (map_add(map, "4", "4") == -1)
			++errors, printf("Test106: map_add(4, 4) failed\n");
		if (map_add(map, "5", "3") == -1)
			++errors, printf("Test107: map_add(5, 3) failed\n");
		if (map_add(map, "6", "2") == -1)
			++errors, printf("Test108: map_add(6, 2) failed\n");
		if (map_add(map, "7", "1") == -1)
			++errors, printf("Test109: map_add(7, 1) failed\n");

		if (!(mapper = mapper_create(map)))
			++errors, printf("Test110: mapper_create() failed\n");
		else
		{
			while (mapper_has_next(mapper))
			{
				void *item = mapper_next(mapper);
				if (!item)
					++errors, printf("Test111: mapper_next() failed\n");
				mapper_remove(mapper);
			}

			if (map_size(map))
				++errors, printf("Test112: mapper_remove() failed (%d item remaining, not 0)\n", map_size(map));

			mapper_destroy(mapper);
			if (mapper)
				++errors, printf("Test113: mapper_destroy() failed (%p, not null)\n", mapper);
		}

		map_destroy(map);
		if (map)
			++errors, printf("Test114: map_destroy() failed (%p, not null)\n", map);
	}

	/* Test map_remove_current */

	if (!(map = map_create(NULL)))
		++errors, printf("Test 115: map_create(NULL) failed\n");
	else
	{
		if (map_add(map, "1", "1") == -1)
			++errors, printf("Test116: map_add(1, 1) failed\n");
		if (map_add(map, "2", "2") == -1)
			++errors, printf("Test117: map_add(2, 2) failed\n");
		if (map_add(map, "3", "3") == -1)
			++errors, printf("Test118: map_add(3, 3) failed\n");
		if (map_add(map, "4", "4") == -1)
			++errors, printf("Test119: map_add(4, 4) failed\n");

		while (map_has_next(map))
		{
			void *item = map_next(map);
			if (!item)
				++errors, printf("Test120: map_next() failed\n");

			map_remove_current(map);
		}

		if (map_size(map))
			++errors, printf("Test121: map_remove_current() failed (%d item remaining, not 0)\n", map_size(map));

		mapper_destroy(mapper);
		if (mapper)
			++errors, printf("Test122: mapper_destroy() failed (%p, not null)\n", mapper);

		map_destroy(map);
		if (map)
			++errors, printf("Test123: map_destroy() failed (%p, not null)\n", map);
	}

	/* Test map_create_generic (Point -> char *) */

	if (!(map = map_create_generic((map_copy_t *)point_copy, (map_cmp_t *)point_cmp, (map_hash_t *)point_hash, (map_destroy_t *)point_release, NULL)))
		++errors, printf("Test124: map_create_generic() failed\n");
	else
	{
		Point *point = point_create(0, 0);

		if (map_add(map, point_create(0, 0), "(0, 0)") == -1)
			++errors, printf("Test125: map_add(point(0, 0)) failed\n");
		if (map_add(map, point_create(1, 0), "(1, 0)") == -1)
			++errors, printf("Test126: map_add(point(1, 0)) failed\n");
		if (map_add(map, point_create(0, 1), "(0, 1)") == -1)
			++errors, printf("Test127: map_add(point(0, 1)) failed\n");
		if (map_add(map, point_create(1, 1), "(1, 1)") == -1)
			++errors, printf("Test128: map_add(point(1, 1)) failed\n");
		if (map_add(map, point_create(-1, 0), "(-1, 0)") == -1)
			++errors, printf("Test129: map_add(point(-1, 0)) failed\n");
		if (map_add(map, point_create(0, -1), "(0, -1)") == -1)
			++errors, printf("Test130: map_add(point(0, -1)) failed\n");
		if (map_add(map, point_create(-1, -1), "(-1, -1)") == -1)
			++errors, printf("Test131: map_add(point(-1, -1)) failed\n");
		if (map_add(map, point_create(2, 0), "(2, 0)") == -1)
			++errors, printf("Test132: map_add(point(2, 0)) failed\n");
		if (map_add(map, point_create(0, 2), "(0, 2)") == -1)
			++errors, printf("Test133: map_add(point(0, 2)) failed\n");
		if (map_add(map, point_create(2, 2), "(2, 2)") == -1)
			++errors, printf("Test134: map_add(point(2, 2)) failed\n");
		if (map_add(map, point_create(-2, 0), "(-2, 0)") == -1)
			++errors, printf("Test135: map_add(point(-2, 0)) failed\n");
		if (map_add(map, point_create(0, -2), "(0, -2)") == -1)
			++errors, printf("Test136: map_add(point(0, -2)) failed\n");
		if (map_add(map, point_create(-2, -2), "(-2, -2)") == -1)
			++errors, printf("Test137: map_add(point(-2, -2)) failed\n");
		if (map_add(map, point_create(0, 0), "(0, 0)") != -1)
			++errors, printf("Test138: map_add(point(0, 0)) failed\n");

		if (map_size(map) != 13)
			++errors, printf("Test139: map_create_generic() failed (%d items, not %d)\n", map_size(map), 13);

		point->x = 0;
		point->y = 0;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test140: map_get(generic) failed\n");
		else if (strcmp(value, "(0, 0)"))
			++errors, printf("Test141: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(0, 0)");

		point->x = 1;
		point->y = 0;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test142: map_get(generic) failed\n");
		else if (strcmp(value, "(1, 0)"))
			++errors, printf("Test143: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(1, 0)");

		point->x = 0;
		point->y = 1;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test144: map_get(generic) failed\n");
		else if (strcmp(value, "(0, 1)"))
			++errors, printf("Test145: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(0, 1)");

		point->x = 1;
		point->y = 1;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test146: map_get(generic) failed\n");
		else if (strcmp(value, "(1, 1)"))
			++errors, printf("Test147: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(1, 1)");

		point->x = -1;
		point->y = 0;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test148: map_get(generic) failed\n");
		else if (strcmp(value, "(-1, 0)"))
			++errors, printf("Test149: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(-1, 0)");

		point->x = 0;
		point->y = -1;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test150: map_get(generic) failed\n");
		else if (strcmp(value, "(0, -1)"))
			++errors, printf("Test151: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(0, -1)");

		point->x = -1;
		point->y = -1;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test152: map_get(generic) failed\n");
		else if (strcmp(value, "(-1, -1)"))
			++errors, printf("Test153: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(-1, -1)");

		point->x = 2;
		point->y = 0;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test154: map_get(generic) failed\n");
		else if (strcmp(value, "(2, 0)"))
			++errors, printf("Test155: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(2, 0)");

		point->x = 0;
		point->y = 2;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test156: map_get(generic) failed\n");
		else if (strcmp(value, "(0, 2)"))
			++errors, printf("Test157: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(0, 2)");

		point->x = 2;
		point->y = 2;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test158: map_get(generic) failed\n");
		else if (strcmp(value, "(2, 2)"))
			++errors, printf("Test159: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(2, 2)");

		point->x = -2;
		point->y = 0;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test160: map_get(generic) failed\n");
		else if (strcmp(value, "(-2, 0)"))
			++errors, printf("Test161: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(-2, 0)");

		point->x = 0;
		point->y = -2;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test162: map_get(generic) failed\n");
		else if (strcmp(value, "(0, -2)"))
			++errors, printf("Test163: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(0, -2)");

		point->x = -2;
		point->y = -2;
		value = map_get(map, point);
		if (!value)
			++errors, printf("Test164: map_get(generic) failed\n");
		else if (strcmp(value, "(-2, -2)"))
			++errors, printf("Test165: map_get(generic) failed (\"%s\", not \"%s\")\n", value, "(-2, -2)");

		point_release(point);
		map_destroy(map);
		if (map)
			++errors, printf("Test166: map_destroy() failed (%p, not null)\n", map);
	}

	/* Test map_create_generic (int -> int) and map growth */

	if (!(map = map_create_generic((map_copy_t *)direct_copy, (map_cmp_t *)direct_cmp, (map_hash_t *)direct_hash, NULL, NULL)))
		++errors, printf("Test167: map_create_generic() failed\n");
	else
	{
		if (map_add(map, (void *)1, (void *)1) == -1)
			++errors, printf("Test168: map_add(1, 1) failed\n");
		if (map_add(map, (void *)2, (void *)2) == -1)
			++errors, printf("Test169: map_add(2, 2) failed\n");
		if (map_add(map, (void *)3, (void *)3) == -1)
			++errors, printf("Test170: map_add(3, 3) failed\n");
		if (map_add(map, (void *)4, (void *)4) == -1)
			++errors, printf("Test171: map_add(4, 4) failed\n");
		if (map_add(map, (void *)5, (void *)5) == -1)
			++errors, printf("Test172: map_add(5, 5) failed\n");
		if (map_add(map, (void *)6, (void *)6) == -1)
			++errors, printf("Test173: map_add(6, 6) failed\n");
		if (map_add(map, (void *)7, (void *)7) == -1)
			++errors, printf("Test174: map_add(7, 7) failed\n");
		if (map_add(map, (void *)8, (void *)8) == -1)
			++errors, printf("Test175: map_add(8, 8) failed\n");
		if (map_add(map, (void *)9, (void *)9) == -1)
			++errors, printf("Test175: map_add(9, 9) failed\n");
		if (map_add(map, (void *)10, (void *)10) == -1)
			++errors, printf("Test176: map_add(10, 10) failed\n");
		if (map_add(map, (void *)11, (void *)11) == -1)
			++errors, printf("Test177: map_add(11, 11) failed\n");
		if (map_add(map, (void *)12, (void *)12) == -1)
			++errors, printf("Test178: map_add(12, 12) failed\n");
		if (map_add(map, (void *)13, (void *)13) == -1)
			++errors, printf("Test179: map_add(13, 13) failed\n");
		if (map_add(map, (void *)14, (void *)14) == -1)
			++errors, printf("Test180: map_add(13, 14) failed\n");
		if (map_add(map, (void *)15, (void *)15) == -1)
			++errors, printf("Test181: map_add(13, 15) failed\n");
		if (map_add(map, (void *)16, (void *)16) == -1)
			++errors, printf("Test182: map_add(13, 16) failed\n");
		if (map_add(map, (void *)17, (void *)17) == -1)
			++errors, printf("Test183: map_add(17, 17) failed\n");
		if (map_add(map, (void *)18, (void *)18) == -1)
			++errors, printf("Test184: map_add(18, 18) failed\n");
		if (map_add(map, (void *)19, (void *)19) == -1)
			++errors, printf("Test185: map_add(19, 19) failed\n");
		if (map_add(map, (void *)20, (void *)20) == -1)
			++errors, printf("Test186: map_add(20, 20) failed\n");
		if (map_add(map, (void *)21, (void *)21) == -1)
			++errors, printf("Test187: map_add(21, 21) failed\n");
		if (map_add(map, (void *)22, (void *)22) == -1)
			++errors, printf("Test188: map_add(22, 22) failed\n");
		if (map_add(map, (void *)23, (void *)23) == -1)
			++errors, printf("Test189: map_add(23, 23) failed\n");
		if (map_add(map, (void *)24, (void *)24) == -1)
			++errors, printf("Test190: map_add(24, 24) failed\n");
		if (map_add(map, (void *)25, (void *)25) == -1)
			++errors, printf("Test191: map_add(25, 25) failed\n");
		if (map_add(map, (void *)25, (void *)25) != -1)
			++errors, printf("Test192: map_add(25, 25) failed\n");

		if (map_size(map) != 25)
			++errors, printf("Test193: map_create_generic() failed (%d items, not %d)\n", map_size(map), 13);

		if ((int)map_get(map, (void *)1) != 1)
			++errors, printf("Test194: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)1), 1);
		if ((int)map_get(map, (void *)2) != 2)
			++errors, printf("Test195: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)2), 2);
		if ((int)map_get(map, (void *)3) != 3)
			++errors, printf("Test196: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)3), 3);
		if ((int)map_get(map, (void *)4) != 4)
			++errors, printf("Test197: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)4), 4);
		if ((int)map_get(map, (void *)5) != 5)
			++errors, printf("Test198: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)5), 5);
		if ((int)map_get(map, (void *)6) != 6)
			++errors, printf("Test199: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)6), 6);
		if ((int)map_get(map, (void *)7) != 7)
			++errors, printf("Test200: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)7), 7);
		if ((int)map_get(map, (void *)8) != 8)
			++errors, printf("Test201: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)8), 8);
		if ((int)map_get(map, (void *)9) != 9)
			++errors, printf("Test202: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)9), 9);
		if ((int)map_get(map, (void *)10) != 10)
			++errors, printf("Test203: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)10), 10);
		if ((int)map_get(map, (void *)11) != 11)
			++errors, printf("Test204: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)11), 11);
		if ((int)map_get(map, (void *)12) != 12)
			++errors, printf("Test205: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)12), 12);
		if ((int)map_get(map, (void *)13) != 13)
			++errors, printf("Test206: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)13), 13);
		if ((int)map_get(map, (void *)14) != 14)
			++errors, printf("Test207: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)14), 14);
		if ((int)map_get(map, (void *)15) != 15)
			++errors, printf("Test208: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)15), 15);
		if ((int)map_get(map, (void *)16) != 16)
			++errors, printf("Test209: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)16), 16);
		if ((int)map_get(map, (void *)17) != 17)
			++errors, printf("Test210: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)17), 17);
		if ((int)map_get(map, (void *)18) != 18)
			++errors, printf("Test211: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)18), 18);
		if ((int)map_get(map, (void *)19) != 19)
			++errors, printf("Test212: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)19), 19);
		if ((int)map_get(map, (void *)20) != 20)
			++errors, printf("Test213: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)20), 20);
		if ((int)map_get(map, (void *)21) != 21)
			++errors, printf("Test214: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)21), 21);
		if ((int)map_get(map, (void *)22) != 22)
			++errors, printf("Test215: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)22), 22);
		if ((int)map_get(map, (void *)23) != 23)
			++errors, printf("Test216: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)23), 23);
		if ((int)map_get(map, (void *)24) != 24)
			++errors, printf("Test217: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)24), 24);
		if ((int)map_get(map, (void *)25) != 25)
			++errors, printf("Test218: map_get(generic, int) failed (%d, not %d)\n", (int)map_get(map, (void *)25), 25);

		map_destroy(map);
		if (map)
			++errors, printf("Test219: map_destroy() failed (%p, not null)\n", map);
	}

	if (errors)
		printf("%d/219 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
