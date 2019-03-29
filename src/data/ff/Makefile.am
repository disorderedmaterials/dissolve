noinst_LTLIBRARIES = libff.la

# SPC/Fw (Wu, Tepper, Voth)
libff_la_SOURCES = spcfw.cpp
noinst_HEADERS = spcfw.h

# Universal Forcefield (Rappe et al.)
libff_la_SOURCES += uff.cpp uffatomtype.cpp
noinst_HEADERS += uff.h uffatomtype.h

AM_CPPFLAGS = -I../../ @DISSOLVE_CFLAGS@

