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

/*
* $OpenBSD: strlcpy.c,v 1.4 1999/05/01 18:56:41 millert Exp $
* $OpenBSD: strlcat.c,v 1.5 2001/01/13 16:17:24 millert Exp $
* Modified by raf <raf2@raf.org>
*
* Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
* THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    String *str_create_locked(Locker *locker, const char *fmt, ...);
    String *str_vcreate(const char *fmt, va_list args);
    String *str_vcreate_locked(Locker *locker, const char *fmt, va_list args);
    String *str_create_sized(size_t size, const char *fmt, ...);
    String *str_create_locked_sized(Locker *locker, size_t size, const char *fmt, ...);
    String *str_vcreate_sized(size_t size, const char *fmt, va_list args);
    String *str_vcreate_locked_sized(Locker *locker, size_t size, const char *fmt, va_list args);
    String *str_copy(const String *str);
    String *str_copy_locked(Locker *locker, const String *str);
    String *str_fgetline(FILE *stream);
    String *str_fgetline_locked(Locker *locker, FILE *stream);
    void str_release(String *str);
    void *str_destroy(String **str);
    int str_rdlock(const String *str);
    int str_wrlock(const String *str);
    int str_unlock(const String *str);
    int str_empty(const String *str);
    size_t str_length(const String *str);
    char *cstr(const String *str);
    ssize_t str_set_length(String *str, size_t length);
    ssize_t str_set_length_unlocked(String *str, size_t length);
    ssize_t str_recalc_length(String *str);
    ssize_t str_recalc_length_unlocked(String *str);
    String *str_clear(String *str);
    String *str_remove(String *str, ssize_t index);
    String *str_remove_range(String *str, ssize_t index, ssize_t range);
    String *str_insert(String *str, ssize_t index, const char *fmt, ...);
    String *str_vinsert(String *str, ssize_t index, const char *fmt, va_list args);
    String *str_insert_str(String *str, ssize_t index, const String *src);
    String *str_append(String *str, const char *fmt, ...);
    String *str_vappend(String *str, const char *fmt, va_list args);
    String *str_append_str(String *str, const String *src);
    String *str_prepend(String *str, const char *fmt, ...);
    String *str_vprepend(String *str, const char *fmt, va_list args);
    String *str_prepend_str(String *str, const String *src);
    String *str_replace(String *str, ssize_t index, ssize_t range, const char *fmt, ...);
    String *str_vreplace(String *str, ssize_t index, ssize_t range, const char *fmt, va_list args);
    String *str_replace_str(String *str, ssize_t index, ssize_t range, const String *src);
    String *str_substr(const String *str, ssize_t index, ssize_t range);
    String *str_substr_locked(Locker *locker, const String *str, ssize_t index, ssize_t range);
    String *substr(const char *str, ssize_t index, ssize_t range);
    String *substr_locked(Locker *locker, const char *str, ssize_t index, ssize_t range);
    String *str_splice(String *str, ssize_t index, ssize_t range);
    String *str_splice_locked(Locker *locker, String *str, ssize_t index, ssize_t range);
    String *str_repeat(size_t count, const char *fmt, ...);
    String *str_repeat_locked(Locker *locker, size_t count, const char *fmt, ...);
    String *str_vrepeat(size_t count, const char *fmt, va_list args);
    String *str_vrepeat_locked(Locker *locker, size_t count, const char *fmt, va_list args);
    int str_tr(String *str, const char *from, const char *to, int option);
    int str_tr_str(String *str, const String *from, const String *to, int option);
    int tr(char *str, const char *from, const char *to, int option);
    StrTR *str_tr_compile(const String *from, const String *to, int option);
    StrTR *str_tr_compile_locked(Locker *locker, const String *from, const String *to, int option);
    StrTR *tr_compile(const char *from, const char *to, int option);
    StrTR *tr_compile_locked(Locker *locker, const char *from, const char *to, int option);
    void tr_release(StrTR *tr);
    void *tr_destroy(StrTR **tr);
    int str_tr_compiled(String *str, StrTR *table);
    int tr_compiled(char *str, StrTR *table);
    List *str_regexpr(const char *pattern, const String *text, int cflags, int eflags);
    List *str_regexpr_locked(Locker *locker, const char *pattern, const String *text, int cflags, int eflags);
    List *regexpr(const char *pattern, const char *text, int cflags, int eflags);
    List *regexpr_locked(Locker *locker, const char *pattern, const char *text, int cflags, int eflags);
    int regexpr_compile(regex_t *compiled, const char *pattern, int cflags);
    void regexpr_release(regex_t *compiled);
    List *str_regexpr_compiled(const regex_t *compiled, const String *text, int eflags);
    List *str_regexpr_compiled_locked(Locker *locker, const regex_t *compiled, const String *text, int eflags);
    List *regexpr_compiled(const regex_t *compiled, const char *text, int eflags);
    List *regexpr_compiled_locked(Locker *locker, const regex_t *compiled, const char *text, int eflags);
    String *str_regsub(const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all);
    String *str_regsub_compiled(const regex_t *compiled, const char *replacement, String *text, int eflags, int all);
    List *str_fmt(const String *str, size_t line_width, StrAlignment alignment);
    List *str_fmt_locked(Locker *locker, const String *str, size_t line_width, StrAlignment alignment);
    List *fmt(const char *str, size_t line_width, StrAlignment alignment);
    List *fmt_locked(Locker *locker, const char *str, size_t line_width, StrAlignment alignment);
    List *str_split(const String *str, const char *delim);
    List *str_split_locked(Locker *locker, const String *str, const char *delim);
    List *split(const char *str, const char *delim);
    List *split_locked(Locker *locker, const char *str, const char *delim);
    List *str_regexpr_split(const String *str, const char *delim, int cflags, int eflags);
    List *str_regexpr_split_locked(Locker *locker, const String *str, const char *delim, int cflags, int eflags);
    List *regexpr_split(const char *str, const char *delim, int cflags, int eflags);
    List *regexpr_split_locked(Locker *locker, const char *str, const char *delim, int cflags, int eflags);
    String *str_join(const List *str, const char *delim);
    String *str_join_locked(Locker *locker, const List *str, const char *delim);
    String *join(const List *str, const char *delim);
    String *join_locked(Locker *locker, const List *str, const char *delim);
    String *str_trim(String *str);
    char *trim(char *str);
    String *str_trim_left(String *str);
    char *trim_left(char *str);
    String *str_trim_right(String *str);
    char *trim_right(char *str);
    String *str_squeeze(String *str);
    char *squeeze(char *str);
    String *str_quote(const String *str, const char *quotable, char quote_char);
    String *str_quote_locked(Locker *locker, const String *str, const char *quotable, char quote_char);
    String *quote(const char *str, const char *quotable, char quote_char);
    String *quote_locked(Locker *locker, const char *str, const char *quotable, char quote_char);
    String *str_unquote(const String *str, const char *quotable, char quote_char);
    String *str_unquote_locked(Locker *locker, const String *str, const char *quotable, char quote_char);
    String *unquote(const char *str, const char *quotable, char quote_char);
    String *unquote_locked(Locker *locker, const char *str, const char *quotable, char quote_char);
    String *str_encode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *str_encode_locked(Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *str_decode(const String *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *str_decode_locked(Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *encode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *encode_locked(Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *decode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable);
    String *decode_locked(Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable);
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
    int strcasecmp(const char *s1, const char *s2);
    int strncasecmp(const char *s1, const char *s2, size_t n);
    size_t strlcpy(char *dst, const char *src, size_t size);
    size_t strlcat(char *dst, const char *src, size_t size);

=head1 DESCRIPTION

This module provides text strings that grow and shrink automatically and
functions for manipulating them. Some of the functions were modelled on the
I<list(3)> module. Others were modelled on the string functions and
operators in I<perlfunc(1)> and I<perlop(1)>. Others came from OpenBSD.

=over 4

=cut

*/

#include "std.h"

#include "err.h"
#include "str.h"
#include "mem.h"
#include "fio.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

/* Minimum string length: must be a power of 2 */

static const size_t MIN_STRING_SIZE = 32;

/* Maximum bytes for an empty string: must be a power of 2 greater than MIN_STRING_SIZE */

static const size_t MIN_EMPTY_STRING_SIZE = 1024;

struct String
{
	size_t size;    /* number of bytes allocated */
	size_t length;  /* number of bytes used (including nul) */
	char *str;      /* vector of characters */
	Locker *locker; /* locking strategy for this string */
};

#define CHARSET 256

struct StrTR
{
	int squash;           /* whether or not to squash duplicate characters */
	short table[CHARSET]; /* the translation table */
	Locker *locker;       /* locking strategy for this structure */
};

typedef enum
{
	TRCODE_NOMAP = -1,
	TRCODE_DELETE = -2
}
TRCode;

#define is_alnum(c)  isalnum((int)(unsigned char)(c))
#define is_print(c)  isprint((int)(unsigned char)(c))
#define is_space(c)  isspace((int)(unsigned char)(c))
#define is_digit(c)  isdigit((int)(unsigned char)(c))
#define is_xdigit(c) isxdigit((int)(unsigned char)(c))
#define to_lower(c)  tolower((int)(unsigned char)(c))
#define to_upper(c)  toupper((int)(unsigned char)(c))

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
		return mem_resize(&str->str, str->size) ? 0 : -1;

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
		return mem_resize(&str->str, str->size) ? 0 : -1;

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
	memmove(str->str + index, str->str + index + range, (str->length - index - range) * sizeof(*str->str));

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

B<Warning: Do not under any circumstances ever pass a non-literal string as
the fmt argument unless you know exactly how many conversions will take
place. Being careless with this is a very good way to build potential
security holes into your programs. The same is true for all functions that
take a printf()-like format string as an argument.>

    String *str = str_create(buf);       // EVIL
    String *str = str_create("%s", buf); // GOOD

=cut

*/

String *str_create(const char *fmt, ...)
{
	String *str;
	va_list args;
	va_start(args, fmt);
	str = str_vcreate_locked_sized(NULL, MIN_STRING_SIZE, fmt, args);
	va_end(args);
	return str;
}

/*

=item C<String *str_create_locked(Locker *locker, const char *fmt, ...)>

Equivalent to I<str_create()> except that multiple threads accessing the new
string will be synchronized by C<locker>.

=cut

*/

String *str_create_locked(Locker *locker, const char *fmt, ...)
{
	String *str;
	va_list args;
	va_start(args, fmt);
	str = str_vcreate_locked_sized(locker, MIN_STRING_SIZE, fmt, args);
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
	return str_vcreate_locked_sized(NULL, MIN_STRING_SIZE, fmt, args);
}

/*

=item C<String *str_vcreate_locked(Locker *locker, const char *fmt, va_list args)>

Equivalent to I<str_vcreate()> except that multiple threads accessing the new
string will be synchronized by C<locker>.

=cut

*/

String *str_vcreate_locked(Locker *locker, const char *fmt, va_list args)
{
	return str_vcreate_locked_sized(locker, MIN_STRING_SIZE, fmt, args);
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
	str = str_vcreate_locked_sized(NULL, size, fmt, args);
	va_end(args);
	return str;
}

/*

=item C<String *str_create_locked_sized(Locker *locker, size_t size, const char *fmt, ...)>

Equivalent to I<str_create_sized()> except that multiple threads accessing
the new string will be synchronised by C<locker>.

=cut

*/

String *str_create_locked_sized(Locker *locker, size_t size, const char *fmt, ...)
{
	String *str;
	va_list args;
	va_start(args, fmt);
	str = str_vcreate_locked_sized(locker, size, fmt, args);
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
	return str_vcreate_locked_sized(NULL, size, fmt, args);
}

/*

=item C<String *str_vcreate_locked_sized(Locker *locker, size_t size, const char *fmt, va_list args)>

Equivalent to I<str_vcreate_sized()> except that multiple threads accessing
the new string will be synchronised by C<locker>.

=cut

*/

String *str_vcreate_locked_sized(Locker *locker, size_t size, const char *fmt, va_list args)
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
		if (!mem_resize(&buf, size))
		{
			mem_release(buf);
			return NULL;
		}

		length = vsnprintf(buf, size, fmt, args);
		if (length != -1 && length < size)
			break;
	}

	if (!(str = mem_new(String)))
	{
		mem_release(buf);
		return NULL;
	}

	str->size = size;
	str->length = length + 1;
	str->str = buf;
	str->locker = locker;

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
	return str_copy_locked(NULL, str);
}

/*

=item C<String *str_copy_locked(Locker *locker, const String *str)>

Equivalent to I<str_copy()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_copy_locked(Locker *locker, const String *str)
{
	return str_substr_locked(locker, str, 0, -1);
}

/*

=item C<String *str_fgetline(FILE *stream)>

Similar to I<fgets(3)> except that it recognises UNIX ("\n"), DOS ("\r\n")
and Macintosh ("\r") line endings (even different line ending in the same
file) and it can read a line of any size into the I<String> that it returns.
Reading stops after an C<EOF> or the end of the line. A newline is placed in
the string. A C<nul> is placed after the last character in the buffer. On
success, returns a new I<String>. It is the caller's responsibility to
deallocate the new string with I<str_release()> or I<str_destroy()>. On
error, or when the end of file occurs while no characters have been read,
returns C<NULL>. Calls to this function can be mixed with calls to other
input functions from the I<stdio> library for the same input stream.

=cut

*/

String *str_fgetline(FILE *stream)
{
	return str_fgetline_locked(NULL, stream);
}

/*

=item C<String *str_fgetline_locked(Locker *locker, FILE *stream)>

Equivalent to I<str_fgetline()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

void flockfile(FILE *stream); /* Missing from old glibc headers */
void funlockfile(FILE *stream);

String *str_fgetline_locked(Locker *locker, FILE *stream)
{
	char buf[BUFSIZ];
	String *ret = str_create_locked(locker, "");

	if (!ret)
		return NULL;

	flockfile(stream);

	while (fgetline(buf, BUFSIZ, stream))
	{
		if (!str_append(ret, "%s", buf))
		{
			str_release(ret);
			break;
		}

		if (cstr(ret)[ret->length - 2] == '\n')
			break;
	}

	funlockfile(stream);

	return ret;
}

/*

=item C<void str_release(String *str)>

Releases (deallocates) C<str>.

=cut

*/

void str_release(String *str)
{
	Locker *locker;

	if (!str)
		return;

	if (str_wrlock(str) == -1)
		return;

	locker = str->locker;
	mem_release(str->str);
	mem_release(str);
	locker_unlock(locker);
}

/*

=item C<void *str_destroy(String **str)>

Destroys (deallocates and sets to C<NULL>) C<str>. Returns C<NULL>. B<Note:>
strings shared by multiple threads must not be destroyed until after the
threads have finished with it.

=cut

*/

void *str_destroy(String **str)
{
	if (str && *str)
	{
		str_release(*str);
		*str = NULL;
	}

	return NULL;
}

/*

=item C<int str_rdlock(const String *str)>

Claims a read lock on C<str> (if C<str> was created with a I<locker>). Clients
need to call this only before calling C<cstr()> (for the purpose of reading the
raw string data) on a string that was created with a I<locker>. It is the
client's responsibility to call I<str_unlock()> when finished with the raw
string data. The only functions that may be called on C<str> between calls to
I<str_rdlock()> and I<str_unlock()> are I<cstr()> and I<str_length
unlocked()>. On success, returns C<0>. On error, returns C<-1>.

=cut

*/

int str_rdlock(const String *str)
{
	if (!str)
		return set_errno(EINVAL);

	return locker_rdlock(str->locker);
}

/*

=item C<int str_wrlock(const String *str)>

Claims a write lock on C<str> (if C<str> was created with a I<locker>). Clients
need to call this only before calling C<cstr()> (for the purpose of modifying
the raw string data) on a string that was created with a I<locker>. It is the
client's responsibility to call I<str_unlock()> when finished with the raw
string data. The only functions that may be called on C<str> between calls to
I<str_wrlock()> and I<str_unlock()> are I<cstr()>, I<str_length_unlocked()>,
I<str_set_length_unlocked()> and I<str_recalc_length_unlocked()>). On success,
returns C<0>. On error, returns C<-1>.

=cut

*/

int str_wrlock(const String *str)
{
	if (!str)
		return set_errno(EINVAL);

	return locker_wrlock(str->locker);
}

/*

=item C<int str_unlock(const String *str)>

Unlocks C<str> if C<str> was created with a C<locker>.
On success, returns C<0>. On error, returns C<-1>.

=cut

*/

int str_unlock(const String *str)
{
	if (!str)
		return set_errno(EINVAL);

	return locker_unlock(str->locker);
}

/*

=item C<int str_empty(const String *str)>

Returns whether or not C<str> is the empty string.

=cut

*/

int str_empty(const String *str)
{
	int empty;

	if (!str)
		return 1;

	if (str_rdlock(str) == -1)
		return -1;

	empty = (str->length == 1);

	if (str_unlock(str) == -1)
		return -1;

	return empty;
}

/*

=item C<size_t str_length(const String *str)>

Returns the length of C<str>.

=cut

*/

static size_t str_length_locked(const String *str, int lock_str)
{
	size_t length;

	if (!str)
		return 0;

	if (lock_str && str_rdlock(str) == -1)
		return -1;

	length = str->length - 1;

	if (lock_str && str_unlock(str) == -1)
		return -1;

	return length;
}

size_t str_length(const String *str)
{
	return str_length_locked(str, 1);
}

/*

=item C<size_t str_length(const String *str)>

Returns the length of C<str> without claiming a read lock on C<str>. This is
only to be used between calls to I<str_rdlock()> or I<str_wrlock()> and
I<str_unlock()>.

=cut

*/

static size_t str_length_unlocked(const String *str)
{
	return str_length_locked(str, 0);
}

/*

=item C<char *cstr(const String *str)>

Returns the raw C string in C<str>. Do not use this pointer to extend the
length of the string. It's ok to use it to reduce the length of the string
provided you call I<str_set_length()> or I<str_recalc_length()> immediately
afterwards. When used on a string that is shared by multiple threads,
I<cstr()> must appear between calls to I<str_rdlock()> or I<str_wrlock()>
and I<str_unlock()>.

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

static ssize_t str_set_length_locked(String *str, size_t length, int lock_str)
{
	ssize_t len;

	if (!str)
		return -1;

	if (lock_str && str_wrlock(str) == -1)
		return -1;

	if (length >= str->length)
	{
		if (lock_str)
			str_unlock(str);
		return -1;
	}

	str->length = length + 1;
	str->str[str->length - 1] = '\0';
	len = str->length - 1;

	if (lock_str && str_unlock(str) == -1)
		return -1;

	return len;
}

ssize_t str_set_length(String *str, size_t length)
{
	return str_set_length_locked(str, length, 1);
}

/*

=item C<ssize_t str_set_length_unlocked(String *str, size_t length)>

Equivalent to I<str_set_length()> except that C<str> is not write locked.
This can only be used between calls to I<str_wrlock()> and I<str_unlock()>.
On success, returns C<0>. In error, returns C<-1>.

=cut

*/

ssize_t str_set_length_unlocked(String *str, size_t length)
{
	return str_set_length_locked(str, length, 0);
}

/*

=item C<ssize_t str_recalc_length(String *str)>

Calculates and stores the length of C<str>. Only needed after the raw C
string returned by I<cstr()> has been used to shorten a string. Note: Treats
C<str> as a C<nul>-terminated string and should be avoided. Use
I<str_set_length()> instead. On success, returns the length of C<str>. On
error, returns -1.

=cut

*/

static ssize_t str_recalc_length_locked(String *str, int lock_str)
{
	ssize_t len;

	if (!str)
		return -1;

	if (lock_str && str_wrlock(str) == -1)
		return -1;

	str->length = strlen(str->str) + 1;
	len = str->length - 1;

	if (lock_str && str_unlock(str) == -1)
		return -1;

	return len;
}

ssize_t str_recalc_length(String *str)
{
	return str_recalc_length_locked(str, 1);
}

/*

=item C<ssize_t str_recalc_length_unlocked(String *str)>

Equivalent to I<str_recalc_length()> except that C<str> is not write locked.
This can only be used between calls to I<str_wrlock()> and I<str_unlock()>.
On success, returns C<0>. In error, returns C<-1>.

=cut

*/

ssize_t str_recalc_length_unlocked(String *str)
{
	return str_recalc_length_locked(str, 0);
}

/*

=item C<String *str_clear(String *str)>

Makes C<str> the empty string. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

String *str_clear(String *str)
{
	return str_remove_range(str, 0, -1);
}

/*

=item C<String *str_remove(String *str, ssize_t index)>

Removes the C<index>'th char from C<str>. If C<index> is negative, it refers to
a character position relative to the end of the string (C<-1> is the position
after the last character, C<-2> is the position before the last character and
so on). On success, returns C<str>. On error, returns C<NULL>.

=cut

*/

String *str_remove(String *str, ssize_t index)
{
	return str_remove_range(str, index, 1);
}

/*

=item C<String *str_remove_range(String *str, ssize_t index, ssize_t range)>

Removes C<range> characters from C<str> starting at C<index>. If C<index> or
C<range> are negative, they refer to character positions relative to the end of
the string (C<-1> is the position after the last character, C<-2> is the
position before the last character and so on). On success, returns C<str>. On
error, returns C<NULL>.

=cut

*/

static String *str_remove_range_locked(String *str, ssize_t index, ssize_t range, int lock_str)
{
	if (!str)
		return NULL;

	if (lock_str && str_wrlock(str) == -1)
		return NULL;

	if (index < 0)
		index = str->length + index;

	if (range < 0)
		range = str->length + range - index;

	if (str->length - 1 < index + range)
	{
		if (lock_str)
			str_unlock(str);
		return NULL;
	}

	contract(str, index, range);

	if (lock_str && str_unlock(str) == -1)
		return NULL;

	return str;
}

String *str_remove_range(String *str, ssize_t index, ssize_t range)
{
	return str_remove_range_locked(str, index, range, 1);
}

static String *str_remove_range_unlocked(String *str, ssize_t index, ssize_t range)
{
	return str_remove_range_locked(str, index, range, 0);
}

/*

=item C<String *str_insert(String *str, ssize_t index, const char *fmt, ...)>

Adds the string specified by C<fmt> to C<str> at position C<index>. If C<index>
is negative, it refers to a character position relative to the end of the
string (C<-1> is the position after the last character, C<-2> is the position
before the last character and so on). On success, returns C<str>. On error,
returns C<NULL>.

=cut

*/

String *str_insert(String *str, ssize_t index, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vinsert(str, index, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vinsert(String *str, ssize_t index, const char *fmt, va_list args)>

Equivalent to I<str_insert()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

static String *str_insert_str_unlocked(String *str, ssize_t index, const String *src);

String *str_vinsert(String *str, ssize_t index, const char *fmt, va_list args)
{
	String *tmp, *ret;

	if (!str)
		return NULL;

	if (str_wrlock(str) == -1)
		return NULL;

	if (index < 0)
		index = str->length + index;

	if (str->length - 1 < index)
	{
		str_unlock(str);
		return NULL;
	}

	if (!(tmp = str_vcreate(fmt, args)))
	{
		str_unlock(str);
		return NULL;
	}

	ret = str_insert_str_unlocked(str, index, tmp);
	str_release(tmp);

	if (str_unlock(str) == -1)
		return NULL;

	return ret;
}

/*

=item C<String *str_insert_str(String *str, ssize_t index, const String *src)>

Inserts C<src> into C<str>, starting at position C<index>. If C<index> is
negative, it refers to a character position relative to the end of the string
(C<-1> is the position after the last character, C<-2> is the position before
the last character and so on). On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

static String *str_insert_str_locked(String *str, ssize_t index, const String *src, int lock_str)
{
	size_t length;

	if (!str || !src)
		return NULL;

	if (str_rdlock(src) == -1)
		return NULL;

	if (lock_str && str_wrlock(str) == -1)
	{
		str_unlock(src);
		return NULL;
	}

	if (index < 0)
		index = str->length + index;

	if (str->length - 1 < index)
	{
		if (lock_str)
			str_unlock(str);
		str_unlock(src);
		return NULL;
	}

	length = src->length - 1;

	if (expand(str, index, length) == -1)
	{
		if (lock_str)
			str_unlock(str);
		str_unlock(src);
		return NULL;
	}

	memcpy(str->str + index, src->str, length);

	if (lock_str)
		str_unlock(str);
	str_unlock(src);

	return str;
}

String *str_insert_str(String *str, ssize_t index, const String *src)
{
	return str_insert_str_locked(str, index, src, 1);
}

static String *str_insert_str_unlocked(String *str, ssize_t index, const String *src)
{
	return str_insert_str_locked(str, index, src, 0);
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
	ret = str_vinsert(str, -1, fmt, args);
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
	return str_vinsert(str, -1, fmt, args);
}

/*

=item C<String *str_append_str(String *str, const String *src)>

Appends C<src> to C<str>. On success, returns C<str>. On error, returns
C<NULL>.

=cut

*/

String *str_append_str(String *str, const String *src)
{
	return str_insert_str(str, -1, src);
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

=item C<String *str_replace(String *str, ssize_t index, ssize_t range, const char *fmt, ...)>

Replaces C<range> characters in C<str>, starting at C<index>, with the string
specified by C<fmt>. If C<index> or C<range> are negative, they refer to
character positions relative to the end of the string (C<-1> is the position
after the last character, C<-2> is the position before the last character and
so on). On success, returns C<str>. On error, returns C<NULL>.

=cut

*/

String *str_replace(String *str, ssize_t index, ssize_t range, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vreplace(str, index, range, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_vreplace(String *str, ssize_t index, ssize_t range, const char *fmt, va_list args)>

Equivalent to I<str_prepend()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

String *str_vreplace(String *str, ssize_t index, ssize_t range, const char *fmt, va_list args)
{
	String *tmp, *ret;

	if (!str)
		return NULL;

	if (!(tmp = str_vcreate(fmt, args)))
		return NULL;

	ret = str_replace_str(str, index, range, tmp);
	str_release(tmp);

	return ret;
}

/*

=item C<String *str_replace_str(String *str, ssize_t index, ssize_t range, const String *src)>

Replaces C<range> characters in C<str>, starting at C<index>, with C<src>. If
C<index> or C<range> are negative, they refer to character positions relative
to the end of the string (C<-1> is the position after the last character, C<-2>
is the position before the last character and so on). On success, return
C<str>. On error, returns C<NULL>.

=cut

*/

static String *str_replace_str_locked(String *str, ssize_t index, ssize_t range, const String *src, int lock_str)
{
	size_t length;

	if (!src || !str)
		return NULL;

	if (str_rdlock(src) == -1)
		return NULL;

	if (lock_str && str_wrlock(str) == -1)
	{
		str_unlock(src);
		return NULL;
	}

	if (index < 0)
		index = str->length + index;

	if (range < 0)
		range = str->length + range - index;

	if (str->length - 1 < index + range)
	{
		if (lock_str)
			str_unlock(str);
		str_unlock(src);
		return NULL;
	}

	length = src->length - 1;

	if (adjust(str, index, range, length) == -1)
	{
		if (lock_str)
			str_unlock(str);
		str_unlock(src);
		return NULL;
	}

	memcpy(str->str + index, src->str, length);

	if (lock_str)
		str_unlock(str);
	str_unlock(src);

	return str;
}

String *str_replace_str(String *str, ssize_t index, ssize_t range, const String *src)
{
	return str_replace_str_locked(str, index, range, src, 1);
}

static String *str_replace_str_unlocked(String *str, ssize_t index, ssize_t range, const String *src)
{
	return str_replace_str_locked(str, index, range, src, 0);
}

/*

=item C<String *str_substr(const String *str, ssize_t index, ssize_t range)>

Creates a new I<String> object consisting of C<range> characters from C<str>,
starting at C<index>. If C<index> or C<range> are negative, they refer to
character positions relative to the end of the string (C<-1> is the position
after the last character, C<-2> is the position before the last character and
so on). On success, returns the new string. It is the caller's responsibility
to deallocate the new string with I<str_release()> or I<str_destroy()>. On
error, returns C<NULL>.

=cut

*/

String *str_substr(const String *str, ssize_t index, ssize_t range)
{
	return str_substr_locked(NULL, str, index, range);
}

/*

=item C<String *str_substr_locked(Locker *locker, const String *str, ssize_t index, ssize_t range)>

Equivalent to I<str_substr()> except that multiple threads accessing the new
substring will be synchronised by C<locker>.

=cut

*/

static String *str_substr_locked_locked(Locker *locker, const String *str, ssize_t index, ssize_t range, int lock_str)
{
	String *ret;

	if (!str)
		return NULL;

	if (lock_str && str_rdlock(str) == -1)
		return NULL;

	if (index < 0)
		index = str->length + index;

	if (range < 0)
		range = str->length + range - index;

	if (str->length - 1 < index + range)
	{
		if (lock_str)
			str_unlock(str);
		return NULL;
	}

	if (!(ret = str_create_locked_sized(locker, range + 1, NULL)))
	{
		if (lock_str)
			str_unlock(str);
		return NULL;
	}

	memcpy(ret->str, str->str + index, range);
	ret->length = range + 1;
	ret->str[ret->length - 1] = '\0';

	if (lock_str && str_unlock(str) == -1)
		return NULL;

	return ret;
}

String *str_substr_locked(Locker *locker, const String *str, ssize_t index, ssize_t range)
{
	return str_substr_locked_locked(locker, str, index, range, 1);
}

static String *str_substr_locked_unlocked(Locker *locker, const String *str, ssize_t index, ssize_t range)
{
	return str_substr_locked_locked(locker, str, index, range, 0);
}

/*

=item C<String *substr(const char *str, ssize_t index, ssize_t range)>

Equivalent to I<str_substr()> but works on an ordinary C string. It is the
caller's responsibility to ensure that C<str> points to at least C<index +
range> bytes.

=cut

*/

String *substr(const char *str, ssize_t index, ssize_t range)
{
	return substr_locked(NULL, str, index, range);
}

/*

=item C<String *substr_locked(Locker *locker, const char *str, ssize_t index, ssize_t range)>

Equivalent to I<substr()> except that multiple threads accessing the new
substring will be synchronised by C<locker>. Note that no locking is
performed on C<str> as it is a raw C string.

=cut

*/

String *substr_locked(Locker *locker, const char *str, ssize_t index, ssize_t range)
{
	String *ret;
	size_t len = 0;

	if (!str)
		return NULL;

	if (index < 0 || range < 0)
		len = strlen(str) + 1;

	if (index < 0)
		index = len + index;

	if (range < 0)
		range = len + range - index;

	if (!(ret = str_create_locked_sized(locker, range + 1, NULL)))
		return NULL;

	memcpy(ret->str, str + index, range);
	ret->length = range + 1;
	ret->str[ret->length - 1] = '\0';

	return ret;
}

/*

=item C<String *str_splice(String *str, ssize_t index, ssize_t range)>

Removes a substring from C<str> starting at C<index> of length C<range>
characters. If C<index> or C<range> are negative, they refer to character
positions relative to the end of the string (C<-1> is the position after the
last character, C<-2> is the position before the last character and so on). On
success, returns the substring. It is the caller's responsibility to deallocate
the new substring with I<str_release()> or I<str_destroy()>. On error, returns
C<NULL>.

=cut

*/

String *str_splice(String *str, ssize_t index, ssize_t range)
{
	return str_splice_locked(NULL, str, index, range);
}

/*

=item C<String *str_splice_locked(Locker *locker, String *str, ssize_t index, ssize_t range)>

Equivalent to I<str_splice()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_splice_locked(Locker *locker, String *str, ssize_t index, ssize_t range)
{
	String *ret;

	if (!str)
		return NULL;

	if (str_wrlock(str) == -1)
		return NULL;

	if (!(ret = str_substr_locked_unlocked(locker, str, index, range)))
	{
		str_unlock(str);
		return NULL;
	}

	if (!str_remove_range_unlocked(str, index, range))
	{
		str_unlock(str);
		str_release(ret);
		return NULL;
	}

	if (str_unlock(str) == -1)
		return NULL;

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
	ret = str_vrepeat_locked(NULL, count, fmt, args);
	va_end(args);
	return ret;
}

/*

=item C<String *str_repeat_locked(Locker *locker, size_t count, const char *fmt, ...)>

Equivalent to I<str_repeat()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_repeat_locked(Locker *locker, size_t count, const char *fmt, ...)
{
	String *ret;
	va_list args;
	va_start(args, fmt);
	ret = str_vrepeat_locked(locker, count, fmt, args);
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
	return str_vrepeat_locked(NULL, count, fmt, args);
}

/*

=item C<String *str_vrepeat_locked(Locker *locker, size_t count, const char *fmt, va_list args)>

Equivalent to I<str_vrepeat()> except that multiple threads accessing the
new string will be synchronised by C<locker>.

=cut

*/

String *str_vrepeat_locked(Locker *locker, size_t count, const char *fmt, va_list args)
{
	String *tmp, *ret;
	size_t i;

	if (!(tmp = str_vcreate(fmt, args)))
		return NULL;

	if (!(ret = str_create_locked_sized(locker, str_length(tmp) * count + 1, NULL)))
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

static StrTR *tr_compile_table(StrTR *table, const char *from, const char *to, int option);

int str_tr(String *str, const char *from, const char *to, int option)
{
	StrTR table[1];

	if (!str || !from)
		return -1;

	table->locker = NULL;

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

static StrTR *str_tr_compile_table(StrTR *table, const String *from, const String *to, int option);

int str_tr_str(String *str, const String *from, const String *to, int option)
{
	StrTR table[1];

	if (!str || !from)
		return -1;

	table->locker = NULL;

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

	table->locker = NULL;

	if (!tr_compile_table(table, from, to, option))
		return -1;

	return tr_compiled(str, table);
}

/*

=item C<StrTR *tr_compile(const char *from, const char *to, int option)>

Compiles C<from>, C<to> and C<option> into a translation table to be passed
to I<str_tr_compiled()> or I<tr_compiled()>. On success, returns the new
translation table. It is the caller's responsibility to deallocate the
translation table with I<tr_release()> or I<tr_destroy()>. On error, returns
C<NULL>.

=cut

*/

StrTR *tr_compile(const char *from, const char *to, int option)
{
	return tr_compile_locked(NULL, from, to, option);
}

/*

=item C<StrTR *tr_compile_locked(Locker *locker, const char *from, const char *to, int option)>

Equivalent to I<tr_compile()> except that multiple threads accessing the new
translation table will be synchronised by C<locker>.

=cut

*/

StrTR *tr_compile_locked(Locker *locker, const char *from, const char *to, int option)
{
	StrTR *ret;

	if (!(ret = mem_new(StrTR)))
		return NULL;

	ret->locker = locker;

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
	return str_tr_compile_locked(NULL, from, to, option);
}

/*

=item C<StrTR *str_tr_compile_locked(Locker *locker, const String *from, const String *to, int option)>

Equivalent to I<str_tr_compile()> except that multiple threads accessing the
new translation table will be synchronised by C<locker>.

=cut

*/

StrTR *str_tr_compile_locked(Locker *locker, const String *from, const String *to, int option)
{
	StrTR *ret;

	if (!(ret = mem_new(StrTR)))
		return NULL;

	ret->locker = locker;

	return str_tr_compile_table(ret, from, to, option);
}

/*

=item C<void tr_release(StrTR *tr)>

Releases (deallocates) C<tr>.

=cut

*/

void tr_release(StrTR *tr)
{
	Locker *locker;

	if (!tr)
		return;

	locker = tr->locker;
	if (locker_wrlock(locker) == -1)
		return;

	mem_release(tr);
	locker_unlock(locker);
}

/*

=item C<void *tr_destroy(StrTR **tr)>

Destroys (deallocates and sets to C<NULL>) C<tr>. Returns C<NULL>. B<Note:>
translation tables shared by multiple threads must not be destroyed until
after the threads have finished with it.

=cut

*/

void *tr_destroy(StrTR **tr)
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

	if (locker_wrlock(table->locker) == -1)
		return NULL;

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

	if (locker_unlock(table->locker) == -1)
		return NULL;

	return table;
}

/*

C<StrTR *tr_compile_table(StrTR *table, const char *from, const char *to, int option)>

Equivalent to I<tr_compile()> except that C<from>, C<to> and C<option> are
compiled into the translation table pointed to by C<table>. On success,
returns C<table>. On error, returns C<NULL>.

*/

static StrTR *tr_compile_table(StrTR *table, const char *from, const char *to, int option)
{
	return do_tr_compile_table(table, from, -1, to, -1, option);
}

/*

C<StrTR *str_tr_compile_table(StrTR *table, const String *from, const String *to, int option)>

Equivalent to I<tr_compile_table()> except that C<from> and C<to> are
I<String> objects. This is needed when C<from> or C<to> need to contain
C<nul> characters.

*/

static StrTR *str_tr_compile_table(StrTR *table, const String *from, const String *to, int option)
{
	StrTR *ret;

	if (str_rdlock(from) == -1)
		return NULL;

	if (str_rdlock(to) == -1)
	{
		str_unlock(from);
		return NULL;
	}

	ret = do_tr_compile_table(table, from->str, str_length_unlocked(from), to->str, str_length_unlocked(to), option);

	str_unlock(from);
	str_unlock(to);

	return ret;
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

	if (locker_rdlock(table->locker) == -1)
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

	if (locker_unlock(table->locker) == -1)
		return -1;

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
	int ret;

	if (!str || !table)
		return -1;

	if (locker_rdlock(table->locker) == -1)
		return -1;

	if (str_wrlock(str) == -1)
	{
		locker_unlock(table->locker);
		return -1;
	}

	ret = do_tr_compiled((unsigned char *)str->str, &str->length, table);
	str_unlock(str);
	locker_unlock(table->locker);

	return ret;
}

/*

=item C<int tr_compiled(char *str, StrTR *table)>

Equivalent to I<str_tr_compiled()> but works on an ordinary C string.

=cut

*/

int tr_compiled(char *str, StrTR *table)
{
	int ret;

	if (!str || !table)
		return -1;

	if (locker_rdlock(table->locker) == -1)
		return -1;

	ret = do_tr_compiled((unsigned char *)str, NULL, table);
	locker_unlock(table->locker);

	return ret;
}

#ifndef REGEX_MISSING

/*

=item C<List *str_regexpr(const char *pattern, const String *text, int cflags, int eflags)>

I<str_regexpr()> is an interface to POSIX 1003.2 compliant regular
expression matching. C<pattern> is a regular expression. C<text> is the
string to be searched for matches. C<cflags> is passed to I<regcomp(3)>
along with C<REG_EXTENDED>. C<eflags> is passed to I<regexec(3)>. On
success, returns a I<List> of (at most 33) I<String>s containing the
matching substring followed by the matching substrings of any parenthesised
subexpressions. It is the caller's responsibility to deallocate the list
with C<list_release()> or C<list_destroy()>. On error (including no match),
returns C<NULL>. Only use this function when the regular expression will be
used only once. Otherwise, use I<regexpr_compile()> or I<regcomp(3)> and
I<str_regexpr_compiled()> or I<regexpr_compiled()> or I<regexec(3)>.

Note: If you require perl pattern matching, you could use Philip Hazel's
I<PCRE> package, C<ftp://ftp.cus.cam.ac.uk/pub/software/programs/pcre/> or
link against the perl library itself.

=cut

*/

List *str_regexpr(const char *pattern, const String *text, int cflags, int eflags)
{
	return str_regexpr_locked(NULL, pattern, text, cflags, eflags);
}

/*

=item C<List *str_regexpr_locked(Locker *locker, const char *pattern, const String *text, int cflags, int eflags)>

Equivalent to I<str_regexpr()> except that multiple threads accessing the new
list will be synchronised by C<locker>.

=cut

*/

List *str_regexpr_locked(Locker *locker, const char *pattern, const String *text, int cflags, int eflags)
{
	List *ret;

	if (!pattern || !text)
		return NULL;

	if (str_rdlock(text) == -1)
		return NULL;

	ret = regexpr_locked(locker, pattern, text->str, cflags, eflags);

	if (str_unlock(text) == -1)
	{
		list_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<List *regexpr(const char *pattern, const char *text, int cflags, int eflags)>

Equivalent to I<str_regexpr()> but works on an ordinary C string.

=cut

*/

List *regexpr(const char *pattern, const char *text, int cflags, int eflags)
{
	return regexpr_locked(NULL, pattern, text, cflags, eflags);
}

/*

=item C<List *regexpr_locked(Locker *locker, const char *pattern, const char *text, int cflags, int eflags)>

Equivalent to I<regexpr()> except that multiple threads accessing the new list
will be synchronised by C<locker>.

=cut

*/

List *regexpr_locked(Locker *locker, const char *pattern, const char *text, int cflags, int eflags)
{
	regex_t compiled[1];
	List *ret;

	if (!pattern || !text)
		return NULL;

	if (regexpr_compile(compiled, pattern, cflags))
		return NULL;

	ret = regexpr_compiled_locked(locker, compiled, text, eflags);
	regfree(compiled);

	return ret;
}

/*

=item C<int regexpr_compile(regex_t *compiled, const char *pattern, int cflags)>

Compiles a POSIX 1003.2 compliant regular expression. C<compiled> is the
location in which to compile the expression. C<pattern> is the regular
expression. C<cflags> is passed to I<regcomp(3)> along with C<REG_EXTENDED>.
Call this, followed by I<re_compiled()> when the regular expression will be
used multiple times.

=cut

*/

int regexpr_compile(regex_t *compiled, const char *pattern, int cflags)
{
	if (!compiled || !pattern)
		return REG_BADPAT;

	return regcomp(compiled, pattern, cflags | REG_EXTENDED);
}

/*

=item C<void regexpr_release(regex_t *compiled)>

Just another name for I<regfree(3)>.

=cut

*/

void regexpr_release(regex_t *compiled)
{
	if (compiled)
		regfree(compiled);
}

/*

=item C<List *str_regexpr_compiled(const regex_t *compiled, String char *text, int eflags)>

I<regexpr_compiled()> is an interface to the POSIX 1003.2 regular expression
function, I<regexec(3)>. C<compiled> is the compiled regular expression
prepared by I<regexpr_compile()> or I<regcomp(3)>. C<text> is the string to be
searched for a match. C<eflags> is passed to I<regexec(3)>. On success, returns
a I<List> of (at most 33) I<String>s containing the matching substring followed
by the matching substrings of any parenthesised subexpressions. It is the
caller's responsibility to deallocate the list with C<list_release()> or
C<list_destroy()>. On error (including no match), returns C<NULL>.

=cut

*/

List *str_regexpr_compiled(const regex_t *compiled, const String *text, int eflags)
{
	return str_regexpr_compiled_locked(NULL, compiled, text, eflags);
}

/*

=item C<List *str_regexpr_compiled_locked(Locker *locker, const regex_t *compiled, const String *text, int eflags)>

Equivalent to I<str_regexpr_compiled()> except that multiple threads accessing
the new list will be synchronised by C<locker>.

=cut

*/

List *str_regexpr_compiled_locked(Locker *locker, const regex_t *compiled, const String *text, int eflags)
{
	List *ret;

	if (!compiled || !text)
		return NULL;

	if (str_rdlock(text) == -1)
		return NULL;

	ret = regexpr_compiled_locked(locker, compiled, text->str, eflags);

	if (str_unlock(text) == -1)
	{
		list_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<List *regexpr_compiled(const regex_t *compiled, const char *text, int eflags)>

Equivalent to I<str_regexpr_compiled()> but works on an ordinary C string.

=cut

*/

List *regexpr_compiled(const regex_t *compiled, const char *text, int eflags)
{
	return regexpr_compiled_locked(NULL, compiled, text, eflags);
}

/*

=item C<List *regexpr_compiled_locked(Locker *locker, const regex_t *compiled, const char *text, int eflags)>

Equivalent to I<regexpr_compiled()> except that multiple threads accessing the
new list will be synchronised by C<locker>.

=cut

*/

List *regexpr_compiled_locked(Locker *locker, const regex_t *compiled, const char *text, int eflags)
{
	regmatch_t match[33];
	List *ret;
	int i;

	if (!compiled || !text)
		return NULL;

	if (regexec(compiled, text, 33, match, eflags))
		return NULL;

	if (!(ret = list_create_locked(locker, (list_release_t *)str_release)))
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
along with C<REG_EXTENDED>. C<eflags> is passed to I<regexec(3)>. C<all>
specifies whether to substitute the first match (if zero) or all matches (if
non-zero). C<replacement> specifies the string that replaces each match. If
C<replacement> contains C<"$#"> or C<"${##}"> (where C<"#"> is a decimal
digit), the substring that matches the corresponding subexpression is
interpolated in its place. Up to 32 subexpressions are supported. If
C<replacement> contains C<"$$">, then C<"$"> is interpolated in its place.
C<eplacement> also understands the following I<perl(1)> quote escape
sequences:

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
Otherwise, use I<regexpr_compile()> or I<regcomp(3)> and
I<str_regsub_compiled()>.

=cut

*/

String *str_regsub(const char *pattern, const char *replacement, String *text, int cflags, int eflags, int all)
{
	regex_t compiled[1];
	String *ret;

	if (!pattern || !replacement || !text)
		return NULL;

	if (regexpr_compile(compiled, pattern, cflags))
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
#define MAX_MATCHES 33
#define MAX_STATES 33
	enum
	{
		RS_LC = 1,
		RS_UC = 2,
		RS_QM = 4,
		RS_FIRST = 8,
		RS_LCFIRST = RS_LC | RS_FIRST,
		RS_UCFIRST = RS_UC | RS_FIRST
	};

	regmatch_t match[MAX_MATCHES];
	String *rep;
	int matches;
	size_t start;
	int states[MAX_STATES];
	int i, s;

	if (!compiled || !replacement || !text)
		return NULL;

	if (str_wrlock(text) == -1)
		return NULL;

	for (start = 0, matches = 0; start <= text->length - 1; ++matches)
	{
		if (regexec(compiled, text->str + start, MAX_MATCHES, match, eflags))
		{
			if (str_unlock(text) == -1)
				return NULL;

			return (matches) ? text : NULL;
		}

		/*
		** Interpolate any $$, $# and ${##} in replacement
		** with subexpression matching substrings
		*/

		if (!(rep = str_create("%s", replacement)))
		{
			str_unlock(text);
			return NULL;
		}

		for (i = 0; i < rep->length - 1; ++i)
		{
			if (rep->str[i] == '$')
			{
				if (rep->str[i + 1] == '$')
				{
					if (!str_remove(rep, i))
					{
						str_release(rep);
						str_unlock(text);
						return NULL;
					}
				}
				else
				{
					int ref;
					int j = i + 1;

					if (rep->str[j] == '{')
					{
						for (++j, ref = 0; is_digit(rep->str[j]); ++j)
							ref *= 10, ref += rep->str[j] - '0';

						if (rep->str[j] != '}')
						{
							str_release(rep);
							str_unlock(text);
							return NULL;
						}
					}
					else if (is_digit(rep->str[i + 1]))
					{
						ref = rep->str[j] - '0';
					}
					else
					{
						str_release(rep);
						str_unlock(text);
						return NULL;
					}

					if (ref < 0 || ref >= MAX_MATCHES || match[ref].rm_so == -1)
					{
						str_release(rep);
						str_unlock(text);
						return NULL;
					}

					if (!str_replace(rep, i, j + 1 - i, "%.*s", match[ref].rm_eo - match[ref].rm_so, text->str + match[ref].rm_so))
					{
						str_release(rep);
						str_unlock(text);
						return NULL;
					}

					i += match[ref].rm_eo - match[ref].rm_so - 1;
				}
			}
		}

		/* Perform \l \L \u \U \Q \E transformations on replacement */

#define FAIL { str_release(rep); str_unlock(text); return NULL; }
#define PUSH_STATE(state) { if (s >= MAX_STATES - 1) FAIL states[s + 1] = states[s] | (state); ++s; }
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
					rep->str[i] = to_lower(rep->str[i]);

				if (states[s] & RS_UC)
					rep->str[i] = to_upper(rep->str[i]);

				if (states[s] & RS_QM && !is_alnum(rep->str[i]))
					if (!str_insert(rep, i++, "\\"))
						FAIL

				if (states[s] & RS_FIRST)
					POP_STATE
			}
		}

		/* Replace matching substring in text with rep */

		if (!str_replace_str_unlocked(text, start + match[0].rm_so, match[0].rm_eo - match[0].rm_so, rep))
		{
			str_release(rep);
			str_unlock(text);
			return NULL;
		}

		/* Zero length match (at every position), move on or get stuck */

		if (match[0].rm_so == 0 && match[0].rm_eo == 0)
		{
			++match[0].rm_so;
			++match[0].rm_eo;
		}

		start += match[0].rm_so + rep->length - 1;
		str_release(rep);

		if (!all)
			break;
	}

	if (str_unlock(text) == -1)
		return null;

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

B<Note:> I<str_fmt()> provides straightforward formatting completely lacking
in any aesthetic merit. If you need awesome paragraph formatting, pipe text
through I<par(1)> instead (available from
L<http://www.cs.berkeley.edu/~amc/Par/|http://www.cs.berkeley.edu/~amc/Par/>).

=cut

*/

List *str_fmt(const String *str, size_t line_width, StrAlignment alignment)
{
	return str_fmt_locked(NULL, str, line_width, alignment);
}

/*

=item C<List *str_fmt_locked(Locker *locker, const String *str, size_t line_width, StrAlignment alignment)>

Equivalent to I<str_fmt()> except that multiple threads accessing the new
list will be synchronised by C<locker>.

=cut

*/

List *str_fmt_locked(Locker *locker, const String *str, size_t line_width, StrAlignment alignment)
{
	List *ret;

	if (!str)
		return NULL;

	if (str_rdlock(str) == -1)
		return NULL;

	ret = fmt_locked(locker, str->str, line_width, alignment);

	if (str_unlock(str) == -1)
	{
		list_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<List *fmt(const char *str, size_t line_width, StrAlignment alignment)>

Equivalent to I<str_fmt()> but works on an ordinary C string.

=cut

*/

List *fmt(const char *str, size_t line_width, StrAlignment alignment)
{
	return fmt_locked(NULL, str, line_width, alignment);
}

/*

=item C<List *fmt(const char *str, size_t line_width, StrAlignment alignment)>

Equivalent to I<fmt()> except that multiple threads accessing the new list
will be synchronised by C<locker>.

=cut

*/

List *fmt_locked(Locker *locker, const char *str, size_t line_width, StrAlignment alignment)
{
	List *para;
	String *line = NULL;
	const char *s, *r;
	size_t len;

	if (!str)
		return NULL;

	switch (alignment)
	{
		case ALIGN_LEFT:
		case ALIGN_RIGHT:
		case ALIGN_FULL:
		{
			if (!(para = list_create_locked(locker, (list_release_t *)str_release)))
				return NULL;

			for (s = str; *s; ++s)
			{
				while (is_space(*s))
					++s;

				for (r = s; *r && !is_space(*r); ++r)
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
						if (!(line = str_create_sized(line_width, "%.*s", r - s, s)))
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
			if (!(para = split_locked(locker, str, "\n")))
				return NULL;

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

C<List *do_split_locked(Locker *locker, const char *str, ssize_t length, const char *delim)>

Splits C<str> into tokens separated by sequences of characters occurring in
C<delim>. If C<length> is C<-1>, C<str> is interpreted as a
C<nul>-terminated C string. Otherwise, C<str> is interpreted as an arbitrary
string of length C<length>. On success, returns a new I<List> of I<String>
objects. It is the caller's responsibility to deallocate the list with
C<list_release()> or C<list_destroy()>. If C<locker> is non-C<NULL>,
multiple threads accessing the new list will be synchronised by C<locker>.
On error, returns C<NULL>.

*/

static List *do_split_locked(Locker *locker, const char *str, ssize_t length, const char *delim)
{
	List *ret;
	const char *s, *r;

	if (!str || !delim)
		return NULL;

	ret = list_create_locked(locker, (list_release_t *)str_release);
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
	return str_split_locked(NULL, str, delim);
}

/*

=item C<List *str_split_locked(Locker *locker, const String *str, const char *delim)>

Equivalent to I<str_split()> except that multiple threads accessing the new
list will be synchronised by C<locker>.

=cut

*/

List *str_split_locked(Locker *locker, const String *str, const char *delim)
{
	List *ret;

	if (!str || !delim)
		return NULL;

	if (str_rdlock(str) == -1)
		return NULL;

	ret = do_split_locked(locker, str->str, str->length - 1, delim);

	if (str_unlock(str) == -1)
	{
		list_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<List *split(const char *str, const char *delim)>

Equivalent to I<str_split()> but works on an ordinary C string.

=cut

*/

List *split(const char *str, const char *delim)
{
	return split_locked(NULL, str, delim);
}

/*

=item C<List *split_locked(Locker *locker, const char *str, const char *delim)>

Equivalent to I<split()> except that multiple threads accessing the new list
will be synchronised by C<locker>.

=cut

*/

List *split_locked(Locker *locker, const char *str, const char *delim)
{
	if (!str || !delim)
		return NULL;

	return do_split_locked(locker, str, -1, delim);
}

#ifndef REGEX_MISSING

/*

=item C<List *str_regexpr_split(const String *str, const char *delim, int cflags, int eflags)>

Splits C<str> into tokens separated by occurrences of the regular expression,
C<delim>. C<str> is interpreted as a C<nul>-terminated C string. C<cflags> is
passed to I<regcomp(3)> and C<eflags> is passed to C<regexec(3)>. On success,
returns a new I<List> of I<String> objects. It is the caller's responsibility
to deallocate the list with C<list_release()> or C<list_destroy()>. On error,
returns C<NULL>.

=cut

*/

List *str_regexpr_split(const String *str, const char *delim, int cflags, int eflags)
{
	return str_regexpr_split_locked(NULL, str, delim, cflags, eflags);
}

/*

=item C<List *str_regexpr_split_locked(Locker *locker, const String *str, const char *delim, int cflags, int eflags)>

Equivalent to I<str_regexpr_split()> except that multiple threads accessing
the new list will be synchronised by C<locker>.

=cut

*/

List *str_regexpr_split_locked(Locker *locker, const String *str, const char *delim, int cflags, int eflags)
{
	List *ret;

	if (!str || !delim)
		return NULL;

	if (str_rdlock(str) == -1)
		return NULL;

	ret = regexpr_split_locked(locker, str->str, delim, cflags, eflags);

	if (str_unlock(str) == -1)
	{
		list_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<List *regexpr_split(const char *str, const char *delim, int cflags, int eflags)>

Equivalent to I<str_regexpr_split()> but works on an ordinary C string.

=cut

*/

List *regexpr_split(const char *str, const char *delim, int cflags, int eflags)
{
	return regexpr_split_locked(NULL, str, delim, cflags, eflags);
}

/*

=item C<List *regexpr_split_locked(Locker *locker, const char *str, const char *delim, int cflags, int eflags)>

Equivalent to I<regexpr_split()> except that multiple threads accessing the
new list will be synchronised by C<locker>.

=cut

*/

List *regexpr_split_locked(Locker *locker, const char *str, const char *delim, int cflags, int eflags)
{
	List *ret;
	String *token;
	regex_t compiled[1];
	regmatch_t match[1];
	int start, matches;

	if (!str || !delim)
		return NULL;

	if (regexpr_compile(compiled, delim, cflags))
		return NULL;

	if (!(ret = list_create_locked(locker, (list_release_t *)str_release)))
		return NULL;

	for (start = 0, matches = 0; str[start]; ++matches)
	{
		if (regexec(compiled, str + start, 1, match, eflags))
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
			if (!(token = substr(str, start, match[0].rm_so)))
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
		if (!(token = str_create("%s", str + start)))
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
	return str_join_locked(NULL, list, delim);
}

/*

=item C<String *str_join_locked(Locker *locker, const List *list, const char *delim)>

Equivalent to I<str_join()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_join_locked(Locker *locker, const List *list, const char *delim)
{
	String *ret;
	String *del;
	Lister *lister;
	int i;

	if (!list)
		return NULL;

	if (!(ret = str_create_locked(locker, NULL)))
		return NULL;

	if (!(del = str_create(delim ? "%s" : NULL, delim)))
	{
		str_release(ret);
		return NULL;
	}

	if (!(lister = lister_create((List *)list)))
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
	return join_locked(NULL, list, delim);
}

/*

=item C<String *join_locked(Locker *locker, const List *list, const char *delim)>

Equivalent to I<join()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *join_locked(Locker *locker, const List *list, const char *delim)
{
	String *ret;
	String *del;
	Lister *lister;
	int i;

	if (!list)
		return NULL;

	if (!(ret = str_create_locked(locker, NULL)))
		return NULL;

	if (!(del = str_create(delim ? "%s" : NULL, delim)))
	{
		str_release(ret);
		return NULL;
	}

	if (!(lister = lister_create((List *)list)))
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

	if (!str)
		return NULL;

	if (str_wrlock(str) == -1)
		return NULL;

	if (!str->str)
	{
		str_unlock(str);
		return NULL;
	}

	for (s = str->str; is_space(*s); ++s)
	{}

	if (s > str->str)
	{
		if (!str_remove_range_unlocked(str, 0, s - str->str))
		{
			str_unlock(str);
			return NULL;
		}
	}

	for (s = str->str + str->length - 1; s > str->str && is_space(s[-1]); --s)
	{}

	if (is_space(*s))
	{
		if (!str_remove_range_unlocked(str, s - str->str, str->length - 1 - (s - str->str)))
		{
			str_unlock(str);
			return NULL;
		}
	}

	if (str_unlock(str) == -1)
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

	for (s = str; is_space(*s); ++s)
	{}

	len = strlen(s);

	if (s > str)
		memmove(str, s, len + 1);

	for (s = str + len; s > str && is_space(*--s); )
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

	if (!str)
		return NULL;

	if (str_wrlock(str) == -1)
		return NULL;

	if (!str->str)
	{
		str_unlock(str);
		return NULL;
	}

	for (s = str->str; is_space(*s); ++s)
	{}

	if (s > str->str)
	{
		if (!str_remove_range_unlocked(str, 0, s - str->str))
		{
			str_unlock(str);
			return NULL;
		}
	}

	if (str_unlock(str) == -1)
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

	for (s = str; is_space(*s); ++s)
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

	if (!str)
		return NULL;

	if (str_wrlock(str) == -1)
		return NULL;

	if (!str->str)
	{
		str_unlock(str);
		return NULL;
	}

	for (s = str->str + str->length - 1; s > str->str && is_space(s[-1]); --s)
	{}

	if (is_space(*s))
	{
		if (!str_remove_range_unlocked(str, s - str->str, str->length - 1 - (s - str->str)))
		{
			str_unlock(str);
			return NULL;
		}
	}

	if (str_unlock(str) == -1)
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

	for (s = str + len; s > str && is_space(*--s); )
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

	if (str_wrlock(str) == -1)
		return NULL;

	for (r = s = str->str; s - str->str < str->length - 1; ++s)
	{
		if (!is_space(*s))
		{
			if (was_space && started)
				*r++ = ' ';
			*r++ = *s;
			started = 1;
		}

		was_space = is_space(*s);
	}

	if (r - str->str < str->length)
	{
		if (!str_remove_range_unlocked(str, r - str->str, str->length - 1 - (r - str->str)))
		{
			str_unlock(str);
			return NULL;
		}
	}

	if (str_unlock(str) == -1)
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
		if (!is_space(*s))
		{
			if (was_space && started)
				*r++ = ' ';
			*r++ = *s;
			started = 1;
		}

		was_space = is_space(*s);
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
	return str_quote_locked(NULL, str, quotable, quote_char);
}

/*

=item C<String *str_quote_locked(Locker *locker, const String *str, const char *quotable, char quote_char)>

Equivalent to I<str_quote()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_quote_locked(Locker *locker, const String *str, const char *quotable, char quote_char)
{
	String *ret;
	size_t i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_copy_locked(locker, str)))
		return NULL;

	for (i = 0; i < ret->length - 1; ++i)
		if (ret->str[i] && strchr(quotable, ret->str[i]))
			if (!str_insert(ret, i++, "%c", quote_char))
			{
				str_release(ret);
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
	return quote_locked(NULL, str, quotable, quote_char);
}

/*

=item C<String *quote_locked(Locker *locker, const char *str, const char *quotable, char quote_char)>

Equivalent to I<quote()> except that multiple threads accessing the new string
will be synchronised by C<locker>.

=cut

*/

String *quote_locked(Locker *locker, const char *str, const char *quotable, char quote_char)
{
	String *ret;
	size_t i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_create_locked(locker, "%s", str)))
		return NULL;

	for (i = 0; i < ret->length - 1; ++i)
		if (strchr(quotable, ret->str[i]))
			if (!str_insert(ret, i++, "%c", quote_char))
			{
				str_release(ret);
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
	return str_unquote_locked(NULL, str, quotable, quote_char);
}

/*

=item C<String *str_unquote_locked(Locker *locker, const String *str, const char *quotable, char quote_char)>

Equivalent to I<str_unquote()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_unquote_locked(Locker *locker, const String *str, const char *quotable, char quote_char)
{
	String *ret;
	int i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_copy_locked(locker, str)))
		return NULL;

	for (i = 0; i < (int)ret->length - 2; ++i)
		if (ret->str[i] == quote_char && ret->str[i + 1] && strchr(quotable, ret->str[i + 1]))
			if (!str_remove(ret, i))
			{
				str_release(ret);
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
	return unquote_locked(NULL, str, quotable, quote_char);
}

/*

=item C<String *unquote_locked(Locker *locker, const char *str, const char *quotable, char quote_char)>

Equivalent to I<unquote()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *unquote_locked(Locker *locker, const char *str, const char *quotable, char quote_char)
{
	String *ret;
	int i;

	if (!str || !quotable)
		return NULL;

	if (!(ret = str_create_locked(locker, "%s", str)))
		return NULL;

	for (i = 0; i < (int)ret->length - 2; ++i)
		if (ret->str[i] == quote_char && strchr(quotable, ret->str[i + 1]))
			if (!str_remove(ret, i))
			{
				str_release(ret);
				return NULL;
			}

	return ret;
}

/*

C<static String *do_encode_locked(Locker *locker, const char *str, ssize_t length, const char *uncoded, const char *coded, char quote_char, int printable)>

Performs encoding as described in I<str_encode()>.

*/

static String *do_encode_locked(Locker *locker, const char *str, size_t length, const char *uncoded, const char *coded, char quote_char, int printable)
{
	static const char hex[] = "0123456789abcdef";
	String *encoded;
	const char *target;
	const char *s;

	if (!str || !uncoded || !coded)
		return NULL;

	if (!(encoded = str_create_locked_sized(locker, length * 4 + 1, "")))
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
		else if (printable && !is_print(*s))
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

C<static String *do_decode_locked(Locker *locker, const char *str, ssize_t length, const char *uncoded, const char *coded, char quote_char, int printable)>

Performs decoding as described in I<str_decode()>.

*/

static String *do_decode_locked(Locker *locker, const char *str, size_t length, const char *uncoded, const char *coded, char quote_char, int printable)
{
	String *decoded;
	const char *start;
	const char *slosh;
	char *target;

	if (!str || !uncoded || !coded)
		return NULL;

	if (!(decoded = str_create_locked_sized(locker, length + 1, "")))
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

			if (is_digit(*s) && *s <= '7')
			{
				--s;

				do
				{
					++digits;
					c <<= 3, c |= *++s - '0';
				}
				while (digits < 3 && is_digit(s[1]) && s[1] <= '7');
			}
			else if (*s == 'x' && is_xdigit(s[1]))
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
				while (digits < 2 && is_xdigit(s[1]));
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
	return str_encode_locked(NULL, str, uncoded, coded, quote_char, printable);
}

/*

=item C<String *str_encode_locked(Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<str_encode()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_encode_locked(Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	String *ret;

	if (!str || !uncoded || !coded)
		return NULL;

	if (str_rdlock(str) == -1)
		return NULL;

	ret = do_encode_locked(locker, str->str, str->length - 1, uncoded, coded, quote_char, printable);

	if (str_unlock(str) == -1)
	{
		str_release(ret);
		return NULL;
	}

	return ret;
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
	return str_decode_locked(NULL, str, uncoded, coded, quote_char, printable);
}

/*

=item C<String *str_decode_locked(Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<str_decode()> except that multiple threads accessing the new
string will be synchronised by C<locker>.

=cut

*/

String *str_decode_locked(Locker *locker, const String *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	String *ret;

	if (!str || !uncoded || !coded)
		return NULL;

	if (str_rdlock(str) == -1)
		return NULL;

	ret = do_decode_locked(locker, str->str, str->length - 1, uncoded, coded, quote_char, printable);

	if (str_unlock(str) == -1)
	{
		str_release(ret);
		return NULL;
	}

	return ret;
}

/*

=item C<String *encode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<str_encode()> but works on an ordinary C string.

=cut

*/

String *encode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	return encode_locked(NULL, str, uncoded, coded, quote_char, printable);
}

/*

=item C<String *encode_locked(Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<encode()> except that multiple threads accessing the new string
will be synchronised by C<locker>.

=cut

*/

String *encode_locked(Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	if (!str || !uncoded || !coded)
		return NULL;

	return do_encode_locked(locker, str, strlen(str), uncoded, coded, quote_char, printable);
}

/*

=item C<String *decode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<str_decode()> but works on an ordinary C string.

=cut

*/

String *decode(const char *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	return decode_locked(NULL, str, uncoded, coded, quote_char, printable);
}

/*

=item C<String *decode_locked(Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable)>

Equivalent to I<decode()> except that multiple threads accessing the new string
will be synchronised by C<locker>.

=cut

*/

String *decode_locked(Locker *locker, const char *str, const char *uncoded, const char *coded, char quote_char, int printable)
{
	if (!str || !uncoded || !coded)
		return NULL;

	return do_decode_locked(locker, str, strlen(str), uncoded, coded, quote_char, printable);
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

	if (str_wrlock(str) == -1)
		return NULL;

	for (i = 0; i < str->length - 1; ++i)
		str->str[i] = to_lower(str->str[i]);

	if (str_unlock(str) == -1)
		return NULL;

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
		*s = to_lower(*s);

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

	if (str_wrlock(str) == -1)
		return NULL;

	if (str->length > 1)
		*str->str = to_lower(*str->str);

	if (str_unlock(str) == -1)
		return NULL;

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

	*str = to_lower(*str);

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

	if (str_wrlock(str) == -1)
		return NULL;

	for (i = 0; i < str->length - 1; ++i)
		str->str[i] = to_upper(str->str[i]);

	if (str_unlock(str) == -1)
		return NULL;

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
		*s = to_upper(*s);

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

	if (str_wrlock(str) == -1)
		return NULL;

	if (str->length > 1)
		*str->str = to_upper(*str->str);

	if (str_unlock(str) == -1)
		return NULL;

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

	*str = to_upper(*str);

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

	if (!str)
		return -1;

	if (str_wrlock(str) == -1)
		return -1;

	if (str->length == 1)
	{
		str_unlock(str);
		return -1;
	}

	ret = str->str[str->length - 2];

	if (contract(str, str->length - 2, 1) == -1)
	{
		str_unlock(str);
		return -1;
	}

	if (str_unlock(str) == -1)
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
	int ret;

	if (!str)
		return -1;

	if (str_wrlock(str) == -1)
		return -1;

	if (str->length == 1)
	{
		if (str_unlock(str) == -1)
			return -1;
		return 0;
	}

	length = str->length;

	for (s = str->str + str->length - 2; *s == '\n' || *s == '\r'; --s)
		if (contract(str, str->length - 2, 1) == -1)
		{
			str_unlock(str);
			return -1;
		}

	ret = length - str->length;

	if (str_unlock(str) == -1)
		return -1;

	return ret;
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
	int ret;

	if (!str)
		return 0;

	if (str_rdlock(str) == -1)
		return 0;

	ret = bin(str->str);

	if (str_unlock(str) == -1)
		return 0;

	return ret;
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
	int ret;

	if (!str)
		return 0;

	if (str_rdlock(str) == -1)
		return 0;

	ret = hex(str->str);

	if (str_unlock(str) == -1)
		return 0;

	return ret;
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
	int ret;

	if (!str)
		return 0;

	if (str_rdlock(str) == -1)
		return 0;

	ret = oct(str->str);

	if (str_unlock(str) == -1)
		return 0;

	return ret;
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

#ifdef NEEDS_STRCASECMP

/*

=item I<int strcasecmp(const char *s1, const char *s2)>

Compares two strings, C<s1> and C<s2>, ignoring the case of the characters.
It returns an integer less than, equal to, or greater than zero if C<s1> is
found to be less than, equal to, or greater than C<s2>, respectively.

=cut

*/

int strcasecmp(const char *s1, const char *s2)
{
	while (*s1 && *s2)
	{
		int c1 = to_lower(*s1++);
		int c2 = to_lower(*s2++);

		if (c1 != c2)
			return c1 - c2;
	}

	return to_lower(*s1) - to_lower(*s2);
}

#endif

#ifdef NEEDS_STRNCASECMP

/*

=item I<int strncasecmp(const char *s1, const char *s2, size_t n)>

Equivalent to I<strcasecmp()> except that it only compares the first C<n>
characters.

=cut

*/

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	while (n-- && *s1 && *s2)
	{
		int c1 = to_lower(*s1++);
		int c2 = to_lower(*s2++);

		if (c1 != c2)
			return c1 - c2;
	}

	return to_lower(*s1) - to_lower(*s2);
}

#endif

#ifdef NEEDS_STRLCPY

/*

=item I<size_t strlcpy(char *dst, const char *src, size_t size)>

Copies C<src> into C<dst> (which is C<size> bytes long). The result, C<dst>,
will be no longer than C<size - 1> bytes and will be C<nul> terminated
(unless C<size> is zero). This is similar to I<strncpy()> except that it
always terminates the string with a C<nul> byte (so it's safer) and it
doesn't fill the remainder of the buffer with C<nul> bytes (so it's faster).
Returns the length of C<src> (If this is >= C<size>, truncation occurred).

=cut

*/

size_t strlcpy(char *dst, const char *src, size_t size)
{
	const char *s = src;
	char *d = dst;
	size_t n = size;

	if (n)
		while (--n && (*d++ = *s++))
		{}

	if (n == 0)
	{
		if (size)
			*d = '\0';

		while (*s++)
		{}
	}

	return s - src - 1;
}

#endif

#ifdef NEEDS_STRLCAT

/*

=item I<size_t strlcat(char *dst, const char *src, size_t size)>

Appends C<src> to C<dst> (which is C<size> bytes long). The result, C<dst>,
will be no longer than C<size - 1> bytes and will be C<nul> terminated
(unless C<size> is zero). This is similar to I<strncat()> except that the
last argument is the size of the buffer, not the amount of space available.
(so it's more intuitive and hence safer). Returns the sum of the lengths of
C<src> and C<dst> (If this is >= C<size>, truncation occurred).

=cut

*/

size_t strlcat(char *dst, const char *src, size_t size)
{
	const char *s = src;
	char *d = dst;
	size_t n = size;
	size_t dlen = 0;

	while (n-- && *d)
		++d;

	dlen = d - dst;

	if (++n == 0)
	{
		while (*s++)
		{}

		return dlen + s - src - 1;
	}

	for (; *s; ++s)
		if (n - 1)
			--n, *d++ = *s;

	*d = '\0';

	return dlen + s - src;
}

#endif

/*

=back

=head1 MT-Level

MT-Disciplined

By default, I<String>s are not MT-Safe because most programs are single
threaded and synchronisation doesn't come for free. Even in multi threaded
programs, not all I<String>s are necessarily shared between multiple
threads.

When a I<String> is shared between multiple threads which need to be
synchronised, the method of synchronisation must be carefully selected by the
client code. There are tradeoffs between concurrency and overhead. The greater
the concurrency, the greater the overhead. More locks give greater concurrency
but have greater overhead. Readers/Writer locks can give greater concurrency
than Mutex locks but have greater overhead. One lock for each I<String> may be
required, or one lock for all (or a set of) I<String>s may be more appropriate.

Generally, the best synchronisation strategy for a given application can only
be determined by testing/benchmarking the written application. It is important
to be able to experiment with the synchronisation strategy at this stage of
development without pain.

To facilitate this, I<String>s can be created with I<string_create_locked()>
which takes a I<Locker> argument. The I<Locker> specifies a lock and a set of
functions for manipulating the lock. Each I<String> can have it's own lock by
creating a separate I<Locker> for each I<String>. Multiple I<String>s can share
the same lock by sharing the same I<Locker>. Only the application developer can
determine what is appropriate for each application on a string by string basis.

MT-Disciplined means that the application developer has a mechanism for
specifying the synchronisation requirements to be applied to library code.

=head1 BUGS

Doesn't support multibyte/widechar strings, UTF8, UNICODE or ISO 10646
but suport can probably be layered over the top of I<String>.

The C<delim> parameter to the I<split()> and I<join()> functions is an
ordinary C string so it can't contain C<nul> characters.

The C<quotable> parameter to the I<quote()> and I<unquote()> functions is an
ordinary C string so it can't contain C<nul> characters.

The C<uncoded> and C<coded> parameters to the I<str_encode()> and
I<str_decode()> functions are ordinary C strings so they can't contain
C<nul> characters.

Uses I<malloc(3)>. Need to decouple memory type and allocation strategy from
this code.

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<thread(3)|thread(3)>,
L<string(3)|string(3)>,
L<regcomp(3)|regcomp(3)>,
L<regexec(3)|regexec(3)>,
L<regerror(3)|regerror(3)>,
L<regfree(3)|regfree(3)>,
L<perlfunc(1)|perlfunc(1)>,
L<perlop(1)|perlop(1)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

=cut

*/

#ifdef TEST

#include <semaphore.h>

void str_print(const char *str, size_t length)
{
	int i;

	printf("\"");

	for (i = 0; i < length + 1; ++i)
		printf(is_print(str[i]) ? "%c" : "\\%03o", (unsigned char)str[i]);

	printf("\"");
}

String *mtstr = NULL;
Locker *locker = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
sem_t barrier;
sem_t length;
const int lim = 1000;
int debug = 0;
int errors = 0;
pthread_mutex_t errors_lock = PTHREAD_MUTEX_INITIALIZER;

int bugger()
{
	pthread_mutex_lock(&mutex);
	++errors;
	pthread_mutex_unlock(&mutex);
	return -1;
}

void *produce(void *arg)
{
	int test = *(int *)arg >> 16;
	int id = *(int *)arg & 0x0000ffff;
	int i;

	if (debug)
		printf("p%d: loop\n", id);

	for (i = 0; i <= lim; ++i)
	{
		char c = 'a' + (i % 26);

		if (debug)
			printf("p%d: str_append %d\n", id, i);

		if (!str_append(mtstr, "%c", c))
			bugger(), printf("Test%d: str_append(mtstr, '\\%o'), failed\n", test, c);

		sem_post(&length);
	}

	sem_post(&barrier);
	return NULL;
}

void *consume(void *arg)
{
	int test = *(int *)arg >> 16;
	int id = *(int *)arg & 0x0000ffff;
	int i;

	if (debug)
		printf("c%d: loop\n", id);

	for (i = 0; i < lim; ++i)
	{
		while (sem_wait(&length) != 0)
		{}

		if (debug)
			printf("c%d: str_remove\n", id);

		if (!str_remove(mtstr, 0))
			bugger(), printf("Test%d: str_remove(mtstr, 0) failed\n", test);
	}

	if (i != lim)
		bugger(), printf("Test%d: consumer read %d items, not %d\n", test, i, lim);

	sem_post(&barrier);
	return NULL;
}

void *writer(void *arg)
{
	int test = *(int *)arg >> 16;
	int id = *(int *)arg & 0x0000ffff;
	int i;

	if (debug)
		printf("w%d: loop\n", id);

	for (i = 0; i < lim; ++i)
	{
		String *str;

		if (debug)
			printf("w%d: loop %d/%d\n", id, i, lim / 10);

		if (debug)
			printf("w%d: loop %d/%d wrlock/cstr/set_length/recalc_length\n", id, i, lim / 10);

		if (str_wrlock(mtstr) == -1)
			bugger(), printf("Test%d: str_wrlock(mtstr) failed (%s)\n", test, strerror(errno));
		else
		{
			char *str = cstr(mtstr);
			size_t len = str_length_unlocked(mtstr);

			str[0] = 'a';

			if (str_set_length_unlocked(mtstr, len) == -1)
				bugger(), printf("Test%d: str_set_length_unlocked(mtstr, %d) failed\n", test, len);

			if (str_recalc_length_unlocked(mtstr) == -1)
				bugger(), printf("Test%d: str_recalc_length_unlocked(mtstr) failed\n", test);

			if (str_unlock(mtstr) == -1)
				bugger(), printf("Test%d: str_unlock(mtstr) failed (%s)\n", test, strerror(errno));
		}

		if (debug)
			printf("w%d: loop %d/%d replace\n", id, i, lim / 10);

		if (!str_replace(mtstr, 0, -1, "abc"))
			bugger(), printf("Test%d: str_replace(mtstr, 0, -1, \"abc\") failed\n", test);

		if (debug)
			printf("w%d: loop %d/%d splice\n", id, i, lim / 10);

		if (!(str = str_splice(mtstr, 0, 0)))
			bugger(), printf("Test%d: str_splice(mtstr, 0, 0) failed\n", test);
		else
			str_destroy(&str);

		if (debug)
			printf("w%d: loop %d/%d tr\n", id, i, lim / 10);

		if (str_tr(mtstr, "a-z", "A-Z", 0) == -1)
			bugger(), printf("Test%d: str_tr(mtstr, \"a-z\", \"A-Z\", 0) failed\n", test);

		if (debug)
			printf("w%d: loop %d/%d regsub\n", id, i, lim / 10);

		str_regsub("[a-z]", "A", mtstr, 0, 0, 1);

		if (debug)
			printf("w%d: loop %d/%d trim\n", id, i, lim / 10);

		if (!str_trim(mtstr))
			bugger(), printf("Test%d: str_trim(mtstr) failed\n", test);
		if (!str_trim_left(mtstr))
			bugger(), printf("Test%d: str_trim_left(mtstr) failed\n", test);
		if (!str_trim_right(mtstr))
			bugger(), printf("Test%d: str_trim_right(mtstr) failed\n", test);

		if (debug)
			printf("w%d: loop %d/%d squeeze\n", id, i, lim / 10);

		if (!str_squeeze(mtstr))
			bugger(), printf("Test%d: str_squeeze(mtstr) failed\n", test);

		if (debug)
			printf("w%d: loop %d/%d lc/uc/lcfirst/ucfirst\n", id, i, lim / 10);

		if (!str_lc(mtstr))
			bugger(), printf("Test%d: str_lc(mtstr) failed\n", test);
		if (!str_uc(mtstr))
			bugger(), printf("Test%d: str_uc(mtstr) failed\n", test);
		if (!str_lcfirst(mtstr))
			bugger(), printf("Test%d: str_lcfirst(mtstr) failed\n", test);
		if (!str_ucfirst(mtstr))
			bugger(), printf("Test%d: str_ucfirst(mtstr) failed\n", test);

		if (debug)
			printf("w%d: loop %d/%d chop/chomp\n", id, i, lim / 10);

		if (str_chop(mtstr) == -1)
			bugger(), printf("Test%d: str_chop(mtstr) failed\n", test);
		if (str_chomp(mtstr) == -1)
			bugger(), printf("Test%d: str_chomp(mtstr) failed\n", test);
	}

	sem_post(&barrier);
	return NULL;
}

void *reader(void *arg)
{
	int test = *(int *)arg >> 16;
	int id = *(int *)arg & 0x0000ffff;
	int i;

	if (debug)
		printf("r%d: loop\n", id);

	for (i = 0; i < lim / 10; ++i)
	{
		String *str;
		List *list;
		size_t length;
		int empty;
		
		str = str_copy(mtstr);
		if (debug)
			printf("r%d: loop %d/%d str = '%s'\n", id, i, lim / 10, cstr(str));
		str_destroy(&str);

		empty = str_empty(mtstr);
		if (debug)
			printf("r%d: loop %d/%d empty = %d\n", id, i, lim / 10, empty);

		length = str_length(mtstr);
		if (debug)
			printf("r%d: loop %d/%d length = %d\n", id, i, lim / 10, length);

		if (str_rdlock(mtstr) == -1)
			bugger(), printf("Test%d: str_rdlock(mtstr) failed (%s)\n", test, strerror(errno));
		else
		{
			const char *s = cstr(mtstr);

			if (debug)
				printf("r%d: loop %d/%d cstr = \"%s\"\n", id, i, lim / 10, s);

			if (str_unlock(mtstr) == -1)
				bugger(), printf("Test%d: str_unlock(mtstr) failed (%s)\n", test, strerror(errno));
		}

		if (!(str = str_create("")))
			bugger(), printf("Test%d: str_create() failed\n", test);
		else
		{
			if (!str_insert_str(str, 0, mtstr))
				bugger(), printf("Test%d: str_insert_str(str, 0, mtstr) failed\n", test);

			if (!str_replace_str(str, 0, 0, mtstr))
				bugger(), printf("Test%d: str_replace_str(str, 0, 0, mtstr) failed\n", test);

			str_destroy(&str);
		}

		if (!(list = str_fmt(mtstr, 10, ALIGN_FULL)))
			bugger(), printf("Test%d: str_fmt(mtstr, 10, ALIGN_FULL) failed\n", test);
		else
			list_destroy(&list);

		if (!(list = str_split(mtstr, "")))
			bugger(), printf("Test%d: str_split(mtstr, \"\") failed\n", test);
		else
			list_destroy(&list);

		if (!(list = str_regexpr("[a-z]?", mtstr, 0, 0)))
			bugger(), printf("Test%d: str_regexpr(\"[a-z]?\", mtstr, 0, 0) failed\n", test);
		else
			list_destroy(&list);

		if (!(list = str_regexpr_split(mtstr, "", 0, 0)))
			bugger(), printf("Test%d: str_regexpr_split(mtstr, \"\") failed\n", test);
		else
			list_destroy(&list);

		if (!(str = str_encode(mtstr, "abcdef", "abcdef", '\\', 1)))
			bugger(), printf("Test%d: str_encode(mtstr, \"abcdef\", \"abcdef\", '\\') failed\n", test);
		else
			str_destroy(&str);

		if (!(str = str_decode(mtstr, "abcdef", "abcdef", '\\', 1)))
			bugger(), printf("Test%d: str_decode(mtstr, \"abcdef\", \"abcdef\", '\\') failed\n", test);
		else
			str_destroy(&str);

		str_bin(mtstr);
		str_hex(mtstr);
		str_oct(mtstr);
	}

	sem_post(&barrier);
	return NULL;
}

void mt_test(int test, Locker *locker)
{
	if (!(mtstr = str_create_locked(locker, NULL)))
		bugger(), printf("Test%d: str_create_locked(NULL) failed\n", test);
	else
	{
		static int iid[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
		pthread_attr_t attr;
		pthread_t id;
		int i;

		for (i = 0; i < 9; ++i)
		{
			iid[i] &= 0x0000ffff;
			iid[i] |= test << 16;
		}

		sem_init(&barrier, 0, 0);
		sem_init(&length, 0, 0);
		thread_attr_init(&attr);
		pthread_create(&id, &attr, produce, iid + 0);
		pthread_create(&id, &attr, produce, iid + 1);
		pthread_create(&id, &attr, produce, iid + 2);
		pthread_create(&id, &attr, consume, iid + 3);
		pthread_create(&id, &attr, consume, iid + 4);
		pthread_create(&id, &attr, consume, iid + 5);
		pthread_create(&id, &attr, writer,  iid + 6);
		pthread_create(&id, &attr, writer,  iid + 7);
		pthread_create(&id, &attr, writer,  iid + 8);
		pthread_create(&id, &attr, reader,  iid + 9);
		pthread_create(&id, &attr, reader,  iid + 10);
		pthread_create(&id, &attr, reader,  iid + 11);
		pthread_attr_destroy(&attr);

		for (i = 0; i < 12; ++i)
			while (sem_wait(&barrier) != 0)
			{}

		str_destroy(&mtstr);
		if (mtstr)
			bugger(), printf("Test%d: str_destroy(&mtstr) failed\n", test);
	}
}

int main(int ac, char **av)
{
	const char * const testfile = "str_fgetline.test";
	String *a, *b, *c;
	char tst[BUFSIZ];
	List *list;
	StrTR *trtable;
	int i, big, rc;
	FILE *stream;
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
		++errors, printf("Test%d: %s failed: length %u (not %u)\n", (i), (#action), str_length(str), (length));

#define CHECK_CLEN(i, action, str, length) \
	if (strlen(str) != (length)) \
		++errors, printf("Test%d: %s failed: length %u (not %u)\n", (i), (#action), strlen(str), (length));

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
	TEST_ACT(13, !str_destroy(&c))
	TEST_ACT(14, !str_copy(NULL))

	/* Test str_fgetline() */

#define TEST_FGETLINE(test_num, test_length) \
	TEST_ACT((test_num), stream = fopen(testfile, "wb")) \
	else \
	{ \
		const size_t length = (test_length); \
		for (i = 0; i < length - 1; ++i) \
			fputc('a' + i % 26, stream); \
		fputc('\n', stream); \
		fclose(stream); \
		TEST_ACT((test_num), stream = fopen(testfile, "r")) \
		else \
		{ \
			String *line = str_fgetline(stream); \
			fclose(stream); \
			if (!line) \
				++errors, printf("Test%d: str_fgetline() failed: returned NULL\n", (test_num)); \
			else \
			{ \
				if (str_length(line) != length) \
					++errors, printf("Test%d: str_fgetline() failed: length is %d, not %d\n", (test_num), str_length(line), length); \
				else \
				{ \
					for (i = 0; i < length - 1; ++i) \
						if (cstr(line)[i] != 'a' + i % 26) \
						{ \
							++errors, printf("Test%d: str_fgetline() failed: pos %d contains '\\x%02x', not '\\x%02x'\n", (test_num), i, cstr(line)[i], 'a' + i % 26); \
							break; \
						} \
					if (cstr(line)[length - 1] != '\n') \
						++errors, printf("Test%d: str_fgetline() failed: last char is '\\x%02x', not '\\x%02x'\n", (test_num), cstr(line)[length - 1], '\n'); \
				} \
				str_destroy(&line); \
			} \
		} \
	} \
	unlink(testfile);

	TEST_FGETLINE(15, 7)
	TEST_FGETLINE(16, 127)
	TEST_FGETLINE(17, BUFSIZ - 1)
	TEST_FGETLINE(18, BUFSIZ)
	TEST_FGETLINE(19, BUFSIZ + 1)
	TEST_FGETLINE(20, (BUFSIZ - 20) * 4)

	/* Test insert, append, prepend, remove, replace */

	TEST_STR(21, str_remove(a, 30), a, 30, "This is a test string abc x 37")
	TEST_STR(22, str_remove(a, 0), a, 29, "his is a test string abc x 37")
	TEST_STR(23, str_remove(a, 10), a, 28, "his is a tst string abc x 37")
	TEST_STR(24, str_replace(a, 0, 0, "123"), a, 31, "123his is a tst string abc x 37")
	TEST_STR(25, str_replace(a, 1, 1, "123"), a, 33, "11233his is a tst string abc x 37")
	TEST_STR(26, str_replace(a, 0, 5, "456"), a, 31, "456his is a tst string abc x 37")
	TEST_STR(27, str_replace(a, 30, 1, "789"), a, 33, "456his is a tst string abc x 3789")
	TEST_STR(28, str_replace(a, 33, 0, "a"), a, 34, "456his is a tst string abc x 3789a")
	TEST_STR(29, str_remove_range(a, 0, 0), a, 34, "456his is a tst string abc x 3789a")
	TEST_STR(30, str_remove_range(a, 0, 3), a, 31, "his is a tst string abc x 3789a")
	TEST_STR(31, str_remove_range(a, 13, 0), a, 31, "his is a tst string abc x 3789a")
	TEST_STR(32, str_remove_range(a, 13, 7), a, 24, "his is a tst abc x 3789a")
	TEST_STR(33, str_remove_range(a, 23, 0), a, 24, "his is a tst abc x 3789a")
	TEST_STR(34, str_remove_range(a, 21, 2), a, 22, "his is a tst abc x 37a")

	TEST_ACT(35, c = str_create("__test__"))

	TEST_STR(36, str_prepend_str(a, c), a, 30, "__test__his is a tst abc x 37a")
	TEST_STR(37, str_insert_str(b, 1, c), b, 17, "d__test__ghiefabc")
	TEST_STR(38, str_append_str(a, b), a, 47, "__test__his is a tst abc x 37ad__test__ghiefabc")

	TEST_STR(39, str_replace_str(a, 1, 2, b), a, 62,  "_d__test__ghiefabcest__his is a tst abc x 37ad__test__ghiefabc")
	TEST_STR(40, str_replace_str(a, 60, 2, b), a, 77, "_d__test__ghiefabcest__his is a tst abc x 37ad__test__ghiefad__test__ghiefabc")
	TEST_STR(41, str_remove_range(b, 1, 3), b, 14, "dest__ghiefabc")

	TEST_STR(42, str_prepend(b, NULL), b, 14, "dest__ghiefabc")
	TEST_STR(43, str_insert(b, 5, NULL), b, 14, "dest__ghiefabc")
	TEST_STR(44, str_append(b, NULL), b, 14, "dest__ghiefabc")
	TEST_STR(45, str_prepend(b, ""), b, 14, "dest__ghiefabc")
	TEST_STR(46, str_insert(b, 5, ""), b, 14, "dest__ghiefabc")
	TEST_STR(47, str_append(b, ""), b, 14, "dest__ghiefabc")
	str_destroy(&a);
	str_destroy(&b);
	str_destroy(&c);

	/* Test relative index/range */

	TEST_STR(48, a = str_create("0123456789"), a, 10, "0123456789")
	TEST_STR(49, str_remove_range(a, -5, -1), a, 6, "012345")
	TEST_STR(50, str_remove_range(a, -1, -1), a, 6, "012345")
	TEST_STR(51, str_remove_range(a, -3, -2), a, 5, "01235")
	TEST_STR(52, str_insert(a, -1, "abc"), a, 8, "01235abc")
	TEST_STR(53, str_replace(a, -5, -2, "XYZ"), a, 8, "0123XYZc")
	TEST_STR(54, b = str_substr(a, -4, -2), b, 2, "YZ")
	TEST_STR(55, c = substr(cstr(a), -4, -2), c, 2, "YZ")
	str_destroy(&a);
	str_destroy(&b);
	str_destroy(&c);

	/* Test str_repeat() */

	TEST_STR(56, c = str_repeat(0, ""), c, 0, "")
	str_destroy(&c);
	TEST_STR(57, c = str_repeat(10, ""), c, 0, "")
	str_destroy(&c);
	TEST_STR(58, c = str_repeat(0, "%d", 11 * 11), c, 0, "")
	str_destroy(&c);
	TEST_STR(59, c = str_repeat(10, "%d", 11 * 11), c, 30, "121121121121121121121121121121")
	str_destroy(&c);
	TEST_STR(60, c = str_repeat(10, " "), c, 10, "          ")
	str_destroy(&c);

	/* Test big string creation and big string growth (big KB) */

	big = 64;
	TEST_LEN(61, a = str_create("%*s", 1024 * big, ""), a, 1024 * big)
	str_destroy(&a);

	TEST_ACT(62, a = str_create(NULL))

	for (i = 0; i < big; ++i)
	{
		if (!str_append(a, "%1024s", ""))
		{
			++errors, printf("Test62: str_append(\"%%1024s\", \"\") failed (on iteration %d)\n", i);
			break;
		}

		if (str_length(a) != ((i + 1) * 1024))
		{
			++errors, printf("Test62: str_append(\"%%1024s\", \"\") failed: (on iteration %d) length %d, not %d\n", i, str_length(a), (i + 1) * 1024);
			break;
		}
	}

	str_destroy(&a);

	/* Test big string sized creation and big string growth (big KB) */

	TEST_LEN(63, a = str_create_sized(1024 * big, "%*s", 1024 * big, ""), a, 1024 * big)
	str_destroy(&a);

	TEST_ACT(64, a = str_create_sized(1024 * big, NULL))

	for (i = 0; i < big; ++i)
	{
		if (!str_append(a, "%1024s", ""))
		{
			++errors, printf("Test64: str_append(\"%%1024s\", \"\") failed (on iteration %d)\n", i);
			break;
		}

		if (str_length(a) != ((i + 1) * 1024))
		{
			++errors, printf("Test64: str_append(\"%%1024s\", \"\") failed: (on iteration %d) length %d, not %d\n", i, str_length(a), (i + 1) * 1024);
			break;
		}
	}

	str_destroy(&a);

	/* Test substr */

	TEST_ACT(65, a = str_create("abcdefghijkl"))
	TEST_STR(66, b = str_substr(a, 0, 0), b, 0, "")
	str_destroy(&b);
	TEST_STR(67, c = str_substr(a, 0, 3), c, 3, "abc")
	str_destroy(&c);
	TEST_STR(68, b = str_substr(a, 6, 0), b, 0, "")
	str_destroy(&b);
	TEST_STR(69, c = str_substr(a, 6, 3), c, 3, "ghi")
	str_destroy(&c);
	TEST_STR(70, b = str_substr(a, 9, 0), b, 0, "")
	str_destroy(&b);
	TEST_STR(71, c = str_substr(a, 9, 3), c, 3, "jkl")
	str_destroy(&c);
	TEST_STR(72, c = str_substr(a, 0, 12), c, 12, "abcdefghijkl")
	str_destroy(&c);
	str_destroy(&a);

	TEST_STR(73, b = substr("abcdefghijkl", 0, 0), b, 0, "")
	str_destroy(&b);
	TEST_STR(74, c = substr("abcdefghijkl", 0, 3), c, 3, "abc")
	str_destroy(&c);
	TEST_STR(75, b = substr("abcdefghijkl", 6, 0), b, 0, "")
	str_destroy(&b);
	TEST_STR(76, c = substr("abcdefghijkl", 6, 3), c, 3, "ghi")
	str_destroy(&c);
	TEST_STR(77, b = substr("abcdefghijkl", 9, 0), b, 0, "")
	str_destroy(&b);
	TEST_STR(78, c = substr("abcdefghijkl", 9, 3), c, 3, "jkl")
	str_destroy(&c);
	TEST_STR(79, c = substr("abcdefghijkl", 0, 12), c, 12, "abcdefghijkl")
	str_destroy(&c);
	str_destroy(&a);

	/* Test splice */

#define TEST_SPLICE(i, action, ostr, olen, oval, nstr, nlen, nval) \
	TEST_ACT(i, action) \
	CHECK_STR(i, action, ostr, olen, oval) \
	CHECK_STR(i, action, nstr, nlen, nval) \
	str_destroy(&(nstr));

	TEST_ACT   (80, a = str_create("aaabbbcccdddeeefffggghhhiiijjjkkklll"))
	TEST_SPLICE(81, b = str_splice(a, 0, 0), a, 36, "aaabbbcccdddeeefffggghhhiiijjjkkklll", b, 0, "")
	TEST_SPLICE(82, b = str_splice(a, 0, 3), a, 33, "bbbcccdddeeefffggghhhiiijjjkkklll", b, 3, "aaa")
	TEST_SPLICE(83, b = str_splice(a, 3, 0), a, 33, "bbbcccdddeeefffggghhhiiijjjkkklll", b, 0, "")
	TEST_SPLICE(84, b = str_splice(a, 3, 6), a, 27, "bbbeeefffggghhhiiijjjkkklll", b, 6, "cccddd")
	TEST_SPLICE(85, b = str_splice(a, 3, 0), a, 27, "bbbeeefffggghhhiiijjjkkklll", b, 0, "")
	TEST_SPLICE(86, b = str_splice(a, 24, 3), a, 24, "bbbeeefffggghhhiiijjjkkk", b, 3, "lll")
	str_destroy(&a);

	/* Test tr */

#define TEST_TR(i, action, orig, str, bytes, length, value) \
	TEST_ACT(i, str = str_copy(orig)) \
	TEST_EQ(i, action, bytes) \
	CHECK_STR(i, action, str, length, value) \
	str_destroy(&(str));

	TEST_ACT(87, a = str_create(" .,;'/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))
	TEST_ACT(88, b = str_create("bookkeeper"))
	TEST_TR (89, str_tr(c, "A-Z", "a-z", 0), a, c, 26, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz")
	TEST_TR (90, str_tr(c, "a-z", "A-Z", 0), a, c, 26, 68, " .,;'/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (91, str_tr(c, "a", "a", 0), a, c, 1, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (92, str_tr(c, "0-9", NULL, 0), a, c, 10, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (93, str_tr(c, "a-zA-Z", NULL, TR_SQUASH), b, c, 10, 7, "bokeper")
	TEST_TR (94, str_tr(c, "a-zA-Z", " ", TR_SQUASH), b, c, 10, 1, " ")
	TEST_TR (95, str_tr(c, "a-zA-Z", " ", TR_COMPLEMENT | TR_SQUASH), a, c, 16, 53, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (96, str_tr(c, "AAA", "XYZ", 0), a, c, 1, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzXBCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (97, str_tr(c, "a-z", "*", 0), a, c, 26, 68, " .,;'/0123456789**************************ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (98, str_tr(c, "a-z", "*", TR_COMPLEMENT), a, c, 42, 68, "****************abcdefghijklmnopqrstuvwxyz**************************")
	TEST_TR (99, str_tr(c, "a-z", " ", TR_SQUASH), a, c, 26, 43, " .,;'/0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (100, str_tr(c, "a-z", " ", TR_SQUASH | TR_COMPLEMENT), a, c, 42, 28, " abcdefghijklmnopqrstuvwxyz ")
	TEST_TR (101, str_tr(c, "a-z", "x-z", TR_DELETE), a, c, 26, 45, " .,;'/0123456789xyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (102, str_tr(c, "a-z", "", TR_DELETE), a, c, 26, 42, " .,;'/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (103, str_tr(c, "a-z", "   ", TR_DELETE), a, c, 26, 45, " .,;'/0123456789   ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (104, str_tr(c, "a-z", "", TR_DELETE | TR_COMPLEMENT), a, c, 42, 26, "abcdefghijklmnopqrstuvwxyz")
	TEST_TR (105, str_tr(c, "a-z", "                                                           ", TR_DELETE | TR_COMPLEMENT), a, c, 42, 41, "               abcdefghijklmnopqrstuvwxyz")
	TEST_TR (106, str_tr(c, "a-z", "                                                           ", TR_DELETE | TR_COMPLEMENT | TR_SQUASH), a, c, 42, 27, " abcdefghijklmnopqrstuvwxyz")
	TEST_TR (107, str_tr(c, "a-z", "             ", TR_DELETE), a, c, 26, 55, " .,;'/0123456789             ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (108, str_tr(c, "a-z", "             ", TR_DELETE | TR_SQUASH), a, c, 26, 43, " .,;'/0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (109, str_tr(c, "a-z", "a-b", 0), a, c, 26, 68, " .,;'/0123456789abbbbbbbbbbbbbbbbbbbbbbbbbABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (110, str_tr(c, "a-zA-Z", "A-Za-z", 0), a, c, 52, 68, " .,;'/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
	TEST_TR (111, str_tr(c, "a-zA-Z", "A-Za-z", TR_COMPLEMENT), a, c, 16, 68, "gusznvwxyzzzzzzzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
	TEST_TR (112, str_tr(c, "A-Z", "a-m", 0), a, c, 26, 68, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmmmmmmmmmmmmmm")
	TEST_TR (113, str_tr(c, "A-Z", "a-m", TR_SQUASH), a, c, 26, 55, " .,;'/0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklm")
	TEST_TR (114, str_tr(c, "a-zA-Z", "n-za-mN-ZA-M", 0), a, c, 52, 68, " .,;'/0123456789nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM")
	str_destroy(&a);
	str_destroy(&b);

#define TEST_TR_DIRECT(i, action, str, bytes, length, value) \
	TEST_EQ(i, action, bytes) \
	CHECK_STR(i, action, str, length, value)

	big = 1024;
	TEST_ACT(115, a = str_create("abcDEFghiJKLmnoPQRstuVWXyz"))
	TEST_ACT(115, trtable = tr_compile("a-zA-Z", "A-Za-z", 0))
	for (i = 0; i < big; ++i)
	{
		TEST_TR_DIRECT(115, tr_compiled(a->str, trtable), a, 26, 26, "ABCdefGHIjklMNOpqrSTUvwxYZ")
		TEST_TR_DIRECT(115, tr_compiled(a->str, trtable), a, 26, 26, "abcDEFghiJKLmnoPQRstuVWXyz")
	}
	TEST_ACT(115, !tr_destroy(&trtable))
	str_destroy(&a);

	TEST_ACT(116, a = str_create("\170\171\172\173\174\175\176\177\200\201\202\203\204\205\206\207\210"))
	TEST_ACT(116, b = str_create("\200-\377"))
	TEST_ACT(116, c = str_create("%c-\177", '\000'))
	TEST_TR_DIRECT(116, str_tr_str(a, b, c, 0), a, 9, 17, "\170\171\172\173\174\175\176\177\000\001\002\003\004\005\006\007\010")
	str_destroy(&a);
	str_destroy(&b);
	str_destroy(&c);

#ifndef REGEX_MISSING

	/* Test str_regexpr */

	TEST_ACT(117, a = str_create("abcabcabc"))
	else
	{
		TEST_ACT(117, list = str_regexpr("a((.*)a(.*))a", a, 0, 0))
		else
		{
			CHECK_LIST_LENGTH(117, str_regexpr(), list, 4)
			CHECK_LIST_ITEM(117, str_regexpr("a((.*)a(.*))a", "abcabcabc"), 0, "abcabca")
			CHECK_LIST_ITEM(117, str_regexpr("a((.*)a(.*))a", "abcabcabc"), 1, "bcabc")
			CHECK_LIST_ITEM(117, str_regexpr("a((.*)a(.*))a", "abcabcabc"), 2, "bc")
			CHECK_LIST_ITEM(117, str_regexpr("a((.*)a(.*))a", "abcabcabc"), 3, "bc")
			list_destroy(&list);
		}

		str_destroy(&a);
	}

	/* Test str_regsub */

#define TEST_REGSUB(i, str, pat, rep, cflags, eflags, all, len, res) \
	TEST_ACT((i), a = str_create("%s", (str))) \
	else \
	{ \
		TEST_STR((i), str_regsub((pat), (rep), a, (cflags), (eflags), (all)), a, (len), (res)) \
		str_destroy(&a); \
	}

	TEST_REGSUB(118, "xabcabcabcx", "a((.*)a(.*))a", "$0", 0, 0, 0, 11, "xabcabcabcx")
	TEST_REGSUB(119, "xabcabcabcx", "a((.*)a(.*))a", "$$$2${3}!", 0, 0, 0, 10, "x$bcbc!bcx")
	TEST_REGSUB(120, "xabcabcabcx", "a((.*)a(.*))a", "$$$2$31!", 0, 0, 0, 11, "x$bcbc1!bcx")
	TEST_REGSUB(121, "xabcabcabcx", "a", "z", 0, 0, 0, 11, "xzbcabcabcx")
	TEST_REGSUB(122, "xabcabcabcx", "a", "z", 0, 0, 1, 11, "xzbczbczbcx")
	TEST_REGSUB(123, "aba", "a", "z", 0, 0, 0, 3, "zba")
	TEST_REGSUB(124, "aba", "a", "z", 0, 0, 1, 3, "zbz")
	TEST_REGSUB(125, "xabcabcabcx", "((((((((((((((((((((((((((((((((a))))))))))))))))))))))))))))))))", "!${32}!", 0, 0, 0, 13, "x!a!bcabcabcx")
	TEST_REGSUB(126, "xabcabcabcx", "((((((((((((((((((((((((((((((((a))))))))))))))))))))))))))))))))", "!${32}!", 0, 0, 1, 17, "x!a!bc!a!bc!a!bcx")
	TEST_REGSUB(127, "\\a:b:c:d:e:f:G:H:I:", "(...)(..)(..)(..)(..)(..)(..)(..)(..)", "$1\\U$2\\Q$3\\l$4\\E$5\\E$6\\L$7\\Q\\u$8\\E\\E$9\\\\l", 0, 0, 0, 24, "\\a:B:C\\:d\\:E:f:g:H\\:I:\\l")
	TEST_REGSUB(128, "abcdef", "", "-", 0, 0, 0, 7, "-abcdef")
	TEST_REGSUB(129, "abcdef", "", "-", 0, 0, 1, 13, "-a-b-c-d-e-f-")

#endif

	/* Test fmt */

	/* Opening paragraphs from Silas Marner by George Eliot */

	TEST_ACT(130, a = str_create(
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

	TEST_ACT(130, list = str_fmt(a, 70, ALIGN_LEFT))
	CHECK_LIST_LENGTH(130, str_fmt(a, 70, ALIGN_LEFT), list, 14)
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 0,  "In the days when the spinning wheels hummed busily in the farmhouses")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 1,  "and even the great ladies, clothed in silk and thread lace, had their")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 2,  "toy spinning wheels of polished oak, there might be seen in districts")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 3,  "far away among the lanes, or deep in the bosom of the hills, certain")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 4,  "pallid undersized men, who, by the size of the brawny country folk,")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 5,  "looked like the remnants of a disinherited race. The shepherd's dog")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 6,  "barked fiercely when one of these alien-looking men appeared on the")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 7,  "upland, dark against the early winter sunset; for what dog likes a")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 8,  "figure bent under a heavy bag? And these pale men rarely stirred")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 9,  "abroad without that mysterious burden. The shepherd himself, though he")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 10, "had good reason to believe that the bag held nothing but flaxen")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 11, "thread, or else the long rolls of strong linen spun from that thread,")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 12, "was not quite sure that this trade of weaving, indispensible though it")
	CHECK_LIST_ITEM(130, str_fmt(a, 70, ALIGN_LEFT), 13, "was, could be carried on entirely without the help of the Evil One.")
	list_destroy(&list);

	TEST_ACT(131, list = str_fmt(a, 70, ALIGN_RIGHT))
	CHECK_LIST_LENGTH(131, str_fmt(a, 70, ALIGN_RIGHT), list, 14)
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 0,  "  In the days when the spinning wheels hummed busily in the farmhouses")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 1,  " and even the great ladies, clothed in silk and thread lace, had their")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 2,  " toy spinning wheels of polished oak, there might be seen in districts")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 3,  "  far away among the lanes, or deep in the bosom of the hills, certain")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 4,  "   pallid undersized men, who, by the size of the brawny country folk,")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 5,  "   looked like the remnants of a disinherited race. The shepherd's dog")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 6,  "   barked fiercely when one of these alien-looking men appeared on the")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 7,  "    upland, dark against the early winter sunset; for what dog likes a")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 8,  "      figure bent under a heavy bag? And these pale men rarely stirred")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 9,  "abroad without that mysterious burden. The shepherd himself, though he")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 10, "       had good reason to believe that the bag held nothing but flaxen")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 11, " thread, or else the long rolls of strong linen spun from that thread,")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 12, "was not quite sure that this trade of weaving, indispensible though it")
	CHECK_LIST_ITEM(131, str_fmt(a, 70, ALIGN_RIGHT), 13, "   was, could be carried on entirely without the help of the Evil One.")
	list_destroy(&list);

	TEST_ACT(132, list = str_fmt(a, 70, ALIGN_CENTRE));
	CHECK_LIST_LENGTH(132, str_fmt(a, 70, ALIGN_CENTRE), list, 16)
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 0,  "      In the days when the spinning wheels hummed busily in the")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 1,  "      farmhouses and even the great ladies, clothed in silk and")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 2,  "     thread lace, had their toy spinning wheels of polished oak,")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 3,  "      there might be seen in districts far away among the lanes,")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 4,  "     or deep in the bosom of the hills, certain pallid undersized")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 5,  "    men, who, by the size of the brawny country folk, looked like")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 6,  "    the remnants of a disinherited race. The shepherd's dog barked")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 7,  "     fiercely when one of these alien-looking men appeared on the")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 8,  "      upland, dark against the early winter sunset; for what dog")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 9,  "   likes a figure bent under a heavy bag? And these pale men rarely")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 10, "     stirred abroad without that mysterious burden. The shepherd")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 11, "   himself, though he had good reason to believe that the bag held")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 12, "  nothing but flaxen thread, or else the long rolls of strong linen")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 13, "     spun from that thread, was not quite sure that this trade of")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 14, "      weaving, indispensible though it was, could be carried on")
	CHECK_LIST_ITEM(132, str_fmt(a, 70, ALIGN_CENTRE), 15, "              entirely without the help of the Evil One.")
	list_destroy(&list);

	TEST_ACT(133, list = str_fmt(a, 70, ALIGN_FULL))
	CHECK_LIST_LENGTH(133, str_fmt(a, 70, ALIGN_FULL), list, 14)
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 0,  "In the days when the spinning wheels hummed busily in  the  farmhouses")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 1,  "and even the great ladies, clothed in silk and thread lace, had  their")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 2,  "toy spinning wheels of polished oak, there might be seen in  districts")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 3,  "far away among the lanes, or deep in the bosom of the  hills,  certain")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 4,  "pallid undersized men, who, by the size of the  brawny  country  folk,")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 5,  "looked like the remnants of a disinherited race.  The  shepherd's  dog")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 6,  "barked fiercely when one of these alien-looking men  appeared  on  the")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 7,  "upland, dark against the early winter sunset; for  what  dog  likes  a")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 8,  "figure bent under a heavy bag?  And  these  pale  men  rarely  stirred")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 9,  "abroad without that mysterious burden. The shepherd himself, though he")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 10, "had good reason to believe  that  the  bag  held  nothing  but  flaxen")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 11, "thread, or else the long rolls of strong linen spun from that  thread,")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 12, "was not quite sure that this trade of weaving, indispensible though it")
	CHECK_LIST_ITEM(133, str_fmt(a, 70, ALIGN_FULL), 13, "was, could be carried on entirely without the help of the Evil One.")
	list_destroy(&list);
	str_destroy(&a);

#define TEST_FMT3(i, alignment, width, line1, line2, line3) \
	TEST_ACT(i, list = str_fmt(a, width, alignment)) \
	CHECK_LIST_LENGTH(i, str_fmt(a, width, alignment), list, 3) \
	CHECK_LIST_ITEM(i, str_fmt(a, width, alignment), 0, line1) \
	CHECK_LIST_ITEM(i, str_fmt(a, width, alignment), 1, line2) \
	CHECK_LIST_ITEM(i, str_fmt(a, width, alignment), 2, line3) \
	list_destroy(&list);

	TEST_ACT(134, a = str_create("123456789\n1234567890\n12345678901"))
	TEST_FMT3(135, '<', 10, "123456789", "1234567890", "12345678901")
	TEST_FMT3(136, '>', 10, " 123456789", "1234567890", "12345678901")
	TEST_FMT3(137, '=', 10, "123456789", "1234567890", "12345678901")
	TEST_FMT3(138, '|', 10, "123456789", "1234567890", "12345678901")
	str_destroy(&a);

	TEST_ACT(139, a = str_create("12345678901\n123456789\n1234567890"))
	TEST_FMT3(140, '<', 10, "12345678901", "123456789", "1234567890")
	TEST_FMT3(141, '>', 10, "12345678901", " 123456789", "1234567890")
	TEST_FMT3(142, '=', 10, "12345678901", "123456789", "1234567890")
	TEST_FMT3(143, '|', 10, "12345678901", "123456789", "1234567890")
	str_destroy(&a);

	TEST_ACT(144, a = str_create("1234567890\n12345678901\n123456789"))
	TEST_FMT3(145, '<', 10, "1234567890", "12345678901", "123456789")
	TEST_FMT3(146, '>', 10, "1234567890", "12345678901", " 123456789")
	TEST_FMT3(147, '=', 10, "1234567890", "12345678901", "123456789")
	TEST_FMT3(148, '|', 10, "1234567890", "12345678901", "123456789")
	str_destroy(&a);

	TEST_ACT(149, a = str_create("12345678901\n1234567890\n123456789"))
	TEST_FMT3(150, '<', 10, "12345678901", "1234567890", "123456789")
	TEST_FMT3(151, '>', 10, "12345678901", "1234567890", " 123456789")
	TEST_FMT3(152, '=', 10, "12345678901", "1234567890", "123456789")
	TEST_FMT3(153, '|', 10, "12345678901", "1234567890", "123456789")
	str_destroy(&a);

	TEST_ACT(154, a = str_create("1234567890\n123456789\n12345678901"))
	TEST_FMT3(155, '<', 10, "1234567890", "123456789", "12345678901")
	TEST_FMT3(156, '>', 10, "1234567890", " 123456789", "12345678901")
	TEST_FMT3(157, '=', 10, "1234567890", "123456789", "12345678901")
	TEST_FMT3(158, '|', 10, "1234567890", "123456789", "12345678901")
	str_destroy(&a);

	TEST_ACT(159, a = str_create("123456789\n12345678901\n1234567890"))
	TEST_FMT3(160, '<', 10, "123456789", "12345678901", "1234567890")
	TEST_FMT3(161, '>', 10, " 123456789", "12345678901", "1234567890")
	TEST_FMT3(162, '=', 10, "123456789", "12345678901", "1234567890")
	TEST_FMT3(163, '|', 10, "123456789", "12345678901", "1234567890")
	str_destroy(&a);

	TEST_ACT(164, a = str_create(
		"http://www.zip.com.au/~joe/fairly/long/testing/url/\n"
		"hello there\n"
		"http://www.zip.com.au/~joe/fairly/long/testing/url/\n"
	))
	TEST_FMT3(165, '<', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "hello there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_FMT3(166, '>', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "                   hello there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_FMT3(167, '=', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "hello                    there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	TEST_FMT3(168, '|', 30, "http://www.zip.com.au/~joe/fairly/long/testing/url/", "         hello there", "http://www.zip.com.au/~joe/fairly/long/testing/url/")
	str_destroy(&a);

#define TEST_FMT0(i, alignment, width) \
	TEST_ACT(i, list = str_fmt(a, width, alignment)) \
	CHECK_LIST_LENGTH(i, str_fmt(a, width, alignment), list, 0) \
	list_destroy(&list);

	TEST_ACT(169, a = str_create(""))
	TEST_FMT0(170, '<', 10)
	TEST_FMT0(171, '>', 10)
	TEST_FMT0(172, '=', 10)
	TEST_FMT0(173, '|', 10)
	str_destroy(&a);

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
	str_destroy(&a); \
	list_destroy(&list);

#define TEST_SPLIT(i, func, str, delim, tok1, tok2, tok3, tok4) \
	TEST_ACT(i, list = func(str, delim)) \
	CHECK_LIST_LENGTH(i, split(str, delim), list, 4) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 0, tok1) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 1, tok2) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 2, tok3) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 3, tok4) \
	list_destroy(&list);

	TEST_SSPLIT(174, str_split, "a,b.c;d", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(175, str_split, " a , b . c ; d ", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(176, str_split, " a ,b . c;d: ", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(177, str_split, "a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(178, str_split, " a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SSPLIT(179, str_split, "aa,bb.cc;dd", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(180, str_split, " aa , bb . cc ; dd ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(181, str_split, " aa ,bb . cc;dd: ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(182, str_split, "aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(183, str_split, " aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SSPLIT(184, str_split, "abcd", "", "a", "b", "c", "d")

	TEST_SPLIT(185, split, "a,b.c;d", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(186, split, " a , b . c ; d ", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(187, split, " a ,b . c;d: ", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(188, split, "a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(189, split, " a,b.c;d:", " ,.;:", "a", "b", "c", "d")
	TEST_SPLIT(190, split, "aa,bb.cc;dd", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(191, split, " aa , bb . cc ; dd ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(192, split, " aa ,bb . cc;dd: ", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(193, split, "aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(194, split, " aa,bb.cc;dd:", " ,.;:", "aa", "bb", "cc", "dd")
	TEST_SPLIT(195, split, "abcd", "", "a", "b", "c", "d")

#ifndef REGEX_MISSING

	/* Test regexpr_split */

#define TEST_RE_SSPLIT(i, func, str, delim, tok1, tok2, tok3, tok4) \
	TEST_ACT(i, a = str_create(str)) \
	TEST_ACT(i, list = func(a, delim, 0, 0)) \
	CHECK_LIST_LENGTH(i, str_split(str, delim), list, 4) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 0, tok1) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 1, tok2) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 2, tok3) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 3, tok4) \
	str_destroy(&a); \
	list_destroy(&list);

#define TEST_RE_SPLIT(i, func, str, delim, tok1, tok2, tok3, tok4) \
	TEST_ACT(i, list = func(str, delim, 0, 0)) \
	CHECK_LIST_LENGTH(i, split(str, delim), list, 4) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 0, tok1) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 1, tok2) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 2, tok3) \
	CHECK_SPLIT_ITEM(i, func, str, delim, 3, tok4) \
	list_destroy(&list);

	TEST_RE_SSPLIT(196, str_regexpr_split, "a,b.c;d", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SSPLIT(197, str_regexpr_split, " a , b . c ; d ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SSPLIT(198, str_regexpr_split, " a ,b . c;d: ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SSPLIT(199, str_regexpr_split, "a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SSPLIT(200, str_regexpr_split, " a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SSPLIT(201, str_regexpr_split, "aa,bb.cc;dd", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SSPLIT(202, str_regexpr_split, " aa , bb . cc ; dd ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SSPLIT(203, str_regexpr_split, " aa ,bb . cc;dd: ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SSPLIT(204, str_regexpr_split, "aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SSPLIT(205, str_regexpr_split, " aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SSPLIT(206, str_regexpr_split, "abcd", "", "a", "b", "c", "d")

	TEST_RE_SPLIT(207, regexpr_split, "a,b.c;d", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SPLIT(208, regexpr_split, " a , b . c ; d ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SPLIT(209, regexpr_split, " a ,b . c;d: ", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SPLIT(210, regexpr_split, "a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SPLIT(211, regexpr_split, " a,b.c;d:", "[ ,.;:]+", "a", "b", "c", "d")
	TEST_RE_SPLIT(212, regexpr_split, "aa,bb.cc;dd", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SPLIT(213, regexpr_split, " aa , bb . cc ; dd ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SPLIT(214, regexpr_split, " aa ,bb . cc;dd: ", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SPLIT(215, regexpr_split, "aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SPLIT(216, regexpr_split, " aa,bb.cc;dd:", "[ ,.;:]+", "aa", "bb", "cc", "dd")
	TEST_RE_SPLIT(217, regexpr_split, "abcd", "", "a", "b", "c", "d")

#endif

	/* Test join */

#define TEST_JOIN(i, action, str, len, value) \
	TEST_STR(i, action, str, len, value) \
	str_destroy(&str);

	TEST_ACT (218, list = list_make((list_release_t *)str_release, str_create("aaa"), str_create("bbb"), str_create("ccc"), str_create("ddd"), str_create("eee"), str_create("fff"), NULL))
	TEST_JOIN(219, a = str_join(list, NULL), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(220, a = str_join(list, ""), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(221, a = str_join(list, " "), a, 23, "aaa bbb ccc ddd eee fff")
	TEST_JOIN(222, a = str_join(list, ", "), a, 28, "aaa, bbb, ccc, ddd, eee, fff")
	list_destroy(&list);

	TEST_ACT (223, list = list_make((list_release_t *)str_release, str_create("aaa"), str_create("ccc"), str_create("eee"), NULL))
	TEST_ACT (224, list_insert(list, 0, NULL))
	TEST_ACT (225, list_insert(list, 2, NULL))
	TEST_ACT (226, list_insert(list, 5, NULL))
	TEST_JOIN(227, a = str_join(list, NULL), a, 9, "aaaccceee")
	TEST_JOIN(228, a = str_join(list, ""), a, 9, "aaaccceee")
	TEST_JOIN(229, a = str_join(list, " "), a, 14, " aaa  ccc eee ")
	TEST_JOIN(230, a = str_join(list, ", "), a, 19, ", aaa, , ccc, eee, ")
	list_destroy(&list);

	TEST_ACT (231, list = list_make(NULL, "aaa", "bbb", "ccc", "ddd", "eee", "fff", NULL))
	TEST_JOIN(232, a = join(list, NULL), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(233, a = join(list, ""), a, 18, "aaabbbcccdddeeefff")
	TEST_JOIN(234, a = join(list, " "), a, 23, "aaa bbb ccc ddd eee fff")
	TEST_JOIN(235, a = join(list, ", "), a, 28, "aaa, bbb, ccc, ddd, eee, fff")
	list_destroy(&list);

	TEST_ACT (236, list = list_make(NULL, "aaa", "ccc", "eee", NULL))
	TEST_ACT (237, list_insert(list, 0, NULL))
	TEST_ACT (238, list_insert(list, 2, NULL))
	TEST_ACT (239, list_insert(list, 5, NULL))
	TEST_JOIN(240, a = join(list, NULL), a, 9, "aaaccceee")
	TEST_JOIN(241, a = join(list, ""), a, 9, "aaaccceee")
	TEST_JOIN(242, a = join(list, " "), a, 14, " aaa  ccc eee ")
	TEST_JOIN(243, a = join(list, ", "), a, 19, ", aaa, , ccc, eee, ")
	list_destroy(&list);

	/* Test trim */

#define TEST_SFUNC(i, func, str, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_ACT((i), func(a)) \
	CHECK_STR((i), func(str), a, len, val) \
	str_destroy(&a);

#define TEST_FUNC(i, func, str, len, val) \
	strcpy(tst, (str)); \
	TEST_ACT((i), func(tst)) \
	CHECK_CSTR((i), func(str), tst, len, val)

	TEST_SFUNC(244, str_trim, "", 0, "")
	TEST_SFUNC(245, str_trim, " ", 0, "")
	TEST_SFUNC(246, str_trim, "  ", 0, "")
	TEST_SFUNC(247, str_trim, " \t ", 0, "")
	TEST_SFUNC(248, str_trim, "\r \t \n", 0, "")
	TEST_SFUNC(249, str_trim, "abcdef", 6, "abcdef")
	TEST_SFUNC(250, str_trim, " abcdef", 6, "abcdef")
	TEST_SFUNC(251, str_trim, "abcdef ", 6, "abcdef")
	TEST_SFUNC(252, str_trim, " abcdef ", 6, "abcdef")
	TEST_SFUNC(253, str_trim, "  abcdef  ", 6, "abcdef")
	TEST_SFUNC(254, str_trim, "  abc def  ", 7, "abc def")
	TEST_SFUNC(255, str_trim, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_SFUNC(256, str_trim, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_FUNC(257, trim, "", 0, "")
	TEST_FUNC(258, trim, " ", 0, "")
	TEST_FUNC(259, trim, "  ", 0, "")
	TEST_FUNC(260, trim, " \t ", 0, "")
	TEST_FUNC(261, trim, "\r \t \n", 0, "")
	TEST_FUNC(262, trim, "abcdef", 6, "abcdef")
	TEST_FUNC(263, trim, " abcdef", 6, "abcdef")
	TEST_FUNC(264, trim, "abcdef ", 6, "abcdef")
	TEST_FUNC(265, trim, " abcdef ", 6, "abcdef")
	TEST_FUNC(266, trim, "  abcdef  ", 6, "abcdef")
	TEST_FUNC(267, trim, "  abc def  ", 7, "abc def")
	TEST_FUNC(268, trim, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_FUNC(269, trim, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_SFUNC(270, str_trim_left, "", 0, "")
	TEST_SFUNC(271, str_trim_left, " ", 0, "")
	TEST_SFUNC(272, str_trim_left, "  ", 0, "")
	TEST_SFUNC(273, str_trim_left, " \t ", 0, "")
	TEST_SFUNC(274, str_trim_left, "\r \t \n", 0, "")
	TEST_SFUNC(275, str_trim_left, "abcdef", 6, "abcdef")
	TEST_SFUNC(276, str_trim_left, " abcdef", 6, "abcdef")
	TEST_SFUNC(277, str_trim_left, "abcdef ", 7, "abcdef ")
	TEST_SFUNC(278, str_trim_left, " abcdef ", 7, "abcdef ")
	TEST_SFUNC(279, str_trim_left, "  abcdef  ", 8, "abcdef  ")
	TEST_SFUNC(280, str_trim_left, "  abc def  ", 9, "abc def  ")
	TEST_SFUNC(281, str_trim_left, "abc def\v\t\f\n\r", 12, "abc def\v\t\f\n\r")
	TEST_SFUNC(282, str_trim_left, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_FUNC(283, trim_left, "", 0, "")
	TEST_FUNC(284, trim_left, " ", 0, "")
	TEST_FUNC(285, trim_left, "  ", 0, "")
	TEST_FUNC(286, trim_left, " \t ", 0, "")
	TEST_FUNC(287, trim_left, "\r \t \n", 0, "")
	TEST_FUNC(288, trim_left, "abcdef", 6, "abcdef")
	TEST_FUNC(289, trim_left, " abcdef", 6, "abcdef")
	TEST_FUNC(290, trim_left, "abcdef ", 7, "abcdef ")
	TEST_FUNC(291, trim_left, " abcdef ", 7, "abcdef ")
	TEST_FUNC(292, trim_left, "  abcdef  ", 8, "abcdef  ")
	TEST_FUNC(293, trim_left, "  abc def  ", 9, "abc def  ")
	TEST_FUNC(294, trim_left, "abc def\v\t\f\n\r", 12, "abc def\v\t\f\n\r")
	TEST_FUNC(295, trim_left, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_SFUNC(296, str_trim_right, "", 0, "")
	TEST_SFUNC(297, str_trim_right, " ", 0, "")
	TEST_SFUNC(298, str_trim_right, "  ", 0, "")
	TEST_SFUNC(299, str_trim_right, " \t ", 0, "")
	TEST_SFUNC(300, str_trim_right, "\r \t \n", 0, "")
	TEST_SFUNC(301, str_trim_right, "abcdef", 6, "abcdef")
	TEST_SFUNC(302, str_trim_right, " abcdef", 7, " abcdef")
	TEST_SFUNC(303, str_trim_right, "abcdef ", 6, "abcdef")
	TEST_SFUNC(304, str_trim_right, " abcdef ", 7, " abcdef")
	TEST_SFUNC(305, str_trim_right, "  abcdef  ", 8, "  abcdef")
	TEST_SFUNC(306, str_trim_right, "  abc def  ", 9, "  abc def")
	TEST_SFUNC(307, str_trim_right, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_SFUNC(308, str_trim_right, "\v\t\f\n\rabc def", 12, "\v\t\f\n\rabc def")

	TEST_FUNC(309, trim_right, "", 0, "")
	TEST_FUNC(310, trim_right, " ", 0, "")
	TEST_FUNC(311, trim_right, "  ", 0, "")
	TEST_FUNC(312, trim_right, " \t ", 0, "")
	TEST_FUNC(313, trim_right, "\r \t \n", 0, "")
	TEST_FUNC(314, trim_right, "abcdef", 6, "abcdef")
	TEST_FUNC(315, trim_right, " abcdef", 7, " abcdef")
	TEST_FUNC(316, trim_right, "abcdef ", 6, "abcdef")
	TEST_FUNC(317, trim_right, " abcdef ", 7, " abcdef")
	TEST_FUNC(318, trim_right, "  abcdef  ", 8, "  abcdef")
	TEST_FUNC(319, trim_right, "  abc def  ", 9, "  abc def")
	TEST_FUNC(320, trim_right, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_FUNC(321, trim_right, "\v\t\f\n\rabc def", 12, "\v\t\f\n\rabc def")

	/* Test squeeze */

	TEST_SFUNC(322, str_squeeze, "", 0, "")
	TEST_SFUNC(323, str_squeeze, " ", 0, "")
	TEST_SFUNC(324, str_squeeze, "  ", 0, "")
	TEST_SFUNC(325, str_squeeze, " \t ", 0, "")
	TEST_SFUNC(326, str_squeeze, "\r \t \n", 0, "")
	TEST_SFUNC(327, str_squeeze, "abcdef", 6, "abcdef")
	TEST_SFUNC(328, str_squeeze, " ab  cd  ef", 8, "ab cd ef")
	TEST_SFUNC(329, str_squeeze, "ab  cd  ef ", 8, "ab cd ef")
	TEST_SFUNC(330, str_squeeze, "   ab   cd   ef   ", 8, "ab cd ef")
	TEST_SFUNC(331, str_squeeze, "  abcdef  ", 6, "abcdef")
	TEST_SFUNC(332, str_squeeze, "  abc  def  ", 7, "abc def")
	TEST_SFUNC(333, str_squeeze, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_SFUNC(334, str_squeeze, "\v\t\f\n\rabc def", 7, "abc def")

	TEST_FUNC(335, squeeze, "", 0, "")
	TEST_FUNC(336, squeeze, " ", 0, "")
	TEST_FUNC(337, squeeze, "  ", 0, "")
	TEST_FUNC(338, squeeze, " \t ", 0, "")
	TEST_FUNC(339, squeeze, "\r \t \n", 0, "")
	TEST_FUNC(340, squeeze, "abcdef", 6, "abcdef")
	TEST_FUNC(341, squeeze, " ab  cd  ef", 8, "ab cd ef")
	TEST_FUNC(342, squeeze, "ab  cd  ef ", 8, "ab cd ef")
	TEST_FUNC(343, squeeze, "   ab   cd   ef   ", 8, "ab cd ef")
	TEST_FUNC(344, squeeze, "  abcdef  ", 6, "abcdef")
	TEST_FUNC(345, squeeze, "  abc  def  ", 7, "abc def")
	TEST_FUNC(346, squeeze, "abc def\v\t\f\n\r", 7, "abc def")
	TEST_FUNC(347, squeeze, "\v\t\f\n\rabc def", 7, "abc def")

	/* Test quote, unquote */

#define TEST_SQUOTE(i, str, quotable, quote_char, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_ACT((i), b = str_quote(a, quotable, quote_char)) \
	CHECK_STR((i), str_quote(str, quotable, quote_char), b, len, val) \
	TEST_ACT((i), c = str_unquote(b, quotable, quote_char)) \
	CHECK_STR((i), str_unquote(quoted(str), quotable, quote_char), c, strlen(str), str) \
	str_destroy(&c); \
	str_destroy(&b); \
	str_destroy(&a);

#define TEST_QUOTE(i, str, quotable, quote_char, len, val) \
	TEST_ACT((i), a = quote(str, quotable, quote_char)) \
	CHECK_STR((i), quote(str, quotable, quote_char), a, len, val) \
	TEST_ACT((i), b = unquote(cstr(a), quotable, quote_char)) \
	CHECK_STR((i), unquote(quoted(str), quotable, quote_char), b, strlen(str), str) \
	str_destroy(&b); \
	str_destroy(&a);

	TEST_SQUOTE(348, "", "\"", '\\', 0, "")
	TEST_QUOTE (349, "", "\"", '\\', 0, "")
	TEST_SQUOTE(350, "\"hello world\"", "\"", '\\', 15, "\\\"hello world\\\"")
	TEST_QUOTE (351, "\"hello world\"", "\"", '\\', 15, "\\\"hello world\\\"")
	TEST_QUOTE (352, "\"hello world\\\"", "\"\\", '\\', 17, "\\\"hello world\\\\\\\"")
	TEST_SQUOTE(353, "\"hello world\\\"", "\"\\", '\\', 17, "\\\"hello world\\\\\\\"")
	TEST_QUOTE (354, "\\\"hello\\\\world\\\"", "\"\\", '\\', 22, "\\\\\\\"hello\\\\\\\\world\\\\\\\"")
	TEST_SQUOTE(355, "\\\"hello\\\\world\\\"", "\"\\", '\\', 22, "\\\\\\\"hello\\\\\\\\world\\\\\\\"")
	TEST_SQUOTE(356, "\"hello\\ \\world\"", "\"\\", '\\', 19, "\\\"hello\\\\ \\\\world\\\"")
	TEST_QUOTE (357, "\"hello\\ \\world\"", "\"\\", '\\', 19, "\\\"hello\\\\ \\\\world\\\"")
	TEST_SQUOTE(358, "\"hello\\ \\world\"", "\"", '\\', 17, "\\\"hello\\ \\world\\\"")
	TEST_QUOTE (359, "\"hello\\ \\world\"", "\"", '\\', 17, "\\\"hello\\ \\world\\\"")

	/* Test encode, decode */

#define TEST_SENCODE(i, str, uncoded, coded, quote_char, printable, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_ACT((i), b = str_encode(a, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_encode(str, uncoded, coded, quote_char, printable), b, len, val) \
	TEST_ACT((i), c = str_decode(b, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_decode(encoded(str), uncoded, coded, quote_char, printabtle), c, strlen(str), str) \
	str_destroy(&c); \
	str_destroy(&b); \
	str_destroy(&a);

#define TEST_ENCODE(i, str, uncoded, coded, quote_char, printable, len, val) \
	TEST_ACT((i), a = encode(str, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), encode(str, uncoded, coded, quote_char, printable), a, len, val) \
	TEST_ACT((i), b = decode(cstr(a), uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), decode(encoded(str), uncoded, coded, quote_char, printable), b, strlen(str), str) \
	str_destroy(&b); \
	str_destroy(&a);

	TEST_SENCODE(360, "", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 0, "")
	TEST_SENCODE(361, "\a\b\t\n\v\f\r\033\\", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 20, "\\a\\b\\t\\n\\v\\f\\r\\x1b\\\\")
	TEST_SENCODE(362, "a\a\b\tb\n\v\f\rc\033\\d", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 24, "a\\a\\b\\tb\\n\\v\\f\\rc\\x1b\\\\d")
	TEST_SENCODE(363, "", "=", "=", '\\', 0, 0, "")
	TEST_SENCODE(364, "a=b", "=", "=", '\\', 0, 4, "a\\=b")

	TEST_ENCODE(365, "", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 0, "")
	TEST_ENCODE(366, "\a\b\t\n\v\f\r\033\\", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 20, "\\a\\b\\t\\n\\v\\f\\r\\x1b\\\\")
	TEST_ENCODE(367, "a\a\b\tb\n\v\f\rc\033\\d", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 24, "a\\a\\b\\tb\\n\\v\\f\\rc\\x1b\\\\d")
	TEST_ENCODE(368, "", "=", "=", '\\', 0, 0, "")
	TEST_ENCODE(369, "a=b", "=", "=", '\\', 0, 4, "a\\=b")

	/* Test lc, lcfirst */

	TEST_SFUNC(370, str_lc, "", 0, "")
	TEST_SFUNC(371, str_lc, "abc", 3, "abc")
	TEST_SFUNC(372, str_lc, " a b c ", 7, " a b c ")
	TEST_SFUNC(373, str_lc, "ABC", 3, "abc")
	TEST_SFUNC(374, str_lc, " A B C ", 7, " a b c ")
	TEST_SFUNC(375, str_lc, "0123456", 7, "0123456")

	TEST_FUNC(376, lc, "", 0, "")
	TEST_FUNC(377, lc, "abc", 3, "abc")
	TEST_FUNC(378, lc, " a b c ", 7, " a b c ")
	TEST_FUNC(379, lc, "ABC", 3, "abc")
	TEST_FUNC(380, lc, " A B C ", 7, " a b c ")
	TEST_FUNC(381, lc, "0123456", 7, "0123456")

	TEST_SFUNC(382, str_lcfirst, "", 0, "")
	TEST_SFUNC(383, str_lcfirst, "abc", 3, "abc")
	TEST_SFUNC(384, str_lcfirst, " a b c ", 7, " a b c ")
	TEST_SFUNC(385, str_lcfirst, "ABC", 3, "aBC")
	TEST_SFUNC(386, str_lcfirst, " A B C ", 7, " A B C ")
	TEST_SFUNC(387, str_lcfirst, "0123456", 7, "0123456")

	TEST_FUNC(388, lcfirst, "", 0, "")
	TEST_FUNC(389, lcfirst, "abc", 3, "abc")
	TEST_FUNC(390, lcfirst, " a b c ", 7, " a b c ")
	TEST_FUNC(391, lcfirst, "ABC", 3, "aBC")
	TEST_FUNC(392, lcfirst, " A B C ", 7, " A B C ")
	TEST_FUNC(393, lcfirst, "0123456", 7, "0123456")

	/* Test uc, ucfirst */

	TEST_SFUNC(394, str_uc, "", 0, "")
	TEST_SFUNC(395, str_uc, "abc", 3, "ABC")
	TEST_SFUNC(396, str_uc, " a b c ", 7, " A B C ")
	TEST_SFUNC(397, str_uc, "ABC", 3, "ABC")
	TEST_SFUNC(398, str_uc, " A B C ", 7, " A B C ")
	TEST_SFUNC(399, str_uc, "0123456", 7, "0123456")

	TEST_FUNC(400, uc, "", 0, "")
	TEST_FUNC(401, uc, "abc", 3, "ABC")
	TEST_FUNC(402, uc, " a b c ", 7, " A B C ")
	TEST_FUNC(403, uc, "ABC", 3, "ABC")
	TEST_FUNC(404, uc, " A B C ", 7, " A B C ")
	TEST_FUNC(405, uc, "0123456", 7, "0123456")

	TEST_SFUNC(406, str_ucfirst, "", 0, "")
	TEST_SFUNC(407, str_ucfirst, "abc", 3, "Abc")
	TEST_SFUNC(408, str_ucfirst, " a b c ", 7, " a b c ")
	TEST_SFUNC(409, str_ucfirst, "ABC", 3, "ABC")
	TEST_SFUNC(410, str_ucfirst, " A B C ", 7, " A B C ")
	TEST_SFUNC(411, str_ucfirst, "0123456", 7, "0123456")

	TEST_FUNC(412, ucfirst, "", 0, "")
	TEST_FUNC(413, ucfirst, "abc", 3, "Abc")
	TEST_FUNC(414, ucfirst, " a b c ", 7, " a b c ")
	TEST_FUNC(415, ucfirst, "ABC", 3, "ABC")
	TEST_FUNC(416, ucfirst, " A B C ", 7, " A B C ")
	TEST_FUNC(417, ucfirst, "0123456", 7, "0123456")

	/* Test chop */

#define TEST_SFUNC_EQ(i, func, str, eq, len, val) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_EQ((i), func(a), eq) \
	CHECK_STR((i), func(str), a, len, val) \
	str_destroy(&a);

#define TEST_FUNC_EQ(i, func, str, eq, len, val) \
	strcpy(tst, (str)); \
	TEST_EQ((i), func(tst), eq) \
	CHECK_CSTR((i), func(str), tst, len, val)

	TEST_SFUNC_EQ(418, str_chop, "abcdef", 'f', 5, "abcde")
	TEST_SFUNC_EQ(419, str_chop, "abcde", 'e',4, "abcd")
	TEST_SFUNC_EQ(420, str_chop, "abcd", 'd', 3, "abc")
	TEST_SFUNC_EQ(421, str_chop, "abc", 'c', 2, "ab")
	TEST_SFUNC_EQ(422, str_chop, "ab", 'b', 1, "a")
	TEST_SFUNC_EQ(423, str_chop, "a", 'a', 0, "")
	TEST_SFUNC_EQ(424, str_chop, "", -1, 0, "")

	TEST_FUNC_EQ(425, chop, "abcdef", 'f', 5, "abcde")
	TEST_FUNC_EQ(426, chop, "abcde", 'e', 4, "abcd")
	TEST_FUNC_EQ(427, chop, "abcd", 'd', 3, "abc")
	TEST_FUNC_EQ(428, chop, "abc", 'c', 2, "ab")
	TEST_FUNC_EQ(429, chop, "ab", 'b', 1, "a")
	TEST_FUNC_EQ(430, chop, "a", 'a', 0, "")
	TEST_FUNC_EQ(431, chop, "", -1, 0, "")

	/* Test chomp */

	TEST_SFUNC_EQ(432, str_chomp, "abcdef", 0, 6, "abcdef")
	TEST_SFUNC_EQ(433, str_chomp, "abcdef ", 0, 7, "abcdef ")
	TEST_SFUNC_EQ(434, str_chomp, "abcdef \n", 1, 7, "abcdef ")
	TEST_SFUNC_EQ(435, str_chomp, "abcdef \n\r", 2, 7, "abcdef ")

	TEST_FUNC_EQ(436, chomp, "abcdef", 0, 6, "abcdef")
	TEST_FUNC_EQ(437, chomp, "abcdef ", 0, 7, "abcdef ")
	TEST_FUNC_EQ(438, chomp, "abcdef \n", 1, 7, "abcdef ")
	TEST_FUNC_EQ(439, chomp, "abcdef \n\r", 2, 7, "abcdef ")

	/* Test bin, hex, oct */

#define TEST_SNUM(i, func, str, eq) \
	TEST_ACT((i), a = str_create("%s", str)) \
	TEST_EQ((i), func(a), eq) \
	str_destroy(&a);

#define TEST_NUM(i, func, str, eq) \
	strcpy(tst, str); \
	TEST_EQ((i), func(tst), eq)

	TEST_SNUM(440, str_bin, "010", 2)
	TEST_SNUM(441, str_oct, "010", 8)
	TEST_SNUM(442, str_hex, "010", 16)

	TEST_NUM(443, bin, "010", 2)
	TEST_NUM(444, oct, "010", 8)
	TEST_NUM(445, hex, "010", 16)

	TEST_SNUM(446, str_bin, "11111111111111111111111111111111", -1)
	TEST_SNUM(447, str_oct, "037777777777", -1)
	TEST_SNUM(448, str_hex, "ffffffff", -1)

	TEST_NUM(449, bin, "11111111111111111111111111111111", -1)
	TEST_NUM(450, oct, "037777777777", -1)
	TEST_NUM(451, hex, "ffffffff", -1)

	TEST_SNUM(452, str_bin, "0b1010", 10)
	TEST_SNUM(453, str_hex, "0xa", 10)
	TEST_SNUM(454, str_oct, "012", 10)
	TEST_SNUM(455, str_oct, "0b1010", 10)
	TEST_SNUM(456, str_oct, "0xa", 10)

	TEST_NUM(457, bin, "0b1010", 10)
	TEST_NUM(458, hex, "0xa", 10)
	TEST_NUM(459, oct, "012", 10)
	TEST_NUM(460, oct, "0b1010", 10)
	TEST_NUM(461, oct, "0xa", 10)

	/* Test cstr, str_set_length and str_recalc_length */

#define TEST_SET(i, action, str, eq, len, val) \
	TEST_EQ((i), action, eq) \
	CHECK_STR((i), action, str, eq, val)

	TEST_ACT(462, a = str_create("0123456789"))
	TEST_SET(463, (cstr(a)[5] = '\0', str_recalc_length(a)), a, 5, 5, "01234")
	TEST_SET(464, (cstr(a)[5] =  '5', str_recalc_length(a)), a, 10, 10, "0123456789")
	TEST_SET(465, (cstr(a)[5] = '\0', str_set_length(a, 8)), a, 8, 8, "01234\00067")
	TEST_SET(466, (cstr(a)[5] =  '5', str_set_length(a, 7)), a, 7, 7, "0123456\000")
	TEST_SET(467, (cstr(a)[5] = '\0', str_set_length(a, 6)), a, 6, 6, "01234\000")
	str_destroy(&a);

	/* Test strcasecmp() and strncasecmp() */

#ifdef NEEDS_STRCASECMP
	TEST_ACT(468, strcasecmp("Abc", "abc") == 0)
	TEST_ACT(469, strcasecmp("AbcD", "abc") == 100)
	TEST_ACT(470, strcasecmp("Abc", "abcD") == -100)
	TEST_ACT(471, strcasecmp("Abce", "abcD") == 1)
	TEST_ACT(472, strcasecmp("AbcD", "abce") == -1)
#endif

#ifdef NEEDS_STRNCASECMP
	TEST_ACT(473, strncasecmp("Abc", "abc", 2) == 0)
	TEST_ACT(474, strncasecmp("AbC", "abc", 3) == 0)
	TEST_ACT(475, strncasecmp("Abc", "abC", 4) == 0)
	TEST_ACT(476, strncasecmp("AbcD", "abc", 2) == 0)
	TEST_ACT(477, strncasecmp("Abcd", "abc", 3) == 0)
	TEST_ACT(478, strncasecmp("AbcD", "abc", 4) == 100)
	TEST_ACT(479, strncasecmp("Abcd", "abc", 4) == 100)
	TEST_ACT(480, strncasecmp("Abc", "abcd", 2) == 0)
	TEST_ACT(481, strncasecmp("Abc", "abcD", 3) == 0)
	TEST_ACT(482, strncasecmp("Abc", "abcd", 4) == -100)
	TEST_ACT(483, strncasecmp("Abc", "abcD", 5) == -100)
	TEST_ACT(484, strncasecmp("Abce", "abcd", 3) == 0)
	TEST_ACT(485, strncasecmp("Abce", "abcD", 4) == 1)
	TEST_ACT(486, strncasecmp("AbcE", "abcd", 5) == 1)
	TEST_ACT(487, strncasecmp("Abcd", "abce", 3) == 0)
	TEST_ACT(488, strncasecmp("AbcD", "abce", 4) == -1)
	TEST_ACT(489, strncasecmp("Abcd", "abcE", 5) == -1)
#endif

	/* Test strlcpy() and strlcat() */

#ifdef NEEDS_STRLCPY
	memset(tst, 0xff, 4);
	TEST_ACT(490, strlcpy(tst, "", 3) == 0)
	TEST_ACT(491, !strcmp(tst, ""))
	memset(tst, 0xff, 4);
	TEST_ACT(492, strlcpy(tst, "a", 3) == 1)
	TEST_ACT(493, !strcmp(tst, "a"))
	memset(tst, 0xff, 4);
	TEST_ACT(494, strlcpy(tst, "ab", 3) == 2)
	TEST_ACT(495, !strcmp(tst, "ab"))
	memset(tst, 0xff, 4);
	TEST_ACT(496, strlcpy(tst, "abc", 3) == 3)
	TEST_ACT(497, !strcmp(tst, "ab"))
	memset(tst, 0xff, 4);
	TEST_ACT(498, strlcpy(tst, "abcd", 3) == 4)
	TEST_ACT(499, !strcmp(tst, "ab"))
#endif

#ifdef NEEDS_STRLCAT
	tst[0] = '\0';
	TEST_ACT(500, strlcat(tst, "a", 3) == 1)
	TEST_ACT(501, !strcmp(tst, "a"))
	TEST_ACT(502, strlcat(tst, "b", 3) == 2)
	TEST_ACT(503, !strcmp(tst, "ab"))
	TEST_ACT(504, strlcat(tst, "c", 3) == 3)
	TEST_ACT(505, !strcmp(tst, "ab"))
	TEST_ACT(506, strlcat(tst, "d", 3) == 3)
	TEST_ACT(507, !strcmp(tst, "ab"))
#endif

	/* Test strings containing nuls and high bit characters */

#define TEST_ZFUNC(i, func, origlen, str, newlen, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_ACT((i), func(a)) \
	CHECK_STR((i), func(str), a, newlen, val) \
	str_destroy(&a);

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
	str_destroy(&a); \
	list_destroy(&list);

#define TEST_ZQUOTE(i, origlen, str, quotable, quote_char, len, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_ACT((i), b = str_quote(a, quotable, quote_char)) \
	CHECK_STR((i), str_quote(str, quotable, quote_char), b, len, val) \
	TEST_ACT((i), c = str_unquote(b, quotable, quote_char)) \
	CHECK_STR((i), str_unquote(quoted(str), quotable, quote_char), c, origlen, str) \
	str_destroy(&c); \
	str_destroy(&b); \
	str_destroy(&a);

#define TEST_ZENCODE(i, origlen, str, uncoded, coded, quote_char, printable, len, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_ACT((i), b = str_encode(a, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_encode(str, uncoded, coded, quote_char, printable), b, len, val) \
	TEST_ACT((i), c = str_decode(b, uncoded, coded, quote_char, printable)) \
	CHECK_STR((i), str_decode(encoded(str), uncoded, coded, quote_char, printable), c, origlen, str) \
	str_destroy(&c); \
	str_destroy(&b); \
	str_destroy(&a);

#define TEST_ZCHOMP(i, func, origlen, str, eq, newlen, val) \
	TEST_ACT((i), a = substr(str, 0, origlen)) \
	TEST_EQ((i), func(a), eq) \
	CHECK_STR((i), func(str), a, newlen, val) \
	str_destroy(&a);

	TEST_STR(508, a = str_create("%c", '\0'), a, 1, "\000")
	TEST_ACT(509, !str_empty(a))
	TEST_STR(510, b = str_create("abc%cdef\376", '\0'), b, 8, "abc\000def\376")
	TEST_STR(511, c = str_copy(b), c, 8, "abc\000def\376")
	TEST_STR(512, str_remove(b, 0), b, 7, "bc\000def\376")
	TEST_STR(513, str_remove_range(c, 1, 2), c, 6, "a\000def\376")
	TEST_STR(514, str_insert(a, 1, "a%c\376b", '\0'), a, 5, "\000a\000\376b")
	TEST_STR(515, str_insert_str(a, 1, b), a, 12, "\000bc\000def\376a\000\376b")
	TEST_STR(516, str_prepend(a, "a%c\376b", '\0'), a, 16, "a\000\376b\000bc\000def\376a\000\376b")
	TEST_STR(517, str_prepend_str(a, b), a, 23, "bc\000def\376a\000\376b\000bc\000def\376a\000\376b")
	TEST_STR(518, str_append(a, "%c\376", '\0'), a, 25, "bc\000def\376a\000\376b\000bc\000def\376a\000\376b\000\376")
	TEST_STR(519, str_append_str(a, c), a, 31, "bc\000def\376a\000\376b\000bc\000def\376a\000\376b\000\376a\000def\376")
	TEST_STR(520, str_replace(a, 3, 5, "%c\376", '\0'), a, 28, "bc\000\000\376\000\376b\000bc\000def\376a\000\376b\000\376a\000def\376")
	TEST_STR(521, str_replace_str(a, 3, 5, c), a, 29, "bc\000a\000def\376\000bc\000def\376a\000\376b\000\376a\000def\376")
	str_destroy(&c);
	TEST_STR(522, c = str_repeat(10, "1%c1", '\0'), c, 30, "1\00011\00011\00011\00011\00011\00011\00011\00011\00011\0001")
	str_destroy(&c);
	TEST_STR(523, c = str_substr(a, 5, 5), c, 5, "def\376\000")
	str_destroy(&c);
	TEST_STR(524, c = substr(cstr(a), 5, 5), c, 5, "def\376\000")
	str_destroy(&c);
	TEST_STR(525, c = str_splice(a, 5, 5), c, 5, "def\376\000")
	CHECK_STR(526, str_splice(a, 5, 5), a, 24, "bc\000a\000bc\000def\376a\000\376b\000\376a\000def\376")
	str_destroy(&c);
	str_destroy(&b);
	TEST_TR_DIRECT(527, str_tr(a, "a-z", "A-EfG-Z", 0), a, 14, 24, "BC\000A\000BC\000DEf\376A\000\376B\000\376A\000DEf\376")
	TEST_ACT(528, b = str_create("%c", '\0'))
	TEST_ACT(529, c = str_create("%c", '\0'))
	TEST_TR_DIRECT(530, str_tr_str(a, b, c, 0), a, 6, 24, "BC\000A\000BC\000DEf\376A\000\376B\000\376A\000DEf\376")
	str_destroy(&c);
	str_destroy(&b);
	str_destroy(&a);
	TEST_ZSPLIT(531, 9, "\000a\376\000a\376a\000\376", "a", "\000", "\376\000", "\376", "\000\376")
	TEST_ACT(532, list = list_make((list_release_t *)str_release, str_create("%c\376", '\0'), str_create("\376%c", '\0'), str_create("%c%c", '\0', '\0'), NULL))
	TEST_JOIN(533, a = str_join(list, "a"), a, 8, "\000\376a\376\000a\000\000")
	list_destroy(&list);
	TEST_ZFUNC(534, str_trim, 12, "  abc\000\376def  ", 8, "abc\000\376def")
	TEST_ZFUNC(535, str_trim_left, 12, "  abc\000\376def  ", 10, "abc\000\376def  ")
	TEST_ZFUNC(536, str_trim_right, 12, "  abc\000\376def  ", 10, "  abc\000\376def")
	TEST_ZFUNC(537, str_squeeze, 19, "   ab \000 cd\376   ef   ", 11, "ab \000 cd\376 ef")
	TEST_ZQUOTE(538, 16, "\\\"hell\000\\\\w\376rld\\\"", "\"\\", '\\', 22, "\\\\\\\"hell\000\\\\\\\\w\376rld\\\\\\\"")
	TEST_ZENCODE(539, 11, "\a\b\t\n\0\376\v\f\r\033\\", "\a\b\t\n\v\f\r\\", "abtnvfr\\", '\\', 1, 28, "\\a\\b\\t\\n\\x00\\xfe\\v\\f\\r\\x1b\\\\")
	TEST_ZFUNC(540, str_lc, 7, "ABC\000DEF", 7, "abc\000def")
	TEST_ZFUNC(541, str_lcfirst, 7, "ABC\000DEF", 7, "aBC\000DEF")
	TEST_ZFUNC(542, str_uc, 7, "abc\000def", 7, "ABC\000DEF")
	TEST_ZFUNC(543, str_ucfirst, 7, "abc\000def", 7, "Abc\000def")
	TEST_ZFUNC(544, str_chop, 7, "abc\000def", 6, "abc\000de")
	TEST_ZCHOMP(545, str_chomp, 7, "abc\000def", 0, 7, "abc\000def")
	TEST_ZCHOMP(546, str_chomp, 7, "abc\000de\n", 1, 6, "abc\000de")

	/* Test error reporting */

	TEST_ACT(547, !str_copy(NULL))
	TEST_ACT(548, !cstr(NULL))
	TEST_EQ (549, str_set_length(NULL, 0), -1)
	TEST_ACT(550, a = str_create(NULL))
	TEST_EQ (551, str_set_length(a, 33), -1)
	TEST_EQ (552, str_set_length(a, 1), -1)
	TEST_EQ (553, str_recalc_length(NULL), -1)
	TEST_ACT(554, !str_remove_range(NULL, 0, 0))
	TEST_ACT(555, !str_remove_range(a, 1, 0))
	TEST_ACT(556, !str_remove_range(a, 0, 1))
	TEST_ACT(557, !str_insert(NULL, 0, ""))
	TEST_ACT(558, !str_insert(a, 1, ""))
	TEST_ACT(559, !str_insert_str(a, 0, NULL))
	TEST_ACT(560, !str_replace(NULL, 0, 0, ""))
	TEST_ACT(561, !str_replace(a, 0, 1, ""))
	TEST_ACT(562, !str_replace(a, 1, 0, ""))
	TEST_ACT(563, !str_replace_str(NULL, 0, 0, NULL))
	TEST_ACT(564, !str_substr(NULL, 0, 0))
	TEST_ACT(565, !str_substr(a, 0, 1))
	TEST_ACT(566, !str_substr(a, 1, 0))
	TEST_ACT(567, !substr(NULL, 0, 0))
	TEST_ACT(568, !str_splice(NULL, 0, 0))
	TEST_ACT(569, !str_splice(a, 0, 1))
	TEST_ACT(570, !str_splice(a, 1, 0))
	TEST_EQ (571, str_tr(NULL, "a-z", "A-Z", 0), -1)
	TEST_EQ (572, str_tr(a, NULL, "A-Z", 0), -1)
	TEST_EQ (573, str_tr(a, "z-a", "A-Z", 0), -1)
	TEST_EQ (574, str_tr(a, "a-z", "Z-A", 0), -1)
	TEST_ACT(575, b = str_create("a-z"))
	TEST_ACT(576, c = str_create("A-Z"))
	TEST_EQ (577, str_tr_str(NULL, b, c, 0), -1)
	TEST_EQ (578, str_tr_str(a, NULL, c, 0), -1)
	str_destroy(&c);
	TEST_ACT(579, c = str_create("Z-A"))
	TEST_EQ (580, str_tr_str(a, b, c, 0), -1)
	str_destroy(&b);
	TEST_ACT(581, b = str_create("z-a"))
	TEST_EQ (582, str_tr_str(a, b, c, 0), -1)
	str_destroy(&b);
	str_destroy(&c);
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
	TEST_ACT(593, !str_regexpr(NULL, a, 0, 0))
	TEST_ACT(594, !str_regexpr("", NULL, 0, 0))
	TEST_ACT(595, !regexpr(NULL, "", 0, 0))
	TEST_ACT(596, !regexpr("", NULL, 0, 0))
	TEST_EQ (597, regexpr_compile(re, NULL, 0), REG_BADPAT)
	TEST_EQ (598, regexpr_compile(NULL, "", 0), REG_BADPAT)
	TEST_ACT(599, !regexpr_compiled(re, NULL, 0))
	TEST_ACT(600, !regexpr_compiled(NULL, "", 0))
	str_destroy(&a);
	TEST_ACT(601, a = str_create("aaa"))
	TEST_ACT(602, !str_regsub(NULL, "", a, 0, 0, 0))
	TEST_ACT(603, !str_regsub("", NULL, a, 0, 0, 0))
	TEST_ACT(604, !str_regsub("", "", NULL, 0, 0, 0))
	TEST_ACT(605, !str_regsub_compiled(NULL, "", a, 0, 0))
	TEST_ACT(606, !str_regsub_compiled(re, NULL, a, 0, 0))
	TEST_EQ (607, regexpr_compile(re, ".+", 0), 0)
	TEST_ACT(608, !str_regsub_compiled(re, "$", NULL, 0, 0))
	TEST_ACT(609, !str_regsub_compiled(re, "$a", NULL, 0, 0))
	TEST_ACT(610, !str_regsub_compiled(re, "${0", NULL, 0, 0))
	TEST_ACT(611, !str_regsub_compiled(re, "${", NULL, 0, 0))
	TEST_ACT(612, !str_regsub_compiled(re, "${33}", NULL, 0, 0))
	TEST_ACT(613, !str_regsub_compiled(re, "${-12}", NULL, 0, 0))
	TEST_ACT(614, !str_regsub_compiled(re, "${a}", NULL, 0, 0))
	TEST_ACT(615, !str_regsub_compiled(re, "\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q\\L\\U\\Q$0\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E\\E", a, 0, 0))
	TEST_ACT(616, !str_regsub_compiled(re, "\\E", a, 0, 0))
	regexpr_release(re);
#endif
	str_destroy(&a);
	TEST_ACT(617, a = str_create("hello"))
	TEST_ACT(618, !str_fmt(NULL, 10, ALIGN_LEFT))
	TEST_ACT(619, !str_fmt(a, -1, ALIGN_LEFT))
	TEST_ACT(620, !str_fmt(a, 10, '@'))
	str_destroy(&a);
	TEST_ACT(621, !fmt(NULL, 10, ALIGN_LEFT))
	TEST_ACT(622, !fmt("hello", -1, ALIGN_LEFT))
	TEST_ACT(623, !fmt("hello", 10, '@'))
	TEST_ACT(624, !str_split(NULL, ""))
	TEST_ACT(625, !str_split(a, NULL))
	TEST_ACT(626, !split(NULL, ""))
	TEST_ACT(627, !split("", NULL))
#ifndef REGEX_MISSING
	TEST_ACT(628, !str_regexpr_split(NULL, "", 0, 0))
	TEST_ACT(629, !str_regexpr_split(a, NULL, 0, 0))
	TEST_ACT(630, !regexpr_split(NULL, "", 0, 0))
	TEST_ACT(631, !regexpr_split("", NULL, 0, 0))
#endif
	TEST_ACT(632, !str_join(NULL, " "))
	TEST_ACT(633, !join(NULL, " "))
	TEST_ACT(634, !str_trim(NULL))
	TEST_ACT(635, !trim(NULL))
	TEST_ACT(636, !str_trim_left(NULL))
	TEST_ACT(637, !trim_left(NULL))
	TEST_ACT(638, !str_trim_right(NULL))
	TEST_ACT(639, !trim_right(NULL))
	TEST_ACT(640, !str_squeeze(NULL))
	TEST_ACT(641, !squeeze(NULL))
	TEST_ACT(642, !str_quote(NULL, "", '\\'))
	TEST_ACT(643, !str_quote(a, NULL, '\\'))
	TEST_ACT(644, !quote(NULL, "", '\\'))
	TEST_ACT(645, !quote("", NULL, '\\'))
	TEST_ACT(646, !str_unquote(NULL, "", '\\'))
	TEST_ACT(647, !str_unquote(a, NULL, '\\'))
	TEST_ACT(648, !unquote(NULL, "", '\\'))
	TEST_ACT(649, !unquote("", NULL, '\\'))
	TEST_ACT(650, !str_encode(NULL, "", "", '\\', 0))
	TEST_ACT(651, !str_encode(a, NULL, "", '\\', 0))
	TEST_ACT(652, !str_encode(a, "", NULL, '\\', 0))
	TEST_ACT(653, !encode(NULL, "", "", '\\', 0))
	TEST_ACT(654, !encode("", NULL, "", '\\', 0))
	TEST_ACT(655, !encode("", "", NULL, '\\', 0))
	TEST_ACT(656, !str_decode(NULL, "", "", '\\', 0))
	TEST_ACT(657, !str_decode(a, NULL, "", '\\', 0))
	TEST_ACT(658, !str_decode(a, "", NULL, '\\', 0))
	TEST_ACT(659, !decode(NULL, "", "", '\\', 0))
	TEST_ACT(660, !decode("", NULL, "", '\\', 0))
	TEST_ACT(661, !decode("", "", NULL, '\\', 0))
	TEST_ACT(662, !str_lc(NULL))
	TEST_ACT(663, !lc(NULL))
	TEST_ACT(664, !str_lcfirst(NULL))
	TEST_ACT(665, !lcfirst(NULL))
	TEST_ACT(666, !str_uc(NULL))
	TEST_ACT(667, !uc(NULL))
	TEST_ACT(668, !str_ucfirst(NULL))
	TEST_ACT(669, !ucfirst(NULL))
	TEST_EQ (670, str_chop(NULL), -1)
	TEST_EQ (671, str_chop(a), -1)
	TEST_EQ (672, chop(NULL), -1)
	TEST_EQ (673, chop(""), -1)
	TEST_EQ (674, str_chomp(NULL), -1)
	TEST_EQ (675, chomp(NULL), -1)
	TEST_EQ (676, str_bin(NULL), 0)
	TEST_ACT(677, b = str_create("123456789!"))
	TEST_EQ (678, str_bin(b), 0)
	TEST_EQ (679, bin(NULL), 0)
	TEST_EQ (680, bin("123456789!"), 0)
	TEST_EQ (681, str_hex(NULL), 0)
	TEST_EQ (682, str_hex(b), 0)
	TEST_EQ (683, hex(NULL), 0)
	TEST_EQ (684, hex("123456789!"), 0)
	TEST_EQ (685, str_oct(NULL), 0)
	TEST_EQ (686, str_oct(b), 0)
	TEST_EQ (687, oct(NULL), 0)
	TEST_EQ (688, oct("123456789!"), 0)
	str_destroy(&b);
	str_destroy(&a);

	/* Test MT Safety */

	debug = (ac != 1);

	if (debug)
		setbuf(stdout, NULL);

	if (debug)
		locker = locker_create_debug_rwlock(&rwlock);
	else
		locker = locker_create_rwlock(&rwlock);

	if (!locker)
		++errors, printf("Test689: locker_create_rwlock() failed\n");
	else
	{
		mt_test(689, locker);
		locker_destroy(&locker);
	}

	if (debug)
		locker = locker_create_debug_mutex(&mutex);
	else
		locker = locker_create_mutex(&mutex);

	if (!locker)
		++errors, printf("Test690: locker_create_mutex() failed\n");
	else
	{
		mt_test(690, locker);
		locker_destroy(&locker);
	}

	if (errors)
		printf("%d/690 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
