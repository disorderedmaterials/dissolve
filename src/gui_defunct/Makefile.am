bin_PROGRAMS = duqgui

noinst_LIBRARIES = libgui.a

SUFFIXES: .ui .uih .qrc

.ui.o:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/$*.h
	$(CXX) -I../ -I../lib $(CXXFLAGS) $(QTGUI_CFLAGS) -c -o $@ moc_$*.cpp
	rm moc_$*.cpp

.uih.o:
	$(QTMOC) -o $*.cpp -f$< $<
	$(CXX) -I$(top_srcdir)/src/lib -I../ -I./ $(CXXFLAGS) $(QTGUI_CFLAGS) -c -o $@ $*.cpp
	rm $*.cpp

.qrc.o:
	$(QTRCC) -o $*.cpp -name $* $<
	$(CXX) -I$(top_srcdir)/src/lib -I../ -I./ $(CXXFLAGS) $(QTGUI_CFLAGS) -c -o $@ $*.cpp
	rm $*.cpp

clean:
	rm -f *.o ui_* guiduq.cpp plotwidget.cpp viewer.cpp comboboxdelegate.cpp doublespindelegate.cpp

libgui_a_SOURCES = comboboxdelegate.uih comboboxdelegate_funcs.cpp doublespindelegate.uih doublespindelegate_funcs.cpp icons.qrc integerspindelegate.uih integerspindelegate_funcs.cpp 
libgui_a_SOURCES += atomtypes.ui element.ui fq.ui samples.ui species.ui sq.ui system.ui mainwindow.ui
libgui_a_SOURCES += mainwindow_funcs.cpp mainwindow_menus.cpp mainwindow_setup.cpp
libgui_a_SOURCES += atomtypes_funcs.cpp element_funcs.cpp fq_funcs.cpp samples_funcs.cpp species_funcs.cpp sq_funcs.cpp system_funcs.cpp
libgui_a_SOURCES += plotwidget.uih plotwidget_data.cpp plotwidget_funcs.cpp plotwidget_paintevent.cpp
libgui_a_SOURCES += viewer.uih viewer_engine.cpp viewer_funcs.cpp viewer_input.cpp viewer_prefs.cpp viewer_primitive.cpp viewer_primitiveinfo.cpp viewer_primitivevertex.cpp viewer_textprimitive.cpp viewer_trianglechopper.cpp viewer_vertexchunk.cpp
libgui_a_SOURCES += speciesviewer.uih speciesviewer_funcs.cpp speciesviewer_input.cpp speciesviewer_scene.cpp speciesviewer_funcs.cpp
libgui_a_SOURCES += configviewer.uih configviewer_funcs.cpp configviewer_input.cpp configviewer_scene.cpp configviewer_funcs.cpp
libgui_a_SOURCES += guiduq.uih guiduq_funcs.cpp

noinst_HEADERS = mainwindow.h fq.h sq.h samples.h system.h element.h pointerdialog.h
noinst_HEADERS += viewer_primitive.h viewer_primitiveinfo.h viewer_textprimitive.h viewer_trianglechopper.h viewer_vertexchunk.h

# Target 'duq'
duqgui_SOURCES = main.cpp ../lib/base/mutex.cpp

duqgui_LDADD = libgui.a ../lib/main/libmain.a ../lib/classes/libclasses.a ../lib/base/libbase.a ../lib/math/libmath.a @MPI_LDLIBS@ @GUI_LDLIBS@ -lreadline

AM_CPPFLAGS = -I../ -I../lib @DUQ_CFLAGS@ AM_CPPFLAGS@ -DWITHGUI

