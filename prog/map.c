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

I<libprog(map)> - map module

=head1 SYNOPSIS

    #include <prog/map.h>

    typedef struct Map Map;
    typedef struct Mapper Mapper;

    #define map_destroy(map)
    #define mapper_destroy(mapper)

    Map *map_create(list_destroy_t *destroy);
    void map_release(Map *map);
    void *map_destroy_func(Map **map);
    int map_add(Map *map, const char *key, void *value);
    int map_put(Map *map, const char *key, void *value);
    int map_remove(Map *map, const char *key);
    void *map_lookup(const Map *map, const char *key);
    Mapper *mapper_create(const Map *map);
    void mapper_release(Mapper *mapper);
    void *mapper_destroy_func(Mapper **mapper);
    int map_has_next(Mapper *mapper);
    void *map_next(Mapper *mapper);
    List *map_keys(const Map *map);
    List *map_values(const Map *map);


=head1 DESCRIPTION

This module provides functions for manipulating and iterating over a set of
mappings from strings to homogeneous data (or heterogeneous data if it's
polymorphic), also known as associative arrays. I<Maps> may own their items.
I<Maps> created with a non-C<NULL> destroy function use that function to
destroy an item when it is removed from the map and to destroy each item
when the map itself it destroyed.

=over 4

=cut

*/

#include <stdlib.h>

#include "map.h"
#include "mem.h"
#include "err.h"

/*
** The table size must be a prime.
*/

#ifndef MAP_SIZE
#define MAP_SIZE 101
#endif

typedef struct Mapping Mapping;

struct Map
{
	List *chain[MAP_SIZE];   /* array of hash buckets */
	list_destroy_t *destroy; /* destructor function for items */
};

struct Mapping
{
	char *key;               /* a map key */
	void *value;             /* a map value */
	list_destroy_t *destroy; /* destructor function for value */
};

struct Mapper
{
	const Map *map;           /* the map being iterated over */
	ssize_t chain_index;      /* the index of the chain of the current item */
	ssize_t item_index;       /* the index of the current item */
	ssize_t next_chain_index; /* the index of the chain of the next item */
	ssize_t next_item_index;  /* the index of the next item */
};

/*

C<size_t hash(const char *key)>

JPW hash function. Returns a hash value (in the range 0..100) for C<key>.

*/

static size_t hash(const char *key)
{
	size_t g, h = 0;

	while (*key)
		if ((g = (h <<= 4, h += *key++) & 0xf0000000))
			h ^= (g >> 24) ^ g;

	return h % MAP_SIZE;
}

/*

C<Mapping *mapping_create(const char *key, void *value, list_destroy_t *destroy)>

Creates a new mapping from C<key> to C<value>. C<destroy> is the destructor
function for C<value>. Returns the new mapping on success, or C<NULL> on
error.

*/

static Mapping *mapping_create(const char *key, void *value, list_destroy_t *destroy)
{
	Mapping *mapping;

	if (!(mapping = mem_create(1, Mapping)))
		return NULL;

	if (!(mapping->key = mem_strdup(key)))
	{
		mem_release(mapping);
		return NULL;
	}

	mapping->value = value;
	mapping->destroy = destroy;

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

	if (mapping->destroy && mapping->value)
		mapping->destroy(mapping->value);

	mem_release(mapping->key);
	mem_release(mapping);
}

/*

=item C<Map *map_create(list_destroy_t *destroy)>

Creates a I<Map> with C<destroy> as its item destructor. Returns the the new
map on success, or C<NULL> on error.

=cut

*/

Map *map_create(list_destroy_t *destroy)
{
	Map *map;

	if (!(map = mem_create(1, Map)))
		return NULL;

	memset(map->chain, '\0', MAP_SIZE * sizeof(List *));
	map->destroy = destroy;

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

	for (i = 0; i < MAP_SIZE; ++i)
		list_release(map->chain[i]);

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

=item C<item map_add(Map *map, const char *key, void *value)>

Adds the C<(key, value)> mapping to C<map> if C<key> is not already present.
Returns 0 on success, or -1 on error.

=cut

*/

int map_add(Map *map, const char *key, void *value)
{
	Mapping *mapping;
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return -1;

	h = hash(key);

	if (!map->chain[h] && !(map->chain[h] = list_create((list_destroy_t *)mapping_release)))
		return -1;

	chain = map->chain[h];
	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		mapping = (Mapping *)list_item(chain, c);

		if (!strcmp(mapping->key, key))
			return -1;
	}

	if (!(mapping = mapping_create(key, value, map->destroy)))
		return -1;

	return list_append(chain, mapping) ? 0 : -1;
}

/*

=item C<item map_put(Map *map, const char *key, void *value)>

Adds the C<(key, value)> mapping to C<map>, replacing any existing
C<(key, value)> mapping. Returns 0 on success, or -1 on error.

=cut

*/

int map_put(Map *map, const char *key, void *value)
{
	Mapping *mapping;
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return -1;

	h = hash(key);

	if (!map->chain[h] && !(map->chain[h] = list_create((list_destroy_t *)mapping_release)))
		return -1;

	chain = map->chain[h];
	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		mapping = (Mapping *)list_item(chain, c);

		if (!strcmp(mapping->key, key))
		{
			list_remove(chain, c);
			break;
		}
	}

	if (!(mapping = mapping_create(key, value, map->destroy)))
		return -1;

	return list_append(chain, mapping) ? 0 : -1;
}

/*

=item C<int map_remove(Map *map, const char *key)>

Removes C<(key, value)> mapping from C<map> if it is present. If C<map> was
created with a destroy function, then the value will be destroyed. Returns 0
on success, or -1 on error.

=cut

*/

int map_remove(Map *map, const char *key)
{
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return -1;

	h = hash(key);
	chain = map->chain[h];

	if (!chain)
		return -1;

	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		Mapping *mapping = (Mapping *)list_item(chain, c);

		if (!strcmp(mapping->key, key))
			return list_remove(chain, c) ? 0 : -1;
	}

	return -1;
}

/*

=item C<void *map_lookup(const Map *map, const char *key)>

Returns the value associated with C<key> in C<map>, or C<NULL> if there is
none.

=cut

*/

void *map_lookup(const Map *map, const char *key)
{
	List *chain;
	size_t h, c, length;

	if (!map || !key)
		return NULL;

	h = hash(key);
	chain = map->chain[h];

	if (!chain)
		return NULL;

	length = list_length(chain);

	for (c = 0; c < length; ++c)
	{
		Mapping *mapping = (Mapping *)list_item(chain, c);

		if (!strcmp(mapping->key, key))
			return mapping->value;
	}

	return NULL;
}

/*

=item C<Mapper *mapper_create(const Map *map)>

Creates an iterator for C<map>. Returns the iterator on success, or C<NULL>
on error.

=cut

*/

Mapper *mapper_create(const Map *map)
{
	Mapper *mapper;

	if (!map)
		return NULL;

	if (!(mapper = mem_create(1, Mapper)))
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

=item C<int map_has_next(Mapper *mapper)>

Returns whether or not there is another item in the map that C<mapper> is
iterating over.

=cut

*/

int map_has_next(Mapper *mapper)
{
	List *chain;

	if (!mapper)
		return 0;

	// Find the current/first chain

	mapper->next_chain_index = mapper->chain_index;
	mapper->next_item_index = mapper->item_index;

	if (mapper->next_chain_index == -1)
		++mapper->next_chain_index;

	while (mapper->next_chain_index < MAP_SIZE && !mapper->map->chain[mapper->next_chain_index])
		++mapper->next_chain_index;

	if (mapper->next_chain_index == MAP_SIZE)
		return 0;

	chain = mapper->map->chain[mapper->next_chain_index];

	// Find the next item

	if (++mapper->next_item_index < list_length(chain))
		return 1;

	do
	{
		++mapper->next_chain_index;

		while (mapper->next_chain_index < MAP_SIZE && !mapper->map->chain[mapper->next_chain_index])
			++mapper->next_chain_index;

		if (mapper->next_chain_index == MAP_SIZE)
			return 0;

		chain = mapper->map->chain[mapper->next_chain_index];
	}
	while (!list_length(chain));

	mapper->next_item_index = 0;

	return 1;
}

/*

C<void *map_next_mapping(Mapper *Mapper)>

Returns the next mapping in the map that C<mapper> is iterating over.

*/

static Mapping *map_next_mapping(Mapper *mapper)
{
	mapper->chain_index = mapper->next_chain_index;
	mapper->item_index = mapper->next_item_index;

	return (Mapping *)list_item(mapper->map->chain[mapper->chain_index], mapper->item_index);
}

/*

=item C<void *map_next(Mapper *Mapper)>

Returns the next item in the map that C<mapper> is iterating over.

=cut

*/

void *map_next(Mapper *mapper)
{
	if (!mapper)
		return NULL;

	return map_next_mapping(mapper)->value;
}

/*

=item C<List *map_keys(const Map *map)>

Creates and returns a list of all of the keys contained in C<map>. The
caller is required to destroy the list. Also, the keys in the list are owned
by C<map> so the list returned must not outlive the map.

=cut

*/

List *map_keys(const Map *map)
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

	while (map_has_next(mapper))
	{
		Mapping *mapping = map_next_mapping(mapper);

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

=item C<List *map_values(const Map *map)>

Creates and returns a list of all of the values contained in C<map>. The
caller is required to destroy the list. Also, the values in the list are not
owned by the list so it must not outlive C<map> if C<map> owns them.

=cut

*/

List *map_values(const Map *map)
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

	while (map_has_next(mapper))
	{
		Mapping *mapping = map_next_mapping(mapper);

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

=back

=head1 BUGS

I<Map> is implemented as a hash table with 101 buckets. Each bucket is a
I<List>. So maps don't get "full" but performance can degrade if there are
too many items. You can't change the table size or the hash function used
without recompiling I<libprog>.

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
#include <string.h>

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

	for (i = 0; i < MAP_SIZE; ++i)
	{
		if (map->chain[i])
		{
			List *chain = map->chain[i];

			for (j = 0; j < list_length(chain); ++j)
			{
				Mapping *mapping = (Mapping *)list_item(chain, j);

				printf("    [%d/%d] \"%s\" -> \"%s\"\n", i, j, mapping->key, (char *)mapping->value);
			}
		}
	}

	printf("}\n");
}
#endif

static void test_hash(void)
{
	FILE *words = fopen("/usr/dict/words", "r");
	char word[BUFSIZ];
	Map *map;
	size_t num;
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

	for (num = 0; fgets(word, BUFSIZ, words); ++num)
	{
		char *eow = strchr(word, '\n');
		if (eow)
			*eow = '\0';

		map_add(map, word, mem_strdup(word));
	}

	fclose(words);

	printf("%d entries into %d buckets:\n\n", num, MAP_SIZE);

	for (c = 0; c < 101; ++c)
	{
		size_t length;

		if (!map->chain[c])
			continue;

		length = list_length(map->chain[c]);
		/* printf("[%d] %d entries\n", c, length); */

		if (length > max)
			max = length;
		if (length < min)
			min = length;
		sum += length;
	}

	map_destroy(map);

	printf("avg = %d\n", sum / MAP_SIZE);
	printf("min = %d\n", min);
	printf("max = %d\n", max);

	exit(0);
}

static int sort_cmp(const char **a, const char **b)
{
	return strcmp(*a, *b);
}

int main(int ac, char **av)
{
	int errors = 0;
	Map *map;
	Mapper *mapper;
	char *value;
	List *keys, *values;

	if (ac == 2 && !strcmp(av[1], "hash"))
		test_hash();

	printf("Testing: map\n");

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

		value = (char *)map_lookup(map, "abc");
		if (!value || strcmp(value, "abc"))
			++errors, printf("Test6: map_lookup(\"abc\") failed\n");
		value = (char *)map_lookup(map, "def");
		if (!value || strcmp(value, "def"))
			++errors, printf("Test7: map_lookup(\"def\") failed\n");
		value = (char *)map_lookup(map, "ghi");
		if (!value || strcmp(value, "ghi"))
			++errors, printf("Test8: map_lookup(\"ghi\") failed\n");
		value = (char *)map_lookup(map, "jkl");
		if (!value || strcmp(value, "jkl"))
			++errors, printf("Test9: map_lookup(\"jkl\") failed\n");
		value = (char *)map_lookup(map, "zzz");
		if (value)
			++errors, printf("Test10: map_lookup(\"zzz\") failed\n");

		if (!(mapper = mapper_create(map)))
			++errors, printf("Test11: mapper_create() failed\n");
		else
		{
			if (!(keys = list_create(NULL)))
				printf("Test12: failed to create keys list\n");

			while (map_has_next(mapper))
				list_append(keys, map_next(mapper));

			mapper_destroy(mapper);

			if (list_length(keys) != 4)
				++errors, printf("Test12: mapper failed (%d iterations not 4)\n", list_length(keys));
			else
			{
				list_sort(keys, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(keys, 0), "abc"))
					++errors, printf("Test13: mapper failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
				if (strcmp((char *)list_item(keys, 1), "def"))
					++errors, printf("Test14: mapper failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
				if (strcmp((char *)list_item(keys, 2), "ghi"))
					++errors, printf("Test15: mapper failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
				if (strcmp((char *)list_item(keys, 3), "jkl"))
					++errors, printf("Test16: mapper failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
			}

			list_destroy(keys);
		}

		if (!(keys = map_keys(map)))
			++errors, printf("Test17: map_keys() failed\n");
		else
		{
			if (list_length(keys) != 4)
				++errors, printf("Test18: map_keys failed (%d keys not 4)\n", list_length(keys));
			else
			{
				list_sort(keys, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(keys, 0), "abc"))
					++errors, printf("Test19: map_keys failed (\"%s\", not \"abc\")\n", (char *)list_item(keys, 0));
				if (strcmp((char *)list_item(keys, 1), "def"))
					++errors, printf("Test20: map_keys failed (\"%s\", not \"def\")\n", (char *)list_item(keys, 1));
				if (strcmp((char *)list_item(keys, 2), "ghi"))
					++errors, printf("Test21: map_keys failed (\"%s\", not \"ghi\")\n", (char *)list_item(keys, 2));
				if (strcmp((char *)list_item(keys, 3), "jkl"))
					++errors, printf("Test22: map_keys failed (\"%s\", not \"jkl\")\n", (char *)list_item(keys, 3));
			}

			list_destroy(keys);
		}

		if (!(values = map_values(map)))
			++errors, printf("Test23: map_values() failed\n");
		else
		{
			if (list_length(values) != 4)
				++errors, printf("Test24: map_values failed (%d values not 4)\n", list_length(values));
			else
			{
				list_sort(values, (list_cmp_t *)sort_cmp);

				if (strcmp((char *)list_item(values, 0), "abc"))
					++errors, printf("Test25: map_values failed (\"%s\", not \"abc\")\n", (char *)list_item(values, 0));
				if (strcmp((char *)list_item(values, 1), "def"))
					++errors, printf("Test26: map_values failed (\"%s\", not \"def\")\n", (char *)list_item(values, 1));
				if (strcmp((char *)list_item(values, 2), "ghi"))
					++errors, printf("Test27: map_values failed (\"%s\", not \"ghi\")\n", (char *)list_item(values, 2));
				if (strcmp((char *)list_item(values, 3), "jkl"))
					++errors, printf("Test28: map_values failed (\"%s\", not \"jkl\")\n", (char *)list_item(values, 3));
			}

			list_destroy(values);
		}

		if (map_remove(map, "zzz") != -1)
			++errors, printf("Test29: map_remove(\"zzz\") failed\n");
		if (map_remove(map, "abc") == -1)
			++errors, printf("Test30: map_remove(\"abc\") failed\n");
		if (map_remove(map, "def") == -1)
			++errors, printf("Test31: map_remove(\"def\") failed\n");
		if (map_remove(map, "ghi") == -1)
			++errors, printf("Test32: map_remove(\"ghi\") failed\n");
		if (map_remove(map, "jkl") == -1)
			++errors, printf("Test33: map_remove(\"jkl\") failed\n");

		map_destroy(map);
		if (map)
			++errors, printf("Test34: map_destroy() failed\n");
	}

	if (errors)
		printf("%d/34 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
