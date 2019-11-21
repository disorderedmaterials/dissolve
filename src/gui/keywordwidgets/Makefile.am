noinst_LTLIBRARIES = libkeywordwidgets.la

SUFFIXES: .ui .hui .qrc

.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/keywordwidgets/$*.h
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
	-rm -f ui_* *.o moc_*.cpp bool.cpp charstring.cpp double.cpp enumoptions.cpp int.cpp species.cpp windowfunction.cpp

libkeywordwidgets_la_SOURCES = base.cpp
libkeywordwidgets_la_SOURCES += dropdown.ui dropwidget.hui dropdown_funcs.cpp dropwidget_funcs.cpp

libkeywordwidgets_la_SOURCES += atomtypeselection.ui atomtypeselection_funcs.cpp
libkeywordwidgets_la_SOURCES += bool.hui bool_funcs.cpp
libkeywordwidgets_la_SOURCES += broadeningfunction.ui broadeningfunction_funcs.cpp
libkeywordwidgets_la_SOURCES += charstring.hui charstring_funcs.cpp
libkeywordwidgets_la_SOURCES += dialog.ui dialog_funcs.cpp double.hui double_funcs.cpp
libkeywordwidgets_la_SOURCES += enumoptions.hui enumoptions_funcs.cpp expressionvariablelist.ui expressionvariablelist_funcs.cpp
libkeywordwidgets_la_SOURCES += fileandformat.ui fileandformat_funcs.cpp
libkeywordwidgets_la_SOURCES += integer.hui integer_funcs.cpp
libkeywordwidgets_la_SOURCES += isotopologuereferencelist.ui isotopologuereferencelist_funcs.cpp
libkeywordwidgets_la_SOURCES += module.ui module_funcs.cpp modulegroups.ui modulegroups_funcs.cpp modulereflist.ui modulereflist_funcs.cpp
libkeywordwidgets_la_SOURCES += node.ui node_funcs.cpp nodeandinteger.ui nodeandinteger_funcs.cpp nodereflist.ui nodereflist_funcs.cpp nodevalue.ui nodevalue_funcs.cpp nodevalueenumoptions.ui nodevalueenumoptions_funcs.cpp
libkeywordwidgets_la_SOURCES += pairbroadeningfunction.ui pairbroadeningfunction_funcs.cpp
libkeywordwidgets_la_SOURCES += range.ui range_funcs.cpp
libkeywordwidgets_la_SOURCES += species.hui species_funcs.cpp speciesreflist.ui speciesreflist_funcs.cpp speciessitereflist.ui speciessitereflist_funcs.cpp
libkeywordwidgets_la_SOURCES += vec3double.ui vec3double_funcs.cpp vec3integer.ui vec3integer_funcs.cpp vec3labels.cpp vec3nodevalue.ui vec3nodevalue_funcs.cpp
libkeywordwidgets_la_SOURCES += widget.hui widget_funcs.cpp windowfunction.hui windowfunction_funcs.cpp

noinst_HEADERS = atomtypeselection.h
noinst_HEADERS += base.h broadeningfunction.h
noinst_HEADERS += dialog.h dropdown.h
noinst_HEADERS += expressionvariablelist.h
noinst_HEADERS += fileandformat.h
noinst_HEADERS += isotopologuereferencelist.h
noinst_HEADERS += module.h modulegroups.h modulereflist.h
noinst_HEADERS += node.h nodeandinteger.h nodereflist.h nodevalue.h nodevalueenumoptions.h
noinst_HEADERS += pairbroadeningfunction.h
noinst_HEADERS += range.h
noinst_HEADERS += speciesreflist.h speciessitereflist.h
noinst_HEADERS += vec3double.h vec3integer.h vec3labels.h vec3nodevalue.h
noinst_HEADERS += widgets.h

AM_CPPFLAGS = -I$(top_srcdir)/src @DISSOLVE_CFLAGS@ @GUI_CFLAGS@ @GUI_INCLUDES@
