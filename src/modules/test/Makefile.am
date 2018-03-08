noinst_LTLIBRARIES = libtest.la libmodulewidget.la

libtest_la_SOURCES = core.cpp definition.cpp functions.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = test.h

AM_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

