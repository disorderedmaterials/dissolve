noinst_LTLIBRARIES = libmodules.la

libmodules_la_SOURCES = module.cpp modulelist.cpp moduleregistry.cpp
libmodules_la_SOURCES += atomshake.cpp energy.cpp export.cpp partials.cpp skeleton.cpp # forces.cpp grainshake.cpp intrashake.cpp md.cpp twist.cpp

noinst_HEADERS = module.h modulelist.h moduleregistry.h
noinst_HEADERS += atomshake.h energy.h export.h partials.h skeleton.h # forces.h grainshake.h intrashake.h md.h twist.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@
