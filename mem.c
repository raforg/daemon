/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdlib.h>

#include "mem.h"

/*
** #define mem_create(size, type)
**
** Allocates enough memory to store size objects of type type.
** The memory returned is of type type*. Uses malloc(3).
**
** #define mem_resize(mem, num)
**
** Alters the amount of memory pointed to by mem. Uses mem_resize_fn().
** If mem is null, malloc(3) will be used to allocate new memory. If
** size is zero, free(3) will be called to deallocate the memory. Otherwise,
** realloc(3) will be called.
**
** #define mem_release(mem)
**
** Same as free(3). Only to be used in destructor functions. In other cases,
** use mem_destroy().
**
** #define mem_destroy(mem)
**
** Uses mem_destroy_fn() to deallocate the memory pointed to by mem and assign
** null to mem.
*/

/*
** void *mem_resize_fn(void **mem, size_t size)
**
** A single interface for altering the size of allocated memory.
** mem points to the pointer to be affected. size is the size in
** bytes of the memory that this pointer is to point to. If the
** pointer is null, malloc(3) is used to obtain memory. If size
** is zero, free(3) is used to release the memory. In all other
** cases, realloc(3) is used to alter the size of the memory.
** In all cases, the pointer pointed to by mem is assigned to the
** memory's location (or null when size is zero).
** This is only used by the macros in mem.h to ensure that pointers
** that are free()d are always zeroed. If an error occurs (malloc(3)
** or realloc(3) fail or mem is null), it returns NULL without setting
** *mem to anything.
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
** void *mem_destroy_fn(void **mem)
**
** Calls free(3) on the pointer pointed to by mem. Then assigns null to this
** pointer. This is only used by the macros in mem.h to ensure that pointers
** that are free()d are always zeroed.
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

#ifdef TEST

#include <stdio.h>

int main(int ac, char **av)
{
	int *mem1 = NULL;
	char *mem2 = NULL;
	int errors = 0;

	printf("Testing: %s\n", *av);

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

	exit(0);
}

#endif

/* vi:set ts=4 sw=4: */
