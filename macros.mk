#
# daemon - http://libslack.org/daemon/
#
# Copyright (C) 1999-2001 raf <raf@raf.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# or visit http://www.gnu.org/copyleft/gpl.html
#

# 20030901 raf <raf@raf.org>

# Uncomment this to override the default value of 600 seconds
# as the minimum amount of time that a client can live if it
# is to be respawned
#
# DAEMON_DEFINES += -DRESPAWN_THRESHOLD=600

# Uncomment this to override the default configuration file path
#
# DAEMON_DEFINES += -DCONFIG_PATH=\"/etc/daemon.conf\"

# Uncomment this to disable debugging completely
#
# DAEMON_DEFINES += -DNDEBUG

DAEMON_NAME := daemon
DAEMON_VERSION := 0.6.1
DAEMON_DATE := 20030901
DAEMON_URL := http://libslack.org/daemon/
DAEMON_ID := $(DAEMON_NAME)-$(DAEMON_VERSION)
DAEMON_DIST := $(DAEMON_ID).tar.gz
DAEMON_HTML_DIST := $(DAEMON_ID).html.tar.gz

DAEMON_DEFINES += -DDAEMON_NAME=\"$(DAEMON_NAME)\"
DAEMON_DEFINES += -DDAEMON_VERSION=\"$(DAEMON_VERSION)\"
DAEMON_DEFINES += -DDAEMON_DATE=\"$(DAEMON_DATE)\"
DAEMON_DEFINES += -DDAEMON_URL=\"$(DAEMON_URL)\"

# Uncomment this if your system has POSIX threads reader/writer locks.
#
DAEMON_DEFINES += -DHAVE_SNPRINTF=1
DAEMON_DEFINES += -DHAVE_VSSCANF=1
DAEMON_DEFINES += -DHAVE_GETOPT_LONG=1
DAEMON_DEFINES += -DHAVE_PTHREAD_RWLOCK=1
# DAEMON_DEFINES += -DNO_POSIX_SOURCE=1
# DAEMON_DEFINES += -DNO_XOPEN_SOURCE=1

DAEMON_TARGET := $(DAEMON_SRCDIR)/$(DAEMON_NAME)
DAEMON_MODULES := daemon

DAEMON_HTMLDIR := $(DATA_INSDIR)/$(DAEMON_NAME)/doc

DAEMON_CONFDIR := $(CONF_INSDIR)

DAEMON_CFILES := $(patsubst %, $(DAEMON_SRCDIR)/%.c, $(DAEMON_MODULES))
DAEMON_OFILES := $(patsubst %, $(DAEMON_SRCDIR)/%.o, $(DAEMON_MODULES))
DAEMON_PODFILES := $(DAEMON_CFILES)
DAEMON_MANFILES := $(patsubst %.c, %.$(APP_MANSECT), $(DAEMON_PODFILES))
DAEMON_HTMLFILES := $(patsubst %.c, %.$(APP_MANSECT).html, $(DAEMON_PODFILES))
DAEMON_CONFFILE := $(DAEMON_NAME).conf
DAEMON_MANLINK := $(DAEMON_CONFFILE).$(FMT_MANSECT)

ifeq ($(MAN_GZIP), 1)
DAEMON_MANFILES := $(patsubst %, %.gz, $(DAEMON_MANFILES))
DAEMON_MANLINK := $(patsubst %, %.gz, $(DAEMON_MANLINK))
endif

TAG_FILES += $(DAEMON_HFILES) $(DAEMON_CFILES)
DEPEND_CFILES += $(DAEMON_CFILES)
DEPEND_HFILES += $(DAEMON_HFILES)

ifeq ($(DAEMON_SRCDIR), .)
DAEMON_MAIN := 1
endif

ALL_TARGETS += daemon
MAN_TARGETS += man-daemon
HTML_TARGETS += html-daemon
ifeq ($(DAEMON_MAIN), 1)
INSTALL_TARGETS += install-daemon
UNINSTALL_TARGETS += uninstall-daemon
endif
DIST_TARGETS += dist-daemon
RPM_TARGETS += rpm-daemon
DEB_TARGETS += deb-daemon
SOL_TARGETS += sol-daemon
OBSD_TARGETS += obsd-daemon
FBSD_TARGETS += fbsd-daemon

CLEAN_FILES += $(DAEMON_OFILES) $(DAEMON_MANFILES) $(DAEMON_HTMLFILES) $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE)
CLOBBER_FILES += $(DAEMON_TARGET) $(DAEMON_SRCDIR)/tags $(DAEMON_SRCDIR)/debian configure-stamp build-stamp
DEBIAN_CLOBBER_FILES += $(DAEMON_TARGET) $(DAEMON_SRCDIR)/tags

DAEMON_RPM_FILES += $(patsubst %, $(APP_INSDIR)/%, $(notdir $(DAEMON_TARGET)))
DAEMON_RPM_DOCFILES += $(patsubst %, $(APP_MANDIR)/%, $(notdir $(DAEMON_MANFILES)))
DAEMON_RPM_DOCFILES += $(patsubst %, $(FMT_MANDIR)/%, $(notdir $(DAEMON_MANLINK)))
DAEMON_SOL := RAFOdmn

DAEMON_CPPFLAGS += $(DAEMON_DEFINES) $(patsubst %, -I%, $(DAEMON_INCDIRS))
DAEMON_CCFLAGS += $(CCFLAGS)
# DAEMON_LDFLAGS += -pthread
DAEMON_CFLAGS += $(DAEMON_CPPFLAGS) $(DAEMON_CCFLAGS)
DAEMON_LIBS += slack
DAEMON_LIBS += pthread
DAEMON_LIBS += util

# Uncomment these on Solaris for sockets
#
# DAEMON_LIBS += xnet
# DAEMON_LIBS += socket
# DAEMON_LIBS += nsl

DAEMON_LDFLAGS += $(patsubst %, -L%, $(DAEMON_LIBDIRS)) $(patsubst %, -l%, $(DAEMON_LIBS))

SLACK_SRCDIR := libslack
SLACK_INCDIRS := libslack
SLACK_LIBDIRS := libslack
include $(SLACK_SRCDIR)/macros.mk

DAEMON_SUBTARGETS := $(SLACK_TARGET)
DAEMON_SUBDIRS := $(SLACK_SRCDIR) $(SLACK_SUBDIRS)

