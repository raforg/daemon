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

/*

=head1 NAME

I<libslack(net)> - network module

=head1 SYNOPSIS

    #include <slack/net.h>

    typedef unsigned short port_t;
    typedef struct sockaddr sockaddr;

    int net_server(const char *iface, port_t port, sockaddr *addr, size_t *addrsize);
    int net_client(const char *host, port_t port, sockaddr *addr, size_t *addrsize);
    int net_udp_server(const char *iface, port_t port, sockaddr *addr, size_t *addrsize);
    int net_udp_client(const char *host, port_t port, sockaddr *addr, size_t *addrsize);
    int net_create_server(const char *iface, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize);
    int net_create_client(const char *host, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize);
    ssize_t net_read(int sockfd, char *buf, size_t count);
    ssize_t net_write(int sockfd, const char *buf, size_t count);
    ssize_t net_expect(int sockfd, size_t timeout, const char *fmt, ...);
    ssize_t vnet_expect(int sockfd, size_t timeout, const char *fmt, va_list args);
    ssize_t net_send(int sockfd, const char *fmt, ...);
    ssize_t vnet_send(int sockfd, const char *fmt, va_list args);
    int net_chat(int fd, char *script);
    ssize_t net_pack(int sockfd, int flags, const char *fmt, ...);
    ssize_t net_vpack(int sockfd, int flags, const char *fmt, va_list args);
    ssize_t net_packto(int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, ...);
    ssize_t net_vpackto(int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, va_list args);
    ssize_t net_unpack(int sockfd, int flags, const char *fmt, ...);
    ssize_t net_vunpack(int sockfd, int flags, const char *fmt, va_list args);
    ssize_t net_unpackfrom(int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, ...);
    ssize_t net_vunpackfrom(int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, va_list args);
    ssize_t pack(void *buf, size_t size, const char *fmt, ...);
    ssize_t vpack(void *buf, size_t size, const char *fmt, va_list args);
    ssize_t unpack(void *buf, size_t size, const char *fmt, ...);
    ssize_t vunpack(void *buf, size_t size, const char *fmt, va_list args);
    int mail(const char *server, const char *sender, const char *recipients, const char *subject, const char *message);
    char *rfc822_localtime(char *buf, size_t max, time_t time);
    char *rfc822_gmtime(char *buf, size_t max, time_t time);

=head1 DESCRIPTION

This module provides functions that create client and server sockets,
that expect and send text dialogues, and that pack and unpack arbitrary
packets.

=over 4

=cut

*/

#include "std.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <termios.h>

#include "net.h"
#include "err.h"
#include "str.h"

#ifdef NEEDS_SNPRINTF
#include "snprintf.h"
#endif

#ifdef NEEDS_VSSCANF
#include "vsscanf.h"
#endif

#ifdef SOCKS
#include "socks.h"
#endif

#ifndef MSG_SIZE
#define MSG_SIZE 8192
#endif

/*

=item C<int net_server(const char *iface, port_t port, sockaddr *addr, size_t *addrsize)>

Creates a tcp server socket ready to I<accept(2)> connections on
C<iface:port>. If C<iface> is C<NULL>, connections will be accepted on all
local network interfaces. Otherwise, connections will only be accepted on
the specified interface (as determined by I<gethostbyname(3)>). If C<addr>
is not C<NULL>, the address bound to is stored at that address. If
C<addrsize> is not C<NULL>, the length of C<addr> is stored at that address.
On success, returns the new socket's file descriptor. On error, returns -1
with C<errno> set appropriately.

=cut

*/

int net_server(const char *iface, port_t port, sockaddr *addr, size_t *addrsize)
{
	return net_create_server(iface, port, SOCK_STREAM, 0, addr, addrsize);
}

/*

=item C<int net_client(const char *host, port_t port, sockaddr *addr, size_t *addrsize, size_t *addrsize)>

Creates a tcp client socket and connects to the server listening at
C<host:port> (as determined by I<gethostbyname(3)>). If C<addr> is not
C<NULL>, the address of the peer is stored at that address. If C<addrsize>
is not C<NULL>, the length of C<addr> is stored at that address. On success,
returns the new socket's file descriptor. On error, returns -1 with C<errno>
set appropriately.

=cut

*/

int net_client(const char *host, port_t port, sockaddr *addr, size_t *addrsize)
{
	return net_create_client(host, port, SOCK_STREAM, 0, addr, addrsize);
}

/*

=item C<int net_udp_server(const char *iface, port_t port, sockaddr *addr, size_t *addrsize)>

Creates a udp server socket ready to I<recv(2)> packets on C<iface:port>. If
C<iface> is C<NULL>, packets will be accepted on all local network
interfaces. Otherwise, packets will only be accepted on the specified
interface (as determined by I<gethostbyname(3)>). If C<addr> is not C<NULL>,
the address bound to is stored at that address. If C<addrsize> is not C<NULL>,
the length of C<addr> is stored at that address. On success, returns the new
socket's file descriptor. On error, returns -1 with C<errno> set
appropriately.

=cut

*/

int net_udp_server(const char *iface, port_t port, sockaddr *addr, size_t *addrsize)
{
	return net_create_server(iface, port, SOCK_DGRAM, 0, addr, addrsize);
}

/*

=item C<int net_udp_client(const char *host, port_t port, sockaddr *addr, size_t *addrsize)>

Creates a udp client socket and connects to the server listening at
C<host:port> (as determined by I<gethostbyname(3)>). If C<addr> is not
C<NULL>, the address of the peer is stored at that address. If C<addrsize>
is not C<NULL>, the length of C<addr> is stored at that address. On success,
returns the new socket's file descriptor. On error, returns -1 with C<errno>
set appropriately.

=cut

*/

int net_udp_client(const char *host, port_t port, sockaddr *addr, size_t *addrsize)
{
	return net_create_client(host, port, SOCK_DGRAM, 0, addr, addrsize);
}

/*

=item C<int net_create_server(const char *iface, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize)>

Creates a server socket of the specified C<type> (e.g. SOCK_STREAM or
SOCK_DGRAM) and C<protocol> (usually zero) ready to I<accept(2)> connections
on C<iface:port>. If C<iface> is C<NULL>, connections will be accepted on
all local network interfaces. Otherwise, connections will only be accepted
on the specified interface (as determined by I<gethostbyname(3)>). If
C<addr> is not C<NULL>, the address bound to is stored at that address. If
C<addrsize> is not C<NULL>, the length of C<addr> is stored at that address.
On success, returns the new socket's file descriptor. On error, returns -1
with C<errno> set appropriately.

=cut

*/

typedef struct sockaddr_in sockaddr_in;
#ifdef PF_INET6
typedef struct sockaddr_in6 sockaddr_in6;
#endif

static sockaddr *net_ipaddr(sockaddr_in *ip, size_t family, void *addr, size_t addrsize, port_t port)
{
	memset(ip, '\0', sizeof(sockaddr_in));
	ip->sin_family = family;
	memcpy(&ip->sin_addr, addr, addrsize);
	ip->sin_port = htons(port);
	return (sockaddr *)ip;
}

#ifdef PF_INET6
static sockaddr *net_ip6addr(sockaddr_in6 *ip, size_t family, void *addr, size_t addrsize, port_t port)
{
	memset(ip, '\0', sizeof(sockaddr_in6));
	ip->sin6_family = family;
	memcpy(&ip->sin6_addr, addr, addrsize);
	ip->sin6_port = htons(port);
	return (sockaddr *)ip;
}
#endif

int net_create_server(const char *iface, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize)
{
	int sockfd;
	sockaddr *ipaddr;
	size_t ipsize;
	struct hostent *host;
#ifdef PF_INET6
	union { sockaddr_in v4; sockaddr_in6 v6; } ip;
#else
	union { sockaddr_in v4; } ip;
#endif

	/* Set ipaddr and ipsize to the specified interface, or any */

	if (iface)
	{
		if (!(host = gethostbyname(iface)))
			return set_errno(ENOENT);

		if (host->h_addrtype == PF_INET && host->h_length == sizeof ip.v4.sin_addr)
		{
			ipaddr = net_ipaddr(&ip.v4, host->h_addrtype, host->h_addr, host->h_length, port);
			ipsize = sizeof ip.v4;
		}
#ifdef PF_INET6
		else if (host->h_addrtype == PF_INET6 && host->h_length == sizeof ip.v6.sin6_addr)
		{
			ipaddr = net_ip6addr(&ip.v6, host->h_addrtype, host->h_addr, host->h_length, port);
			ipsize = sizeof ip.v6;
		}
#endif
		else
			return set_errno(ENOSYS);
	}
	else
	{
		unsigned long any = htonl(INADDR_ANY);
		ipaddr = net_ipaddr(&ip.v4, PF_INET, &any, sizeof any, port);
		ipsize = sizeof ip.v4;
	}

	/* Create the socket and bind to ipaddr. If connection oriented, listen */

	if ((sockfd = socket(ipaddr->sa_family, type, protocol)) == -1)
		return -1;

	if (bind(sockfd, ipaddr, ipsize) == -1)
		return -1;

	switch (type)
	{
		case SOCK_STREAM: case SOCK_SEQPACKET:
			if (listen(sockfd, 5) == -1)
				return -1;
	}

	/* Return sockfd, ipaddr and ipsize */

	if (addr && addrsize && *addrsize >= ipsize)
		memcpy(addr, ipaddr, ipsize);

	if (addrsize)
		*addrsize = ipsize;

	return sockfd;
}

/*

=item C<int net_create_client(const char *host, port_t port, int type, sockaddr *addr, size_t *addrsize)>

Creates a client socket of the specified C<type> (e.g. SOCK_STREAM or
SOCK_DGRAM) and C<proto> (usually zero) and connects to the server listening
on C<host:port> (as determined by I<gethostbyname(3)>). If C<addr> is not
C<NULL>, the address of the server is stored at that address. If C<addrsize>
is not C<NULL>, the length of C<addr> is stored at that address. On success,
returns the new socket's file descriptor. On error, returns -1 with C<errno>
set appropriately.

=cut

*/

int net_create_client(const char *host, port_t port, int type, int protocol, sockaddr *addr, size_t *addrsize)
{
	int sockfd;
	sockaddr *ipaddr;
	size_t ipsize;
	struct hostent *h;
#ifdef PF_INET6
	union { sockaddr_in v4; sockaddr_in6 v6; } ip;
#else
	union { sockaddr_in v4; } ip;
#endif

	if (!host)
		return set_errno(EINVAL);

	/* Set ipaddr and ipsize to the specified host address */

	if (!(h = gethostbyname(host)))
		return set_errno(ENOENT);

	if (h->h_addrtype == PF_INET && h->h_length == sizeof ip.v4.sin_addr)
	{
		ipaddr = net_ipaddr(&ip.v4, h->h_addrtype,  h->h_addr, h->h_length, port);
		ipsize = sizeof ip.v4;
	}
#ifdef PF_INET6
	else if (h->h_addrtype == PF_INET6 && h->h_length == sizeof ip.v6.sin6_addr)
	{
		ipaddr = net_ip6addr(&ip.v6, h->h_addrtype,  h->h_addr, h->h_length, port);
		ipsize = sizeof ip.v6;
	}
#endif
	else
		return set_errno(ENOSYS);

	/* Create the socket. If connectionless, bind. Then connect to ipaddr */

	if ((sockfd = socket(ipaddr->sa_family, type, protocol)) == -1)
		return -1;

	if (type == SOCK_DGRAM) /* This isn't really needed for UDP */
	{
		sockaddr_in localip;
		unsigned long any = htonl(INADDR_ANY);
		net_ipaddr(&localip, PF_INET, &any, sizeof any, 0);

		if (bind(sockfd, (sockaddr *)&localip, sizeof localip) == -1)
			return -1;
	}

	if (connect(sockfd, ipaddr, ipsize) == -1)
		return -1;

	/* Return sockfd, ipaddr and ipsize */

	if (addr && addrsize && *addrsize >= ipsize)
		memcpy(addr, ipaddr, ipsize);

	if (addrsize)
		*addrsize = ipsize;

	return sockfd;
}

/*

=item C<ssize_t net_read(int sockfd, const char *buf, size_t count)>

Repeatedly calls I<read(2)> on the connection oriented socket, C<sockfd>,
until C<count> bytes have been read into C<buf> or until EOF is encountered.
On success, returns the number of bytes read. On error, returns -1 with
C<errno> set appropriately.

=cut

*/

ssize_t net_read(int sockfd, char *buf, size_t count)
{
	char *b;
	ssize_t bytes;

	for (b = buf; count; count -= bytes, b += bytes)
	{
		bytes = read(sockfd, b, count);

		if (bytes == -1)
			return -1;

		if (bytes == 0)
			break;
	}

	return b - buf;
}

/*

=item C<ssize_t net_write(int sockfd, const char *buf, size_t count)>

Repeatedly calls I<write(2)> on the connection oriented socket, C<sockfd>,
until C<count> bytes from C<buf> have been written. On success, returns the
number of bytes written. On error, returns -1.

=cut

*/

ssize_t net_write(int sockfd, const char *buf, size_t count)
{
	const char *b;
	ssize_t bytes;

	for (b = buf; count; count -= bytes, b += bytes)
	{
		bytes = write(sockfd, b, count);

		if (bytes <= 0)
			return bytes;
	}

	return b - buf;
}

/*

=item C<ssize_t net_expect(int sockfd, size_t timeout, const char *fmt, ...)>

Expects and confirms a formatted text message from a remote connection on the
socket, C<sockfd>. C<timeout> is the number of seconds to wait before timing
out. If C<timeout> is 0, waits forever. On success, returns the number of
conversions performed (see I<scanf(3)>. When the connection closes, returns 0.
On error, returns -1 with C<errno> set appropriately. B<Note:> Interferes
with other calls to I<alarm(2)> and I<setitimer(3)>.

=cut

*/

ssize_t net_expect(int sockfd, size_t timeout, const char *fmt, ...)
{
	va_list args;
	ssize_t rc;

	va_start(args, fmt);
	rc = net_vexpect(sockfd, timeout, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t net_vexpect(int sockfd, size_t timeout, const char *fmt, va_list args)>

Equivalent to I<net_expect()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

ssize_t net_vexpect(int sockfd, size_t timeout, const char *fmt, va_list args)
{
	char buf[MSG_SIZE];
	ssize_t len;

	if (timeout)
		alarm(timeout); /* XXX don't break existing timers */

	len = read(sockfd, buf, MSG_SIZE);

	if (timeout)
		alarm(0);

	if (len < 1)
		return len;

	buf[len] = '\0';

	return vsscanf(buf, fmt, args);
}

/*

=item C<ssize_t net_send(int sockfd, const char *fmt, ...)>

Sends a formatted string to a remote connection on the socket, C<sockfd>.
On success, returns the number of bytes written. On error, returns -1
with C<errno> set appropriately.

=cut

*/

ssize_t net_send(int sockfd, const char *fmt, ...)
{
	va_list args;
	ssize_t rc;

	va_start(args, fmt);
	rc = net_vsend(sockfd, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t net_vsend(int sockfd, const char *fmt, va_list args)>

Equivalent to I<net_send()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

ssize_t net_vsend(int sockfd, const char *fmt, va_list args)
{
	char buf[MSG_SIZE];
	int len;

	len = vsnprintf(buf, MSG_SIZE, fmt, args);
	if (len == -1 || len >= MSG_SIZE)
		return set_errno(ENOSPC);

	return write(sockfd, buf, len);
}

/*

=item C<int net_chat(int fd, const char *script)>

Process a chat-like C<script> string on the file descriptor C<fd>. This
function and the following documentation are taken from I<UNIX System V
Network Programming> by Stephen A. Rago. I doubt that this can be useful for
sockets (more useful for serial lines) but it belongs in the I<net> module
if anywhere.

A chat script defines a conversation between the local and remote systems in
the following way. Expected input strings (called I<expect strings>) are
separated from resulting output strings (called I<send strings>) by white
space. The local process reads data until the expect string is received.
Then the local process transmits the send string, usually followed by a
carriage return. The carriage return can be suppressed by terminating the
string with pattern C<\c>. This continues until the end of the chat script
has been reached.

The expect strings only need to describe a portion of the desired input. For
example, when expecting a password prompt, it is sufficient to use C<word:>
for the expect string. This increases the flexibility of the script because
it will match C<Password:>, C<password:>, C<Enter password:> and any other
string ending in C<word:>.

An expect string can optionally contain a subsend-subexpect pair of strings.
If a I<read> times out, the subsend string will be transmitted and the
subexpect string will become the new expect string. If this string is not
received, then the conversation has failed. The subsend and subexpect
strings are separated by dashes. The string C<in:--in:> means the expect
string C<in:>, but if it is not received, transmit nothing except a carriage
return, and expect C<in:> again.

There are several special characters that can be used in the chat script to
describe the expect and send strings:

    ""      Expect the null string
    \b      Backspace
    \c      Do not send a carriage return after the string
    \d      Delay for one second
    \K      Insert a break
    \n      Send a newline
    \N      Send a nul
    \p      Pause for less than one second
    \r      Send a carriage return
    \s      Send a space
    \t      Send a tab
    \\      Send a \
    \ooo    Send the ASCII character whose value is the octal number ooo

On successful completion of the conversation, returns 0. On error, returns -1.

=cut

*/

static void nop(int signo)
{}

static int doparse(char **script, char **exp, char **snd, char **subexp, char **subsnd)
{
	char *s, *p;
	int first;

	*exp = *snd = *subexp = *subsnd = NULL;

	for (s = *script; isspace((int)*s); ++s)
	{}

	if (*s == '\0')
		return 0;

	for (*exp = s; *s && !isspace((int)*s); ++s)
	{}

	if (*s == '\0')
		*script = s;
	else
		*script = s + 1, *s = '\0'; 

	first = 1;

	for (p = *exp; *p; ++p)
	{
		if (*p == '-')
		{
			*p = '\0';
			if (first)
				*subsnd = p + 1, first = 0;
			else
				*subexp = p + 1;
		}
	}

	if (*++s == '\0')
		return 1;

	for (*snd = s; *s && !isspace((int)*s); ++s)
	{}

	if (*s == '\0')
		*script = s;
	else
		*script = s + 1, *s = '\0';

	return 1;
}

static int doexpect(int fd, const char *s)
{
	int i, n, len;
	char buf[MSG_SIZE];

	len = strlen(s);

	if (len == 0)
		return 1;

	if (len == 2 && s[0] == '"' && s[1] == '"')
		return 1;

	errno = 0;
	alarm(30);

	for (n = 0, i = 0; i < MSG_SIZE; ++i)
	{
		if (read(fd, buf + i, 1) != 1)
		{
			alarm(0);
			return 0;
		}

		if (i < len - 1)
			continue;

		if (strncmp(s, buf + n, len) == 0)
		{
			alarm(0);
			return 1;
		}

		++n;

		if (errno == EINTR)
			return 0;
	}

	alarm(0);

	return 0;
}

#define SEND(c) if (write(fd, (c), 1) != 1) return -1

static int dosend(int fd, const char *s)
{
	int docr = 1;
	static struct timeval pause[1] = {{ 0, 300 }};

	for (; *s; ++s)
	{
		switch (*s)
		{
			case '\\':
				switch (*++s)
				{
					case 'b': SEND("\b"); break;
					case 'c': if (s[1] == '\0') docr = 0; break;
					case 'd': sleep(1); break;
					case 'K': tcsendbreak(fd, 0); break;
					case 'n': SEND("\n"); break;
					case 'N': SEND("\0"); break;
					case 'p': select(0, NULL, NULL, NULL, pause);
					case 'r': SEND("\r"); break;
					case 's': SEND(" "); break;
					case 't': SEND("\t"); break;
					case '0': case '1': case '2': case '3':
						if (s[1] >= '0' && s[1] <= '7' && s[2] >= '0' && s[2] <= '7')
						{
							unsigned char n;
							n = (*s++ - '0') << 6;
							n |= (*s++ - '0') << 3;
							n |= (*s - '0');
							SEND(&n);
						}
					default: SEND(s); break;
				}
				break;

			default:
				SEND(s);
				break;
		}
	}

	if (docr)
		SEND("\r");

	return 0;
}

#undef SEND

static int dochat(int fd, char *script)
{
	char *exp, *snd, *subexp, *subsnd;

	while (doparse(&script, &exp, &snd, &subexp, &subsnd))
	{
		if (doexpect(fd, exp))
		{
			if (dosend(fd, snd) == -1)
				return -1;
		}
		else if (!subexp)
			return -1;

		if (dosend(fd, subsnd) == -1)
			return -1;

		if (doexpect(fd, subexp) == 0)
			return -1;

		if (dosend(fd, snd) == -1)
			return -1;
	}

	return 0;
}

static int before_alarm(struct sigaction *sa, struct sigaction *osa)
{
	memset(sa, '\0', sizeof(struct sigaction));
	sigemptyset(&sa->sa_mask);
	sa->sa_handler = nop;
	sa->sa_flags = 0;

	return sigaction(SIGALRM, sa, osa);
}

static int after_alarm(struct sigaction *osa)
{
	return sigaction(SIGALRM, osa, NULL);
}

int net_chat(int fd, char *script)
{
	struct sigaction sa[1], osa[1];
	int rc;

	if (!script)
		return set_errno(EINVAL);

	if (before_alarm(sa, osa) == -1)
		return -1;

	rc = dochat(fd, script);

	if (after_alarm(osa) == -1)
		return -1;

	return rc;
}

/*

=item C<ssize_t net_pack(int sockfd, int flags, const char *fmt, ...)>

Creates a packet containing data packed by I<pack()> as specified
by C<fmt> and sends it on the connected socket, C<sockfd>, with
I<send(2)>. C<flags> is passed to I<send(2)>. On success, returns
the number of bytes packed and sent. On error, returns -1 with
C<errno> set appropriately.

=cut

*/

ssize_t net_pack(int sockfd, int flags, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = net_vpack(sockfd, flags, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t net_vpack(int sockfd, int flags, const char *fmt, va_list args)>

Equivalent to I<net_pack()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

ssize_t net_vpack(int sockfd, int flags, const char *fmt, va_list args)
{
	char buf[MSG_SIZE];
	int rc;

	if ((rc = vpack(buf, MSG_SIZE, fmt, args)) == -1)
		return -1;

	return send(sockfd, buf, rc, flags);
}

/*

=item C<ssize_t net_packto(int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, ...)>

Creates a packet containing data packed by I<pack()> as specified by C<fmt>
and sends it on the unconnected socket, C<sockfd>, to the address specified
by C<to> with length C<tolen> with I<sendto(2)>. I<flags> is passed to
I<sendto(2)>. On success, returns the number of bytes packed and sent. On
error, returns -1 with C<errno> set appropriately.

=cut

*/

ssize_t net_packto(int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = net_vpackto(sockfd, flags, to, tolen, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t net_vpackto(int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, va_list args)>

Equivalent to I<net_packto()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

ssize_t net_vpackto(int sockfd, int flags, const sockaddr *to, size_t tolen, const char *fmt, va_list args)
{
	char buf[MSG_SIZE];
	int rc;

	if ((rc = vpack(buf, MSG_SIZE, fmt, args)) == -1)
		return -1;

	return sendto(sockfd, buf, rc, flags, to, tolen);
}

/*

=item C<ssize_t net_unpack(int sockfd, int flags, const char *fmt, ...)>

Receives a packet of data on the connected socket, C<sockfd>, with
I<recv(2)>, and unpacks it with I<unpack()> as specified by C<fmt>. I<flags>
is passed to I<recv(2)>. On success, returns the number of bytes received
and unpacked. On error, returns -1 with C<errno> set appropriately.

=cut

*/

ssize_t net_unpack(int sockfd, int flags, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = net_vunpack(sockfd, flags, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t net_vunpack(int sockfd, int flags, const char *fmt, va_list args)>

Equivalent to I<net_unpack()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

ssize_t net_vunpack(int sockfd, int flags, const char *fmt, va_list args)
{
	char buf[MSG_SIZE];
	int rc;

	if ((rc = recv(sockfd, buf, MSG_SIZE, flags)) == -1)
		return -1;

	return vunpack(buf, rc, fmt, args);
}

/*

=item C<ssize_t net_unpackfrom(int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, ...)>

Receives a packet of data on the unconnected socket, C<sockfd>, with
I<recvfrom(2)>, and unpacks it with I<unpack()> as specified by C<fmt>. If
C<from> is non-C<NULL>, the source address of the message is stored there.
C<fromlen> is a value-result parameter, initialized to the size of the
C<from> buffer, and modified on return to indicate the actual size of the
address stored there. I<flags> is passed to I<recvfrom(2)>. On success,
returns the number of bytes received and unpacked. On error, returns -1 with
C<errno> set appropriately.

=cut

*/

ssize_t net_unpackfrom(int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = net_vunpackfrom(sockfd, flags, from, fromlen, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t net_vunpackfrom(int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, va_list args)>

Equivalent to I<net_unpackfrom()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

ssize_t net_vunpackfrom(int sockfd, int flags, sockaddr *from, size_t *fromlen, const char *fmt, va_list args)
{
	char buf[MSG_SIZE];
	int rc;

	if ((rc = recvfrom(sockfd, buf, MSG_SIZE, flags, from, fromlen)) == -1)
		return -1;

	return vunpack(buf, rc, fmt, args);
}

/*

=item C<ssize_t pack(void *buf, size_t size, const char *fmt, ...)>

Packs data into C<buf> as described by C<fmt>. The arguments after C<fmt>
contain the data to be packed. C<size> is the size of C<buf>. Returns the
number of bytes packed on success, or -1 on error with C<errno> set
appropriately.

Note, this is based on the I<pack()> function in I<perl(1)> (in fact, the
following documentation is from I<perlfunc(1)>) except that the C<*> count
specifier has different semantics, there's no non nul-terminated strings or
machine dependant formats or uuencoding or BER integer compression,
everything is in network byte order, and floats are represented as strings
so I<pack()> is suitable for serialising data to be written to disk or sent
across a network to other hosts. OK, C<v> and C<w> specifically aren't in
network order but sometimes that's needed to.

C<fmt> can contain the following type specifiers:

    a   A string with arbitrary binary data
    z   A nul terminated string, will be nul padded
    b   A bit string (rounded out to nearest byte boundary)
    h   A hex string (rounded out to nearest byte boundary)
    c   A char (8 bits)
    s   A short (16 bits)
    i   An int (32 bits)
    l   A long (64 bits - only on some systems)
    f   A single-precision float (length byte + text + nul)
    d   A double-precision float (length byte + text + nul)
    v   A short in "VAX" (little-endian) order (16 bits)
    w   An int in "VAX" (little-endian) order (32 bits)
    p   A pointer (32 bits)
    x   A nul byte
    X   Back up a byte
    @   Null fill to absolute position

The following rules apply:

Each letter may optionally be followed by a number giving a repeat count or
length. With all types except C<"a">, C<"z">, C<"b"> and C<"h"> the
I<pack()> function will gobble up that many arguments. Unfortunately,
you can't use C<"*"> to gobble up all remaining items like you can in
I<perl(1)>. However, you can use C<"*"> to obtain the count from the
argument list (like I<printf(3)>). The count argument must appear before the
first corresponding data argument.

The C<"a"> and C<"z"> types gobble just one value, but pack it as a string
of length count (specified by the corresponding number), truncating or
padding with nuls as necessary. It is the caller's responsibility to ensure
that the data arguments point to sufficient memory. When unpacking, C<"z">
strips everything after the first nul, and C<"a"> returns data verbatim.

Likewise, the C<"b"> field packs a string that many bits long.

The C<"h"> field packs a string that many nybbles long.

The C<"p"> type packs a pointer. You are responsible for ensuring the memory
pointed to is not a temporary value (which can potentially get deallocated
before you get around to using the packed result). A C<NULL> pointer is
created if the corresponding value for C<"p"> is C<NULL>. Of course, C<"p">
is useless if the packed data is to be sent over a network to another
process.

The integer formats C<"c">, C<"s">, C<"i"> and C<"l"> are all on network
byte order and so can safely be packed for sending over a network to another
process. However, C<"l"> rely on a non-ANSI C language feature (namely, the
C<long long int> type) and so should never be used in portable code, even if
it is supported on the local system. There is no guarantee that a long long
packed on one system will be unpackable on another.

Real numbers (floats and doubles) are packed in text format. Due to the
multiplicity of floating formats around, this is done to safely transport
real numbers across a network to another process.

It is the caller's responsibility to ensure that there are sufficient
arguments provided to satisfy the requirements of C<fmt>.

=cut

*/

ssize_t pack(void *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = vpack(buf, size, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t vpack(void *buf, size_t size, const char *fmt, va_list args)>

Equivalent to I<pack()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

#define GET_COUNT() \
	count = 1; \
	if (*fmt == '*') \
		++fmt, count = va_arg(args, size_t); \
	else if (isdigit((int)*fmt))\
		for (count = 0; isdigit((int)*fmt); ++fmt) \
			count *= 10, count += *fmt - '0'; \
	if ((ssize_t)count < 1) \
		return set_errno(EINVAL);

#define CHECK_SPACE(required) \
	if (p + (required) > pkt + size) \
		return set_errno(ENOSPC);

ssize_t vpack(void *buf, size_t size, const char *fmt, va_list args)
{
	size_t count;
	unsigned char *pkt = buf;
	unsigned char *p = pkt;
	char tmp[128];

	if (!pkt || !fmt)
		return set_errno(EINVAL);

	while (*fmt)
	{
		switch (*fmt++)
		{
			case 'a': /* A string with arbitrary binary data */
			{
				void *data;
				GET_COUNT()
				CHECK_SPACE(count)
				data = va_arg(args, void *);
				if (!data)
					return set_errno(EINVAL);
				memcpy(p, data, count);
				p += count;
				break;
			}

			case 'z': /* A nul terminated string, will be nul padded */
			{
				char *data;
				size_t len;
				GET_COUNT()
				CHECK_SPACE(count)
				data = va_arg(args, char *);
				if (!data)
					return set_errno(EINVAL);
				len = strlen(data);
				if (len > count)
					len = count;
				memcpy(p, data, len);
				p += len;
				count -= len;
				if (count)
					memset(p, '\0', count);
				p += count;
				break;
			}

			case 'b': /* A bit string (rounded out to nearest byte boundary) */
			{
				char *data;
				unsigned char byte;
				int shift;
				GET_COUNT()
				CHECK_SPACE((count + 7) >> 3)
				data = va_arg(args, char *);
				if (!data)
					return set_errno(EINVAL);
				byte = 0x00;
				shift = 7;
				while (count--)
				{
					switch (*data++)
					{
						case '0':
							break;
						case '1':
							byte |= 1 << shift;
							break;
						default:
							return set_errno(EINVAL);
					}
					if (--shift == -1)
					{
						*p++ = byte;
						byte = 0x00;
						shift = 7;
					}
				}
				if (shift != 7)
					*p++ = byte;
				break;
			}

			case 'h': /* A hex string (rounded out to nearest byte boundary) */
			{
				char *data;
				unsigned char byte;
				int shift;
				GET_COUNT()
				CHECK_SPACE((count + 1) >> 1)
				data = va_arg(args, char *);
				if (!data)
					return set_errno(EINVAL);
				byte = 0x00;
				shift = 4;
				while (count--)
				{
					unsigned char nybble = *data++;
					switch (nybble)
					{
						case '0': case '1': case '2': case '3': case '4':
						case '5': case '6': case '7': case '8': case '9':
							byte |= (nybble - '0') << shift;
							break;
						case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
							byte |= (nybble - 'a' + 10) << shift;
							break;
						case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
							byte |= (nybble - 'A' + 10) << shift;
							break;
						default:
							return set_errno(EINVAL);
					}
					if ((shift -= 4) == -4)
					{
						*p++ = byte;
						byte = 0x00;
						shift = 4;
					}
				}
				if (shift != 4)
					*p++ = byte;
				break;
			}

			case 'c': /* A char (8 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count)
				while (count--)
					*p++ = (unsigned char)va_arg(args, int);
				break;
			}

			case 's': /* A short (16 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 1)
				while (count--)
				{
					unsigned short data = (unsigned short)va_arg(args, int);
					*p++ = (data >> 8) & 0xff;
					*p++ = data & 0xff;
				}

				break;
			}

			case 'i': /* An int (32 bits) */
			case 'p': /* A pointer (32 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 2)
				while (count--)
				{
					unsigned long data = (unsigned long)va_arg(args, int);
					*p++ = (data >> 24) & 0xff;
					*p++ = (data >> 16) & 0xff;
					*p++ = (data >> 8) & 0xff;
					*p++ = data & 0xff;
				}

				break;
			}

#ifdef HAS_LONG_LONG
			case 'l': /* A long (64 bits - only on some systems) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 3)
				while (count--)
				{
					unsigned long long data = (unsigned long long)va_arg(args, long long);
					*p++ = (data >> 56) & 0xff;
					*p++ = (data >> 48) & 0xff;
					*p++ = (data >> 40) & 0xff;
					*p++ = (data >> 32) & 0xff;
					*p++ = (data >> 24) & 0xff;
					*p++ = (data >> 16) & 0xff;
					*p++ = (data >> 8) & 0xff;
					*p++ = data & 0xff;
				}

				break;
			}
#else
			case 'l': /* A long (64 bits - only on some systems) */
			{
				return set_errno(ENOSYS);
			}
#endif

			case 'f': /* A single-precision float (length byte + text + nul) */
			case 'd': /* A double-precision float (length byte + text + nul) */
			{
				GET_COUNT()
				while (count--)
				{
					double data = va_arg(args, double);
					int rc = snprintf(tmp, 128, "%g", data);
					size_t len;
					if (rc == -1 || rc >= 128)
						return set_errno(ENOSPC);
					len = strlen(tmp) + 1;
					CHECK_SPACE(len + 1)
					*p++ = len & 0xff;
					memcpy(p, tmp, len);
					p += len;
				}

				break;
			}

			case 'v': /* A short in "VAX" (little-endian) order (16 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 1)
				while (count--)
				{
					unsigned short data = (unsigned short)va_arg(args, int);
					*p++ = data & 0xff;
					*p++ = (data >> 8) & 0xff;
				}

				break;
			}

			case 'w': /* An int in "VAX" (little-endian) order (32 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 2)
				while (count--)
				{
					unsigned int data = (unsigned int)va_arg(args, int);
					*p++ = data & 0xff;
					*p++ = (data >> 8) & 0xff;
					*p++ = (data >> 16) & 0xff;
					*p++ = (data >> 24) & 0xff;
				}

				break;
			}

			case 'x': /* A nul byte */
			{
				GET_COUNT()
				CHECK_SPACE(count)
				memset(p, '\0', count);
				p += count;
				break;
			}

			case 'X': /* Back up a byte */
			{
				GET_COUNT()
				if (p - count < pkt)
					return set_errno(EINVAL);
				p -= count;
				break;
			}

			case '@': /* Null fill to absolute position */
			{
				GET_COUNT()
				if (count > size)
					return set_errno(ENOSPC);
				if (pkt + count < p)
					return set_errno(EINVAL);
				memset(p, '\0', count - (p - pkt));
				p += count - (p - pkt);
				break;
			}

			default:
			{
				return set_errno(EINVAL);
			}
		}
	}

	return p - pkt;
}

/*

=item C<ssize_t unpack(void *buf, size_t size, const char *fmt, ...)>

Unpacks the data in C<buf> which was packed by I<pack()>. C<size> is the
size of C<buf>. C<fmt> must be equivalent to the C<fmt> argument to the call
to I<pack()> that packed the data. The remaining arguments must be pointers
to variables that will hold the unpacked data or C<NULL>. If any are C<NULL>
the corresponding data will be skipped (i.e. not unpacked). Unpacked C<"z">,
C<"b"> and C<"h"> strings are always nul terminated. It is the caller's
responsibility to ensure that the pointers into which these strings are
unpacked contain enough memory (count + 1 bytes). It is the caller's
responsibility to ensure that the non-C<NULL> pointers into which C<"a">
strings are unpacked also contain enough memory (count bytes). It is the
caller's responsibility to ensure that there are sufficient arguments
supplied to satisfy the requirements of C<fmt>, even if they are just
C<NULL> pointers. Returns the number of bytes unpacked on success, or -1 on
error.

=cut

*/

ssize_t unpack(void *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = vunpack(buf, size, fmt, args);
	va_end(args);

	return rc;
}

/*

=item C<ssize_t vunpack(void *buf, size_t size, va_list args)>

Equivalent to I<unpack()> with the variable argument list specified
directly as for I<vprintf(3)>.

=cut

*/

#define CHECK_SKIP(count, action) \
	if (!data) \
	{ \
		p += (count); \
		action; \
	}

ssize_t vunpack(void *buf, size_t size, const char *fmt, va_list args)
{
	unsigned char *pkt = buf;
	unsigned char *p = pkt;
	size_t count;

	if (!pkt || !fmt)
		return set_errno(EINVAL);

	while (*fmt)
	{
		switch (*fmt++)
		{
			case 'a': /* A string with arbitrary binary data */
			{
				void *data;
				GET_COUNT()
				CHECK_SPACE(count)
				data = va_arg(args, void *);
				CHECK_SKIP(count, break)
				memcpy(data, p, count);
				p += count;
				break;
			}

			case 'z': /* A nul terminated string, will be nul padded */
			{
				char *data;
				size_t len;
				GET_COUNT()
				CHECK_SPACE(count)
				data = va_arg(args, char *);
				CHECK_SKIP(count, break)
				for (len = 0; p + len < pkt + size && p[len]; ++len)
					;
				if (len > count)
					len = count;
				memcpy(data, p, len);
				p += len;
				count -= len;
				memset(data + len, '\0', count ? count : 1);
				p += count;
				break;
			}

			case 'b': /* A bit string (rounded out to nearest byte boundary) */
			{
				char bin[] = "01";
				char *data;
				int shift;
				GET_COUNT()
				CHECK_SPACE((count + 7) >> 3)
				data = va_arg(args, char *);
				CHECK_SKIP((count + 7) >> 3, break)
				shift = 7;
				while (count--)
				{
					*data++ = bin[(*p & (0x01 << shift)) >> shift];
					if (--shift == -1)
						++p, shift = 7;
				}
				if (shift != 7)
					++p;
				*data = '\0';
				break;
			}

			case 'h': /* A hex string (rounded out to nearest byte boundary) */
			{
				char hex[] = "0123456789abcdef";
				char *data;
				int shift;
				GET_COUNT()
				CHECK_SPACE((count + 1) >> 1)
				data = va_arg(args, char *);
				CHECK_SKIP((count + 1) >> 1, break)
				shift = 4;
				while (count--)
				{
					*data++ = hex[(*p & (0x0f << shift)) >> shift];
					if ((shift -= 4) == -4)
						++p, shift = 4;
				}
				if (shift != 4)
					++p;
				*data = '\0';
				break;
			}

			case 'c': /* A char (8 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count)
				while (count--)
				{
					signed char *data = va_arg(args, signed char *);
					CHECK_SKIP(1, continue)
					*data = (signed char)*p++;
				}
				break;
			}

			case 's': /* A short (16 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 1)
				while (count--)
				{
					signed short *data = va_arg(args, signed short *);
					CHECK_SKIP(2, continue)
					*data = (signed short)*p++ << 8;
					*data |= *p++;
				}
				break;
			}

			case 'i': /* An int (32 bits) */
			case 'p': /* A pointer (32 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 2)
				while (count--)
				{
					signed long *data = va_arg(args, signed long *);
					CHECK_SKIP(4, continue)
					*data = (signed long)*p++ << 24;
					*data |= (signed long)*p++ << 16;
					*data |= (signed long)*p++ << 8;
					*data |= (signed long)*p++;
				}

				break;
			}

			case 'v': /* A short in "VAX" (little-endian) order (16 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 1)
				while (count--)
				{
					signed short *data = va_arg(args, signed short *);
					CHECK_SKIP(2, continue)
					*data = *p++;
					*data |= (unsigned short)*p++ << 8;
				}
				break;
			}

			case 'w': /* An int in "VAX" (little-endian) order (32 bits) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 2)
				while (count--)
				{
					signed int *data = va_arg(args, signed int *);
					CHECK_SKIP(4, continue)
					*data = (signed long)*p++;
					*data |= (signed long)*p++ << 8;
					*data |= (signed long)*p++ << 16;
					*data |= (signed long)*p++ << 24;
				}

				break;
			}

#ifdef HAS_LONG_LONG
			case 'l': /* A long (64 bits - only on some systems) */
			{
				GET_COUNT()
				CHECK_SPACE(count << 3)
				while (count--)
				{
					signed long long *data = va_arg(args, signed long long *);
					CHECK_SKIP(8, continue)
					*data = (signed long long)*p++ << 56;
					*data |= (signed long long)*p++ << 48;
					*data |= (signed long long)*p++ << 40;
					*data |= (signed long long)*p++ << 32;
					*data |= (signed long long)*p++ << 24;
					*data |= (signed long long)*p++ << 16;
					*data |= (signed long long)*p++ << 8;
					*data |= (signed long long)*p++;
				}

				break;
			}
#else
			case 'l':
			{
				return set_errno(ENOSYS);
			}
#endif

			case 'f': /* A single-precision float (length byte + text + nul) */
			{
				GET_COUNT()
				while (count--)
				{
					float *data = va_arg(args, float *);
					size_t len;
					CHECK_SPACE(1);
					len = (size_t)*p++;
					CHECK_SPACE(len)
					CHECK_SKIP(len, continue)
					sscanf((const char *)p, "%g", data);
					p += len;
				}

				break;
			}

			case 'd': /* A double-precision float (length byte + text + nul) */
			{
				GET_COUNT()
				while (count--)
				{
					double *data = va_arg(args, double *);
					size_t len;
					CHECK_SPACE(1);
					len = (size_t)*p++;
					CHECK_SPACE(len);
					CHECK_SKIP(len, continue)
					sscanf((const char *)p, "%lg", data);
					p += len;
				}

				break;
			}

			case 'x': /* A nul byte */
			{
				GET_COUNT()
				CHECK_SPACE(count)
				p += count;
				break;
			}

			case 'X': /* Back up a byte */
			{
				GET_COUNT()
				if (p - count < pkt)
					return set_errno(EINVAL);
				p -= count;
				break;
			}

			case '@': /* Null fill to absolute position */
			{
				GET_COUNT()
				if (count > size)
					return set_errno(ENOSPC);
				if (pkt + count < p)
					return set_errno(EINVAL);
				p += count - (p - pkt);
				break;
			}

			default:
			{
				return set_errno(EINVAL);
			}
		}
	}

	return p - pkt;
}

/*

=item C<int mail(const char *server, const char *sender, const char *recipients, const char *subject, const char *message)>

Sends a mail message consisting of C<subject> and C<message> from C<sender>
to the addresses in C<recipients>. C<recipients> contains mail addresses
separated by sequences of comma and/or space characters. C<message> must not
contain any lines containing only a C<``.''> character. On success, returns
0. On error, returns -1 with C<errno> set appropriately.

=cut

*/

static int rcpt(int smtp, const char *recipients)
{
	List *list = split(recipients, ", ");

	if (!list || !list_length(list))
		return -1;

	while (list_has_next(list))
	{
		char *recipient = cstr((String *)list_next(list));
		int rc, code;

		if (net_send(smtp, "RCPT TO: <%s>\r\n", recipient) == -1 ||
			(rc = net_expect(smtp, 10, "%d", &code)) == -1)
		{
			list_release(list);
			return -1;
		}
		
		if (rc != 1 || code != 250)
		{
			list_release(list);
			return set_errno(EPROTO);
		}
	}

	list_release(list);
	return 0;
}

int mail(const char *server, const char *sender, const char *recipients, const char *subject, const char *message)
{
	int smtp;
	int code;
	int rc;
	char c;

	if (!sender || !recipients)
		return set_errno(EINVAL);

	smtp = net_client(server ? server : "localhost", 25, NULL, NULL);
	if (smtp == -1)
		return -1;

#define fail { close(smtp); return -1; }
#define try(action) if ((action) == -1) fail
#define try_send(args) try(net_send args)
#define try_expect(args, cnv, resp) try(rc = net_expect args) \
	if (rc != (cnv) || code != (resp)) { close(smtp); return set_errno(EPROTO); }

	try_expect((smtp, 10, "%d%c", &code, &c), 2, 220)
	while (c == '-')
		try_expect((smtp, 10, "%d%c", &code, &c), 2, 220)

	try_send((smtp, "HELO localhost\r\n"))
	try_expect((smtp, 10, "%d", &code), 1, 250)
	try_send((smtp, "MAIL FROM: <%s>\r\n", sender))
	try_expect((smtp, 10, "%d", &code), 1, 250)
	try(rcpt(smtp, recipients))
	try_send((smtp, "DATA\r\n"))
	try_expect((smtp, 10, "%d", &code), 1, 354)
	try_send((smtp, "From: %s\r\n", sender))
	try_send((smtp, "To: %s\r\n", recipients))
	try_send((smtp, "Subject: %s\r\n\r\n", (subject) ? subject : ""))
	try_send((smtp, "%s\r\n.\r\n", (message) ? message : ""))
	try_expect((smtp, 10, "%d", &code), 1, 250)
	try_send((smtp, "QUIT\r\n"))
	try_expect((smtp, 10, "%d", &code), 1, 221)
	close(smtp);

	return 0;
}

/*

=item C<char *rfc822_localtime(char *buf, size_t max, time_t t)>

Formats the time, C<time>, as local time into C<buf> using the rfc822 date
format (e.g. C<Sun, 06 Nov 1994 08:49:37 +0000>). On success, returns
C<buf>. On error, returns C<NULL>.

=cut

*/

char *rfc822_localtime(char *buf, size_t max, time_t time)
{
	static const char sign[2] = { '+', '-' };
	struct tm tm = *localtime(&time);
	size_t size = strftime(buf, max, "%a, %d %b %Y %H:%M:%S ", &tm);
	if (size == 0)
		return NULL;
	size = snprintf(buf + size, max - size, "%c%04d", sign[timezone > 0], abs(timezone) / 36);
	if (size <= 0 || size >= max)
		return NULL;
	return buf;
}

/*

=item C<char *rfc822_gmtime(char *buf, size_t max, time_t t)>

Formats the time, C<time>, as UTC into C<buf> using the rfc822 date format
(e.g. C<Sun, 06 Nov 1994 08:49:37 GMT>). On success, returns C<buf>. On
error, returns C<NULL>.

=cut

*/

char *rfc822_gmtime(char *buf, size_t max, time_t time)
{
	struct tm tm = *gmtime(&time);
	size_t size = strftime(buf, max, "%a, %d %b %Y %H:%M:%S GMT", &tm);
	if (size == 0)
		return NULL;
	return buf;
}

/*

=back

=head1 ERRORS

These are the errors generated by the functions that return -1 on error.
Additional errors may be generated and returned from the underlying system
calls. See their manual pages.

=over 4

=item ENOENT

I<gethostbyname(3)> failed to identify the C<host> or C<iface> argument
passed to one of the socket functions.

=item ENOSYS

I<gethostbyname(3)> returned an address from an unsupported address family.

The C<"l"> format was used with I<pack()> or I<unpack()> when the
system doesn't support it or it wasn't compiled into I<libslack>.

=item EINVAL

A string argument is C<NULL>.

A pack format count is not a positive integer.

An argument containing C<"a">, C<"z">, C<"b"> or C<"h"> data to be packed is
C<NULL>.

An argument containing C<"b"> data to be packed contains characters outside
the range [01].

An argument containing C<"h"> data to be packed contains characters outside
the range [0-9a-fA-F].

An C<"X"> pack instruction is trying to go back past the start of the
packet.

The count argument to an C<"@"> pack instruction refers to a location before
that where the instruction was encountered (i.e. it's trying to pack leftwards).

The C<fmt> argument to I<pack()> or I<unpack()> contains an illegal
character.

=item ENOSPC

A message was too large to be sent with C<net_send()>.

A packet was too small to store all of the data to be packed or unpacked.

=item EPROTO

I<mail()> encountered an error in the dialogue with the SMTP server. This
most likely cause of this is a missing or inadequate domain name for the
sender address on systems where I<sendmail(8)> requires a real domain name.

=back

=head1 EXAMPLES

A TCP server:

    #include <stdio.h>
    #include <unistd.h>
    #include <slack/net.h>

    void provide_service(int fd) { ... }

    int main()
    {
        int servfd, clntfd;
        if ((servfd = net_server(NULL, 30000, NULL, NULL)) == -1)
            return 1;

        while ((clntfd = accept(servfd, NULL, NULL)) != -1)
        {
            pid_t pid;
            switch (pid = fork())
            {
                case -1: return 1;
                case  0: provide_service(clntfd); _exit(0);
                default: close(clntfd); break;
            }
        }

        return 1;
    }

A TCP client:

    #include <stdio.h>
    #include <unistd.h>
    #include <slack/net.h>

    void request_service(int fd) { ... }

    int main()
    {
        int servfd = net_client("localhost", 30000, NULL, NULL);
        if (servfd == -1)
            return 1;

        request_service(servfd);
        close(servfd);
        return 0;
    }

A UDP server:

    #include <stdio.h>
    #include <slack/net.h>
    #include <netinet/in.h>

    void provide_service(char *pkt) { ... }

    int main()
    {
        char pkt[8];
        struct sockaddr_in ipaddr;
        size_t ipsize = sizeof ipaddr;
        int servfd;
        if ((servfd = net_udp_server(NULL, 30000, NULL, NULL)) == -1)
            return 1;

        for (;;)
        {
            if (recvfrom(servfd, pkt, 8, 0, (sockaddr *)&ipaddr, &ipsize) == -1)
                return 1;

            provide_service(pkt);

            if (sendto(servfd, pkt, 8, 0, (sockaddr *)&ipaddr, ipsize) == -1)
                return 1;
        }
    }

A UDP client:

    #include <stdio.h>
    #include <unistd.h>
    #include <slack/net.h>

    void request_service(char *pkt) { ... }
    void receive_service(char *pkt) { ... }

    int main()
    {
        char pkt[8];
        int servfd = net_udp_client("localhost", 30000, NULL, NULL);
        if (servfd == -1)
            return 1;

        request_service(pkt);

        if (send(servfd, pkt, 8, 0) == -1)
            return 1;

        if (recv(servfd, pkt, 8, 0) == -1)
            return 1;

        receive_service(pkt);

        close(servfd);
        return 0;
    }

Expect/Send SMTP protocol:

    #include <slack/net.h>
    
    int tmail(char *sender, char *recipient, char *subject, char *message)
    {
        int smtp = net_client("localhost", 25, NULL, NULL);
		int code;
        int rc =
            smtp != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 220 &&
            net_send(smtp, "HELO %s\r\n", "localhost") != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 250 &&
            net_send(smtp, "MAIL FROM: <%s>\r\n", sender) != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 250 &&
            net_send(smtp, "RCPT TO: <%s>\r\n", recipient) != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 250 &&
            net_send(smtp, "DATA\n") != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 354 &&
            net_send(smtp, "From: %s\r\n", sender) != -1 &&
            net_send(smtp, "To: %s\r\n", recipient) != -1 &&
            net_send(smtp, "Subject: %s\r\n", subject) != -1 &&
            net_send(smtp, "\n%s\r\n.\r\n", message) != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 250 &&
            net_send(smtp, "QUIT\r\n") != -1 &&
            net_expect(smtp, 10, "%d", &code) == 1 && code == 221;

        if (smtp != -1)
            close(smtp);

        return rc;
    }
    
    int main(int ac, char **av)
    {
        return !tmail("raf@raf.org", "raf@raf.org", "This is a test", "Are you receiving me?\n");
    }

Unpack the size of a gif image:

    unsigned short width, height;
    unpack(gif, 10, "z6v2", NULL, &width, &height);

Pack examples from I<perlfunc(1)>:

    pack(pkt, 4, "cccc", 'A', 'B', 'C', 'D');    // "ABCD"
    pack(pkt, 4, "c4",   'A', 'B', 'C', 'D');    // "ABCD"
    pack(pkt, 6, "ccxxcc", 'A', 'B', 'C', 'D');  // "AB\0\0CD"
    pack(pkt, 4, "s2", 1, 2);                    // "\0\1\0\2"
    pack(pkt, 4, "a4", "abcd", "x", "y", "z");   // "abcd"
    pack(pkt, 4, "aaaa", "abcd", "x", "y", "z"); // "axyz"
    pack(pkt, 14, "z14", "abcdefg");             // "abcdefg\0\0\0\0\0\0\0"

    int bin(const char *bin)
    {
        char pkt[4], data[33];
        size_t binlen;
        int ret;

        binlen = strlen(bin);
        memset(data, '0', 32 - binlen);
        strcpy(data + 32 - binlen, bin);
        pack(pkt, 4, "b32", data);
        unpack(pkt, 4, "i", &ret);
        return ret;
    }

    int hex(const char *hex)
    {
        char pkt[4], data[9];
        size_t hexlen;
        int ret;

        hexlen = strlen(hex);
        memset(data, '0', 8 - hexlen);
        strcpy(data + 8 - hexlen, hex);
        pack(pkt, 4, "h8", data);
        unpack(pkt, 4, "i", &ret);
        return ret;
    }

=head1 BUGS

The server and client functions only support IPv4 addresses (and IPv6
addresses on systems (e.g. Linux) that support IPv6). However, when creating
a server that binds to all local network interfaces, only IPv4 addresses are
supported.

When creating a UDP client socket, you can't specify a specific port number
to I<bind(2)> to. The system automatically assigns one.

The pack functions assume the following: There are 8 bits in a byte. A char
is 1 byte. A short can be stored in 2 bytes. Integers, long integers and
pointers can be stored in 4 bytes. Long long integers can be stored in 8
bytes. If these datatypes are larger on your system, only the least
significant byte(s) will be packed.

Packing long long integers is not portable.

The I<net_expect()> function interferes with other calls to I<alarm(2)> and
I<setitimer(2)>.

The chat function is probably a complete waste of time and may be removed
some day. The timeout is fixed at 30 seconds. It can't abort prematurely. It
gives no feedback until the end of the script has been reached.

=head1 SEE ALSO

L<socket(2)|socket(2)>,
L<bind(2)|bind(2)>,
L<listen(2)|listen(2)>,
L<accept(2)|accept(2)>,
L<connect(2)|connect(2)>,
L<shutdown(2)|shutdown(2)>,
L<select(2)|select(2)>,
L<read(2)|read(2)>,
L<write(2)|write(2)>,
L<close(2)|close(2)>,
L<send(2)|send(2)>,
L<sendto(2)|sendto(2)>,
L<recv(2)|recv(2)>,
L<recvfrom(2)|recvfrom(2)>,
L<gethostbyname(3)|gethostbyname(3)>,
L<perlfunc(1)|perlfunc(1)>,
L<chat(8)|chat(8)>,
L<fdopen(3)|fdopen(3)>,
L<scanf(3)|scanf(3)>,
L<printf(3)|printf(3)>,
L<conf(3)|conf(3)>,
L<daemon(3)|daemon(3)>,
L<err(3)|err(3)>,
L<fifo(3)|fifo(3)>,
L<hsort(3)|hsort(3)>,
L<lim(3)|lim(3)>,
L<list(3)|list(3)>,
L<log(3)|log(3)>,
L<map(3)|map(3)>,
L<mem(3)|mem(3)>,
L<msg(3)|msg(3)>,
L<opt(3)|opt(3)>,
L<prog(3)|prog(3)>,
L<prop(3)|prop(3)>,
L<sig(3)|sig(3)>,
L<str(3)|str(3)>

=head1 AUTHOR

20000902 raf <raf@raf.org>

=cut

*/

#ifdef TEST

#include <signal.h>
#include <pwd.h>

#include <sys/utsname.h>
#include <sys/wait.h>

void print_pkt(const char *name, void *buf, size_t size)
{
	unsigned char *pkt = buf;
	size_t i;

	while (size && pkt[size - 1] == '\0')
		--size;

	printf("%s =\n{\n", name);

	for (i = 0; i < size; ++i)
	{
		if (i && i % 10 == 0)
			printf("\n");
		if (i % 10 == 0)
			printf("    ");
		printf("0x%02x%s", pkt[i], (i < size - 1) ? ", " : "");
	}

	printf("%s}\n\n", (i % 10) ? "\n" : "");
}

int wait_for_child(pid_t pid)
{
	int status[1];

	if (waitpid(pid, status, 0) == -1)
	{
		fprintf(stderr, "Failed to waitpid(%d) (%s)\n", (int)pid, strerror(errno));
		exit(1);
	}

	if (WIFSIGNALED(*status))
	{
		fprintf(stderr, "Child failed: received signal %d\n", WTERMSIG(*status));
		exit(1);
	}

	return WEXITSTATUS(*status);
}

int main(int ac, char **av)
{
	pid_t pid;
	int server;
	int errors = 0;
	struct sigaction sa[1], osa[1];
	char *fmt;
	void *a, *a2;
	char *z, *z2;
	char *b, *b2;
	char *h, *h2;
	signed char sc, sc2;
	unsigned char uc, uc2;
	signed short us, us2;
	unsigned short ss, ss2;
	signed int si, si2;
	unsigned int ui = 37, ui2;
	unsigned int uia, uia2;
	unsigned int uib, uib2;
	signed short sv1, sv2;
	unsigned short uv1, uv2;
	signed int sw1, sw2;
	unsigned int uw1, uw2;
	float f, f2;
	double da, da2;
	double db, db2;
	double dc, dc2;
	double dd, dd2;
	void *p, *p2;
#ifdef HAS_LONG_LONG
	signed long long sl, sl2;
	unsigned long long ul, ul2;
#endif
	char pkt[1024];
	char tstmem[1024];
	ssize_t pkt_len;
	struct passwd *pwd;
	int rc;
	time_t t;

	unsigned char pkt_cmp[1024] =
	{
		0x00, 0x01, 0x02, 0x00, 0x03, 0x04, 0x00, 0x05, 0x06, 0x00, 
		0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 
		0x64, 0x00, 0x24, 0x92, 0x40, 0x01, 0x23, 0x45, 0x67, 0x89, 
		0xab, 0xcd, 0xef, 0xfd, 0x03, 0xff, 0xfd, 0x00, 0x03, 0xff, 
		0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
		0x06, 0xfd, 0xff, 0x03, 0x00, 0xfd, 0xff, 0xff, 0xff, 0x03, 
		0x00, 0x00, 0x00, 0x06, 0x34, 0x33, 0x2e, 0x32, 0x31, 0x00, 
		0x06, 0x31, 0x32, 0x2e, 0x33, 0x34, 0x00, 0x07, 0x2d, 0x31, 
		0x32, 0x2e, 0x33, 0x34, 0x00, 0x08, 0x31, 0x2e, 0x35, 0x65, 
		0x2b, 0x31, 0x30, 0x00, 0x09, 0x2d, 0x35, 0x2e, 0x31, 0x65, 
		0x2d, 0x31, 0x30, 0x00, 0x08, 0x06, 0x1e, 0xdf
	};

#ifdef HAS_LONG_LONG
	unsigned char lpkt_cmp[16] =
	{
		0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
	};
#endif

	printf("Testing: net\n");

	/* Test tcp client and server sockets */

	if (before_alarm(sa, osa) == -1)
	{
		printf("Failed to handle SIGALRM (%s)\n", strerror(errno));
		return 1;
	}

	server = net_server("localhost", 30000, NULL, NULL);
	if (server == -1)
		++errors, printf("Test1: net_server(\"localhost\", 30000) failed: %s\n", strerror(errno));
	else
	{
		switch (pid = fork())
		{
			case -1:
			{
				printf("Failed to fork (%s)\n", strerror(errno));
				return 1;
			}

			default:
			{
				int s;
				sockaddr_in addr[1];
				size_t addrlen = sizeof(sockaddr_in);

				alarm(5);
				s = accept(server, (sockaddr *)addr, &addrlen);
				if (s == -1)
					++errors, printf("Test2: accept() failed (%s)\n", strerror(errno));
				else
				{
					char test[4];

					if (read(s, test, 4) == -1)
						++errors, printf("Test3: read(s, HELO) failed (%s)\n", strerror(errno));
					else if (memcmp(test, "HELO", 4))
						++errors, printf("Test4: read(s, HELO) failed (read \"%4.4s\", not \"%4.4s\")\n", test, "HELO");
					else if (write(s, "OLEH", 4) == -1)
						++errors, printf("Test5: write(s, OLEH) failed (%s)\n", strerror(errno));
					if (close(s) == -1)
						++errors, printf("Test6: close(s) failed (%s)\n", strerror(errno));
				}

				alarm(0);
				errors += wait_for_child(pid);
				break;
			}

			case 0:
			{
				int i, c = 0;
				errors = 0;

				for (i = 0; i < 5; ++i)
				{
					c = net_client("localhost", 30000, NULL, NULL);
					if (c != -1)
						break;
					sleep(1);
				}

				if (c == -1)
					++errors, printf("Test7: net_client(\"localhost\", 30000) failed (%s)\n", strerror(errno));
				else
				{
					char test[4];
					alarm(5);
					if (write(c, "HELO", 4) == -1)
						++errors, printf("Test8: write(c, HELO) failed (%s)\n", strerror(errno));
					else if (read(c, test, 4) == -1)
						++errors, printf("Test9: read(c, OLEH) failed (%s)\n", strerror(errno));
					else if (memcmp(test, "OLEH", 4))
						++errors, printf("Test10: read(c, OLEH) failed (read \"%4.4s\", not \"%4.4s\")\n", test, "OLEH");
					if (close(c) == -1)
						++errors, printf("Test11: close(c) failed (%s)\n", strerror(errno));
					alarm(0);
				}

				return errors;
			}
		}

		if (close(server) == -1)
			++errors, printf("Test12: close(server) failed (%s)\n", strerror(errno));
	}

	/* Test udp client and server sockets */

	server = net_udp_server("localhost", 30000, NULL, NULL);
	if (server == -1)
		++errors, printf("Test13: net_udp_server(\"localhost\", 30000) failed: %s\n", strerror(errno));
	else
	{
		switch (pid = fork())
		{
			case -1:
			{
				printf("Failed to fork (%s)\n", strerror(errno));
				return 1;
			}

			default:
			{
				char test[4];
				sockaddr_in ipaddr;
				size_t ipsize = sizeof ipaddr;

				alarm(5);
				if (recvfrom(server, test, 4, 0, (sockaddr *)&ipaddr, &ipsize) == -1)
					++errors, printf("Test14: recvfrom(server, HELO) failed (%s)\n", strerror(errno));
				else if (memcmp(test, "HELO", 4))
					++errors, printf("Test15: recvfrom(server, HELO) failed (recv \"%4.4s\", not \"%4.4s\")\n", test, "HELO");
				else if (sendto(server, "OLEH", 4, 0, (sockaddr *)&ipaddr, ipsize) == -1)
					++errors, printf("Test16: sendto(server, OLEH) failed (%s)\n", strerror(errno));
				alarm(0);
				errors += wait_for_child(pid);
				break;
			}

			case 0:
			{
				int i, c = 0;
				errors = 0;

				for (i = 0; i < 5; ++i)
				{
					c = net_udp_client("localhost", 30000, NULL, NULL);
					if (c != -1)
						break;
					sleep(1);
				}

				if (c == -1)
					++errors, printf("Test17: net_client(\"localhost\", 30000) failed (%s)\n", strerror(errno));
				else
				{
					char test[4];
					alarm(5);
					if (send(c, "HELO", 4, 0) == -1)
						++errors, printf("Test18: send(c, HELO) failed (%s)\n", strerror(errno));
					else if (recv(c, test, 4, 0) == -1)
						++errors, printf("Test19: recv(c, OLEH) failed (%s)\n", strerror(errno));
					else if (memcmp(test, "OLEH", 4))
						++errors, printf("Test20: recv(c, OLEH) failed (recv \"%4.4s\", not \"%4.4s\")\n", test, "OLEH");
					if (close(c) == -1)
						++errors, printf("Test21: close(c) failed (%s)\n", strerror(errno));
					alarm(0);
				}

				return errors;
			}
		}

		if (close(server) == -1)
			++errors, printf("Test22: close(server) failed (%s)\n", strerror(errno));
	}

	/* Test pack() and unpack() */

	fmt = "a10z*b*h*c2s2i3v2w2fd4pxX@*";
	a = "\000\001\002\000\003\004\000\005\006\000";
	z = "hello world";
	b = "001001001001001001";
	h = "0123456789abcdef";
	sc = -3;
	uc = 3;
	ss = -3;
	us = 3;
	si = -3;
	uia = 3;
	uib = 6;
	sv1 = -3;
	uv1 = 3;
	sw1 = -3;
	uw1 = 3;
	f = 43.21;
	da = 12.34;
	db = -12.34;
	dc = 1.5e10;
	dd = -5.1e-10;
	p = a;

	pkt_len = pack(pkt, 1024, fmt,
		a,
		strlen(z) + 1, z,
		strlen(b), b,
		strlen(h), h,
		sc, uc,
		ss, us,
		si, uia, uib,
		sv1, uv1, sw1, uw1,
		f, da, db, dc, dd,
		p,
		1024
	);

	if (pkt_len == -1)
		++errors, printf("Test23: pack(\"%s\") failed (%s)\n", fmt, strerror(errno));
	else if (pkt_len != 1024)
		++errors, printf("Test23: pack(\"%s\") failed (returned %d, not %d)\n", fmt, pkt_len, 1024);
	else
	{
		a2 = malloc(10);
		z2 = malloc(strlen(z) + 1);
		b2 = malloc(strlen(b) + 1);
		h2 = malloc(strlen(h) + 1);

		if (a2 && z2 && b2 && h2)
		{
			pkt_len = unpack(pkt, pkt_len, fmt,
				a2,
				strlen(z) + 1, z2,
				strlen(b), b2,
				strlen(h), h2,
				&sc2, &uc2,
				&ss2, &us2,
				&si2, &uia2, &uib2,
				&sv2, &uv2, &sw2, &uw2,
				&f2, &da2, &db2, &dc2, &dd2,
				&p2,
				1024
			);

			if (pkt_len == -1)
				++errors, printf("Test24: unpack(\"%s\") failed (%s)\n", fmt, strerror(errno));
			else if (pkt_len != 1024)
				++errors, printf("Test24: unpack(\"%s\") failed (returned %d, not %d)\n", fmt, pkt_len, 1024);
			else
			{
				if (memcmp(a, a2, 10))
					++errors, printf("Test25: pack(a) failed\n");
				if (strcmp(z, z2))
					++errors, printf("Test26: pack(z) failed (packed %s, unpacked %s)\n", z, z2);
				if (strcmp(b, b2))
					++errors, printf("Test27: pack(b) failed (packed %s, unpacked %s)\n", b, b2);
				if (strcmp(h, h2))
					++errors, printf("Test28: pack(h) failed (packed %s, unpacked %s)\n", h, h2);
				if (sc != sc2)
					++errors, printf("Test29: pack(c) failed (packed %d, unpacked %d)\n", sc, sc2);
				if (uc != uc2)
					++errors, printf("Test30: pack(C) failed (packed %u, unpacked %u)\n", uc, uc2);
				if (ss != ss2)
					++errors, printf("Test31: pack(s) failed (packed %d, unpacked %d)\n", sc, sc2);
				if (us != us2)
					++errors, printf("Test32: pack(S) failed (packed %u, unpacked %u)\n", us, us2);
				if (si != si2)
					++errors, printf("Test33: pack(i) failed (packed %d, unpacked %d)\n", si, si2);
				if (uia != uia2)
					++errors, printf("Test34: pack(I) failed (packed %u, unpacked %u)\n", uia, uia2);
				if (uib != uib2)
					++errors, printf("Test35: pack(I) failed (packed %u, unpacked %u)\n", uib, uib2);
				if (sv1 != sv2)
					++errors, printf("Test36: pack(v) failed (packed %d, unpacked %d)\n", sv1, sv2);
				if (uv1 != uv2)
					++errors, printf("Test37: pack(V) failed (packed %u, unpacked %u)\n", uv1, uv2);
				if (sw1 != sw2)
					++errors, printf("Test38: pack(w) failed (packed %d, unpacked %d)\n", sw1, sw2);
				if (uw1 != uw2)
					++errors, printf("Test39: pack(W) failed (packed %u, unpacked %u)\n", uw1, uw2);
				if (f != f2)
					++errors, printf("Test40: pack(f) failed (packed %g, unpacked %g)\n", (double)f, (double)f2);
				if (da != da2)
					++errors, printf("Test41: pack(d) failed (packed %g, unpacked %g)\n", da, da2);
				if (db != db2)
					++errors, printf("Test42: pack(d) failed (packed %g, unpacked %g)\n", db, db2);
				if (dc != dc2)
					++errors, printf("Test43: pack(d) failed (packed %g, unpacked %g)\n", dc, dc2);
				if (dd != dd2)
					++errors, printf("Test44: pack(d) failed (packed %g, unpacked %g)\n", dd, dd2);
				if (p != p2)
					++errors, printf("Test45: pack(p) failed (packed %p, unpacked %p)\n", p, p2);
			}

			/* Test unpack with skipping */

			memset(z2, '\0', strlen(z) + 1);
			sc2 = 0;
			uc2 = 0;
			ss2 = 0;
			si2 = 0;
			uib2 = 0;
			da2 = 0.0;
			dc2 = 0.0;
			p2 = NULL;

			pkt_len = unpack(pkt, pkt_len, fmt,
				NULL,
				strlen(z) + 1, z2,
				strlen(b), NULL,
				strlen(h), NULL,
				&sc2, &uc2,
				&ss2, NULL,
				&si2, NULL, &uib2,
				NULL, NULL, NULL, NULL,
				NULL, &da2, NULL, &dc2, NULL,
				&p2,
				1024
			);

			if (pkt_len == -1)
				++errors, printf("Test46: unpack(\"%s\", NULL) failed (%s)\n", fmt, strerror(errno));
			else if (pkt_len != 1024)
				++errors, printf("Test46: unpack(\"%s\", NULL) failed (returned %d, not %d)\n", fmt, pkt_len, 1024);
			else
			{
				if (strcmp(z, z2))
					++errors, printf("Test47: pack(z, NULL) failed (packed %s, unpacked %s)\n", z, z2);
				if (sc != sc2)
					++errors, printf("Test48: pack(c, NULL) failed (packed %d, unpacked %d)\n", sc, sc2);
				if (uc != uc2)
					++errors, printf("Test49: pack(C, NULL) failed (packed %u, unpacked %u)\n", uc, uc2);
				if (ss != ss2)
					++errors, printf("Test50: pack(s, NULL) failed (packed %d, unpacked %d)\n", sc, sc2);
				if (si != si2)
					++errors, printf("Test51: pack(i, NULL) failed (packed %d, unpacked %d)\n", si, si2);
				if (uib != uib2)
					++errors, printf("Test52: pack(I, NULL) failed (packed %u, unpacked %u)\n", uib, uib2);
				if (da != da2)
					++errors, printf("Test53: pack(d, NULL) failed (packed %g, unpacked %g)\n", da, da2);
				if (dc != dc2)
					++errors, printf("Test54: pack(d, NULL) failed (packed %g, unpacked %g)\n", dc, dc2);
				if (p != p2)
					++errors, printf("Test55: pack(p, NULL) failed (packed %p, unpacked %p)\n", p, p2);
			}
		}
		else
			++errors, printf("Test24: failed to run (%s)\n", strerror(errno));

		free(a2);
		free(z2);
		free(b2);
		free(h2);

		/* Test binary compatibility (ignoring the packed pointer) */

		if (memcmp(pkt, pkt_cmp, 104) || memcmp(pkt + 108, pkt_cmp + 108, 1024 - 108))
		{
			++errors, printf("Test56: pack(\"%s\") failed (packed data looks wrong)\n", fmt);
			print_pkt("good packet", pkt_cmp, 1024);
			print_pkt("bad packet", pkt, 1024);
		}
	}

#ifdef HAS_LONG_LONG
	sl = ~(((signed long long)0x01020304 << 32) | 0x05060708);
	ul = ((unsigned long long)0x01020304 << 32) | 0x05060708;

	pkt_len = pack(pkt, 1024, "l2", sl, ul);

	if (pkt_len == -1)
		++errors, printf("Test57: pack(\"l2\") failed (%s)\n", strerror(errno));
	else if (pkt_len != 16)
		++errors, printf("Test57: pack(\"l2\") failed (returned %d, not %d)\n", pkt_len, 16);
	else
	{
		pkt_len = unpack(pkt, 16, "l2", &sl2, &ul2);

		if (pkt_len == -1)
			++errors, printf("Test58: unpack(\"l2\") failed (%s)\n", strerror(errno));
		else if (pkt_len != 16)
			++errors, printf("Test58: unpack(\"l2\") failed (returned %d, not %d)\n", pkt_len, 16);
		else
		{
			if (sl != sl2)
				++errors, printf("Test59: pack(\"l2\") failed\n");
			if (ul != ul2)
				++errors, printf("Test60: pack(\"l2\") failed\n");
		}

		/* Test long long binary compatibility */

		if (memcmp(pkt, lpkt_cmp, 16))
		{
			++errors, printf("Test61: pack(\"l2\") failed (packed data looks wrong)\n");
			print_pkt("good packet", lpkt_cmp, 16);
			print_pkt("bad packet", pkt, 16);
		}
	}
#endif

	/* Test packing sizes */

#define TEST_SIZE(i, fmt, size, pfmt, cast, data1, data2, data2ref, test) \
	pkt_len = pack(pkt, (size), (fmt), (data1)); \
	if (pkt_len == -1) \
		++errors, printf("Test%d: pack(%d, \"%s\") failed (%s)\n", (i), (size), (fmt), strerror(errno)); \
	else if ((size) && pkt_len != (size)) \
		++errors, printf("Test%d: pack(%d, \"%s\") failed (size = %d, not %d)\n", (i), (size), (fmt), pkt_len, (size)); \
	else \
	{ \
		pkt_len = unpack(pkt, (size), (fmt), (data2ref)); \
		if (pkt_len == -1) \
			++errors, printf("Test%d: unpack(%d, \"%s\") failed (%s)\n", (i), (size), (fmt), strerror(errno)); \
		else if ((size) && pkt_len != (size)) \
			++errors, printf("Test%d: unpack(%d, \"%s\") failed (size = %d, not %d)\n", (i), (size), (fmt), pkt_len, (size)); \
		else if (test) \
		{ \
			char a[128], b[128]; \
			snprintf(a, 128, pfmt, cast data1); \
			snprintf(b, 128, pfmt, cast data2); \
			++errors, printf("Test%d: unpack(%d, \"%s\") failed (%s != %s)\n", (i), (size), (fmt), a, b); \
		} \
	}

#define TEST_SINT(i, fmt, size, data1, data2) TEST_SIZE(i, fmt, size, "%ld", (signed   long), data1, data2, &data2, data2 != data1)
#define TEST_UINT(i, fmt, size, data1, data2) TEST_SIZE(i, fmt, size, "%lu", (unsigned long), data1, data2, &data2, data2 != data1)
#define TEST_STR(i, fmt, size, pfmt, len, data1, data2) TEST_SIZE(i, fmt, size, pfmt, (char *), data1, data2, data2, memcmp(data1, data2, len))

	TEST_SINT(62, "c", 1, sc, sc2)
	TEST_UINT(63, "c", 1, uc, uc2)
	TEST_SINT(64, "s", 2, ss, ss2)
	TEST_UINT(65, "s", 2, us, us2)
	TEST_SINT(66, "i", 4, si, si2)
	TEST_UINT(67, "i", 4, ui, ui2)
	TEST_UINT(68, "v", 2, sv1, sv2)
	TEST_UINT(69, "v", 2, uv1, uv2)
	TEST_SINT(70, "w", 4, sw1, sw2)
	TEST_SINT(71, "w", 4, uw1, uw2)
#ifdef HAS_LONG_LONG
	TEST_SINT(72, "l", 8, sl, sl2)
	TEST_UINT(73, "l", 8, ul, ul2)
#endif

	TEST_STR(74, "b1", 1, "%1.1s", 1, b, tstmem)
	TEST_STR(75, "b2", 1, "%2.2s", 2, b, tstmem)
	TEST_STR(76, "b3", 1, "%3.3s", 3, b, tstmem)
	TEST_STR(77, "b4", 1, "%4.4s", 4, b, tstmem)
	TEST_STR(78, "b5", 1, "%5.5s", 5, b, tstmem)
	TEST_STR(79, "b6", 1, "%6.6s", 6, b, tstmem)
	TEST_STR(80, "b7", 1, "%7.7s", 7, b, tstmem)
	TEST_STR(81, "b8", 1, "%8.8s", 8, b, tstmem)
	TEST_STR(82, "b9", 2, "%9.9s", 9, b, tstmem)
	TEST_STR(83, "b10", 2, "%10.10s", 10, b, tstmem)
	TEST_STR(84, "b11", 2, "%11.11s", 11, b, tstmem)
	TEST_STR(85, "b12", 2, "%12.12s", 12, b, tstmem)
	TEST_STR(86, "b13", 2, "%13.13s", 13, b, tstmem)
	TEST_STR(87, "b14", 2, "%14.14s", 14, b, tstmem)
	TEST_STR(88, "b15", 2, "%15.15s", 15, b, tstmem)
	TEST_STR(89, "b16", 2, "%16.16s", 16, b, tstmem)
	TEST_STR(90, "b17", 3, "%17.17s", 17, b, tstmem)
	TEST_STR(91, "b18", 3, "%18.18s", 18, b, tstmem)

	TEST_STR(92, "h1", 1, "%1.1s", 1, h, tstmem)
	TEST_STR(93, "h2", 1, "%2.2s", 2, h, tstmem)
	TEST_STR(94, "h3", 2, "%3.3s", 3, h, tstmem)
	TEST_STR(95, "h4", 2, "%4.4s", 4, h, tstmem)
	TEST_STR(96, "h5", 3, "%5.5s", 5, h, tstmem)
	TEST_STR(97, "h6", 3, "%6.6s", 6, h, tstmem)
	TEST_STR(98, "h7", 4, "%7.7s", 7, h, tstmem)
	TEST_STR(99, "h8", 4, "%8.8s", 8, h, tstmem)
	TEST_STR(100, "h9", 5, "%9.9s", 9, h, tstmem)

	/* Test error reporting */

#define TEST_FAILURE(i, test, error) \
	pkt_len = test; \
	if (pkt_len != -1) \
		++errors, printf("Test%d: %s error failed (size %d, no error, not %s)\n", (i), #test, pkt_len, strerror(error)); \
	else if (errno != error) \
		++errors, printf("Test%d: %s error failed (%s, not %s)\n", (i), #test, strerror(errno), strerror(error));

	TEST_FAILURE(101, pack(NULL, 1, "a", a), EINVAL)
	TEST_FAILURE(102, pack(pkt, 1, NULL, a), EINVAL)

	TEST_FAILURE(103, pack(pkt, 1, "a0", a), EINVAL)
	TEST_FAILURE(104, pack(pkt, 1, "a*", 0, a), EINVAL)
	TEST_FAILURE(105, pack(pkt, 1, "a*", -1, a), EINVAL)
	TEST_FAILURE(106, pack(pkt, 0, "a", a), ENOSPC)
	TEST_FAILURE(107, pack(pkt, 1, "a2", a), ENOSPC)
	TEST_FAILURE(108, pack(pkt, 1, "a*", 2, a), ENOSPC)
	TEST_FAILURE(109, pack(pkt, 1, "a", NULL), EINVAL)

	TEST_FAILURE(110, pack(pkt, 1, "z0", z), EINVAL)
	TEST_FAILURE(111, pack(pkt, 1, "z*", 0, z), EINVAL)
	TEST_FAILURE(112, pack(pkt, 1, "z*", -1, z), EINVAL)
	TEST_FAILURE(113, pack(pkt, 0, "z", z), ENOSPC)
	TEST_FAILURE(114, pack(pkt, 1, "z2", z), ENOSPC)
	TEST_FAILURE(115, pack(pkt, 1, "z*", 2, z), ENOSPC)
	TEST_FAILURE(116, pack(pkt, 1, "z", NULL), EINVAL)

	TEST_FAILURE(117, pack(pkt, 1, "b0", b), EINVAL)
	TEST_FAILURE(118, pack(pkt, 1, "b*", 0, b), EINVAL)
	TEST_FAILURE(119, pack(pkt, 1, "b*", -1, b), EINVAL)
	TEST_FAILURE(120, pack(pkt, 0, "b", b), ENOSPC)
	TEST_FAILURE(121, pack(pkt, 1, "b9", b), ENOSPC)
	TEST_FAILURE(122, pack(pkt, 1, "b*", 9, b), ENOSPC)
	TEST_FAILURE(123, pack(pkt, 1, "b", NULL), EINVAL)
	TEST_FAILURE(124, pack(pkt, 1, "b8", a), EINVAL)
	TEST_FAILURE(125, pack(pkt, 1, "b8", h), EINVAL)

	TEST_FAILURE(126, pack(pkt, 1, "h0", h), EINVAL)
	TEST_FAILURE(127, pack(pkt, 1, "h*", 0, h), EINVAL)
	TEST_FAILURE(128, pack(pkt, 1, "h*", -1, h), EINVAL)
	TEST_FAILURE(129, pack(pkt, 0, "h", h), ENOSPC)
	TEST_FAILURE(130, pack(pkt, 1, "h3", h), ENOSPC)
	TEST_FAILURE(131, pack(pkt, 1, "h*", 3, h), ENOSPC)
	TEST_FAILURE(132, pack(pkt, 1, "h", NULL), EINVAL)
	TEST_FAILURE(133, pack(pkt, 1, "h", a), EINVAL)

	TEST_FAILURE(134, pack(pkt, 1, "c0", sc), EINVAL)
	TEST_FAILURE(135, pack(pkt, 1, "c*", 0, sc), EINVAL)
	TEST_FAILURE(136, pack(pkt, 1, "c*", -1, sc), EINVAL)
	TEST_FAILURE(137, pack(pkt, 0, "c", sc), ENOSPC)
	TEST_FAILURE(138, pack(pkt, 1, "c2", sc, sc), ENOSPC)
	TEST_FAILURE(139, pack(pkt, 1, "c*", 2, sc, sc), ENOSPC)

	TEST_FAILURE(140, pack(pkt, 1, "c0", uc), EINVAL)
	TEST_FAILURE(141, pack(pkt, 1, "c*", 0, uc), EINVAL)
	TEST_FAILURE(142, pack(pkt, 1, "c*", -1, uc), EINVAL)
	TEST_FAILURE(143, pack(pkt, 0, "c", uc), ENOSPC)
	TEST_FAILURE(144, pack(pkt, 1, "c2", uc, uc), ENOSPC)
	TEST_FAILURE(145, pack(pkt, 1, "c*", 2, uc, uc), ENOSPC)

	TEST_FAILURE(146, pack(pkt, 1, "s0", ss), EINVAL)
	TEST_FAILURE(147, pack(pkt, 1, "s*", 0, ss), EINVAL)
	TEST_FAILURE(148, pack(pkt, 1, "s*", -1, ss), EINVAL)
	TEST_FAILURE(149, pack(pkt, 0, "s", ss), ENOSPC)
	TEST_FAILURE(150, pack(pkt, 1, "s", ss), ENOSPC)
	TEST_FAILURE(151, pack(pkt, 2, "s2", ss, ss), ENOSPC)
	TEST_FAILURE(152, pack(pkt, 3, "s*", 2, ss, ss), ENOSPC)

	TEST_FAILURE(153, pack(pkt, 1, "s0", us), EINVAL)
	TEST_FAILURE(154, pack(pkt, 1, "s*", 0, us), EINVAL)
	TEST_FAILURE(155, pack(pkt, 1, "s*", -1, us), EINVAL)
	TEST_FAILURE(156, pack(pkt, 0, "s", us), ENOSPC)
	TEST_FAILURE(157, pack(pkt, 1, "s", us), ENOSPC)
	TEST_FAILURE(158, pack(pkt, 2, "s2", us, us), ENOSPC)
	TEST_FAILURE(159, pack(pkt, 3, "s*", 2, us, us), ENOSPC)

	TEST_FAILURE(160, pack(pkt, 1, "i0", si), EINVAL)
	TEST_FAILURE(161, pack(pkt, 1, "i*", 0, si), EINVAL)
	TEST_FAILURE(162, pack(pkt, 1, "i*", -1, si), EINVAL)
	TEST_FAILURE(163, pack(pkt, 0, "i", si), ENOSPC)
	TEST_FAILURE(164, pack(pkt, 1, "i", si), ENOSPC)
	TEST_FAILURE(165, pack(pkt, 2, "i", si), ENOSPC)
	TEST_FAILURE(166, pack(pkt, 3, "i", si), ENOSPC)
	TEST_FAILURE(167, pack(pkt, 4, "i2", si, si), ENOSPC)
	TEST_FAILURE(168, pack(pkt, 5, "i*", 2, si, si), ENOSPC)

	TEST_FAILURE(169, pack(pkt, 1, "i0", ui), EINVAL)
	TEST_FAILURE(170, pack(pkt, 1, "i*", 0, ui), EINVAL)
	TEST_FAILURE(171, pack(pkt, 1, "i*", -1, ui), EINVAL)
	TEST_FAILURE(172, pack(pkt, 0, "i", ui), ENOSPC)
	TEST_FAILURE(173, pack(pkt, 1, "i", ui), ENOSPC)
	TEST_FAILURE(174, pack(pkt, 2, "i", ui), ENOSPC)
	TEST_FAILURE(175, pack(pkt, 3, "i", ui), ENOSPC)
	TEST_FAILURE(176, pack(pkt, 4, "i2", ui, ui), ENOSPC)
	TEST_FAILURE(177, pack(pkt, 5, "i*", 2, ui, ui), ENOSPC)

	TEST_FAILURE(178, pack(pkt, 1, "v0", sv1), EINVAL)
	TEST_FAILURE(179, pack(pkt, 1, "v*", 0, sv1), EINVAL)
	TEST_FAILURE(180, pack(pkt, 1, "v*", -1, sv1), EINVAL)
	TEST_FAILURE(181, pack(pkt, 0, "v", sv1), ENOSPC)
	TEST_FAILURE(182, pack(pkt, 1, "v", sv1), ENOSPC)
	TEST_FAILURE(183, pack(pkt, 2, "v2", sv1, sv2), ENOSPC)
	TEST_FAILURE(184, pack(pkt, 3, "v*", 2, sv1, sv2), ENOSPC)

	TEST_FAILURE(185, pack(pkt, 1, "v0", uv1), EINVAL)
	TEST_FAILURE(186, pack(pkt, 1, "v*", 0, uv1), EINVAL)
	TEST_FAILURE(187, pack(pkt, 1, "v*", -1, uv1), EINVAL)
	TEST_FAILURE(188, pack(pkt, 0, "v", uv1), ENOSPC)
	TEST_FAILURE(189, pack(pkt, 1, "v", uv1), ENOSPC)
	TEST_FAILURE(190, pack(pkt, 2, "v2", uv1, uv2), ENOSPC)
	TEST_FAILURE(191, pack(pkt, 3, "v*", 2, uv1, uv2), ENOSPC)

	TEST_FAILURE(192, pack(pkt, 1, "w0", sw1), EINVAL)
	TEST_FAILURE(193, pack(pkt, 1, "w*", 0, sw1), EINVAL)
	TEST_FAILURE(194, pack(pkt, 1, "w*", -1, sw1), EINVAL)
	TEST_FAILURE(195, pack(pkt, 0, "w", sw1), ENOSPC)
	TEST_FAILURE(196, pack(pkt, 1, "w", sw1), ENOSPC)
	TEST_FAILURE(197, pack(pkt, 2, "w", uv1), ENOSPC)
	TEST_FAILURE(198, pack(pkt, 3, "w", uv1), ENOSPC)
	TEST_FAILURE(199, pack(pkt, 4, "w2", sw1, sw2), ENOSPC)
	TEST_FAILURE(200, pack(pkt, 5, "w*", 2, sw1, sw2), ENOSPC)

	TEST_FAILURE(201, pack(pkt, 1, "w0", uw1), EINVAL)
	TEST_FAILURE(202, pack(pkt, 1, "w*", 0, uw1), EINVAL)
	TEST_FAILURE(203, pack(pkt, 1, "w*", -1, uw1), EINVAL)
	TEST_FAILURE(204, pack(pkt, 0, "w", uw1), ENOSPC)
	TEST_FAILURE(205, pack(pkt, 1, "w", uw1), ENOSPC)
	TEST_FAILURE(206, pack(pkt, 2, "w", uw1), ENOSPC)
	TEST_FAILURE(207, pack(pkt, 3, "w", uw1), ENOSPC)
	TEST_FAILURE(208, pack(pkt, 4, "w2", uw1, uw2), ENOSPC)
	TEST_FAILURE(209, pack(pkt, 5, "w*", 2, uw1, uw2), ENOSPC)

	TEST_FAILURE(210, pack(pkt, 1, "p0", p), EINVAL)
	TEST_FAILURE(211, pack(pkt, 1, "p*", 0, p), EINVAL)
	TEST_FAILURE(212, pack(pkt, 1, "p*", -1, p), EINVAL)
	TEST_FAILURE(213, pack(pkt, 0, "p", p), ENOSPC)
	TEST_FAILURE(214, pack(pkt, 1, "p", p), ENOSPC)
	TEST_FAILURE(215, pack(pkt, 2, "p", p), ENOSPC)
	TEST_FAILURE(216, pack(pkt, 3, "p", p), ENOSPC)
	TEST_FAILURE(217, pack(pkt, 4, "p2", p, p), ENOSPC)
	TEST_FAILURE(218, pack(pkt, 5, "p*", 2, p, p), ENOSPC)

#ifdef HAS_LONG_LONG
	TEST_FAILURE(219, pack(pkt, 1, "l0", sl), EINVAL)
	TEST_FAILURE(220, pack(pkt, 1, "l*", 0, sl), EINVAL)
	TEST_FAILURE(221, pack(pkt, 1, "l*", -1, sl), EINVAL)
	TEST_FAILURE(222, pack(pkt, 0, "l", sl), ENOSPC)
	TEST_FAILURE(223, pack(pkt, 1, "l", sl), ENOSPC)
	TEST_FAILURE(224, pack(pkt, 2, "l", sl), ENOSPC)
	TEST_FAILURE(225, pack(pkt, 3, "l", sl), ENOSPC)
	TEST_FAILURE(226, pack(pkt, 4, "l", sl), ENOSPC)
	TEST_FAILURE(227, pack(pkt, 5, "l", sl), ENOSPC)
	TEST_FAILURE(228, pack(pkt, 6, "l", sl), ENOSPC)
	TEST_FAILURE(229, pack(pkt, 7, "l", sl), ENOSPC)
	TEST_FAILURE(230, pack(pkt, 8, "l2", sl, sl), ENOSPC)
	TEST_FAILURE(231, pack(pkt, 9, "l*", 2, sl, sl), ENOSPC)

	TEST_FAILURE(232, pack(pkt, 1, "l0", ul), EINVAL)
	TEST_FAILURE(233, pack(pkt, 1, "l*", 0, ul), EINVAL)
	TEST_FAILURE(234, pack(pkt, 1, "l*", -1, ul), EINVAL)
	TEST_FAILURE(235, pack(pkt, 0, "l", ul), ENOSPC)
	TEST_FAILURE(236, pack(pkt, 1, "l", ul), ENOSPC)
	TEST_FAILURE(237, pack(pkt, 2, "l", ul), ENOSPC)
	TEST_FAILURE(238, pack(pkt, 3, "l", ul), ENOSPC)
	TEST_FAILURE(239, pack(pkt, 4, "l", ul), ENOSPC)
	TEST_FAILURE(240, pack(pkt, 5, "l", ul), ENOSPC)
	TEST_FAILURE(241, pack(pkt, 6, "l", ul), ENOSPC)
	TEST_FAILURE(242, pack(pkt, 7, "l", ul), ENOSPC)
	TEST_FAILURE(243, pack(pkt, 8, "l2", ul, ul), ENOSPC)
	TEST_FAILURE(244, pack(pkt, 9, "l*", 2, ul, ul), ENOSPC)
#else
	TEST_FAILURE(245, pack(pkt, 8, "l", 0), ENOSYS)
	TEST_FAILURE(246, pack(pkt, 8, "l", 0), ENOSYS)
#endif

	f = -1.5; /* packed length = 6 */
	TEST_FAILURE(247, pack(pkt, 1, "f0", f), EINVAL)
	TEST_FAILURE(248, pack(pkt, 1, "f*", 0, f), EINVAL)
	TEST_FAILURE(249, pack(pkt, 1, "f*", -1, f), EINVAL)
	TEST_FAILURE(250, pack(pkt, 0, "f", f), ENOSPC)
	TEST_FAILURE(251, pack(pkt, 1, "f", f), ENOSPC)
	TEST_FAILURE(252, pack(pkt, 2, "f", f), ENOSPC)
	TEST_FAILURE(253, pack(pkt, 3, "f", f), ENOSPC)
	TEST_FAILURE(254, pack(pkt, 4, "f", f), ENOSPC)
	TEST_FAILURE(255, pack(pkt, 5, "f", f), ENOSPC)
	TEST_FAILURE(256, pack(pkt, 6, "f2", f, f), ENOSPC)
	TEST_FAILURE(257, pack(pkt, 7, "f*", 2, f, f), ENOSPC)

	da = db = 1.5; /* packed length = 5 */
	TEST_FAILURE(258, pack(pkt, 1, "d0", da), EINVAL)
	TEST_FAILURE(259, pack(pkt, 1, "d*", 0, da), EINVAL)
	TEST_FAILURE(260, pack(pkt, 1, "d*", -1, da), EINVAL)
	TEST_FAILURE(261, pack(pkt, 0, "d", da), ENOSPC)
	TEST_FAILURE(262, pack(pkt, 1, "d", da), ENOSPC)
	TEST_FAILURE(263, pack(pkt, 2, "d", da), ENOSPC)
	TEST_FAILURE(264, pack(pkt, 3, "d", da), ENOSPC)
	TEST_FAILURE(265, pack(pkt, 4, "d", da), ENOSPC)
	TEST_FAILURE(266, pack(pkt, 5, "d2", da, db), ENOSPC)
	TEST_FAILURE(267, pack(pkt, 6, "d*", 2, da, db), ENOSPC)

	TEST_FAILURE(268, pack(pkt, 1, "x0"), EINVAL)
	TEST_FAILURE(269, pack(pkt, 1, "x*", 0), EINVAL)
	TEST_FAILURE(270, pack(pkt, 1, "x*", -1), EINVAL)
	TEST_FAILURE(271, pack(pkt, 0, "x"), ENOSPC)
	TEST_FAILURE(272, pack(pkt, 1, "x2"), ENOSPC)
	TEST_FAILURE(273, pack(pkt, 1, "x*", 2), ENOSPC)

	TEST_FAILURE(274, pack(pkt, 1, "X0"), EINVAL)
	TEST_FAILURE(275, pack(pkt, 1, "X*", 0), EINVAL)
	TEST_FAILURE(272, pack(pkt, 1, "X*", -1), EINVAL)
	TEST_FAILURE(273, pack(pkt, 0, "X"), EINVAL)
	TEST_FAILURE(274, pack(pkt, 1, "X"), EINVAL)
	TEST_FAILURE(275, pack(pkt, 1, "X2"), EINVAL)
	TEST_FAILURE(276, pack(pkt, 1024, "X*", 2), EINVAL)

	TEST_FAILURE(281, pack(pkt, 1, "@0"), EINVAL)
	TEST_FAILURE(282, pack(pkt, 1, "@*", 0), EINVAL)
	TEST_FAILURE(283, pack(pkt, 1, "@*", -1), EINVAL)
	TEST_FAILURE(284, pack(pkt, 0, "@"), ENOSPC)
	TEST_FAILURE(285, pack(pkt, 0, "@1"), ENOSPC)
	TEST_FAILURE(286, pack(pkt, 1, "@2"), ENOSPC)
	TEST_FAILURE(287, pack(pkt, 1, "@*", 2), ENOSPC)

	TEST_FAILURE(288, pack(pkt, 1, "?", 0), EINVAL)

	TEST_FAILURE(289, unpack(NULL, 1, "a", a), EINVAL)
	TEST_FAILURE(290, unpack(pkt, 1, NULL, a), EINVAL)

	TEST_FAILURE(291, unpack(pkt, 1, "a0", a), EINVAL)
	TEST_FAILURE(292, unpack(pkt, 1, "a*", 0, a), EINVAL)
	TEST_FAILURE(293, unpack(pkt, 1, "a*", -1, a), EINVAL)
	TEST_FAILURE(294, unpack(pkt, 0, "a", a), ENOSPC)
	TEST_FAILURE(295, unpack(pkt, 1, "a2", a), ENOSPC)
	TEST_FAILURE(296, unpack(pkt, 1, "a*", 2, a), ENOSPC)
	TEST_FAILURE(297, unpack(pkt, 0, "a", NULL), ENOSPC)

	TEST_FAILURE(298, unpack(pkt, 1, "z0", z), EINVAL)
	TEST_FAILURE(299, unpack(pkt, 1, "z*", 0, z), EINVAL)
	TEST_FAILURE(300, unpack(pkt, 1, "z*", -1, z), EINVAL)
	TEST_FAILURE(301, unpack(pkt, 0, "z", z), ENOSPC)
	TEST_FAILURE(302, unpack(pkt, 1, "z2", z), ENOSPC)
	TEST_FAILURE(303, unpack(pkt, 1, "z*", 2, z), ENOSPC)
	TEST_FAILURE(304, unpack(pkt, 0, "z", NULL), ENOSPC)

	TEST_FAILURE(305, unpack(pkt, 1, "b0", b), EINVAL)
	TEST_FAILURE(306, unpack(pkt, 1, "b*", 0, b), EINVAL)
	TEST_FAILURE(307, unpack(pkt, 1, "b*", -1, b), EINVAL)
	TEST_FAILURE(308, unpack(pkt, 0, "b", b), ENOSPC)
	TEST_FAILURE(309, unpack(pkt, 1, "b9", b), ENOSPC)
	TEST_FAILURE(310, unpack(pkt, 1, "b*", 9, b), ENOSPC)
	TEST_FAILURE(311, unpack(pkt, 0, "b", NULL), ENOSPC)

	TEST_FAILURE(312, unpack(pkt, 1, "h0", h), EINVAL)
	TEST_FAILURE(313, unpack(pkt, 1, "h*", 0, h), EINVAL)
	TEST_FAILURE(314, unpack(pkt, 1, "h*", -1, h), EINVAL)
	TEST_FAILURE(315, unpack(pkt, 0, "h", h), ENOSPC)
	TEST_FAILURE(316, unpack(pkt, 1, "h3", h), ENOSPC)
	TEST_FAILURE(317, unpack(pkt, 1, "h*", 3, h), ENOSPC)
	TEST_FAILURE(318, unpack(pkt, 0, "h", NULL), ENOSPC)

	TEST_FAILURE(319, unpack(pkt, 1, "c0", &sc), EINVAL)
	TEST_FAILURE(320, unpack(pkt, 1, "c*", 0, &sc), EINVAL)
	TEST_FAILURE(321, unpack(pkt, 1, "c*", -1, &sc), EINVAL)
	TEST_FAILURE(322, unpack(pkt, 0, "c", &sc), ENOSPC)
	TEST_FAILURE(323, unpack(pkt, 1, "c2", &sc, &sc), ENOSPC)
	TEST_FAILURE(324, unpack(pkt, 1, "c*", 2, &sc, &sc), ENOSPC)

	TEST_FAILURE(325, unpack(pkt, 1, "c0", &uc), EINVAL)
	TEST_FAILURE(326, unpack(pkt, 1, "c*", 0, &uc), EINVAL)
	TEST_FAILURE(327, unpack(pkt, 1, "c*", -1, &uc), EINVAL)
	TEST_FAILURE(328, unpack(pkt, 0, "c", &uc), ENOSPC)
	TEST_FAILURE(329, unpack(pkt, 1, "c2", &uc, &uc), ENOSPC)
	TEST_FAILURE(330, unpack(pkt, 1, "c*", 2, &uc, &uc), ENOSPC)

	TEST_FAILURE(331, unpack(pkt, 1, "s0", &ss), EINVAL)
	TEST_FAILURE(332, unpack(pkt, 1, "s*", 0, &ss), EINVAL)
	TEST_FAILURE(333, unpack(pkt, 1, "s*", -1, &ss), EINVAL)
	TEST_FAILURE(334, unpack(pkt, 0, "s", &ss), ENOSPC)
	TEST_FAILURE(335, unpack(pkt, 1, "s", &ss), ENOSPC)
	TEST_FAILURE(336, unpack(pkt, 2, "s2", &ss, &ss), ENOSPC)
	TEST_FAILURE(337, unpack(pkt, 3, "s*", 2, &ss, &ss), ENOSPC)

	TEST_FAILURE(338, unpack(pkt, 1, "s0", &us), EINVAL)
	TEST_FAILURE(339, unpack(pkt, 1, "s*", 0, &us), EINVAL)
	TEST_FAILURE(340, unpack(pkt, 1, "s*", -1, &us), EINVAL)
	TEST_FAILURE(341, unpack(pkt, 0, "s", &us), ENOSPC)
	TEST_FAILURE(342, unpack(pkt, 1, "s", &us), ENOSPC)
	TEST_FAILURE(343, unpack(pkt, 2, "s2", &us, &us), ENOSPC)
	TEST_FAILURE(344, unpack(pkt, 3, "s*", 2, &us, &us), ENOSPC)

	TEST_FAILURE(345, unpack(pkt, 1, "i0", &si), EINVAL)
	TEST_FAILURE(346, unpack(pkt, 1, "i*", 0, &si), EINVAL)
	TEST_FAILURE(347, unpack(pkt, 1, "i*", -1, &si), EINVAL)
	TEST_FAILURE(348, unpack(pkt, 0, "i", &si), ENOSPC)
	TEST_FAILURE(349, unpack(pkt, 1, "i", &si), ENOSPC)
	TEST_FAILURE(350, unpack(pkt, 2, "i", &si), ENOSPC)
	TEST_FAILURE(351, unpack(pkt, 3, "i", &si), ENOSPC)
	TEST_FAILURE(352, unpack(pkt, 4, "i2", &si, &si), ENOSPC)
	TEST_FAILURE(353, unpack(pkt, 5, "i*", 2, &si, &si), ENOSPC)

	TEST_FAILURE(354, unpack(pkt, 1, "i0", &ui), EINVAL)
	TEST_FAILURE(355, unpack(pkt, 1, "i*", 0, &ui), EINVAL)
	TEST_FAILURE(356, unpack(pkt, 1, "i*", -1, &ui), EINVAL)
	TEST_FAILURE(357, unpack(pkt, 0, "i", &ui), ENOSPC)
	TEST_FAILURE(358, unpack(pkt, 1, "i", &ui), ENOSPC)
	TEST_FAILURE(359, unpack(pkt, 2, "i", &ui), ENOSPC)
	TEST_FAILURE(360, unpack(pkt, 3, "i", &ui), ENOSPC)
	TEST_FAILURE(361, unpack(pkt, 4, "i2", &ui, &ui), ENOSPC)
	TEST_FAILURE(362, unpack(pkt, 5, "i*", 2, &ui, &ui), ENOSPC)

	TEST_FAILURE(363, unpack(pkt, 1, "p0", &p), EINVAL)
	TEST_FAILURE(364, unpack(pkt, 1, "p*", 0, &p), EINVAL)
	TEST_FAILURE(365, unpack(pkt, 1, "p*", -1, &p), EINVAL)
	TEST_FAILURE(366, unpack(pkt, 0, "p", &p), ENOSPC)
	TEST_FAILURE(367, unpack(pkt, 1, "p", &p), ENOSPC)
	TEST_FAILURE(368, unpack(pkt, 2, "p", &p), ENOSPC)
	TEST_FAILURE(369, unpack(pkt, 3, "p", &p), ENOSPC)
	TEST_FAILURE(370, unpack(pkt, 4, "p2", &p, &p2), ENOSPC)
	TEST_FAILURE(371, unpack(pkt, 5, "p*", 2, &p, &pkt), ENOSPC)

#ifdef HAS_LONG_LONG
	TEST_FAILURE(372, unpack(pkt, 1, "l0", &sl), EINVAL)
	TEST_FAILURE(373, unpack(pkt, 1, "l*", 0, &sl), EINVAL)
	TEST_FAILURE(374, unpack(pkt, 1, "l*", -1, &sl), EINVAL)
	TEST_FAILURE(375, unpack(pkt, 0, "l", &sl), ENOSPC)
	TEST_FAILURE(376, unpack(pkt, 1, "l", &sl), ENOSPC)
	TEST_FAILURE(377, unpack(pkt, 2, "l", &sl), ENOSPC)
	TEST_FAILURE(378, unpack(pkt, 3, "l", &sl), ENOSPC)
	TEST_FAILURE(379, unpack(pkt, 4, "l", &sl), ENOSPC)
	TEST_FAILURE(380, unpack(pkt, 5, "l", &sl), ENOSPC)
	TEST_FAILURE(381, unpack(pkt, 6, "l", &sl), ENOSPC)
	TEST_FAILURE(382, unpack(pkt, 7, "l", &sl), ENOSPC)
	TEST_FAILURE(383, unpack(pkt, 8, "l2", &sl, &sl), ENOSPC)
	TEST_FAILURE(384, unpack(pkt, 9, "l*", 2, &sl, &sl), ENOSPC)

	TEST_FAILURE(385, unpack(pkt, 1, "l0", &ul), EINVAL)
	TEST_FAILURE(386, unpack(pkt, 1, "l*", 0, &ul), EINVAL)
	TEST_FAILURE(387, unpack(pkt, 1, "l*", -1, &ul), EINVAL)
	TEST_FAILURE(388, unpack(pkt, 0, "l", &ul), ENOSPC)
	TEST_FAILURE(389, unpack(pkt, 1, "l", &ul), ENOSPC)
	TEST_FAILURE(390, unpack(pkt, 2, "l", &ul), ENOSPC)
	TEST_FAILURE(391, unpack(pkt, 3, "l", &ul), ENOSPC)
	TEST_FAILURE(392, unpack(pkt, 4, "l", &ul), ENOSPC)
	TEST_FAILURE(393, unpack(pkt, 5, "l", &ul), ENOSPC)
	TEST_FAILURE(394, unpack(pkt, 6, "l", &ul), ENOSPC)
	TEST_FAILURE(395, unpack(pkt, 7, "l", &ul), ENOSPC)
	TEST_FAILURE(396, unpack(pkt, 8, "l2", &ul, &ul), ENOSPC)
	TEST_FAILURE(397, unpack(pkt, 9, "l*", 2, &ul, &ul), ENOSPC)
#else
	TEST_FAILURE(398, unpack(pkt, 8, "l", NULL), ENOSYS)
	TEST_FAILURE(399, unpack(pkt, 8, "l", NULL), ENOSYS)
#endif

	f = -1.5; /* packed length = 6 */
	pack(pkt, 1024, "ff", f, f); /* needs this to find length */
	TEST_FAILURE(400, unpack(pkt, 1, "f0", &f), EINVAL)
	TEST_FAILURE(401, unpack(pkt, 1, "f*", 0, &f), EINVAL)
	TEST_FAILURE(402, unpack(pkt, 1, "f*", -1, &f), EINVAL)
	TEST_FAILURE(403, unpack(pkt, 0, "f", &f), ENOSPC)
	TEST_FAILURE(404, unpack(pkt, 1, "f", &f), ENOSPC)
	TEST_FAILURE(405, unpack(pkt, 2, "f", &f), ENOSPC)
	TEST_FAILURE(406, unpack(pkt, 3, "f", &f), ENOSPC)
	TEST_FAILURE(407, unpack(pkt, 4, "f", &f), ENOSPC)
	TEST_FAILURE(408, unpack(pkt, 5, "f", &f), ENOSPC)
	TEST_FAILURE(409, unpack(pkt, 6, "f2", &f, &f), ENOSPC)
	TEST_FAILURE(410, unpack(pkt, 7, "f*", 2, &f, &f), ENOSPC)

	da = db = 1.5; /* packed length = 5 */
	pack(pkt, 1024, "dd", da, db); /* needs this to find length */
	TEST_FAILURE(411, unpack(pkt, 1, "d0", &da), EINVAL)
	TEST_FAILURE(412, unpack(pkt, 1, "d*", 0, &da), EINVAL)
	TEST_FAILURE(413, unpack(pkt, 1, "d*", -1, &da), EINVAL)
	TEST_FAILURE(414, unpack(pkt, 0, "d", &da), ENOSPC)
	TEST_FAILURE(415, unpack(pkt, 1, "d", &da), ENOSPC)
	TEST_FAILURE(416, unpack(pkt, 2, "d", &da), ENOSPC)
	TEST_FAILURE(417, unpack(pkt, 3, "d", &da), ENOSPC)
	TEST_FAILURE(418, unpack(pkt, 4, "d", &da), ENOSPC)
	TEST_FAILURE(419, unpack(pkt, 5, "d2", &da, &db), ENOSPC)
	TEST_FAILURE(420, unpack(pkt, 6, "d*", 2, &da, &db), ENOSPC)

	TEST_FAILURE(421, unpack(pkt, 1, "x0"), EINVAL)
	TEST_FAILURE(422, unpack(pkt, 1, "x*", 0), EINVAL)
	TEST_FAILURE(423, unpack(pkt, 1, "x*", -1), EINVAL)
	TEST_FAILURE(424, unpack(pkt, 0, "x"), ENOSPC)
	TEST_FAILURE(425, unpack(pkt, 1, "x2"), ENOSPC)
	TEST_FAILURE(426, unpack(pkt, 1, "x*", 2), ENOSPC)

	TEST_FAILURE(427, unpack(pkt, 1, "X0"), EINVAL)
	TEST_FAILURE(428, unpack(pkt, 1, "X*", 0), EINVAL)
	TEST_FAILURE(429, unpack(pkt, 1, "X*", -1), EINVAL)
	TEST_FAILURE(430, unpack(pkt, 0, "X"), EINVAL)
	TEST_FAILURE(431, unpack(pkt, 1, "X"), EINVAL)
	TEST_FAILURE(432, unpack(pkt, 1, "X2"), EINVAL)
	TEST_FAILURE(433, unpack(pkt, 1024, "X*", 2), EINVAL)

	TEST_FAILURE(434, unpack(pkt, 1, "@0"), EINVAL)
	TEST_FAILURE(435, unpack(pkt, 1, "@*", 0), EINVAL)
	TEST_FAILURE(436, unpack(pkt, 1, "@*", -1), EINVAL)
	TEST_FAILURE(437, unpack(pkt, 0, "@"), ENOSPC)
	TEST_FAILURE(438, unpack(pkt, 0, "@1"), ENOSPC)
	TEST_FAILURE(439, unpack(pkt, 1, "@2"), ENOSPC)
	TEST_FAILURE(440, unpack(pkt, 1, "@*", 2), ENOSPC)

	TEST_FAILURE(441, unpack(pkt, 1, "?", 0), EINVAL)

	/* Test truncation of string data */

#define TEST_TRUNC(i, fmt, size, pfmt, len, data1, data2, init) \
	memset(data2, init, 1024); \
	TEST_STR(i, fmt, size, pfmt, len, data1, data2) \
	if (data2[len] != 0) \
		++errors, printf("Test%d: unpack(%s, trunc) failed (%s[%d] == %d, not %d)\n", i, fmt, #fmt, len, ((char *)data1)[len], 0);

	TEST_TRUNC(442, "a3", 3, "%3.3s", 3, a, tstmem, '\0')
	TEST_TRUNC(443, "z3", 3, "%s", 3, z, tstmem, ' ')
	TEST_TRUNC(444, "b3", 1, "%s", 3, b, tstmem, ' ')
	TEST_TRUNC(445, "h3", 2, "%s", 3, h, tstmem, ' ')

	/* Test net_expect() and net_send() */

	server = net_server("localhost", 30001, NULL, NULL);
	if (server == -1)
		++errors, printf("Test446: net_server(\"localhost\", 30001) failed: %s\n", strerror(errno));
	else
	{
		switch (pid = fork())
		{
			case -1:
			{
				printf("Failed to fork (%s)\n", strerror(errno));
				return 1;
			}

			default:
			{
				int s;
				sockaddr_in addr[1];
				size_t addrlen = sizeof(sockaddr_in);

				alarm(5);
				s = accept(server, (sockaddr *)addr, &addrlen);
				if (s == -1)
					++errors, printf("Test447: accept() failed (%s)\n", strerror(errno));
				else
				{
					char test[4];
					ssize_t rc;

					if ((rc = net_expect(s, 5, "%s", test)) != 1)
						++errors, printf("Test448: net_expect(s, 5, HELO) failed (%s)\n", (rc == 0) ? "eof" : strerror(errno));
					else if (memcmp(test, "HELO", 4))
						++errors, printf("Test449: net_expect(s, 5, HELO) failed (received \"%4.4s\", not \"%4.4s\")\n", test, "HELO");
					else if (net_send(s, "OLEH") == -1)
						++errors, printf("Test450: net_send(s, OLEH) failed (%s)\n", strerror(errno));
					if (close(s) == -1)
						++errors, printf("Test451: close(s) failed (%s)\n", strerror(errno));
				}

				alarm(0);
				errors += wait_for_child(pid);
				break;
			}

			case 0:
			{
				int i, c = 0;
				errors = 0;

				for (i = 0; i < 5; ++i)
				{
					c = net_client("localhost", 30001, NULL, NULL);
					if (c != -1)
						break;
					sleep(1);
				}

				if (c == -1)
					++errors, printf("Test452: net_client(\"localhost\", 30001) failed (%s)\n", strerror(errno));
				else
				{
					char test[4];
					ssize_t rc;

					if (net_send(c, "HELO") == -1)
						++errors, printf("Test453: net_send(c, HELO) failed (%s)\n", strerror(errno));
					else if ((rc = net_expect(c, 5, "%s", test)) != 1)
						++errors, printf("Test454: net_expect(c, OLEH) failed (%s)\n", (rc == 0) ? "eof" : strerror(errno));
					else if (memcmp(test, "OLEH", 4))
						++errors, printf("Test455: net_expect(c, OLEH) failed (received \"%4.4s\", not \"%4.4s\")\n", test, "OLEH");
					if (close(c) == -1)
						++errors, printf("Test456: close(c) failed (%s)\n", strerror(errno));
				}

				return errors;
			}
		}

		if (close(server) == -1)
			++errors, printf("Test457: close(server) failed (%s)\n", strerror(errno));
	}

	/* Test net_pack(), net_packto(), net_unpack() and net_unpackfrom()*/

	server = net_udp_server("localhost", 30001, NULL, NULL);
	if (server == -1)
		++errors, printf("Test458: net_udp_server(\"localhost\", 30001) failed: %s\n", strerror(errno));
	else
	{
		switch (pid = fork())
		{
			case -1:
			{
				printf("Failed to fork (%s)\n", strerror(errno));
				return 1;
			}

			default:
			{
				int neti;
				char netz[5];
				sockaddr_in ipaddr;
				size_t ipsize = sizeof ipaddr;

				alarm(5);
				if (net_unpackfrom(server, 0, (sockaddr *)&ipaddr, &ipsize, "iz4", &neti, netz) == -1)
					++errors, printf("Test459: net_unpackfrom(server, \"iz4\", 37, HELO) failed (%s)\n", strerror(errno));
				else if (neti != 37)
					++errors, printf("Test460: net_unpackfrom(server, \"iz4\", 37, HELO) failed (%d != %d)\n", neti, 37);
				else if (strcmp(netz, "HELO"))
					++errors, printf("Test461: net_unpackfrom(server, \"iz4\", 37, HELO) failed (recv \"%4.4s\", not \"%4.4s\")\n", netz, "HELO");
				else if (net_packto(server, 0, (sockaddr *)&ipaddr, ipsize, "iz4", neti + 1, "OLEH") == -1)
					++errors, printf("Test462: net_packto(server, \"iz4\", 38, OLEH) failed (%s)\n", strerror(errno));
				alarm(0);
				errors += wait_for_child(pid);
				break;
			}

			case 0:
			{
				int i, c = 0;
				errors = 0;

				for (i = 0; i < 5; ++i)
				{
					c = net_udp_client("localhost", 30001, NULL, NULL);
					if (c != -1)
						break;
					sleep(1);
				}

				if (c == -1)
					++errors, printf("Test463: net_client(\"localhost\", 30001) failed (%s)\n", strerror(errno));
				else
				{
					int neti;
					char netz[4];
					alarm(5);
					if (net_pack(c, 0, "iz4", 37, "HELO") == -1)
						++errors, printf("Test464: net_pack(c, \"iz4\", 37, HELO) failed (%s)\n", strerror(errno));
					else if (net_unpack(c, 0, "iz4", &neti, netz) == -1)
						++errors, printf("Test465: net_unpack(c, 38, OLEH) failed (%s)\n", strerror(errno));
					else if (neti != 38)
						++errors, printf("Test466: net_unpack(c, 38, OLEH) failed (%d != %d)\n", neti, 38);
					else if (strcmp(netz, "OLEH"))
						++errors, printf("Test467: net_unpack(c, 38, OLEH) failed (recv \"%4.4s\", not \"%4.4s\")\n", netz, "OLEH");
					if (close(c) == -1)
						++errors, printf("Test468: close(c) failed (%s)\n", strerror(errno));
					alarm(0);
				}

				return errors;
			}
		}

		if (close(server) == -1)
			++errors, printf("Test469: close(server) failed (%s)\n", strerror(errno));
	}

	/* XXX Test net_chat()? */

	/* Test mail() */

	pwd = getpwuid(getuid());
	if (!pwd)
		++errors, printf("Test470: failed to test mail() (getpwuid() failed))\n");
	else
	{
		struct utsname utsbuf[1];
		char addr[128];

		if (uname(utsbuf) == -1)
			++errors, printf("Test470: failed to test mail() (uname() failed %s)\n", strerror(errno));
		else
		{
			snprintf(addr, 128, "%s@%s", pwd->pw_name, utsbuf->nodename);
			rc = mail(NULL, addr, addr, "subject", "message");
			if (rc != 0)
				++errors, printf("Test470: mail(NULL, \"%s\", \"%s\", \"%s\", \"%s\") failed: returned %d, not %d (%s)\n", addr, addr, "subject", "message", rc, 0, strerror(errno));
		}

		TEST_FAILURE(471, mail(NULL, addr, NULL, NULL, NULL), EINVAL)
		TEST_FAILURE(472, mail(NULL, NULL, addr, NULL, NULL), EINVAL)
	}

	if (!rfc822_localtime(pkt, 1024, t = time(NULL)))
		++errors, printf("Test473: rfc822_localtime() failed\n");
	else
	{
		struct tm tm = *localtime(&t);
		size_t size = strftime(tstmem, 1024, "%a, %d %b %Y %H:%M:%S ", &tm);
		snprintf(tstmem + size, 1024 - size, "%c%04d", "+-"[timezone > 0], abs(timezone) / 36);
		if (strcmp(pkt, tstmem))
			++errors, printf("Test474: rfc822_localtime() failed (returned \"%s\", not \"%s\")\n", pkt, tstmem);
	}

	if (!rfc822_gmtime(pkt, 1024, t = time(NULL)))
		++errors, printf("Test475: rfc822_gmtime() failed\n");
	else
	{
		struct tm tm = *gmtime(&t);
		strftime(tstmem, 1024, "%a, %d %b %Y %H:%M:%S GMT", &tm);
		if (strcmp(pkt, tstmem))
			++errors, printf("Test476: rfc822_gmtime() failed (returned \"%s\", not \"%s\")\n", pkt, tstmem);
	}

	if (errors)
		printf("%d/476 tests failed\n", errors);
	else
		printf("All tests passed\n");

	printf("\n");
	printf("    Note: can't verify mail delivery.\n");
	printf("    Look for mail consisting of: \"subject\" and \"message\"\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
