# Target 'duq'
bin_PROGRAMS = duq

duq_SOURCES = main.cpp base/messenger_printtext.cpp

EXTRA_DIST = version.h

SUBDIRS = base classes main math module modules templates

noinst_LTLIBRARIES = libduq.la
libduq_la_SOURCES = 
libduq_la_LIBADD = main/libmain.la module/libmodule.la module/keywordtypes/libkeywordtypes.la modules/libmodules.la classes/libclasses.la base/libbase.la math/libmath.la @MPI_LDLIBS@
libduq_la_LDFLAGS = -static

duq_LDADD = libduq.la

duq_CPPFLAGS = @DUQ_CFLAGS@

# Target 'monitor'
if BUILDGUI

bin_PROGRAMS += monitor

monitor_SOURCES = monitor.cpp gui/messenger_printtext.cpp

SUBDIRS += gui

monitor_LDADD = libduq.la gui/libgui.la gui/qcustomplot/libqcustomplot.la @GUI_LDLIBS@

monitor_CPPFLAGS = @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

endif

