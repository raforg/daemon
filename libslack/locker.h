/*
* libslack - http://libslack.org/
*
* Copyright (C) 1999-2002 raf <raf@raf.org>
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
* 20020916 raf <raf@raf.org>
*/

#ifndef LIBSLACK_LOCKER_H
#define LIBSLACK_LOCKER_H

#include <pthread.h>

#include <slack/hdr.h>

typedef struct Locker Locker;
typedef int lockerf_t(void *lock);

#ifndef HAVE_PTHREAD_RWLOCK

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

_begin_decls
Locker *locker_create_mutex _args ((pthread_mutex_t *mutex));
Locker *locker_create_rwlock _args ((pthread_rwlock_t *rwlock));
Locker *locker_create_debug_mutex _args ((pthread_mutex_t *mutex));
Locker *locker_create_debug_rwlock _args ((pthread_rwlock_t *rwlock));
Locker *locker_create _args ((void *lock, lockerf_t *tryrdlock, lockerf_t *rdlock, lockerf_t *trywrlock, lockerf_t *wrlock, lockerf_t *unlock));
void locker_release _args ((Locker *locker));
void *locker_destroy _args ((Locker **locker));
int locker_tryrdlock _args ((Locker *locker));
int locker_rdlock _args ((Locker *locker));
int locker_trywrlock _args ((Locker *locker));
int locker_wrlock _args ((Locker *locker));
int locker_unlock _args ((Locker *locker));
#ifndef HAVE_PTHREAD_RWLOCK
int pthread_rwlock_init _args ((pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr));
int pthread_rwlock_destroy _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_rdlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_tryrdlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_wrlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_trywrlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlock_unlock _args ((pthread_rwlock_t *rwlock));
int pthread_rwlockattr_init _args ((pthread_rwlockattr_t *attr));
int pthread_rwlockattr_destroy _args ((pthread_rwlockattr_t *attr));
int pthread_rwlockattr_getpshared _args ((const pthread_rwlockattr_t *attr, int *pshared));
int pthread_rwlockattr_setpshared _args ((pthread_rwlockattr_t *attr, int pshared));
#endif
_end_decls

/* Don't look below here - optimisations only */

struct Locker
{
	void *lock;
	lockerf_t *tryrdlock;
	lockerf_t *rdlock;
	lockerf_t *trywrlock;
	lockerf_t *wrlock;
	lockerf_t *unlock;
};

#define locker_tryrdlock(locker) ((locker) ? (locker)->tryrdlock((locker)->lock) : 0)
#define locker_rdlock(locker)    ((locker) ? (locker)->rdlock((locker)->lock) : 0)
#define locker_trywrlock(locker) ((locker) ? (locker)->trywrlock((locker)->lock) : 0)
#define locker_wrlock(locker)    ((locker) ? (locker)->wrlock((locker)->lock) : 0)
#define locker_unlock(locker)    ((locker) ? (locker)->unlock((locker)->lock) : 0)

#endif

/* vi:set ts=4 sw=4: */
