noinst_LTLIBRARIES = libdelegates.la

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/$*.h
	${LIBTOOL} --tag=CXX --mode=compile ${CXX} ${AM_CPPFLAGS} -c moc_$*.cpp -o $@
	rm moc_$*.cpp

.hui.lo:
	$(QTMOC) -o $*.cpp -f$< $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) ${AM_CPPFLAGS} -c $*.cpp -o $@
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o moc_*.cpp combolist.cpp exponentialspin.cpp integerspin.cpp isotopecombo.cpp

libdelegates_la_SOURCES = combolist.hui combolist_funcs.cpp
libdelegates_la_SOURCES += exponentialspin.hui exponentialspin_funcs.cpp
libdelegates_la_SOURCES += integerspin.hui integerspin_funcs.cpp
libdelegates_la_SOURCES += intraformcombo.hui intraformcombo_funcs.cpp
libdelegates_la_SOURCES += isotopecombo.hui isotopecombo_funcs.cpp
libdelegates_la_SOURCES += isotopologuecombo.hui isotopologuecombo_funcs.cpp
libdelegates_la_SOURCES += null.cpp
libdelegates_la_SOURCES += usedspeciescombo.hui usedspeciescombo_funcs.cpp

noinst_HEADERS = null.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
