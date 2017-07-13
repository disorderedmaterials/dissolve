noinst_LTLIBRARIES = libgui.la

SUBDIRS = qcustomplot

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/$*.h
	${LIBTOOL} --tag=CXX --mode=compile ${CXX} -I$(top_srcdir)/src -I../ -I./ ${AM_CPPFLAGS} -c moc_$*.cpp -o $@
	rm moc_$*.cpp

.hui.lo:
	$(QTMOC) -o $*.cpp -f$< $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) -I$(top_srcdir)/src -I../ -I./ ${AM_CPPFLAGS} -c $*.cpp -o $@
	rm $*.cpp

.qrc.lo:
	$(QTRCC) -o $*.cpp -name $* $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) -I$(top_srcdir)/src -I../ -I./ ${AM_CPPFLAGS} -c -o $@ $*.cpp
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o moc_*.cpp icons.cpp

libgui_la_SOURCES = # icons.qrc mainwindow.ui

noinst_HEADERS = # mainwindow.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_INCLUDES@ @DUQ_CFLAGS@
