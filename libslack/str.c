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

I<libslack(str)> - string module

=head1 SYNOPSIS

    #include <slack/str.h>

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

    String *str_create(const char *fmt, ...);
    String *str_vcreate(const char *fmt, va_list args);
    String *str_create_sized(size_t size, const char *fmt, ...);
    String *str_vcreate_sized(size_t size, const char *fmt, va_list args);
    String *str_copy(const String *str);
    void str_release(String *str);
    #define str_destroy(str) str_destroy_func(&(str)
    void *str_destroy_func(String **str);
    int str_empty(const String *str);
    size_t str_length(const String *str);
    char *cstr(const String *str);
    ssize_t str_set_length(String *str, size_t length);
    ssize_t str_recalc_length(String *str);
    String *str_clear(String *str);
    String *str_remove(String *str, size_t index);
    String *str_remove_range(String *str, size_t index, size_t range);
    String *str_insert(String *str, size_t index, const char *fmt, ...);
    String *str_vinsert(String *str, size_t index, const char *fmt, va_list args);
    String *str_insert_str(String *str, size_t index, const String *src);
    String *str_append(String *str, const char *fmt, ...);
    String *str_vappend(String *str, const char *fmt, va_list args);
    String *str_append_str(String *str, const String *src);
    String *str_prepend(String *str, const char *fmt, ...);
    String *str_vprepend(String *str, const char *fmt, va_list args);
    String *str_prepend_str(String *str, const String *src);
    String *str_replace(String *str, size_t index, size_t range, const char *fmt, ...);
    String *str_vreplace(String *str, size_t index, size_t range, const char *fmt, va_list args);
    String *str_replace_str(String *str, size_t index, size_t range, const String *src);
    String *str_substr(const String *str, size_t index, size_t range);
    String *substr(const char *str, size_t index, size_t range);
    String *str_splice(String *str, size_t index, size_t range);
    String *str_repeat(size_t count, const char *fmt, ...);
    String *str_vrepeat(size_t count, const char *fmt, va_list args);
    int str_tr(String *str, const char *from, const char *to, int option);
    int str_tr_str(String *str, const String *from, const String *to, int option);
    int tr(char *str, const char *from, const char *to, int option);
    StrTR *str_tr_compile(const String *from, const String *to, int option);
    StrTR *tr_compile(const char *from, const char *to, int option);
    void tr_release(StrTR *tr);
    #define tr_destroy(tr) tr_destroy_func(&(tr)
    void *tr_destroy_func(StrTR **tr);
    StrTR *str_tr_compile_table(StrTR *table, const String *from, const String *to, int option);
    StrTR *tr_compile_table(StrTR *table, const char *from, const char *to, int option);
    int str_tr_compiled(String *str, StrTR *table);
    int tr_compiled(char *str, StrTR *table);
    List *str_regex(const char *pattern, const String *text, int cflags, int eflags);
    List *regex(const char *pattern, const char *text, int cflags, int eflags);
    int regex_compile(regex_t *compiled, const char *pattern, int cflags);
    void regex_release(regex_t *compiled);
    List *regex_compiled(const regex_t *compiled, const char *text, int eflags);
    String *str_regsub(const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all);
    String *str_regsub_compiled(const regex_t *compiled, const char *replacement, String *text, int eflags, int all);
    List *str_fmt(const String *str, size_t line_width, StrAlignment alignment);
    List *fmt(const char *str, size_t line_width, StrAlignment alignment);
    List *str_split(const String *str, const char *delim);
    List *split(const char *str, const char *delim);
    List *str_regex_split(const String *str, const char *delim);
    List *regex_split(const char *str, const char *delim);
    String *str_join(const List *str, const char *delim);
    String *join(const List *str, const char *delim);
    String *str_trim(String *str);
    char *trim(char *str);
    String *str_trim_left(String *str);
    char *trim_left(char *str);
    String *str_trim_right(String *str);
    char *trim_right(char *str);
    String *str_squeeze(String *str);
    char *squeeze(char *str);
    String *str_quote(const String *str, const char *quotable, char quote_char);
    String *quote(const char *str, const char *quotable, char quote_char);
    String *str_unquote(const String *str, const char *quotable, char quote_char);
    String *unquote(const char *str, const char *quotable, char quote_char);
    String *str_encode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *str_decode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *encode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *decode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *str_lc(String *str);
    char *lc(char *str);
    String *str_lcfirst(String *str);
    char *lcfirst(char *str);
    String *str_uc(String *str);
    char *uc(char *str);
    String *str_ucfirst(String *str);
    char *ucfirst(char *str);
    char str_chop(String *str);
    char chop(char *str);
    int str_chomp(String *str);
    int chomp(char *str);
    int str_bin(const String *bin);
    int bin(const char *bin);
    int str_hex(const String *hex);
    int hex(const char *hex);
    int str_oct(const String *oct);
    int oct(const char *oct);

=head1 DESCRIPTION

This module provides text strings that grow and shrink automatically and
functions for manipulating them. Some of the functions were modelled on the
I<list(3)> module. Others were modelled on the string functions and
operators in I<perlfunc(1)> and I<perlop(1)>.

=over 4

=cut

*/

#include "std.h"

#include "str.h"
#include "mem.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

/* Minimum string length: must be a power of 2 */

static const size_t MIN_STRING_SIZE = 32;

/* Maximum bytes for an empty string: must be a power of 2 greater than MIN_STRING_SIZE */

static const size_t MIN_EMPTY_STRING_SIZE = 1024;

struct String
{
	size_t size;             /* number of bytes allocated */
	size_t length;           /* number of bytes used (including nul) */
	char *str;               /* vector of characters */
};

#define CHARSET 256

struct StrTR
{
	int squash;
	short table[CHARSET];
};

typedef enum
{
	TRCODE_NOMAP = -1,
	TRCODE_DELETE = -2
}
TRCode;

/*

C<int grow(String *str, size_t bytes)>

Allocates enough memory to add C<bytes> extra bytes to C<str> if necessary.
On success, returns 0. On error, returns -1.

*/

static int grow(String *str, size_t bytes)
{
	int grown = 0;

	while (str->length + bytes > str->size)
	{
		if (str->size)
			str->size <<= 1;
		else
			str->size = MIN_STRING_SIZE;

		grown = 1;
	}

	if (grown)
		return mem_resize(str->str, str->size) ? 0 : -1;

	return 0;
}

/*

C<int shrink(String *str, size_t bytes)>

Allocates less memory for removing C<bytes> bytes from C<str> if necessary.
On success, returns 0. On error, returns -1.

*/

static int shrink(String *str, size_t bytes)
{
	int shrunk = 0;

	while (str->length - bytes < str->size >> 1)
	{
		if (str->size <= MIN_EMPTY_STRING_SIZE)
			break;

		str->size >>= 1;
		shrunk = 1;
	}

	if (shrunk)
		return mem_resize(str->str, str->size) ? 0 : -1;

	return 0;
}

/*

C<int expand(String *str, ssize_t index, size_t range)>

Slides C<str>'s bytes, starting at C<index>, C<range> positions to the right to
make room for more. On success, returns 0. On error, returns -1.

*/

static int expand(String *str, ssize_t index, size_t range)
{
	if (grow(str, range) == -1)
		return -1;

	memmove(str->str + index + range, str->str + index, (str->length - index) * sizeof(*str->str));
	str->length += range;

	return 0;
}

/*

C<int contract(String *str, ssize_t index, size_t range)>

Slides C<str>'s bytes, starting at C<index> + C<range>, C<range> positions
to the left to close a gap starting at C<index>. On success, returns 0. On
error, returns -1.

*/

static int contract(String *str, ssize_t index, size_t range)
{
	memmove(str->str + index, str->str + index + range, (str->length - index) * sizeof(*str->str));

	if (shrink(str, range) == -1)
		return -1;

	str->length -= range;

	return 0;
}

/*

C<int adjust(String *str, ssize_t index, size_t range, size_t length)>

Expands or contracts C<str> as required so that I<str[index + range ..]>
occupies I<str[index + length ..]>. On success, returns 0. On error,
returns -1.

*/

static int adjust(String *str, ssize_t index, size_t range, size_t length)
{
	if (range < length)
		return expand(str, index + range, length - range);

	if (range > length)
		return contract(str, index + length, range - length);

	return 0;
}

/*

=item C<String *str_create(const char *fmt, ...)>

Creates a I<String> specified by C<fmt> and the following arguments as in
I<sprintf(3)>. On success, returns the new string. It is the caller's
responsibility to deallocate the new string with I<str_release()> or
I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_create(const char *fmt, ...)
{
	String *str;
	va_list args;
	va_start(args, fmt);
	str = str_vcreate_sized(MIN_STRING_SIZE, fmt, args);
	va_end(args);
	return str;
}

/*

=item C<String *str_vcreate(const char *fmt, va_list args)>

Equivalent to I<str_create()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vcreate(const char *fmt, va_list args)
{
	return str_vcreate_sized(MIN_STRING_SIZE, fmt, args);
}


/*

=item C<String *str_create_sized(size_t size, const char *fmt, ...)>

Creates a I<String> specified by C<fmt> and the following arguments as in
I<sprintf(3)>. The initial allocation for the string data is at least
C<size> bytes. On success, returns the new string. It is the caller's
responsibility to deallocate the new string with I<str_release()> or
I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_create_sized(size_t size, const char *fmt, ...)
{
	String *str;
	va_list args;
	va_start(args, fmt);
	str = str_vcreate_sized(size, fmt, args);
	va_end(args);
	return str;
}

/*

=item C<String *str_vcreate_sized(size_t size, const char *fmt, va_list args)>

Equivalent to I<str_create_sized()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vcreate_sized(size_t size, const char *fmt, va_list args)
{
	String *str;
	char *buf = NULL;
	ssize_t length;
	unsigned int bit;

	for (bit = 1; bit; bit <<= 1)
	{
		if (bit >= size)
		{
			size = bit;
			break;
		}
	}

	if (!bit)
		return NULL;

	if (!fmt)
		fmt = "";

	for (;; size <<= 1)
	{
		if (!mem_resize(buf, size))
		{
			mem_destroy(buf);
			return NULL;
		}

		length = vsnprintf(buf, size, fmt, args);
		if (length != -1 && length < size)
			break;
	}

	if (!(str = mem_new(String)))
	{
		mem_destroy(buf);
		return NULL;
	}

	str->size = size;
	str->length = length + 1;
	str->str = buf;
	return str;
}

/*

=item C<String *str_copy(const String *str)>

Creates a clone of C<str>. On success, returns the clone. It is the caller's
responsibility to deallocate the new string with I<str_release()> or
I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_copy(const String *str)
{
	if (!str)
		return NULL;

	return str_substr(str, 0, str->length - 1);
}

/*

=item C<void str_release(String *str)>

Releases (deallocates) C<str>.

=cut

*/

void str_release(String *str)
{
	if (!str)
		return;

	mem_release(str->str);
	mem_release(str);
}

/*

=item C< #define str_destroy(str)>

Destroys (deallocates and sets to C<NULL>) C<str>. Returns C<NULL>.

=item C<void *str_destroy_func(String **str)>

Destroys (deallocates and sets to C<NULL>) C<str>. Returns C<NULL>. This
function is exposed as an implementation side effect. Don't call it
directly. Call I<str_destroy()> instead.

=cut

*/

void *str_destroy_func(String **str)
{
	if (str && *str)
	{
		str_release(*str);
		*str = NULL;
	}

	return NULL;
}

/*

=item C<int str_empty(const String *str)>

Returns whether or not C<str> is the empty string.

=cut

*/

int str_empty(const String *str)
{
	return !str || str->length == 1;
}

/*

=item C<size_t str_length(const String *str)>

Returns the length of C<str>.

=cut

*/

size_t str_length(const String *str)
{
	return (str) ? str->length - 1 : 0;
}

/*

=item C<char *cstr(const String *str)>

Returns the raw C string in C<str>. Do not use this pointer to extend the
length of the string. It's ok to use it to reduce the length of the string
provided you call I<str_set_length()> or I<str_recalc_length()> immediately
afterwards.

=cut

*/

char *cstr(const String *str)
{
	if (!str)
		return NULL;

	return str->str;
}

/*

=item C<ssize_t str_set_length(String *str, size_t length)>

Sets the length of C<str> to C<length>. Only needed after the raw C string
returned by I<cstr()> has been used to shorten a string. On success, returns
the length of C<str>. On error, returns -1.

=cut

*/

ssize_t str_set_length(String *str, size_t length)
{
	if (!str || length >= str->size || length >= str->length)
		return -1;

	++length;
	if (contract(str, length - 1, str->length - length) == -1)
		return -1;

	return str->length - 1;
}

/*

=item C<ssize_t str_recalc_length(String *str)>

Calculates and stores the length of C<str>. Only needed after the raw C
string returned by I<cstr()> has been used to shorten a string. Note: Treats
C<str> as a C<nul>-terminated string and should be avoided. On success,
returns the length of C<str>. On error, returns -1.

=cut

*/

ssize_t str_recalc_length(String *str)
{
	size_t length;

	if (!str)
		return -1;

	length = strlen(str->str) + 1;
	if (contract(str, str->length, str->length - length) == -1)
		return -1;

	return str->length - 1;
}

/*

=item C<String *str_clear(String *str)>

Makes C<str> the empty string. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

String *str_clear(String *str)
{
	return str_remove_range(str, 0, str_length(str));
}

/*

=item C<String *str_remove(String *str, size_t index)>

Removes the C<index>'th char from C<str>. On success, returns C<str>. On
error, returns C<NULL>.

=cut

*/

String *str_remove(String *str, size_t index)
{
	return str_remove_range(str, index, 1);
}

/*

=item C<String *str_remove_range(String *str, size_t index, size_t range)>

Removes C<range> characters from C<str> starting at C<index>. On success,
returns C<str>. On error, returns C<NULL>.

=cut

*/

String *str_remove_range(String *str, size_t index, size_t range)
{
	if (!str || str->length - 1 < index + range)
		return NULL;

	contract(str, index, range);

	return str;
}

/*

=item C<String *str_insert(String *str, size_t index, const char *fmt, ...)>

Adds the string specified by C<fmt> to C<str> at position C<index>. On
success, returns C<str>. On error, returns C<NULL>.

=cut

*/

String *str_insert(String *str, size_t index, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vinsert(str, index, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vinsert(String *str, size_t index, const char *fmt, va_list args)>

Equivalent to I<str_insert()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vinsert(String *str, size_t index, const char *fmt, va_list args)
{
	String *tmp, *ret;

	if (!str || str->length - 1 < index)
		return NULL;

	tmp = str_vcreate(fmt, args);
	if (!tmp)
		return NULL;

	ret = str_insert_str(str, index, tmp);
	str_release(tmp);

	return ret;
}

/*

=item C<String *str_insert_str(String *str, size_t index, const String *src)>

Inserts C<src> into C<str>, starting at position C<index>. On success,
returns C<str>. On error, returns C<NULL>.

=cut

*/

String *str_insert_str(String *str, size_t index, const String *src)
{
	size_t length;

	if (!src || !str || str->length - 1 < index)
		return NULL;

	length = src->length - 1;
	if (expand(str, index, length) == -1)
		return NULL;

	memcpy(str->str + index, src->str, length);

	return str;
}

/*

=item C<String *str_append(String *str, const char *fmt, ...)>

Appends the string specified by C<fmt> to C<str>. On success, returns
C<str>. On error, returns C<NULL>.

=cut

*/

String *str_append(String *str, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vinsert(str, str_length(str), fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vappend(String *str, const char *fmt, va_list args)>

Equivalent to I<str_append()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vappend(String *str, const char *fmt, va_list args)
{
	return str_vinsert(str, str_length(str), fmt, args);
}

/*

=item C<String *str_append_str(String *str, const String *src)>

Appends C<src> to C<str>. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

String *str_append_str(String *str, const String *src)
{
	return str_insert_str(str, str_length(str), src);
}

/*

=item C<String *str_prepend(String *str, const char *fmt, ...)>

Prepends the string specified by C<fmt> to C<str>. On success, returns
C<str>. On error, returns C<NULL>.

=cut

*/

String *str_prepend(String *str, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vinsert(str, 0, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vprepend(String *str, const char *fmt, va_list args)>

Equivalent to I<str_prepend()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vprepend(String *str, const char *fmt, va_list args)
{
	return str_vinsert(str, 0, fmt, args);
}

/*

=item C<String *str_prepend_str(String *str, const String *src)>

Prepends C<src> to C<str>. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

String *str_prepend_str(String *str, const String *src)
{
	return str_insert_str(str, 0, src);
}

/*

=item C<String *str_replace(String *str, size_t index, size_t range, const char *fmt, ...)>

Replaces C<range> characters in C<str>, starting at C<index>, with the
string specified by C<fmt>. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

String *str_replace(String *str, size_t index, size_t range, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vreplace(str, index, range, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vreplace(String *str, size_t index, size_t range, const char *fmt, va_list args)>

Equivalent to I<str_prepend()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vreplace(String *str, size_t index, size_t range, const char *fmt, va_list args)
{
	String *tmp, *ret;

	if (!str || str->length - 1 < index + range)
		return NULL;

	tmp = str_vcreate(fmt, args);
	if (!tmp)
		return NULL;

	ret = str_replace_str(str, index, range, tmp);
	str_release(tmp);

	return ret;
}

/*

=item C<String *str_replace_str(String *str, size_t index, size_t range, const String *src)>

Replaces C<range> characters in C<str>, starting at C<index>, with C<src>.
On success, return C<str>. On error, returns C<NULL>.

=cut

*/

String *str_replace_str(String *str, size_t index, size_t range, const String *src)
{
	size_t length;

	if (!src || !str || str->length - 1 < index + range)
		return NULL;

	length = src->length - 1;
	if (adjust(str, index, range, length) == -1)
		return NULL;

	memcpy(str->str + index, src->str, length);

	return str;
}

/*

=item C<String *str_substr(const String *str, size_t index, size_t range)>

Creates a new I<String> object consisting of C<range> characters from
C<str>, starting at C<index>. On success, returns the new string. It is the
caller's responsibility to deallocate the new string with I<str_release()>
or I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_substr(const String *str, size_t index, size_t range)
{
	String *ret;

	if (!str || str->length - 1 < index + range)
		return NULL;

	if (!(ret = str_create_sized(str->size, NULL)))
		return NULL;

	memcpy(ret->str, str->str + index, range);
	ret->length = range + 1;
	ret->str[ret->length - 1] = '\0';

	return ret;
}

/*

=item C<String *substr(const char *str, size_t index, size_t range)>

Equivalent to I<str_substr()> but works on an ordinary C string. It is the
caller's responsibility to ensure that C<str> points to at least C<index +
range> bytes.

=cut

*/

String *substr(const char *str, size_t index, size_t range)
{
	String *ret;

	if (!str)
		return NULL;

	if (!(ret = str_create_sized(index + range + 1, NULL)))
		return NULL;

	memcpy(ret->str, str + index, range);
	ret->length = range + 1;
	ret->str[ret->length - 1] = '\0';

	return ret;
}

/*

=item C<String *str_splice(String *str, size_t index, size_t range)>

Removes a substring from C<str> starting at C<index> of length C<range>
characters. On success, returns the substring. It is the caller's
responsibility to deallocate the new substring with I<str_release()> or
I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_splice(String *str, size_t index, size_t range)
{
	String *ret;

	if (!str || str->length - 1 < index + range)
		return NULL;

	ret = str_substr(str, index, range);
	if (!ret)
		return NULL;

	if (!str_remove_range(str, index, range))
	{
		str_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<String *str_repeat(size_t count, const char *fmt, ...)>

Creates a new I<String> containing the string determined by C<fmt> repeated
C<count> times. On success, return the new string. It is the caller's
responsibility to deallocate the new string with I<str_release()> or
I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_repeat(size_t count, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vrepeat(count, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vrepeat(size_t count, const char *fmt, va_list args)>

Equivalent to I<str_repeat()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vrepeat(size_t count, const char *fmt, va_list args)
{
	String *tmp, *ret;
	size_t i;

	tmp = str_vcreate(fmt, args);
	if (!tmp)
		return NULL;

	ret = str_create_sized(str_length(tmp) * count + 1, NULL);
	if (!ret)
	{
		str_release(tmp);
		return NULL;
	}

	for (i = 0; i < count; ++i)
	{
		if (!str_append_str(ret, tmp))
		{
			str_release(tmp);
			str_release(ret);
			return NULL;
		}
	}

	str_release(tmp);

	return ret;
}

/*

=item C<int str_tr(String *str, const char *from, const char *to, int option)>

This is just like the I<perl(1)> I<tr> operator. The following documentation
was taken from I<perlop(1)>.

Transliterates all occurrences of the characters in C<from> with the
corresponding character in C<to>. On success, returns the number of
characters replaced or deleted. On error, returns -1.

A character range may be specified with a hyphen, so C<str_tr(str, "A-J",
"0-9")> does the same replacement as C<str_tr(str, "ACEGIBDFHJ",
"0246813579")>.

Note also that the whole range idea is rather unportable between character
sets - and even within character sets they may cause results you probably
didn't expect. A sound principle is to use only ranges that begin from and
end at either alphabets of equal case (a-e, A-E), or digits (0-4). Anything
else is unsafe. If in doubt, spell out the character sets in full.

Options:

    TR_COMPLEMENT Complement from.
    TR_DELETE     Delete found but unreplaced characters.
    TR_SQUASH     Squash duplicate replaced characters.

If TR_COMPLEMENT is specified, C<from> is complemented. If TR_DELETE is
specified, any characters specified by C<from> not found in C<to> are
deleted. (Note that this is slightly more flexible than the behavior of some
tr programs, which delete anything they find in C<from>.) If TR_SQUASH is
specified, sequences of characters that were transliterated to the same
character are squashed down to a single instance of the character.

If TR_DELETE is used, C<to> is always interpreted exactly as specified.
Otherwise, if C<to> is shorter than C<from>, the final character is
replicated till it is long enough. If C<to> is empty or C<NULL>, C<from> is
replicated. This latter is useful for counting characters in a class or for
squashing character sequences in a class.

Examples:

    str_tr(s, "A-Z", "a-z", 0);             // canonicalize to lower case
    str_tr(s, "a-z", "A-Z", 0);             // canonicalize to upper case
    str_tr(s, "a-zA-Z", "A-Za-z", 0);       // swap upper and lower case
    str_tr(s, "*", "*", 0);                 // count the stars in str
    str_tr(s, "0-9", "", 0);                // count the digits in $_
    str_tr(s, "a-zA-Z", "", TR_SQUASH);     // bookkeeper -> bokeper
    str_tr(s, "a-zA-Z", " ", TR_COMPLEMENT | TR_SQUASH); // change non-alphas to single space
    str_tr(c, "a-zA-Z", "n-za-mN-ZA-M", 0); // Rot13

    from = str_create("\200-\377");
    to = str_create("%c-\177", '\000');
    str_tr_str(s, from, to, 0);             // clear 8th bit

If multiple transliterations are given for a character, only the first one
is used:

    str_tr(str, "AAA", "XYZ", 0);

will transliterate any A to X.

=cut

*/

int str_tr(String *str, const char *from, const char *to, int option)
{
	StrTR table[1];

	if (!str || !from)
		return -1;

	if (!tr_compile_table(table, from, to, option))
		return -1;

	return str_tr_compiled(str, table);
}

/*

=item C<int str_tr_str(String *str, const String *from, const String *to, int option)>

Equivalent to I<str_tr()> except that C<from> and C<to> are I<String>
objects. This is needed when C<from> or C<to> need to contain C<nul>
characters.

=cut

*/

int str_tr_str(String *str, const String *from, const String *to, int option)
{
	StrTR table[1];

	if (!str || !from)
		return -1;

	if (!str_tr_compile_table(table, from, to, option))
		return -1;

	return str_tr_compiled(str, table);
}

/*

=item C<int tr(char *str, const char *from, const char *to, int option)>

Equivalent to I<str_tr()> but works on an ordinary C string.

=cut

*/

int tr(char *str, const char *from, const char *to, int option)
{
	StrTR table[1];

	if (!str || !from)
		return -1;

	if (!tr_compile_table(table, from, to, option))
		return -1;

	return tr_compiled(str, table);
}

/*

=item C<StrTR *tr_compile(const char *from, const char *to, int option)>

Compiles C<from>, C<to> and C<option> into a translation table to be passed
to I<str_tr_compiled()> or I<tr_compiled()>. On success, returns the new
translation table. It is the caller's responsibility to deallocate the
translation table with I<tr_release()>, I<tr_destroy()> or I<free()>. On
error, returns C<NULL>.

=cut

*/

StrTR *tr_compile(const char *from, const char *to, int option)
{
	StrTR *ret;

	ret = mem_new(StrTR);
	if (!ret)
		return NULL;

	return tr_compile_table(ret, from, to, option);
}

/*

=item C<StrTR *str_tr_compile(const String *from, const String *to, int option)>

Equivalent to I<tr_compile()> except that C<from> and C<to> are I<String>
objects. This is needed when C<from> or C<to> need to contain C<nul>
characters.

=cut

*/

StrTR *str_tr_compile(const String *from, const String *to, int option)
{
	StrTR *ret;

	ret = mem_new(StrTR);
	if (!ret)
		return NULL;

	return str_tr_compile_table(ret, from, to, option);
}

/*

=item C<void tr_release(StrTR *tr)>

Releases (deallocates) C<tr>.

=cut

*/

void tr_release(StrTR *tr)
{
	if (!tr)
		return;

	mem_release(tr);
}

/*

=item C< #define tr_destroy(tr)>

Destroys (deallocates and sets to C<NULL>) C<tr>. Returns C<NULL>.

=item C<void *tr_destroy_func(StrTR **tr)>

Destroys (deallocates and sets to C<NULL>) C<tr>. Returns C<NULL>. This
function is exposed as an implementation side effect. Don't call it
directly. Call I<tr_destroy()> instead.

=cut

*/

void *tr_destroy_func(StrTR **tr)
{
	if (tr && *tr)
	{
		tr_release(*tr);
		*tr = NULL;
	}

	return NULL;
}

/*

C<static StrTR *do_tr_compile_table(StrTR *table, const char *from, ssize_t fromlen, const char *to, ssize_t tolen, int option)>

Compiles C<from>, C<to> and C<option> into the translation table, C<table>,
to be passed to I<str_tr_compiled()> or I<tr_compiled()>. If C<fromlen> is
C<-1>, then C<from> is interpreted as a C<nul>-terminated C string.
Otherwise, C<from> is an arbitrary string of length C<fromlen>. If C<tolen>
is C<-1>, then C<to> is interpreted as a C<nul>-terminated C string.
Otherwise, C<to> is an arbitrary string of length C<tolen>. On success,
returns C<table>. On error, returns C<NULL>.

*/

static StrTR *do_tr_compile_table(StrTR *table, const char *from, ssize_t fromlen, const char *to, ssize_t tolen, int option)
{
	const char *f, *t;
	char *xf, *xt;
	char xfrom[CHARSET], xto[CHARSET];
	short tbl[CHARSET];
	int i, j, k;

	if (!table || !from)
		return NULL;

	for (i = 0; i < CHARSET; ++i)
		tbl[i] = TRCODE_NOMAP;

	/* Parse the from string */

	for (xf = xfrom, f = from; ((fromlen == -1) ? *f : (f - from < fromlen)) && xf - xfrom < CHARSET; ++f)
	{
		i = j = *f;

		if (f[1] == '-' && f[2])
			j = f[2], f += 2;

		if (j < i)
			return NULL;

		for (k = i; k <= j; ++k)
			*xf++ = tbl[k] = k;
	}

	if (xf - xfrom == CHARSET)
		return NULL;

	if (option & TR_COMPLEMENT)
	{
		char tmp[CHARSET];

		for (xf = tmp, k = 0; k < CHARSET; ++k)
			if (tbl[k] == TRCODE_NOMAP)
				*xf++ = k;

		memcpy(xfrom, tmp, xf - tmp);
		xf = xfrom + (xf - tmp);
	}

	/* Parse the to string */

	if (!to || ((tolen == -1) ? *to == '\0' : tolen == 0))
		to = (option & TR_DELETE) ? "" : from;

	for (xt = xto, t = to; ((tolen == -1) ? *t : (t - to < tolen)) && xt - xto < CHARSET; ++t)
	{
		i = j = *t;

		if (t[1] == '-' && t[2])
			j = t[2], t += 2;

		if (j < i)
			return NULL;

		for (k = i; k <= j; ++k)
			*xt++ = k;
	}

	if (xt - xto == CHARSET)
		return NULL;

	if (!(option & TR_DELETE))
	{
		size_t flen = xf - xfrom;
		size_t tlen = xt - xto;

		if (tlen < flen)
		{
			memset(xt, xt[-1], flen - tlen);
			xt += flen - tlen;
		}
	}

	/* Build the translation table */

	table->squash = option & TR_SQUASH;
	for (i = 0; i < CHARSET; ++i)
		table->table[i] = TRCODE_NOMAP;

	for (i = j = 0; xfrom + i < xf; ++i, ++j)
	{
		unsigned char fc = xfrom[i];
		unsigned char tc = xto[j];
		if (table->table[fc] == TRCODE_NOMAP)
			table->table[fc] = (xto + j < xt) ? tc : TRCODE_DELETE;
	}

	return table;
}

/*

=item C<StrTR *tr_compile_table(StrTR *table, const char *from, const char *to, int option)>

Equivalent to I<tr_compile()> except that C<from>, C<to> and C<option> are
compiled into the translation table pointed to by C<table>. On success,
returns C<table>. On error, returns C<NULL>.

=cut

*/

StrTR *tr_compile_table(StrTR *table, const char *from, const char *to, int option)
{
	if (!table || !from)
		return NULL;

	return do_tr_compile_table(table, from, -1, to, -1, option);
}

/*

=item C<StrTR *str_tr_compile_table(StrTR *table, const String *from, const String *to, int option)>

Equivalent to I<tr_compile_table()> except that C<from> and C<to> are
I<String> objects. This is needed when C<from> or C<to> need to contain
C<nul> characters.

=cut

*/

StrTR *str_tr_compile_table(StrTR *table, const String *from, const String *to, int option)
{
	if (!table || !from)
		return NULL;

	return do_tr_compile_table(table, from->str, str_length(from), to->str, str_length(to), option);
}

/*

C<static int do_tr_compiled(unsigned char *str, size_t *length, StrTR *table)>

Performs the character translation specified by C<table> (as created by
I<tr_compile()> or equivalent) on C<str>. If C<length> is C<NULL>, C<str> is
interpreted as a C<nul>-terminated C string. Otherwise, C<str> is
interpreted as an arbitrary string of length C<*length>. The integer that
C<length> points to is decremented by the number of bytes deleted by the
translation. On success, returns the number of characters replaced or
deleted. On error, returns -1.

=cut

*/

static int do_tr_compiled(unsigned char *str, size_t *length, StrTR *table)
{
	int ret = 0;
	int deleted = 0;
	unsigned char *r, *s;
	short t;

	if (!str || !table)
		return -1;

	for (r = s = str; (length) ? s - str < *length - 1 : *s; ++s)
	{
		switch (t = table->table[(int)*s])
		{
			case TRCODE_DELETE:
				++deleted;
				++ret;
				break;

			case TRCODE_NOMAP:
				if (!table->squash || r == str || r[-1] != *s)
					*r++ = *s;
				else
					++deleted;
				break;

			default:
				if (!table->squash || r == str || r[-1] != t)
					*r++ = t;
				else
					++deleted;
				++ret;
				break;
		}
	}

	*r = '\0';
	if (length && deleted)
		*length -= deleted;

	return ret;
}

/*

=item C<int str_tr_compiled(String *str, StrTR *table)>

Performs the character translation specified by C<table> (as created by
I<tr_compile()> or equivalent) on C<str>. Use this whenever the same
translation will be performed multiple times. On success, returns the number
of characters replaced or deleted. On error, returns -1.

=cut

*/

int str_tr_compiled(String *str, StrTR *table)
{
	if (!str || !table)
		return -1;

	return do_tr_compiled((unsigned char *)str->str, &str->length, table);
}

/*

=item C<int tr_compiled(char *str, StrTR *table)>

Equivalent to I<str_tr_compiled()> but works on an ordinary C string.

=cut

*/

int tr_compiled(char *str, StrTR *table)
{
	if (!str || !table)
		return -1;

	return do_tr_compiled((unsigned char *)str, NULL, table);
}

#ifndef REGEX_MISSING

/*

=item C<List *str_regex(const char *pattern, const String *text, int cflags, int eflags)>

I<str_regex()> is an interface to POSIX 1003.2 compliant regular expression
matching. C<pattern> is a regular expression. C<text> is the string to be
searched for matches. C<cflags> is passed to I<regcomp(3)> along with
C<REG_EXTENDED | REG_NEWLINE>. C<eflags> is passed to I<regexec(3)>. On
success, returns a I<List> of (at most 33) I<String>s containing the
matching substring followed by the matching substrings of any parenthesised
subexpressions. It is the caller's responsibility to deallocate the list
with C<list_release()> or C<list_destroy()>. On error (including no match),
returns C<NULL>. Only use this function when the regular expression will be
used only once. Otherwise, use I<regex_compile()> or I<regcomp(3)> and
I<str_regex_compiled()> or I<regex_compiled()> or I<regexec(3)>.

Note: If you require perl pattern matching, you could use Philip Hazel's
I<PCRE> package, C<ftp://ftp.cus.cam.ac.uk/pub/software/programs/pcre/> or
link against the perl library itself.

=cut

*/

List *str_regex(const char *pattern, const String *text, int cflags, int eflags)
{
	if (!pattern || !text)
		return NULL;

	return regex(pattern, text->str, cflags, eflags);
}

/*

=item C<List *regex(const char *pattern, const char *text, int cflags, int eflags)>

Equivalent to I<str_regex()> but works on an ordinary C string.

=cut

*/

List *regex(const char *pattern, const char *text, int cflags, int eflags)
{
	regex_t compiled[1];
	List *ret;

	if (!pattern || !text)
		return NULL;

	if (regex_compile(compiled, pattern, cflags))
		return NULL;

	ret = regex_compiled(compiled, text, eflags);
	regfree(compiled);

	return ret;
}

/*

=item C<int regex_compile(regex_t *compiled, const char *pattern, int cflags)>

Compiles a POSIX 1003.2 compliant regular expression. C<compiled> is the
location in which to compile the expression. C<pattern> is the regular
expression. C<cflags> is passed to I<regcomp(3)> along with C<REG_EXTENDED>
and C<REG_NEWLINE>. Call this, followed by I<re_compiled()> when the regular
expression will be used multiple times.

=cut

*/

int regex_compile(regex_t *compiled, const char *pattern, int cflags)
{
	if (!compiled || !pattern)
		return REG_BADPAT;

	return regcomp(compiled, pattern, cflags | REG_EXTENDED | REG_NEWLINE);
}

/*

=item C<void regex_release(regex_t *compiled)>

Just another name for I<regfree(3)>.

=cut

*/

void regex_release(regex_t *compiled)
{
	if (compiled)
		regfree(compiled);
}

/*

=item C<List *str_regex_compiled(const regex_t *compiled, String char *text, int eflags)>

I<regex_compiled()> is an interface to the POSIX 1003.2 regular expression
function, I<regexec(3)>. C<compiled> is the compiled regular expression
prepared by I<regex_compile()> or I<regcomp(3)>. C<text> is the string to be
searched for a match. C<eflags> is passed to I<regexec(3)>. On success, returns
a I<List> of (at most 33) I<String>s containing the matching substring followed
by the matching substrings of any parenthesised subexpressions. It is the
caller's responsibility to deallocate the list with C<list_release()> or
C<list_destroy()>. On error (including no match), returns C<NULL>.

=cut

*/

List *str_regex_compiled(const regex_t *compiled, const String *text, int eflags)
{
	if (!compiled || !text)
		return NULL;

	return regex_compiled(compiled, text->str, eflags);
}

/*

=item C<List *regex_compiled(const regex_t *compiled, const char *text, int eflags)>

Equivalent to I<str_regex_compiled()> but works on an ordinary C string.

=cut

*/

List *regex_compiled(const regex_t *compiled, const char *text, int eflags)
{
	regmatch_t match[33];
	List *ret;
	int i;

	if (!compiled || !text)
		return NULL;

	if (regexec(compiled, text, 33, match, eflags))
		return NULL;

	ret = list_create((list_destroy_t *)str_release);
	if (!ret)
		return NULL;

	for (i = 0; i < 33 && match[i].rm_so != -1; ++i)
	{
		String *m = substr(text, match[i].rm_so, match[i].rm_eo - match[i].rm_so);

		if (!m)
		{
			list_release(ret);
			return NULL;
		}

		if (!list_append(ret, m))
		{
			str_release(m);
			list_release(ret);
			return NULL;
		}
	}

	return ret;
}

/*

=item C<String *str_regsub(const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all)>

I<str_regsub()> is an interface to POSIX 1003.2 compliant regular expression
matching and substitution. C<pattern> is a regular expression. C<text> is
the string to be searched for matches. C<cflags> is passed to I<regcomp(3)>
along with C<REG_EXTENDED | REG_NEWLINE>. C<eflags> is passed to
I<regexec(3)>. C<all> specifies whether to substitute the first match (if
zero) or all matches (if non-zero). C<replacement> specifies the string that
replaces each match. If C<replacement> contains C<"$#"> or C<"${##}"> (where
C<"#"> is a decimal digit), the substring that matches the corresponding
subexpression is interpolated in its place. Up to 32 subexpressions are
supported. If C<replacement> contains C<"$$">, then C<"$"> is interpolated
in its place. C<eplacement> also understands the following I<perl(1)> quote
escape sequences:

    \l  lowercase next character
    \u  uppercase next character
    \L  lowercase until matching \E
    \U  uppercase until matching \E
    \Q  backslash non-alphanumeric characters until matching \E
    \E  end case/quotemeta modification

Note that these sequences don't behave exactly like in I<perl(1)>. Namely,
an C<\l> appearing between a C<\U> and an C<\E> does lowercase the next
character and a C<\E> sequence without a matching C<\L>, C<\U> or C<\Q> is
an error. Also note that only 32 levels of nesting are supported.

On success, returns C<text>. On error (including no match), returns C<NULL>.
Only use this function when the regular expression will be used only once.
Otherwise, use I<regex_compile()> or I<regcomp(3)> and
I<str_regsub_compiled()>.

=cut

*/

String *str_regsub(const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all)
{
	regex_t compiled[1];
	String *ret;

	if (!pattern || !replacement || !text)
		return NULL;

	if (regex_compile(compiled, pattern, cflags))
		return NULL;

	ret = str_regsub_compiled(compiled, replacement, text, eflags, all);
	regfree(compiled);

	return ret;
}

/*

=item C<String *str_regsub_compiled(const regex_t *compiled, const char *replacement, String *text, int eflags, int all)>

Equivalent to I<str_regsub()> but works on an already compiled C<regex_t>,
C<compiled>.

=cut

*/

String *str_regsub_compiled(const regex_t *compiled, const char *replacement, String *text, int eflags, int all)
{
	enum
	{
		RS_LC = 1,
		RS_UC = 2,
		RS_QM = 4,
		RS_FIRST = 8,
		RS_LCFIRST = RS_LC | RS_FIRST,
		RS_UCFIRST = RS_UC | RS_FIRST
	};

	regmatch_t match[33];
	String *rep;
	int matches;
	size_t start;
	int states[33];
	int i, s;

	if (!compiled || !replacement || !text)
		return NULL;

	for (start = 0, matches = 0; start < text->length - 1; ++matches)
	{
		if (regexec(compiled, text->str + start, 33, match, eflags))
			return (matches) ? text : NULL;

		/*
		** Interpolate any $$, $# and ${##} in replacement
		** with subexpression matching substrings
		*/

		rep = str_create("%s", replacement);
		if (!rep)
			return NULL;

		for (i = 0; i < rep->length - 1; ++i)
		{
			if (rep->str[i] == '$')
			{
				if (rep->str[i + 1] == '$')
				{
					if (!str_remove(rep, i))
					{
						str_release(rep);
						return NULL;
					}
				}
				else
				{
					int ref;
					int j = i + 1;

					if (rep->str[j] == '{')
					{
						for (++j, ref = 0; isdigit((int)rep->str[j]); ++j)
							ref *= 10, ref += rep->str[j] - '0';

						if (rep->str[j] != '}')
						{
							str_release(rep);
							return NULL;
						}
					}
					else if (isdigit((int)rep->str[i + 1]))
					{
						ref = rep->str[j] - '0';
					}
					else
					{
						str_release(rep);
						return NULL;
					}

					if (ref < 0 || ref > 32 || match[ref].rm_so == -1)
					{
						str_release(rep);
						return NULL;
					}

					if (!str_replace(rep, i, j + 1 - i, "%.*s", match[ref].rm_eo - match[ref].rm_so, text->str + match[ref].rm_so))
					{
						str_release(rep);
						return NULL;
					}

					i += match[ref].rm_eo - match[ref].rm_so - 1;
				}
			}
		}

		/* Perform \l \L \u \U \Q \E transformations on replacement */

#define FAIL { str_release(rep); return NULL; }
#define PUSH_STATE(state) { if (s >= 32) FAIL states[s + 1] = states[s] | (state); ++s; }
#define POP_STATE { if (s == 0) FAIL --s; }
#define REMOVE_CODE { if (!str_remove_range(rep, i, 2)) FAIL --i; }
#define NEG(t) states[s] &= ~(RS_##t##C);

		for (states[s = 0] = 0, i = 0; i < rep->length - 1; ++i)
		{
			if (rep->str[i] == '\\')
			{
				switch (rep->str[i + 1])
				{
					case 'l': { PUSH_STATE(RS_LCFIRST) NEG(U) REMOVE_CODE break; } 
					case 'L': { PUSH_STATE(RS_LC) NEG(U) REMOVE_CODE break; } 
					case 'u': { PUSH_STATE(RS_UCFIRST) NEG(L) REMOVE_CODE break; } 
					case 'U': { PUSH_STATE(RS_UC) NEG(L) REMOVE_CODE break; } 
					case 'Q': { PUSH_STATE(RS_QM) REMOVE_CODE break; } 
					case 'E': { POP_STATE REMOVE_CODE break; } 
					case '\\': { if (!str_remove(rep, i)) FAIL break; }
				}
			}
			else
			{
				if (states[s] & RS_LC)
					rep->str[i] = tolower((int)rep->str[i]);

				if (states[s] & RS_UC)
					rep->str[i] = toupper((int)rep->str[i]);

				if (states[s] & RS_QM && !isalnum((int)rep->str[i]))
					if (!str_insert(rep, i++, "\\"))
						FAIL

				if (states[s] & RS_FIRST)
					POP_STATE
			}
		}

		/* Replace matching substring in text with rep */

		if (!str_replace_str(text, start + match[0].rm_so, match[0].rm_eo - match[0].rm_so, rep))
		{
			str_release(rep);
			return NULL;
		}

		start += match[0].rm_so + rep->length - 1;
		str_release(rep);

		if (!all)
			break;
	}

	return text;
}

#endif

/*

=item C<List *str_fmt(const String *str, size_t line_width, StrAlignment alignment)>

Formats C<str> into a I<List> of I<String> objects with length no greater
than C<line_width> (unless there are individual words longer than
C<line_width>) with the alignment specified by C<alignment>:

=over 4

=item C<ALIGN_LEFT> (C<`<'>)

The lines will be left justified (with one space between words).

=item C<ALIGN_RIGHT> (`>')

The lines will be right justified (with one space between words).

=item C<ALIGN_CENTRE> or C<ALIGN_CENTER> (C<`|'>)

C<str> will be split into lines at each newline character (C<`\n'>). The
lines will then be centred (with one space between words) padded with spaces
to the left.

=item C<ALIGN_FULL> (C<`='>)

The lines will be fully justified (possibly with multiple spaces between
words).

=back

On success, returns a new I<List> of I<String> objects. It is the caller's
responsibility to deallocate the list with C<list_release()> or
C<list_destroy()>. On error, returns C<NULL>. Note that C<str> is interpreted
as a C<nul>-terminated string.

=cut

*/

List *str_fmt(const String *str, size_t line_width, StrAlignment alignment)
{
	if (!str)
		return NULL;

	return fmt(str->str, line_width, alignment);
}

/*

=item C<List *fmt(const char *str, size_t line_width, StrAlignment alignment)>

Equivalent to I<str_fmt()> but works on an ordinary C string.

=cut

*/

List *fmt(const char *str, size_t line_width, StrAlignment alignment)
{
	List *para;
	String *line = NULL;
	const char *s, *r;
	size_t len;

	if (!str)
		return NULL;

	para = list_create((list_destroy_t *)str_release);
	if (!para)
		return NULL;

	switch (alignment)
	{
		case ALIGN_LEFT:
		case ALIGN_RIGHT:
		case ALIGN_FULL:
		{
			for (s = str; *s; ++s)
			{
				while (isspace((int)*s))
					++s;

				for (r = s; *r && !isspace((int)*r); ++r)
				{}

				if (r > s)
				{
					len = str_length(line);

					if (len + (len != 0) + (r - s) > line_width)
					{
						if (len && !list_append(para, line))
						{
							str_release(line);
							list_release(para);
							return NULL;
						}

						line = NULL;
					}

					if (!line)
					{
						line = str_create_sized(line_width, "%.*s", r - s, s);
						if (!line)
						{
							list_release(para);
							return NULL;
						}
					}
					else if (!str_append(line, " %.*s", r - s, s))
					{
						str_release(line);
						list_release(para);
						return NULL;
					}

					s = r;
					if (!*s)
						--s;
				}
			}

			if (str_length(line) && !list_append(para, line))
			{
				str_release(line);
				list_release(para);
				return NULL;
			}

			if (alignment == ALIGN_RIGHT)
			{
				while (list_has_next(para))
				{
					line = (String *)list_next(para);
					len = str_length(line);

					if (len >= line_width)
						continue;

					if (!str_prepend(line, "%*s", line_width - len, ""))
					{
						list_release(para);
						return NULL;
					}
				}
			}
			else if (alignment == ALIGN_FULL)
			{
				int i;
				for (i = 0; i < (int)list_length(para) - 1; ++i)
				{
					size_t extra;
					size_t gaps;

					line = (String *)list_item(para, i);
					len = str_length(line);

					if (len >= line_width)
						continue;

					extra = line_width - len;
					gaps = 0;

					for (s = line->str; *s; ++s)
						if (*s == ' ')
							++gaps;

					for (s = line->str; gaps && *s; ++s)
					{
						if (*s == ' ')
						{
							int gap = extra / gaps;

							if (!str_insert(line, s - line->str, "%*s", gap, ""))
							{
								list_release(para);
								return NULL;
							}

							extra -= gap;
							--gaps;
							s += gap;
						}
					}
				}
			}

			break;
		}

		case ALIGN_CENTRE:
		{
			para = split(str, "\n");

			while (list_has_next(para))
			{
				size_t extra;
				line = (String *)list_next(para);
				str_squeeze(line);
				len = str_length(line);

				if (len >= line_width)
					continue;
				extra = (line_width - len) / 2;
				if (extra && !str_prepend(line, "%*s", extra, ""))
				{
					list_release(para);
					return NULL;
				}
			}

			break;
		}

		default:
		{
			return NULL;
		}
	}

	return para;
}

/*

C<static List *do_split(const char *str, ssize_t length, const char *delim)>

Splits C<str> into tokens separated by sequences of characters occurring in
C<delim>. If C<length> is C<-1>, C<str> is interpreted as a
C<nul>-terminated C string. Otherwise, C<str> is interpreted as an arbitrary
string of length C<length>. On success, returns a new I<List> of I<String>
objects. It is the caller's responsibility to deallocate the list with
C<list_release()> or C<list_destroy()>. On error, returns C<NULL>.

*/

List *do_split(const char *str, ssize_t length, const char *delim)
{
	List *ret;
	const char *s, *r;

	if (!str || !delim)
		return NULL;

	ret = list_create((list_destroy_t *)str_release);
	if (!ret)
		return NULL;

	for (s = str; (length == -1) ? *s : s - str < length; ++s)
	{
		while ((length == -1) ? (*s && strchr(delim, *s)) : (s - str < length && (*s && strchr(delim, *s))))
			++s;

		if (!*delim)
			r = s + 1;
		else
			for (r = s; (length == -1) ? (*r && !strchr(delim, *r)) : (r - str < length && (!*r || !strchr(delim, *r))); ++r)
			{}

		if (r > s)
		{
			String *token = substr(s, 0, r - s);
			if (!token)
			{
				list_release(ret);
				return NULL;
			}

			if (!list_append(ret, token))
			{
				str_release(token);
				list_release(ret);
				return NULL;
			}

			s = r;
			if (!*delim)
				--s;
		}

		if ((length == -1) ? !*s : (s - str == length))
			break;
	}

	return ret;
}

/*

=item C<List *str_split(const String *str, const char *delim)>

Splits C<str> into tokens separated by sequences of characters occurring
in C<delim>. On success, returns a new I<List> of I<String> objects. It is
the caller's responsibility to deallocate the list with C<list_release()> or
C<list_destroy()>. On error, returns C<NULL>.

=cut

*/

List *str_split(const String *str, const char *delim)
{
	if (!str || !delim)
		return NULL;

	return do_split(str->str, str->length - 1, delim);
}

/*

=item C<List *split(const char *str, const char *delim)>

Equivalent to I<str_split()> but works on an ordinary C string.

=cut

*/

List *split(const char *str, const char *delim)
{
	if (!str || !delim)
		return NULL;

	return do_split(str, -1, delim);
}

#ifndef REGEX_MISSING

/*

=item C<List *str_regex_split(const String *str, const char *delim)>

Splits C<str> into tokens separated by occurrences of the regular expression,
C<delim>. C<str> is interpreted as a C<nul>-terminated C string. On success,
returns a new I<List> of I<String> objects. It is the caller's responsibility
to deallocate the list with C<list_release()> or C<list_destroy()>. On error,
returns C<NULL>.

=cut

*/

List *str_regex_split(const String *str, const char *delim)
{
	if (!str || !delim)
		return NULL;

	return regex_split(str->str, delim);
}

/*

=item C<List *regex_split(const char *str, const char *delim)>

Equivalent to I<str_regex_split()> but works on an ordinary C string.

=cut

*/

List *regex_split(const char *str, const char *delim)
{
	List *ret;
	String *token;
	regex_t compiled[1];
	regmatch_t match[1];
	int start, matches;

	if (!str || !delim)
		return NULL;

	if (regex_compile(compiled, delim, 0))
		return NULL;

	ret = list_create((list_destroy_t *)str_release);
	if (!ret)
		return NULL;

	for (start = 0, matches = 0; str[start]; ++matches)
	{
		if (regexec(compiled, str + start, 1, match, 0))
			break;

		/* Zero length match (at every position), make a token of each character */

		if (match[0].rm_so == 0 && match[0].rm_eo == 0)
		{
			++match[0].rm_so;
			++match[0].rm_eo;
		}

		/* Make a token of any text before the match */

		if (match[0].rm_so)
		{
			token = substr(str, start, match[0].rm_so);
			if (!token)
			{
				list_release(ret);
				return NULL;
			}

			if (!list_append(ret, token))
			{
				str_release(token);
				list_release(ret);
				return NULL;
			}
		}

		start += match[0].rm_eo;
	}

	/* Make a token of any text after the last match */

	if (str[start])
	{
		token = str_create("%s", str + start);
		if (!token)
		{
			list_release(ret);
			return NULL;
		}

		if (!list_append(ret, token))
		{
			str_release(token);
			list_release(ret);
			return NULL;
		}
	}

	return ret;
}

#endif

/*

=item C<String *str_join(const List *list, const char *delim)>

Joins the I<String> objects in C<list> with C<delim> inserted between each
one. On success, returns the resulting I<String>. It is the caller's
responsibility to deallocate the string with I<str_release()> or
I<str_destroy()>. On error, returns NULL.

=cut

*/

String *str_join(const List *list, const char *delim)
{
	String *ret;
	String *del;
	Lister *lister;
	int i;

	if (!list)
		return NULL;

	ret = str_create(NULL);
	if (!ret)
		return NULL;

	del = str_create(delim ? "%s" : NULL, delim);
	if (!del)
	{
		str_release(ret);
		return NULL;
	}

	lister = lister_create((List *)list);
	if (!lister)
	{
		str_release(ret);
		str_release(del);
		return NULL;
	}

	for (i = 0; lister_has_next(lister); ++i)
	{
		String *s = (String *)lister_next(lister);

		if (i && !str_append_str(ret, del))
		{
			str_release(ret);
			str_release(del);
			lister_release(lister);
			return NULL;
		}

		if (s && !str_append_str(ret, s))
		{
			str_release(ret);
			str_release(del);
			lister_release(lister);
			return NULL;
		}
	}

	str_release(del);
	lister_release(lister);

	return ret;
}

/*

=item C<String *join(const List *list, const char *delim)>

Equivalent to I<str_join()> but works on a list of ordinary C strings.

=cut

*/

String *join(const List *list, const char *delim)
{
	String *ret;
	String *del;
	Lister *lister;
	int i;

	if (!list)
		return NULL;

	ret = str_create(NULL);
	if (!ret)
		return NULL;

	del = str_create(delim ? "%s" : NULL, delim);
	if (!del)
	{
		str_release(ret);
		return NULL;
	}

	lister = lister_create((List *)list);
	if (!lister)
	{
		str_release(ret);
		str_release(del);
		return NULL;
	}

	for (i = 0; lister_has_next(lister); ++i)
	{
		char *s = (char *)lister_next(lister);

		if (i && !str_append_str(ret, del))
		{
			str_release(ret);
			str_release(del);
			lister_release(lister);
			return NULL;
		}

		if (s && !str_append(ret, "%s", s))
		{
			str_release(ret);
			str_release(del);
			lister_release(lister);
			return NULL;
		}
	}

	str_release(del);
	lister_release(lister);

	return ret;
}

/*

=item C<String *str_trim(String *str)>

Trims leading and trailing spaces from C<str>. On success, returns C<str>.
On error, returns C<NULL>.

=cut

*/

String *str_trim(String *str)
{
	char *s;

	if (!str || !str->str)
		return NULL;

	for (s = str->str; isspace((int)*s); ++s)
	{}

	if (s > str->str)
		if (!str_remove_range(str, 0, s - str->str))
			return NULL;

	for (s = str->str + str->length - 1; s > str->str && isspace((int)s[-1]); --s)
	{}

	if (isspace((int)*s))
		if (!str_remove_range(str, s - str->str, str->length - 1 - (s - str->str)))
			return NULL;

	return str;
}

/*

=item C<char *trim(char *str)>

Equivalent to I<str_trim()> but works on an ordinary C string.

=cut

*/

char *trim(char *str)
{
	char *s;
	size_t len;

	if (!str)
		return NULL;

	for (s = str; isspace((int)*s); ++s)
	{}

	len = strlen(s);

	if (s > str)
		memmove(str, s, len + 1);

	for (s = str + len; s > str && isspace((int)*--s); )
		*s = '\0';

	return str;
}

/*

=item C<String *str_trim_left(String *str)>

Trims leading spaces from C<str>. On success, returns C<str>. On error,
returns C<NULL>.

=cut

*/

String *str_trim_left(String *str)
{
	char *s;

	if (!str || !str->str)
		return NULL;

	for (s = str->str; isspace((int)*s); ++s)
	{}

	if (s > str->str)
		if (!str_remove_range(str, 0, s - str->str))
			return NULL;

	return str;
}

/*

=item C<char *trim_left(char *str)>

Equivalent to I<str_trim_left()> but works on an ordinary C string.

=cut

*/

char *trim_left(char *str)
{
	char *s;
	size_t len;

	if (!str)
		return NULL;

	for (s = str; isspace((int)*s); ++s)
	{}

	len = strlen(s);

	if (s > str)
		memmove(str, s, len + 1);

	return str;
}

/*

=item C<String *trim_right(String *str)>

Trims trailing spaces from C<str>. On success, returns C<str>. On error,
returns C<NULL>.

=cut

*/

String *str_trim_right(String *str)
{
	char *s;

	if (!str || !str->str)
		return NULL;

	for (s = str->str + str->length - 1; s > str->str && isspace((int)s[-1]); --s)
	{}

	if (isspace((int)*s))
		if (!str_remove_range(str, s - str->str, str->length - 1 - (s - str->str)))
			return NULL;

	return str;
}

/*

=item C<char *trim_right(char *str)>

Equivalent to I<str_trim_right()> but works on an ordinary C string.

=cut

*/

char *trim_right(char *str)
{
	char *s;
	size_t len;

	if (!str)
		return NULL;

	len = strlen(str);

	for (s = str + len; s > str && isspace((int)*--s); )
		*s = '\0';

	return str;
}

/*

=item C<String *str_squeeze(String *str)>

Trims leading and trailing spaces from C<str> and replaces all other
sequences of whitespace with a single space. On success, returns C<str>. On
error, returns C<NULL>.

=cut

*/

String *str_squeeze(String *str)
{
	char *s, *r;
	int started = 0;
	int was_space = 0;

	if (!str)
		return NULL;

	for (r = s = str->str; s - str->str < str->length - 1; ++s)
	{
		if (!isspace((int)*s))
		{
			if (was_space && started)
				*r++ = ' ';
			*r++ = *s;
			started = 1;
		}

		was_space = isspace((int)*s);
	}

	if (r - str->str < str->length)
		if (!str_remove_range(str, r - str->str, str->length - 1 - (r - str->str)))
			return NULL;

	return str;
}

/*

=item C<char *squeeze(char *str)>

Equivalent to I<str_squeeze()> but works on an ordinary C string.

=cut

*/

char *squeeze(char *str)
{
	char *s, *r;
	int started = 0;
	int was_space = 0;

	if (!str)
		return NULL;

	for (r = s = str; *s; ++s)
	{
		if (!isspace((int)*s))
		{
			if (was_space && started)
				*r++ = ' ';
			*r++ = *s;
			started = 1;
		}

		was_space = isspace((int)*s);
	}

	*r = '\0';

	return str;
}

/*

=item C<String *str_quote(const String *str, const char *quotable, char quote_char)>

Creates a new string containing C<str> with every occurrence of any
character in C<quotable> preceeded by C<quote_char>. On success, returns the
new string. It is the caller's responsibility to deallocate the new string
with I<str_release()> or I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_quote(const String *str, const char *quotable, char quote_char)
{
	String *ret;
	size_t i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_copy(str)))
		return NULL;

	for (i = 0; i < ret->length - 1; ++i)
		if (ret->str[i] && strchr(quotable, ret->str[i]))
			if (!str_insert(ret, i++, "%c", quote_char))
			{
				str_destroy(ret);
				return NULL;
			}

	return ret;
}

/*

=item C<String *quote(const char *str, const char *quotable, char quote_char)>

Equivalent to I<str_quote()> but works on an ordinary C string.

=cut

*/

String *quote(const char *str, const char *quotable, char quote_char)
{
	String *ret;
	size_t i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_create("%s", str)))
		return NULL;

	for (i = 0; i < ret->length - 1; ++i)
		if (strchr(quotable, ret->str[i]))
			if (!str_insert(ret, i++, "%c", quote_char))
			{
				str_destroy(ret);
				return NULL;
			}

	return ret;
}

/*

=item C<String *str_unquote(const String *str, const char *quotable, char quote_char)>

Creates a new string containing C<str> with every occurrence of
C<quote_char> that is followed by any character in C<quotable> removed. On
success, returns the new I<String>. It is the caller's responsibility to
deallocate the new string with I<str_release()> or I<str_destroy()>. On
error, returns C<NULL>.

=cut

*/

String *str_unquote(const String *str, const char *quotable, char quote_char)
{
	String *ret;
	int i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_copy(str)))
		return NULL;

	for (i = 0; i < (int)ret->length - 2; ++i)
		if (ret->str[i] == quote_char && ret->str[i + 1] && strchr(quotable, ret->str[i + 1]))
			if (!str_remove(ret, i))
			{
				str_destroy(ret);
				return NULL;
			}

	return ret;
}

/*

=item C<String *unquote(const char *str, const char *quotable, char quote_char)>

Equivalent to I<str_unquote()> but works on an ordinary C string.

=cut

*/

String *unquote(const char *str, const char *quotable, char quote_char)
{
	String *ret;
	int i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_create("%s", str)))
		return NULL;

	for (i = 0; i < (int)ret->length - 2; ++i)
		if (ret->str[i] == quote_char && strchr(quotable, ret->str[i + 1]))
			if (!str_remove(ret, i))
			{
				str_destroy(ret);
				return NULL;
			}

	return ret;
}

/*

C<static String *do_encode(const char *str, ssize_t length, const char *uncoded, const char *coded, char quote_char, int printable)>

Performs encoding as described in I<str_encode()>.

*/

static String *do_encode(const char *str, size_t length, const char *uncoded, const char *coded, char quote_char, int printable)
{
	static char hex[] = "0123456789abcdef";
	String *encoded;
	const char *target;
	const char *s;

	if (!str || !uncoded || !coded)
		return NULL;

	encoded = str_create_sized(length * 4 + 1, "");
	if (!encoded)
		return NULL;

	for (s = str; s - str < length; ++s)
	{
		if (*s && (target = strchr(uncoded, *s)))
		{
			if (!str_append(encoded, "%c%c", quote_char, coded[target - uncoded]))
			{
				str_release(encoded);
				return NULL;
			}
		}
		else if (printable && !isprint((int)*s))
		{
			if (!str_append(encoded, "%cx%c%c", quote_char, hex[(unsigned char)*s >> 4], hex[(unsigned char)*s & 0x0f]))
			{
				str_release(encoded);
				return NULL;
			}
		}
		else
		{
			if (!str_append(encoded, "%c", *s))
			{
				str_release(encoded);
				return NULL;
			}
		}
	}

	return encoded;
}

/*

C<static String *do_decode(const char *str, ssize_t length, const char *uncoded, const char *coded, char quote_char, int printable)>

Performs decoding as described in I<str_decode()>.

*/

static String *do_decode(const char *str, size_t length, const char *uncoded, const char *coded, char quote_char, int printable)
{
	String *decoded;
	const char *start;
	const char *slosh;
	char *target;

	if (!str || !uncoded || !coded)
		return NULL;

	decoded = str_create_sized(length + 1, "");
	if (!decoded)
		return NULL;

	for (start = str; start - str < length; start = slosh + 1)
	{
		for (slosh = start; slosh - str < length; ++slosh)
			if (*slosh == quote_char)
				break;

		if (slosh - str == length)
			break;

		if (printable)
		{
			int digits = 0;
			const char *s = slosh + 1;
			char c = '\0';

			if (isdigit((int)*s) && *s <= '7')
			{
				--s;

				do
				{
					++digits;
					c <<= 3, c |= *++s - '0';
				}
				while (digits < 3 && isdigit((int)s[1]) && s[1] <= '7');
			}
			else if (*s == 'x' && isxdigit((int)s[1]))
			{
				do
				{
					++digits;
					c <<= 4;

					switch (*++s)
					{
						case '0': case '1': case '2': case '3': case '4':
						case '5': case '6': case '7': case '8': case '9':
							c |= *s - '0';
							break;
						case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
							c |= *s - 'a' + 10;
							break;
						case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
							c |= *s - 'A' + 10;
							break;
					}
				}
				while (digits < 2 && isxdigit((int)s[1]));
			}

			if (digits)
			{
				if (!str_append(decoded, "%*.*s%c", slosh - start, slosh - start, start, c))
				{
					str_release(decoded);
					return NULL;
				}

				slosh = s; /* Skip over ASCII code */
				continue;
			}
		}

		if (!slosh[1] || !(target = strchr(coded, slosh[1])))
		{
			if (!str_append(decoded, "%*.*s%c", slosh - start, slosh - start, start, quote_char))
			{
				str_release(decoded);
				return NULL;
			}

			continue;
		}

		if (!str_append(decoded, "%*.*s%c", slosh - start, slosh - start, start, uncoded[target - coded]))
		{
			str_release(decoded);
			return NULL;
		}

		++slosh; /* Skip over quoted char */
	}

	if (!str_append(decoded, "%s", start))
	{
		str_release(decoded);
		return NULL;
	}

	return decoded;
}

/*

=item C<String *str_encode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Returns a copy of C<str> with every occurrance in C<str> of characters in
C<uncoded> replaced with C<quote_char> followed by the corresponding (by
position) character in C<coded>. If C<printable> is non-zero, other
non-printable characters are replaced with their ASCII codes in hexadecimal.
It is the caller's responsibility to deallocate the new string with
I<str_release()> or I<str_destroy()>. On error, returns C<NULL>.

Example:

    // Encode a string into a C string literal
    str_encode(str, "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1);

    // Decode a C string literal 
    str_decode(str, "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1);

=cut

*/

String *str_encode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	if (!str || !uncoded || !coded)
		return NULL;

	return do_encode(str->str, str->length - 1, uncoded, coded, quote_char, printable);
}

/*

=item C<String *str_decode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Returns a copy of C<str> with every occurrance in C<str> of C<quote_char>
followed by a character in C<coded> replaced with the corresponding (by
position) character in C<uncoded>. If C<printable> is non-zero, every
occurrance in C<str> of an ASCII code in octal or hexadecimal (i.e. "\ooo"
or "\xhh") is replaced with the corresponding ASCII character. It is the
caller's responsibility to deallocate the new string with I<str_release()>
or I<str_destroy()>. On error, returns C<NULL>.

=cut

*/

String *str_decode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	if (!str || !uncoded || !coded)
		return NULL;

	return do_decode(str->str, str->length - 1, uncoded, coded, quote_char, printable);
}

/*

=item C<String *encode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<str_encode()> but works on an ordinary C string.

=cut

*/

String *encode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	if (!str || !uncoded || !coded)
		return NULL;

	return do_encode(str, strlen(str), uncoded, coded, quote_char, printable);
}

/*

=item C<String *decode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<str_decode()> but works on an ordinary C string.

=cut

*/

String *decode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	if (!str || !uncoded || !coded)
		return NULL;

	return do_decode(str, strlen(str), uncoded, coded, quote_char, printable);
}

/*

=item C<String *str_lc(String *str)>

Converts C<str> into lower case. On success, returns C<str>. On error,
returns C<NULL>.

=cut

*/

String *str_lc(String *str)
{
	size_t i;

	if (!str)
		return NULL;

	for (i = 0; i < str->length - 1; ++i)
		str->str[i] = tolower((int)str->str[i]);

	return str;
}

/*

=item C<char *lc(char *str)>

Converts C<str> into lower case. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

char *lc(char *str)
{
	char *s;

	if (!str)
		return NULL;

	for (s = str; *s; ++s)
		*s = tolower((int)*s);

	return str;
}

/*

=item C<String *str_lcfirst(String *str)>

Converts the first character in C<str> into lower case. On success, returns
C<str>. On error, returns C<NULL>.

=cut

*/

String *str_lcfirst(String *str)
{
	if (!str)
		return NULL;

	if (str->length > 1)
		*str->str = tolower((int)*str->str);

	return str;
}

/*

=item C<char *lcfirst(char *str)>

Converts the first character in C<str> into lower case. On success, returns
C<str>. On error, returns C<NULL>.

=cut

*/

char *lcfirst(char *str)
{
	if (!str)
		return NULL;

	*str = tolower((int)*str);

	return str;
}

/*

=item C<String *str_uc(String *str)>

Converts C<str> into upper case. On success, returns C<str>. On error,
returns C<NULL>.

=cut

*/

String *str_uc(String *str)
{
	size_t i;

	if (!str)
		return NULL;

	for (i = 0; i < str->length - 1; ++i)
		str->str[i] = toupper((int)str->str[i]);

	return str;
}

/*

=item C<char *uc(char *str)>

Converts C<str> into upper case. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

char *uc(char *str)
{
	char *s;

	if (!str)
		return NULL;

	for (s = str; *s; ++s)
		*s = toupper((int)*s);

	return str;
}

/*

=item C<String *str_ucfirst(String *str)>

Converts the first character in C<str> into upper case. On success, returns
C<str>. On error, returns C<NULL>.

=cut

*/

String *str_ucfirst(String *str)
{
	if (!str)
		return NULL;

	if (str->length > 1)
		*str->str = toupper((int)*str->str);

	return str;
}

/*

=item C<char *ucfirst(char *str)>

Converts the first character in C<str> into upper case. On success, returns
C<str>. On error, returns C<NULL>.

=cut

*/

char *ucfirst(char *str)
{
	if (!str)
		return NULL;

	*str = toupper((int)*str);

	return str;
}

/*

=item C<int str_chop(String *str)>

Chops a character off the end of C<str>. On success, returns the character
chopped. On error, returns -1.

=cut

*/

int str_chop(String *str)
{
	int ret;

	if (!str || str->length == 1)
		return -1;

	ret = str->str[str->length - 2];
	if (contract(str, str->length - 2, 1) == -1)
		return -1;

	return ret;
}

/*

=item C<int chop(char *str)>

Chops a character off the end of C<str>. On success, returns the character
chopped. On error, returns -1.

=cut

*/

int chop(char *str)
{
	int ret;

	if (!str || *str == '\0')
		return -1;

	while (str[1])
		++str;

	ret = *str;
	*str = '\0';

	return ret;
}

/*

=item C<int str_chomp(String *str)>

Chops a newline off the end of C<str>. On success, returns the number of
characters chomped. On error, returns -1.

=cut

*/

int str_chomp(String *str)
{
	char *s;
	size_t length;

	if (!str)
		return -1;

	if (str->length == 1)
		return 0;

	length = str->length;
	for (s = str->str + str->length - 2; *s == '\n' || *s == '\r'; --s)
		if (contract(str, str->length - 2, 1) == -1)
			return -1;

	return length - str->length;
}

/*

=item C<int chomp(char *str)>

Chops a newline off the end of C<str>. On success, returns the number of
characters chomped. On error, returns -1.

=cut

*/

int chomp(char *str)
{
	char *s;

	if (!str)
		return -1;

	if (str[0] == '\0')
		return 0;

	while (str[1])
		++str;

	for (s = str; *s == '\n' || *s == '\r'; --s)
		*s = '\0';

	return str - s;
}

/*

=item C<int str_bin(const String *str)>

Returns the integer specified by the binary string, C<str>. C<str> may
either be a string of C<[0-1]> or C<"0b"> followed by a string of C<[0-1]>.
On error, returns 0.

=cut

*/

int str_bin(const String *str)
{
	if (!str)
		return 0;

	return bin(str->str);
}

/*

=item C<int bin(const char *str)>

Returns the integer specified by the binary string, C<str>. C<str> may
either be a string of C<[0-1]> or C<"0b"> followed by a string of C<[0-1]>.
On error, returns 0.

=cut

*/

int bin(const char *str)
{
	int ret = 0;

	if (!str)
		return 0;

	if (str[0] == '0' && str[1] == 'b')
		str += 2;

	for (; *str; ++str)
	{
		ret <<= 1;

		switch (*str)
		{
			case '0': break;
			case '1': ret |= 1; break;
			default:  return 0;
		}
	}

	return ret;
}

/*

=item C<int str_hex(const String *str)>

Returns the integer specified by the hexadecimal string, C<str>. C<str> may
either be a string of C<[0-9a-fA-F]> or C<"0x"> followed by a string of
C<[0-9a-fA-f]>. On error, returns 0.

=cut

*/

int str_hex(const String *str)
{
	if (!str)
		return 0;

	return hex(str->str);
}

/*

=item C<int hex(const char *str)>

Returns the integer specified by the hexadecimal string, C<str>. C<str> may
either be a string of C<[0-9a-fA-F]> or C<"0x"> followed by a string of
C<[0-9a-fA-f]>. On error, returns 0.

=cut

*/

int hex(const char *str)
{
	int ret = 0;

	if (!str)
		return 0;

	if (str[0] == '0' && str[1] == 'x')
		str += 2;

	for (; *str; ++str)
	{
		ret <<= 4;

		switch (*str)
		{
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				ret |= *str - '0';
				break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
				ret |= *str - 'a' + 10;
				break;
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				ret |= *str - 'A' + 10;
				break;
			default:
				return 0;
		}
	}

	return ret;
}

/*

=item C<int str_oct(const String *str)>

Returns the integer specified by the binary, octal or hexadecimal string,
C<str>. C<str> may either be C<"0x"> followed by a string of C<[0-9a-fA-F]>
(hexadecimal), C<"0b"> followed by a string of C<[0-1]> (binary) or C<"0">
followed by a a string of C<[0-7]> (octal). On error, returns 0.

=cut

*/

int str_oct(const String *str)
{
	if (!str)
		return 0;

	return oct(str->str);
}

/*

=item C<int oct(const char *str)>

Returns the integer specified by the binary, octal or hexadecimal string,
C<str>. C<str> may either be C<"0x"> followed by a string of C<[0-9a-fA-F]>
(hexadecimal), C<"0b"> followed by a string of C<[0-1]> (binary) or C<"0">
followed by a a string of C<[0-7]> (octal). On error, returns 0.

=cut

*/

int oct(const char *str)
{
	int ret = 0;

	if (!str || str[0] != '0')
		return 0;

	if (str[1] == 'b')
		return bin(str);

	if (str[1] == 'x')
		return hex(str);

	for (++str; *str; ++str)
	{
		ret <<= 3;

		switch (*str)
		{
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				ret |= *str - '0';
				break;
			default:
				return 0;
		}
	}

	return ret;
}

/*

=back

=head1 BUGS

Doesn't support multibyte/widechar strings, UTF8, UNICODE or ISO 10646.

The C<delim> parameter to the I<split()> and I<join()> functions is an
ordinary C string so it can't contain C<nul> characters.

The C<quotable> parameter to the I<quote()> and I<unquote()> functions is an
ordinary C string so it can't contain C<nul> characters.

The C<uncoded> and C<coded> parameters to the I<str_encode()> and
I<str_decode()> functions are ordinary C strings so they can't contain
C<nul> characters.

=head1 SEE ALSO

L<regcomp(3)|regcomp(3)>,
L<regexec(3)|regexec(3)>,
L<regerror(3)|regerror(3)>,
L<regfree(3)|regfree(3)>,
L<perlfunc(1)|perlfunc(1)>,
L<perlop(1)|perlop(1)>,
L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<msg(3)|msg(3)>,
L<net(3)|net(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

void str_print(const char *str, size_t length)
{
	int i;

	printf("\"");

	for (i = 0; i < length + 1; ++i)
		printf(isprint((int)(unsigned char)str[i]) ? "%c" : "\\%03o", (unsigned char)str[i]);

	printf("\"");
}

int main(int ac, char **av)
{
	int errors = 0;
	String *a, *b, *c;
	char tst[BUFSIZ];
	List *list;
	StrTR *trtable;
	int i, big, rc;
#ifndef REGEX_MISSING
	regex_t re[1];
#endif

	printf("Testing: str\n");

#define TEST_ACT(i, action) \
	if (!(action)) \
		++errors, printf("Test%d: %s failed\n", (i), (#action));

#define TEST_EQ(i, action, eq) \
	if (!((rc = (action)) == (eq))) \
		++errors, printf("Test%d: %s failed: returned %d, not %d\n", (i), (#action), rc, (eq));

#define TEST_STR(i, action, str, length, value) \
	TEST_ACT(i, action) \
	CHECK_STR(i, action, str, length, value)

#define TEST_CSTR(i, action, str, length, value) \
	TEST_ACT(i, action) \
	CHECK_CSTR(i, action, str, length, value)

#define TEST_LEN(i, action, str, length) \
	TEST_ACT(i, action) \
	CHECK_LEN(i, action, str, length)

#define CHECK_LEN(i, action, str, length) \
	if (str_length(str) != (length)) \
		++errors, printf("Test%d: %s failed: length %d (not %d)\n", (i), (#action), str_length(str), (length));

#define CHECK_CLEN(i, action, str, length) \
	if (strlen(str) != (length)) \
		++errors, printf("Test%d: %s failed: length %d (not %d)\n", (i), (#action), strlen(str), (length));

#define CHECK_VAL(i, action, str, length, value) \
	if (memcmp(cstr(str), (value), str_length(str) + 1)) \
	{ \
		++errors, printf("Test%d: %s failed: returned:\n", (i), (#action)); \
		str_print(cstr(str), length); \
		printf("\nnot:\n"); \
		str_print(value, length); \
		printf("\n"); \
	}

#define CHECK_CVAL(i, action, str, length, value) \
	if (strcmp(str, (value))) \
	{ \
		++errors, printf("Test%d: %s failed: returned:\n", (i), (#action)); \
		str_print(str, length); \
		printf("\nnot:\n"); \
		str_print(value, length); \
		printf("\n"); \
	}

#define CHECK_STR(i, action, str, length, value) \
	CHECK_LEN(i, action, str, length) \
	CHECK_VAL(i, action, str, length, value)

#define CHECK_CSTR(i, action, str, length, value) \
	CHECK_CLEN(i, action, str, length) \
	CHECK_CVAL(i, action, str, length, value)

#define CHECK_LIST_LENGTH(i, action, list, len) \
	if (list_length(list) != (len)) \
		++errors, printf("Test%d: %s failed (length %d, not %d)\n", (i), #action, list_length(list), (len));

#define CHECK_LIST_ITEM(i, action, index, tok) \
	if (list_item(list, index) && memcmp(cstr((String *)list_item(list, index)), tok, str_length((String *)list_item(list, index)) + 1)) \
	{ \
		++errors; \
		printf("Test%d: %s failed (item %d is \"%s\", not \"%s\")\n", i, #action, index, cstr((String *)list_item(list, index)), tok); \
	}

	/* Test create, empty, length, clear, insert, append, prepend */

	TEST_STR(1, a = str_create("This is a test string %s %c %d\n", "abc", 'x', 37), a, 31, "This is a test string abc x 37\n")
	TEST_STR(2, b = str_create(NULL), b, 0, "")
	TEST_ACT(3, str_empty(b))
	TEST_STR(4, str_append(b, "abc"), b, 3, "abc")
	TEST_ACT(5, !str_empty(b))
	TEST_STR(6, str_clear(b), b, 0, "")
	TEST_ACT(7, str_empty(b))
	TEST_STR(8, str_append(b, "abc"), b, 3, "abc")
	TEST_STR(9, str_prepend(b, "def"), b, 6, "defabc")
	TEST_STR(10, str_insert(b, 1, "ghi"), b, 9, "dghiefabc")
	TEST_ACT(11, str_empty(NULL))

	/* Test string copying and destruction */

	TEST_STR(12, c = str_copy(a), c, 31, "This is a test string abc x 37\n")
	TEST_ACT(13, !str_destroy(c))
	TEST_ACT(14, !str_copy(NULL))

	/* Test insert, append, prepend, remove, replace */

	TEST_STR(15, str_remove(a, 30), a, 30, "This is a test string abc x 37")
	TEST_STR(16, str_remove(a, 0), a, 29, "his is a test string abc x 37")
	TEST_STR(17, str_remove(a, 10), a, 28, "his is a tst string abc x 37")
	TEST_STR(18, str_replace(a, 0, 0, "123"), a, 31, "123his is a tst string abc x 37")
	TEST_STR(19, str_replace(a, 1, 1, "123"), a, 33, "11233his is a tst string abc x 37")
	TEST_STR(20, str_replace(a, 0, 5, "456"), a, 31, "456his is a tst string abc x 37")
	TEST_STR(21, str_replace(a, 30, 1, "789"), a, 33, "456his is a tst string abc x 3789")
	TEST_STR(22, str_replace(a, 33, 0, "a"), a, 34, "456his is a tst string abc x 3789a")
	TEST_STR(23, str_remove_range(a, 0, 0), a, 34, "456his is a tst string abc x 3789a")
	TEST_STR(24, str_remove_range(a, 0, 3), a, 31, "his is a tst string abc x 3789a")
	TEST_STR(25, str_remove_range(a, 13, 0), a, 31, "his is a tst string abc x 3789a")
	TEST_STR(26, str_remove_range(a, 13, 7), a, 24, "his is a tst abc x 3789a")
	TEST_STR(27, str_remove_range(a, 23, 0), a, 24, "his is a tst abc x 3789a")
	TEST_STR(28, str_remove_range(a, 21, 2), a, 22, "his is a tst abc x 37a")

	TEST_ACT(29, c = str_create("__test__"))

	TEST_STR(30, str_prepend_str(a, c), a, 30, "__test__his is a tst abc x 37a")
	TEST_STR(31, str_insert_str(b, 1, c), b, 17, "d__test__ghiefabc")
	TEST_STR(32, str_append_str(a, b), a, 47, "__test__his is a tst abc x 37ad__test__ghiefabc")

	TEST_STR(33, str_replace_str(a, 1, 2, b), a, 62,  "_d__test__ghiefabcest__his is a tst abc x 37ad__test__ghiefabc")
	TEST_STR(34, str_replace_str(a, 60, 2, b), a, 77, "_d__test__ghiefabcest__his is a tst abc x 37ad__test__ghiefad__test__ghiefabc")
	TEST_STR(35, str_remove_range(b, 1, 3), b, 14, "dest__ghiefabc")

	TEST_STR(36, str_prepend(b, NULL), b, 14, "dest__ghiefabc")
	TEST_STR(37, str_insert(b, 5, NULL), b, 14, "dest__ghiefabc")
	TEST_STR(38, str_append(b, NULL), b, 14, "dest__ghiefabc")
	TEST_STR(39, str_prepend(b, ""), b, 14, "dest__ghiefabc")
	TEST_STR(40, str_insert(b, 5, ""), b, 14, "dest__ghiefabc")
	TEST_STR(41, str_append(b, ""), b, 14, "dest__ghiefabc")

	TEST_ACT(42, !str_destroy(a))
	TEST_ACT(43, !str_destroy(b))
	TEST_ACT(44, !str_destroy(c))

	/* Test str_repeat() */

	TEST_STR(45, c = str_repeat(0, ""), c, 0, "")
	TEST_ACT(46, !str_destroy(c))
	TEST_STR(47, c = str_repeat(10, ""), c, 0, "")
	TEST_ACT(48, !str_destroy(c))
	TEST_STR(49, c = str_repeat(0, "%d", 11 * 11), c, 0, "")
	TEST_ACT(50, !str_destroy(c))
	TEST_STR(51, c = str_repeat(10, "%d", 11 * 11), c, 30, "121121121121121121121121121121")
	TEST_ACT(52, !str_destroy(c))
	TEST_STR(53, c = str_repeat(10, " "), c, 10, "          ")
	TEST_ACT(54, !str_destroy(c))

	/* Test big string creation and big string growth (big KB) */

	big = 64;
	TEST_LEN(55, a = str_create("%*s", 1024 * big, ""), a, 1024 * big)
	TEST_ACT(55, !str_destroy(a))

	TEST_ACT(56, a = str_create(NULL))

	for (i = 0; i < big; ++i)
	{
		if (!str_append(a, "%1024s", ""))
		{
			++errors, printf("Test46: str_append(\"%%1024s\", \"\") failed (on iteration %d)\n", i);
			break;
		}

		if (str_length(a) != ((i + 1) * 1024))
		{
			++errors, printf("Test46: str_append(\"%%1024s\", \"\") failed: (on iteration %d) length %d, not %d\n", i, str_length(a), (i + 1) * 1024);
			break;
		}
	}

	TEST_ACT(56, !str_destroy(a))

	/* Test big string sized creation and big string growth (big KB) */

	TEST_LEN(57, a = str_create_sized(1024 * big, "%*s", 1024 * big, ""), a, 1024 * big)
	TEST_ACT(57, !str_destroy(a))

	TEST_ACT(58, a = str_create_sized(1024 * big, NULL))

	for (i = 0; i < big; ++i)
	{
		if (!str_append(a, "%1024s", ""))
		{
			++errors, printf("Test48: str_append(\"%%1024s\", \"\") failed (on iteration %d)\n", i);
			break;
		}

		if (str_length(a) != ((i + 1) * 1024))
		{
			++errors, printf("Test48: str_append(\"%%1024s\", \"\") failed: (on iteration %d) length %d, not %d\n", i, str_length(a), (i + 1) * 1024);
			break;
		}
	}

	TEST_ACT(58, !str_destroy(a))

	/* Test substr */

	TEST_ACT(59, a = str_create("abcdefghijkl"))
	TEST_STR(60, b = str_substr(a, 0, 0), b, 0, "")
	TEST_ACT(61, !str_destroy(b))
	TEST_STR(62, c = str_substr(a, 0, 3), c, 3, "abc")
	TEST_ACT(63, !str_destroy(c))
	TEST_STR(64, b = str_substr(a, 6, 0), b, 0, "")
	TEST_ACT(65, !str_destroy(b))
	TEST_STR(66, c = str_substr(a, 6, 3), c, 3, "ghi")
	TEST_ACT(67, !str_destroy(c))
	TEST_STR(68, b = str_substr(a, 9, 0), b, 0, "")
	TEST_ACT(69, !str_destroy(b))
	TEST_STR(70, c = str_substr(a, 9, 3), c, 3, "jkl")
	TEST_ACT(71, !str_destroy(c))
	TEST_STR(72, c = str_substr(a, 0, 12), c, 12, "abcdefghijkl")
	TEST_ACT(73, !str_destroy(c))
	TEST_ACT(74, !str_destroy(a))

	TEST_STR(75, b = substr("abcdefghijkl", 0, 0), b, 0, "")
	TEST_ACT(76, !str_destroy(b))
	TEST_STR(77, c = substr("abcdefghijkl", 0, 3), c, 3, "abc")
	TEST_ACT(78, !str_destroy(c))
	TEST_STR(79, b = substr("abcdefghijkl", 6, 0), b, 0, "")
	TEST_ACT(80, !str_destroy(b))
	TEST_STR(81, c = substr("abcdefghijkl", 6, 3), c, 3, "ghi")
	TEST_ACT(82, !str_destroy(c))
	TEST_STR(83, b = substr("abcdefghijkl", 9, 0), b, 0, "")
	TEST_ACT(84, !str_destroy(b))
	TEST_STR(85, c = substr("abcdefghijkl", 9, 3), c, 3, "jkl")
	TEST_ACT(86, !str_destroy(c))
	TEST_STR(87, c = substr("abcdefghijkl", 0, 12), c, 12, "abcdefghijkl")
	TEST_ACT(88, !str_destroy(c))
	TEST_ACT(89, !str_destroy(a))

	/* Test splice */

#define TEST_SPLICE(i, action, ostr, olen, oval, nstr, nlen, nval) \
	TEST_ACT(i, action) \
	CHECK_STR(i, action, ostr, olen, oval) \
	CHECK_STR(i, action, nstr, nlen, nval) \
	TEST_ACT(i, !str_destroy(nstr))

	TEST_ACT   (90, a = str_create("aaabbbcccdddeeefffggghhhiiijjjkkklll"))
	TEST_SPLICE(91, b = str_splice(a, 0, 0), a, 36, "aaabbbcccdddeeefffggghhhiiijjjkkklll", b, 0, "")
	TEST_SPLICE(92, b = str_splice(a, 0, 3), a, 33, "bbbcccdddeeefffggghhhiiijjjkkklll", b, 3, "aaa")
	TEST_SPLICE(93, b = str_splice(a, 3, 0), a, 33, "bbbcccdddeeefffggghhhiiijjjkkklll", b, 0, "")
	TEST_SPLICE(94, b = str_splice(a, 3, 6), a, 27, "bbbeeefffggghhhiiijjjkkklll", b, 6, "cccddd")
	TEST_SPLICE(95, b = str_splice(a, 3, 0), a, 27, "bbbeeefffggghhhiiijjjkkklll", b, 0, "")
	TEST_SPLICE(96, b = str_splice(a, 24, 3), a, 24, "bbbeeefffggghhhiiijjjkkk", b, 3, "lll")
	TEST_ACT   (97, !str_destroy(a))

	/* Test tr */

#define TEST_TR(i, action, orig, str, bytes, length, value) \
	TEST_ACT(i, str = str_copy(orig)) \
	TEST_EQ(i, action, bytes) \
	CHECK_STR(i, action, str, length, value) \
	TEST_ACT(i, !str_destroy(str))

	TEST_ACT(98, a = str_create(" .,;'/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))
	TEST_ACT(99, b = str_create("bookkeeper"))
	TEST_TR (100, str_tr(c, "A-Z", "a-z", 0), a, c, 26, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz")
	TEST_TR (101, str_tr(c, "a-z", "A-Z", 0), a, c, 26, 68, " .,;'/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (102, str_tr(c, "a", "a", 0), a, c, 1, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (103, str_tr(c, "0-9", NULL, 0), a, c, 10, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (104, str_tr(c, "a-zA-Z", NULL, TR_SQUASH), b, c, 10, 7, "bokeper")
	TEST_TR (105, str_tr(c, "a-zA-Z", " ", TR_SQUASH), b, c, 10, 1, " ")
	TEST_TR (106, str_tr(c, "a-zA-Z", " ", TR_COMPLEMENT | TR_SQUASH), a, c, 16, 53, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (107, str_tr(c, "AAA", "XYZ", 0), a, c, 1, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzXBCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (108, str_tr(c, "a-z", "*", 0), a, c, 26, 68, " .,;'/0123456789**************************ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (109, str_tr(c, "a-z", "*", TR_COMPLEMENT), a, c, 42, 68, "****************abcdefghijklmnopqrstuvwxyz**************************")
	TEST_TR (110, str_tr(c, "a-z", " ", TR_SQUASH), a, c, 26, 43, " .,;'/0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (111, str_tr(c, "a-z", " ", TR_SQUASH | TR_COMPLEMENT), a, c, 42, 28, " abcdefghijklmnopqrstuvwxyz ")
	TEST_TR (112, str_tr(c, "a-z", "x-z", TR_DELETE), a, c, 26, 45, " .,;'/0123456789xyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (113, str_tr(c, "a-z", "", TR_DELETE), a, c, 26, 42, " .,;'/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (113, str_tr(c, "a-z", "   ", TR_DELETE), a, c, 26, 45, " .,;'/0123456789   ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (114, str_tr(c, "a-z", "", TR_DELETE | TR_COMPLEMENT), a, c, 42, 26, "abcdefghijklmnopqrstuvwxyz")
	TEST_TR (115, str_tr(c, "a-z", "                                                           ", TR_DELETE | TR_COMPLEMENT), a, c, 42, 41, "               abcdefghijklmnopqrstuvwxyz")
	TEST_TR (116, str_tr(c, "a-z", "                                                           ", TR_DELETE | TR_COMPLEMENT | TR_SQUASH), a, c, 42, 27, " abcdefghijklmnopqrstuvwxyz")
	TEST_TR (117, str_tr(c, "a-z", "             ", TR_DELETE), a, c, 26, 55, " .,;'/0123456789             ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (118, str_tr(c, "a-z", "             ", TR_DELETE | TR_SQUASH), a, c, 26, 43, " .,;'/0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (119, str_tr(c, "a-z", "a-b", 0), a, c, 26, 68, " .,;'/0123456789abbbbbbbbbbbbbbbbbbbbbbbbbABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (120, str_tr(c, "a-zA-Z", "A-Za-z", 0), a, c, 52, 68, " .,;'/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
	TEST_TR (121, str_tr(c, "a-zA-Z", "A-Za-z", TR_COMPLEMENT), a, c, 16, 68, "gusznvwxyzzzzzzzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (122, str_tr(c, "A-Z", "a-m", 0), a, c, 26, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmmmmmmmmmmmmmm")
	TEST_TR (123, str_tr(c, "A-Z", "a-m", TR_SQUASH), a, c, 26, 55, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklm")
	TEST_TR (124, str_tr(c, "a-zA-Z", "n-za-mN-ZA-M", 0), a, c, 52, 68, " .,;'/0123456789nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM")
	TEST_ACT(125, !str_destroy(a))
	TEST_ACT(126, !str_destroy(b))

#define TEST_TR_DIRECT(i, action, str, bytes, length, value) \
	TEST_EQ(i, action, bytes) \
	CHECK_STR(i, action, str, length, value)

	big = 1024;
	TEST_ACT(127, a = str_create("abcDEFghiJKLmnoPQRstuVWXyz"))
	TEST_ACT(127, trtable = tr_compile("a-zA-Z", "A-Za-z", 0))
	for (i = 0; i < big; ++i)
	{
		TEST_TR_DIRECT(127, tr_compiled(a->str, trtable), a, 26, 26, "ABCdefGHIjklMNOpqrSTUvwxYZ")
		TEST_TR_DIRECT(127, tr_compiled(a->str, trtable), a, 26, 26, "abcDEFghiJKLmnoPQRstuVWXyz")
	}
	TEST_ACT(127, !tr_destroy(trtable))
	TEST_ACT(127, !str_destroy(a))

	TEST_ACT(128, a = str_create("\170\171\172\173\174\175\176\177\200\201\202\203\204\205\206\207\210"))
	TEST_ACT(128, b = str_create("\200-\377"))
	TEST_ACT(128, c = str_create("%c-\177", '\000'))
	TEST_TR_DIRECT(128, str_tr_str(a, b, c, 0), a, 9, 17, "\170\171\172\173\174\175\176\177\000\001\002\003\004\005\006\007\010")
	TEST_ACT(128, !str_destroy(a))
	TEST_ACT(128, !str_destroy(b))
	TEST_ACT(128, !str_destroy(c))

#ifndef REGEX_MISSING

	/* Test str_regex */

	TEST_ACT(129, a = str_create("abcabcabc"))
	else
	{
		TEST_ACT(129, list = str_regex("a((.*)a(.*))a", a, 0, 0))
		else
		{
			CHECK_LIST_LENGTH(129, str_regex(), list, 4)
			CHECK_LIST_ITEM(129, str_regex("a((.*)a(.*))a", "abcabcabc"), 0, "abcabca")
			CHECK_LIST_ITEM(129, str_regex("a((.*)a(.*))a", "abcabcabc"), 1, "bcabc")
			CHECK_LIST_ITEM(129, str_regex("a((.*)a(.*))a", "abcabcabc"), 2, "bc")
			CHECK_LIST_ITEM(129, str_regex("a((.*)a(.*))a", "abcabcabc"), 3, "bc")
			TEST_ACT(129, !list_destroy(list))
		}

		TEST_ACT(129, !str_destroy(a))
	}

	/* Test str_regsub */

#define TEST_REGSUB(i, str, pat, rep, cflags, eflags, all, len, res) \
	TEST_ACT((i), a = str_create("%s", (str))) \
	else \
	{ \
		TEST_STR((i), str_regsub((pat), (rep), a, (cflags), (eflags), (all)), a, (len), (res)) \
		TEST_ACT((i), !str_destroy(a)) \
	}

	TEST_REGSUB(130, "xabcabcabcx", "a((.*)a(.*))a", "$0", 0, 0, 0, 11, "xabcabcabcx")
	TEST_REGSUB(131, "xabcabcabcx", "a((.*)a(.*))a", "$$$2${3}!", 0, 0, 0, 10, "x$bcbc!bcx")
	TEST_REGSUB(132, "xabcabcabcx", "a((.*)a(.*))a", "$$$2$31!", 0, 0, 0, 11, "x$bcbc1!bcx")
	TEST_REGSUB(133, "xabcabcabcx", "a", "z", 0, 0, 0, 11, "xzbcabcabcx")
	TEST_REGSUB(134, "xabcabcabcx", "a", "z", 0, 0, 1, 11, "xzbczbczbcx")
	TEST_REGSUB(135, "aba", "a", "z", 0, 0, 0, 3, "zba")
	TEST_REGSUB(136, "aba", "a", "z", 0, 0, 1, 3, "zbz")
	TEST_REGSUB(137, "xabcabcabcx", "((((((((((((((((((((((((((((((((a))))))))))))))))))))))))))))))))", "!${32}!", 0, 0, 0, 13, "x!a!bcabcabcx")
	TEST_REGSUB(138, "xabcabcabcx", "((((((((((((((((((((((((((((((((a))))))))))))))))))))))))))))))))", "!${32}!", 0, 0, 1, 17, "x!a!bc!a!bc!a!bcx")
	TEST_REGSUB(139, "\\a:b:c:d:e:f:G:H:I:", "(...)(..)(..)(..)(..)(..)(..)(..)(..)", "$1\\U$2\\Q$3\\l$4\\E$5\\E$6\\L$7\\Q\\u$8\\E\\E$9\\\\l", 0, 0, 0, 24, "\\a:B:C\\:d\\:E:f:g:H\\:I:\\l")

#endif

	/* Test fmt */

	/* Opening paragraphs from Silas Marner by George Eliot */

	TEST_ACT(140, a = str_create(
		"In the days when the spinning wheels hummed busily in the\n"
		"farmhouses and even the great ladies, clothed in silk and\n"
		"thread lace, had their toy spinning wheels of polished oak,\n"
		"there might be seen in districts far away among the lanes,\n"
		"or deep in the bosom of the hills, certain pallid undersized\n"
		"men, who, by the size of the brawny country folk, looked like\n"
		"the remnants of a disinherited race. The shepherd's dog barked\n"
		"fiercely when one of these alien-looking men appeared on the\n"
		"upland, dark against the early winter sunset; for what dog\n"
		"likes a figure bent under a heavy bag? And these pale men rarely\n"
		"stirred abroad without that mysterious burden. The shepherd\n"
		"himself, though he had good reason to believe that the bag held\n"
		"nothing but flaxen thread, or else the long rolls of strong linen\n"
		"spun from that thread, was not quite sure that this trade of\n"
		"weaving, indispensible though it was, could be carried on\n"
		"entirely without the help of the Evil One.\n"
	))

	TEST_ACT(141, list = str_fmt(a, 70, ALIGN_LEFT))
	CHECK_LIST_LENGTH(141, str_fmt(a, 70, ALIGN_LEFT), list, 14)
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 0,  "In the days when the spinning wheels hummed busily in the farmhouses")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 1,  "and even the great ladies, clothed in silk and thread lace, had their")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 2,  "toy spinning wheels of polished oak, there might be seen in districts")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 3,  "far away among the lanes, or deep in the bosom of the hills, certain")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 4,  "pallid undersized men, who, by the size of the brawny country folk,")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 5,  "looked like the remnants of a disinherited race. The shepherd's dog")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 6,  "barked fiercely when one of these alien-looking men appeared on the")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 7,  "upland, dark against the early winter sunset; for what dog likes a")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 8,  "figure bent under a heavy bag? And these pale men rarely stirred")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 9,  "abroad without that mysterious burden. The shepherd himself, though he")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 10, "had good reason to believe that the bag held nothing but flaxen")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 11, "thread, or else the long rolls of strong linen spun from that thread,")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 12, "was not quite sure that this trade of weaving, indispensible though it")
	CHECK_LIST_ITEM(141, str_fmt(a, 70, ALIGN_LEFT), 13, "was, could be carried on entirely without the help of the Evil One.")
	TEST_ACT(141, !list_destroy(list))

	TEST_ACT(142, list = str_fmt(a, 70, ALIGN_RIGHT))
	CHECK_LIST_LENGTH(142, str_fmt(a, 70, ALIGN_RIGHT), list, 14)
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 0,  "  In the days when the spinning wheels hummed busily in the farmhouses")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 1,  " and even the great ladies, clothed in silk and thread lace, had their")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 2,  " toy spinning wheels of polished oak, there might be seen in districts")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 3,  "  far away among the lanes, or deep in the bosom of the hills, certain")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 4,  "   pallid undersized men, who, by the size of the brawny country folk,")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 5,  "   looked like the remnants of a disinherited race. The shepherd's dog")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 6,  "   barked fiercely when one of these alien-looking men appeared on the")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 7,  "    upland, dark against the early winter sunset; for what dog likes a")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 8,  "      figure bent under a heavy bag? And these pale men rarely stirred")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 9,  "abroad without that mysterious burden. The shepherd himself, though he")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 10, "       had good reason to believe that the bag held nothing but flaxen")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 11, " thread, or else the long rolls of strong linen spun from that thread,")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 12, "was not quite sure that this trade of weaving, indispensible though it")
	CHECK_LIST_ITEM(142, str_fmt(a, 70, ALIGN_RIGHT), 13, "   was, could be carried on entirely without the help of the Evil One.")
	TEST_ACT(142, !list_destroy(list))

	TEST_ACT(143, list = str_fmt(a, 70, ALIGN_CENTRE));
	CHECK_LIST_LENGTH(143, str_fmt(a, 70, ALIGN_CENTRE), list, 16)
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 0,  "      In the days when the spinning wheels hummed busily in the")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 1,  "      farmhouses and even the great ladies, clothed in silk and")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 2,  "     thread lace, had their toy spinning wheels of polished oak,")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 3,  "      there might be seen in districts far away among the lanes,")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 4,  "     or deep in the bosom of the hills, certain pallid undersized")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 5,  "    men, who, by the size of the brawny country folk, looked like")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 6,  "    the remnants of a disinherited race. The shepherd's dog barked")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 7,  "     fiercely when one of these alien-looking men appeared on the")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 8,  "      upland, dark against the early winter sunset; for what dog")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 9,  "   likes a figure bent under a heavy bag? And these pale men rarely")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 10, "     stirred abroad without that mysterious burden. The shepherd")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 11, "   himself, though he had good reason to believe that the bag held")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 12, "  nothing but flaxen thread, or else the long rolls of strong linen")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 13, "     spun from that thread, was not quite sure that this trade of")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 14, "      weaving, indispensible though it was, could be carried on")
	CHECK_LIST_ITEM(143, str_fmt(a, 70, ALIGN_CENTRE), 15, "              entirely without the help of the Evil One.")
	TEST_ACT(143, !list_destroy(list))

	TEST_ACT(144, list = str_fmt(a, 70, ALIGN_FULL))
	CHECK_LIST_LENGTH(144, str_fmt(a, 70, ALIGN_FULL), list, 14)
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 0,  "In the days when the spinning wheels hummed busily in  the  farmhouses")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 1,  "and even the great ladies, clothed in silk and thread lace, had  their")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 2,  "toy spinning wheels of polished oak, there might be seen in  districts")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 3,  "far away among the lanes, or deep in the bosom of the  hills,  certain")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 4,  "pallid undersized men, who, by the size of the  brawny  country  folk,")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 5,  "looked like the remnants of a disinherited race.  The  shepherd's  dog")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 6,  "barked fiercely when one of these alien-looking men  appeared  on  the")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 7,  "upland, dark against the early winter sunset; for  what  dog  likes  a")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 8,  "figure bent under a heavy bag?  And  these  pale  men  rarely  stirred")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 9,  "abroad without that mysterious burden. The shepherd himself, though he")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 10, "had good reason to believe  that  the  bag  held  nothing  but  flaxen")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 11, "thread, or else the long rolls of strong linen spun from that  thread,")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 12, "was not quite sure that this trade of weaving, indispensible though it")
	CHECK_LIST_ITEM(144, str_fmt(a, 70, ALIGN_FULL), 13, "was, could be carried on entirely without the help of the Evil One.")
	TEST_ACT(144, !list_destroy(list))

	TEST_ACT(145, !str_destroy(a))

#define TEST_FMT3(i, alignment, width, line1, line2, line3) \
	TEST_ACT(i, list = str_fmt(a, width, alignment)) \
	CHECK_LIST_LENGTH(i, str_fmt(a, width, alignment), list, 3) \
	CHECK_LIST_ITEM(i, str_fmt(a, width, alignment), 0, line1) \
	CHECK_LIST_ITEM(i, str_fmt(a, width, alignment), 1, line2) \
	CHECK_LIST_ITEM(i, str_fmt(a, width, alignment), 2, line3) \
	TEST_ACT(i, !list_destroy(list))

	TEST_ACT(146, a = str_create("123456789\n1234567890\n12345678901"))
	TEST_FMT3(147, '<', 10, "123456789", "1234567890", "12345678901")
	TEST_FMT3(148, '>', 10, " 123456789", "1234567890", "12345678901")
	TEST_FMT3(149, '=', 10, "123456789", "1234567890", "12345678901")
	TEST_FMT3(150, '|', 10, "123456789", "1234567890", "12345678901")
	TEST_ACT(151, !str_destroy(a))

	TEST_ACT(152, a = str_create("12345678901\n123456789\n1234567890"))
	TEST_FMT3(153, '<', 10, "12345678901", "123456789", "1234567890")
	TEST_FMT3(154, '>', 10, "12345678901", " 123456789", "1234567890")
	TEST_FMT3(155, '=', 10, "12345678901", "123456789", "1234567890")
	TEST_FMT3(156, '|', 10, "12345678901", "123456789", "1234567890")
	TEST_ACT(157, !str_destroy(a))

	TEST_ACT(158, a = str_create("1234567890\n12345678901\n123456789"))
	TEST_FMT3(159, '<', 10, "1234567890", "12345678901", "123456789")
	TEST_FMT3(160, '>', 10, "1234567890", "12345678901", " 123456789")
	TEST_FMT3(161, '=', 10, "1234567890", "12345678901", "123456789")
	TEST_FMT3(162, '|', 10, "1234567890", "12345678901", "123456789")
	TEST_ACT(163, !str_destroy(a))

	TEST_ACT(164, a = str_create("12345678901\n1234567890\n123456789"))
	TEST_FMT3(165, '<', 10, "12345678901", "1234567890", "123456789")
	TEST_FMT3(166, '>', 10, "12345678901", "1234567890", " 123456789")
	TEST_FMT3(167, '=', 10, "12345678901", "1234567890", "123456789")
	TEST_FMT3(168, '|', 10, "12345678901", "1234567890", "123456789")
	TEST_ACT(169, !str_destroy(a))

	TEST_ACT(170, a = str_create("1234567890\n123456789\n12345678901"))
	TEST_FMT3(171, '<', 10, "1234567890", "123456789", "12345678901")
	TEST_FMT3(172, '>', 10, "1234567890", " 123456789", "12345678901")
	TEST_FMT3(173, '=', 10, "1234567890", "123456789", "12345678901")
	TEST_FMT3(174, '|', 10, "1234567890", "123456789", "12345678901")
	TEST_ACT(175, !str_destroy(a))

	TEST_ACT(176, a = str_create("123456789\n12345678901\n1234567890"))
	TEST_FMT3(177, '<', 10, "123456789", "12345678901", "1234567890")
	TEST_FMT3(178, '>', 10, " 123456789", "12345678901", "1234567890")
	TEST_FMT3(179, '=', 10, "123456789", "12345678901", "1234567890")
	TEST_FMT3(180, '|', 10, "123456789", "12345678901", "1234567890")
	TEST_ACT(181, !str_destroy(a))

	TEST_ACT(182, a = str_create(
		"http://www.zip.com.au/~joe/fairly/long/testing/url/\n"
		"hello there\n"
		"http://www.zip.com.au/~joe/fairly/long/testing/url/\n"
	))
	TEST_FMT3(183, '<', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "hello there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_FMT3(184, '>', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "                   hello there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_FMT3(185, '=', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "hello                    there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_FMT3(186, '|', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "         hello there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_ACT(187, !str_destroy(a))

#define TEST_FMT0(i, alignment, width) \
	TEST_ACT(i, list = str_fmt(a, width, alignment)) \
	CHECK_LIST_LENGTH(i, str_fmt(a, width, alignment), list, 0) \
	TEST_ACT(i, !list_destroy(list))

	TEST_ACT(188, a = str_create(""))
	TEST_FMT0(189, '<', 10)
	TEST_FMT0(190, '>', 10)
	TEST_FMT0(191, '=', 10)
	TEST_FMT0(192, '|', 10)
	TEST_ACT(193, !str_destroy(a))

	/* Test split */

#define CHECK_SPLIT_ITEM(i, func, str, delim, index, tok) \
	if (list_item(list, index) && memcmp(cstr((String *)list_item(list, index)), tok, str_length((String *)list_item(list, index)) + 1)) \
	{ \
		++errors; \
		printf("Test%d: %s(", i, #func); \
		str_print(str, strlen(str)); \
		printf(", "); \
		str_print(delim, strlen(delim)); \
		printf(") failed (token %d is ", index); \
		str_print(cstr((String *)list_item(list, index)), str_length((String *)list_item(list, index))); \
		printf(", not \"%s\")\n", tok); \
	}

#define TEST_SSPLIT(i, func, str, delim, tok1, tok2, tok3, tok4) \
	TEST_ACT(i, a = str_create(str)) \
	TEST_ACT(i, list = func(a, delim)) \
	CHECK_LIST_LENGTH(i, str_split(str, delim), list, 4) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 0, tok1) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 1, tok2) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 2, tok3) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 3, tok4) \
	TEST_ACT(i, !str_destroy(a)) \
	TEST_ACT(i, !list_destroy(list))

#define TEST_SPLIT(i, func, str, delim, tok1, tok2, tok3, tok4) \
	TEST_ACT(i, list = func(str, delim)) \
	CHECK_LIST_LENGTH(i, split(str, delim), list, 4) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 0, tok1) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 1, tok2) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 2, tok3) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 3, tok4) \
	TEST_ACT(i, !list_destroy(list))

	TEST_SSPLIT(194, str_split, "a,b.c;d", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(195, str_split, " a , b . c ; d ", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(196, str_split, " a ,b . c;d: ", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(197, str_split, "a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(198, str_split, " a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(199, str_split, "aa,bb.cc;dd", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(200, str_split, " aa , bb . cc ; dd ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(201, str_split, " aa ,bb . cc;dd: ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(202, str_split, "aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(203, str_split, " aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(205, str_split, "abcd", "", "a", "b", "c", "d")

	TEST_SPLIT(206, split, "a,b.c;d", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(207, split, " a , b . c ; d ", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(208, split, " a ,b . c;d: ", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(209, split, "a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(210, split, " a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(211, split, "aa,bb.cc;dd", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(212, split, " aa , bb . cc ; dd ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(213, split, " aa ,bb . cc;dd: ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(214, split, "aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(215, split, " aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(216, split, "abcd", "", "a", "b", "c", "d")

#ifndef REGEX_MISSING

	/* Test regex_split */

	TEST_SSPLIT(217, str_regex_split, "a,b.c;d", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SSPLIT(218, str_regex_split, " a , b . c ; d ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SSPLIT(219, str_regex_split, " a ,b . c;d: ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SSPLIT(220, str_regex_split, "a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SSPLIT(221, str_regex_split, " a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SSPLIT(222, str_regex_split, "aa,bb.cc;dd", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(223, str_regex_split, " aa , bb . cc ; dd ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(224, str_regex_split, " aa ,bb . cc;dd: ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(225, str_regex_split, "aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(226, str_regex_split, " aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(227, str_regex_split, "abcd", "", "a", "b", "c", "d")

	TEST_SPLIT(228, regex_split, "a,b.c;d", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SPLIT(229, regex_split, " a , b . c ; d ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SPLIT(230, regex_split, " a ,b . c;d: ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SPLIT(231, regex_split, "a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SPLIT(232, regex_split, " a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_SPLIT(233, regex_split, "aa,bb.cc;dd", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SPLIT(234, regex_split, " aa , bb . cc ; dd ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SPLIT(235, regex_split, " aa ,bb . cc;dd: ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SPLIT(236, regex_split, "aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SPLIT(237, regex_split, " aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_SPLIT(238, regex_split, "abcd", "", "a", "b", "c", "d")

#endif

	/* Test join */

#define TEST_JOIN(i, action, str, len, value) \
	TEST_STR(i, action, str, len, value) \
	TEST_ACT(i, !str_destroy(str))

	TEST_ACT (239, list = list_make((list_destroy_t *)str_release, str_create("aaa"), str_create("bbb"), str_create("ccc"), str_create("ddd"), str_create("eee"), str_create("fff"), NULL))
	TEST_JOIN(240, a = str_join(list, NULL), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(241, a = str_join(list, ""), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(242, a = str_join(list, " "), a, 23, "aaa bbb ccc ddd eee fff")
	TEST_JOIN(243, a = str_join(list, ", "), a, 28, "aaa, bbb, ccc, ddd, eee, fff")
	TEST_ACT (244, !list_destroy(list))

	TEST_ACT (245, list = list_make((list_destroy_t *)str_release, str_create("aaa"), str_create("ccc"), str_create("eee"), NULL))
	TEST_ACT (246, list_insert(list, 0, NULL))
	TEST_ACT (247, list_insert(list, 2, NULL))
	TEST_ACT (248, list_insert(list, 5, NULL))
	TEST_JOIN(249, a = str_join(list, NULL), a, 9, "aaaccceee")
	TEST_JOIN(250, a = str_join(list, ""), a, 9, "aaaccceee")
	TEST_JOIN(251, a = str_join(list, " "), a, 14, " aaa  ccc eee ")
	TEST_JOIN(252, a = str_join(list, ", "), a, 19, ", aaa, , ccc, eee, ")
	TEST_ACT (253, !list_destroy(list))

	TEST_ACT (254, list = list_make(NULL, "aaa", "bbb", "ccc", "ddd", "eee", "fff", NULL))
	TEST_JOIN(255, a = join(list, NULL), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(256, a = join(list, ""), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(257, a = join(list, " "), a, 23, "aaa bbb ccc ddd eee fff")
	TEST_JOIN(258, a = join(list, ", "), a, 28, "aaa, bbb, ccc, ddd, eee, fff")
	TEST_ACT (259, !list_destroy(list))

	TEST_ACT (260, list = list_make(NULL, "aaa", "ccc", "eee", NULL))
	TEST_ACT (261, list_insert(list, 0, NULL))
	TEST_ACT (262, list_insert(list, 2, NULL))
	TEST_ACT (263, list_insert(list, 5, NULL))
	TEST_JOIN(264, a = join(list, NULL), a, 9, "aaaccceee")
	TEST_JOIN(265, a = join(list, ""), a, 9, "aaaccceee")
	TEST_JOIN(266, a = join(list, " "), a, 14, " aaa  ccc eee ")
	TEST_JOIN(267, a = join(list, ", "), a, 19, ", aaa, , ccc, eee, ")
	TEST_ACT (268, !list_destroy(list))

	/* Test trim */

#define TEST_SFUNC(i, func, str, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_ACT((i), func(a)) \
	CHECK_STR((i), func(str), a, len, val) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_FUNC(i, func, str, len, val) \
	strcpy(tst, (str)); \
	TEST_ACT((i), func(tst)) \
	CHECK_CSTR((i), func(str), tst, len, val)

	TEST_SFUNC(269, str_trim, "", 0, "")
	TEST_SFUNC(270, str_trim, " ", 0, "")
	TEST_SFUNC(271, str_trim, "  ", 0, "")
	TEST_SFUNC(272, str_trim, " \t ", 0, "")
	TEST_SFUNC(273, str_trim, "\r \t \n", 0, "")
	TEST_SFUNC(274, str_trim, "abcdef", 6, "abcdef")
	TEST_SFUNC(275, str_trim, " abcdef", 6, "abcdef")
	TEST_SFUNC(276, str_trim, "abcdef ", 6, "abcdef")
	TEST_SFUNC(277, str_trim, " abcdef ", 6, "abcdef")
	TEST_SFUNC(278, str_trim, "  abcdef  ", 6, "abcdef")
	TEST_SFUNC(279, str_trim, "  abc def  ", 7, "abc def")
	TEST_SFUNC(280, str_trim, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_SFUNC(281, str_trim, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_FUNC(282, trim, "", 0, "")
	TEST_FUNC(283, trim, " ", 0, "")
	TEST_FUNC(284, trim, "  ", 0, "")
	TEST_FUNC(285, trim, " \t ", 0, "")
	TEST_FUNC(286, trim, "\r \t \n", 0, "")
	TEST_FUNC(287, trim, "abcdef", 6, "abcdef")
	TEST_FUNC(288, trim, " abcdef", 6, "abcdef")
	TEST_FUNC(289, trim, "abcdef ", 6, "abcdef")
	TEST_FUNC(290, trim, " abcdef ", 6, "abcdef")
	TEST_FUNC(291, trim, "  abcdef  ", 6, "abcdef")
	TEST_FUNC(292, trim, "  abc def  ", 7, "abc def")
	TEST_FUNC(293, trim, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_FUNC(294, trim, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_SFUNC(295, str_trim_left, "", 0, "")
	TEST_SFUNC(296, str_trim_left, " ", 0, "")
	TEST_SFUNC(297, str_trim_left, "  ", 0, "")
	TEST_SFUNC(298, str_trim_left, " \t ", 0, "")
	TEST_SFUNC(299, str_trim_left, "\r \t \n", 0, "")
	TEST_SFUNC(300, str_trim_left, "abcdef", 6, "abcdef")
	TEST_SFUNC(301, str_trim_left, " abcdef", 6, "abcdef")
	TEST_SFUNC(302, str_trim_left, "abcdef ", 7, "abcdef ")
	TEST_SFUNC(303, str_trim_left, " abcdef ", 7, "abcdef ")
	TEST_SFUNC(304, str_trim_left, "  abcdef  ", 8, "abcdef  ")
	TEST_SFUNC(305, str_trim_left, "  abc def  ", 9, "abc def  ")
	TEST_SFUNC(306, str_trim_left, "abc def\v\t\f\n\r", 12, "abc def\v\t\f\n\r")
	TEST_SFUNC(307, str_trim_left, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_FUNC(308, trim_left, "", 0, "")
	TEST_FUNC(309, trim_left, " ", 0, "")
	TEST_FUNC(310, trim_left, "  ", 0, "")
	TEST_FUNC(311, trim_left, " \t ", 0, "")
	TEST_FUNC(312, trim_left, "\r \t \n", 0, "")
	TEST_FUNC(313, trim_left, "abcdef", 6, "abcdef")
	TEST_FUNC(314, trim_left, " abcdef", 6, "abcdef")
	TEST_FUNC(315, trim_left, "abcdef ", 7, "abcdef ")
	TEST_FUNC(316, trim_left, " abcdef ", 7, "abcdef ")
	TEST_FUNC(317, trim_left, "  abcdef  ", 8, "abcdef  ")
	TEST_FUNC(318, trim_left, "  abc def  ", 9, "abc def  ")
	TEST_FUNC(319, trim_left, "abc def\v\t\f\n\r", 12, "abc def\v\t\f\n\r")
	TEST_FUNC(320, trim_left, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_SFUNC(321, str_trim_right, "", 0, "")
	TEST_SFUNC(322, str_trim_right, " ", 0, "")
	TEST_SFUNC(323, str_trim_right, "  ", 0, "")
	TEST_SFUNC(324, str_trim_right, " \t ", 0, "")
	TEST_SFUNC(325, str_trim_right, "\r \t \n", 0, "")
	TEST_SFUNC(326, str_trim_right, "abcdef", 6, "abcdef")
	TEST_SFUNC(327, str_trim_right, " abcdef", 7, " abcdef")
	TEST_SFUNC(328, str_trim_right, "abcdef ", 6, "abcdef")
	TEST_SFUNC(329, str_trim_right, " abcdef ", 7, " abcdef")
	TEST_SFUNC(330, str_trim_right, "  abcdef  ", 8, "  abcdef")
	TEST_SFUNC(331, str_trim_right, "  abc def  ", 9, "  abc def")
	TEST_SFUNC(332, str_trim_right, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_SFUNC(333, str_trim_right, "\v\t\f\n\rabc def", 12, "\v\t\f\n\rabc def")

	TEST_FUNC(334, trim_right, "", 0, "")
	TEST_FUNC(335, trim_right, " ", 0, "")
	TEST_FUNC(336, trim_right, "  ", 0, "")
	TEST_FUNC(337, trim_right, " \t ", 0, "")
	TEST_FUNC(338, trim_right, "\r \t \n", 0, "")
	TEST_FUNC(339, trim_right, "abcdef", 6, "abcdef")
	TEST_FUNC(340, trim_right, " abcdef", 7, " abcdef")
	TEST_FUNC(341, trim_right, "abcdef ", 6, "abcdef")
	TEST_FUNC(342, trim_right, " abcdef ", 7, " abcdef")
	TEST_FUNC(343, trim_right, "  abcdef  ", 8, "  abcdef")
	TEST_FUNC(344, trim_right, "  abc def  ", 9, "  abc def")
	TEST_FUNC(345, trim_right, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_FUNC(346, trim_right, "\v\t\f\n\rabc def", 12, "\v\t\f\n\rabc def")

	/* Test squeeze */

	TEST_SFUNC(347, str_squeeze, "", 0, "")
	TEST_SFUNC(348, str_squeeze, " ", 0, "")
	TEST_SFUNC(349, str_squeeze, "  ", 0, "")
	TEST_SFUNC(350, str_squeeze, " \t ", 0, "")
	TEST_SFUNC(351, str_squeeze, "\r \t \n", 0, "")
	TEST_SFUNC(352, str_squeeze, "abcdef", 6, "abcdef")
	TEST_SFUNC(353, str_squeeze, " ab  cd  ef", 8, "ab cd ef")
	TEST_SFUNC(354, str_squeeze, "ab  cd  ef ", 8, "ab cd ef")
	TEST_SFUNC(355, str_squeeze, "   ab   cd   ef   ", 8, "ab cd ef")
	TEST_SFUNC(356, str_squeeze, "  abcdef  ", 6, "abcdef")
	TEST_SFUNC(357, str_squeeze, "  abc  def  ", 7, "abc def")
	TEST_SFUNC(358, str_squeeze, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_SFUNC(359, str_squeeze, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_FUNC(360, squeeze, "", 0, "")
	TEST_FUNC(361, squeeze, " ", 0, "")
	TEST_FUNC(362, squeeze, "  ", 0, "")
	TEST_FUNC(363, squeeze, " \t ", 0, "")
	TEST_FUNC(364, squeeze, "\r \t \n", 0, "")
	TEST_FUNC(365, squeeze, "abcdef", 6, "abcdef")
	TEST_FUNC(366, squeeze, " ab  cd  ef", 8, "ab cd ef")
	TEST_FUNC(367, squeeze, "ab  cd  ef ", 8, "ab cd ef")
	TEST_FUNC(368, squeeze, "   ab   cd   ef   ", 8, "ab cd ef")
	TEST_FUNC(369, squeeze, "  abcdef  ", 6, "abcdef")
	TEST_FUNC(370, squeeze, "  abc  def  ", 7, "abc def")
	TEST_FUNC(371, squeeze, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_FUNC(372, squeeze, "\v\t\f\n\rabc def", 7, "abc def")

	/* Test quote, unquote */

#define TEST_SQUOTE(i, str, quotable, quote_char, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_ACT((i), b = str_quote(a, quotable, quote_char)) \
	CHECK_STR((i), str_quote(str, quotable, quote_char), b, len, val) \
	TEST_ACT((i), c = str_unquote(b, quotable, quote_char)) \
	CHECK_STR((i), str_unquote(quoted(str), quotable, quote_char), c, strlen(str), str) \
	TEST_ACT((i), !str_destroy(c)) \
	TEST_ACT((i), !str_destroy(b)) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_QUOTE(i, str, quotable, quote_char, len, val) \
	TEST_ACT((i), a = quote(str, quotable, quote_char)) \
	CHECK_STR((i), quote(str, quotable, quote_char), a, len, val) \
	TEST_ACT((i), b = unquote(cstr(a), quotable, quote_char)) \
	CHECK_STR((i), unquote(quoted(str), quotable, quote_char), b, strlen(str), str) \
	TEST_ACT((i), !str_destroy(b)) \
	TEST_ACT((i), !str_destroy(a))

	TEST_SQUOTE(373, "", "\"", '\\', 0, "")
	TEST_QUOTE (374, "", "\"", '\\', 0, "")
	TEST_SQUOTE(375, "\"hello world\"", "\"", '\\', 15, "\\\"hello world\\\"")
	TEST_QUOTE (376, "\"hello world\"", "\"", '\\', 15, "\\\"hello world\\\"")
	TEST_QUOTE (377, "\"hello world\\\"", "\"\\", '\\', 17, "\\\"hello world\\\\\\\"")
	TEST_SQUOTE(378, "\"hello world\\\"", "\"\\", '\\', 17, "\\\"hello world\\\\\\\"")
	TEST_QUOTE (379, "\\\"hello\\\\world\\\"", "\"\\", '\\', 22, "\\\\\\\"hello\\\\\\\\world\\\\\\\"")
	TEST_SQUOTE(380, "\\\"hello\\\\world\\\"", "\"\\", '\\', 22, "\\\\\\\"hello\\\\\\\\world\\\\\\\"")
	TEST_SQUOTE(381, "\"hello\\ \\world\"", "\"\\", '\\', 19, "\\\"hello\\\\ \\\\world\\\"")
	TEST_QUOTE (382, "\"hello\\ \\world\"", "\"\\", '\\', 19, "\\\"hello\\\\ \\\\world\\\"")
	TEST_SQUOTE(383, "\"hello\\ \\world\"", "\"", '\\', 17, "\\\"hello\\ \\world\\\"")
	TEST_QUOTE (384, "\"hello\\ \\world\"", "\"", '\\', 17, "\\\"hello\\ \\world\\\"")

	/* Test encode, decode */

#define TEST_SENCODE(i, str, uncoded, coded, quote_char, printable, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_ACT((i), b = str_encode(a, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_encode(str, uncoded, coded, quote_char, printable), b, len, val) \
	TEST_ACT((i), c = str_decode(b, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_decode(encoded(str), uncoded, coded, quote_char, printabtle), c, strlen(str), str) \
	TEST_ACT((i), !str_destroy(c)) \
	TEST_ACT((i), !str_destroy(b)) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_ENCODE(i, str, uncoded, coded, quote_char, printable, len, val) \
	TEST_ACT((i), a = encode(str, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), encode(str, uncoded, coded, quote_char, printable), a, len, val) \
	TEST_ACT((i), b = decode(cstr(a), uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), decode(encoded(str), uncoded, coded, quote_char, printable), b, strlen(str), str) \
	TEST_ACT((i), !str_destroy(b)) \
	TEST_ACT((i), !str_destroy(a))

	TEST_SENCODE(385, "", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 0, "")
	TEST_SENCODE(386, "\a\b\t\n\v\f\r\033\\", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 20, "\\a\\b\\t\\n\\v\\f\\r\\x1b\\\\")
	TEST_SENCODE(387, "a\a\b\tb\n\v\f\rc\033\\d", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 24, "a\\a\\b\\tb\\n\\v\\f\\rc\\x1b\\\\d")
	TEST_SENCODE(388, "", "=", "=", '\\', 0, 0, "")
	TEST_SENCODE(389, "a=b", "=", "=", '\\', 0, 4, "a\\=b")

	TEST_ENCODE(390, "", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 0, "")
	TEST_ENCODE(391, "\a\b\t\n\v\f\r\033\\", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 20, "\\a\\b\\t\\n\\v\\f\\r\\x1b\\\\")
	TEST_ENCODE(392, "a\a\b\tb\n\v\f\rc\033\\d", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 24, "a\\a\\b\\tb\\n\\v\\f\\rc\\x1b\\\\d")
	TEST_ENCODE(393, "", "=", "=", '\\', 0, 0, "")
	TEST_ENCODE(394, "a=b", "=", "=", '\\', 0, 4, "a\\=b")

	/* Test lc, lcfirst */

	TEST_SFUNC(395, str_lc, "", 0, "")
	TEST_SFUNC(396, str_lc, "abc", 3, "abc")
	TEST_SFUNC(397, str_lc, " a b c ", 7, " a b c ")
	TEST_SFUNC(398, str_lc, "ABC", 3, "abc")
	TEST_SFUNC(399, str_lc, " A B C ", 7, " a b c ")
	TEST_SFUNC(400, str_lc, "0123456", 7, "0123456")

	TEST_FUNC(401, lc, "", 0, "")
	TEST_FUNC(402, lc, "abc", 3, "abc")
	TEST_FUNC(403, lc, " a b c ", 7, " a b c ")
	TEST_FUNC(404, lc, "ABC", 3, "abc")
	TEST_FUNC(405, lc, " A B C ", 7, " a b c ")
	TEST_FUNC(406, lc, "0123456", 7, "0123456")

	TEST_SFUNC(407, str_lcfirst, "", 0, "")
	TEST_SFUNC(408, str_lcfirst, "abc", 3, "abc")
	TEST_SFUNC(409, str_lcfirst, " a b c ", 7, " a b c ")
	TEST_SFUNC(410, str_lcfirst, "ABC", 3, "aBC")
	TEST_SFUNC(411, str_lcfirst, " A B C ", 7, " A B C ")
	TEST_SFUNC(412, str_lcfirst, "0123456", 7, "0123456")

	TEST_FUNC(413, lcfirst, "", 0, "")
	TEST_FUNC(414, lcfirst, "abc", 3, "abc")
	TEST_FUNC(415, lcfirst, " a b c ", 7, " a b c ")
	TEST_FUNC(416, lcfirst, "ABC", 3, "aBC")
	TEST_FUNC(417, lcfirst, " A B C ", 7, " A B C ")
	TEST_FUNC(418, lcfirst, "0123456", 7, "0123456")

	/* Test uc, ucfirst */

	TEST_SFUNC(419, str_uc, "", 0, "")
	TEST_SFUNC(420, str_uc, "abc", 3, "ABC")
	TEST_SFUNC(421, str_uc, " a b c ", 7, " A B C ")
	TEST_SFUNC(422, str_uc, "ABC", 3, "ABC")
	TEST_SFUNC(423, str_uc, " A B C ", 7, " A B C ")
	TEST_SFUNC(424, str_uc, "0123456", 7, "0123456")

	TEST_FUNC(425, uc, "", 0, "")
	TEST_FUNC(426, uc, "abc", 3, "ABC")
	TEST_FUNC(427, uc, " a b c ", 7, " A B C ")
	TEST_FUNC(428, uc, "ABC", 3, "ABC")
	TEST_FUNC(429, uc, " A B C ", 7, " A B C ")
	TEST_FUNC(430, uc, "0123456", 7, "0123456")

	TEST_SFUNC(431, str_ucfirst, "", 0, "")
	TEST_SFUNC(432, str_ucfirst, "abc", 3, "Abc")
	TEST_SFUNC(433, str_ucfirst, " a b c ", 7, " a b c ")
	TEST_SFUNC(434, str_ucfirst, "ABC", 3, "ABC")
	TEST_SFUNC(435, str_ucfirst, " A B C ", 7, " A B C ")
	TEST_SFUNC(436, str_ucfirst, "0123456", 7, "0123456")

	TEST_FUNC(437, ucfirst, "", 0, "")
	TEST_FUNC(438, ucfirst, "abc", 3, "Abc")
	TEST_FUNC(439, ucfirst, " a b c ", 7, " a b c ")
	TEST_FUNC(440, ucfirst, "ABC", 3, "ABC")
	TEST_FUNC(441, ucfirst, " A B C ", 7, " A B C ")
	TEST_FUNC(442, ucfirst, "0123456", 7, "0123456")

	/* Test chop */

#define TEST_SFUNC_EQ(i, func, str, eq, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_EQ((i), func(a), eq) \
	CHECK_STR((i), func(str), a, len, val) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_FUNC_EQ(i, func, str, eq, len, val) \
	strcpy(tst, (str)); \
	TEST_EQ((i), func(tst), eq) \
	CHECK_CSTR((i), func(str), tst, len, val)

	TEST_SFUNC_EQ(443, str_chop, "abcdef", 'f', 5, "abcde")
	TEST_SFUNC_EQ(444, str_chop, "abcde", 'e',4, "abcd")
	TEST_SFUNC_EQ(445, str_chop, "abcd", 'd', 3, "abc")
	TEST_SFUNC_EQ(446, str_chop, "abc", 'c', 2, "ab")
	TEST_SFUNC_EQ(447, str_chop, "ab", 'b', 1, "a")
	TEST_SFUNC_EQ(448, str_chop, "a", 'a', 0, "")
	TEST_SFUNC_EQ(449, str_chop, "", -1, 0, "")

	TEST_FUNC_EQ(450, chop, "abcdef", 'f', 5, "abcde")
	TEST_FUNC_EQ(451, chop, "abcde", 'e', 4, "abcd")
	TEST_FUNC_EQ(452, chop, "abcd", 'd', 3, "abc")
	TEST_FUNC_EQ(453, chop, "abc", 'c', 2, "ab")
	TEST_FUNC_EQ(454, chop, "ab", 'b', 1, "a")
	TEST_FUNC_EQ(455, chop, "a", 'a', 0, "")
	TEST_FUNC_EQ(456, chop, "", -1, 0, "")

	/* Test chomp */

	TEST_SFUNC_EQ(457, str_chomp, "abcdef", 0, 6, "abcdef")
	TEST_SFUNC_EQ(458, str_chomp, "abcdef ", 0, 7, "abcdef ")
	TEST_SFUNC_EQ(459, str_chomp, "abcdef \n", 1, 7, "abcdef ")
	TEST_SFUNC_EQ(460, str_chomp, "abcdef \n\r", 2, 7, "abcdef ")

	TEST_FUNC_EQ(461, chomp, "abcdef", 0, 6, "abcdef")
	TEST_FUNC_EQ(462, chomp, "abcdef ", 0, 7, "abcdef ")
	TEST_FUNC_EQ(463, chomp, "abcdef \n", 1, 7, "abcdef ")
	TEST_FUNC_EQ(464, chomp, "abcdef \n\r", 2, 7, "abcdef ")

	/* Test bin, hex, oct */

#define TEST_SNUM(i, func, str, eq) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_EQ((i), func(a), eq) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_NUM(i, func, str, eq) \
	strcpy(tst, str); \
	TEST_EQ((i), func(tst), eq)

	TEST_SNUM(465, str_bin, "010", 2)
	TEST_SNUM(466, str_oct, "010", 8)
	TEST_SNUM(467, str_hex, "010", 16)

	TEST_NUM(468, bin, "010", 2)
	TEST_NUM(469, oct, "010", 8)
	TEST_NUM(470, hex, "010", 16)

	TEST_SNUM(471, str_bin, "11111111111111111111111111111111", -1)
	TEST_SNUM(472, str_oct, "037777777777", -1)
	TEST_SNUM(473, str_hex, "ffffffff", -1)

	TEST_NUM(474, bin, "11111111111111111111111111111111", -1)
	TEST_NUM(475, oct, "037777777777", -1)
	TEST_NUM(476, hex, "ffffffff", -1)

	TEST_SNUM(477, str_bin, "0b1010", 10)
	TEST_SNUM(478, str_hex, "0xa", 10)
	TEST_SNUM(479, str_oct, "012", 10)
	TEST_SNUM(480, str_oct, "0b1010", 10)
	TEST_SNUM(481, str_oct, "0xa", 10)

	TEST_NUM(482, bin, "0b1010", 10)
	TEST_NUM(483, hex, "0xa", 10)
	TEST_NUM(484, oct, "012", 10)
	TEST_NUM(485, oct, "0b1010", 10)
	TEST_NUM(486, oct, "0xa", 10)

	/* Test cstr, str_set_length and str_recalc_length */

#define TEST_SET(i, action, str, eq, len, val) \
	TEST_EQ((i), action, eq) \
	CHECK_STR((i), action, str, eq, val)

	TEST_ACT(487, a = str_create("0123456789"))
	TEST_SET(488, (cstr(a)[5] = '\0', str_recalc_length(a)), a, 5, 5, "01234")
	TEST_SET(489, (cstr(a)[5] =  '5', str_recalc_length(a)), a, 10, 10, "0123456789")
	TEST_SET(490, (cstr(a)[5] = '\0', str_set_length(a, 8)), a, 8, 8, "01234\00067")
	TEST_SET(491, (cstr(a)[5] =  '5', str_set_length(a, 7)), a, 7, 7, "0123456\000")
	TEST_SET(492, (cstr(a)[5] = '\0', str_set_length(a, 6)), a, 6, 6, "01234\000")
	TEST_ACT(493, !str_destroy(a))

	/* Test strings containing nuls and high bit characters */

#define TEST_ZFUNC(i, func, origlen, str, newlen, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_ACT((i), func(a)) \
	CHECK_STR((i), func(str), a, newlen, val) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_ZSPLIT(i, origlen, str, delim, tok1, tok2, tok3, tok4) \
	TEST_ACT(i, a = substr(str, 0, origlen)) \
	TEST_ACT(i, list = str_split(a, delim)) \
	if (list_length(list) != 4) \
	{ \
		++errors; \
		printf("Test%d: str_split(\"", i); \
		str_print(str, origlen); \
		printf("\", \"%s\") failed (%d tokens, not %d)\n", delim, list_length(list), 4); \
	} \
	CHECK_SPLIT_ITEM(i, str_split, str, delim, 0, tok1) \
	CHECK_SPLIT_ITEM(i, str_split, str, delim, 1, tok2) \
	CHECK_SPLIT_ITEM(i, str_split, str, delim, 2, tok3) \
	CHECK_SPLIT_ITEM(i, str_split, str, delim, 3, tok4) \
	TEST_ACT(i, !str_destroy(a)) \
	TEST_ACT(i, !list_destroy(list))

#define TEST_ZQUOTE(i, origlen, str, quotable, quote_char, len, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_ACT((i), b = str_quote(a, quotable, quote_char)) \
	CHECK_STR((i), str_quote(str, quotable, quote_char), b, len, val) \
	TEST_ACT((i), c = str_unquote(b, quotable, quote_char)) \
	CHECK_STR((i), str_unquote(quoted(str), quotable, quote_char), c, origlen, str) \
	TEST_ACT((i), !str_destroy(c)) \
	TEST_ACT((i), !str_destroy(b)) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_ZENCODE(i, origlen, str, uncoded, coded, quote_char, printable, len, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_ACT((i), b = str_encode(a, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_encode(str, uncoded, coded, quote_char, printable), b, len, val) \
	TEST_ACT((i), c = str_decode(b, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_decode(encoded(str), uncoded, coded, quote_char, printable), c, origlen, str) \
	TEST_ACT((i), !str_destroy(c)) \
	TEST_ACT((i), !str_destroy(b)) \
	TEST_ACT((i), !str_destroy(a))

#define TEST_ZCHOMP(i, func, origlen, str, eq, newlen, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_EQ((i), func(a), eq) \
	CHECK_STR((i), func(str), a, newlen, val) \
	TEST_ACT((i), !str_destroy(a))

	TEST_STR(494, a = str_create("%c", '\0'), a, 1, "\000")
	TEST_ACT(495, !str_empty(a))
	TEST_STR(496, b = str_create("abc%cdef\376", '\0'), b, 8, "abc\000def\376")
	TEST_STR(497, c = str_copy(b), c, 8, "abc\000def\376")
	TEST_STR(498, str_remove(b, 0), b, 7, "bc\000def\376")
	TEST_STR(499, str_remove_range(c, 1, 2), c, 6, "a\000def\376")
	TEST_STR(500, str_insert(a, 1, "a%c\376b", '\0'), a, 5, "\000a\000\376b")
	TEST_STR(501, str_insert_str(a, 1, b), a, 12, "\000bc\000def\376a\000\376b")
	TEST_STR(502, str_prepend(a, "a%c\376b", '\0'), a, 16, "a\000\376b\000bc\000def\376a\000\376b")
	TEST_STR(503, str_prepend_str(a, b), a, 23, "bc\000def\376a\000\376b\000bc\000def\376a\000\376b")
	TEST_STR(504, str_append(a, "%c\376", '\0'), a, 25, "bc\000def\376a\000\376b\000bc\000def\376a\000\376b\000\376")
	TEST_STR(505, str_append_str(a, c), a, 31, "bc\000def\376a\000\376b\000bc\000def\376a\000\376b\000\376a\000def\376")
	TEST_STR(506, str_replace(a, 3, 5, "%c\376", '\0'), a, 28, "bc\000\000\376\000\376b\000bc\000def\376a\000\376b\000\376a\000def\376")
	TEST_STR(507, str_replace_str(a, 3, 5, c), a, 29, "bc\000a\000def\376\000bc\000def\376a\000\376b\000\376a\000def\376")
	TEST_ACT(508, !str_destroy(c))
	TEST_STR(509, c = str_repeat(10, "1%c1", '\0'), c, 30, "1\00011\00011\00011\00011\00011\00011\00011\00011\00011\0001")
	TEST_ACT(510, !str_destroy(c))
	TEST_STR(511, c = str_substr(a, 5, 5), c, 5, "def\376\000")
	TEST_ACT(512, !str_destroy(c))
	TEST_STR(513, c = substr(cstr(a), 5, 5), c, 5, "def\376\000")
	TEST_ACT(514, !str_destroy(c))
	TEST_STR(515, c = str_splice(a, 5, 5), c, 5, "def\376\000")
	CHECK_STR(516, str_splice(a, 5, 5), a, 24, "bc\000a\000bc\000def\376a\000\376b\000\376a\000def\376")
	TEST_ACT(517, !str_destroy(c))
	TEST_ACT(518, !str_destroy(b))
	TEST_TR_DIRECT(519, str_tr(a, "a-z", "A-EfG-Z", 0), a, 14, 24, "BC\000A\000BC\000DEf\376A\000\376B\000\376A\000DEf\376")
	TEST_ACT(520, b = str_create("%c", '\0'))
	TEST_ACT(521, c = str_create("%c", '\0'))
	TEST_TR_DIRECT(522, str_tr_str(a, b, c, 0), a, 6, 24, "BC\000A\000BC\000DEf\376A\000\376B\000\376A\000DEf\376")
	TEST_ACT(523, !str_destroy(c))
	TEST_ACT(524, !str_destroy(b))
	TEST_ACT(525, !str_destroy(a))
	TEST_ZSPLIT(526, 9, "\000a\376\000a\376a\000\376", "a", "\000", "\376\000", "\376", "\000\376")
	TEST_ACT(527, list = list_make((list_destroy_t *)str_release, str_create("%c\376", '\0'), str_create("\376%c", '\0'), str_create("%c%c", '\0', '\0'), NULL))
	TEST_JOIN(528, a = str_join(list, "a"), a, 8, "\000\376a\376\000a\000\000")
	TEST_ACT(529, !list_destroy(list))
	TEST_ZFUNC(530, str_trim, 12, "  abc\000\376def  ", 8, "abc\000\376def")
	TEST_ZFUNC(531, str_trim_left, 12, "  abc\000\376def  ", 10, "abc\000\376def  ")
	TEST_ZFUNC(532, str_trim_right, 12, "  abc\000\376def  ", 10, "  abc\000\376def")
	TEST_ZFUNC(533, str_squeeze, 19, "   ab \000 cd\376   ef   ", 11, "ab \000 cd\376 ef")
	TEST_ZQUOTE(534, 16, "\\\"hell\000\\\\w\376rld\\\"", "\"\\", '\\', 22, "\\\\\\\"hell\000\\\\\\\\w\376rld\\\\\\\"")
	TEST_ZENCODE(535, 11, "\a\b\t\n\0\376\v\f\r\033\\", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 28, "\\a\\b\\t\\n\\x00\\xfe\\v\\f\\r\\x1b\\\\")
	TEST_ZFUNC(536, str_lc, 7, "ABC\000DEF", 7, "abc\000def")
	TEST_ZFUNC(537, str_lcfirst, 7, "ABC\000DEF", 7, "aBC\000DEF")
	TEST_ZFUNC(538, str_uc, 7, "abc\000def", 7, "ABC\000DEF")
	TEST_ZFUNC(539, str_ucfirst, 7, "abc\000def", 7, "Abc\000def")
	TEST_ZFUNC(540, str_chop, 7, "abc\000def", 6, "abc\000de")
	TEST_ZCHOMP(541, str_chomp, 7, "abc\000def", 0, 7, "abc\000def")
	TEST_ZCHOMP(542, str_chomp, 7, "abc\000de\n", 1, 6, "abc\000de")

	/* Test error reporting */

	TEST_ACT(543, !str_copy(NULL))
	TEST_ACT(544, !cstr(NULL))
	TEST_EQ (545, str_set_length(NULL, 0), -1)
	TEST_ACT(546, a = str_create(NULL))
	TEST_EQ (547, str_set_length(a, 33), -1)
	TEST_EQ (548, str_set_length(a, 1), -1)
	TEST_EQ (549, str_recalc_length(NULL), -1)
	TEST_ACT(550, !str_remove_range(NULL, 0, 0))
	TEST_ACT(551, !str_remove_range(a, 1, 0))
	TEST_ACT(552, !str_remove_range(a, 0, 1))
	TEST_ACT(553, !str_insert(NULL, 0, ""))
	TEST_ACT(554, !str_insert(a, 1, ""))
	TEST_ACT(555, !str_insert_str(a, 0, NULL))
	TEST_ACT(556, !str_replace(NULL, 0, 0, ""))
	TEST_ACT(557, !str_replace(a, 0, 1, ""))
	TEST_ACT(558, !str_replace(a, 1, 0, ""))
	TEST_ACT(559, !str_replace_str(NULL, 0, 0, NULL))
	TEST_ACT(560, !str_substr(NULL, 0, 0))
	TEST_ACT(561, !str_substr(a, 0, 1))
	TEST_ACT(562, !str_substr(a, 1, 0))
	TEST_ACT(563, !substr(NULL, 0, 0))
	TEST_ACT(564, !str_splice(NULL, 0, 0))
	TEST_ACT(565, !str_splice(a, 0, 1))
	TEST_ACT(566, !str_splice(a, 1, 0))
	TEST_EQ (567, str_tr(NULL, "a-z", "A-Z", 0), -1)
	TEST_EQ (568, str_tr(a, NULL, "A-Z", 0), -1)
	TEST_EQ (569, str_tr(a, "z-a", "A-Z", 0), -1)
	TEST_EQ (570, str_tr(a, "a-z", "Z-A", 0), -1)
	TEST_ACT(571, b = str_create("a-z"))
	TEST_ACT(572, c = str_create("A-Z"))
	TEST_EQ (573, str_tr_str(NULL, b, c, 0), -1)
	TEST_EQ (574, str_tr_str(a, NULL, c, 0), -1)
	TEST_ACT(575, !str_destroy(c))
	TEST_ACT(576, c = str_create("Z-A"))
	TEST_EQ (577, str_tr_str(a, b, c, 0), -1)
	TEST_ACT(578, !str_destroy(b))
	TEST_ACT(579, b = str_create("z-a"))
	TEST_EQ (580, str_tr_str(a, b, c, 0), -1)
	TEST_ACT(581, !str_destroy(b))
	TEST_ACT(582, !str_destroy(c))
	TEST_EQ (583, tr(NULL, "a-z", "A-Z", 0), -1)
	TEST_EQ (584, tr(tst, NULL, "A-Z", 0), -1)
	TEST_EQ (585, tr(tst, "z-a", "A-Z", 0), -1)
	TEST_EQ (586, tr(tst, "a-z", "Z-A", 0), -1)
	TEST_EQ (587, tr(tst, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", "a-z", 0), -1)
	TEST_EQ (588, tr(tst, "a-z", "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", 0), -1)
	TEST_EQ (589, str_tr_compiled(NULL, NULL), -1)
	TEST_EQ (590, str_tr_compiled(a, NULL), -1)
	TEST_EQ (591, tr_compiled(NULL, NULL), -1)
	TEST_EQ (592, tr_compiled("", NULL), -1)
#ifndef REGEX_MISSING
	TEST_ACT(593, !str_regex(NULL, a, 0, 0))
	TEST_ACT(594, !str_regex("", NULL, 0, 0))
	TEST_ACT(595, !regex(NULL, "", 0, 0))
	TEST_ACT(596, !regex("", NULL, 0, 0))
	TEST_EQ (597, regex_compile(re, NULL, 0), REG_BADPAT)
	TEST_EQ (598, regex_compile(NULL, "", 0), REG_BADPAT)
	TEST_ACT(599, !regex_compiled(re, NULL, 0))
	TEST_ACT(600, !regex_compiled(NULL, "", 0))
	TEST_ACT(601, a = str_create("aaa"))
	TEST_ACT(601, !str_regsub(NULL, "", a, 0, 0, 0))
	TEST_ACT(602, !str_regsub("", NULL, a, 0, 0, 0))
	TEST_ACT(603, !str_regsub("", "", NULL, 0, 0, 0))
	TEST_ACT(604, !str_regsub_compiled(NULL, "", a, 0, 0))
	TEST_ACT(605, !str_regsub_compiled(re, NULL, a, 0, 0))
	TEST_EQ (606, regex_compile(re, ".+", 0), 0)
	TEST_ACT(607, !str_regsub_compiled(re, "$", NULL, 0, 0))
	TEST_ACT(608, !str_regsub_compiled(re, "$a", NULL, 0, 0))
	TEST_ACT(609, !str_regsub_compiled(re, "${0", NULL, 0, 0))
	TEST_ACT(610, !str_regsub_compiled(re, "${", NULL, 0, 0))
	TEST_ACT(611, !str_regsub_compiled(re, "${33}", NULL, 0, 0))
	TEST_ACT(612, !str_regsub_compiled(re, "${-12}", NULL, 0, 0))
	TEST_ACT(613, !str_regsub_compiled(re, "${a}", NULL, 0, 0))
	TEST_ACT(614, !str_regsub_compiled(re, "\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q$0\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E", a, 0, 0))
	TEST_ACT(615, !str_regsub_compiled(re, "\\E", a, 0, 0))
	regex_release(re);
#endif
	TEST_ACT(616, !str_destroy(a))
	TEST_ACT(617, a = str_create("hello"))
	TEST_ACT(618, !str_fmt(NULL, 10, ALIGN_LEFT))
	TEST_ACT(619, !str_fmt(a, -1, ALIGN_LEFT))
	TEST_ACT(620, !str_fmt(a, 10, '@'))
	TEST_ACT(621, !str_destroy(a))
	TEST_ACT(622, !fmt(NULL, 10, ALIGN_LEFT))
	TEST_ACT(623, !fmt("hello", -1, ALIGN_LEFT))
	TEST_ACT(624, !fmt("hello", 10, '@'))
	TEST_ACT(625, !str_split(NULL, ""))
	TEST_ACT(626, !str_split(a, NULL))
	TEST_ACT(627, !split(NULL, ""))
	TEST_ACT(628, !split("", NULL))
#ifndef REGEX_MISSING
	TEST_ACT(629, !str_regex_split(NULL, ""))
	TEST_ACT(630, !str_regex_split(a, NULL))
	TEST_ACT(631, !regex_split(NULL, ""))
	TEST_ACT(632, !regex_split("", NULL))
#endif
	TEST_ACT(633, !str_join(NULL, " "))
	TEST_ACT(634, !join(NULL, " "))
	TEST_ACT(635, !str_trim(NULL))
	TEST_ACT(636, !trim(NULL))
	TEST_ACT(637, !str_trim_left(NULL))
	TEST_ACT(638, !trim_left(NULL))
	TEST_ACT(639, !str_trim_right(NULL))
	TEST_ACT(640, !trim_right(NULL))
	TEST_ACT(641, !str_squeeze(NULL))
	TEST_ACT(642, !squeeze(NULL))
	TEST_ACT(643, !str_quote(NULL, "", '\\'))
	TEST_ACT(644, !str_quote(a, NULL, '\\'))
	TEST_ACT(645, !quote(NULL, "", '\\'))
	TEST_ACT(646, !quote("", NULL, '\\'))
	TEST_ACT(647, !str_unquote(NULL, "", '\\'))
	TEST_ACT(648, !str_unquote(a, NULL, '\\'))
	TEST_ACT(649, !unquote(NULL, "", '\\'))
	TEST_ACT(650, !unquote("", NULL, '\\'))
	TEST_ACT(651, !str_encode(NULL, "", "", '\\', 0))
	TEST_ACT(652, !str_encode(a, NULL, "", '\\', 0))
	TEST_ACT(653, !str_encode(a, "", NULL, '\\', 0))
	TEST_ACT(654, !encode(NULL, "", "", '\\', 0))
	TEST_ACT(655, !encode("", NULL, "", '\\', 0))
	TEST_ACT(656, !encode("", "", NULL, '\\', 0))
	TEST_ACT(657, !str_decode(NULL, "", "", '\\', 0))
	TEST_ACT(658, !str_decode(a, NULL, "", '\\', 0))
	TEST_ACT(659, !str_decode(a, "", NULL, '\\', 0))
	TEST_ACT(660, !decode(NULL, "", "", '\\', 0))
	TEST_ACT(661, !decode("", NULL, "", '\\', 0))
	TEST_ACT(662, !decode("", "", NULL, '\\', 0))
	TEST_ACT(663, !str_lc(NULL))
	TEST_ACT(664, !lc(NULL))
	TEST_ACT(665, !str_lcfirst(NULL))
	TEST_ACT(666, !lcfirst(NULL))
	TEST_ACT(667, !str_uc(NULL))
	TEST_ACT(668, !uc(NULL))
	TEST_ACT(669, !str_ucfirst(NULL))
	TEST_ACT(670, !ucfirst(NULL))
	TEST_EQ (671, str_chop(NULL), -1)
	TEST_EQ (672, str_chop(a), -1)
	TEST_EQ (673, chop(NULL), -1)
	TEST_EQ (674, chop(""), -1)
	TEST_EQ (675, str_chomp(NULL), -1)
	TEST_EQ (676, chomp(NULL), -1)
	TEST_EQ (677, str_bin(NULL), 0)
	TEST_ACT(678, b = str_create("123456789!"))
	TEST_EQ (679, str_bin(b), 0)
	TEST_EQ (680, bin(NULL), 0)
	TEST_EQ (681, bin("123456789!"), 0)
	TEST_EQ (682, str_hex(NULL), 0)
	TEST_EQ (683, str_hex(b), 0)
	TEST_EQ (684, hex(NULL), 0)
	TEST_EQ (685, hex("123456789!"), 0)
	TEST_EQ (686, str_oct(NULL), 0)
	TEST_EQ (687, str_oct(b), 0)
	TEST_EQ (688, oct(NULL), 0)
	TEST_EQ (689, oct("123456789!"), 0)
	TEST_ACT(690, !str_destroy(b))
	TEST_ACT(691, !str_destroy(a))

	if (errors)
		printf("%d/691 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
