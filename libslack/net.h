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

#ifndef LIBSLACK_NET_H
#define LIBSLACK_NET_H

#include <stdarg.h>

#include <sys/socket.h>

#include <slack/hdr.h>

typedef unsigned short port_t;
typedef struct sockaddr sockaddr;

__START_DECLS
int net_server __PROTO ((const char *iface, port_t port, sockaddr *addr, size_t *addrsize));
int net_client __PROTO ((const char *host, port_t port, sockaddr *addr, size_t *addrsize));
int net_udp_server __PROTO ((const char *iface, port_t port, sockaddr *addr, size_t *addrsize));
int net_udp_client __PROTO ((const char *host, port_t port, sockaddr *addr, size_t *addrsize));
int net_create_server __PROTO ((const char *iface, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize));
int net_create_client __PROTO ((const char *host, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize));
ssize_t net_read __PROTO ((int sockfd, char *buf, size_t count));
ssize_t net_write __PROTO ((int sockfd, const char *buf, size_t count));
ssize_t net_expect __PROTO ((int sockfd, size_t timeout, const char *fmt, ...));
ssize_t net_vexpect __PROTO ((int sockfd, size_t timeout, const char *fmt, va_list args));
ssize_t net_send __PROTO ((int sockfd, const char *fmt, ...));
ssize_t net_vsend __PROTO ((int sockfd, const char *fmt, va_list args));
int net_chat __PROTO ((int fd, char *script));
ssize_t net_pack __PROTO ((int sockfd, int flags, const char *fmt, ...));
ssize_t net_vpack __PROTO ((int sockfd, int flags, const char *fmt, va_list args));
ssize_t net_packto __PROTO ((int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, ...));
ssize_t net_vpackto __PROTO ((int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, va_list args));
ssize_t net_unpack __PROTO ((int sockfd, int flags, const char *fmt, ...));
ssize_t net_vunpack __PROTO ((int sockfd, int flags, const char *fmt, va_list args));
ssize_t net_unpackfrom __PROTO ((int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, ...));
ssize_t net_vunpackfrom __PROTO ((int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, va_list args));
ssize_t pack __PROTO ((void *buf, size_t size, const char *fmt, ...));
ssize_t vpack __PROTO ((void *buf, size_t size, const char *fmt, va_list args));
ssize_t unpack __PROTO ((void *buf, size_t size, const char *fmt, ...));
ssize_t vunpack __PROTO ((void *buf, size_t size, const char *fmt, va_list args));
int mail __PROTO ((const char *server, const char *sender, const char *recipients, const char *subject, const char *message));
char *rfc822_localtime __PROTO ((char *buf, size_t max, time_t time));
char *rfc822_gmtime __PROTO ((char *buf, size_t max, time_t time));
__STOP_DECLS

#endif

/* vi:set ts=4 sw=4: */
