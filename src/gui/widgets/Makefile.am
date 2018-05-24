noinst_LTLIBRARIES = libwidgets.la

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

.qrc.lo:
	$(QTRCC) -o $*.cpp -name $* $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) ${AM_CPPFLAGS} -c -o $@ $*.cpp
	rm $*.cpp

clean-local:
	-rm -f ui_* *.o moc_*.cpp colourbutton.cpp gradientbar.cpp mimetreewidget.cpp supertabbar.cpp supertabwidget.cpp texponentialspin.cpp tmenubutton.cpp tpopupwidget.cpp

libwidgets_la_SOURCES = colourbutton.hui gradientbar.hui mimetreewidget.hui supertabbar.hui supertabwidget.hui texponentialspin.hui tmenubutton.hui tpopupwidget.hui

libwidgets_la_SOURCES += colourbutton_funcs.cpp
libwidgets_la_SOURCES += gradientbar_funcs.cpp
libwidgets_la_SOURCES += mimestrings.cpp mimetreewidget_funcs.cpp mimetreewidgetitem.cpp
libwidgets_la_SOURCES += subwidget.cpp subwindow.cpp supertabbar_funcs.cpp supertabwidget_funcs.cpp
libwidgets_la_SOURCES += texponentialspin_funcs.cpp
libwidgets_la_SOURCES += tmenubutton_funcs.cpp tmenubuttongroup.cpp tpopupwidget_funcs.cpp


noinst_HEADERS = mimestrings.h mimetreewidgetitem.h
noinst_HEADERS += subwidget.h subwindow.h
noinst_HEADERS += tmenubuttongroup.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
