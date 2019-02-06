noinst_LTLIBRARIES = libviewer.la

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
	-rm -f ui_* *.o moc_*.cpp configurationviewer.cpp speciesviewer.cpp viewer.cpp

libviewer_la_SOURCES = configurationviewer.hui speciesviewer.hui viewer.hui

libviewer_la_SOURCES += configurationviewer_input.cpp configurationviewer_scene.cpp
libviewer_la_SOURCES += speciesviewer_funcs.cpp speciesviewer_input.cpp speciesviewer_scene.cpp
libviewer_la_SOURCES += primitive.cpp primitiveinfo.cpp primitivevertex.cpp
libviewer_la_SOURCES += textprimitive.cpp
libviewer_la_SOURCES += vertexchunk.cpp viewer_engine.cpp viewer_funcs.cpp viewer_input.cpp viewer_prefs.cpp

noinst_HEADERS = primitive.h primitiveinfo.h textprimitive.h vertexchunk.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

