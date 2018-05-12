noinst_LTLIBRARIES = libmodulewidget.la

SUFFIXES: .ui .hui

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(builddir)/$*.h
	${LIBTOOL} --tag=CXX --mode=compile ${CXX} -I./ ${AM_CPPFLAGS} -c moc_$*.cpp -o $@
	rm moc_$*.cpp

.hui.lo:
	$(QTMOC) -o $*.cpp -f$< $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) ${AM_CPPFLAGS} -c $*.cpp -o $@
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o moc_*.cpp

libmodulewidget_la_SOURCES = modulewidget.ui modulewidget_funcs.cpp
libmodulewidget_la_SOURCES += gui.cpp 

noinst_HEADERS = modulewidget.h

AM_CPPFLAGS = -I${top_srcdir}/src @DUQ_CFLAGS@ @GUI_CFLAGS@ @DUQ_READLINE@ @GUI_INCLUDES@

