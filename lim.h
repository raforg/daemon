/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef LIM_H
#define LIM_H

#ifdef __cplusplus
extern "C" {
#endif

long limit_arg(void);
long limit_child(void);
long limit_tick(void);
long limit_group(void);
long limit_open(void);
long limit_stream(void);
long limit_tzname(void);
long limit_job(void);
long limit_save_ids(void);
long limit_version(void);
long limit_pcanon(const char *path);
long limit_fcanon(int fd);
long limit_canon(void);
long limit_pinput(const char *path);
long limit_finput(int fd);
long limit_input(void);
long limit_pvdisable(const char *path);
long limit_fvdisable(int fd);
long limit_vdisable(void);
long limit_plink(const char *path);
long limit_flink(int fd);
long limit_link(void);
long limit_pname(const char *path);
long limit_fname(int fd);
long limit_name(void);
long limit_ppath(const char *path);
long limit_fpath(int fd);
long limit_path(void);
long limit_ppipe(const char *path);
long limit_fpipe(int fd);
long limit_pnotrunc(const char *path);
long limit_fnotrunc(int fd);
long limit_notrunc(void);
long limit_pchown(const char *path);
long limit_fchown(int fd);
long limit_chown(void);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
