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

ifneq ($(DAEMON_TARGET),./$(DAEMON_NAME))

.PHONY: $(DAEMON_NAME)

$(DAEMON_NAME): $(DAEMON_TARGET)

endif

$(DAEMON_TARGET): $(DAEMON_OFILES) $(DAEMON_SUBMODULES)
	$(CC) $(DAEMON_CFLAGS) -o $(DAEMON_TARGET) $(DAEMON_OFILES) $(DAEMON_LDFLAGS)

$(DAEMON_SRCDIR)/%.o: $(DAEMON_SRCDIR)/%.c
	$(CC) $(DAEMON_CFLAGS) -o $@ -c $<

.PHONY: man-daemon html-daemon

man-daemon: $(DAEMON_MANFILES)

$(DAEMON_SRCDIR)/%.$(APP_MANSECT): $(DAEMON_SRCDIR)/%.c
	pod2man --center='$(APP_MANSECTNAME)' --section=$(APP_MANSECT) $< > $@

html-daemon: $(DAEMON_HTMLFILES)

$(DAEMON_SRCDIR)/%.$(APP_MANSECT).html: $(DAEMON_SRCDIR)/%.c
	pod2html --noindex < $< > $@ 2>/dev/null

.PHONY: install-daemon install-daemon-bin install-daemon-man

install-daemon: install-daemon-bin install-daemon-man

install-daemon-bin:
	install -m 555 $(DAEMON_TARGET) $(APP_INSDIR)

install-daemon-man: man-daemon
	install -m 444 $(DAEMON_MANFILES) $(APP_MANDIR)

.PHONY: uninstall-daemon uninstall-daemon-bin uninstall-daemon-man

uninstall-daemon: uninstall-daemon-bin uninstall-daemon-man

uninstall-daemon-bin:
	cd $(APP_INSDIR) && rm -f $(DAEMON_TARGET) || exit 0

uninstall-daemon-man:
	cd $(APP_MANDIR) && rm -f $(DAEMON_MANFILES) && cd $(APP_CATDIR) && rm -f $(DAEMON_MANFILES) || exit 0

help::
	@echo "make $(DAEMON_NAME)                -- makes $(DAEMON_TARGET) and $(DAEMON_SUBMODULES)"
	@echo "make man-$(DAEMON_NAME)            -- makes the $(DAEMON_NAME) manpages"
	@echo "make html-$(DAEMON_NAME)           -- makes the $(DAEMON_NAME) manpages in html"
	@echo "make install-daemon        -- installs $(DAEMON_NAME) and its manpage"
	@echo "make install-daemon-bin    -- installs $(DAEMON_NAME) in $(APP_INSDIR)"
	@echo "make install-daemon-man    -- installs the $(DAEMON_NAME) manpage in $(APP_MANDIR)"
	@echo "make uninstall-daemon      -- uninstalls $(DAEMON_NAME) and its manpage"
	@echo "make uninstall-daemon-bin  -- uninstalls $(DAEMON_NAME) from $(APP_INSDIR)"
	@echo "make uninstall-daemon-man  -- uninstalls the $(DAEMON_NAME) manpage from $(APP_MANDIR)"
	@echo

help-macros::
	@echo "DAEMON_NAME = $(DAEMON_NAME)"
	@echo "DAEMON_VERSION = $(DAEMON_VERSION)"
	@echo "DAEMON_ID = $(DAEMON_ID)"
	@echo "DAEMON_DIST = $(DAEMON_DIST)"
	@echo "DAEMON_TARGET = $(DAEMON_TARGET)"
	@echo "DAEMON_MODULES = $(DAEMON_MODULES)"
	@echo "DAEMON_SUBMODULES = $(DAEMON_SUBMODULES)"
	@echo "DAEMON_SRCDIR = $(DAEMON_SRCDIR)"
	@echo "DAEMON_INCDIRS = $(DAEMON_INCDIRS)"
	@echo "DAEMON_LIBDIRS = $(DAEMON_LIBDIRS)"
	@echo "DAEMON_LIBS = $(DAEMON_LIBS)"
	@echo "DAEMON_CFILES = $(DAEMON_CFILES)"
	@echo "DAEMON_OFILES = $(DAEMON_OFILES)"
	@echo "DAEMON_HFILES = $(DAEMON_HFILES)"
	@echo "DAEMON_PODFILES = $(DAEMON_PODFILES)"
	@echo "DAEMON_MANFILES = $(DAEMON_MANFILES)"
	@echo "DAEMON_HTMLFILES = $(DAEMON_HTMLFILES)"
	@echo "DAEMON_DEFINES = $(DAEMON_DEFINES)"
	@echo "DAEMON_CPPFLAGS = $(DAEMON_CPPFLAGS)"
	@echo "DAEMON_CCFLAGS = $(DAEMON_CCFLAGS)"
	@echo "DAEMON_CFLAGS = $(DAEMON_CFLAGS)"
	@echo "DAEMON_LDFLAGS = $(DAEMON_LDFLAGS)"
	@echo

include $(PROG_SRCDIR)/rules.mk

