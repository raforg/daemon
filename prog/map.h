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

#ifndef LIBPROG_MAP_H
#define LIBPROG_MAP_H

#include <sys/types.h>

#include <prog/hdr.h>
#include <prog/list.h>

typedef struct Map Map;
typedef struct Mapper Mapper;

#define map_destroy(map) map_destroy_func(&(map))
#define mapper_destroy(mapper) mapper_destroy_func(&(mapper))

__BEGIN_DECLS
Map *map_create __PROTO ((list_destroy_t *destroy));
void map_release __PROTO ((Map *map));
void *map_destroy_func __PROTO ((Map **map));
int map_add __PROTO ((Map *map, const char *key, void *value));
int map_put __PROTO ((Map *map, const char *key, void *value));
int map_remove __PROTO ((Map *map, const char *key));
void *map_lookup __PROTO ((const Map *map, const char *key));
Mapper *mapper_create __PROTO ((const Map *map));
void mapper_release __PROTO ((Mapper *mapper));
void *mapper_destroy_func __PROTO ((Mapper **mapper));
int map_has_next __PROTO ((Mapper *mapper));
void *map_next __PROTO ((Mapper *mapper));
List *map_keys __PROTO ((const Map *map));
List *map_values __PROTO ((const Map *map));
__END_DECLS

#endif

/* vi:set ts=4 sw=4: */
