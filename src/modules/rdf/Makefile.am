noinst_LTLIBRARIES = librdf.la libmodulewidget.la

librdf_la_SOURCES = core.cpp definition.cpp functions.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = rdf.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

