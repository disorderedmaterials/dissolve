SUBDIRS = base classes main math modules templates

bin_PROGRAMS = duq

# Target 'duq'
duq_SOURCES = main.cpp

duq_LDADD = main/libmain.a classes/libclasses.a base/libbase.a math/libmath.a -lreadline @MPI_LDLIBS@

AM_CPPFLAGS = -I../ @DUQ_CFLAGS@

