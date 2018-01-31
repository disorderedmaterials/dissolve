noinst_LTLIBRARIES = librefine.la libmodulewidget.la

librefine_la_SOURCES = core.cpp definition.cpp functions.cpp method.cpp options.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = refine.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

