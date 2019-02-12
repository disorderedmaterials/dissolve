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

#libviewer_la_SOURCES = configurationviewer.hui speciesviewer.hui viewer.hui
libviewer_la_SOURCES = dataviewer.hui viewer.hui
libviewer_la_SOURCES += graphwidget.ui

#libviewer_la_SOURCES += configurationviewer_input.cpp configurationviewer_scene.cpp
#libviewer_la_SOURCES += speciesviewer_funcs.cpp speciesviewer_input.cpp speciesviewer_scene.cpp

libviewer_la_SOURCES += dataviewer_contextmenu.cpp dataviewer_funcs.cpp dataviewer_input.cpp dataviewer_interaction.cpp dataviewer_io.cpp dataviewer_render.cpp
libviewer_la_SOURCES += graphwidget_funcs.cpp
libviewer_la_SOURCES += viewer_funcs.cpp viewer_input.cpp viewer_render.cpp

#noinst_HEADERS = 

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

