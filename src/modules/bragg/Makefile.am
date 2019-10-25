noinst_LTLIBRARIES = libbragg.la libmodulewidget.la

libbragg_la_SOURCES = core.cpp definition.cpp functions.cpp init.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = bragg.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

