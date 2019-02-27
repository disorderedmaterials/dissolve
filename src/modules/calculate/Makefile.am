# Calculate modules library and non-GUI dependents
noinst_LTLIBRARIES = libcalculatemodules.la libcalculatemodulesdummywidgets.la

libcalculatemodules_la_SOURCES = 
libcalculatemodules_la_LIBADD = 
libcalculatemodules_la_LDFLAGS = -static

libcalculatemodules_CPPFLAGS = -I${top_srcdir}/src @DISSOLVE_CFLAGS@

libcalculatemodulesdummywidgets_la_SOURCES = 
libcalculatemodulesdummywidgets_la_LIBADD = 
libcalculatemodulesdummywidgets_la_LDFLAGS = -static

libcalculatemodulesdummywidgets_CPPFLAGS = -I${top_srcdir}/src @DISSOLVE_CFLAGS@

# Module targets
SUBDIRS = dangle
libcalculatemodules_la_LIBADD += dangle/libcalculatedangle.la
libcalculatemodulesdummywidgets_la_LIBADD += dangle/libmodulewidget.la
SUBDIRS += rdf
libcalculatemodules_la_LIBADD += rdf/libcalculaterdf.la
libcalculatemodulesdummywidgets_la_LIBADD += rdf/libmodulewidget.la

# GUI targets for modules
if BUILDGUI

noinst_LTLIBRARIES += libcalculatemoduleswidgets.la

libcalculatemoduleswidgets_la_SOURCES = 
libcalculatemoduleswidgets_la_LIBADD = 
libcalculatemoduleswidgets_la_LDFLAGS = -static

libcalculatemoduleswidgets_CPPFLAGS = -I${top_srcdir}/src @DISSOLVE_CFLAGS@ @GUI_INCLUDES@

libcalculatemoduleswidgets_la_LIBADD += dangle/gui/libmodulewidget.la
libcalculatemoduleswidgets_la_LIBADD += rdf/gui/libmodulewidget.la

endif

