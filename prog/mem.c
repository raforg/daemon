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

/*

=head1 NAME

I<libprog(mem)> - memory module

=head1 SYNOPSIS

    #include <prog/mem.h>

    #define mem_create(size, type)
    #define mem_resize(mem, size)
    #define mem_release(mem)
    #define mem_destroy(mem)

    void *mem_resize_fn(void **mem, size_t size);
    void *mem_destroy_fn(void **mem);
    char *mem_strdup(const char *str);

=head1 DESCRIPTION

This module is just an interface to I<malloc(3)>, I<realloc(3)> and
I<free(3)> that tries to ensure that pointers that don't point to anything
are C<NULL>.

=over 4

=cut

*/

#include <stdlib.h>
#include <string.h>

#include "mem.h"

/*

=item C< #define mem_create(size, type)>

Allocates enough memory (with I<malloc(3)>) to store C<size> objects of type
C<type>. The memory returned is of type C<type *>. Returns the address of
the allocated memory if successful, C<NULL> otherwise.

=item C< #define mem_resize(mem, num)>

Alters the amount of memory pointed to by C<mem>. If C<mem> is C<NULL>,
I<malloc(3)> is used to allocate new memory. If size is zero,
I<free(3)> is called to deallocate the memory and C<mem> is set to C<NULL>.
Otherwise, I<realloc(3)> is called. If I<realloc()> needs to allocate new
memory to satisfy a request, C<mem> is set to the new address. Returns
I<mem> on success (though it may be C<NULL> if C<size> is zero). On error,
C<NULL> is returned and I<mem> is not altered.

=item C< #define mem_release(mem)>

Releases (deallocates) C<mem>. Same as I<free(3)>. Only to be used in
destructor functions. In other cases, use I<mem_destroy()> which also sets
C<mem> to C<NULL>.

=item C< #define mem_destroy(mem)>

Destroys (deallocates and sets to C<NULL>) the memory pointed to by C<mem>.

=cut

*/

/*

=item C<void *mem_resize_fn(void **mem, size_t size)>

A single interface for altering the size of allocated memory. C<mem> points
to the pointer to be affected. C<size> is the size in bytes of the memory
that this pointer is to point to. If the pointer is C<NULL>, I<malloc(3)> is
used to obtain memory. If C<size> is zero, I<free(3)> is used to release the
memory. In all other cases, I<realloc(3)> is used to alter the size of the
memory. In all cases, the pointer pointed to by C<mem> is assigned to the
memory's location (or C<NULL> when C<size> is zero). This function is
exposed as an implementation side effect. Don't call it directly. Call
I<mem_resize()> instead. If an error occurs (I<malloc(3)> or I<realloc(3)>
fail or C<mem> is C<NULL>), return C<NULL> without setting C<*mem> to
anything.

=cut

*/

void *mem_resize_fn(void **mem, size_t size)
{
	void *p;

	if (!mem)
		return NULL;

	if (size)
	{
		if (*mem)
		{
			p = realloc(*mem, size);
			if (p)
				return *mem = p;
		}
		else
		{
			p = malloc(size);
			if (p)
				return *mem = p;
		}
	}
	else
	{
		free(*mem);
		return *mem = NULL;
	}

	return NULL;
}

/*

=item C<void *mem_destroy_fn(void **mem)>

Calls I<free(3)> on the pointer pointed to by C<mem>. Then assigns C<NULL>
to this pointer. Returns C<NULL>. This function is exposed as an
implementation side effect. Don't call it directly. Call I<mem_destroy()>
instead.

=cut

*/

void *mem_destroy_fn(void **mem)
{
	if (mem && *mem)
	{
		free(*mem);
		*mem = NULL;
	}

	return NULL;
}

/*

=item C<char *mem_strdup(const char *str)>

Returns a copy of C<str> on success, or C<NULL> on error. The caller must
deallocate the memory returned.

=cut

*/

char *mem_strdup(const char *str)
{
	char *s;

	if (!str || !(s = mem_create(strlen(str) + 1, char)))
		return NULL;

	return strcpy(s, str);
}

/*

=back

=head1 SEE ALSO

L<malloc(3)|malloc(3)>,
L<realloc(3)|realloc(3)>,
L<free(3)|free(3)>,
L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<msg(3)|msg(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>

=head1 AUTHOR

raf <raf2@zip.com.au>

=cut

*/

#ifdef TEST

#include <stdio.h>

int main(int ac, char **av)
{
	int *mem1 = NULL;
	char *mem2 = NULL;
	int errors = 0;

	printf("Testing: mem\n");

	mem1 = mem_create(100, int);
	if (!mem1)
		++errors, printf("Test1: mem_create(100, int) failed\n");

	mem_resize(mem1, 50);
	if (!mem1)
		++errors, printf("Test2: mem_resize(100 -> 50) failed\n");

	mem_resize(mem1, 0);
	if (mem1)
		++errors, printf("Test3: mem_resize(50 -> 0) failed\n");

	mem_resize(mem1, 50);
	if (!mem1)
		++errors, printf("Test4: mem_resize(0 -> 50) failed\n");

	mem_destroy(mem1);
	if (mem1)
		++errors, printf("Test5: mem_destroy() failed\n");

	mem2 = mem_create(0, char);
	if (!mem2)
		++errors, printf("Test6: mem_create(0, char) failed\n");

	mem_resize(mem2, 10);
	if (!mem2)
		++errors, printf("Test7: mem_resize(0 -> 10) failed\n");

	mem_resize(mem2, 0);
	if (mem2)
		++errors, printf("Test8: mem_resize(10 -> 0) failed\n");

	mem_destroy(mem2);
	if (mem2)
		++errors, printf("Test9: mem_destroy() failed\n");

	if (errors)
		printf("%d/9 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
