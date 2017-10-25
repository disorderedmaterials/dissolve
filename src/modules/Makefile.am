noinst_LTLIBRARIES = libmodules.la

libmodules_la_SOURCES = module.cpp modulekeywordbase.cpp modulekeywordlist.cpp modulelist.cpp moduleregistry.cpp
libmodules_la_SOURCES += modulekeyword_bool.cpp modulekeyword_charstring.cpp modulekeyword_complex.cpp modulekeyword_double.cpp modulekeyword_function.cpp modulekeyword_int.cpp
libmodules_la_LIBADD = 
libmodules_la_LDFLAGS = -static

noinst_HEADERS = module.h modulekeywordbase.h modulekeyworddata.h modulekeywordlist.h modulelist.h moduleregistry.h
noinst_HEADERS += modulekeyword_bool.h modulekeyword_charstring.h modulekeyword_complex.h modulekeyword_double.h modulekeyword_function.h modulekeyword_int.h modulekeywords.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

# Module targets
SUBDIRS = atomshake
libmodules_la_LIBADD += atomshake/libatomshake.la
SUBDIRS += checks
libmodules_la_LIBADD += checks/libchecks.la
SUBDIRS += energy
libmodules_la_LIBADD += energy/libenergy.la
SUBDIRS += export
libmodules_la_LIBADD += export/libexport.la
SUBDIRS += forces
libmodules_la_LIBADD += forces/libforces.la
SUBDIRS += import
libmodules_la_LIBADD += import/libimport.la
SUBDIRS += md
libmodules_la_LIBADD += md/libmd.la
SUBDIRS += molshake
libmodules_la_LIBADD += molshake/libmolshake.la
SUBDIRS += partials
libmodules_la_LIBADD += partials/libpartials.la
SUBDIRS += skeleton
libmodules_la_LIBADD += skeleton/libskeleton.la
SUBDIRS += test
libmodules_la_LIBADD += test/libtest.la
