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

#ifndef LIBSLACK_ERR_H
#define LIBSLACK_ERR_H

#include <stdlib.h>
#include <stdarg.h>

#include <slack/hdr.h>

#undef debug
#undef vdebug
#undef debugsys
#undef vdebugsys
#undef check

#ifdef NDEBUG
#define debug(args)
#define vdebug(args)
#define debugsys(args)
#define vdebugsys(args)
#define check(cond, msg)
#else
#define debug(args) debugf args;
#define vdebug(args) vdebugf args;
#define debugsys(args) debugsysf args;
#define vdebugsys(args) vdebugsysf args;
#define check(test, msg) dump("Internal Error: %s: %s [\"%s\":%d]", (#test), (msg), __FILE__, __LINE__);
#endif

_start_decls
void msg _args ((const char *fmt, ...));
void vmsg _args ((const char *fmt, va_list args));
void verbose _args ((size_t level, const char *fmt, ...));
void vverbose _args ((size_t level, const char *fmt, va_list args));
void debugf _args ((size_t level, const char *fmt, ...));
void vdebugf _args ((size_t level, const char *fmt, va_list args));
int error _args ((const char *fmt, ...));
int verror _args ((const char *fmt, va_list args));
void fatal _args ((const char *fmt, ...));
void vfatal _args ((const char *fmt, va_list args));
void dump _args ((const char *fmt, ...));
void vdump _args ((const char *fmt, va_list args));
void debugsysf _args ((size_t level, const char *fmt, ...));
void vdebugsysf _args ((size_t level, const char *fmt, va_list args));
int errorsys _args ((const char *fmt, ...));
int verrorsys _args ((const char *fmt, va_list args));
void fatalsys _args ((const char *fmt, ...));
void vfatalsys _args ((const char *fmt, va_list args));
void dumpsys _args ((const char *fmt, ...));
void vdumpsys _args ((const char *fmt, va_list args));
int set_errno _args ((int errnum));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
