noinst_LTLIBRARIES = libff.la

# OPLS-AA (2005)
libff_la_SOURCES = oplsaa2005_atomtypes.cpp
libff_la_SOURCES += oplsaa2005_noblegases.cpp
noinst_HEADERS = oplsaa2005_atomtypes.h
noinst_HEADERS += oplsaa2005_noblegases.h

# SPC/Fw (Wu, Tepper, Voth)
libff_la_SOURCES += spcfw.cpp
noinst_HEADERS += spcfw.h

# Universal Forcefield (Rappe et al.)
libff_la_SOURCES += uff.cpp uffatomtype.cpp
noinst_HEADERS += uff.h uffatomtype.h

AM_CPPFLAGS = -I../../ @DISSOLVE_CFLAGS@

