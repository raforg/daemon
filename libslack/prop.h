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

#ifndef LIBSLACK_PROP_H
#define LIBSLACK_PROP_H

#include <slack/hdr.h>

__START_DECLS
const char *prop_get __PROTO ((const char *name));
const char *prop_get_or __PROTO ((const char *name, const char *default_value));
const char *prop_set __PROTO ((const char *name, const char *value));
int prop_get_int __PROTO ((const char *name));
int prop_get_int_or __PROTO ((const char *name, int default_value));
int prop_set_int __PROTO ((const char *name, int value));
double prop_get_double __PROTO ((const char *name));
double prop_get_double_or __PROTO ((const char *name, double default_value));
double prop_set_double __PROTO ((const char *name, double value));
int prop_get_bool __PROTO ((const char *name));
int prop_get_bool_or __PROTO ((const char *name, int default_value));
int prop_set_bool __PROTO ((const char *name, int value));
int prop_unset __PROTO ((const char *name));
int prop_save __PROTO ((void));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
