# Target 'duq'
bin_PROGRAMS = duq

duq_SOURCES = main.cpp base/messenger_printtext.cpp

EXTRA_DIST = version.h

SUBDIRS = base classes main math module templates

noinst_LTLIBRARIES = libduq.la
libduq_la_SOURCES = 
libduq_la_LIBADD = main/libmain.la module/libmodule.la module/keywordtypes/libkeywordtypes.la modules/libmodules.la classes/libclasses.la base/libbase.la math/libmath.la @MPI_LDLIBS@
libduq_la_LDFLAGS = -static

duq_LDADD = libduq.la modules/libmoduledummywidgets.la

duq_CPPFLAGS = @DUQ_CFLAGS@

# Target 'monitor'
if BUILDGUI

bin_PROGRAMS += monitor

monitor_SOURCES = monitor.cpp

SUBDIRS += gui

noinst_LTLIBRARIES += libduqgui.la
libduqgui_la_SOURCES = gui/messenger_printtext.cpp
libduqgui_la_LIBADD = libduq.la modules/libmodulewidgets.la gui/libgui.la gui/uchroma/libuchroma.la libduq.la
libduqgui_la_LDFLAGS = -static

monitor_LDADD = libduqgui.la @GUI_LDLIBS@ @MPI_LDLIBS@

monitor_CPPFLAGS = @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

endif

# Add on the 'modules' subdirectory target last, so it gets built after the 'gui' target on which it depends
SUBDIRS += modules
