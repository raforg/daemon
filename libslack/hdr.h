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

#ifndef LIBSLACK_HDR_H
#define LIBSLACK_HDR_H

#undef _start_decls
#undef _end_decls
#undef _args
#undef _throw
#undef const

#ifdef __cplusplus
#define _start_decls extern "C" {
#define _end_decls }
#else
#define _start_decls
#define _end_decls
#endif

#define _throw
#if 0
#ifdef __GNUC__
#if __cplusplus && ((__GNUC__) >= 3 || (__GNUC_MINOR__) >= 8)
#undef _throw
#define _throw throw ()
#endif
#endif
#endif

#if defined __STDC__ || defined __cplusplus
#define _args(args) args _throw
#else
#define _args(args) ()
#define const
#endif

#endif

/* vi:set ts=4 sw=4: */
