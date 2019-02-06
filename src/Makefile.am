#
# Target libdissolve
#

EXTRA_DIST = version.h

SUBDIRS = analyse base classes data expression main math module genericitems templates

noinst_LTLIBRARIES = libdissolve.la
libdissolve_la_SOURCES = 
libdissolve_la_LIBADD = main/libmain.la module/libmodule.la module/keywordtypes/libkeywordtypes.la modules/libmodules.la classes/libclasses.la analyse/libanalyse.la analyse/nodes/libanalysisnodes.la expression/libexpression.la base/libbase.la math/libmath.la data/libdata.la @MPI_LDLIBS@
libdissolve_la_LDFLAGS = -static

#
# Target 'dissolve' or 'dissolve-serial'
#

if BUILDPARALLEL

bin_PROGRAMS = dissolve

dissolve_SOURCES = main.cpp

dissolve_LDADD = libdissolve.la modules/libmoduledummywidgets.la

dissolve_CPPFLAGS = @DISSOLVE_CFLAGS@

else

bin_PROGRAMS = dissolve-serial

dissolve_serial_SOURCES = main.cpp

dissolve_serial_LDADD = libdissolve.la modules/libmoduledummywidgets.la

dissolve_serial_CPPFLAGS = @DISSOLVE_CFLAGS@

endif

# Target 'dissolve-gui'
if BUILDGUI

# Main GUI
bin_PROGRAMS += dissolve-gui

dissolve_gui_SOURCES = dissolve-gui.cpp

SUBDIRS += gui

noinst_LTLIBRARIES += libdissolvegui.la
libdissolvegui_la_SOURCES =
libdissolvegui_la_LIBADD = libdissolve.la modules/libmodulewidgets.la gui/libgui.la gui/delegates/libdelegates.la gui/keywordwidgets/libkeywordwidgets.la gui/uchroma/libuchroma.la gui/viewer/libviewer.la gui/widgets/libwidgets.la libdissolve.la
libdissolvegui_la_LDFLAGS = -static
libdissolvegui_la_CPPFLAGS = @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

dissolve_gui_LDADD = libdissolvegui.la @GUI_LDLIBS@ @MPI_LDLIBS@

dissolve_gui_CPPFLAGS = @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

# Guide Editor
bin_PROGRAMS += dguided

dguided_SOURCES = dguided.cpp

dguided_LDADD = gui/libdguided.la libdissolvegui.la @GUI_LDLIBS@ @MPI_LDLIBS@

dguided_CPPFLAGS = @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

endif

# Add on the 'modules' subdirectory target last, so it gets built after the 'gui' target on which it depends
SUBDIRS += modules
