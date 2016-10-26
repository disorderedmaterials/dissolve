noinst_LIBRARIES = libmodules.a

libmodules_a_SOURCES = module.cpp
libmodules_a_SOURCES += atomshake.cpp #energy.cpp forces.cpp grainshake.cpp intrashake.cpp md.cpp perturb.cpp twist.cpp

noinst_HEADERS = module.h
noinst_HEADERS += atomshake.h # energy.h forces.h grainshake.h intrashake.h md.h perturb.h twist.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@
