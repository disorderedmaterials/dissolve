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
libgui_la_SOURCES += selectconfigurationwidget.ui selectforcefieldwidget.ui selectspecieswidget.ui
libgui_la_SOURCES += wizardwidgetfooter.ui wizardwidgetheader.ui wizardwidget.hui
# UI - Wizards
libgui_la_SOURCES += addconfigurationwizard.ui addforcefieldtermswizard.ui
libgui_la_SOURCES += importspecieswizard.ui
libgui_la_SOURCES += pairpotentialwidget.ui
# UI - Dialogs
libgui_la_SOURCES += addconfigurationdialog.ui addforcefieldtermsdialog.ui
libgui_la_SOURCES += getconfigurationnamedialog.ui getmodulelayernamedialog.ui getspeciesnamedialog.ui
libgui_la_SOURCES += importspeciesdialog.ui
libgui_la_SOURCES += selectconfigurationdialog.ui selectelementdialog.ui selectforcefielddialog.ui selectspeciesdialog.ui selectsystemtemplatedialog.ui
# UI - Main Tabs
libgui_la_SOURCES += gui.ui
libgui_la_SOURCES += configurationtab.ui
libgui_la_SOURCES += forcefieldtab.ui
libgui_la_SOURCES += layertab.ui
libgui_la_SOURCES += moduletab.ui
libgui_la_SOURCES += speciestab.ui
libgui_la_SOURCES += workspacetab.ui
# UI - Utility Classes
libgui_la_SOURCES += outputhandler.hui thread.hui

# Implementations
libgui_la_SOURCES += addconfigurationdialog_funcs.cpp addconfigurationwizard_funcs.cpp addforcefieldtermsdialog_funcs.cpp addforcefieldtermswizard_funcs.cpp
libgui_la_SOURCES += configurationtab_funcs.cpp
libgui_la_SOURCES += forcefieldtab_funcs.cpp
libgui_la_SOURCES += getconfigurationnamedialog_funcs.cpp getmodulelayernamedialog_funcs.cpp getspeciesnamedialog_funcs.cpp
libgui_la_SOURCES += gui_control.cpp gui_funcs.cpp gui_io.cpp gui_start.cpp gui_tabs.cpp
libgui_la_SOURCES += importspeciesdialog_funcs.cpp importspecieswizard_funcs.cpp
libgui_la_SOURCES += layereditor_funcs.cpp layertab_funcs.cpp
libgui_la_SOURCES += maintab.cpp
libgui_la_SOURCES += menu_configuration.cpp menu_file.cpp menu_help.cpp menu_layer.cpp menu_simulation.cpp menu_species.cpp menu_workspace.cpp
libgui_la_SOURCES += modulechart_funcs.cpp modulechartblock.cpp modulecharthotspot.cpp modulechartinsertionblock_funcs.cpp modulechartmoduleblock_funcs.cpp
libgui_la_SOURCES += modulecontrolwidget_funcs.cpp moduletab_funcs.cpp modulewidget.cpp
libgui_la_SOURCES += outputhandler_funcs.cpp
libgui_la_SOURCES += pairpotentialwidget_funcs.cpp procedureeditor_funcs.cpp
libgui_la_SOURCES += selectconfigurationdialog_funcs.cpp selectconfigurationwidget_funcs.cpp selectelementdialog_funcs.cpp selectforcefielddialog_funcs.cpp selectforcefieldwidget_funcs.cpp selectspeciesdialog_funcs.cpp selectspecieswidget_funcs.cpp selectsystemtemplatedialog_funcs.cpp speciestab_funcs.cpp stockcolours.cpp systemtemplate.cpp
libgui_la_SOURCES += thread_funcs.cpp
libgui_la_SOURCES += tmdiarea.hui tmdiarea_funcs.cpp
libgui_la_SOURCES += wizardwidget_funcs.cpp wizardwidgethighlight.cpp wizardwidgetpageinfo.cpp workspacetab_funcs.cpp

noinst_HEADERS = addconfigurationdialog.h addconfigurationwizard.h addforcefieldtermsdialog.h addforcefieldtermswizard.h
noinst_HEADERS += configurationtab.h
noinst_HEADERS += forcefieldtab.h
noinst_HEADERS += getconfigurationnamedialog.h getmodulelayernamedialog.h getspeciesnamedialog.h gui.h
noinst_HEADERS += importspeciesdialog.h importspecieswizard.h
noinst_HEADERS += layereditor.h layertab.h
noinst_HEADERS += maintab.h modulechartblock.h modulecharthotspot.h modulechartinsertionblock.h modulechartmetrics.h modulechartmoduleblock.h modulecontrolwidget.h moduletab.h modulewidget.h
noinst_HEADERS += pairpotentialwidget.h procedureeditor.h
noinst_HEADERS += selectconfigurationdialog.h selectconfigurationwidget.h selectelementdialog.h selectforcefielddialog.h selectforcefieldwidget.h selectspeciesdialog.h selectspecieswidget.h selectsystemtemplatedialog.h speciestab.h stockcolours.h systemtemplate.h
noinst_HEADERS += wizardwidgetfooter.h wizardwidgetheader.h wizardwidgethighlight.h wizardwidgetpageinfo.h workspacetab.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
