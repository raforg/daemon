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

#ifndef LIBSLACK_STR_H
#define LIBSLACK_STR_H

#include <stdarg.h>

#include <regex.h>

#include <slack/hdr.h>
#include <slack/list.h>

typedef struct String String;
typedef struct StrTR StrTR;

typedef enum
{
	ALIGN_LEFT       = '<',
	ALIGN_RIGHT      = '>',
	ALIGN_CENTRE     = '|',
	ALIGN_CENTER     = '|',
	ALIGN_FULL       = '='
}
StrAlignment;

typedef enum
{
	TR_COMPLEMENT = 1,
	TR_DELETE     = 2,
	TR_SQUASH     = 4
}
StrTROption;

#undef str_destroy
#undef tr_destroy

__START_DECLS
String *str_create __PROTO ((const char *fmt, ...));
String *str_vcreate __PROTO ((const char *fmt, va_list args));
String *str_create_sized __PROTO ((size_t size, const char *fmt, ...));
String *str_vcreate_sized __PROTO ((size_t size, const char *fmt, va_list args));
String *str_copy __PROTO ((const String *str));
void str_release __PROTO ((String *str));
#define str_destroy(str) str_destroy_func(&(str))
void *str_destroy_func __PROTO ((String **str));
int str_empty __PROTO ((const String *str));
size_t str_length __PROTO ((const String *str));
char *cstr __PROTO ((const String *str));
ssize_t str_set_length __PROTO ((String *str, size_t length));
ssize_t str_recalc_length __PROTO ((String *str));
String *str_clear __PROTO ((String *str));
String *str_remove __PROTO ((String *str, size_t index));
String *str_remove_range __PROTO ((String *str, size_t index, size_t range));
String *str_insert __PROTO ((String *str, size_t index, const char *fmt, ...));
String *str_vinsert __PROTO ((String *str, size_t index, const char *fmt, va_list args));
String *str_insert_str __PROTO ((String *str, size_t index, const String *src));
String *str_append __PROTO ((String *str, const char *fmt, ...));
String *str_vappend __PROTO ((String *str, const char *fmt, va_list args));
String *str_append_str __PROTO ((String *str, const String *src));
String *str_prepend __PROTO ((String *str, const char *fmt, ...));
String *str_vprepend __PROTO ((String *str, const char *fmt, va_list args));
String *str_prepend_str __PROTO ((String *str, const String *src));
String *str_replace __PROTO ((String *str, size_t index, size_t range, const char *fmt, ...));
String *str_vreplace __PROTO ((String *str, size_t index, size_t range, const char *fmt, va_list args));
String *str_replace_str __PROTO ((String *str, size_t index, size_t range, const String *src));
String *str_substr __PROTO ((const String *str, size_t index, size_t range));
String *substr __PROTO ((const char *str, size_t index, size_t range));
String *str_splice __PROTO ((String *str, size_t index, size_t range));
String *str_repeat __PROTO ((size_t count, const char *fmt, ...));
String *str_vrepeat __PROTO ((size_t count, const char *fmt, va_list args));
int str_tr __PROTO ((String *str, const char *from, const char *to, int option));
int str_tr_str __PROTO ((String *str, const String *from, const String *to, int option));
int tr __PROTO ((char *str, const char *from, const char *to, int option));
StrTR *str_tr_compile __PROTO ((const String *from, const String *to, int option));
StrTR *tr_compile __PROTO ((const char *from, const char *to, int option));
void tr_release __PROTO ((StrTR *tr));
#define tr_destroy(tr) tr_destroy_func(&(tr))
void *tr_destroy_func __PROTO ((StrTR **tr));
StrTR *str_tr_compile_table __PROTO ((StrTR *table, const String *from, const String *to, int option));
StrTR *tr_compile_table __PROTO ((StrTR *table, const char *from, const char *to, int option));
int str_tr_compiled __PROTO ((String *str, StrTR *table));
int tr_compiled __PROTO ((char *str, StrTR *table));
List *str_regex __PROTO ((const char *pattern, const String *text, int cflags, int eflags));
List *regex __PROTO ((const char *pattern, const char *text, int cflags, int eflags));
int regex_compile __PROTO ((regex_t *compiled, const char *pattern, int cflags));
void regex_release __PROTO ((regex_t *compiled));
List *str_regex_compiled __PROTO ((const regex_t *compiled, const String *text, int eflags));
List *regex_compiled __PROTO ((const regex_t *compiled, const char *text, int eflags));
String *str_regsub __PROTO ((const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all));
String *str_regsub_compiled __PROTO ((const regex_t *compiled, const char *replacement, String *text, int eflags, int all));
List *str_fmt __PROTO ((const String *str, size_t line_width, StrAlignment alignment));
List *fmt __PROTO ((const char *str, size_t line_width, StrAlignment alignment));
List *str_split __PROTO ((const String *str, const char *delim));
List *split __PROTO ((const char *str, const char *delim));
List *str_regex_split __PROTO ((const String *str, const char *delim));
List *regex_split __PROTO ((const char *str, const char *delim));
String *str_join __PROTO ((const List *list, const char *delim));
String *join __PROTO ((const List *list, const char *delim));
String *str_trim __PROTO ((String *str));
char *trim __PROTO ((char *str));
String *str_trim_left __PROTO ((String *str));
char *trim_left __PROTO ((char *str));
String *str_trim_right __PROTO ((String *str));
char *trim_right __PROTO ((char *str));
String *str_squeeze __PROTO ((String *str));
char *squeeze __PROTO ((char *str));
String *str_quote __PROTO ((const String *str, const char *quotable, char quote_char));
String *quote __PROTO ((const char *str, const char *quotable, char quote_char));
String *str_unquote __PROTO ((const String *str, const char *quotable, char quote_char));
String *unquote __PROTO ((const char *str, const char *quotable, char quote_char));
String *str_encode __PROTO ((const String *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *str_decode __PROTO ((const String *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *encode __PROTO ((const char *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *decode __PROTO ((const char *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *str_lc __PROTO ((String *str));
char *lc __PROTO ((char *str));
String *str_lcfirst __PROTO ((String *str));
char *lcfirst __PROTO ((char *str));
String *str_uc __PROTO ((String *str));
char *uc __PROTO ((char *str));
String *str_ucfirst __PROTO ((String *str));
char *ucfirst __PROTO ((char *str));
int str_chop __PROTO ((String *str));
int chop __PROTO ((char *str));
int str_chomp __PROTO ((String *str));
int chomp __PROTO ((char *str));
int str_bin __PROTO ((const String *bin));
int bin __PROTO ((const char *bin));
int str_hex __PROTO ((const String *hex));
int hex __PROTO ((const char *hex));
int str_oct __PROTO ((const String *oct));
int oct __PROTO ((const char *oct));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
