/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (C) 1999 raf <raf2@zip.com.au>
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
*/

#ifndef LIBPROG_HDR_H
#define LIBPROG_HDR_H

#ifdef __cplusplus
#define __hdr_begin_decls extern "C" {
#define __hdr_end_decls   }
#else
#define __hdr_begin_decls
#define __hdr_end_decls
#endif

#if defined __STDC__ || defined __cplusplus
#define __hdr_proto(args) args
#else
#define __hdr_proto(args) ()
#define const
#endif

#undef __PROTO
#undef __BEGIN_DECLS
#undef __END_DECLS

#define __PROTO       __hdr_proto
#define __BEGIN_DECLS __hdr_begin_decls
#define __END_DECLS   __hdr_end_decls

#endif

/* vi:set ts=4 sw=4: */
