noinst_LTLIBRARIES = libwidgets.la

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(builddir)/$*.h
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
	-rm -f ui_* *.o moc_*.cpp colourbutton.cpp elementselector.cpp exponentialspin.cpp gradientbar.cpp mimetreewidget.cpp superstackedwidget.cpp tmenubutton.cpp tpopupwidget.cpp

libwidgets_la_SOURCES = nocontrols.ui
libwidgets_la_SOURCES += checkindicator.hui colourbutton.hui elementselector.hui exponentialspin.hui gradientbar.hui mimetreewidget.hui superstackedwidget.hui tmenubutton.hui tpopupwidget.hui

libwidgets_la_SOURCES += checkindicator_funcs.cpp
libwidgets_la_SOURCES += colourbutton_funcs.cpp
libwidgets_la_SOURCES += elementselector_funcs.cpp exponentialspin_funcs.cpp
libwidgets_la_SOURCES += gradientbar_funcs.cpp
libwidgets_la_SOURCES += mimestrings.cpp mimetreewidget_funcs.cpp mimetreewidgetitem.cpp
libwidgets_la_SOURCES += nocontrols_funcs.cpp
libwidgets_la_SOURCES += sampleddoublelabel.ui sampleddoublelabel_funcs.cpp superstackedwidget_funcs.cpp
libwidgets_la_SOURCES += tmenubutton_funcs.cpp tmenubuttongroup.cpp tpopupwidget_funcs.cpp

noinst_HEADERS = mimestrings.h mimetreewidgetitem.h
noinst_HEADERS += nocontrols.h
noinst_HEADERS += sampleddoublelabel.h
noinst_HEADERS += tmenubuttongroup.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
