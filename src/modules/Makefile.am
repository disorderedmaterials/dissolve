noinst_LTLIBRARIES = libmodules.la

libmodules_la_SOURCES = module.cpp modulelist.cpp moduleregistry.cpp
libmodules_la_SOURCES += atomshake.cpp energy.cpp export.cpp md.cpp partials.cpp skeleton.cpp test.cpp # forces.cpp grainshake.cpp intrashake.cpp twist.cpp

noinst_HEADERS = module.h modulelist.h moduleregistry.h
noinst_HEADERS += atomshake.h energy.h export.h md.h partials.h skeleton.h test.h # forces.h grainshake.h intrashake.htwist.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@
