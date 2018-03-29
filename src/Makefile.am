#
# Target libduq
#

EXTRA_DIST = version.h

SUBDIRS = base classes main math module templates

noinst_LTLIBRARIES = libduq.la
libduq_la_SOURCES = 
libduq_la_LIBADD = main/libmain.la module/libmodule.la module/keywordtypes/libkeywordtypes.la modules/libmodules.la classes/libclasses.la base/libbase.la math/libmath.la @MPI_LDLIBS@
libduq_la_LDFLAGS = -static

#
# Target 'duq' or 'duq-serial'
#

if BUILDPARALLEL

bin_PROGRAMS = duq

duq_SOURCES = main.cpp

duq_LDADD = libduq.la modules/libmoduledummywidgets.la

duq_CPPFLAGS = @DUQ_CFLAGS@

else

bin_PROGRAMS = duq-serial

duq_serial_SOURCES = main.cpp

duq_serial_LDADD = libduq.la modules/libmoduledummywidgets.la

duq_serial_CPPFLAGS = @DUQ_CFLAGS@

endif

# Target 'monitor'
if BUILDGUI

bin_PROGRAMS += duq-gui

duq_gui_SOURCES = duq-gui.cpp

SUBDIRS += gui

noinst_LTLIBRARIES += libduqgui.la
libduqgui_la_SOURCES =
libduqgui_la_LIBADD = libduq.la modules/libmodulewidgets.la gui/libgui.la gui/delegates/libdelegates.la gui/keywordwidgets/libkeywordwidgets.la gui/uchroma/libuchroma.la gui/widgets/libwidgets.la libduq.la
libduqgui_la_LDFLAGS = -static
libduqgui_la_CPPFLAGS = @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

duq_gui_LDADD = libduqgui.la @GUI_LDLIBS@ @MPI_LDLIBS@

duq_gui_CPPFLAGS = @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

endif

# Add on the 'modules' subdirectory target last, so it gets built after the 'gui' target on which it depends
SUBDIRS += modules
