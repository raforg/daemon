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
* 20011109 raf <raf@raf.org>
*/

#ifndef LIBSLACK_AGENT_H
#define LIBSLACK_AGENT_H

#include <slack/locker.h>

typedef struct Agent Agent;
typedef int agent_action_t(Agent *agent, void *arg);
typedef int agent_reaction_t(Agent *agent, int fd, int revents, void *arg);

_begin_decls
Agent *agent_create _args ((void));
Agent *agent_create_with_locker _args ((Locker *locker));
Agent *agent_create_measured _args ((void));
Agent *agent_create_measured_with_locker _args ((Locker *locker));
Agent *agent_create_using_select _args ((void));
Agent *agent_create_using_select_with_locker _args ((Locker *locker));
void agent_release _args ((Agent *agent));
void *agent_destroy _args ((Agent **agent));
int agent_rdlock _args ((const Agent *agent));
int agent_wrlock _args ((const Agent *agent));
int agent_unlock _args ((const Agent *agent));
int agent_connect _args ((Agent *agent, int fd, int events, agent_reaction_t *reaction, void *arg));
int agent_connect_unlocked _args ((Agent *agent, int fd, int events, agent_reaction_t *reaction, void *arg));
int agent_disconnect _args ((Agent *agent, int fd));
int agent_disconnect_unlocked _args ((Agent *agent, int fd));
int agent_transfer _args ((Agent *agent, int fd, Agent *dst));
int agent_transfer_unlocked _args ((Agent *agent, int fd, Agent *dst));
int agent_send _args ((Agent *agent, int fd, int sockfd));
int agent_send_unlocked _args ((Agent *agent, int fd, int sockfd));
int agent_recv _args ((Agent *agent, int sockfd, agent_reaction_t *reaction, void *arg));
int agent_recv_unlocked _args ((Agent *agent, int sockfd, agent_reaction_t *reaction, void *arg));
int agent_detail _args ((Agent *agent, int fd));
int agent_detail_unlocked _args ((Agent *agent, int fd));
const struct timeval * const agent_last _args ((Agent *agent, int fd));
const struct timeval * const agent_last_unlocked _args ((Agent *agent, int fd));
int agent_velocity _args ((Agent *agent, int fd));
int agent_velocity_unlocked _args ((Agent *agent, int fd));
int agent_acceleration _args ((Agent *agent, int fd));
int agent_acceleration_unlocked _args ((Agent *agent, int fd));
int agent_dadt _args ((Agent *agent, int fd));
int agent_dadt_unlocked _args ((Agent *agent, int fd));
void *agent_schedule _args ((Agent *agent, long sec, long usec, agent_action_t *action, void *arg));
void *agent_schedule_unlocked _args ((Agent *agent, long sec, long usec, agent_action_t *action, void *arg));
int agent_cancel _args ((Agent *agent, void *action_id));
int agent_cancel_unlocked _args ((Agent *agent, void *action_id));
int agent_start _args ((Agent *agent));
int agent_stop _args ((Agent *agent));
_end_decls

#endif

/* vi:set ts=4 sw=4: */
