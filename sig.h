/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef SIG_H
#define SIG_H

typedef void sighandler_t(int signo);

#ifdef __cplusplus
extern "C" {
#endif

int signal_in(int signo);
int signal_set_handler(int signo, int flags, sighandler_t *handler);
int signal_received(int signo);
int signal_handle(int signo);
void signal_handle_all(void);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
