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
	-rm -f ui_* *.o moc_*.cpp combolist.cpp isotopecombo.cpp texponentialspin.cpp

libdelegates_la_SOURCES = combolist.hui combolist_funcs.cpp
libdelegates_la_SOURCES += isotopecombo.hui isotopecombo_funcs.cpp
libdelegates_la_SOURCES += texponentialspin.hui texponentialspin_funcs.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
