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

#ifndef LIBSLACK_CONF_H
#define LIBSLACK_CONF_H

#include <slack/hdr.h>

typedef void conf_parse_t(void *obj, const char *conf_path, char *line, size_t lineno);

__START_DECLS
void conf_skip_spaces __PROTO ((char **in));
void conf_skip_spaces_backwards __PROTO ((char **in, const char *start));
int conf_expect_word __PROTO ((char **in, const char *expect));
int conf_get_word __PROTO ((char **in, char *word, int length));
void *conf_parse __PROTO ((const char *conf_path, void *obj, conf_parse_t *parse_line));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
