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

#ifndef LIBSLACK_MAP_H
#define LIBSLACK_MAP_H

#include <slack/hdr.h>
#include <slack/list.h>

typedef struct Map Map;
typedef struct Mapper Mapper;
typedef struct Mapping Mapping;
typedef list_destroy_t map_destroy_t;
typedef list_copy_t map_copy_t;
typedef list_cmp_t map_cmp_t;
typedef size_t map_hash_t(size_t table_size, const void *key);
typedef void map_action_t(void *key, void *item, void *data);

#undef map_destroy
#undef mapper_destroy

__START_DECLS
Map *map_create __PROTO ((map_destroy_t *destroy));
Map *map_create_sized __PROTO ((size_t size, map_destroy_t *destroy));
Map *map_create_with_hash __PROTO ((map_hash_t *hash, map_destroy_t *destroy));
Map *map_create_with_hash_sized __PROTO ((size_t size, map_hash_t *hash, map_destroy_t *destroy));
Map *map_create_generic __PROTO ((map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy));
Map *map_create_generic_sized __PROTO ((size_t size, map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_destroy_t *key_destroy, map_destroy_t *value_destroy));
void map_release __PROTO ((Map *map));
#define map_destroy(map) map_destroy_func(&(map))
void *map_destroy_func __PROTO ((Map **map));
int map_own __PROTO ((Map *map, map_destroy_t *destroy));
map_destroy_t *map_disown __PROTO ((Map *map));
int map_add __PROTO ((Map *map, const void *key, void *value));
int map_put __PROTO ((Map *map, const void *key, void *value));
int map_insert __PROTO ((Map *map, const void *key, void *value, int replace));
int map_remove __PROTO ((Map *map, const void *key));
void *map_get __PROTO ((const Map *map, const void *key));
Mapper *mapper_create __PROTO ((Map *map));
void mapper_release __PROTO ((Mapper *mapper));
#define mapper_destroy(mapper) mapper_destroy_func(&(mapper))
void *mapper_destroy_func __PROTO ((Mapper **mapper));
int mapper_has_next __PROTO ((Mapper *mapper));
void *mapper_next __PROTO ((Mapper *mapper));
const Mapping *mapper_next_mapping __PROTO ((Mapper *mapper));
void mapper_remove __PROTO ((Mapper *mapper));
int map_has_next __PROTO ((Map *map));
void *map_next __PROTO ((Map *map));
const Mapping *map_next_mapping __PROTO ((Map *map));
void map_remove_current __PROTO ((Map *map));
const void *mapping_key __PROTO ((const Mapping *mapping));
const void *mapping_value __PROTO ((const Mapping *mapping));
List *map_keys __PROTO ((Map *map));
List *map_values __PROTO ((Map *map));
void map_apply __PROTO ((Map *, map_action_t *action, void *data));
ssize_t map_size __PROTO ((const Map *map));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
