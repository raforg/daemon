#
# daemon: http://www.zip.com.au/~raf2/lib/software/daemon
#
# Copyright (c) 1999 raf
#

# Uncomment these to override the defines in main.h
# PATH_SEP = /
# ROOT_DIR = /
# PID_DIR = /var/run
# ETC_DIR = /etc
# APP = $(TARGETAPP)
# DEFINES += -DPATH_SEP=\"$(PATH_SEP)\" -DROOT_DIR=\"$(ROOT_DIR)\" -DPID_DIR=\"$(PID_DIR)\" -DETC_DIR=\"$(ETC_DIR)\" -DAPP=\"$(APP)\"

# Uncomment these if your system doesn't have snprintf()
# SNPRINTF = snprintf
# DEFINES += -DNEEDS_SNPRINTF=1

# Uncomment this under SVR4 if not predefined
# DEFINES += -DSVR4

PROJECT = daemon
VERSION = 0.1
TARGETAPP = daemon
TARGETLIBNAME = prog
INSTALLIBNAME = $(TARGETLIBNAME)-$(VERSION)
TARGETLIB = lib$(TARGETLIBNAME).a
INSTALLIB = lib$(INSTALLIBNAME).a
TARGETDIST = $(PROJECT)-$(VERSION).tar.gz

PREFIX = /usr/local
APPINSDIR = $(PREFIX)/bin
LIBINSDIR = $(PREFIX)/lib
MANINSDIR = $(PREFIX)/man
HDRINSDIR = $(PREFIX)/include/$(TARGETLIBNAME)
APPMANSECT = 1
LIBMANSECT = 3

APPFILES = main
APPCFILES = $(patsubst %,%.c,$(APPFILES))
APPOFILES = $(patsubst %,%.o,$(APPFILES))
APPHFILES = # $(patsubst %,%.h,$(APPFILES))
APPMFILES = $(TARGETAPP).$(APPMANSECT)
APPMANDIR = $(MANINSDIR)/man$(APPMANSECT)
APPCATDIR = $(MANINSDIR)/cat$(APPMANSECT)

LIBFILES = daemon prog opt mem msg err sig lim slog fifo $(SNPRINTF)
LIBCFILES = $(patsubst %,%.c,$(LIBFILES))
LIBOFILES = $(patsubst %,%.o,$(LIBFILES))
LIBHFILES = $(patsubst %,%.h,$(LIBFILES))
LIBMFILES = $(patsubst %,%.$(LIBMANSECT),$(LIBFILES)) lib$(TARGETLIBNAME).$(LIBMANSECT)
LIBMANDIR = $(MANINSDIR)/man$(LIBMANSECT)
LIBTESTS = $(patsubst %,%.test,$(LIBFILES))

DEFINES += -DBUILD_PROG

CFLAGS = -g -O2 $(DEFINES) -Wall -pedantic
LDFLAGS = -L. -l$(TARGETLIBNAME)
ARFLAGS = cr

$(TARGETAPP): $(APPOFILES) $(TARGETLIB)
	$(CC) $(CFLAGS) -o $(TARGETAPP) $(APPOFILES) $(LDFLAGS)

$(TARGETLIB): $(LIBOFILES)
	$(AR) $(ARFLAGS) $(TARGETLIB) $(LIBOFILES)

$(LIBTESTS): $(TARGETLIB)
%.test: %.c
	$(CC) $(CFLAGS) -DTEST -o $@ $< $(LDFLAGS)

test: $(LIBTESTS)
	@for i in $(LIBTESTS); do ./$$i; echo; done

tags:
	@ctags $(APPCFILES) $(APPHFILES) $(LIBCFILES) $(LIBHFILES)

depend dep: $(APPCFILES) $(APPHFILES) $(LIBCFILES) $(LIBHFILES)
	@makedepend $(DEFINES) $(APPCFILES) $(LIBCFILES)

clean:
	@rm -f $(APPOFILES) $(LIBOFILES) tags core Makefile.bak .makefile.bak

clobber: clean
	@rm -f $(TARGETAPP) $(TARGETLIB) $(LIBTESTS)

dist-clobber: clobber
	@perl -pi -e 'last if /[D]O NOT DELETE/;' Makefile

dist: dist-clobber
	@srcdir=`basename \`pwd\``; cd ..; tar czf $(TARGETDIST) $$srcdir

install: install-app install-lib
install-app: install-app-bin # install-app-man
install-lib: install-lib-bin install-lib-h # install-lib-man

install-app-bin:
	install -m 555 $(TARGETAPP) $(APPINSDIR)
#install-app-man:
#	install -m 444 $(APPMFILES) $(APPMANDIR)
install-lib-bin:
	install -m 555 $(TARGETLIB) $(LIBINSDIR)/$(INSTALLIB)
	rm -f $(LIBINSDIR)/$(TARGETLIB)
	ln -s $(INSTALLIB) $(LIBINSDIR)/$(TARGETLIB)
install-lib-h:
	-[ ! -d $(HDRINSDIR) ] && mkdir -p $(HDRINSDIR)
	install -m 444 $(LIBHFILES) $(HDRINSDIR)
#install-lib-man:
#	install -m 444 $(LIBMFILES) $(LIBMANDIR)

uninstall: uninstall-app uninstall-lib
uninstall-app: uninstall-app-bin # uninstall-app-man
uninstall-lib: uninstall-lib-bin uninstall-lib-h # uninstall-lib-man

uninstall-app-bin:
	-cd $(APPINSDIR) && rm -f $(TARGETAPP)
#uninstall-app-man:
#	-cd $(APPMANDIR) && rm -f $(APPMFILES) && cd $(APPCATDIR) && rm -f $(APPMFILES)
uninstall-lib-bin:
	-cd $(LIBINSDIR) && rm -f $(TARGETLIB) $(INSTALLIB)
uninstall-lib-h:
	-cd $(HDRINSDIR) && rm -f $(LIBHFILES) && cd .. && rmdir $(HDRINSDIR)
#uninstall-lib-man:
#	cd $(LIBMANDIR) && rm -f $(LIBMFILES)

help:
	@echo "This makefile provides the following targets."
	@echo
	@echo "make $(TARGETAPP)            -- makes $(TARGETAPP) and $(TARGETLIB) (default)"
	@echo "make $(TARGETLIB)       -- just makes $(TARGETLIB)"
	@echo "make tags              -- generates a tags file using ctags"
	@echo "make depend            -- generates source dependencies using makedepend"
	@echo "make dep               -- same as depend"
	@echo "make clean             -- removes object files, tags, core and Makefile.bak"
	@echo "make clobber           -- same as clean but also removes $(TARGETAPP) and $(TARGETLIB)"
	@echo "make dist-clobber      -- same as clobber but also removes source dependencies"
	@echo "make dist              -- creates the distribution: ../$(TARGETDIST)"
	@echo "make install           -- installs everything under $(PREFIX)"
	@echo "make install-app       -- installs $(TARGETAPP) and its man page"
	@echo "make install-app-bin   -- installs $(TARGETAPP) in $(APPINSDIR)"
	@echo "make install-app-man   -- installs the $(TARGETAPP) man page in $(APPMANDIR)"
	@echo "make install-lib       -- installs $(TARGETLIB), its headers and man pages"
	@echo "make install-lib-bin   -- installs $(TARGETLIB) in $(LIBINSDIR)"
	@echo "make install-lib-h     -- installs $(TARGETLIB) headers in $(HDRINSDIR)"
	@echo "make install-lib-man   -- installs $(TARGETLIB) man pages in $(LIBMANDIR)"
	@echo "make uninstall         -- uninstalls everything"
	@echo "make uninstall-app     -- uninstalls $(TARGETAPP) and its man page"
	@echo "make uninstall-app-bin -- uninstalls $(TARGETAPP) from $(APPINSDIR)"
	@echo "make uninstall-app-man -- uninstalls the $(TARGETAPP) man page from $(APPMANDIR)"
	@echo "make uninstall-lib     -- uninstalls $(TARGETLIB), its headers and man pages"
	@echo "make uninstall-lib-bin -- uninstalls $(TARGETLIB) from $(LIBINSDIR)"
	@echo "make uninstall-lib-h   -- uninstalls $(TARGETLIB) headers from $(HDRINSDIR)"
	@echo "make uninstall-lib-man -- uninstalls $(TARGETLIB) man pages from $(LIBMANDIR)"
	@echo "make help              -- shows this list of targets"
	@echo

