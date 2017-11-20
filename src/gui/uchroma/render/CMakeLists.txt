bison_target(TextPrimitiveParser textprimitive_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/textprimitive_grammar.cc COMPILE_FLAGS -y )

add_library(render
  ${BISON_TextPrimitiveParser_OUTPUTS}
  fontinstance.cpp
  linestipple.cpp
  linestyle.cpp
  primitive.cpp
  primitiveinfo.cpp
  primitiveinstance.cpp
  primitivelist.cpp
  surface.cpp
  surface_grid.cpp
  surface_linexy.cpp
  surface_linezy.cpp
  surface_full.cpp
  textformat.cpp
  textfragment.cpp
  textprimitive.cpp
  textprimitivelist.cpp
  fontinstance.h
  linestipple.h
  linestyle.h
  primitive.h
  primitiveinfo.h
  primitiveinstance.h
  primitivelist.h
  surface.h
  textformat.h
  textfragment.h
  textprimitive.h
  textprimitivelist.h
)

target_include_directories(render PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)
