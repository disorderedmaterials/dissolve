noinst_LTLIBRARIES = libgui.la

SUBDIRS = delegates helpers keywordwidgets widgets viewer charts

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
libgui_la_SOURCES += layereditor.ui
libgui_la_SOURCES += modulechartinsertionblock.ui modulechartmoduleblock.ui modulecontrolwidget.ui
libgui_la_SOURCES += modulechart.hui
# UI - Editors
libgui_la_SOURCES += procedureeditor.ui
# UI - Widgets
libgui_la_SOURCES += keywordswidget.hui
libgui_la_SOURCES += selectforcefieldwidget.ui 
libgui_la_SOURCES += wizardwidgetfooter.ui wizardwidgetheader.ui wizardwidget.hui
# UI - Wizards
libgui_la_SOURCES += addconfigurationwizard.ui addforcefieldtermswizard.ui addprocessinglayerwizard.ui
libgui_la_SOURCES += importspecieswizard.ui
libgui_la_SOURCES += pairpotentialwidget.ui
# UI - Dialogs
libgui_la_SOURCES += addconfigurationdialog.ui addforcefieldtermsdialog.ui addprocessinglayerdialog.ui
libgui_la_SOURCES += getconfigurationnamedialog.ui getspeciesnamedialog.ui
libgui_la_SOURCES += importspeciesdialog.ui
libgui_la_SOURCES += selectelementdialog.ui selectforcefielddialog.ui selectspeciesdialog.ui selectspecieswidget.ui selectsystemtemplatedialog.ui
# UI - Main Tabs
libgui_la_SOURCES += gui.ui
libgui_la_SOURCES += configurationtab.ui
libgui_la_SOURCES += forcefieldtab.ui
libgui_la_SOURCES += modulelayertab.ui moduletab.ui
libgui_la_SOURCES += speciestab.ui
libgui_la_SOURCES += workspacetab.ui
# UI - Utility Classes
libgui_la_SOURCES += outputhandler.hui thread.hui

# Implementations
libgui_la_SOURCES += addconfigurationdialog_funcs.cpp addconfigurationwizard_funcs.cpp addforcefieldtermsdialog_funcs.cpp addforcefieldtermswizard_funcs.cpp addprocessinglayerdialog_funcs.cpp addprocessinglayerwizard_funcs.cpp
libgui_la_SOURCES += configurationtab_funcs.cpp
libgui_la_SOURCES += forcefieldtab_funcs.cpp
libgui_la_SOURCES += getconfigurationnamedialog_funcs.cpp getspeciesnamedialog_funcs.cpp
libgui_la_SOURCES += gui_control.cpp gui_funcs.cpp gui_io.cpp gui_start.cpp gui_tabs.cpp
libgui_la_SOURCES += importspeciesdialog_funcs.cpp importspecieswizard_funcs.cpp
libgui_la_SOURCES += keywordswidget_funcs.cpp
libgui_la_SOURCES += layereditor_funcs.cpp
libgui_la_SOURCES += maintab.cpp
libgui_la_SOURCES += menu_configuration.cpp menu_file.cpp menu_help.cpp menu_layer.cpp menu_simulation.cpp menu_species.cpp menu_workspace.cpp
libgui_la_SOURCES += modulechart_funcs.cpp modulechartblock.cpp modulecharthotspot.cpp modulechartinsertionblock_funcs.cpp modulechartmoduleblock_funcs.cpp
libgui_la_SOURCES += modulecontrolwidget_funcs.cpp modulelayertab_funcs.cpp moduletab_funcs.cpp modulewidget.cpp
libgui_la_SOURCES += outputhandler_funcs.cpp
libgui_la_SOURCES += pairpotentialwidget_funcs.cpp procedureeditor_funcs.cpp
libgui_la_SOURCES += selectelementdialog_funcs.cpp selectforcefielddialog_funcs.cpp selectforcefieldwidget_funcs.cpp selectspeciesdialog_funcs.cpp selectspeciesdialog_funcs.cpp selectsystemtemplatedialog_funcs.cpp speciestab_funcs.cpp stockcolours.cpp systemtemplate.cpp
libgui_la_SOURCES += thread_funcs.cpp
libgui_la_SOURCES += tmdiarea.hui tmdiarea_funcs.cpp
libgui_la_SOURCES += wizardwidget_funcs.cpp wizardwidgethighlight.cpp wizardwidgetpageinfo.cpp workspacetab_funcs.cpp

noinst_HEADERS = addconfigurationdialog.h addconfigurationwizard.h addforcefieldtermsdialog.h addforcefieldtermswizard.h addprocessinglayerdialog.h addprocessinglayerwizard.h
noinst_HEADERS += configurationtab.h
noinst_HEADERS += forcefieldtab.h
noinst_HEADERS += getconfigurationnamedialog.h getspeciesnamedialog.h gui.h
noinst_HEADERS += importspeciesdialog.h importspecieswizard.h
noinst_HEADERS += keywordwidgets.h
noinst_HEADERS += layereditor.h
noinst_HEADERS += maintab.h modulechartblock.h modulecharthotspot.h modulechartinsertionblock.h modulechartmetrics.h modulechartmoduleblock.h modulecontrolwidget.h modulelayertab.h moduletab.h modulewidget.h
noinst_HEADERS += pairpotentialwidget.h
noinst_HEADERS += selectelementdialog.h selectforcefielddialog.h selectforcefieldwidget.h selectspeciesdialog.h selectspecieswidget.h selectsystemtemplatedialog.h speciestab.h stockcolours.h systemtemplate.h
noinst_HEADERS += wizardwidgetfooter.h wizardwidgetheader.h wizardwidgethighlight.h wizardwidgetpageinfo.h workspacetab.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
