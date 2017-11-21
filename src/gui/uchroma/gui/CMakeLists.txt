# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(gui_MOC_HDRS
  viewer.hui

  editdataset.h
  editfitkernel.h
  editfitresults.h
  editlinestyle.h
  editnumberformat.h
  editreference.h

  operate_setz.h
  operate_bgsub.h

  selectequation.h
  selectsymbol.h
  selecttarget.h

  axes.h
  create.h
  data.h
  import.h
  log.h
  saveimage.h
  style.h
  view.h
)
QT5_WRAP_CPP(gui_MOC_SRCS ${gui_MOC_HDRS})

# User Interface Files
SET(gui_UIS
  editdataset.ui
  editfitkernel.ui
  editfitresults.ui
  editlinestyle.ui
  editnumberformat.ui
  editreference.ui

  operate_bgsub.ui
  operate_setz.ui

  selectequation.ui
  selectsymbol.ui
  selecttarget.ui

  axes.ui
  create.ui
  data.ui
  import.ui
  log.ui
  saveimage.ui
  style.ui
  view.ui
)
QT5_WRAP_UI(gui_UIS_H ${gui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(gui_SRCS
  editdataset_funcs.cpp
  editfitkernel_funcs.cpp
  editfitresults_funcs.cpp
  editlinestyle_funcs.cpp
  editnumberformat_funcs.cpp
  editreference_funcs.cpp

  operate_setz_funcs.cpp
  operate_bgsub_funcs.cpp

  selectequation_funcs.cpp
  selectsymbol_funcs.cpp
  selecttarget_funcs.cpp

  viewer_funcs.cpp
  viewer_input.cpp
  viewer_scene.cpp

  axes_funcs.cpp
  create_funcs.cpp
  data_funcs.cpp
  import_funcs.cpp
  import_seqxy.cpp
  log_funcs.cpp
  saveimage_funcs.cpp
  style_funcs.cpp
  view_funcs.cpp
)

# Resources
set(gui_RES_QRC
  uchroma.qrc 
)
QT5_ADD_RESOURCES(gui_RES ${gui_RES_QRC})

# Target 'gui'
add_library(gui
${gui_RES}
${gui_UIS_H}
${gui_SRCS}
${gui_MOC_SRCS}
)
target_include_directories(gui PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)
