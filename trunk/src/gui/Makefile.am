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

libgui_a_SOURCES = comboboxdelegate.uih comboboxdelegate_funcs.cpp doublespindelegate.uih doublespindelegate_funcs.cpp guiduq.uih guiduq_funcs.cpp icons.qrc integerspindelegate.uih integerspindelegate_funcs.cpp ptable.ui ptable_funcs.cpp
libgui_a_SOURCES += mainwindow.ui mainwindow_experiment.cpp mainwindow_funcs.cpp mainwindow_menus.cpp mainwindow_potential.cpp mainwindow_signals.cpp mainwindow_simulation.cpp mainwindow_species.cpp
libgui_a_SOURCES += plotwidget.uih plotwidget_data.cpp plotwidget_funcs.cpp plotwidget_paintevent.cpp viewer.uih viewer_engine.cpp viewer_funcs.cpp viewer_input.cpp viewer_prefs.cpp viewer_primitive.cpp viewer_primitiveinfo.cpp viewer_primitivevertex.cpp viewer_scene.cpp viewer_textprimitive.cpp viewer_trianglechopper.cpp viewer_vertexchunk.cpp

noinst_HEADERS = mainwindow.h ptable.h pointerdialog.h viewer_primitive.h viewer_primitiveinfo.h viewer_textprimitive.h viewer_trianglechopper.h viewer_vertexchunk.h

# Target 'duq'
duqgui_SOURCES = main.cpp ../lib/base/mutex.cpp

duqgui_LDADD = libgui.a ../lib/main/libmain.a ../lib/classes/libclasses.a ../lib/base/libbase.a ../lib/math/libmath.a @MPI_LDLIBS@ @GUI_LDLIBS@ -lreadline

INCLUDES = -I../ -I../lib @DUQ_CFLAGS@ @GUI_INCLUDES@ -DWITHGUI

