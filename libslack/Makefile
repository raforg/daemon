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

CC := gcc
TEST := /usr/bin/test
PREFIX := /usr/local
APP_INSDIR := $(PREFIX)/bin
LIB_INSDIR := $(PREFIX)/lib
MAN_INSDIR := $(PREFIX)/man
HDR_INSDIR := $(PREFIX)/include
APP_MANSECT := 1
LIB_MANSECT := 3
APP_MANDIR := $(MAN_INSDIR)/man$(APP_MANSECT)
APP_CATDIR := $(MAN_INSDIR)/cat$(APP_MANSECT)
LIB_MANDIR := $(MAN_INSDIR)/man$(LIB_MANSECT)
LIB_CATDIR := $(MAN_INSDIR)/cat$(LIB_MANSECT)
APP_MANSECTNAME := User Commands
LIB_MANSECTNAME := C Library Functions - libslack

CCFLAGS += -O2 -Wall -pedantic

CLEAN_FILES += tags core Makefile.bak .makefile.bak MANIFEST pod2html-*

SLACK_IS_ROOT := 1
SLACK_SRCDIR := .
SLACK_INCDIRS := .
SLACK_LIBDIRS := .
include $(SLACK_SRCDIR)/macros.mk

.PHONY: all ready test man html install uninstall

all: ready $(ALL_TARGETS)
ready: $(READY_TARGETS)
test: ready $(TEST_TARGETS)
man: $(MAN_TARGETS)
html: $(HTML_TARGETS)
install: $(INSTALL_TARGETS)
uninstall: $(UNINSTALL_TARGETS)

.PHONY: help help-macros depend dep clean clobber distclean dist

help::
	@echo "This makefile provides the following targets."
	@echo
	@echo "make help                  -- shows this list of targets"
	@echo "make help-macros           -- shows the values of all make macros"
	@echo "make ready                 -- prepares the source directory for make"
	@echo "make all                   -- makes $(SLACK_TARGET) (default)"
	@echo "make test                  -- generates and performs library unit tests"
	@echo "make man                   -- generates all manpages"
	@echo "make html                  -- generates all manpages in html"
	@echo "make install               -- installs everything under $(PREFIX)"
	@echo "make uninstall             -- uninstalls everything"
	@echo "make depend                -- generates source dependencies using makedepend"
	@echo "make tags                  -- generates a tags file using ctags"
	@echo "make clean                 -- removes object files, tags, core and Makefile.bak"
	@echo "make clobber               -- same as clean but also removes $(SLACK_TARGET) and tests"
	@echo "make distclean             -- same as clobber but also removes source dependencies"
	@echo "make MANIFEST              -- creates the MANIFEST file"
	@echo "make dist                  -- creates the distribution: ../$(SLACK_DIST)"
	@echo

help-macros::
	@echo "CC = $(CC)"
	@echo "TEST = $(TEST)"
	@echo "PREFIX = $(PREFIX)"
	@echo "APP_INSDIR = $(APP_INSDIR)"
	@echo "LIB_INSDIR = $(LIB_INSDIR)"
	@echo "MAN_INSDIR = $(MAN_INSDIR)"
	@echo "HDR_INSDIR = $(HDR_INSDIR)"
	@echo "APP_MANSECT = $(APP_MANSECT)"
	@echo "LIB_MANSECT = $(LIB_MANSECT)"
	@echo "APP_MANDIR = $(APP_MANDIR)"
	@echo "APP_CATDIR = $(APP_CATDIR)"
	@echo "LIB_MANDIR = $(LIB_MANDIR)"
	@echo "LIB_CATDIR = $(LIB_CATDIR)"
	@echo "TAG_FILES = $(TAG_FILES)"
	@echo "DEPEND_CFILES = $(DEPEND_CFILES)"
	@echo "DEPEND_HFILES = $(DEPEND_HFILES)"
	@echo "CCFLAGS = $(CCFLAGS)"
	@echo "READY_TARGETS = $(READY_TARGETS)"
	@echo "ALL_TARGETS = $(ALL_TARGETS)"
	@echo "TEST_TARGETS = $(TEST_TARGETS)"
	@echo "MAN_TARGETS = $(MAN_TARGETS)"
	@echo "HTML_TARGETS = $(HTML_TARGETS)"
	@echo "INSTALL_TARGETS = $(INSTALL_TARGETS)"
	@echo "UNINSTALL_TARGETS = $(UNINSTALL_TARGETS)"
	@echo "CLEAN_FILES = $(CLEAN_FILES)"
	@echo "CLOBBER_FILES = $(CLOBBER_FILES)"
	@echo

MANIFEST:
	@find . -name \* -print > MANIFEST

tags: $(TAG_FILES)
	@ctags $(TAG_FILES)

depend: ready $(DEPEND_CFILES) $(DEPEND_HFILES)
	@makedepend $(SLACK_CPPFLAGS) $(DEPEND_CFILES)

clean::
	@rm -rf $(CLEAN_FILES)

clobber::
	@rm -rf $(CLEAN_FILES) $(CLOBBER_FILES)

distclean:: clobber
	@perl -pi -e 'last if /[D]O NOT DELETE/;' Makefile

dist: distclean MANIFEST
	@src=`basename \`pwd\``; \
	dst=$(SLACK_ID); \
	cd ..; \
	$(TEST) "$$src" != "$$dst" -a ! -e "$$dst" && ln -s $$src $$dst; \
	tar chzf $(SLACK_DIST) $$dst; \
	$(TEST) -L "$$dst" && rm -f $$dst; \
	rm $$src/MANIFEST; \
	tar tzf $$dst.tar.gz

include $(SLACK_SRCDIR)/rules.mk

