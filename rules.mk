
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

# 20020916 raf <raf@raf.org>

ifneq ($(DAEMON_TARGET),./$(DAEMON_NAME))

.PHONY: $(DAEMON_NAME)

$(DAEMON_NAME): $(DAEMON_TARGET)

endif

$(DAEMON_TARGET): $(DAEMON_OFILES) $(DAEMON_SUBTARGETS)
	$(CC) -o $(DAEMON_TARGET) $(DAEMON_OFILES) $(DAEMON_LDFLAGS)

.PHONY: man-daemon html-daemon

man-daemon: $(DAEMON_MANFILES)

html-daemon: $(DAEMON_HTMLFILES)

.PHONY: install-daemon install-daemon-bin install-daemon-man install-daemon-html install-daemon-conf

install-daemon: install-daemon-bin install-daemon-man

install-daemon-bin:
	mkdir -p $(APP_INSDIR)
	install -m 755 $(DAEMON_TARGET) $(APP_INSDIR)
	strip $(patsubst %, $(APP_INSDIR)/%, $(notdir $(DAEMON_TARGET)))

install-daemon-man: man-daemon
	@mkdir -p $(APP_MANDIR); \
	install -m 644 $(DAEMON_MANFILES) $(APP_MANDIR); \
	mkdir -p $(FMT_MANDIR); \
	rm -f $(FMT_MANDIR)/$(notdir $(DAEMON_MANLINK)); \
	ln -s ../man$(APP_MANSECT)/$(notdir $(DAEMON_MANFILES)) $(FMT_MANDIR)/$(notdir $(DAEMON_MANLINK))

install-daemon-html: html-daemon
	@mkdir -p $(DAEMON_HTMLDIR); \
	install -m 644 $(DAEMON_HTMLFILES) $(DAEMON_HTMLDIR)

install-daemon-conf: $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE)
	@mkdir -p $(DAEMON_CONFDIR); \
	install -m 644 $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE) $(DAEMON_CONFDIR)

.PHONY: uninstall-daemon uninstall-daemon-bin uninstall-daemon-man uninstall-daemon-html uninstall-daemon-conf

uninstall-daemon: uninstall-daemon-bin uninstall-daemon-man

uninstall-daemon-bin:
	rm -f $(patsubst %, $(APP_INSDIR)/%, $(notdir $(DAEMON_TARGET)))

uninstall-daemon-man:
	@rm -f $(patsubst %, $(APP_MANDIR)/%, $(notdir $(DAEMON_MANFILES))) \
	$(FMT_MANDIR)/$(notdir $(DAEMON_MANLINK))

uninstall-daemon-html:
	@rm -f $(patsubst %, $(DAEMON_HTMLDIR)/%, $(notdir $(DAEMON_HTMLFILES)))

uninstall-daemon-conf:
	@rm -f $(DAEMON_CONFDIR)/$(DAEMON_CONFFILE)

.PHONY: dist-daemon dist-html-daemon rpm-daemon deb-daemon sol-daemon obsd-daemon fbsd-daemon

dist-daemon: distclean
	@set -e; \
	up="`pwd`/.."; \
	cd $(DAEMON_SRCDIR); \
	src=`basename \`pwd\``; \
	dst=$(DAEMON_ID); \
	cd ..; \
	[ "$$src" != "$$dst" -a ! -d "$$dst" ] && ln -s $$src $$dst; \
	tar chzf $$up/$(DAEMON_DIST) $$dst; \
	[ -h "$$dst" ] && rm -f $$dst; \
	tar tzfv $$up/$(DAEMON_DIST); \
	ls -l $$up/$(DAEMON_DIST)

dist-html-daemon: html-daemon
	@set -e; \
	up="`pwd`/.."; \
	cd $(DAEMON_SRCDIR); \
	src=`basename \`pwd\``; \
	dst=$(DAEMON_ID); \
	cd ..; \
	[ "$$src" != "$$dst" -a ! -d "$$dst" ] && ln -s $$src $$dst; \
	tar chzf $$up/$(DAEMON_HTML_DIST) $(patsubst $(DAEMON_SRCDIR)/%, $$dst/%, $(DAEMON_SRCDIR)/README $(DAEMON_HTMLFILES)); \
	[ -h "$$dst" ] && rm -f $$dst; \
	tar tzfv $$up/$(DAEMON_HTML_DIST); \
	ls -l $$up/$(DAEMON_HTML_DIST)

REDHAT := /usr/src/redhat
#RPMBUILD := rpm     # rpm 3.x
RPMBUILD := rpmbuild # rpm 4.x

rpm-daemon: $(DAEMON_SRCDIR)/daemon.spec
	@set -e; \
	up="`pwd`/.."; \
	cp $$up/$(DAEMON_DIST) $(REDHAT)/SOURCES; \
	$(RPMBUILD) --buildroot "/tmp/$(DAEMON_ID)" -ba $(DAEMON_SRCDIR)/daemon.spec; \
	rm -rf $(DAEMON_SRCDIR)/daemon.spec "/tmp/$(DAEMON_ID)"; \
	mv $(REDHAT)/SRPMS/$(DAEMON_ID)-*.src.rpm $$up; \
	mv $(REDHAT)/RPMS/*/$(DAEMON_ID)-*.*.rpm $$up; \
	rm -rf $(REDHAT)/BUILD/$(DAEMON_ID); \
	rm -f $(REDHAT)/SOURCES/$(DAEMON_DIST); \
	rm -f $(REDHAT)/SPECS/daemon.spec; \
	rpm -qlpv $$up/$(DAEMON_ID)-*.*.rpm

$(DAEMON_SRCDIR)/daemon.spec:
	@set -e; \
	perl -ne ' \
		next if /^~+$$/; \
		chop($$section = $$_), next if /^[A-Z]+$$/; \
		$$summary = $$_ if $$section eq "README" && /^\w/; \
		$$description .= $$_ if $$section eq "DESCRIPTION"; \
		if ($$section ne "README" && $$section ne "DESCRIPTION") \
		{ \
			print "Summary: $$summary"; \
			print "Name: $(DAEMON_NAME)\n"; \
			print "Version: $(DAEMON_VERSION)\n"; \
			print "Release: 1\n"; \
			print "Group: System Environment/Daemons\n"; \
			print "Source: $(DAEMON_URL)download/$(DAEMON_DIST)\n"; \
			print "URL: $(DAEMON_URL)\n"; \
			print "Copyright: GPL\n"; \
			print "Prefix: $(PREFIX)\n"; \
			print "%description\n"; \
			print $$description; \
			print "%prep\n"; \
			print "%setup\n"; \
			print "%build\n"; \
			print "make\n"; \
			print "%install\n"; \
			print "make PREFIX=\"\$${RPM_BUILD_ROOT}$(PREFIX)\" install-daemon\n"; \
			print "%files\n"; \
			exit; \
		} \
	' < $(DAEMON_SRCDIR)/README > $(DAEMON_SRCDIR)/daemon.spec; \
	for file in $(DAEMON_RPM_FILES); do echo $$file >> $(DAEMON_SRCDIR)/daemon.spec; done; \
	for file in $(sort $(DAEMON_RPM_DOCFILES)); do echo %doc $$file >> $(DAEMON_SRCDIR)/daemon.spec; done

# Extra debuild options: Don't sign anything (I'm not a debian maintainer)
DEBUILD_OPTIONS := -us -uc

# Extra debuild options should be:
# DEBUILD_OPTIONS := -mmaintainer@debian.org

deb-daemon: $(DAEMON_SRCDIR)/debian
	@set -e; \
	CDPATH=""; \
	DAEMON_RENAMED=""; \
	case "`pwd`" in \
		*/$(DAEMON_NAME)) \
			cd ..; \
			[ -d $(DAEMON_NAME)-$(DAEMON_VERSION) ] && exit 1; \
			mv $(DAEMON_NAME) $(DAEMON_NAME)-$(DAEMON_VERSION); \
			cd $(DAEMON_NAME)-$(DAEMON_VERSION); \
			DAEMON_RENAMED="yes";; \
	esac; \
	echo "Building debian packages."; \
	debuild -rfakeroot -tc $(DEBUILD_OPTIONS); \
	rm -rf $(CLEAN_FILES) $(CLOBBER_FILES); \
	[ "$$DAEMON_RENAMED" = "yes" ] && cd .. && mv $(DAEMON_NAME)-$(DAEMON_VERSION) $(DAEMON_NAME) && cd $(DAEMON_NAME); \
	echo "Listing package info and contents."; \
	dpkg --info ../$(DAEMON_NAME)_$(DAEMON_VERSION)-*_*.deb; \
	dpkg --contents ../$(DAEMON_NAME)_$(DAEMON_VERSION)-*_*.deb

$(DAEMON_SRCDIR)/debian:
	@set -e; \
	CDPATH=""; \
	DAEMON_RENAMED=""; \
	case "`pwd`" in \
		*/$(DAEMON_NAME)) \
			cd ..; \
			[ -d $(DAEMON_NAME)-$(DAEMON_VERSION) ] && exit 1; \
			mv $(DAEMON_NAME) $(DAEMON_NAME)-$(DAEMON_VERSION); \
			cd $(DAEMON_NAME)-$(DAEMON_VERSION); \
			DAEMON_RENAMED="yes"; \
			;; \
	esac; \
	echo "Creating debian directory."; \
	echo | dh_make -s -e raf@raf.org; \
	[ "$$DAEMON_RENAMED" = "yes" ] && cd .. && mv $(DAEMON_NAME)-$(DAEMON_VERSION) $(DAEMON_NAME) && cd $(DAEMON_NAME); \
	cd $(DAEMON_SRCDIR)/debian; \
	echo "Cleaning up debian directory."; \
	rm -f README.Debian dirs *.ex ex.*; \
	echo "Creating debian/conffiles."; \
	echo /etc/daemon.conf > conffiles; \
	echo "Completing debian/control."; \
	perl -p -i -e ' \
		BEGIN { \
			open(README, "../README") or die("failed to open ../README\n"); \
			while (<README>) \
			{ \
				next if /^~+$$/; \
				chop($$section = $$_), next if /^[A-Z]+$$/; \
				$$summary = $$_ if $$section eq "README" && /^\w/; \
				$$description .= $$_ if $$section eq "DESCRIPTION"; \
				if ($$section ne "README" && $$section ne "DESCRIPTION") \
				{ \
					chop($$description); \
					chop($$description); \
					chop($$summary); \
					$$summary = $$1 if $$summary =~ /^\w+ - (.*)$$/; \
					$$description =~ s/^$$/./mg; \
					$$description =~ s/^/ /mg; \
					last; \
				} \
			} \
			close(README); \
		} \
		s/^Section: unknown$$/Section: utils/; \
		s/^Description: <insert up to 60 chars description>$$/Description: $$summary/; \
		s/^ <insert long description, indented with spaces>$$/$$description/; \
		s/^Priority: optional$$/Priority: optional\nBuild-Depends: debhelper, perl/; \
	' control; \
	echo "Completing debian/copyright."; \
	perl -p -i -e ' \
		s/<fill in ftp site>/http:\/\/libslack.org\/daemon\//; \
		s/Upstream Author\(s\):/Upstream Author:/; \
		s/<put author\(s\) name and email here>/raf <raf\@raf.org>/; \
		s/^Copyright:$$/Copyright (C) 1999-2002 raf <raf\@raf.org>/; \
		s/<Must follow here>/This software is released under the terms of the GNU General Public License:\n\n    http:\/\/www.gnu.org\/copyleft\/gpl.html (on the Web)\n    file:\/usr\/share\/common-licenses\/GPL  (on Debian systems)\n/; \
	' copyright; \
	echo "Cleaning up debian/changelog."; \
	perl -p -i -e 'last if /^Local variables:/' changelog; \
	echo "Creating debian/doc-base."; \
	echo "Document: $(DAEMON_NAME)" > doc-base; \
	echo "Title: $(DAEMON_NAME) manual" >> doc-base; \
	echo "Author: raf <raf@raf.org>" >> doc-base; \
	echo "Abstract: $(DAEMON_NAME)(1) - turns other processes into daemons" >> doc-base; \
	echo "Section: Apps/Tools" >> doc-base; \
	echo "" >> doc-base; \
	echo "Format: HTML" >> doc-base; \
	echo "Index: /usr/share/doc/$(DAEMON_NAME)/html/" >> doc-base; \
	echo "Files: /usr/share/doc/$(DAEMON_NAME)/html/*.html" >> doc-base; \
	echo "" >> doc-base; \
	echo "Completing debian/rules."; \
	perl -p -i -e ' \
		s/^\t-\$$\(MAKE\) clean$$/\t\-\$$(MAKE) debian-clobber/; \
		s/^\t\$$\(MAKE\)$$/\t\$$(MAKE) all daemon.conf man-daemon html-daemon/; \
		s/^\t\$$\(MAKE\) install .*$$/\t\$$(MAKE) PREFIX=debian\/$(DAEMON_NAME)\/usr install-daemon-bin\n\t\$$(MAKE) PREFIX=debian\/$(DAEMON_NAME)\/usr\/share install-daemon-man\n\t\$$(MAKE) DAEMON_HTMLDIR=debian\/$(DAEMON_NAME)\/usr\/share\/doc\/daemon\/html install-daemon-html\n\t\$$(MAKE) DAEMON_CONFDIR=debian\/$(DAEMON_NAME)\/etc install-daemon-conf/; \
		s/^\tdh_installexamples$$/#\tdh_installexamples/; \
		s/^\tdh_installmenu$$/#\tdh_installmenu/; \
		s/^\tdh_installcron$$/#\tdh_installcron/; \
		s/^\tdh_installman$$/#\tdh_installman/; \
		s/^\tdh_installinfo$$/#\tdh_installinfo/; \
		s/^\tdh_suidregister$$/#\tdh_suidregister/; \
		s/[ \t]+$$//; \
	' rules; \

.PHONY: debian-clobber

debian-clobber::
	@rm -rf $(CLEAN_FILES) $(DEBIAN_CLOBBER_FILES)

$(DAEMON_SRCDIR)/$(DAEMON_CONFFILE):
	@echo '# /etc/daemon.conf: system-wide daemon(1) configuration.' > $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE); \
	echo '# See daemon(1) for full documentation.' >> $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE); \
	echo '' >> $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE); \
	echo '# Format: <name|"*"> <option(","option)*>' >> $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE); \
	echo '' >> $(DAEMON_SRCDIR)/$(DAEMON_CONFFILE)

sol-daemon: $(DAEMON_SRCDIR)/daemon.pkginfo
	@set -e; \
	base="`pwd`"; \
	up="$$base/.."; \
	mkdir -p $(DAEMON_SRCDIR)/solaris/install; \
	mkdir -p $(DAEMON_SRCDIR)/solaris/build; \
	mkdir -p $(DAEMON_SRCDIR)/solaris/info; \
	cd $(DAEMON_SRCDIR)/solaris/build; \
	tar xzf $$up/$(DAEMON_DIST); \
	cd $(DAEMON_ID); \
	conf/solaris8-cc; \
	make PREFIX=../../install FINAL_PREFIX="$(PREFIX)" all install-daemon; \
	cd "$$base"; \
	mv $(DAEMON_SRCDIR)/daemon.pkginfo $(DAEMON_SRCDIR)/solaris/info/pkginfo; \
	cd $(DAEMON_SRCDIR)/solaris/install; \
	pkgproto . > ../info/prototype; \
	echo "i pkginfo" >> ../info/prototype; \
	cd ../info; \
	pkgmk -o -b ../install -r ../install $(DAEMON_SOL); \
	cd "$$base"; \
	rm -rf $(DAEMON_SRCDIR)/solaris; \
	arch="`uname -m`"; \
	pkgtrans /var/spool/pkg $(DAEMON_ID).$$arch.pkg $(DAEMON_SOL); \
	rm -rf /var/spool/pkg/$(DAEMON_SOL); \
	mv /var/spool/pkg/$(DAEMON_ID).$$arch.pkg $$up; \
	gzip $$up/$(DAEMON_ID).$$arch.pkg

$(DAEMON_SRCDIR)/daemon.pkginfo:
	@set -e; \
	perl -ne ' \
		next if /^~+$$/; \
		chop($$section = $$_), next if /^[A-Z]+$$/; \
		chop($$description = $$_) if $$section eq "README" && /^\w/; \
		if ($$section ne "README") \
		{ \
			print "PKG=\"$(DAEMON_SOL)\"\n"; \
			print "NAME=\"$$description\"\n"; \
			print "VERSION=\"$(DAEMON_VERSION)\"\n"; \
			print "CATEGORY=\"application\"\n"; \
			print "BASEDIR=\"$(FINAL_PREFIX)\"\n"; \
			exit; \
		} \
	' < $(DAEMON_SRCDIR)/README > $(DAEMON_SRCDIR)/daemon.pkginfo

obsd-daemon: $(DAEMON_SRCDIR)/obsd-daemon-oneline $(DAEMON_SRCDIR)/obsd-daemon-description
	@set -e; \
	base="`pwd`"; \
	up="$$base/.."; \
	mkdir -p "$$base/obsd-$(DAEMON_NAME)/build"; \
	mkdir -p "$$base/obsd-$(DAEMON_NAME)/install"; \
	cd "$$base/obsd-$(DAEMON_NAME)/build"; \
	tar xzf "$$up/$(DAEMON_DIST)"; \
	cd ./$(DAEMON_ID); \
	conf/openbsd; \
	make PREFIX=../../install FINAL_PREFIX="$(PREFIX)" all install-daemon; \
	cd "$$base"; \
	echo "@name $(DAEMON_ID)" > $(DAEMON_SRCDIR)/obsd-daemon-packinglist; \
	echo "@cwd $(PREFIX)" >> $(DAEMON_SRCDIR)/obsd-daemon-packinglist; \
	echo "@src $$base/obsd-$(DAEMON_NAME)/install" >> $(DAEMON_SRCDIR)/obsd-daemon-packinglist; \
	for file in $(patsubst $(PREFIX)/%, %, $(sort $(DAEMON_RPM_FILES) $(DAEMON_RPM_DOCFILES))); do echo $$file >> $(DAEMON_SRCDIR)/obsd-daemon-packinglist; done; \
	pkg_create -f $(DAEMON_SRCDIR)/obsd-daemon-packinglist -c $(DAEMON_SRCDIR)/obsd-daemon-oneline -d $(DAEMON_SRCDIR)/obsd-daemon-description -v $(DAEMON_NAME); \
	arch="`uname -m`"; \
	mv $(DAEMON_NAME).tgz "$$up/$(DAEMON_ID)-obsd-$$arch.tar.gz"; \
	rm -rf "$$base/obsd-$(DAEMON_NAME)" $(DAEMON_SRCDIR)/obsd-daemon-packinglist $(DAEMON_SRCDIR)/obsd-daemon-oneline $(DAEMON_SRCDIR)/obsd-daemon-description

$(DAEMON_SRCDIR)/obsd-daemon-oneline:
	@perl -ne ' \
		next if /^~+$$/; \
		chop($$section = $$_), next if /^[A-Z]+$$/; \
		chop($$description = $$_) if $$section eq "README" && /^\w/; \
		if ($$section ne "README") \
		{ \
			my ($$name, $$desc) = $$description =~ /^(\w+) - (.*)$$/; \
			print "$$desc\n"; \
			exit; \
		} \
	' < $(DAEMON_SRCDIR)/README > $(DAEMON_SRCDIR)/obsd-daemon-oneline

$(DAEMON_SRCDIR)/obsd-daemon-description:
	@perl -ne ' \
		next if /^~+$$/; \
		chop($$section = $$_), next if /^[A-Z]+$$/; \
		$$description .= $$_ if $$section eq "DESCRIPTION"; \
		if ($$section ne "README" && $$section ne "DESCRIPTION") \
		{ \
			print $$description; \
			exit; \
		} \
	' < $(DAEMON_SRCDIR)/README > $(DAEMON_SRCDIR)/obsd-daemon-description

fbsd-daemon: $(DAEMON_SRCDIR)/fbsd-daemon-oneline $(DAEMON_SRCDIR)/fbsd-daemon-description
	@set -e; \
	base="`pwd`"; \
	up="$$base/.."; \
	mkdir -p "$$base/fbsd-$(DAEMON_NAME)/build"; \
	mkdir -p "$$base/fbsd-$(DAEMON_NAME)/install"; \
	cd "$$base/fbsd-$(DAEMON_NAME)/build"; \
	tar xzf "$$up/$(DAEMON_DIST)"; \
	cd ./$(DAEMON_ID); \
	conf/freebsd; \
	make PREFIX=../../install FINAL_PREFIX="$(PREFIX)" all install-daemon; \
	cd "$$base"; \
	echo "@name $(DAEMON_ID)" > $(DAEMON_SRCDIR)/fbsd-daemon-packinglist; \
	echo "@cwd $(PREFIX)" >> $(DAEMON_SRCDIR)/fbsd-daemon-packinglist; \
	echo "@srcdir $$base/fbsd-$(DAEMON_NAME)/install" >> $(DAEMON_SRCDIR)/fbsd-daemon-packinglist; \
	for file in $(patsubst $(PREFIX)/%, %, $(sort $(DAEMON_RPM_FILES) $(DAEMON_RPM_DOCFILES))); do echo $$file >> $(DAEMON_SRCDIR)/fbsd-daemon-packinglist; done; \
	pkg_create -f $(DAEMON_SRCDIR)/fbsd-daemon-packinglist -c $(DAEMON_SRCDIR)/fbsd-daemon-oneline -d $(DAEMON_SRCDIR)/fbsd-daemon-description -v $(DAEMON_NAME); \
	arch="`uname -m`"; \
	mv $(DAEMON_NAME).tgz "$$up/$(DAEMON_ID)-fbsd-$$arch.tar.gz"; \
	rm -rf "$$base/fbsd-$(DAEMON_NAME)" $(DAEMON_SRCDIR)/fbsd-daemon-packinglist $(DAEMON_SRCDIR)/fbsd-daemon-oneline $(DAEMON_SRCDIR)/fbsd-daemon-description

$(DAEMON_SRCDIR)/fbsd-daemon-oneline:
	@perl -ne ' \
		next if /^~+$$/; \
		chop($$section = $$_), next if /^[A-Z]+$$/; \
		chop($$description = $$_) if $$section eq "README" && /^\w/; \
		if ($$section ne "README") \
		{ \
			my ($$name, $$desc) = $$description =~ /^(\w+) - (.*)$$/; \
			print "$$desc\n"; \
			exit; \
		} \
	' < $(DAEMON_SRCDIR)/README > $(DAEMON_SRCDIR)/fbsd-daemon-oneline

$(DAEMON_SRCDIR)/fbsd-daemon-description:
	@perl -ne ' \
		next if /^~+$$/; \
		chop($$section = $$_), next if /^[A-Z]+$$/; \
		$$description .= $$_ if $$section eq "DESCRIPTION"; \
		if ($$section ne "README" && $$section ne "DESCRIPTION") \
		{ \
			print $$description; \
			exit; \
		} \
	' < $(DAEMON_SRCDIR)/README > $(DAEMON_SRCDIR)/fbsd-daemon-description

# Present make targets separately in help if we are not alone

ifneq ($(DAEMON_SRCDIR), .)
DAEMON_SPECIFIC_HELP := 1
else
ifneq ($(DAEMON_SUBTARGETS),)
DAEMON_SPECIFIC_HELP := 1
endif
endif

ifeq ($(DAEMON_SPECIFIC_HELP), 1)
help::
	@echo " $(DAEMON_NAME)               -- makes $(DAEMON_TARGET) and $(DAEMON_SUBTARGETS)"; \
	echo " man-$(DAEMON_NAME)           -- makes the $(DAEMON_NAME) manpages"; \
	echo " html-$(DAEMON_NAME)          -- makes the $(DAEMON_NAME) manpages in html"; \
	echo " install-daemon        -- installs $(DAEMON_NAME) and its manpage"; \
	echo " install-daemon-bin    -- installs $(DAEMON_NAME) in $(APP_INSDIR)"; \
	echo " install-daemon-man    -- installs the $(DAEMON_NAME) manpage in $(APP_MANDIR)"; \
	echo " install-daemon-html   -- installs the $(DAEMON_NAME) html manpage in $(DAEMON_HTMLDIR)"; \
	echo " install-daemon-conf   -- installs the $(DAEMON_NAME).conf file in $(DAEMON_CONFDIR)"; \
	echo " uninstall-daemon      -- uninstalls $(DAEMON_NAME) and its manpage"; \
	echo " uninstall-daemon-bin  -- uninstalls $(DAEMON_NAME) from $(APP_INSDIR)"; \
	echo " uninstall-daemon-man  -- uninstalls the $(DAEMON_NAME) manpage from $(APP_MANDIR)"; \
	echo " uninstall-daemon-html -- uninstalls the $(DAEMON_NAME) html manpage from $(DAEMON_HTMLDIR)"; \
	echo " uninstall-daemon-conf -- uninstalls the $(DAEMON_NAME).conf file from $(DAEMON_CONFDIR)"; \
	echo " dist-daemon           -- makes a source tarball for daemon+libslack"; \
	echo " dist-html-daemon      -- makes a tarball of daemon's html manpages"; \
	echo " rpm-daemon            -- makes source and binary rpm packages for daemon"; \
	echo " deb-daemon            -- makes source and binary deb package for daemon"; \
	echo " sol-daemon            -- makes a binary solaris package for daemon"; \
	echo " obsd-daemon           -- makes a binary openbsd package for daemon"; \
	echo " fbsd-daemon           -- makes a binary freebsd package for daemon"; \
	echo
endif

help-macros::
	@echo "DAEMON_NAME = $(DAEMON_NAME)"; \
	echo "DAEMON_VERSION = $(DAEMON_VERSION)"; \
	echo "DAEMON_ID = $(DAEMON_ID)"; \
	echo "DAEMON_DIST = $(DAEMON_DIST)"; \
	echo "DAEMON_HTML_DIST = $(DAEMON_HTML_DIST)"; \
	echo "DAEMON_TARGET = $(DAEMON_TARGET)"; \
	echo "DAEMON_MODULES = $(DAEMON_MODULES)"; \
	echo "DAEMON_SRCDIR = $(DAEMON_SRCDIR)"; \
	echo "DAEMON_INCDIRS = $(DAEMON_INCDIRS)"; \
	echo "DAEMON_LIBDIRS = $(DAEMON_LIBDIRS)"; \
	echo "DAEMON_LIBS = $(DAEMON_LIBS)"; \
	echo "DAEMON_CFILES = $(DAEMON_CFILES)"; \
	echo "DAEMON_OFILES = $(DAEMON_OFILES)"; \
	echo "DAEMON_HFILES = $(DAEMON_HFILES)"; \
	echo "DAEMON_HTMLDIR = $(DAEMON_HTMLDIR)"; \
	echo "DAEMON_PODFILES = $(DAEMON_PODFILES)"; \
	echo "DAEMON_MANFILES = $(DAEMON_MANFILES)"; \
	echo "DAEMON_HTMLFILES = $(DAEMON_HTMLFILES)"; \
	echo "DAEMON_CONFFILE = $(DAEMON_CONFFILE)"; \
	echo "DAEMON_RPM_FILES = $(DAEMON_RPM_FILES)"; \
	echo "DAEMON_RPM_DOCFILES = $(DAEMON_RPM_DOCFILES)"; \
	echo "DAEMON_DEFINES = $(DAEMON_DEFINES)"; \
	echo "DAEMON_CPPFLAGS = $(DAEMON_CPPFLAGS)"; \
	echo "DAEMON_CCFLAGS = $(DAEMON_CCFLAGS)"; \
	echo "DAEMON_CFLAGS = $(DAEMON_CFLAGS)"; \
	echo "DAEMON_LDFLAGS = $(DAEMON_LDFLAGS)"; \
	echo "DAEMON_SUBTARGETS = $(DAEMON_SUBTARGETS)"; \
	echo "DAEMON_SUBDIRS = $(DAEMON_SUBDIRS)"; \
	echo

include $(SLACK_SRCDIR)/rules.mk

$(DAEMON_SRCDIR)/%.o: $(DAEMON_SRCDIR)/%.c
	$(CC) $(DAEMON_CFLAGS) -o $@ -c $<

ifneq ($(findstring quotes,$(shell $(POD2MAN) --help 2>&1)),)
NOQUOTES := --quotes=none
endif

$(DAEMON_SRCDIR)/%.$(APP_MANSECT): $(DAEMON_SRCDIR)/%.c
	$(POD2MAN) --center='$(APP_MANSECTNAME)' --section=$(APP_MANSECT) $(NOQUOTES) $< > $@

$(DAEMON_SRCDIR)/%.gz: $(DAEMON_SRCDIR)/%
	$(GZIP) $<

$(DAEMON_SRCDIR)/%.$(APP_MANSECT).html: $(DAEMON_SRCDIR)/%.c
	$(POD2HTML) --noindex < $< > $@ 2>/dev/null

