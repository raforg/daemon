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

#ifndef LIBSLACK_HDR_H
#define LIBSLACK_HDR_H

#undef __hdr_start_decls
#undef __hdr_stop_decls

#ifdef __cplusplus
#define __hdr_start_decls extern "C" {
#define __hdr_stop_decls   }
#else
#define __hdr_start_decls
#define __hdr_stop_decls
#endif

#undef __hdr_proto
#undef const

#if defined __STDC__ || defined __cplusplus
#define __hdr_proto(args) args
#else
#define __hdr_proto(args) ()
#define const
#endif

#undef __PROTO
#undef __START_DECLS
#undef __STOP_DECLS

#define __PROTO       __hdr_proto
#define __START_DECLS __hdr_start_decls
#define __STOP_DECLS   __hdr_stop_decls

#endif

/* vi:set ts=4 sw=4: */
