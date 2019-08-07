#
# Target libdissolve
#

EXTRA_DIST = version.h

SUBDIRS = base classes data expression genericitems io keywords main math module procedure templates

noinst_LTLIBRARIES = libdissolve.la
libdissolve_la_SOURCES = 
libdissolve_la_LIBADD = main/libmain.la module/libmodule.la io/libimportexport.la keywords/libkeywords.la modules/libmodules.la classes/libclasses.la procedure/libprocedure.la procedure/nodes/libprocedurenodes.la expression/libexpression.la genericitems/libgenericitems.la base/libbase.la math/libmath.la data/libdata.la data/ff/libff.la @MPI_LDLIBS@
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
libdissolvegui_la_LIBADD = libdissolve.la modules/libmodulewidgets.la io/libimportexport.la gui/libgui.la gui/delegates/libdelegates.la gui/charts/libcharts.la gui/keywordwidgets/libkeywordwidgets.la gui/viewer/libviewer.la gui/viewer/render/librender.la gui/widgets/libwidgets.la libdissolve.la
libdissolvegui_la_LDFLAGS = -static
libdissolvegui_la_CPPFLAGS = @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

dissolve_gui_LDADD = libdissolvegui.la @GUI_LDLIBS@ @MPI_LDLIBS@

dissolve_gui_CPPFLAGS = @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

endif

# Add on the 'modules' subdirectory target last, so it gets built after the 'gui' target on which it depends
SUBDIRS += modules
