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
* 20011109 raf <raf@raf.org>
*/

#ifndef LIBSLACK_MAP_H
#define LIBSLACK_MAP_H

#include <slack/hdr.h>
#include <slack/list.h>
#include <slack/locker.h>

typedef struct Map Map;
typedef struct Mapper Mapper;
typedef struct Mapping Mapping;
typedef list_release_t map_release_t;
typedef list_copy_t map_copy_t;
typedef list_cmp_t map_cmp_t;
typedef size_t map_hash_t(size_t table_size, const void *key);
typedef void map_action_t(void *key, void *item, void *data);

_begin_decls
Map *map_create _args ((map_release_t *destroy));
Map *map_create_sized _args ((size_t size, map_release_t *destroy));
Map *map_create_with_hash _args ((map_hash_t *hash, map_release_t *destroy));
Map *map_create_sized_with_hash _args ((size_t size, map_hash_t *hash, map_release_t *destroy));
Map *map_create_with_locker _args ((Locker *locker, map_release_t *destroy));
Map *map_create_with_locker_sized _args ((Locker *locker, size_t size, map_release_t *destroy));
Map *map_create_with_locker_with_hash _args ((Locker *locker, map_hash_t *hash, map_release_t *destroy));
Map *map_create_with_locker_sized_with_hash _args ((Locker *locker, size_t size, map_hash_t *hash, map_release_t *destroy));
Map *map_create_generic _args ((map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_release_t *key_destroy, map_release_t *value_destroy));
Map *map_create_generic_sized _args ((size_t size, map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_release_t *key_destroy, map_release_t *value_destroy));
Map *map_create_generic_with_locker _args ((Locker *locker, map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_release_t *key_destroy, map_release_t *value_destroy));
Map *map_create_generic_with_locker_sized _args ((Locker *locker, size_t size, map_copy_t *copy, map_cmp_t *cmp, map_hash_t *hash, map_release_t *key_destroy, map_release_t *value_destroy));
int map_rdlock _args ((const Map *map));
int map_wrlock _args ((const Map *map));
int map_unlock _args ((const Map *map));
void map_release _args ((Map *map));
void *map_destroy _args ((Map **map));
int map_own _args ((Map *map, map_release_t *destroy));
int map_own_unlocked _args ((Map *map, map_release_t *destroy));
map_release_t *map_disown _args ((Map *map));
map_release_t *map_disown_unlocked _args ((Map *map));
int map_add _args ((Map *map, const void *key, void *value));
int map_add_unlocked _args ((Map *map, const void *key, void *value));
int map_put _args ((Map *map, const void *key, void *value));
int map_put_unlocked _args ((Map *map, const void *key, void *value));
int map_insert _args ((Map *map, const void *key, void *value, int replace));
int map_insert_unlocked _args ((Map *map, const void *key, void *value, int replace));
int map_remove _args ((Map *map, const void *key));
int map_remove_unlocked _args ((Map *map, const void *key));
void *map_get _args ((Map *map, const void *key));
void *map_get_unlocked _args ((const Map *map, const void *key));
Mapper *mapper_create _args ((Map *map));
Mapper *mapper_create_rdlocked _args ((Map *map));
Mapper *mapper_create_wrlocked _args ((Map *map));
Mapper *mapper_create_unlocked _args ((Map *map));
void mapper_release _args ((Mapper *mapper));
void mapper_release_unlocked _args ((Mapper *mapper));
void *mapper_destroy _args ((Mapper **mapper));
void *mapper_destroy_unlocked _args ((Mapper **mapper));
int mapper_has_next _args ((Mapper *mapper));
void *mapper_next _args ((Mapper *mapper));
const Mapping *mapper_next_mapping _args ((Mapper *mapper));
void mapper_remove _args ((Mapper *mapper));
int map_has_next _args ((Map *map));
void map_break _args ((Map *map));
void *map_next _args ((Map *map));
const Mapping *map_next_mapping _args ((Map *map));
void map_remove_current _args ((Map *map));
const void *mapping_key _args ((const Mapping *mapping));
const void *mapping_value _args ((const Mapping *mapping));
List *map_keys _args ((Map *map));
List *map_keys_unlocked _args ((Map *map));
List *map_keys_with_locker _args ((Locker *locker, Map *map));
List *map_keys_with_locker_unlocked _args ((Locker *locker, Map *map));
List *map_values _args ((Map *map));
List *map_values_unlocked _args ((Map *map));
List *map_values_with_locker _args ((Locker *locker, Map *map));
List *map_values_with_locker_unlocked _args ((Locker *locker, Map *map));
void map_apply _args ((Map *map, map_action_t *action, void *data));
void map_apply_rdlocked _args ((Map *map, map_action_t *action, void *data));
void map_apply_wrlocked _args ((Map *map, map_action_t *action, void *data));
void map_apply_unlocked _args ((Map *map, map_action_t *action, void *data));
ssize_t map_size _args ((Map *map));
ssize_t map_size_unlocked _args ((const Map *map));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
