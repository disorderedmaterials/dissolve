# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!

# Meta-Objects
SET(uchromagui_MOC_HDRS
  paneorganiser.hui
  viewer.hui

  editdataset.h
  editfitkernel.h
  editfitresults.h
  editlinestyle.h
  editnumberformat.h
  editreference.h
  editviewlayout.h

  operate_setz.h
  operate_bgsub.h

  selectequation.h
  selectsymbol.h
  selecttarget.h

  axes.h
  create.h
  data.h
  import.h
  saveimage.h
  style.h
  transform.h
  view.h

  uchromafull.h
  uchromaview.h
)
QT5_WRAP_CPP(uchromagui_MOC_SRCS ${uchromagui_MOC_HDRS})

# User Interface Files
SET(uchromagui_UIS
  editdataset.ui
  editfitkernel.ui
  editfitresults.ui
  editlinestyle.ui
  editnumberformat.ui
  editreference.ui
  editviewlayout.ui

  operate_bgsub.ui
  operate_setz.ui

  selectequation.ui
  selectsymbol.ui
  selecttarget.ui

  axes.ui
  create.ui
  data.ui
  import.ui
  saveimage.ui
  style.ui
  transform.ui
  view.ui

  uchromafull.ui
  uchromaview.ui
)
QT5_WRAP_UI(uchromagui_UIS_H ${uchromagui_UIS})

# Source files (not Qt UI files or custom .h [uih])
SET(uchromagui_SRCS
  editdataset_funcs.cpp
  editfitkernel_funcs.cpp
  editfitresults_funcs.cpp
  editlinestyle_funcs.cpp
  editnumberformat_funcs.cpp
  editreference_funcs.cpp
  editviewlayout_funcs.cpp

  operate_setz_funcs.cpp
  operate_bgsub_funcs.cpp

  paneorganiser_funcs.cpp

  selectequation_funcs.cpp
  selectsymbol_funcs.cpp
  selecttarget_funcs.cpp

  viewer_context.cpp
  viewer_funcs.cpp
  viewer_input.cpp
  viewer_scene.cpp

  axes_funcs.cpp
  create_funcs.cpp
  data_funcs.cpp
  import_funcs.cpp
  import_seqxy.cpp
  saveimage_funcs.cpp
  style_funcs.cpp
  transform_funcs.cpp
  view_funcs.cpp

  uchromafull_collection.cpp
  uchromafull_funcs.cpp
  uchromafull_slots.cpp
  uchromafull_update.cpp
  uchromaview_funcs.cpp
  uchromaview_slots.cpp
  uchromaview_update.cpp
)

# Resources
set(uchromagui_RES_QRC
  uchroma.qrc 
)
QT5_ADD_RESOURCES(uchromagui_RES ${uchromagui_RES_QRC})

# Target 'gui'
add_library(uchromagui
  ${uchromagui_RES}
  ${uchromagui_UIS_H}
  ${uchromagui_SRCS}
  ${uchromagui_MOC_SRCS}
)
target_include_directories(uchromagui PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

