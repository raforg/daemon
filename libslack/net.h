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
* 20011109 raf <raf@raf.org>
*/

#ifndef LIBSLACK_NET_H
#define LIBSLACK_NET_H

#include <stdarg.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <slack/list.h>

#include <slack/hdr.h>

typedef struct sockaddr sockaddr;
typedef unsigned short sockport_t;
typedef struct sockopt_t sockopt_t;

typedef union sockaddr_any sockaddr_any;
typedef struct sockaddr_un sockaddr_un;
typedef struct sockaddr_in sockaddr_in;
#ifdef AF_INET6
typedef struct sockaddr_in6 sockaddr_in6;
#endif

typedef struct net_interface_t net_interface_t;
typedef struct rudp_t rudp_t;

struct sockopt_t
{
	int level;
	int optname;
	const void *optval;
	int optlen;
};

union sockaddr_any
{
	sockaddr any;
	sockaddr_un un;
	sockaddr_in in;
#ifdef AF_INET6
	sockaddr_in6 in6;
#endif
};

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#ifndef IFHWADDRLEN
#define IFHWADDRLEN 6
#endif

struct net_interface_t
{
	char name[IFNAMSIZ];      /* network interface name, null terminated */
	unsigned int index;       /* network interface index */
	short flags;              /* IFF_ constants from <net/if.h> */
	int mtu;                  /* Maximum Transmission Unit */
	sockaddr_any *addr;       /* address */
	sockaddr_any *brdaddr;    /* broadcast address, if any */
	sockaddr_any *dstaddr;    /* destination address, if any */
	sockaddr_any *hwaddr;     /* hardware address, if any (not on Solaris) */
};

_begin_decls
int net_server _args ((const char *interface, const char *service, sockport_t port, int rcvbufsz, int sndbufsz, sockaddr *addr, size_t *addrsize));
int net_client _args ((const char *host, const char *service, sockport_t port, long timeout, int rcvbufsz, int sndbufsz, sockaddr *addr, size_t *addrsize));
int net_udp_server _args ((const char *interface, const char *service, sockport_t port, int rcvbufsz, int sndbufsz, sockaddr *addr, size_t *addrsize));
int net_udp_client _args ((const char *host, const char *service, sockport_t port, int rcvbufsz, int sndbufsz, sockaddr *addr, size_t *addrsize));
int net_create_server _args ((const char *interface, const char *service, sockport_t port, int type, int protocol, sockopt_t *sockopts, sockaddr *addr, size_t *addrsize));
int net_create_client _args ((const char *host, const char *service, sockport_t port, sockport_t localport, int type, int protocol, long timeout, sockopt_t *sockopts, sockaddr *addr, size_t *addrsize));
int net_multicast_sender _args ((const char *group, const char *service, sockport_t port, sockopt_t *sockopts, sockaddr *addr, size_t *addrsize, const char *ifname, unsigned int ifindex, int ttl, unsigned int noloopback));
int net_multicast_receiver _args ((const char *group, const char *service, sockport_t port, sockopt_t *sockopts, sockaddr *addr, size_t *addrsize, const char *ifname, unsigned int ifindex));
int net_multicast_join _args ((int sockfd, const sockaddr *addr, size_t addrsize, const char *ifname, unsigned int ifindex));
int net_multicast_leave _args ((int sockfd, const sockaddr *addr, size_t addrsize, const char *ifname, unsigned int ifindex));
int net_multicast_set_interface _args ((int sockfd, const char *ifname, unsigned int ifindex));
int net_multicast_get_interface _args ((int sockfd));
int net_multicast_set_loopback _args ((int sockfd, unsigned int loopback));
int net_multicast_get_loopback _args ((int sockfd));
int net_multicast_set_ttl _args ((int sockfd, int ttl));
int net_multicast_get_ttl _args ((int sockfd));
int net_tos_lowdelay _args ((int sockfd));
int net_tos_throughput _args ((int sockfd));
int net_tos_reliability _args ((int sockfd));
int net_tos_lowcost _args ((int sockfd));
int net_tos_normal _args ((int sockfd));
struct hostent *net_gethostbyname _args ((const char *name, struct hostent *hostbuf, void **buf, size_t *size, int *herrno));
struct servent *net_getservbyname _args ((const char *name, const char *proto, struct servent *servbuf, void **buf, size_t *size));
int net_options _args ((int sockfd, sockopt_t *sockopts));
List *net_interfaces _args ((void));
List *net_interfaces_with_locker _args ((Locker *locker));
List *net_interfaces_by_family _args ((int family));
List *net_interfaces_by_family_with_locker _args ((int family, Locker *locker));
rudp_t *rudp_create _args ((void));
void rudp_release _args ((rudp_t *rudp));
void *rudp_destroy _args ((rudp_t **rudp));
ssize_t net_rudp_transact _args ((int sockfd, rudp_t *rudp, const void *obuf, size_t osize, void *ibuf, size_t isize));
ssize_t net_rudp_transactwith _args ((int sockfd, rudp_t *rudp, const void *obuf, size_t osize, int oflags, void *ibuf, size_t isize, int iflags, sockaddr_any *addr, size_t addrsize));
ssize_t net_pack _args ((int sockfd, long timeout, int flags, const char *fmt, ...));
ssize_t net_vpack _args ((int sockfd, long timeout, int flags, const char *fmt, va_list args));
ssize_t net_packto _args ((int sockfd, long timeout, int flags, const sockaddr *to, size_t tosize, const char *fmt, ...));
ssize_t net_vpackto _args ((int sockfd, long timeout, int flags, const sockaddr *to, size_t tosize, const char *fmt, va_list args));
ssize_t net_unpack _args ((int sockfd, long timeout, int flags, const char *fmt, ...));
ssize_t net_vunpack _args ((int sockfd, long timeout, int flags, const char *fmt, va_list args));
ssize_t net_unpackfrom _args ((int sockfd, long timeout, int flags, sockaddr *from, size_t *fromsize, const char *fmt, ...));
ssize_t net_vunpackfrom _args ((int sockfd, long timeout, int flags, sockaddr *from, size_t *fromsize, const char *fmt, va_list args));
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
ssize_t sendfd _args ((int sockfd, const void *buf, size_t nbytes, int flags, int fd));
ssize_t recvfd _args ((int sockfd, void *buf, size_t nbytes, int flags, int *fd));
int mail _args ((const char *server, const char *sender, const char *recipients, const char *subject, const char *message));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
