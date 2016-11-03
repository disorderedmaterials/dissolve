# Target 'duq'
bin_PROGRAMS = duq

duq_SOURCES = main.cpp

SUBDIRS = base classes main math modules templates

noinst_LTLIBRARIES = libduq.la
libduq_la_SOURCES = 
libduq_la_LIBADD = main/libmain.la modules/libmodules.la classes/libclasses.la base/libbase.la math/libmath.la @MPI_LDLIBS@
libduq_la_LDFLAGS = -static

duq_LDADD = libduq.la @DUQ_CFLAGS@

AM_CPPFLAGS = -I../ @DUQ_CFLAGS@

