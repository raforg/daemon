#
# libslack - http://libslack.org/
#
# Copyright (C) 1999, 2000 raf <raf@raf.org>
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
# 20000902 raf <raf@raf.org>

# Uncomment these to override the defines in daemon.h and prog.h
#
# SLACK_DEFINES += -DPATH_SEP=\'/\'
# SLACK_DEFINES += -DROOT_DIR=\"/\"
# SLACK_DEFINES += -DETC_DIR=\"/etc\"
# SLACK_DEFINES += -DPID_DIR=\"/var/run\"

# Uncomment this if your system doesn't have snprintf()
#
# SNPRINTF := snprintf
# SLACK_DEFINES += -DNEEDS_SNPRINTF=1

# Uncomment this if your system doesn't have GNU getopt_long()
#
# GETOPT := getopt
# SLACK_DEFINES += -DNEEDS_GETOPT=1

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

# Uncomment this if your system has the "long long int" type (64 bit)
# and you want to include the long format ("l") in pack() and unpack().
#
SLACK_DEFINES += -DHAS_LONG_LONG -Wno-long-long

SLACK_NAME := slack
SLACK_VERSION := 0.3
SLACK_ID := lib$(SLACK_NAME)-$(SLACK_VERSION)
SLACK_DIST := $(SLACK_ID).tar.gz
SLACK_HAS_SUBTARGETS := 0

SLACK_TARGET := $(SLACK_SRCDIR)/lib$(SLACK_NAME).a
SLACK_INSTALL := $(SLACK_ID).a
SLACK_INSTALL_LINK := lib$(SLACK_NAME).a
SLACK_MODULES := conf daemon err fifo $(GETOPT) hsort lim list log map mem msg net opt prog prop sig $(SNPRINTF) str $(VSSCANF)
SLACK_HEADERS := lib hdr socks

SLACK_CFILES := $(patsubst %, $(SLACK_SRCDIR)/%.c, $(SLACK_MODULES))
SLACK_OFILES := $(patsubst %, $(SLACK_SRCDIR)/%.o, $(SLACK_MODULES))
SLACK_HFILES := $(patsubst %, $(SLACK_SRCDIR)/%.h, $(SLACK_MODULES) $(SLACK_HEADERS))
SLACK_PODFILES := $(SLACK_CFILES)
SLACK_MANFILES := $(patsubst %.c, %.$(LIB_MANSECT), $(SLACK_PODFILES))
SLACK_HTMLFILES := $(patsubst %.c, %.$(LIB_MANSECT).html, $(SLACK_PODFILES))

SLACK_TESTDIR := $(SLACK_SRCDIR)/test
SLACK_TESTS := $(patsubst %, $(SLACK_TESTDIR)/%.test, $(SLACK_MODULES))

SLACK_INCLINK := $(SLACK_SRCDIR)/$(SLACK_NAME)

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

CLEAN_FILES += $(SLACK_OFILES) $(SLACK_MANFILES) $(SLACK_HTMLFILES) $(SLACK_SRCDIR)/pod2html-*
CLOBBER_FILES += $(SLACK_TARGET) $(SLACK_TESTDIR) $(SLACK_SRCDIR)/tags

SLACK_DEFINES +=
SLACK_CPPFLAGS += $(SLACK_DEFINES) $(patsubst %, -I%, $(SLACK_INCDIRS))
SLACK_CCFLAGS += $(CCFLAGS)
SLACK_CFLAGS += $(SLACK_CPPFLAGS) $(SLACK_CCFLAGS)
SLACK_LIBS += $(SLACK_NAME)

# Uncomment this on Solaris for the daemon and net modules
#
# SLACK_LIBS += xnet

SLACK_LDFLAGS += $(patsubst %, -L%, $(SLACK_LIBDIRS)) $(patsubst %, -l%, $(SLACK_LIBS))

