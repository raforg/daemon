/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef DAEMON_H
#define DAEMON_H

#ifdef BUILD_PROG
#include "sig.h"
#else
#include <prog/sig.h>
#endif

#ifndef PID_DIR
#define PID_DIR "/var/run"
#endif

#ifndef ROOT_DIR
#define ROOT_DIR "/"
#endif

#ifndef ETC_DIR
#define ETC_DIR "/etc"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int daemon_started_by_init(void);
int daemon_started_by_inetd(void);
int daemon_init(const char *name, sighandler_t *hup, sighandler_t *term);
int daemon_close();

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
