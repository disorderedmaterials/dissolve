# TextPrimitive ANTLR Lexer/Parser
antlr_target(TextPrimitiveGrammarLexer TextPrimitiveLexer.g4 LEXER)
# PACKAGE TextPrimitiveGrammar)
antlr_target(
  TextPrimitiveGrammarParser
  TextPrimitiveParser.g4
  PARSER
  # PACKAGE TextPrimitiveGrammar
  DEPENDS_ANTLR
  TextPrimitiveGrammarLexer
  COMPILE_FLAGS
  -no-listener
  -visitor
  -lib
  ${ANTLR_TextPrimitiveGrammarLexer_OUTPUT_DIR}
)

# Append path to ANTLR parser output, and set cache variable
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_TextPrimitiveGrammarLexer_OUTPUT_DIR})
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_TextPrimitiveGrammarParser_OUTPUT_DIR})
set(ANTLR_OUTPUT_DIRS
    ${ANTLR_OUTPUT_DIRS}
    CACHE INTERNAL ""
)

# Target 'render'
add_library(
  render
  axes.h
  colourDefinition.h
  colourScaleDelta.h
  colourScale.h
  colourScalePoint.h
  fontInstance.h
  lineStipple.h
  lineStyle.h
  numberFormat.h
  primitiveAssembly.h
  primitive.h
  primitiveInfo.h
  primitiveInstance.h
  primitiveList.h
  renderable.h
  renderableConfiguration.h
  renderableData1D.h
  renderableData2D.h
  renderableData3D.h
  renderableGroup.h
  renderableGroupManager.h
  renderableSpecies.h
  renderableSpeciesSite.h
  symbol.h
  textFormat.h
  textFragment.h
  textPrimitive.h
  textPrimitiveList.h
  vertexChunk.h
  view.h
  TextPrimitiveErrorListeners.h
  TextPrimitiveVisitor.h
  axes.cpp
  colourDefinition.cpp
  colourScale.cpp
  colourScaleDelta.cpp
  colourScalePoint.cpp
  fontInstance.cpp
  lineStipple.cpp
  lineStyle.cpp
  numberFormat.cpp
  primitiveAssembly.cpp
  primitive.cpp
  primitiveInfo.cpp
  primitiveInstance.cpp
  primitiveList.cpp
  renderable.cpp
  renderableConfiguration.cpp
  renderableData1D.cpp
  renderableData2D.cpp
  renderableData3D.cpp
  renderableGroup.cpp
  renderableGroupManager.cpp
  renderableSpecies.cpp
  renderableSpeciesSite.cpp
  symbol.cpp
  textFormat.cpp
  textFragment.cpp
  textPrimitive.cpp
  textPrimitiveList.cpp
  vertexChunk.cpp
  view.cpp
  TextPrimitiveErrorListeners.cpp
  TextPrimitiveVisitor.cpp
  ${ANTLR_TextPrimitiveGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_TextPrimitiveGrammarParser_CXX_OUTPUTS}
)

target_include_directories(
  render PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${Qt6Widgets_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
                 ${ANTLR_TextPrimitiveGrammarLexer_OUTPUT_DIR} ${ANTLR_TextPrimitiveGrammarParser_OUTPUT_DIR}
)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(render PUBLIC /Zc:__cplusplus)
endif(MSVC)

target_link_libraries(render PRIVATE base)
