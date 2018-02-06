noinst_LTLIBRARIES = libkeywordwidgets.la

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/keywordwidgets/$*.h
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
	-rm -f ui_* *.o moc_*.cpp

libkeywordwidgets_la_SOURCES = base.cpp
libkeywordwidgets_la_SOURCES += dropdown.ui dropwidget.hui dropdown_funcs.cpp dropwidget_funcs.cpp

libkeywordwidgets_la_SOURCES += bool.hui bool_funcs.cpp
libkeywordwidgets_la_SOURCES += broadeningfunction.ui broadeningfunction_funcs.cpp
libkeywordwidgets_la_SOURCES += charstring.hui charstring_funcs.cpp
libkeywordwidgets_la_SOURCES += double.hui double_funcs.cpp
libkeywordwidgets_la_SOURCES += int.hui int_funcs.cpp
libkeywordwidgets_la_SOURCES += windowfunction.hui windowfunction_funcs.cpp

noinst_HEADERS = base.h broadeningfunction.h dropdown.h

AM_CPPFLAGS = -I$(top_srcdir)/src @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
