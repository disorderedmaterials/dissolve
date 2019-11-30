noinst_LTLIBRARIES = libviewer.la

SUBDIRS = render

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(builddir)/$*.h
	${LIBTOOL} --tag=CXX --mode=compile ${CXX} ${AM_CPPFLAGS} -c moc_$*.cpp -o $@
	rm moc_$*.cpp

.hui.lo:
	$(QTMOC) -o $*.cpp -f$< $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) ${AM_CPPFLAGS} -c $*.cpp -o $@
	rm $*.cpp

.qrc.lo:
	$(QTRCC) -o $*.cpp -name $* $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) ${AM_CPPFLAGS} -c -o $@ $*.cpp
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o moc_*.cpp configurationviewer.cpp dataviewer.cpp speciesviewer.cpp viewer.cpp

libviewer_la_SOURCES = configurationviewer.hui dataviewer.hui siteviewer.hui speciesviewer.hui viewer.hui
libviewer_la_SOURCES += configurationwidget.ui datawidget.ui selectsymbol.ui sitewidget.ui specieswidget.ui

libviewer_la_SOURCES += configurationviewer_input.cpp configurationviewer_funcs.cpp configurationviewer_input.cpp configurationviewer_interaction.cpp configurationviewer_render.cpp configurationwidget_funcs.cpp
libviewer_la_SOURCES += dataviewer_funcs.cpp dataviewer_input.cpp dataviewer_interaction.cpp dataviewer_render.cpp datawidget_funcs.cpp
libviewer_la_SOURCES += selectsymbol_funcs.cpp
libviewer_la_SOURCES += siteviewer_funcs.cpp siteviewer_input.cpp siteviewer_interaction.cpp siteviewer_render.cpp sitewidget_funcs.cpp
libviewer_la_SOURCES += speciesviewer_funcs.cpp speciesviewer_input.cpp speciesviewer_interaction.cpp speciesviewer_render.cpp specieswidget_funcs.cpp
libviewer_la_SOURCES += viewer_data.cpp viewer_funcs.cpp viewer_input.cpp viewer_interaction.cpp viewer_io.cpp viewer_query.cpp viewer_render.cpp

noinst_HEADERS = configurationwidget.h datawidget.h selectsymbol.h specieswidget.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

