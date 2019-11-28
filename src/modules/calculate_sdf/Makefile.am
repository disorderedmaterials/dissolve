noinst_LTLIBRARIES = libcalculatesdf.la libmodulewidget.la

libcalculatesdf_la_SOURCES = core.cpp definition.cpp functions.cpp init.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = sdf.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

