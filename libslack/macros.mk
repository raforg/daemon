#
# libslack - http://libslack.org/
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
# 20010215 raf <raf@raf.org>

# Uncomment these to override the defines in daemon.h and prog.h
#
# SLACK_DEFINES += -DPATH_SEP=\'/\'
# SLACK_DEFINES += -DROOT_DIR=\"/\"
# SLACK_DEFINES += -DETC_DIR=\"/etc\"
# SLACK_DEFINES += -DROOT_PID_DIR=\"/var/run\"
# SLACK_DEFINES += -DUSER_PID_DIR=\"/tmp\"

# Uncomment this if your system doesn't have snprintf()
#
# SNPRINTF := snprintf
# SLACK_DEFINES += -DNEEDS_SNPRINTF=1

# Uncomment this if your system doesn't have GNU getopt_long()
#
# GETOPT := getopt
# SLACK_DEFINES += -DNEEDS_GETOPT=1
# SLACK_CLIENT_CFLAGS += -DNEEDS_GETOPT=1

# Uncomment this if your system doesn't have vsscanf()
#
# VSSCANF := vsscanf
# SLACK_DEFINES += -DNEEDS_VSSCANF=1

# Uncomment this if your system uses SOCKS
#
# SLACK_DEFINES += -DSOCKS=1

# Uncomment this if your system doesn't have POSIX 1003.2 compliant regular
# expression functions and you don't want to download them (see README).
#
# SLACK_DEFINES += -DREGEX_MISSING=1
# SLACK_CLIENT_CFLAGS += -DREGEX_MISSING=1

# Uncomment this if mlock() on your system requires that its first argument
# be a page boundary.
#
# SLACK_DEFINES += -DMLOCK_NEEDS_PAGE_BOUNDARY=1

# Uncomment these if your system doesn't have strcasecmp() and strncasecmp().
#
# SLACK_DEFINES += -DNEEDS_STRCASECMP=1
# SLACK_DEFINES += -DNEEDS_STRNCASECMP=1

# Uncomment these if your system doesn't have strlcpy() and strlcat().
#
SLACK_DEFINES += -DNEEDS_STRLCPY=1
SLACK_DEFINES += -DNEEDS_STRLCAT=1

# Uncomment these if your system doesn't have POSIX threads reader/writer
# locks or barriers, respectively.
#
# SLACK_DEFINES += -DNEEDS_PTHREAD_RWLOCK=1
# SLACK_DEFINES += -DNEEDS_PTHREAD_BARRIER=1
# SLACK_CLIENT_CFLAGS += -DNEEDS_PTHREAD_RWLOCK=1
# SLACK_CLIENT_CFLAGS += -DNEEDS_PTHREAD_BARRIER=1

# Uncomment one of these on SVR4 if required (see next)
#
# SLACK_DEFINES += -DSVR4
# SLACK_DEFINES += -USVR4

# Uncomment this to prevent an extra fork on SVR4 which prevents
# the process from ever gaining a controlling terminal. If this is
# uncommented, the O_NOCTTY flag should be passed to all calls to
# open(2) made by the process (as is required on BSD anyway)
#
# SLACK_DEFINES += -DNO_EXTRA_SVR4_FORK=1

# Uncomment this to override the default value of 8192 bytes
# as the size for message buffers
#
# SLACK_DEFINES += -DMSG_SIZE=8192

# Uncomment this to override the default value of 32 as the maximum
# number of dimensions of an allocated space
#
# SLACK_DEFINES += -DMEM_MAX_DIM=32

# Uncomment these if your system has the "long long int" type (64 bit)
# and you want to include the long format ("l") in pack() and unpack().
#
SLACK_DEFINES += -DWANT_LONG_LONG=1
SLACK_CCFLAGS += -Wno-long-long

# Uncomment this to exclude compilation of the debug locker functions.
# These functions shamefully assume that pthread_self() can be cast into
# an unsigned long. If this is not true on on your system, uncomment this
# define or change the code.
#
# SLACK_DEFINES += -DNO_DEBUG_LOCKERS=1

SLACK_NAME := slack
SLACK_VERSION := 0.4
SLACK_URL := http://libslack.org/
SLACK_ID := lib$(SLACK_NAME)-$(SLACK_VERSION)
SLACK_DIST := $(SLACK_ID).tar.gz

SLACK_TARGET := $(SLACK_SRCDIR)/lib$(SLACK_NAME).a
SLACK_INSTALL := $(SLACK_ID).a
SLACK_INSTALL_LINK := lib$(SLACK_NAME).a
SLACK_CONFIG := $(SLACK_SRCDIR)/lib$(SLACK_NAME)-config
SLACK_MODULES := daemon err fio $(GETOPT) hsort lim list map mem msg net prog prop sig $(SNPRINTF) str thread $(VSSCANF)
SLACK_HEADERS := std lib hdr socks
SLACK_LIB_PODS := libslack
SLACK_APP_PODS := libslack-config

SLACK_HTMLDIR := $(DATA_INSDIR)/lib$(SLACK_NAME)/doc

SLACK_CFILES := $(patsubst %, $(SLACK_SRCDIR)/%.c, $(SLACK_MODULES))
SLACK_OFILES := $(patsubst %, $(SLACK_SRCDIR)/%.o, $(SLACK_MODULES))
SLACK_HFILES := $(patsubst %, $(SLACK_SRCDIR)/%.h, $(SLACK_MODULES) $(SLACK_HEADERS))

SLACK_LIB_PODNAMES := $(patsubst %.pod, %, $(SLACK_LIB_PODS)) $(SLACK_MODULES)
SLACK_LIB_MANFILES := $(patsubst %, $(SLACK_SRCDIR)/%.$(LIB_MANSECT), $(SLACK_LIB_PODNAMES))
SLACK_LIB_HTMLFILES := $(patsubst %, $(SLACK_SRCDIR)/%.$(LIB_MANSECT).html, $(SLACK_LIB_PODNAMES))

SLACK_APP_PODNAMES := $(patsubst %.pod, %, $(SLACK_APP_PODS))
SLACK_APP_MANFILES := $(patsubst %, $(SLACK_SRCDIR)/%.$(APP_MANSECT), $(SLACK_APP_PODNAMES))
SLACK_APP_HTMLFILES := $(patsubst %, $(SLACK_SRCDIR)/%.$(APP_MANSECT).html, $(SLACK_APP_PODNAMES))

SLACK_TESTDIR := $(SLACK_SRCDIR)/test
SLACK_TESTS := $(patsubst %, $(SLACK_TESTDIR)/%, $(SLACK_MODULES))

SLACK_INCLINK := $(SLACK_SRCDIR)/$(SLACK_NAME)

ifeq ($(FINAL_PREFIX),)
FINAL_PREFIX := $(PREFIX)
endif

TAG_FILES += $(SLACK_HFILES) $(SLACK_CFILES)
DEPEND_HFILES += $(SLACK_HFILES)
DEPEND_CFILES += $(SLACK_CFILES)

ALL_TARGETS += slack
READY_TARGETS += ready-slack
TEST_TARGETS += test-slack
MAN_TARGETS += man-slack
HTML_TARGETS += html-slack
INSTALL_TARGETS += install-slack
UNINSTALL_TARGETS += uninstall-slack
DIST_TARGETS += dist-slack
RPM_TARGETS += rpm-slack
DEB_TARGETS += deb-slack
PKG_TARGETS += pkg-slack

CLEAN_FILES += $(SLACK_OFILES) $(SLACK_CONFIG) $(SLACK_LIB_MANFILES) $(SLACK_APP_MANFILES) $(SLACK_LIB_HTMLFILES) $(SLACK_APP_HTMLFILES) $(SLACK_SRCDIR)/pod2html-*
CLOBBER_FILES += $(SLACK_TARGET) $(SLACK_SRCDIR)/tags $(SLACK_TESTDIR)

SLACK_RPM_FILES += $(LIB_INSDIR)/$(SLACK_INSTALL_LINK)
SLACK_RPM_FILES += $(LIB_INSDIR)/$(SLACK_INSTALL)
SLACK_RPM_FILES += $(APP_INSDIR)/$(notdir $(SLACK_CONFIG))
SLACK_RPM_FILES += $(patsubst %, $(HDR_INSDIR)/$(SLACK_NAME)/%, $(notdir $(SLACK_HFILES)))
SLACK_RPM_DOCFILES += $(patsubst %, $(APP_MANDIR)/%, $(notdir $(SLACK_APP_MANFILES)))
SLACK_RPM_DOCFILES += $(patsubst %, $(LIB_MANDIR)/%, $(notdir $(SLACK_LIB_MANFILES)))
SLACK_RPM_DOCFILES += $(foreach MODULE, $(SLACK_MODULES), $(patsubst %, $(LIB_MANDIR)/%.$(LIB_MANSECT), $(shell perl -n -e 'print $$1, "\n" if /^=item C<(?:const )?\w+[\s*]*(\w+)\(.*\)>$$/ or /^=item C< \#define (\w+)\(.*\)>$$/' "$(SLACK_SRCDIR)/$(MODULE).c")))
SLACK_PKG := RAFOslk

SLACK_CPPFLAGS += $(SLACK_DEFINES) $(patsubst %, -I%, $(SLACK_INCDIRS))
SLACK_CCFLAGS += $(CCFLAGS)
SLACK_CFLAGS += $(SLACK_CPPFLAGS) $(SLACK_CCFLAGS)
SLACK_LIBS += $(SLACK_NAME) pthread
SLACK_CLIENT_LIBS += $(SLACK_NAME) pthread

# Uncomment this on Solaris for sockets (used by the daemon and net modules)
#
# SLACK_LIBS += xnet
# SLACK_CLIENT_LIBS += xnet

# Uncomment this on Solaris for semaphores (used by tests)
#
# SLACK_LIBS += posix4

# Uncomment this on Solaris if you are using Sun's C compiler (used by tests)
#
# SLACK_LIBS += m

SLACK_LDFLAGS += $(patsubst %, -L%, $(SLACK_LIBDIRS)) $(patsubst %, -l%, $(SLACK_LIBS))
SLACK_CLIENT_LDFLAGS += $(patsubst %, -l%, $(SLACK_CLIENT_LIBS))

SLACK_SUBTARGETS :=
SLACK_SUBDIRS :=

