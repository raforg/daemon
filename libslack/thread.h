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

#ifndef LIBSLACK_THREAD_H
#define LIBSLACK_THREAD_H

#include <pthread.h>

#include <slack/hdr.h>

typedef struct Locker Locker;
typedef int lockf_t(void *lock);

#ifdef NEEDS_PTHREAD_RWLOCK

typedef struct pthread_rwlock_t pthread_rwlock_t;
typedef struct pthread_rwlockattr_t pthread_rwlockattr_t;

struct pthread_rwlock_t
{
	pthread_mutex_t lock;   /* Lock for structure */
	pthread_cond_t readers; /* Are there readers waiting? */
	pthread_cond_t writers; /* Are there writers waiting? */
	int waiters;            /* Number of writers waiting */
	int state;              /* State: -1 -> writer, 0 -> idle, +ve -> readers */
};

struct pthread_rwlockattr_t
{
	int pshared;            /* Shared between processes or not */
};

#define PTHREAD_RWLOCK_INITIALIZER \
	{ \
		PTHREAD_MUTEX_INITIALIZER, \
		PTHREAD_COND_INITIALIZER, \
		PTHREAD_COND_INITIALIZER, \
		0, 0 \
	}

#endif

#ifdef NEEDS_PTHREAD_BARRIER

typedef struct pthread_barrier_t pthread_barrier_t;
typedef struct pthread_barrierattr_t pthread_barrierattr_t;

struct pthread_barrier_t
{
	pthread_mutex_t lock;  /* Lock for structure */
	pthread_cond_t cond;   /* Waiting list control */
	int count;             /* Number of threads to wait for */
	int sleepers;          /* Number of threads waiting */
	int releasing;         /* Still waking up sleepers */
};

struct pthread_barrierattr_t
{
	int pshared;           /* Shared between processes or not */
};

#define PTHREAD_BARRIER_INITIALIZER \
	{ \
		PTHREAD_MUTEX_INITIALIZER, \
		PTHREAD_COND_INITIALIZER, \
		0, 0, 0 \
	}

#endif

_start_decls
Locker *locker_create_mutex _args ((pthread_mutex_t *mutex));
Locker *locker_create_rwlock _args ((pthread_rwlock_t *rwlock));
Locker *locker_create_debug_mutex _args ((pthread_mutex_t *mutex));
Locker *locker_create_debug_rwlock _args ((pthread_rwlock_t *rwlock));
Locker *locker_create _args ((void *lock, lockf_t *tryrdlock, lockf_t *rdlock, lockf_t *trywrlock, lockf_t *wrlock, lockf_t *unlock));
void locker_release _args ((Locker *locker));
void *locker_destroy _args ((Locker **locker));
int locker_tryrdlock _args ((Locker *locker));
int locker_rdlock _args ((Locker *locker));
int locker_trywrlock _args ((Locker *locker));
int locker_wrlock _args ((Locker *locker));
int locker_unlock _args ((Locker *locker));
int thread_attr_init _args ((pthread_attr_t *attr));
int thread_attr_set _args ((pthread_attr_t *attr, int scope, int detachstate, int inheritsched, int schedpolicy, struct sched_param *schedparam));
int thread_init _args (());
int thread_setcancel _args ((int type, int state, int *oldtype, int *oldstate));
#ifdef NEEDS_PTHREAD_RWLOCK
int pthread_rwlock_init _args ((pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr));
int pthread_rwlock_destroy _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_rdlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_tryrdlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_wrlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_trywrlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_unlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlockattr_init _args ((pthread_rwlockattr_t *attr));
int pthread_rwlockattr_destroy _args ((pthread_rwlockattr_t *attr));
int pthread_rwlockattr_getpshared _args ((const pthread_rwlockattr_t * attr, int *pshared));
int pthread_rwlockattr_setpshared _args ((pthread_rwlockattr_t *attr, int pshared));
#endif
#ifdef NEEDS_PTHREAD_BARRIER
int pthread_barrier_init _args ((pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count));
int pthread_barrier_destroy _args ((pthread_barrier_t *barrier));
int pthread_barrier_wait _args ((pthread_barrier_t *barrier));
int pthread_barrierattr_init _args ((pthread_barrierattr_t *attr));
int pthread_barrierattr_destroy _args ((pthread_barrierattr_t *attr));
int pthread_barrierattr_getpshared _args ((const pthread_barrierattr_t *attr, int *pshared));
int pthread_barrierattr_setpshared _args ((pthread_barrierattr_t *attr, int pshared));
#endif
_end_decls

#endif

/* vi:set ts=4 sw=4: */
