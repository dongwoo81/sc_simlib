# Makefile.in generated by automake 1.6.3 from Makefile.am.
# src/sysc/datatypes/fx/Makefile.  Generated from Makefile.in by configure.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../../../..

prefix = /opt/sc_simlib
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/package
pkglibdir = $(libdir)/package
pkgincludedir = $(includedir)/package
top_builddir = ../../../..

ACLOCAL = aclocal-1.6
AUTOCONF = autoconf
AUTOMAKE = automake-1.6
AUTOHEADER = autoheader

am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_alias = 
build_triplet = i686-pc-linux-gnu
host_alias = 
host_triplet = i686-pc-linux-gnu
target_alias = 
target_triplet = i686-pc-linux-gnu

EXEEXT = 
OBJEXT = o
PATH_SEPARATOR = :
AMTAR = tar
AR_TOOL = ar cru
AS = as
AWK = gawk
CC = g++
CXX = g++
DEBUG_CXXFLAGS = -g
DEPDIR = .deps
EXTRA_CXXFLAGS = -Wall -DSC_INCLUDE_FX
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
OPT_CXXFLAGS = -O3
PACKAGE = package
QT_ARCH = iX86
RANLIB = ranlib
STRIP = 
TARGET_ARCH = linux
VERSION = version
am__include = include
am__quote = 
install_sh = /opt/sc_simlib/config/install-sh
DEFS = -I.
INCLUDES = -I$(top_srcdir)/src

MAIN_LIBRARY = libsystemc.a

noinst_LIBRARIES = libfx.a

H_FILES = \
	fx.h \
	sc_context.h \
	sc_fix.h \
	sc_fixed.h \
	sc_fx_ids.h \
	sc_fxcast_switch.h \
	sc_fxdefs.h \
	sc_fxnum.h \
	sc_fxnum_observer.h \
	sc_fxtype_params.h \
	sc_fxval.h \
	sc_fxval_observer.h \
	sc_ufix.h \
	sc_ufixed.h \
	scfx_ieee.h \
	scfx_mant.h \
	scfx_other_defs.h \
	scfx_params.h \
	scfx_rep.h \
	scfx_string.h \
	scfx_utils.h


NO_H_FILES = \
	scfx_pow10.h


noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sc_fxcast_switch.cpp \
	sc_fxdefs.cpp \
	sc_fxnum.cpp \
	sc_fxnum_observer.cpp \
	sc_fxtype_params.cpp \
	sc_fxval.cpp \
	sc_fxval_observer.cpp \
	scfx_mant.cpp \
	scfx_pow10.cpp \
	scfx_rep.cpp \
	scfx_utils.cpp


NO_CXX_FILES = 

libfx_a_SOURCES = $(H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/sysc/datatypes/fx
subdir = src/sysc/datatypes/fx
mkinstalldirs = $(SHELL) $(top_srcdir)/config/mkinstalldirs
CONFIG_CLEAN_FILES =
LIBRARIES = $(noinst_LIBRARIES)

libfx_a_AR = $(AR) cru
libfx_a_LIBADD =
am__objects_1 =
am__objects_2 = sc_fxcast_switch.$(OBJEXT) sc_fxdefs.$(OBJEXT) \
	sc_fxnum.$(OBJEXT) sc_fxnum_observer.$(OBJEXT) \
	sc_fxtype_params.$(OBJEXT) sc_fxval.$(OBJEXT) \
	sc_fxval_observer.$(OBJEXT) scfx_mant.$(OBJEXT) \
	scfx_pow10.$(OBJEXT) scfx_rep.$(OBJEXT) scfx_utils.$(OBJEXT)
am_libfx_a_OBJECTS = $(am__objects_1) $(am__objects_2)
libfx_a_OBJECTS = $(am_libfx_a_OBJECTS)
DEFAULT_INCLUDES =  -I. -I$(srcdir)
CPPFLAGS = 
LDFLAGS = 
LIBS = 
depcomp =
am__depfiles_maybe =
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) \
	-o $@
CXXFLAGS = -Wall -DSC_INCLUDE_FX -O3
CFLAGS = -Wall -O3
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DIST_SOURCES = $(libfx_a_SOURCES)
HEADERS = $(noinst_HEADERS)

DIST_COMMON = $(noinst_HEADERS) Makefile.am Makefile.in
SOURCES = $(libfx_a_SOURCES)

all: all-am

.SUFFIXES:
.SUFFIXES: .cpp .o .obj
$(srcdir)/Makefile.in:  Makefile.am $(top_srcdir)/config/Make-rules $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  --ignore-deps src/sysc/datatypes/fx/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)

AR = ar

clean-noinstLIBRARIES:
	-test -z "$(noinst_LIBRARIES)" || rm -f $(noinst_LIBRARIES)
libfx.a: $(libfx_a_OBJECTS) $(libfx_a_DEPENDENCIES) 
	-rm -f libfx.a
	$(libfx_a_AR) libfx.a $(libfx_a_OBJECTS) $(libfx_a_LIBADD)
	$(RANLIB) libfx.a

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

.cpp.o:
	$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

.cpp.obj:
	$(CXXCOMPILE) -c -o $@ `cygpath -w $<`
uninstall-info-am:

ETAGS = etags
ETAGSFLAGS =

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$tags$$unique" \
	  || $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ../../../..
distdir = $(top_distdir)/$(PACKAGE)-$(VERSION)

distdir: $(DISTFILES)
	@list='$(DISTFILES)'; for file in $$list; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkinstalldirs) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(LIBRARIES) $(HEADERS)

installdirs:

install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-noinstLIBRARIES mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-generic \
	distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am: install-data-local

install-exec-am:

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic

uninstall-am: uninstall-info-am uninstall-local

.PHONY: GTAGS all all-am check check-am clean clean-generic \
	clean-noinstLIBRARIES distclean distclean-compile \
	distclean-generic distclean-tags distdir dvi dvi-am info \
	info-am install install-am install-data install-data-am \
	install-data-local install-exec install-exec-am install-info \
	install-info-am install-man install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic tags uninstall uninstall-am \
	uninstall-info-am uninstall-local


install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(INCLUDE_DIR)
	for file in $(noinst_HEADERS); do \
	  $(INSTALL_DATA) $(srcdir)/$$file $(INCLUDE_DIR)/$$file; \
	done
	$(AR_TOOL) $(top_builddir)/src/$(MAIN_LIBRARY) \
	  $(libfx_a_OBJECTS)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	for file in $(noinst_HEADERS); do \
	  rm -f $(INCLUDE_DIR)/$$file; \
	done
	-rmdir $(INCLUDE_DIR)
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
