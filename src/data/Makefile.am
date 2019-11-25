noinst_LTLIBRARIES = libdata.la

SUBDIRS = ff sg

libdata_la_SOURCES = atomicmass.cpp atomicradius.cpp
libdata_la_SOURCES += elementcolours.cpp elements.cpp
libdata_la_SOURCES += ff.cpp ffangleterm.cpp ffatomtype.cpp ffbondterm.cpp ffimproperterm.cpp fflibrary.cpp ffparameters.cpp fftorsionterm.cpp
libdata_la_SOURCES += isotopes.cpp
libdata_la_SOURCES += spacegroups.cpp

noinst_HEADERS = atomicmass.h atomicradius.h
noinst_HEADERS += elementcolours.h elements.h
noinst_HEADERS += ff.h ffangleterm.h ffatomtype.h ffbondterm.h ffimproperterm.h fflibrary.h ffparameters.h fftorsionterm.h
noinst_HEADERS += isotopes.h
noinst_HEADERS += spacegroups.h

AM_CPPFLAGS = -I../ @DISSOLVE_CFLAGS@

