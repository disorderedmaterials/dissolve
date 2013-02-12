bin_PROGRAMS = duq

# Target 'duq'
duq_SOURCES = main.cpp ../lib/base/mutex.cpp

duq_LDADD = ../lib/main/libmain.a ../lib/classes/libclasses.a ../lib/base/libbase.a ../lib/math/libmath.a -lreadline @MPI_LDLIBS@

INCLUDES = -I../ -I../lib @DUQ_CFLAGS@

