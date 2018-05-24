noinst_LTLIBRARIES = libatomshake.la libmodulewidget.la

libatomshake_la_SOURCES = core.cpp definition.cpp functions.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = atomshake.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

