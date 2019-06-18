noinst_LTLIBRARIES = libimport.la

libimport_la_SOURCES = core.cpp
libimport_la_SOURCES += definition.cpp
libimport_la_SOURCES += formats.cpp
libimport_la_SOURCES += functions_coordinates.cpp functions_coordinates_dlpoly.cpp  functions_coordinates_epsr.cpp  functions_coordinates_xyz.cpp
libimport_la_SOURCES += functions_data1d.cpp
libimport_la_SOURCES += functions_forces.cpp
libimport_la_SOURCES += options.cpp
libimport_la_SOURCES += process.cpp

noinst_HEADERS = formats.h import.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@
