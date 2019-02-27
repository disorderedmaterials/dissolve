noinst_LTLIBRARIES = libanalyse.la libmodulewidget.la

libanalyse_la_SOURCES = core.cpp definition.cpp functions.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = analyse.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

