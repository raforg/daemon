/*
* daemon: http://www.zip.com.au/~raf2/lib/software/daemon
*
* Copyright (c) 1999 raf
*/

#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

#include <unistd.h>
#include <errno.h>

#include <sys/wait.h>

#ifdef BUILD_PROG
#include "daemon.h"
#include "prog.h"
#include "err.h"
#include "mem.h"
#include "opt.h"
#else
#include <prog/daemon.h>
#include <prog/prog.h>
#include <prog/err.h>
#include <prog/mem.h>
#include <prog/opt.h>
#endif

static struct
{
	char **command; /* command vector to execute */
	pid_t pid;      /* the pid of the child process to run as a daemon */
}
g;

void terminate(int signo)
{
	if (g.pid)
		kill(0, SIGTERM);

	daemon_close();
	exit(0);
}

void stop(void)
{
	terminate(0);
}

char *daemon_name = NULL;

static Option main_optab[] =
{
	{ "name", "name", "Sets the daemon name for the pid file", required_argument, OPT_STRING, OPT_VARIABLE, &daemon_name },
	{ NULL, NULL, NULL, 0, 0, 0, NULL }
};

static Options options[1] = {{ prog_options_table, 0, main_optab }};

#include <stdio.h>

void init(int ac, char **av)
{
	int i;
	int a;

	prog_init();
	prog_set_name(prog_basename(av[0]));
	prog_set_options(options);
	prog_set_version("0.1");
	prog_set_desc
	(
		"Executes another command as a daemon. Useful for running scripts as\n"
		"daemons. If the -n option is supplied, the name given is used to\n"
		"create and lock a file ensuring that only one daemon with the given\n"
		"name is active at the same time.\n"
	);

	a = prog_opt_process(ac, av);

	if (a == ac)
		prog_usage_msg("Invalid arguments - no command supplied");

	if (prog_err_syslog(NULL, 0, LOG_LOCAL5 | LOG_DEBUG) == -1)
		fatal("failed to start error delivery");

	if (daemon_init(daemon_name, NULL, terminate) == -1)
		fatalsys("failed to start daemon");

	g.command = mem_create(ac, char*);
	g.command[ac - 1] = NULL;

	for (i = 0; a < ac; ++i, ++a)
		g.command[i] = av[a];
}

void prog(void)
{
	switch (g.pid = fork())
	{
		case -1:
			fatalsys("couldn't fork()");

		case 0:
			execvp(g.command[0], g.command);
			errorsys("couldn't execvp(%s)", g.command[0]);
			_exit(1);
	}

	for (;;)
	{
		pid_t pid;

		signal_handle_all();
		pid = waitpid(g.pid, NULL, 0);
		if (pid == g.pid || (pid == -1 && errno != EINTR))
			stop();
	}
}

int main(int ac, char **av)
{
	init(ac, av);
	prog();
	return 0;
}

/* vi:set ts=4 sw=4: */
