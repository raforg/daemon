/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef SLOG_H
#define SLOG_H

#ifdef __cplusplus
extern "C" {
#endif

int syslog_lookup_facility(const char *facility);
int syslog_lookup_priority(const char *priority);
const char *syslog_facility_str(int spec);
const char *syslog_priority_str(int spec);
int syslog_parse(const char *spec, int *facility, int *priority);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
