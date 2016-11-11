noinst_LTLIBRARIES = libmodules.la

libmodules_la_SOURCES = module.cpp
libmodules_la_SOURCES += atomshake.cpp energy.cpp export.cpp fq.cpp partials.cpp # forces.cpp grainshake.cpp intrashake.cpp md.cpp twist.cpp

noinst_HEADERS = module.h
noinst_HEADERS += atomshake.h energy.h export.h fq.h partials.h # forces.h grainshake.h intrashake.h md.h twist.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@
