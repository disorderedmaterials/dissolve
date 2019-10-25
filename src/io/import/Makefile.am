noinst_LTLIBRARIES = libimport.la

libimport_la_SOURCES = coordinates.cpp coordinates_dlpoly.cpp coordinates_epsr.cpp coordinates_xyz.cpp
libimport_la_SOURCES += data1d.cpp data1d_histo.cpp data1d_mint.cpp data1d_xy.cpp data2d.cpp data2d_cartesian.cpp data3d.cpp 
libimport_la_SOURCES += forces.cpp forces_dlpoly.cpp forces_xyz.cpp
libimport_la_SOURCES += trajectory.cpp

noinst_HEADERS = coordinates.h
noinst_HEADERS += data1d.h data2d.h data3d.h
noinst_HEADERS += forces.h
noinst_HEADERS += trajectory.h

AM_CPPFLAGS = -I../../ @DISSOLVE_CFLAGS@
