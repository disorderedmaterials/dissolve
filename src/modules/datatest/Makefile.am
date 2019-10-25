noinst_LTLIBRARIES = libdatatest.la libmodulewidget.la

libdatatest_la_SOURCES = core.cpp definition.cpp functions.cpp init.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = datatest.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

