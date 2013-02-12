include_directories(
./
../
${CMAKE_CURRENT_BINARY_DIR} 
${CMAKE_CURRENT_BINARY_DIR}/../
${QT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/src/gui 
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src/
)

# Source files (not Qt UI files or custom .h [uih])
SET(qtproject_SRCS
  ../lib/base/mutex.cpp
  comboboxdelegate_funcs.cpp
  doublespindelegate_funcs.cpp
  integerspindelegate_funcs.cpp
  guiduq_funcs.cpp
  mainwindow_experiment.cpp
  mainwindow_funcs.cpp
  mainwindow_menus.cpp
  mainwindow_potential.cpp
  mainwindow_signals.cpp
  mainwindow_simulation.cpp
  mainwindow_species.cpp
  plotwidget_data.cpp
  plotwidget_funcs.cpp
  plotwidget_paintevent.cpp
  ptable_funcs.cpp
  viewer_engine.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_prefs.cpp
  viewer_primitive.cpp
  viewer_primitiveinfo.cpp
  viewer_primitivevertex.cpp
  viewer_scene.cpp
  viewer_textprimitive.cpp
  viewer_trianglechopper.cpp
  viewer_vertexchunk.cpp
  plotwidget.h
  simulationthread.h
  viewer_primitive.h
  viewer_primitiveinfo.h
  viewer_textprimitive.h
  viewer_trianglechopper.h
  viewer_vertexchunk.h
)

SET(pro_MOC_HDRS
  mainwindow.h
  comboboxdelegate.uih
  doublespindelegate.uih
  guiduq.uih
  integerspindelegate.uih
  plotwidget.uih
  ptable.h
  viewer.uih
)
QT4_WRAP_CPP(pro_MOC_SRCS ${pro_MOC_HDRS} OPTIONS -f)

SET(pro_UIS
  mainwindow.ui
  ptable.ui
)
QT4_WRAP_UI(pro_UIS_H ${pro_UIS})

set(pro_RES_QRC
  icons.qrc 
)
QT4_ADD_RESOURCES(pro_RES ${pro_RES_QRC})

add_library(gui
${pro_RES}
${pro_UIS_H}
${qtproject_SRCS}
${pro_MOC_SRCS}
)

