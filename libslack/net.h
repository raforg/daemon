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

#ifndef LIBSLACK_NET_H
#define LIBSLACK_NET_H

#include <stdarg.h>

#include <sys/socket.h>

#include <slack/hdr.h>

typedef unsigned short port_t;
typedef struct sockaddr sockaddr_t;

_start_decls
int net_server _args ((const char *interface, const char *service, port_t port, int rcvbufsz, int sndbufsz, sockaddr_t *addr, size_t *addrsize));
int net_client _args ((const char *host, const char *service, port_t port, long timeout, int rcvbufsz, int sndbufsz, sockaddr_t *addr, size_t *addrsize));
int net_udp_server _args ((const char *interface, const char *service, port_t port, int rcvbufsz, int sndbufsz, sockaddr_t *addr, size_t *addrsize));
int net_udp_client _args ((const char *host, const char *service, port_t port, int rcvbufsz, int sndbufsz, sockaddr_t *addr, size_t *addrsize));
int net_create_server _args ((const char *interface, const char *service, port_t port, int type, int protocol, int rcvbufsz, int sndbufsz, sockaddr_t *addr, size_t *addrsize));
int net_create_client _args ((const char *host, const char *service, port_t port, int type, int protocol, long timeout, int rcvbufsz, int sndbufsz, sockaddr_t *addr, size_t *addrsize));
ssize_t net_pack _args ((int sockfd, int flags, const char *fmt, ...));
ssize_t net_vpack _args ((int sockfd, int flags, const char *fmt, va_list args));
ssize_t net_packto _args ((int sockfd, int flags, const sockaddr_t *to, size_t tolen, const char *fmt, ...));
ssize_t net_vpackto _args ((int sockfd, int flags, const sockaddr_t *to, size_t tolen, const char *fmt, va_list args));
ssize_t net_unpack _args ((int sockfd, int flags, const char *fmt, ...));
ssize_t net_vunpack _args ((int sockfd, int flags, const char *fmt, va_list args));
ssize_t net_unpackfrom _args ((int sockfd, int flags, sockaddr_t *from, size_t *fromlen, const char *fmt, ...));
ssize_t net_vunpackfrom _args ((int sockfd, int flags, sockaddr_t *from, size_t *fromlen, const char *fmt, va_list args));
ssize_t pack _args ((void *buf, size_t size, const char *fmt, ...));
ssize_t vpack _args ((void *buf, size_t size, const char *fmt, va_list args));
ssize_t unpack _args ((void *buf, size_t size, const char *fmt, ...));
ssize_t vunpack _args ((void *buf, size_t size, const char *fmt, va_list args));
ssize_t net_read _args ((int sockfd, long timeout, char *buf, size_t count));
ssize_t net_write _args ((int sockfd, long timeout, const char *buf, size_t count));
ssize_t net_expect _args ((int sockfd, long timeout, const char *fmt, ...));
ssize_t net_vexpect _args ((int sockfd, long timeout, const char *fmt, va_list args));
ssize_t net_send _args ((int sockfd, long timeout, const char *fmt, ...));
ssize_t net_vsend _args ((int sockfd, long timeout, const char *fmt, va_list args));
int mail _args ((const char *server, const char *sender, const char *recipients, const char *subject, const char *message));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
