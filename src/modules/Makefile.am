# Modules library and non-GUI dependents
noinst_LTLIBRARIES = libmodules.la libmoduledummywidgets.la

libmodules_la_SOURCES = 
libmodules_la_LIBADD = 
libmodules_la_LDFLAGS = -static

libmodules_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

libmoduledummywidgets_la_SOURCES = 
libmoduledummywidgets_la_LIBADD = 
libmoduledummywidgets_la_LDFLAGS = -static

libmoduledummywidgets_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@

# Module targets
SUBDIRS = atomshake
libmodules_la_LIBADD += atomshake/libatomshake.la
libmoduledummywidgets_la_LIBADD += atomshake/libmodulewidget.la
SUBDIRS += checks
libmodules_la_LIBADD += checks/libchecks.la
SUBDIRS += energy
libmodules_la_LIBADD += energy/libenergy.la
libmoduledummywidgets_la_LIBADD += energy/libmodulewidget.la
SUBDIRS += export
libmodules_la_LIBADD += export/libexport.la
SUBDIRS += forces
libmodules_la_LIBADD += forces/libforces.la
SUBDIRS += import
libmodules_la_LIBADD += import/libimport.la
SUBDIRS += intrashake
libmodules_la_LIBADD += intrashake/libintrashake.la
SUBDIRS += md
libmodules_la_LIBADD += md/libmd.la
SUBDIRS += molshake
libmodules_la_LIBADD += molshake/libmolshake.la
SUBDIRS += partials
libmodules_la_LIBADD += partials/libpartials.la
libmoduledummywidgets_la_LIBADD += partials/libmodulewidget.la
SUBDIRS += refine
libmodules_la_LIBADD += refine/librefine.la
libmoduledummywidgets_la_LIBADD += refine/libmodulewidget.la
SUBDIRS += skeleton
libmodules_la_LIBADD += skeleton/libskeleton.la
libmoduledummywidgets_la_LIBADD += skeleton/libmodulewidget.la

# GUI targets for modules
if BUILDGUI

noinst_LTLIBRARIES += libmodulewidgets.la

libmodulewidgets_la_SOURCES = 
libmodulewidgets_la_LIBADD = 
libmodulewidgets_la_LDFLAGS = -static

libmodulewidgets_CPPFLAGS = -I../ -I../../ @DUQ_CFLAGS@ @DUQ_READLINE@ @GUI_INCLUDES@

libmodulewidgets_la_LIBADD += atomshake/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += energy/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += partials/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += refine/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += skeleton/gui/libmodulewidget.la

endif

