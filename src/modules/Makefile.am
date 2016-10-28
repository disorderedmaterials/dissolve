noinst_LIBRARIES = libmodules.a

libmodules_a_SOURCES = module.cpp
libmodules_a_SOURCES += atomshake.cpp #energy.cpp fq.cpp forces.cpp grainshake.cpp intrashake.cpp md.cpp twist.cpp

noinst_HEADERS = module.h
noinst_HEADERS += atomshake.h # energy.h fq.h forces.h grainshake.h intrashake.h md.h twist.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@
