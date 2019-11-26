noinst_LTLIBRARIES = libcalculatecn.la libmodulewidget.la

libcalculatecn_la_SOURCES = core.cpp definition.cpp functions.cpp init.cpp process.cpp
libmodulewidget_la_SOURCES = nogui.cpp

noinst_HEADERS = cn.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

# GUI elements
if BUILDGUI

SUBDIRS = gui

endif

