/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef MSG_H
#define MSG_H

#include <stdarg.h>

typedef struct Msg Msg;

#ifdef __cplusplus
extern "C" {
#endif

void msg_release(Msg *msg);
void *msg_destroy_fn(Msg **msg);
void vmsg_out(Msg *dst, const char *fmt, va_list args);
void msg_out(Msg *dst, const char *fmt, ...);
Msg *msg_create_fd(int fd);
Msg *msg_create_stderr(void);
Msg *msg_create_stdout(void);
Msg *msg_create_file(const char *path);
Msg *msg_create_syslog(const char *ident, int option, int facility);
Msg *msg_create_plex(Msg *msg1, Msg *msg2);
int msg_add_plex(Msg *msg, Msg *item);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
