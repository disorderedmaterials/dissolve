noinst_LTLIBRARIES = libneutronsq.la libmodulewidget.la

libneutronsq_la_SOURCES = core.cpp definition.cpp functions.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = neutronsq.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

