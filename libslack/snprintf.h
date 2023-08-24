/*
* libslack - https://libslack.org
*
* Copyright (C) 1999-2004, 2010, 2020-2023 raf <raf@raf.org>
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
* along with this program; if not, see <https://www.gnu.org/licenses/>.
*
* 20230824 raf <raf@raf.org>
*/

#ifndef LIBSLACK_SNPRINTF_H
#define LIBSLACK_SNPRINTF_H

#include <stdlib.h>
#include <stdarg.h>

#include <slack/hdr.h>

_begin_decls
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list args);
_end_decls

#endif

/* vi:set ts=4 sw=4: */
