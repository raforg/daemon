#
# daemon: http://www.zip.com.au/~raf2/lib/software/daemon
#
# Copyright (C) 1999 raf <raf2@zip.com.au>
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

# Uncomment these to override the defines in daemon.h and prog.h
#
# PROG_DEFINES += -DPATH_SEP=\'/\'
# PROG_DEFINES += -DROOT_DIR=\"/\"
# PROG_DEFINES += -DETC_DIR=\"/etc\"
# PROG_DEFINES += -DPID_DIR=\"/var/run\"

# Uncomment this if your system doesn't have snprintf()
#
# SNPRINTF := snprintf
# PROG_DEFINES += -DNEEDS_SNPRINTF=1

# Uncomment this if your system doesn't have GNU getopt_long_only()
#
# GETOPT := getopt
# PROG_DEFINES += -DNEEDS_GETOPT=1

# Uncomment one of these on SVR4 if required (see next)
#
# PROG_DEFINES += -DSVR4
# PROG_DEFINES += -USVR4

# Uncomment this to prevent an extra fork on SVR4 which prevents
# the process from ever gaining a controlling terminal. If this is
# uncommented, the O_NOCTTY flag should be passed to all calls to
# open(2) made by the process (as is required on BSD anyway)
#
# PROG_DEFINES += -DNO_EXTRA_SVR4_FORK=1

# Uncomment this to override the default value of 8192 bytes
# as the size for message buffers
#
# PROG_DEFINES += -DMSG_SIZE=8192

# Uncomment this to override the default value of 101 as the
# number of hash buckets for Map (must be prime)
#
# PROG_DEFINES += -DMAP_SIZE=101

PROG_NAME := prog
PROG_VERSION := 0.2
PROG_ID := $(PROG_NAME)-$(PROG_VERSION)
PROG_DIST := $(PROG_ID).tar.gz

PROG_TARGET := $(PROG_SRCDIR)/lib$(PROG_NAME).a
PROG_INSTALL := lib$(PROG_ID).a
PROG_INSTALL_LINK := lib$(PROG_NAME).a
PROG_MODULES := conf daemon err fifo $(GETOPT) hsort lim list log map mem msg opt prog prop sig $(SNPRINTF)
PROG_HEADERS := hdr

PROG_CFILES := $(patsubst %, $(PROG_SRCDIR)/%.c, $(PROG_MODULES))
PROG_OFILES := $(patsubst %, $(PROG_SRCDIR)/%.o, $(PROG_MODULES))
PROG_HFILES := $(patsubst %, $(PROG_SRCDIR)/%.h, $(PROG_MODULES) $(PROG_HEADERS))
PROG_PODFILES := $(PROG_CFILES)
PROG_MANFILES := $(patsubst %.c, %.$(LIB_MANSECT), $(PROG_PODFILES))
PROG_HTMLFILES := $(patsubst %.c, %.$(LIB_MANSECT).html, $(PROG_PODFILES))

PROG_TESTDIR := $(PROG_SRCDIR)/test
PROG_TESTS := $(patsubst %, $(PROG_TESTDIR)/%.test, $(PROG_MODULES))

PROG_INCLINK := $(PROG_SRCDIR)/$(PROG_NAME)

TAG_FILES += $(PROG_HFILES) $(PROG_CFILES)
DEPEND_HFILES += $(PROG_HFILES)
DEPEND_CFILES += $(PROG_CFILES)

READY_TARGETS += ready-prog
ALL_TARGETS += prog
TEST_TARGETS += test-prog
MAN_TARGETS += man-prog
HTML_TARGETS += html-prog
INSTALL_TARGETS += install-prog
UNINSTALL_TARGETS += uninstall-prog

CLEAN_FILES += $(PROG_OFILES) $(PROG_MANFILES) $(PROG_HTMLFILES)
CLOBBER_FILES += $(PROG_TARGET) $(PROG_TESTDIR)

PROG_DEFINES +=
PROG_CPPFLAGS += $(PROG_DEFINES) $(patsubst %, -I%, $(PROG_INCDIRS))
PROG_CCFLAGS += $(CCFLAGS)
PROG_CFLAGS += $(PROG_CPPFLAGS) $(PROG_CCFLAGS)
PROG_LIBS += $(PROG_NAME)

# Uncomment this on Solaris for getsockopt(3)
#
# PROG_LIBS += xnet

PROG_LDFLAGS += $(patsubst %, -L%, $(PROG_LIBDIRS)) $(patsubst %, -l%, $(PROG_LIBS))

