/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#ifndef OPT_H
#define OPT_H

#include <getopt.h>

typedef struct option option;
typedef struct Option Option;
typedef struct Options Options;

typedef void (*opt_action_int_t)(int);
typedef void (*opt_action_string_t)(const char *);
typedef void (*opt_action_none_t)(void);

enum OptionArgument
{
	OPT_NONE,
	OPT_INTEGER,
	OPT_STRING
};

enum OptionAction
{
	OPT_NOTHING,
	OPT_VARIABLE,
	OPT_FUNCTION
};

typedef enum OptionArgument OptionArgument;
typedef enum OptionAction OptionAction;

struct Option
{
	const char *name;
	const char *argname;
	const char *desc;
	int has_arg;
	OptionArgument arg_type;
	OptionAction action;
	void *object;
};

struct Options
{
	Options *parent;
	size_t length;
	Option *options;
};

#ifdef __cplusplus
extern "C" {
#endif

int opt_process(int argc, char **argv, Options *options);
char *opt_usage(char *buf, size_t size, Options *options);

#ifdef __cplusplus
}
#endif

#endif

/* vi:set ts=4 sw=4: */
