/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef MEM_H
#define MEM_H

#define mem_create(size, type) malloc((size) * sizeof(type))
#define mem_resize(mem, size) mem_resize_fn((void*)&(mem), (size) * sizeof(*(mem)))
#define mem_release(mem) free(mem)
#define mem_destroy(mem) mem_destroy_fn((void**)&(mem))

#ifdef __cplusplus
extern "C" {
#endif

void *mem_resize_fn(void **mem, size_t size);
void *mem_destroy_fn(void **mem);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
