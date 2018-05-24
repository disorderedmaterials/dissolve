noinst_LTLIBRARIES = libenergy.la libmodulewidget.la

libenergy_la_SOURCES = core.cpp definition.cpp functions.cpp options.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = energy.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

