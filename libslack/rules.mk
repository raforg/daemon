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

ifneq ($(SLACK_TARGET),./$(SLACK_NAME))

.PHONY: $(SLACK_NAME)

$(SLACK_NAME): $(SLACK_TARGET)

endif

$(SLACK_TARGET): $(SLACK_OFILES)
	$(AR) cr $(SLACK_TARGET) $(SLACK_OFILES)

$(SLACK_SRCDIR)/%.o: $(SLACK_SRCDIR)/%.c
	$(CC) $(SLACK_CFLAGS) -o $@ -c $<

.PHONY: ready-slack

ready-slack:
	@$(TEST) -e $(SLACK_INCLINK) || ln -s . $(SLACK_INCLINK)

.PHONY: man-slack html-slack

man-slack: $(SLACK_MANFILES)

$(SLACK_SRCDIR)/%.$(LIB_MANSECT): $(SLACK_SRCDIR)/%.c
	pod2man --center='$(LIB_MANSECTNAME)' --section=$(LIB_MANSECT) $< > $@

html-slack: $(SLACK_HTMLFILES)

$(SLACK_SRCDIR)/%.$(LIB_MANSECT).html: $(SLACK_SRCDIR)/%.c
	pod2html --noindex < $< > $@ 2>/dev/null

.PHONY: install-slack install-slack-bin install-slack-h install-slack-man

install-slack: install-slack-bin install-slack-h install-slack-man

install-slack-bin:
	install -m 444 $(SLACK_TARGET) $(LIB_INSDIR)/$(SLACK_INSTALL)
	rm -f $(LIB_INSDIR)/$(SLACK_INSTALL_LINK)
	ln -s $(SLACK_INSTALL) $(LIB_INSDIR)/$(SLACK_INSTALL_LINK)

install-slack-h:
	$(TEST) ! -d $(HDR_INSDIR)/$(SLACK_NAME) && mkdir $(HDR_INSDIR)/$(SLACK_NAME) || exit 0
	install -m 444 $(SLACK_HFILES) $(HDR_INSDIR)/$(SLACK_NAME)

install-slack-man: man-slack
	install -m 444 $(SLACK_MANFILES) $(LIB_MANDIR)

.PHONY: uninstall-slack uninstall-slack-bin uninstall-slack-h uninstall-slack-man

uninstall-slack: uninstall-slack-bin uninstall-slack-h uninstall-slack-man

uninstall-slack-bin:
	cd $(LIB_INSDIR) && rm -f $(SLACK_INSTALL) $(SLACK_INSTALL_LINK) || exit 0

uninstall-slack-h:
	cd $(HDR_INSDIR) && rm -f $(SLACK_HFILES) && rmdir $(HDR_INSDIR)/$(SLACK_NAME) || exit 0

uninstall-slack-man:
	cd $(LIB_MANDIR) && rm -f $(SLACK_MANFILES) || exit 0

.PHONY: test-slack

test-slack: $(SLACK_TESTS)
	@cd $(SLACK_TESTDIR); for test in $(patsubst $(SLACK_TESTDIR)/%, %, $(SLACK_TESTS)); do echo; ./$$test; done

$(SLACK_TESTS): $(SLACK_TARGET)

$(SLACK_TESTDIR)/%.test: $(SLACK_SRCDIR)/%.c
	@$(TEST) -d $(SLACK_TESTDIR) || mkdir $(SLACK_TESTDIR)
	$(CC) -DTEST $(SLACK_CFLAGS) -o $@ $< $(SLACK_LDFLAGS)

# Present make targets separately in help if we are not alone
ifeq ($(SLACK_IS_ROOT),)
SLACK_HELP := 1
else
ifeq ($(SLACK_HAS_SUBTARGETS), 1)
SLACK_HELP := 1
endif
endif

ifeq ($(SLACK_HELP), 1)
help::
	@echo "make $(SLACK_NAME)                  -- makes $(SLACK_TARGET)"
	@echo "make ready-$(SLACK_NAME)            -- prepares the source directory for make"
	@echo "make man-$(SLACK_NAME)              -- makes the $(SLACK_NAME) manpages"
	@echo "make html-$(SLACK_NAME)             -- makes the $(SLACK_NAME) manpages in html"
	@echo "make install-slack          -- installs $(SLACK_NAME), headers and manpages"
	@echo "make install-slack-bin      -- installs $(SLACK_NAME) in $(LIB_INSDIR)"
	@echo "make install-slack-h        -- installs $(SLACK_NAME) headers in $(HDR_INSDIR)/$(SLACK_NAME)"
	@echo "make install-slack-man      -- installs $(SLACK_NAME) manpages in $(LIB_MANDIR)"
	@echo "make uninstall-slack        -- uninstalls $(SLACK_NAME), its headers and manpages"
	@echo "make uninstall-slack-bin    -- uninstalls $(SLACK_NAME) from $(LIB_INSDIR)"
	@echo "make uninstall-slack-h      -- uninstalls $(SLACK_NAME) headers from $(HDR_INSDIR)/$(SLACK_NAME)"
	@echo "make uninstall-slack-man    -- uninstalls $(SLACK_NAME) manpages from $(LIB_MANDIR)"
	@echo "make test-slack             -- generates and performs library unit tests"
	@echo
endif

help-macros::
	@echo "SLACK_NAME = $(SLACK_NAME)"
	@echo "SLACK_VERSION = $(SLACK_VERSION)"
	@echo "SLACK_ID = $(SLACK_ID)"
	@echo "SLACK_DIST = $(SLACK_DIST)"
	@echo "SLACK_TARGET = $(SLACK_TARGET)"
	@echo "SLACK_INSTALL = $(SLACK_INSTALL)"
	@echo "SLACK_MODULES = $(SLACK_MODULES)"
	@echo "SLACK_HEADERS = $(SLACK_HEADERS)"
	@echo "SLACK_SRCDIR = $(SLACK_SRCDIR)"
	@echo "SLACK_INCDIRS = $(SLACK_INCDIRS)"
	@echo "SLACK_LIBDIRS = $(SLACK_LIBDIRS)"
	@echo "SLACK_LIBS = $(SLACK_LIBS)"
	@echo "SLACK_TESTDIR = $(SLACK_TESTDIR)"
	@echo "SLACK_CFILES = $(SLACK_CFILES)"
	@echo "SLACK_OFILES = $(SLACK_OFILES)"
	@echo "SLACK_HFILES = $(SLACK_HFILES)"
	@echo "SLACK_PODFILES = $(SLACK_PODFILES)"
	@echo "SLACK_MANFILES = $(SLACK_MANFILES)"
	@echo "SLACK_HTMLFILES = $(SLACK_HTMLFILES)"
	@echo "SLACK_TESTS = $(SLACK_TESTS)"
	@echo "SLACK_DEFINES = $(SLACK_DEFINES)"
	@echo "SLACK_CPPFLAGS = $(SLACK_CPPFLAGS)"
	@echo "SLACK_CCFLAGS = $(SLACK_CCFLAGS)"
	@echo "SLACK_CFLAGS = $(SLACK_CFLAGS)"
	@echo "SLACK_LDFLAGS = $(SLACK_LDFLAGS)"
	@echo

distclean::
	@rm -f $(SLACK_INCLINK)
