/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef SNPRINTF_H
#define SNPRINTF_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int snprintf(char *str, size_t n, const char *fmt, ...);
int vsnprintf(char *str, size_t n, const char *fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
