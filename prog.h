/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef PROG_H
#define PROG_H

#include <stdarg.h>

#ifdef BUILD_PROG
#include "opt.h"
#include "msg.h"
#else
#include <prog/opt.h>
#include <prog/msg.h>
#endif

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif

#ifdef __cplusplus
extern "C" {
#endif

void prog_init(void);
const char *prog_set_name(const char *name);
Options *prog_set_options(Options *options);
const char *prog_set_syntax(const char *syntax);
const char *prog_set_desc(const char *desc);
const char *prog_set_version(const char *version);
const char *prog_set_date(const char *date);
const char *prog_set_author(const char *author);
const char *prog_set_contact(const char *contact);
const char *prog_set_vendor(const char *vendor);
const char *prog_set_url(const char *url);
const char *prog_set_legal(const char *legal);
Msg *prog_set_out(Msg *out);
Msg *prog_set_err(Msg *err);
Msg *prog_set_dbg(Msg *dbg);
size_t prog_set_debug_level(size_t level);
const char *prog_name(void);
const Options *prog_options(void);
const char *prog_syntax(void);
const char *prog_desc(void);
const char *prog_version(void);
const char *prog_date(void);
const char *prog_author(void);
const char *prog_contact(void);
const char *prog_vendor(void);
const char *prog_url(void);
const char *prog_legal(void);
Msg *prog_out(void);
Msg *prog_err(void);
Msg *prog_dbg(void);
size_t prog_debug_level(void);
int prog_out_fd(int fd);
int prog_out_stdout(void);
int prog_out_file(const char *path);
int prog_out_syslog(const char *ident, int option, int facility);
int prog_out_none(void);
int prog_err_fd(int fd);
int prog_err_stderr(void);
int prog_err_file(const char *path);
int prog_err_syslog(const char *ident, int option, int facility);
int prog_err_none(void);
int prog_dbg_fd(int fd);
int prog_dbg_stdout(void);
int prog_dbg_stderr(void);
int prog_dbg_file(const char *path);
int prog_dbg_syslog(const char *id, int option, int facility);
int prog_dbg_none(void);
int prog_opt_process(int ac, char **av);
void prog_usage_msg(const char *fmt, ...);
const char *prog_basename(const char *path);

extern Options prog_options_table[1];

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
