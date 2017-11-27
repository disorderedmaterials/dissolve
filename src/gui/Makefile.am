noinst_LTLIBRARIES = libgui.la

SUBDIRS = uchroma

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
	-rm -f ui_* *.o moc_*.cpp colourbutton.cpp gradientbar.cpp icons.cpp texponentialspin.cpp

libgui_la_SOURCES = icons.qrc

libgui_la_SOURCES += keywordwidget_bool.hui keywordwidget_charstring.hui keywordwidget_double.hui keywordwidget_int.hui

libgui_la_SOURCES += colourbutton.hui colourbutton_funcs.cpp
libgui_la_SOURCES += gradientbar.hui gradientbar_funcs.cpp
libgui_la_SOURCES += texponentialspin.hui texponentialspin_funcs.cpp

libgui_la_SOURCES += browser.ui mainwindow.ui modulecontrolwidget.ui pairpotentialwidget.ui

libgui_la_SOURCES += browser_funcs.cpp modulecontrolwidget_funcs.cpp pairpotentialwidget_funcs.cpp 
libgui_la_SOURCES += modulewidget.cpp
libgui_la_SOURCES += subwidget.cpp subwindow.cpp
libgui_la_SOURCES += mainwindow_funcs.cpp

noinst_HEADERS = browser.h mainwindow.h modulecontrolwidget.h modulewidget.h pairpotentialwidget.h subwidget.h subwindow.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
