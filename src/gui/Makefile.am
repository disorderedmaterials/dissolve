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
	-rm -f ui_* *.o moc_*.cpp main.cpp tmdiarea.cpp

libgui_la_SOURCES = main.qrc

# UI - Module Chart
libgui_la_SOURCES += modulechartinsertionblock.ui modulechartmoduleblock.ui modulecontrolwidget.ui moduleeditor.ui paletteblock.ui
libgui_la_SOURCES += modulechart.hui modulepalette.hui
# UI - Widgets
libgui_la_SOURCES += addspecieswizard.ui pairpotentialwidget.ui wizardwidget.hui wizardwidgetfooter.ui wizardwidgetheader.ui
# UI - Dialogs
libgui_la_SOURCES += addspeciesdialog.ui
# UI - Main Tabs
libgui_la_SOURCES += gui.ui analysistab.ui configurationtab.ui forcefieldtab.ui moduletab.ui processingtab.ui speciestab.ui workspacetab.ui
# UI - Utility Classes
libgui_la_SOURCES += outputhandler.hui thread.hui

# Implementations
libgui_la_SOURCES += addspeciesdialog_funcs.cpp addspecieswizard_funcs.cpp analysistab_funcs.cpp
libgui_la_SOURCES += configurationtab_funcs.cpp
libgui_la_SOURCES += forcefieldtab_funcs.cpp
libgui_la_SOURCES += gui_funcs.cpp gui_menu.cpp gui_simulation_control.cpp gui_simulation_state.cpp gui_simulation_tabs.cpp gui_stack.cpp gui_start.cpp
libgui_la_SOURCES += maintab.cpp
libgui_la_SOURCES += modulechart_funcs.cpp modulechartblock.cpp modulecharthotspot.cpp modulechartinsertionblock_funcs.cpp modulechartmoduleblock_funcs.cpp
libgui_la_SOURCES += modulecontrolwidget_funcs.cpp moduleeditor_funcs.cpp modulekeywordswidget_funcs.cpp modulepalette_funcs.cpp moduletab_funcs.cpp modulewidget.cpp
libgui_la_SOURCES += outputhandler_funcs.cpp
libgui_la_SOURCES += pairpotentialwidget_funcs.cpp paletteblock_funcs.cpp processingtab_funcs.cpp
libgui_la_SOURCES += speciestab_funcs.cpp
libgui_la_SOURCES += thread_funcs.cpp
libgui_la_SOURCES += tmdiarea.hui tmdiarea_funcs.cpp
libgui_la_SOURCES += wizardwidget_funcs.cpp wizardwidgetpageinfo.cpp workspacetab_funcs.cpp

noinst_HEADERS = addspeciesdialog.h addspecieswizard.h analysistab.h
noinst_HEADERS += configurationtab.h
noinst_HEADERS += forcefieldtab.h
noinst_HEADERS += gui.h
noinst_HEADERS += maintab.h modulechartblock.h modulecharthotspot.h modulechartinsertionblock.h modulechartmetrics.h modulechartmoduleblock.h modulecontrolwidget.h moduleeditor.h modulekeywordswidget.h moduletab.h modulewidget.h
noinst_HEADERS += pairpotentialwidget.h paletteblock.h processingtab.h
noinst_HEADERS += speciestab.h
noinst_HEADERS += wizardwidgetfooter.h wizardwidgetheader.h wizardwidgetpageinfo.h workspacetab.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
