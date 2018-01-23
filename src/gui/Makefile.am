noinst_LTLIBRARIES = libgui.la

SUBDIRS = keywordwidgets uchroma

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

libgui_la_SOURCES += colourbutton.hui colourbutton_funcs.cpp
libgui_la_SOURCES += gradientbar.hui gradientbar_funcs.cpp
libgui_la_SOURCES += mimestrings.cpp mimetreewidget.hui mimetreewidget_funcs.cpp mimetreewidgetitem.cpp
libgui_la_SOURCES += texponentialspin.hui texponentialspindelegate.hui texponentialspin_funcs.cpp texponentialspindelegate_funcs.cpp
libgui_la_SOURCES += tmenubutton.hui tpopupwidget.hui tmenubutton_funcs.cpp tmenubuttongroup.cpp tpopupwidget_funcs.cpp

libgui_la_SOURCES += browser.ui gui.ui mastertermswidget.ui modulecontrolwidget.ui pairpotentialwidget.ui

libgui_la_SOURCES += browser_funcs.cpp mastertermswidget_funcs.cpp modulecontrolwidget_funcs.cpp pairpotentialwidget_funcs.cpp 
libgui_la_SOURCES += gui_funcs.cpp
libgui_la_SOURCES += modulewidget.cpp
libgui_la_SOURCES += outputhandler.hui outputhandler_funcs.cpp
libgui_la_SOURCES += subwidget.cpp subwindow.cpp
libgui_la_SOURCES += thread.hui thread_funcs.cpp

noinst_HEADERS = browser.h gui.h mimestrings.h mimetreewidgetitem.h mastertermswidget.h modulecontrolwidget.h modulewidget.h pairpotentialwidget.h subwidget.h subwindow.h tmenubuttongroup.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
