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

#ifndef LIBSLACK_STD_H
#define LIBSLACK_STD_H

#ifndef __STDC__
#define __STDC__
#endif

#ifndef __STRICT_ANSI__
#define __STRICT_ANSI__
#endif

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199506L
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#ifndef _ISOC9X_SOURCE
#define _ISOC9X_SOURCE
#endif

#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif

#ifndef _THREAD_SAFE
#define _THREAD_SAFE
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <errno.h>

#endif

/* vi:set ts=4 sw=4: */
