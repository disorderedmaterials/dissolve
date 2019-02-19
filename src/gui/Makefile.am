noinst_LTLIBRARIES = libgui.la libdguided.la

SUBDIRS = delegates keywordwidgets viewer widgets

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
	-rm -f ui_* *.o moc_*.cpp main.cpp tmdiarea.cpp wizardwidget.cpp

libgui_la_SOURCES = main.qrc

# UI - Module Chart
libgui_la_SOURCES += modulechartinsertionblock.ui modulechartmoduleblock.ui modulecontrolwidget.ui moduleeditor.ui
libgui_la_SOURCES += modulechart.hui
# UI - Widgets
libgui_la_SOURCES += wizardwidgetfooter.ui wizardwidgetheader.ui wizardwidget.hui
libgui_la_SOURCES += addconfigurationwizard.ui addprocessinglayerwizard.ui addspecieswizard.ui guidewizard.ui pairpotentialwidget.ui
# UI - Dialogs
libgui_la_SOURCES += addconfigurationdialog.ui addprocessinglayerdialog.ui addspeciesdialog.ui
# UI - Main Tabs
libgui_la_SOURCES += gui.ui configurationtab.ui forcefieldtab.ui modulelayertab.ui moduletab.ui speciestab.ui workspacetab.ui
# UI - Utility Classes
libgui_la_SOURCES += outputhandler.hui thread.hui

# Implementations
libgui_la_SOURCES += addconfigurationdialog_funcs.cpp addconfigurationwizard_funcs.cpp addprocessinglayerdialog_funcs.cpp addprocessinglayerwizard_funcs.cpp addspeciesdialog_funcs.cpp addspecieswizard_funcs.cpp
libgui_la_SOURCES += configurationtab_funcs.cpp
libgui_la_SOURCES += forcefieldtab_funcs.cpp
libgui_la_SOURCES += guidewizard_funcs.cpp gui_funcs.cpp gui_menu.cpp gui_simulation_control.cpp gui_simulation_guide.cpp gui_simulation_state.cpp gui_simulation_tabs.cpp gui_stack.cpp gui_start.cpp guide.cpp guidepage.cpp guidepagehighlight.cpp
libgui_la_SOURCES += maintab.cpp
libgui_la_SOURCES += modulechart_funcs.cpp modulechartblock.cpp modulecharthotspot.cpp modulechartinsertionblock_funcs.cpp modulechartmoduleblock_funcs.cpp
libgui_la_SOURCES += modulecontrolwidget_funcs.cpp moduleeditor_funcs.cpp modulekeywordswidget_funcs.cpp modulelayertab_funcs.cpp moduletab_funcs.cpp modulewidget.cpp
libgui_la_SOURCES += outputhandler_funcs.cpp
libgui_la_SOURCES += pairpotentialwidget_funcs.cpp
libgui_la_SOURCES += speciestab_funcs.cpp
libgui_la_SOURCES += thread_funcs.cpp
libgui_la_SOURCES += tmdiarea.hui tmdiarea_funcs.cpp
libgui_la_SOURCES += wizardwidget_funcs.cpp wizardwidgethighlight.cpp wizardwidgetpageinfo.cpp workspacetab_funcs.cpp

# DGuidEd
libdguided_la_SOURCES = dguided.ui dguided_funcs.cpp

noinst_HEADERS = addconfigurationdialog.h addconfigurationwizard.h addprocessinglayerdialog.h addprocessinglayerwizard.h addspeciesdialog.h addspecieswizard.h
noinst_HEADERS += configurationtab.h
noinst_HEADERS += dguided.h
noinst_HEADERS += forcefieldtab.h
noinst_HEADERS += guidewizard.h gui.h guide.h guidepage.h guidepagehighlight.h
noinst_HEADERS += maintab.h modulechartblock.h modulecharthotspot.h modulechartinsertionblock.h modulechartmetrics.h modulechartmoduleblock.h modulecontrolwidget.h moduleeditor.h modulekeywordswidget.h modulelayertab.h moduletab.h modulewidget.h
noinst_HEADERS += pairpotentialwidget.h
noinst_HEADERS += speciestab.h
noinst_HEADERS += wizardwidgetfooter.h wizardwidgetheader.h wizardwidgethighlight.h wizardwidgetpageinfo.h workspacetab.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
