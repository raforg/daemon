/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999-2001 raf <raf@raf.org>
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
* 20010215 raf <raf@raf.org>
*/

/*

=head1 NAME

I<libslack(thread)> - thread module

=head1 SYNOPSIS

    #include <slack/thread.h>

    typedef struct Locker Locker;
    typedef int lockf_t(void *lock);

    Locker *locker_create_mutex(pthread_mutex_t *mutex);
    Locker *locker_create_rwlock(pthread_rwlock_t *rwlock);
    Locker *locker_create_debug_mutex(pthread_mutex_t *mutex);
    Locker *locker_create_debug_rwlock(pthread_rwlock_t *rwlock);
    Locker *locker_create(void *lock, lockf_t *tryrdlock, lockf_t *rdlock, lockf_t *trywrlock, lockf_t *wrlock, lockf_t *unlock);
    void locker_release(Locker *locker);
    void *locker_destroy(Locker **locker);
    int locker_tryrdlock(Locker *locker);
    int locker_rdlock(Locker *locker);
    int locker_trywrlock(Locker *locker);
    int locker_wrlock(Locker *locker);
    int locker_unlock(Locker *locker);

    int thread_attr_init(pthread_attr_t *attr);
    int thread_attr_set(pthread_attr_t *attr, int scope, int detachstate, int inheritsched, int schedpolicy, struct sched_param *schedparam);
    int thread_init();
    int thread_setcancel(int type, int state, int *oldtype, int *oldstate);

    int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
    int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
    int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
    int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
    int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
    int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
    int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
    int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

    int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
    int pthread_barrier_destroy(pthread_barrier_t *barrier);
    int pthread_barrier_wait(pthread_barrier_t *barrier);
    int pthread_barrierattr_init(pthread_barrierattr_t *attr);
    int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
    int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
    int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

=head1 DESCRIPTION

This module provides an abstraction of thread synchronisation that
facilitates the implementation of C<MT-Disciplined> libraries. I'll explain
what this means.

Libraries need to be C<MT-Safe> when used in a multi threaded program.
However, most programs are single threaded and synchronisation doesn't come
for free so libraries should be C<Unsafe> when used in a single threaded
program. Even in multi threaded programs, some functions or objects may only
be accessed by a single thread and so they should not incur the expense of
synchronisation.

When an object is shared between multiple threads which need to be
synchronised, the method of synchronisation must be carefully selected by
the client code. There are tradeoffs between concurrency and overhead. The
greater the concurrency, the greater the overhead. More locks give greater
concurrency but have greater overhead. Readers/Writer locks can give greater
concurrency than Mutex locks but have greater overhead. One lock for each
object may be required, or one lock for all (or a set of) objects may be
more appropriate.

Generally, the best synchronisation strategy for a given application can
only be determined by testing/benchmarking the written application. It is
important to be able to experiment with the synchronisation strategy at this
stage of development without pain.

The solution, of course, is to decouple the synchronisation strategy from
the library code. To facilitate this, the I<Locker> type and associated
functions can be incorporated into library code to provide the necessary
flexibility.

The I<Locker> type specifies a lock and a set of functions for manipulating
the lock. Arbitrary objects can include a pointer to a I<Locker> object to
use for thread synchronisation. Such objects may each have their own lock by
having separate I<Locker> objects or they may share the same lock by sharing
the same I<Locker> object. Only the application developer can determine what
is appropriate for each application on an case by case basis.

MT-Disciplined means that the application developer has a mechanism for
specifying the synchronisation requirements to be applied to library code.

This module also provides a few shorthand functions for using the I<pthread>
library as well as a few synchronisation variables that are defined in
recent standards but may not be on your system yet (i.e. readers/writer
locks and barriers).

=over 4

=cut

One of the nicest things about POSIX MT programs is how it simplifies signal
handling. A single thread can be devoted to handling signals synchronously
with I<sigwait(3)> while all other threads go about their business, free from
signals. Unfortunately, if you have a Linux system, you may have noticed
that the MT signal handling is not POSIX compliant. This is due to lack of
kernel support. This module provides a (non-transparent) remedy for Linux MT
signal handling that simulates this simplified signal handling method.
C<NOT IMPLEMENTED YET>

*/

#include "std.h"

#include "thread.h"
#include "mem.h"
#include "err.h"

struct Locker
{
	void *lock;
	lockf_t *tryrdlock;
	lockf_t *rdlock;
	lockf_t *trywrlock;
	lockf_t *wrlock;
	lockf_t *unlock;
};

#define try(action) { int rc = (action); if (rc != 0) return rc; }

/*

=item C<Locker *locker_create_mutex(pthread_mutex_t *lock)>

Creates a I<Locker> object that will operate on the mutex lock, C<lock>.
I<locker_tryrdlock()> and I<locker_trywrlock()> will call
I<pthread_mutex_trylock()>. I<locker_rdlock()> and I<locker_wrlock()> will
call I<pthread_mutex_lock()>. I<locker_unlock()> will call
I<pthread_mutex_unlock()>. It is the caller's responsibility to initialise
I<lock> if necessary before use and to destroy I<lock> if necessary after
use. On success, returns the new I<Locker> object. On error, returns
C<NULL>.

=cut

*/

Locker *locker_create_mutex(pthread_mutex_t *lock)
{
	return locker_create
	(
		lock,
		(lockf_t *)pthread_mutex_trylock,
		(lockf_t *)pthread_mutex_lock,
		(lockf_t *)pthread_mutex_trylock,
		(lockf_t *)pthread_mutex_lock,
		(lockf_t *)pthread_mutex_unlock
	);
}

/*

=item C<Locker *locker_create_rwlock(pthread_mutex_t *lock)>

Creates a I<Locker> object that will operate on the readers/writer lock,
C<lock>. I<locker_tryrdlock()> will call I<pthread_rwlock_tryrdlock()>.
I<locker_rdlock()> will call I<pthread_rwlock_rdlock()>.
I<locker_trywrlock()> will call I<pthread_rwlock_trywrlock()>.
I<locker_wrlock()> will call I<pthread_rwlock_wrlock()>. I<locker_unlock()>
will call I<pthread_rwlock_unlock()>. It is the caller's responsibility to
initialise I<lock> if necessary before use and to destroy I<lock> if
necessary after use. On success, returns the new I<Locker> object. On error,
returns C<NULL>.

=cut

*/

Locker *locker_create_rwlock(pthread_rwlock_t *lock)
{
	return locker_create
	(
		lock,
		(lockf_t *)pthread_rwlock_tryrdlock,
		(lockf_t *)pthread_rwlock_rdlock,
		(lockf_t *)pthread_rwlock_trywrlock,
		(lockf_t *)pthread_rwlock_wrlock,
		(lockf_t *)pthread_rwlock_unlock
	);
}

#ifndef NO_DEBUG_LOCKERS

/*

=item C<Locker *locker_create_mutex_debug(pthread_mutex_t *mutex)>

Just like I<locker_create_mutex()> except that debug messages are printed to
standard output before and after each locking function is called. The debug
messages look like "[thread id] funcname(mutex address) ...\n" and "[thread
id] funcname(mutex address) done\n". On success, returns the new I<Locker>.
On error, returns C<NULL>.

=cut

*/

static int debug_invoke(const char *name, lockf_t *action, void *lock)
{
	int err;

	printf("[%lu] %s(%p) ...\n", (unsigned long)pthread_self(), name, lock);
	err = action(lock);
	printf("[%lu] %s(%p) done\n", (unsigned long)pthread_self(), name, lock);
	return err;
}

static int debug_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	return debug_invoke("pthread_mutex_trylock", (lockf_t *)pthread_mutex_trylock, mutex);
}

static int debug_pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return debug_invoke("pthread_mutex_lock", (lockf_t *)pthread_mutex_lock, mutex);
}

static int debug_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return debug_invoke("pthread_mutex_unlock", (lockf_t *)pthread_mutex_unlock, mutex);
}

Locker *locker_create_debug_mutex(pthread_mutex_t *mutex)
{
	return locker_create
	(
		mutex,
		(lockf_t *)debug_pthread_mutex_trylock,
		(lockf_t *)debug_pthread_mutex_lock,
		(lockf_t *)debug_pthread_mutex_trylock,
		(lockf_t *)debug_pthread_mutex_lock,
		(lockf_t *)debug_pthread_mutex_unlock
	);
}

/*

=item C<Locker *locker_create_debug_rwlock(pthread_mutex_t *rwlock)>

Just like I<locker_create_rwlock()> except that debug messages are printed
to standard output before and after each locking function is called. The
debug messages look like "[thread id] funcname(rwlock address) ...\n" and
"[thread id] funcname(rwlock address) done\n". On success, returns the new
I<Locker>. On error, returns C<NULL>.

=cut

*/

static int debug_pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
	return debug_invoke("pthread_rwlock_tryrdlock", (lockf_t *)pthread_rwlock_tryrdlock, rwlock);
}

static int debug_pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
	return debug_invoke("pthread_rwlock_rdlock", (lockf_t *)pthread_rwlock_rdlock, rwlock);
}

static int debug_pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
	return debug_invoke("pthread_rwlock_trywrlock", (lockf_t *)pthread_rwlock_trywrlock, rwlock);
}

static int debug_pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
	return debug_invoke("pthread_rwlock_wrlock", (lockf_t *)pthread_rwlock_wrlock, rwlock);
}

static int debug_pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
	return debug_invoke("pthread_rwlock_unlock", (lockf_t *)pthread_rwlock_unlock, rwlock);
}

Locker *locker_create_debug_rwlock(pthread_rwlock_t *rwlock)
{
	return locker_create
	(
		rwlock,
		(lockf_t *)debug_pthread_rwlock_tryrdlock,
		(lockf_t *)debug_pthread_rwlock_rdlock,
		(lockf_t *)debug_pthread_rwlock_trywrlock,
		(lockf_t *)debug_pthread_rwlock_wrlock,
		(lockf_t *)debug_pthread_rwlock_unlock
	);
}

#endif

/*

=item C<Locker *locker_create(void *lock, lockf_t *tryrdlock, lockf_t *rdlock, lockf_t *trywrlock, lockf_t *wrlock, lockf_t *unlock)>

Creates a I<Locker> object that will operate on the synchronisation
variable, C<lock>. I<locker_tryrdlock()> will call C<tryrdlock>.
I<locker_rdlock()> will call C<rdlock>. I<locker_trywrlock()> will call
C<trywrlock>. I<locker_wrlock()> will call C<wrlock>. I<locker_unlock()>
will call C<unlock>. It is the caller's responsibility to initialise I<lock>
if necessary before use and to destroy I<lock> if necessary after use. On
success, returns the new I<Locker> object. On error, returns C<NULL>.

=cut

*/

Locker *locker_create(void *lock, lockf_t *tryrdlock, lockf_t *rdlock, lockf_t *trywrlock, lockf_t *wrlock, lockf_t *unlock)
{
	Locker *locker;

	if (!(locker = mem_new(Locker)))
		return NULL;

	locker->lock = lock;
	locker->tryrdlock = tryrdlock;
	locker->rdlock = rdlock;
	locker->trywrlock = trywrlock;
	locker->wrlock = wrlock;
	locker->unlock = unlock;

	return locker;
}

/*

=item C<void locker_release(Locker *locker)>

Releases (deallocates) C<locker>. It is the caller's responsibility to
destroy the synchronisation variable used by C<locker> if necessary.

=cut

*/

void locker_release(Locker *locker)
{
	if (!locker)
		return;

	mem_release(locker);
}

/*

=item C<void *locker_destroy(Locker **locker)>

Destroys (deallocates and sets to C<NULL>) C<locker>. Returns C<NULL>. It is
the caller's responsibility to destroy the synchronisation variable used by
C<locker> if necessary.

=cut

*/

void *locker_destroy(Locker **locker)
{
	if (locker && *locker)
	{
		locker_release(*locker);
		*locker = NULL;
	}

	return NULL;
}

/*

C<int locker_invoke(lockf_t *action, void *lock)>

Calls C<action> with the synchronisation variable, C<lock> as its argument.
On success, returns C<0>. On error, returns C<-1> with C<errno> set
appropriately.

*/

static int locker_invoke(lockf_t *action, void *lock)
{
	int err;

	if (!action)
		return set_errno(EINVAL);

	return (err = action(lock)) ? set_errno(err) : 0;
}

/*

=item C<int locker_tryrdlock(Locker *locker)>

Tries to claim a read lock on the synchronisation variable managed by
C<locker>. See I<pthread_mutex_trylock()> and I<pthread_rwlock_tryrdlock()>
for details. On success, returns C<0>. On error, returns C<-1> with C<errno>
set appropriately.

=cut

*/

int locker_tryrdlock(Locker *locker)
{
	if (!locker)
		return 0;

	return locker_invoke(locker->tryrdlock, locker->lock);
}

/*


=item C<int locker_rdlock(Locker *locker)>

Claims a read lock on the synchronisation variable managed by C<locker>. See
I<pthread_mutex_lock()> and I<pthread_rwlock_rdlock()> for details. On
success, returns C<0>. On error, returns C<-1> with C<errno> set
appropriately.

=cut

*/

int locker_rdlock(Locker *locker)
{
	if (!locker)
		return 0;

	return locker_invoke(locker->rdlock, locker->lock);
}

/*


=item C<int locker_trywrlock(Locker *locker)>

Tries to claim a write lock on the synchronisation variable managed by
C<locker>. See I<pthread_mutex_trylock()> and I<pthread_rwlock_trywrlock()>
for details. On success, returns C<0>. On error, returns C<-1> with C<errno>
set appropriately.

=cut

*/

int locker_trywrlock(Locker *locker)
{
	if (!locker)
		return 0;

	return locker_invoke(locker->trywrlock, locker->lock);
}

/*


=item C<int locker_wrlock(Locker *locker)>

Claims a write lock on the synchronisation variable managed by C<locker>.
See I<pthread_mutex_lock()> and I<pthread_rwlock_wrlock()> for details. On
success, returns C<0>. On error, returns C<-1> with C<errno> set
appropriately.

=cut

*/

int locker_wrlock(Locker *locker)
{
	if (!locker)
		return 0;

	return locker_invoke(locker->wrlock, locker->lock);
}

/*


=item C<int locker_unlock(Locker *locker)>

Unlocks the synchronisation variable managed by C<locker>. See
I<pthread_mutex_unlock()> and I<pthread_rwlock_unlock()> for details. On
success, returns C<0>. On error, returns C<-1> with C<errno> set
appropriately.

=cut

*/

int locker_unlock(Locker *locker)
{
	if (!locker)
		return 0;

	return locker_invoke(locker->unlock, locker->lock);
}

/*

=item C<int thread_attr_init(pthread_attr_t *attr)>

Initialises C<attr> for creating detached threads with system scope and
explicit "other" scheduling. I prefer this to the POSIX default attributes
(and Linux doesn't have process scope scheduling anyway). On success,
returns 0. On error, returns the error code from the underlying I<pthread>
library call that failed.

=cut

*/

int thread_attr_init(pthread_attr_t *attr)
{
	if (!attr)
		return EINVAL;

	return thread_attr_set(attr, PTHREAD_SCOPE_SYSTEM, PTHREAD_CREATE_DETACHED, PTHREAD_EXPLICIT_SCHED, SCHED_OTHER, NULL);
}

/*

=item C<int thread_attr_set(pthread_attr_t *attr, int scope, int detachstate, int inheritsched, int schedpolicy, struct sched_param *schedparam)>

Initialises C<attr> with the following arguments. Each argument is passed to
the I<pthread_attr_set> function whose name corresponds with its name. On
success, returns 0. On error, returns the error code from the underlying
I<pthread> library call that failed.

=cut

*/

int thread_attr_set(pthread_attr_t *attr, int scope, int detachstate, int inheritsched, int schedpolicy, struct sched_param *schedparam)
{
	if (!attr)
		return EINVAL;

	try(pthread_attr_init(attr))
	try(pthread_attr_setscope(attr, scope))
	try(pthread_attr_setdetachstate(attr, detachstate))
	try(pthread_attr_setinheritsched(attr, inheritsched))
	try(pthread_attr_setschedpolicy(attr, schedpolicy))

	if (schedparam)
		try(pthread_attr_setschedparam(attr, schedparam))

	return 0;
}

/*

=item C<int thread_init()>

Enables deferred cancellation on the current thread. This is the same as the
POSIX default and is therefore pointless on systems that implement POSIX
threads accurately. It's just here for those who like to be explicit. On
success, returns 0. On error, returns the error code from the underlying
I<pthread> library call that failed.

=cut

*/

int thread_init()
{
	try(thread_setcancel(PTHREAD_CANCEL_DEFERRED, PTHREAD_CANCEL_ENABLE, NULL, NULL))

	return 0;
}

/*

=item C<int thread_setcancel(int type, int state, int *oldtype, int *oldstate)>

Sets the cancellation type and state for the current thread to C<type> and
C<state>, respectively. If C<oldtype> is not C<NULL>, the previous
cancellation type is stored there. If C<oldstate> is not C<NULL>, the
previous cancellation state is stored there. On success, return 0. On error,
returns the error code from the underlying I<pthread> library call that
failed.

=cut

*/

int thread_setcancel(int type, int state, int *oldtype, int *oldstate)
{
	try(pthread_setcanceltype(type, oldtype))
	try(pthread_setcancelstate(state, oldstate))

	return 0;
}

#ifdef NEEDS_PTHREAD_RWLOCK

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
	pthread_mutexattr_t mutexattr;
	pthread_condattr_t condattr;
	int pshared;

	if (!rwlock)
		return EINVAL;

	try(pthread_mutexattr_init(&mutexattr))
	try(pthread_condattr_init(&condattr))

	if (attr)
		try(pthread_rwlockattr_getpshared(attr, &pshared))
	else
		pshared = PTHREAD_PROCESS_PRIVATE;

	try(pthread_mutexattr_setpshared(&mutexattr, pshared))
	try(pthread_condattr_setpshared(&condattr, pshared))

	try(pthread_mutex_init(&rwlock->lock, &mutexattr))
	try(pthread_cond_init(&rwlock->readers, &condattr))
	try(pthread_cond_init(&rwlock->writers, &condattr))

	rwlock->waiters = 0;
	rwlock->state = 0;

	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
	if (!rwlock)
		return EINVAL;

	try(pthread_mutex_destroy(&rwlock->lock))
	try(pthread_cond_destroy(&rwlock->readers))
	try(pthread_cond_destroy(&rwlock->writers))

	return 0;
}

static void rdlock_cleanup(void *arg)
{
	pthread_rwlock_t *rwlock = (pthread_rwlock_t *)arg;

	pthread_mutex_unlock(&rwlock->lock);
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
	if (!rwlock)
		return EINVAL;

	/* Wait until there are no active or queued writers */

	try(pthread_mutex_lock(&rwlock->lock))
	pthread_cleanup_push(rdlock_cleanup, rwlock);

	while (rwlock->state == -1 || rwlock->waiters)
		try(pthread_cond_wait(&rwlock->readers, &rwlock->lock))

	pthread_cleanup_pop(1);
	++rwlock->state;

	return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
	if (!rwlock)
		return EINVAL;

	/* Are there no active or waiting writers? */

	try(pthread_mutex_lock(&rwlock->lock))

	if (rwlock->state != -1 && !rwlock->waiters)
	{
		++rwlock->state;
		try(pthread_mutex_unlock(&rwlock->lock))

		return 0;
	}

	try(pthread_mutex_unlock(&rwlock->lock))

	return EBUSY;
}

static void wrlock_cleanup(void *arg)
{
	pthread_rwlock_t *rwlock = (pthread_rwlock_t *)arg;

	if (--rwlock->waiters == 0 && rwlock->state != -1)
		pthread_cond_broadcast(&rwlock->readers);

	pthread_mutex_unlock(&rwlock->lock);
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
	if (!rwlock)
		return EINVAL;

	try(pthread_mutex_lock(&rwlock->lock))
	++rwlock->waiters;
	pthread_cleanup_push(wrlock_cleanup, rwlock);

	while (rwlock->state != 0)
		try(pthread_cond_wait(&rwlock->writers, &rwlock->lock))

	rwlock->state = -1;
	pthread_cleanup_pop(1);

	return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
	if (!rwlock)
		return EINVAL;

	try(pthread_mutex_lock(&rwlock->lock))

	/* Are there any active or queued writers? */

	if (rwlock->state == 0 && rwlock->waiters == 0)
	{
		rwlock->state = -1;
		try(pthread_mutex_unlock(&rwlock->lock))

		return 0;
	}

	try(pthread_mutex_unlock(&rwlock->lock))

	return EBUSY;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
	if (!rwlock)
		return EINVAL;

	if (rwlock->state == -1)
	{
		rwlock->state = 0;

		if (rwlock->waiters)
			try(pthread_cond_signal(&rwlock->writers))
		else
			try(pthread_cond_broadcast(&rwlock->readers))
	}
	else
	{
		if (--rwlock->state == 0)
			try(pthread_cond_signal(&rwlock->writers))
	}

	try(pthread_mutex_unlock(&rwlock->lock))

	return 0;
}

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr)
{
	if (!attr)
		return EINVAL;

	attr->pshared = PTHREAD_PROCESS_PRIVATE;

	return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
	if (!attr)
		return EINVAL;

	return 0;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared)
{
	if (!attr || !pshared)
		return EINVAL;

	*pshared = attr->pshared;

	return 0;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared)
{
	if (!attr)
		return EINVAL;

	if (pshared != PTHREAD_PROCESS_PRIVATE && pshared != PTHREAD_PROCESS_SHARED)
		return EINVAL;

	attr->pshared = pshared;

	return 0;
}

#endif

#ifdef NEEDS_PTHREAD_BARRIER

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
	pthread_mutexattr_t mutexattr;
	pthread_condattr_t condattr;
	int pshared;

	if (!barrier)
		return EINVAL;

	try(pthread_mutexattr_init(&mutexattr))
	try(pthread_condattr_init(&condattr))

	if (attr)
		try(pthread_barrierattr_getpshared(attr, &pshared))
	else
		pshared = PTHREAD_PROCESS_PRIVATE;

	try(pthread_mutexattr_setpshared(&mutexattr, pshared))
	try(pthread_condattr_setpshared(&condattr, pshared))

	try(pthread_mutex_init(&barrier->lock, &mutexattr))
	try(pthread_cond_init(&barrier->cond, &condattr))

	barrier->count = count;
	barrier->sleepers = 0;
	barrier->releasing = 0;

	return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
	if (!barrier)
		return EINVAL;

	try(pthread_mutex_destroy(&barrier->lock))
	try(pthread_cond_destroy(&barrier->cond))

	return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
	int release = 0;

	try(pthread_mutex_lock(&barrier->lock))

	/* If previous cycle still releasing, wait */

	while (barrier->releasing)
		try(pthread_cond_wait(&barrier->cond, &barrier->lock))

	/* Start release if we are the last thread to arrive, otherwise wait */

	if (++barrier->sleepers == barrier->count)
		release = barrier->releasing = 1;
	else
		while (!barrier->releasing)
			try(pthread_cond_wait(&barrier->cond, &barrier->lock))

	if (--barrier->sleepers == 0)
		barrier->releasing = 0, release = 1;

	try(pthread_mutex_unlock(&barrier->lock))

	/* Wake up waiters (if any) for next cycle */

	if (release)
		try(pthread_cond_broadcast(&barrier->cond))

	return 0;
}

int pthread_barrierattr_init(pthread_barrierattr_t *attr)
{
	if (!attr)
		return EINVAL;

	attr->pshared = PTHREAD_PROCESS_PRIVATE;

	return 0;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *attr)
{
	if (!attr)
		return EINVAL;

	return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared)
{
	if (!attr || !pshared)
		return EINVAL;

	*pshared = attr->pshared;

	return 0;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared)
{
	if (!attr)
		return EINVAL;

	if (pshared != PTHREAD_PROCESS_PRIVATE && pshared != PTHREAD_PROCESS_SHARED)
		return EINVAL;

	attr->pshared = pshared;

	return 0;
}

#endif

/*

=back

=head1 MT-Level

MT-Safe

=head1 SEE ALSO

L<libslack(3)|libslack(3)>,
L<pthread_attr_init(3)|pthread_attr_init(3)>,
L<pthread_attr_setscope(3)|pthread_attr_setscope(3)>,
L<pthread_attr_setdetachstate(3)|pthread_attr_setdetachstate(3)>,
L<pthread_attr_setinheritsched(3)|pthread_attr_setinheritsched(3)>,
L<pthread_attr_setschedpolicy(3)|pthread_attr_setschedpolicy(3)>,
L<pthread_attr_setschedparam(3)|pthread_attr_setschedparam(3)>,
L<pthread_setcanceltype(3)|pthread_setcanceltype(3)>,
L<pthread_setcancelstate(3)|pthread_setcancelstate(3)>

=head1 AUTHOR

20010215 raf <raf@raf.org>

=cut

*/

#ifdef TEST

int main(int ac, char **av)
{
	/* XXX Test thread module */

	return 0;
}

#endif

/* vi:set ts=4 sw=4: */
