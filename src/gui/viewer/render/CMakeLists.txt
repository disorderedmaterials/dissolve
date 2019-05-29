bison_target(TextPrimitiveParser textprimitive_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/textprimitive_grammar.cc COMPILE_FLAGS -y )

# Source files (not Qt UI files or custom .h [uih])
SET(render_SRCS
  axes.h
  colourdefinition.h
  colourscaledelta.h
  colourscale.h
  colourscalepoint.h
  fontinstance.h
  linestipple.h
  linestyle.h
  numberformat.h
  primitiveassembly.h
  primitive.h
  primitiveinfo.h
  primitiveinstance.h
  primitivelist.h
  renderable.h
  renderableconfiguration.h
  renderabledata1d.h
  renderablefactory.h
  renderablegroup.h
  renderablegroupmanager.h
  renderablespecies.h
  symbol.h
  textformat.h
  textfragment.h
  textprimitive.h
  textprimitivelist.h
  vertexchunk.h
  view.h

  axes.cpp
  colourdefinition.cpp
  colourscale.cpp
  colourscaledelta.cpp
  colourscalepoint.cpp
  fontinstance.cpp
  linestipple.cpp
  linestyle.cpp
  numberformat.cpp
  primitiveassembly.cpp
  primitive.cpp
  primitiveinfo.cpp
  primitiveinstance.cpp
  primitivelist.cpp
  renderable.cpp
  renderableconfiguration.cpp
  renderabledata1d.cpp
  renderablefactory.cpp
  renderablegroup.cpp
  renderablegroupmanager.cpp
  renderablespecies.cpp
  symbol.cpp
  textformat.cpp
  textfragment.cpp
  textprimitive.cpp
  textprimitivelist.cpp
  vertexchunk.cpp
  view.cpp
  )

# Target 'render'
add_library(render
  ${BISON_TextPrimitiveParser_OUTPUTS}
  ${render_SRCS}
)
target_include_directories(render PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Widgets_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

