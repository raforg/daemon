/*
 * Revision 12: http://theos.com/~deraadt/snprintf.c
 *
 * Copyright (C) 1997 Theo de Raadt
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*

=head1 NAME

I<snprintf(3)> - safe sprintf

=head1 SYNOPSIS

    #include <slack/snprintf.h>

    int snprintf(char *str, size_t n, const char *fmt, ...);
    int vsnprintf(char *str, size_t n, const char *fmt, va_list args);

=head1 DESCRIPTION

Safe version of I<sprintf(3)>.

=over 4

=cut

*/

#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdio.h>
#if __STDC__
#include <stdarg.h>
#include <stdlib.h>
#else
#include <varargs.h>
#endif
#include <setjmp.h>

#ifndef roundup
#define roundup(x, y) ((((x)+((y)-1))/(y))*(y))
#endif

static int pgsize;
static char *curobj;
static sigjmp_buf bail;

#define EXTRABYTES      2       /* XXX: why 2? you don't want to know */

static char *
msetup(str, n)
	char *str;
	size_t n;
{
	char *e;

	if (n == 0)
		return NULL;
	if (pgsize == 0)
		pgsize = getpagesize();
	curobj = (char *)malloc(n + EXTRABYTES + pgsize * 2);
	if (curobj == NULL)
		return NULL;
	e = curobj + n + EXTRABYTES;
	e = (char *)roundup((unsigned long)e, pgsize);
	if (mprotect(e, pgsize, PROT_NONE) == -1) {
		free(curobj);
		curobj = NULL;
		return NULL;
	}
	e = e - n - EXTRABYTES;
	*e = '\0';
	return (e);
}

static void
mcatch()
{
	siglongjmp(bail, 1);
}

static void
mcleanup(str, n, p)
	char *str;
	size_t n;
	char *p;
{
	/* strncpy(str, p, n-1); */
	memcpy(str, p, n-1); /* To allow for strings containing nuls - raf */
	str[n-1] = '\0';
	if (mprotect((caddr_t)(p + n + EXTRABYTES), pgsize,
	    PROT_READ|PROT_WRITE|PROT_EXEC) == -1)
		mprotect((caddr_t)(p + n + EXTRABYTES), pgsize,
		    PROT_READ|PROT_WRITE);
	free(curobj);
}

/*

=item C<int snprintf(char *str, size_t n, const char *fmt, ...)>

Writes output to the string C<str>, under control of the format string
C<fmt>, that specifies how subsequent arguments are converted for output. It
is similar to I<sprintf(3)>, except that C<n> specifies the maximum number
of characters to produce. The trailing C<nul> character is counted towards
this limit, so you must allocate at least C<n> characters for the string
C<str>.

On success, returns the number of bytes written. If overflow occurred,
returns C<n + 1>. If C<fmt> is C<NULL>, returns C<-1>. If there are signal
handling errors, returns 0. If there are memory errors, returns 0 but a nul
byte is written to C<str>(???).

=cut

*/

int
#if __STDC__
snprintf(char *str, size_t n, const char *fmt, ...)
#else
snprintf(str, n, fmt, va_alist)
	char *str;
	size_t n;
	char *fmt;
	va_dcl
#endif
{
	int rc;
	va_list args;
#if __STDC__
	va_start(args, fmt);
#else
	va_start(args);
#endif

	rc = vsnprintf(str, n, fmt, args);
	va_end(args);
	return rc;
}

/*

=item C<int vsnprintf(char *str, size_t n, const char *fmt, va_list args)>

Equivalent to I<snprintf()> with the variable argument list specified
directly as for I<vsprintf(3)>.

=cut

*/

int
#if __STDC__
vsnprintf(char *str, size_t n, const char *fmt, va_list args)
#else
vsnprintf(str, n, fmt, args)
	char *str;
	size_t n;
	char *fmt;
	va_list args;
#endif
{
	struct sigaction osa, nsa;
	char *p;
	int ret = n + 1;	/* if we bail, indicated we overflowed */

	if (!fmt)
		return (-1);

	memset(&nsa, 0, sizeof nsa);
	nsa.sa_handler = mcatch;
	sigemptyset(&nsa.sa_mask);

	p = msetup(str, n);
	if (p == NULL) {
		*str = '\0';
		return 0;
	}
	if (sigsetjmp(bail, 1) == 0) {
		if (sigaction(SIGSEGV, &nsa, &osa) == -1) {
			mcleanup(str, n, p);
			return (0);
		}
		ret = vsprintf(p, fmt, args);
	}
	mcleanup(str, n, p);
	(void) sigaction(SIGSEGV, &osa, NULL);
	return (ret);
}

/*

=back

=head1 SEE ALSO

L<sprintf(3)|sprintf(3)>,
L<vsprintf(3)|vsprintf(3)>

=head1 AUTHOR

Theo de Raadt <deraadt@theos.com>

=cut

*/

#ifdef TEST

int main(int ac, char **av)
{
	const char * const letters = "abcdefghijklmnopqrstuvwxyz";
	const size_t length = strlen(letters);
	int errors = 0;
	char buf[32];
	int rc;

	printf("Testing: snprintf\n");

	rc = snprintf(buf, 32, NULL);
	if (rc != -1)
		++errors, printf("Test1: snprintf(buf, 32, NULL) == %d (not -1)\n", rc);

	rc = snprintf(buf, 32, "");
	if (rc != 0)
		++errors, printf("Test2: snprintf(buf, 32, \"\") == %d (not 0)\n", rc);

	rc = snprintf(buf, 32, "%s", "");
	if (rc != 0)
		++errors, printf("Test3: snprintf(buf, 32, \"%%s\", \"\") == %d (not 0)\n", rc);

	rc = snprintf(buf, 32, "%s\n", letters);
	if (rc != length + 1)
		++errors, printf("Test4: snprintf(buf, 32, \"%%s\\n\", letters) == %d (not strlen(letters) + 1)\n", rc);

	rc = snprintf(buf, 32, "%31.31s\n", "");
	if (rc != 32)
		++errors, printf("Test5: snprintf(buf, 32, \"%%31.31s\\n\", \"\") == %d (not 32)\n", rc);

	rc = snprintf(buf, 32, "%s%s\n", letters, letters);
	if (rc != 33)
		++errors, printf("Test6: snprintf(buf, 32, \"%%s%%s\\n\", letters, letters) == %d (not 33)\n", rc);

	rc = snprintf(buf, 32, "abc%cdef\n", '\0');
	if (rc != 8)
		++errors, printf("Test7: snprintf(buf, 32, \"abc%%cdef\\n\", '\\0') == %d (not 8)\n", rc);
	if (buf[2] != 'c')
		++errors, printf("Test7: snprintf(buf, 32, \"abc%%cdef\\n\", '\\0') failed (char %d is %d not %d)\n", 2, buf[2], 'c');
	if (buf[3] != '\0')
		++errors, printf("Test7: snprintf(buf, 32, \"abc%%cdef\\n\", '\\0') failed (char %d is %d not %d)\n", 3, buf[3], '\0');
	if (buf[4] != 'd')
		++errors, printf("Test7: snprintf(buf, 32, \"abc%%cdef\\n\", '\\0') failed (char %d is %d not %d)\n", 4, buf[4], 'd');

	if (errors)
		printf("%d/7 tests failed\n", errors);
	else
		printf("All tests passed\n");

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
