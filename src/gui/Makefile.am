noinst_LTLIBRARIES = libgui.la

SUBDIRS = delegates keywordwidgets uchroma widgets

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
	-rm -f ui_* *.o moc_*.cpp icons.cpp tmdiarea.cpp

libgui_la_SOURCES = icons.qrc

libgui_la_SOURCES += gui.ui configurationtab.ui flowblock.ui forcefieldtab.ui modulecontrolwidget.ui moduletab.ui pairpotentialwidget.ui paletteblock.ui processingtab.ui speciestab.ui workspacetab.ui

libgui_la_SOURCES += modulechart.hui modulepalette.hui outputhandler.hui thread.hui

libgui_la_SOURCES += configurationtab_funcs.cpp
libgui_la_SOURCES += flowblock_funcs.cpp forcefieldtab_funcs.cpp
libgui_la_SOURCES += gui_control.cpp gui_funcs.cpp gui_menu.cpp gui_tabs.cpp
libgui_la_SOURCES += maintab.cpp modulechart_funcs.cpp modulecontrolwidget_funcs.cpp modulekeywordswidget_funcs.cpp modulepalette_funcs.cpp moduletab_funcs.cpp modulewidget.cpp
libgui_la_SOURCES += outputhandler_funcs.cpp
libgui_la_SOURCES += pairpotentialwidget_funcs.cpp paletteblock_funcs.cpp processingtab_funcs.cpp
libgui_la_SOURCES += speciestab_funcs.cpp
libgui_la_SOURCES += thread_funcs.cpp
libgui_la_SOURCES += tmdiarea.hui tmdiarea_funcs.cpp
libgui_la_SOURCES += workspacetab_funcs.cpp

noinst_HEADERS = configurationtab.h
noinst_HEADERS += flowblock.h forcefieldtab.h
noinst_HEADERS += gui.h
noinst_HEADERS += maintab.h modulecontrolwidget.h modulekeywordswidget.h moduletab.h modulewidget.h
noinst_HEADERS += pairpotentialwidget.h paletteblock.h processingtab.h
noinst_HEADERS += speciestab.h
noinst_HEADERS += workspacetab.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
