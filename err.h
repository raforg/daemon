/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef ERR_H
#define ERR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void msg(const char *fmt, ...);
void vmsg(const char *fmt, va_list args);
void debug(size_t level, const char *fmt, ...);
void vdebug(size_t level, const char *fmt, va_list args);
int error(const char *fmt, ...);
int verror(const char *fmt, va_list args);
void fatal(const char *fmt, ...);
void vfatal(const char *fmt, va_list args);
void dump(const char *fmt, ...);
void vdump(const char *fmt, va_list args);
void debugsys(size_t level, const char *fmt, ...);
void vdebugsys(size_t level, const char *fmt, va_list args);
int errorsys(const char *fmt, ...);
int verrorsys(const char *fmt, va_list args);
void fatalsys(const char *fmt, ...);
void vfatalsys(const char *fmt, va_list args);
void dumpsys(const char *fmt, ...);
void vdumpsys(const char *fmt, va_list args);
int set_errno(int errnum);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
