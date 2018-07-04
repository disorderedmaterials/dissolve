noinst_LTLIBRARIES = libepsr.la libmodulewidget.la

libepsr_la_SOURCES = core.cpp definition.cpp functions.cpp io.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = epsr.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

