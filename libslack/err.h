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

#ifndef LIBSLACK_ERR_H
#define LIBSLACK_ERR_H

#include <stdlib.h>
#include <stdarg.h>

#include <slack/hdr.h>

#undef debug
#undef vdebug
#undef debugsys
#undef vdebugsys
#undef assert

#ifdef NDEBUG
#define debug(args)
#define vdebug(args)
#define debugsys(args)
#define vdebugsys(args)
#define assert(cond, msg)
#else
#define debug(args) _debug args
#define vdebug(args) _vdebug args
#define debugsys(args) _debugsys args
#define vdebugsys(args) _vdebugsys args
#define assert(test, msg) dump("Internal Error: %s: %s [\"%s\":%d]", (#test), (msg), __FILE__, __LINE__)
#endif

__START_DECLS
void msg __PROTO ((const char *fmt, ...));
void vmsg __PROTO ((const char *fmt, va_list args));
void verbose __PROTO ((size_t level, const char *fmt, ...));
void vverbose __PROTO ((size_t level, const char *fmt, va_list args));
void _debug __PROTO ((size_t level, const char *fmt, ...));
void _vdebug __PROTO ((size_t level, const char *fmt, va_list args));
int error __PROTO ((const char *fmt, ...));
int verror __PROTO ((const char *fmt, va_list args));
void fatal __PROTO ((const char *fmt, ...));
void vfatal __PROTO ((const char *fmt, va_list args));
void dump __PROTO ((const char *fmt, ...));
void vdump __PROTO ((const char *fmt, va_list args));
void _debugsys __PROTO ((size_t level, const char *fmt, ...));
void _vdebugsys __PROTO ((size_t level, const char *fmt, va_list args));
int errorsys __PROTO ((const char *fmt, ...));
int verrorsys __PROTO ((const char *fmt, va_list args));
void fatalsys __PROTO ((const char *fmt, ...));
void vfatalsys __PROTO ((const char *fmt, va_list args));
void dumpsys __PROTO ((const char *fmt, ...));
void vdumpsys __PROTO ((const char *fmt, va_list args));
int set_errno __PROTO ((int errnum));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
