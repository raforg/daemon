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
* 20010215 raf <raf@raf.org>
*/

#ifndef LIBSLACK_STR_H
#define LIBSLACK_STR_H

#include <stdio.h>
#include <stdarg.h>

#include <regex.h>

#include <slack/hdr.h>
#include <slack/list.h>
#include <slack/thread.h>

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

_start_decls
String *str_create _args ((const char *fmt, ...));
String *str_create_locked _args ((Locker *locker, const char *fmt, ...));
String *str_vcreate _args ((const char *fmt, va_list args));
String *str_vcreate_locked _args ((Locker *locker, const char *fmt, va_list args));
String *str_create_sized _args ((size_t size, const char *fmt, ...));
String *str_create_locked_sized _args ((Locker *locker, size_t size, const char *fmt, ...));
String *str_vcreate_sized _args ((size_t size, const char *fmt, va_list args));
String *str_vcreate_locked_sized _args ((Locker *locker, size_t size, const char *fmt, va_list args));
String *str_copy _args ((const String *str));
String *str_copy_locked _args ((Locker *locker, const String *str));
String *str_fgetline _args ((FILE *stream));
String *str_fgetline_locked _args ((Locker *locker, FILE *stream));
void str_release _args ((String *str));
void *str_destroy _args ((String **str));
int str_rdlock _args ((const String *str));
int str_wrlock _args ((const String *str));
int str_unlock _args ((const String *str));
int str_empty _args ((const String *str));
size_t str_length _args ((const String *str));
char *cstr _args ((const String *str));
ssize_t str_set_length _args ((String *str, size_t length));
ssize_t str_set_length_unlocked _args ((String *str, size_t length));
ssize_t str_recalc_length _args ((String *str));
ssize_t str_recalc_length_unlocked _args ((String *str));
String *str_clear _args ((String *str));
String *str_remove _args ((String *str, ssize_t index));
String *str_remove_range _args ((String *str, ssize_t index, ssize_t range));
String *str_insert _args ((String *str, ssize_t index, const char *fmt, ...));
String *str_vinsert _args ((String *str, ssize_t index, const char *fmt, va_list args));
String *str_insert_str _args ((String *str, ssize_t index, const String *src));
String *str_append _args ((String *str, const char *fmt, ...));
String *str_vappend _args ((String *str, const char *fmt, va_list args));
String *str_append_str _args ((String *str, const String *src));
String *str_prepend _args ((String *str, const char *fmt, ...));
String *str_vprepend _args ((String *str, const char *fmt, va_list args));
String *str_prepend_str _args ((String *str, const String *src));
String *str_replace _args ((String *str, ssize_t index, ssize_t range, const char *fmt, ...));
String *str_vreplace _args ((String *str, ssize_t index, ssize_t range, const char *fmt, va_list args));
String *str_replace_str _args ((String *str, ssize_t index, ssize_t range, const String *src));
String *str_substr _args ((const String *str, ssize_t index, ssize_t range));
String *str_substr_locked _args ((Locker *locker, const String *str, ssize_t index, ssize_t range));
String *substr _args ((const char *str, ssize_t index, ssize_t range));
String *substr_locked _args ((Locker *locker, const char *str, ssize_t index, ssize_t range));
String *str_splice _args ((String *str, ssize_t index, ssize_t range));
String *str_splice_locked _args ((Locker *locker, String *str, ssize_t index, ssize_t range));
String *str_repeat _args ((size_t count, const char *fmt, ...));
String *str_repeat_locked _args ((Locker *locker, size_t count, const char *fmt, ...));
String *str_vrepeat _args ((size_t count, const char *fmt, va_list args));
String *str_vrepeat_locked _args ((Locker *locker, size_t count, const char *fmt, va_list args));
int str_tr _args ((String *str, const char *from, const char *to, int option));
int str_tr_str _args ((String *str, const String *from, const String *to, int option));
int tr _args ((char *str, const char *from, const char *to, int option));
StrTR *str_tr_compile _args ((const String *from, const String *to, int option));
StrTR *str_tr_compile_locked _args ((Locker *locker, const String *from, const String *to, int option));
StrTR *tr_compile _args ((const char *from, const char *to, int option));
StrTR *tr_compile_locked _args ((Locker *locker, const char *from, const char *to, int option));
void tr_release _args ((StrTR *tr));
void *tr_destroy _args ((StrTR **tr));
int str_tr_compiled _args ((String *str, StrTR *table));
int tr_compiled _args ((char *str, StrTR *table));
List *str_regexpr _args ((const char *pattern, const String *text, int cflags, int eflags));
List *str_regexpr_locked _args ((Locker *locker, const char *pattern, const String *text, int cflags, int eflags));
List *regexpr _args ((const char *pattern, const char *text, int cflags, int eflags));
List *regexpr_locked _args ((Locker *locker, const char *pattern, const char *text, int cflags, int eflags));
int regexpr_compile _args ((regex_t *compiled, const char *pattern, int cflags));
void regexpr_release _args ((regex_t *compiled));
List *str_regexpr_compiled _args ((const regex_t *compiled, const String *text, int eflags));
List *str_regexpr_compiled_locked _args ((Locker *locker, const regex_t *compiled, const String *text, int eflags));
List *regexpr_compiled _args ((const regex_t *compiled, const char *text, int eflags));
List *regexpr_compiled_locked _args ((Locker *locker, const regex_t *compiled, const char *text, int eflags));
String *str_regsub _args ((const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all));
String *str_regsub_compiled _args ((const regex_t *compiled, const char *replacement, String *text, int eflags, int all));
List *str_fmt _args ((const String *str, size_t line_width, StrAlignment alignment));
List *str_fmt_locked _args ((Locker *locker, const String *str, size_t line_width, StrAlignment alignment));
List *fmt _args ((const char *str, size_t line_width, StrAlignment alignment));
List *fmt_locked _args ((Locker *locker, const char *str, size_t line_width, StrAlignment alignment));
List *str_split _args ((const String *str, const char *delim));
List *str_split_locked _args ((Locker *locker, const String *str, const char *delim));
List *split _args ((const char *str, const char *delim));
List *split_locked _args ((Locker *locker, const char *str, const char *delim));
List *str_regexpr_split _args ((const String *str, const char *delim, int cflags, int eflags));
List *str_regexpr_split_locked _args ((Locker *locker, const String *str, const char *delim, int cflags, int eflags));
List *regexpr_split _args ((const char *str, const char *delim, int cflags, int eflags));
List *regexpr_split_locked _args ((Locker *locker, const char *str, const char *delim, int cflags, int eflags));
String *str_join _args ((const List *list, const char *delim));
String *str_join_locked _args ((Locker *locker, const List *list, const char *delim));
String *join _args ((const List *list, const char *delim));
String *join_locked _args ((Locker *locker, const List *list, const char *delim));
String *str_trim _args ((String *str));
char *trim _args ((char *str));
String *str_trim_left _args ((String *str));
char *trim_left _args ((char *str));
String *str_trim_right _args ((String *str));
char *trim_right _args ((char *str));
String *str_squeeze _args ((String *str));
char *squeeze _args ((char *str));
String *str_quote _args ((const String *str, const char *quotable, char quote_char));
String *str_quote_locked _args ((Locker *locker, const String *str, const char *quotable, char quote_char));
String *quote _args ((const char *str, const char *quotable, char quote_char));
String *quote_locked _args ((Locker *locker, const char *str, const char *quotable, char quote_char));
String *str_unquote _args ((const String *str, const char *quotable, char quote_char));
String *str_unquote_locked _args ((Locker *locker, const String *str, const char *quotable, char quote_char));
String *unquote _args ((const char *str, const char *quotable, char quote_char));
String *unquote_locked _args ((Locker *locker, const char *str, const char *quotable, char quote_char));
String *str_encode _args ((const String *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *str_encode_locked _args ((Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *str_decode _args ((const String *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *str_decode_locked _args ((Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *encode _args ((const char *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *encode_locked _args ((Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *decode _args ((const char *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *decode_locked _args ((Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable));
String *str_lc _args ((String *str));
char *lc _args ((char *str));
String *str_lcfirst _args ((String *str));
char *lcfirst _args ((char *str));
String *str_uc _args ((String *str));
char *uc _args ((char *str));
String *str_ucfirst _args ((String *str));
char *ucfirst _args ((char *str));
int str_chop _args ((String *str));
int chop _args ((char *str));
int str_chomp _args ((String *str));
int chomp _args ((char *str));
int str_bin _args ((const String *bin));
int bin _args ((const char *bin));
int str_hex _args ((const String *hex));
int hex _args ((const char *hex));
int str_oct _args ((const String *oct));
int oct _args ((const char *oct));
int strcasecmp _args ((const char *s1, const char *s2));
int strnasecmp _args ((const char *s1, const char *s2, size_t n));
size_t strlcpy _args ((char *dst, const char *src, size_t size));
size_t strlcat _args ((char *dst, const char *src, size_t size));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
