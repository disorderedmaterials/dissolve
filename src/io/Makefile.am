# Import/Export Library
noinst_LTLIBRARIES = libimportexport.la

libimportexport_la_SOURCES = fileandformat.cpp
libimportexport_la_LIBADD = 
libimportexport_la_LDFLAGS = -static

libimportexport_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

noinst_HEADERS = fileandformat.h

# Subdirectories
SUBDIRS = export import
libimportexport_la_LIBADD += export/libexport.la import/libimport.la
