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

I<libslack(mem)> - memory module

=head1 SYNOPSIS

    #include <slack/mem.h>

    #define mem_new(type)
    #define mem_create(size, type)
    #define mem_resize(mem, size)
    void *mem_resize_fn(void **mem, size_t size);
    #define mem_release(mem)
    #define mem_destroy(mem)
    void *mem_destroy_fn(void **mem);
    char *mem_strdup(const char *str);
    #define mem_create2d(type, x, y)
    #define mem_create3d(type, x, y, z)
    #define mem_create4d(type, x, y, z, a)
    void *mem_create_space(size_t size, ...);
    size_t mem_space_start(size_t size, ...);
    #define mem_release2d(space)
    #define mem_release3d(space)
    #define mem_release4d(space)
    #define mem_release_space(space)
    #define mem_destroy2d(space)
    #define mem_destroy3d(space)
    #define mem_destroy4d(space)
    #define mem_destroy_space(space)

=head1 DESCRIPTION

This module is mostly just an interface to I<malloc(3)>, I<realloc(3)> and
I<free(3)> that tries to ensure that pointers that don't point to anything
are C<NULL>. It also provides dynamically allocated multi-dimensional arrays.

=over 4

=cut

*/

#include "std.h"

#include "mem.h"

/*

=item C< #define mem_new(type)>

Allocates enough memory (with I<malloc(3)>) to store an object of type
C<type>. The memory returned is of type C<type *>. On success, returns the
address of the allocated memory. On error, returns C<NULL>.

=item C< #define mem_create(size, type)>

Allocates enough memory (with I<malloc(3)>) to store C<size> objects of type
C<type>. The memory returned is of type C<type *>. On success, returns the
address of the allocated memory. On error, returns C<NULL>.

=item C< #define mem_resize(mem, num)>

Alters the amount of memory pointed to by C<mem>. If C<mem> is C<NULL>,
I<malloc(3)> is used to allocate new memory. If size is zero, I<free(3)> is
called to deallocate the memory and C<mem> is set to C<NULL>. Otherwise,
I<realloc(3)> is called. If I<realloc(3)> needs to allocate new memory to
satisfy a request, C<mem> is set to the new address. On success, returns
I<mem> (though it may be C<NULL> if C<size> is zero). On error, C<NULL> is
returned and I<mem> is not altered.

=item C<void *mem_resize_fn(void **mem, size_t size)>

A single interface for altering the size of allocated memory. C<mem> points
to the pointer to be affected. C<size> is the size in bytes of the memory
that this pointer is to point to. If the pointer is C<NULL>, I<malloc(3)> is
used to obtain memory. If C<size> is zero, I<free(3)> is used to release the
memory. In all other cases, I<realloc(3)> is used to alter the size of the
memory. In all cases, the pointer pointed to by C<mem> is assigned to the
memory's location (or C<NULL> when C<size> is zero). This function is
exposed as an implementation side effect. Don't call it directly. Call
I<mem_resize()> instead. On error (i.e. I<malloc(3)> or I<realloc(3)> fail
or C<mem> is C<NULL>), returns C<NULL> without setting C<*mem> to anything.

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

=item C< #define mem_release(mem)>

Releases (deallocates) C<mem>. Same as I<free(3)>. Only to be used in
destructor functions. In other cases, use I<mem_destroy()> which also sets
C<mem> to C<NULL>.

=item C< #define mem_destroy(mem)>

Destroys (deallocates and sets to C<NULL>) the memory pointed to by C<mem>.

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

Returns a dynamically allocated copy of C<str>. On error, returns C<NULL>.
The caller must deallocate the memory returned.

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

=item C< #define mem_create2d(i, j, type)>

Shorthand for allocating a 2-dimensional array. See I<mem_create_space()>.

=item C< #define mem_create3d(i, j, k, type)>

Shorthand for allocating a 3-dimensional array. See I<mem_create_space()>.

=item C< #define mem_create4d(i, j, k, l, type)>

Shorthand for allocating a 4-dimensional array. See I<mem_create_space()>.

=item C<void *mem_create_space(size_t size, ...)>

Allocates a multi-dimensional array of elements of size C<size> and sets the
memory to zero. The remaining arguments specify the sizes of each dimension.
The last argument must be zero. There is an arbitrary limit of 32
dimensions. The memory returned is set to zero. The memory returned needs to
be cast or assigned into the appropriate pointer type. You can then set and
access elements exactly like a real multi-dimensional C array. Finally, it
must be deallocated with I<mem_destroy_space()> or I<mem_release_space()> or
I<mem_destroy()> or I<mem_release()> or I<free(3)>.

Note: You must not use I<memset(3)> on all of the returned memory because
the start of this memory contains pointers into the remainder. The exact
amount of this overhead depends on the number and size of dimensions. The
memory is allocated with I<calloc(3)> to reduce the need to I<memset(3)> the
elements but if you need to know where the elements begin, use
I<mem_space_start()>.

The memory returned looks like (e.g.):

   char ***a = mem_create3d(2, 2, 3, char);

                                              +-------------------------+
                                      +-------|-------------------+     |
                              +-------|-------|-------------+     |     |
                      +-------|-------|-------|-------+     |     |     |
                      |       |       |       |       V     V     V     V
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 a ->| a[0]  | a[1]  |a[0][0]|a[0][1]|a[1][0]|a[1][1]| | | | | | | | | | | | |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |       |       ^               ^               a a a a a a a a a a a a
      +-------|-------+               |               0 0 0 0 0 0 1 1 1 1 1 1
              +-----------------------+               0 0 0 1 1 1 0 0 0 1 1 1
                                                      0 1 2 0 1 2 0 1 2 0 1 2

=cut

*/

#ifndef MEM_MAX_DIM
#define MEM_MAX_DIM 32
#endif

void *mem_create_space(size_t size, ...)
{
	size_t dim[MEM_MAX_DIM], d, i, j;
	size_t lengths[MEM_MAX_DIM];
	size_t starts[MEM_MAX_DIM];
	size_t sizes[MEM_MAX_DIM];
	char *space;
	size_t arg, length;
	va_list args;

	va_start(args, size);
	for (d = 0; d < MEM_MAX_DIM && (arg = va_arg(args, size_t)); ++d)
		dim[d] = arg;
	va_end(args);

	length = 0;
	for (i = 0; i < d; ++i)
	{
		starts[i] = length;
		lengths[i] = sizes[i] = (i == d - 1) ? size : sizeof(void *);
		for (j = 0; j <= i; ++j)
			lengths[i] *= dim[j];
		length += lengths[i];
	}

	if (!(space = calloc(length, 1)))
		return NULL;

	for (i = 0; i < d - 1; ++i)
	{
		size_t num = dim[i];

		for (j = 0; j < i; ++j)
			num *= dim[j];

		for (j = 0; j < num; ++j)
			*(char **)(space + starts[i] + j * sizes[i]) = space + starts[i + 1] + j * dim[i + 1] * sizes[i + 1];
	}

	return space;
}

/*

=item C<size_t mem_space_start(size_t size, ...)>

Calculates the amount of overhead required for a multi-dimensional array
created by a call to I<mem_create_space()> with the same arguments. If you
need reset all elements in such an array to zero:

    int ****space = mem_create_space(sizeof(int), 2, 3, 4, 5, 0);
    size_t start = mem_space_start(sizeof(int), 2, 3, 4, 5, 0);
    memset((char *)space + start, '\0', sizeof(int) * 2 * 3 * 4 * 5);

=cut

*/

size_t mem_space_start(size_t size, ...)
{
	size_t dim[MEM_MAX_DIM], d, i, j;
	size_t lengths[MEM_MAX_DIM];
	size_t arg, length;
	va_list args;

	va_start(args, size);
	for (d = 0; d < MEM_MAX_DIM && (arg = va_arg(args, size_t)); ++d)
		dim[d] = arg;
	va_end(args);

	length = 0;
	for (i = 0; i < d; ++i)
	{
		lengths[i] = (i == d - 1) ? size : sizeof(void *);
		for (j = 0; j <= i; ++j)
			lengths[i] *= dim[j];
		length += lengths[i];
	}

	return length - lengths[d - 1];
}

/*

=item C< #define mem_release2d(space)>

Alias for releasing (deallocating) a 2-dimensional array.
See I<mem_release_space()>.

=item C< #define mem_release3d(space)>

Alias for releasing (deallocating) a 3-dimensional array.
See I<mem_release_space()>.

=item C< #define mem_release4d(space)>

Alias for releasing (deallocating) a 4-dimensional array.
See I<mem_release_space()>.

=item C< #define mem_release_space(space)>

Releases (deallocates) a multi-dimensional array, C<space>, allocated with
I<mem_create_space>. Same as I<free(3)>. Only to be used in destructor
functions. In other cases, use I<mem_destroy_space()> which also sets
C<space> to C<NULL>.

=item C< #define mem_destroy2d(space)>

Alias for destroying (deallocating and setting to C<NULL>) a
2-dimensional array. See I<mem_destroy_space()>.

=item C< #define mem_destroy3d(space)>

Alias for destroying (deallocating and setting to C<NULL>) a
3-dimensional array. See I<mem_destroy_space()>.

=item C< #define mem_destroy4d(space)>

Alias for destroying (deallocating and setting to C<NULL>) a
4-dimensional array. See I<mem_destroy_space()>.

=item C< #define mem_destroy_space(mem)>

Destroys (deallocates and sets to C<NULL>) the multi-dimensional array
pointed to by C<space>.

=back

=head1 EXAMPLES

1D array of longs:

    long *mem = mem_create(100, long);
    mem_resize(mem, 200);
    mem_destroy(mem);

3D array of ints:

    int ***space = mem_create3d(10, 20, 30, int);
    int i, j, k;

    for (i = 0; i < 10; ++i)
        for (j = 0; j < 20; ++j)
            for (k = 0; k < 30; ++k)
                space[i][j][k] = i + j + j;

    mem_destroy3d(space);

=head1 SEE ALSO

L<malloc(3)|malloc(3)>,
L<realloc(3)|realloc(3)>,
L<calloc(3)|calloc(3)>,
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
L<net(3)|net(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<str(3)|str(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

int main(int ac, char **av)
{
	int *mem1 = NULL;
	char *mem2 = NULL;
	char *str = NULL;
	int **space2 = NULL;
	int ***space3 = NULL;
	int ****space4 = NULL;
	int *****space5 = NULL;
	char **space2d = NULL;
	double ***space3d = NULL;
	int i, j, k, l, m;
	int errors = 0;

	printf("Testing: mem\n");

	/* Test create, resize and destroy */

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

	/* Test strdup */

	str = mem_strdup("test");
	if (!str)
		++errors, printf("Test10: mem_strdup() failed (returned null)\n");
	else if (strcmp(str, "test"))
	{
		++errors, printf("Test11: mem_strdup() failed (equals \"%s\", not \"test\")\n", str);
		mem_release(str);
	}

	/* Test 2D space allocation and deallocation */

	space2 = mem_create_space(sizeof(int), 1, 1, 0);
	if (!space2)
		++errors, printf("Test12: mem_create_space(1, 1) failed (returned null)\n");
	else
	{
		space2[0][0] = 37;
		if (space2[0][0] != 37)
			++errors, printf("Test13: mem_create_space(1, 1) failed (space2[%d][%d] = %d, not %d)\n", 0, 0, space2[0][0], 37);

		mem_destroy_space(space2);
		if (space2)
			++errors, printf("Test14: mem_destroy_space(1, 1) failed\n");
	}

	space2 = mem_create_space(sizeof(int), 10, 10, 0);
	if (!space2)
		++errors, printf("Test15: mem_create_space(10, 10) failed (returned null)\n");
	else
	{
		int error = 0;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				space2[i][j] = i + j;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				if (space2[i][j] != i + j)
					++error, printf("Test16: mem_create_space(10, 10) failed (space2[%d][%d] = %d, not %d)\n", i, j, space2[i][j], i + j);

		if (error)
			++errors;

		mem_destroy_space(space2);
		if (space2)
			++errors, printf("Test17: mem_destroy_space(10, 10) failed\n");
	}

	/* Test 3D space allocation and deallocation */

	space3 = mem_create_space(sizeof(int), 1, 1, 1, 0);
	if (!space3)
		++errors, printf("Test18: mem_create_space(1, 1, 1) failed (returned null)\n");
	else
	{
		space3[0][0][0] = 37;
		if (space3[0][0][0] != 37)
			++errors, printf("Test19: mem_create_space(1, 1, 1) failed (space3[%d][%d][%d] = %d, not %d)\n", 0, 0, 0, space3[0][0][0], 37);

		mem_destroy_space(space3);
		if (space3)
			++errors, printf("Test20: mem_destroy_space(1, 1, 1) failed\n");
	}

	space3 = mem_create_space(sizeof(int), 10, 10, 10, 0);
	if (!space3)
		++errors, printf("Test21: mem_create_space(10, 10, 10) failed (returned null)\n");
	else
	{
		int error = 0;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					space3[i][j][k] = i + j + k;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					if (space3[i][j][k] != i + j + k)
						++error, printf("Test22: mem_create_space(10, 10, 10) failed (space3[%d][%d][%d] = %d, not %d)\n", i, j, k, space3[i][j][k], i + j + k);

		if (error)
			++errors;

		mem_destroy_space(space3);
		if (space3)
			++errors, printf("Test23: mem_destroy_space(10, 10, 10) failed\n");
	}

	/* Test 4D space allocation and deallocation */

	space4 = mem_create_space(sizeof(int), 1, 1, 1, 1, 0);
	if (!space4)
		++errors, printf("Test24: mem_create_space(1, 1, 1, 1) failed (returned null)\n");
	else
	{
		space4[0][0][0][0] = 37;
		if (space4[0][0][0][0] != 37)
			++errors, printf("Test25: mem_create_space(1, 1, 1, 1) failed (space4[%d][%d][%d][%d] = %d, not %d)\n", 0, 0, 0, 0, space4[0][0][0][0], 37);

		mem_destroy_space(space4);
		if (space4)
			++errors, printf("Test26: mem_destroy_space(1, 1, 1, 1) failed\n");
	}

	space4 = mem_create_space(sizeof(int), 10, 10, 10, 10, 0);
	if (!space4)
		++errors, printf("Test27: mem_create_space(10, 10, 10, 10) failed (returned null)\n");
	else
	{
		int error = 0;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					for (l = 0; l < 10; ++l)
						space4[i][j][k][l] = i + j + k + l;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					for (l = 0; l < 10; ++l)
						if (space4[i][j][k][l] != i + j + k + l)
							++error, printf("Test28: mem_create_space(10, 10, 10, 10) failed (space4[%d][%d][%d][%d] = %d, not %d)\n", i, j, k, l, space4[i][j][k][l], i + j + k + l);

		if (error)
			++errors;

		mem_destroy_space(space4);
		if (space4)
			++errors, printf("Test29: mem_destroy_space(10, 10, 10, 10) failed\n");
	}

	/* Test 5D space allocation and deallocation */

	space5 = mem_create_space(sizeof(int), 1, 1, 1, 1, 1, 0);
	if (!space5)
		++errors, printf("Test30: mem_create_space(1, 1, 1, 1, 1) failed (returned null)\n");
	else
	{
		space5[0][0][0][0][0] = 37;
		if (space5[0][0][0][0][0] != 37)
			++errors, printf("Test31: mem_create_space(1, 1, 1, 1, 1) failed (space5[%d][%d][%d][%d][%d] = %d, not %d)\n", 0, 0, 0, 0, 0, space5[0][0][0][0][0], 37);

		mem_destroy_space(space5);
		if (space5)
			++errors, printf("Test32: mem_destroy_space(1, 1, 1, 1, 1) failed\n");
	}

	space5 = mem_create_space(sizeof(int), 10, 10, 10, 10, 10, 0);
	if (!space5)
		++errors, printf("Test33: mem_create_space(10, 10, 10, 10, 10) failed (returned null)\n");
	else
	{
		int error = 0;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					for (l = 0; l < 10; ++l)
						for (m = 0; m < 10; ++m)
							space5[i][j][k][l][m] = i + j + k + l + m;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					for (l = 0; l < 10; ++l)
						for (m = 0; m < 10; ++m)
							if (space5[i][j][k][l][m] != i + j + k + l + m)
								++error, printf("Test34: mem_create_space(10, 10, 10, 10, 10) failed (space5[%d][%d][%d][%d][%d] = %d, not %d)\n", i, j, k, l, m, space5[i][j][k][l][m], i + j + k + l + m);

		if (error)
			++errors;

		mem_destroy_space(space5);
		if (space5)
			++errors, printf("Test35: mem_destroy_space(10, 10, 10, 10, 10) failed\n");
	}

	/* Test element sizes smaller than sizeof(void *) */

	space2d = mem_create_space(sizeof(char), 1, 1, 0);
	if (!space2d)
		++errors, printf("Test36: mem_create_space(char, 1, 1) failed (returned null)\n");
	else
	{
		space2d[0][0] = 'a';
		if (space2d[0][0] != 'a')
			++errors, printf("Test37: mem_create_space(char, 1, 1) failed (space2d[%d][%d] = '%c', not '%c')\n", 0, 0, space2d[0][0], 'a');

		mem_destroy_space(space2d);
		if (space2d)
			++errors, printf("Test38: mem_destroy_space(char, 1, 1) failed\n");
	}

	space2d = mem_create_space(sizeof(char), 10, 10, 0);
	if (!space2d)
		++errors, printf("Test39: mem_create_space(char, 10, 10) failed (returned null)\n");
	else
	{
		int error = 0;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				space2d[i][j] = 'a' + (i + j) % 26;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				if (space2d[i][j] != 'a' + (i + j) % 26)
					++error, printf("Test40: mem_create_space(char, 10, 10) failed (space2d[%d][%d] = '%c', not '%c')\n", i, j, space2d[i][j], 'a' + (i + j) % 26);

		if (error)
			++errors;

		mem_destroy_space(space2d);
		if (space2d)
			++errors, printf("Test41: mem_destroy_space(char, 10, 10) failed\n");
	}

	/* Test element sizes larger than sizeof(void *) */

	space3d = mem_create_space(sizeof(double), 1, 1, 1, 0);
	if (!space3d)
		++errors, printf("Test42: mem_create_space(double, 1, 1, 1) failed (returned null)\n");
	else
	{
		space3d[0][0][0] = 37.5;
		if (space3d[0][0][0] != 37.5)
			++errors, printf("Test43: mem_create_space(double, 1, 1, 1) failed (space3d[%d][%d][%d] = %g, not %g)\n", 0, 0, 0, space3d[0][0][0], 37.5);

		mem_destroy_space(space3d);
		if (space3d)
			++errors, printf("Test44: mem_destroy_space(double, 1, 1, 1) failed\n");
	}

	space3d = mem_create_space(sizeof(double), 10, 10, 10, 0);
	if (!space3d)
		++errors, printf("Test45: mem_create_space(double, 10, 10, 10) failed (returned null)\n");
	else
	{
		int error = 0;

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					space3d[i][j][k] = (double)(i + j + k);

		for (i = 0; i < 10; ++i)
			for (j = 0; j < 10; ++j)
				for (k = 0; k < 10; ++k)
					if (space3d[i][j][k] != (double)(i + j + k))
						++error, printf("Test46: mem_create_space(double, 10, 10, 10) failed (space3[%d][%d][%d] = %g, not %g)\n", i, j, k, space3d[i][j][k], (double)(i + j + k));

		if (error)
			++errors;

		mem_destroy_space(space3d);
		if (space3d)
			++errors, printf("Test47: mem_destroy_space(double, 10, 10, 10) failed\n");
	}

	/* Test mem_space_start() */

	space4 = mem_create_space(sizeof(int), 2, 3, 4, 5, 0);
	if (!space4)
		++errors, printf("Test48: mem_create_space(int, 2, 3, 4, 5) failed (returned null)\n");
	else
	{
		int error = 0;
		size_t start;

		for (i = 0; i < 2; ++i)
			for (j = 0; j < 3; ++j)
				for (k = 0; k < 4; ++k)
					for (l = 0; l < 5; ++l)
						space4[i][j][k][l] = i + j + k + l;

		for (i = 0; i < 2; ++i)
			for (j = 0; j < 3; ++j)
				for (k = 0; k < 4; ++k)
					for (l = 0; l < 5; ++l)
						if (space4[i][j][k][l] != i + j + k + l)
							++error, printf("Test49: mem_create_space(int, 2, 3, 4, 5) failed (space4[%d][%d][%d][%d] = %d, not %d)\n", i, j, k, l, space4[i][j][k][l], i + j + k + l);

		if (error)
			++errors;

		start = mem_space_start(sizeof(int), 2, 3, 4, 5, 0);
		memset((char *)space4 + start, '\0', sizeof(int) * 2 * 3 * 4 * 5);

		for (i = 0; i < 2; ++i)
			for (j = 0; j < 3; ++j)
				for (k = 0; k < 4; ++k)
					for (l = 0; l < 4; ++l)
						if (space4[i][j][k][l] != 0)
							++error, printf("Test50: mem_space_start(int, 2, 3, 4, 5) failed (space4[%d][%d][%d][%d] = %d, not %d)\n", i, j, k, l, space4[i][j][k][l], 0);

		for (i = 0; i < 2; ++i)
			for (j = 0; j < 3; ++j)
				for (k = 0; k < 4; ++k)
					for (l = 0; l < 5; ++l)
						space4[i][j][k][l] = i + j + k + l;

		for (i = 0; i < 2; ++i)
			for (j = 0; j < 3; ++j)
				for (k = 0; k < 4; ++k)
					for (l = 0; l < 5; ++l)
						if (space4[i][j][k][l] != i + j + k + l)
							++error, printf("Test51: mem_space_start(int, 2, 3, 4, 5) failed (space4[%d][%d][%d][%d] = %d, not %d)\n", i, j, k, l, space4[i][j][k][l], i + j + k + l);

		if (error)
			++errors;

		mem_destroy_space(space4);
		if (space4)
			++errors, printf("Test52: mem_destroy_space(int, 2, 3, 4, 5) failed\n");
	}

	if (errors)
		printf("%d/52 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
