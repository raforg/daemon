/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999-2010 raf <raf@raf.org>
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
* 20100612 raf <raf@raf.org>
*/

#ifndef LIBSLACK_LIB_H
#define LIBSLACK_LIB_H

#include <slack/std.h>
#include <slack/agent.h>
#include <slack/coproc.h>
#include <slack/daemon.h>
#include <slack/err.h>
#include <slack/fio.h>
#include <slack/hsort.h>
#include <slack/lim.h>
#include <slack/link.h>
#include <slack/list.h>
#include <slack/locker.h>
#include <slack/map.h>
#include <slack/mem.h>
#include <slack/msg.h>
#include <slack/net.h>
#include <slack/prog.h>
#include <slack/prop.h>
#include <slack/pseudo.h>
#include <slack/sig.h>
#include <slack/str.h>

#ifndef HAVE_SNPRINTF
#include <slack/snprintf.h>
#endif

#ifndef HAVE_VSSCANF
#include <slack/vsscanf.h>
#endif

#endif

/* vi:set ts=4 sw=4: */
