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

ifneq ($(PROG_TARGET),./$(PROG_NAME))

.PHONY: $(PROG_NAME)

$(PROG_NAME): $(PROG_TARGET)

endif

$(PROG_TARGET): $(PROG_OFILES)
	$(AR) cr $(PROG_TARGET) $(PROG_OFILES)

$(PROG_SRCDIR)/%.o: $(PROG_SRCDIR)/%.c
	$(CC) $(PROG_CFLAGS) -o $@ -c $<

.PHONY: ready-prog

ready-prog:
	@$(TEST) -e $(PROG_INCLINK) || ln -s . $(PROG_INCLINK)

.PHONY: man-prog html-prog

man-prog: $(PROG_MANFILES)

$(PROG_SRCDIR)/%.$(LIB_MANSECT): $(PROG_SRCDIR)/%.c
	pod2man --center='$(LIB_MANSECTNAME)' --section=$(LIB_MANSECT) $< > $@

html-prog: $(PROG_HTMLFILES)

$(PROG_SRCDIR)/%.$(LIB_MANSECT).html: $(PROG_SRCDIR)/%.c
	pod2html --noindex < $< > $@ 2>/dev/null

.PHONY: install-prog install-prog-bin install-prog-h install-prog-man

install-prog: install-prog-bin install-prog-h install-prog-man

install-prog-bin:
	install -m 444 $(PROG_TARGET) $(LIB_INSDIR)/$(PROG_INSTALL)
	rm -f $(LIB_INSDIR)/$(PROG_INSTALL_LINK)
	ln -s $(PROG_INSTALL) $(LIB_INSDIR)/$(PROG_INSTALL_LINK)

install-prog-h:
	$(TEST) ! -d $(HDR_INSDIR)/$(PROG_NAME) && mkdir $(HDR_INSDIR)/$(PROG_NAME) || exit 0
	install -m 444 $(PROG_HFILES) $(HDR_INSDIR)/$(PROG_NAME)

install-prog-man: man-prog
	install -m 444 $(PROG_MANFILES) $(LIB_MANDIR)

.PHONY: uninstall-prog uninstall-prog-bin uninstall-prog-h uninstall-prog-man

uninstall-prog: uninstall-prog-bin uninstall-prog-h uninstall-prog-man

uninstall-prog-bin:
	cd $(LIB_INSDIR) && rm -f $(PROG_INSTALL) $(PROG_INSTALL_LINK) || exit 0

uninstall-prog-h:
	cd $(HDR_INSDIR) && rm -f $(PROG_HFILES) && rmdir $(HDR_INSDIR)/$(PROG_NAME) || exit 0

uninstall-prog-man:
	cd $(LIB_MANDIR) && rm -f $(PROG_MANFILES) || exit 0

.PHONY: test-prog

test-prog: $(PROG_TESTS)
	@cd $(PROG_TESTDIR); for test in $(patsubst $(PROG_TESTDIR)/%, %, $(PROG_TESTS)); do echo; ./$$test; done

$(PROG_TESTS): $(PROG_TARGET)

$(PROG_TESTDIR)/%.test: $(PROG_SRCDIR)/%.c
	@$(TEST) -d $(PROG_TESTDIR) || mkdir $(PROG_TESTDIR)
	$(CC) -DTEST $(PROG_CFLAGS) -o $@ $< $(PROG_LDFLAGS)

help::
	@echo "make $(PROG_NAME)                  -- makes $(PROG_TARGET)"
	@echo "make ready-$(PROG_NAME)            -- prepares the source directory for make"
	@echo "make man-$(PROG_NAME)              -- makes the $(PROG_NAME) manpages"
	@echo "make html-$(PROG_NAME)             -- makes the $(PROG_NAME) manpages in html"
	@echo "make install-prog          -- installs $(PROG_NAME), headers and manpages"
	@echo "make install-prog-bin      -- installs $(PROG_NAME) in $(LIB_INSDIR)"
	@echo "make install-prog-h        -- installs $(PROG_NAME) headers in $(HDR_INSDIR)/$(PROG_NAME)"
	@echo "make install-prog-man      -- installs $(PROG_NAME) manpages in $(LIB_MANDIR)"
	@echo "make uninstall-prog        -- uninstalls $(PROG_NAME), its headers and manpages"
	@echo "make uninstall-prog-bin    -- uninstalls $(PROG_NAME) from $(LIB_INSDIR)"
	@echo "make uninstall-prog-h      -- uninstalls $(PROG_NAME) headers from $(HDR_INSDIR)/$(PROG_NAME)"
	@echo "make uninstall-prog-man    -- uninstalls $(PROG_NAME) manpages from $(LIB_MANDIR)"
	@echo "make test-prog             -- generates and performs library unit tests"
	@echo

help-macros::
	@echo "PROG_NAME = $(PROG_NAME)"
	@echo "PROG_VERSION = $(PROG_VERSION)"
	@echo "PROG_ID = $(PROG_ID)"
	@echo "PROG_DIST = $(PROG_DIST)"
	@echo "PROG_TARGET = $(PROG_TARGET)"
	@echo "PROG_INSTALL = $(PROG_INSTALL)"
	@echo "PROG_MODULES = $(PROG_MODULES)"
	@echo "PROG_HEADERS = $(PROG_HEADERS)"
	@echo "PROG_SRCDIR = $(PROG_SRCDIR)"
	@echo "PROG_INCDIRS = $(PROG_INCDIRS)"
	@echo "PROG_LIBDIRS = $(PROG_LIBDIRS)"
	@echo "PROG_LIBS = $(PROG_LIBS)"
	@echo "PROG_TESTDIR = $(PROG_TESTDIR)"
	@echo "PROG_CFILES = $(PROG_CFILES)"
	@echo "PROG_OFILES = $(PROG_OFILES)"
	@echo "PROG_HFILES = $(PROG_HFILES)"
	@echo "PROG_PODFILES = $(PROG_PODFILES)"
	@echo "PROG_MANFILES = $(PROG_MANFILES)"
	@echo "PROG_HTMLFILES = $(PROG_HTMLFILES)"
	@echo "PROG_TESTS = $(PROG_TESTS)"
	@echo "PROG_DEFINES = $(PROG_DEFINES)"
	@echo "PROG_CPPFLAGS = $(PROG_CPPFLAGS)"
	@echo "PROG_CCFLAGS = $(PROG_CCFLAGS)"
	@echo "PROG_CFLAGS = $(PROG_CFLAGS)"
	@echo "PROG_LDFLAGS = $(PROG_LDFLAGS)"
	@echo

distclean::
	@rm -f $(PROG_INCLINK)
