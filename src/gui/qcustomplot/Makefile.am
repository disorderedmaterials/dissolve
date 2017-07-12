noinst_LTLIBRARIES = libqcustomplot.la

SUFFIXES: .hui

.hui.lo:
	$(QTMOC) -o $*.cpp -f$< $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) -I$(top_srcdir)/src -I../../ -I./ ${AM_CPPFLAGS} -c $*.cpp -o $@
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o moc_*.cpp qcustomplot.cpp tplotwidget.cpp


libqcustomplot_la_SOURCES = plotdata.cpp plotdatastyle.cpp qcustomplot.hui qcustomplot_funcs.cpp tplotwidget.hui tplotwidget_funcs.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src -I../../ -I$(top_srcdir)/src/gui @ATEN_INCLUDES@ @ATEN_CFLAGS@

noinst_HEADERS = plotdata.h plotdatastyle.h
