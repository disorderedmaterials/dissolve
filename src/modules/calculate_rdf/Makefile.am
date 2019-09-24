noinst_LTLIBRARIES = libcalculaterdf.la libmodulewidget.la

libcalculaterdf_la_SOURCES = core.cpp definition.cpp functions.cpp init.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = rdf.h

AM_CPPFLAGS = -I${top_srcdir}/src @DISSOLVE_CFLAGS@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

