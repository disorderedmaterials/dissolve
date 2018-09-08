# Modules library and non-GUI dependents
noinst_LTLIBRARIES = libmodules.la libmoduledummywidgets.la

libmodules_la_SOURCES = 
libmodules_la_LIBADD = 
libmodules_la_LDFLAGS = -static

libmodules_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

libmoduledummywidgets_la_SOURCES = 
libmoduledummywidgets_la_LIBADD = 
libmoduledummywidgets_la_LDFLAGS = -static

libmoduledummywidgets_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@

# Module targets
SUBDIRS = analyse
libmodules_la_LIBADD += analyse/libanalyse.la
libmoduledummywidgets_la_LIBADD += analyse/libmodulewidget.la
SUBDIRS += atomshake
libmodules_la_LIBADD += atomshake/libatomshake.la
libmoduledummywidgets_la_LIBADD += atomshake/libmodulewidget.la
SUBDIRS += bragg
libmodules_la_LIBADD += bragg/libbragg.la
libmoduledummywidgets_la_LIBADD += bragg/libmodulewidget.la
SUBDIRS += calibration
libmodules_la_LIBADD += calibration/libcalibration.la
libmoduledummywidgets_la_LIBADD += calibration/libmodulewidget.la
SUBDIRS += checks
libmodules_la_LIBADD += checks/libchecks.la
SUBDIRS += datatest
libmodules_la_LIBADD += datatest/libdatatest.la
libmoduledummywidgets_la_LIBADD += datatest/libmodulewidget.la
SUBDIRS += energy
libmodules_la_LIBADD += energy/libenergy.la
libmoduledummywidgets_la_LIBADD += energy/libmodulewidget.la
SUBDIRS += epsr
libmodules_la_LIBADD += epsr/libepsr.la
libmoduledummywidgets_la_LIBADD += epsr/libmodulewidget.la
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
SUBDIRS += neutronsq
libmodules_la_LIBADD += neutronsq/libneutronsq.la
libmoduledummywidgets_la_LIBADD += neutronsq/libmodulewidget.la
SUBDIRS += refine
libmodules_la_LIBADD += refine/librefine.la
libmoduledummywidgets_la_LIBADD += refine/libmodulewidget.la
SUBDIRS += rdf
libmodules_la_LIBADD += rdf/librdf.la
libmoduledummywidgets_la_LIBADD += rdf/libmodulewidget.la
SUBDIRS += sanitycheck
libmodules_la_LIBADD += sanitycheck/libsanitycheck.la
SUBDIRS += skeleton
libmodules_la_LIBADD += skeleton/libskeleton.la
libmoduledummywidgets_la_LIBADD += skeleton/libmodulewidget.la
SUBDIRS += sq
libmodules_la_LIBADD += sq/libsq.la
libmoduledummywidgets_la_LIBADD += sq/libmodulewidget.la
SUBDIRS += test
libmodules_la_LIBADD += test/libtest.la
libmoduledummywidgets_la_LIBADD += test/libmodulewidget.la

# Include modules in Calculate directory
SUBDIRS += calculate
libmodules_la_LIBADD += calculate/libcalculatemodules.la
libmoduledummywidgets_la_LIBADD += calculate/libcalculatemodulesdummywidgets.la

# GUI targets for modules
if BUILDGUI

noinst_LTLIBRARIES += libmodulewidgets.la

libmodulewidgets_la_SOURCES = 
libmodulewidgets_la_LIBADD = 
libmodulewidgets_la_LDFLAGS = -static

libmodulewidgets_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@ @DISSOLVE_READLINE@ @GUI_INCLUDES@

libmodulewidgets_la_LIBADD += analyse/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += atomshake/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += calibration/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += energy/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += epsr/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += neutronsq/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += rdf/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += refine/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += skeleton/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += sq/gui/libmodulewidget.la
libmodulewidgets_la_LIBADD += test/gui/libmodulewidget.la

# Add calculate modules GUI widgets
libmodulewidgets_la_LIBADD += calculate/libcalculatemoduleswidgets.la

endif

