noinst_LTLIBRARIES = libgui.la

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/uchroma/gui/$*.h
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
	-rm -f ui_* moc_* *.o icons.cpp paneorganiser.cpp viewer.cpp 

libgui_la_SOURCES = uchroma.qrc

libgui_la_SOURCES += editdataset.ui editfitkernel.ui editfitresults.ui editlinestyle.ui editnumberformat.ui editreference.ui editviewlayout.ui
libgui_la_SOURCES += selectequation.ui selectsymbol.ui selecttarget.ui

libgui_la_SOURCES += axes.ui create.ui data.ui import.ui saveimage.ui style.ui transform.ui view.ui
libgui_la_SOURCES += operate_bgsub.ui operate_setz.ui
libgui_la_SOURCES += uchromafull.ui uchromaview.ui

libgui_la_SOURCES += axes_funcs.cpp create_funcs.cpp data_funcs.cpp import_funcs.cpp import_seqxy.cpp saveimage_funcs.cpp style_funcs.cpp transform_funcs.cpp view_funcs.cpp
libgui_la_SOURCES += editdataset_funcs.cpp editfitkernel_funcs.cpp editfitresults_funcs.cpp editlinestyle_funcs.cpp editnumberformat_funcs.cpp editreference_funcs.cpp editviewlayout_funcs.cpp
libgui_la_SOURCES += operate_bgsub_funcs.cpp operate_setz_funcs.cpp
libgui_la_SOURCES += selectequation_funcs.cpp selectsymbol_funcs.cpp selecttarget_funcs.cpp
libgui_la_SOURCES += uchromafull_collection.cpp uchromafull_funcs.cpp uchromafull_slots.cpp uchromafull_update.cpp 
libgui_la_SOURCES += uchromaview_funcs.cpp uchromaview_slots.cpp uchromaview_update.cpp 

libgui_la_SOURCES += paneorganiser.hui paneorganiser_funcs.cpp

libgui_la_SOURCES += viewer.hui viewer_context.cpp viewer_funcs.cpp viewer_input.cpp viewer_scene.cpp

noinst_HEADERS = axes.h create.h data.h import.h saveimage.h style.h transform.h view.h
noinst_HEADERS += editdataset.h editfitkernel.h editfitresults.h editlinestyle.h editnumberformat.h editreference.h editviewlayout.h
noinst_HEADERS += operate_bgsub.h operate_setz.h
noinst_HEADERS += selectequation.h selectsymbol.h selecttarget.h
noinst_HEADERS += uchromafull.h uchromaview.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I../ -I./ @DUQ_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@

