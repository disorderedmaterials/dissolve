# CIFImport ANTLR Lexer/Parser
antlr_target(CIFImportGrammarLexer CIFImportLexer.g4 LEXER)
# PACKAGE CIFImportGrammar)
antlr_target(
  CIFImportGrammarParser
  CIFImportParser.g4
  PARSER
  # PACKAGE CIFImportGrammar
  DEPENDS_ANTLR
  CIFImportGrammarLexer
  COMPILE_FLAGS
  -no-listener
  -visitor
  -lib
  ${ANTLR_CIFImportGrammarLexer_OUTPUT_DIR}
)

# Append path to ANTLR parser output, and set cache variable
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_CIFImportGrammarLexer_OUTPUT_DIR})
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_CIFImportGrammarParser_OUTPUT_DIR})
set(ANTLR_OUTPUT_DIRS
    ${ANTLR_OUTPUT_DIRS}
    CACHE INTERNAL ""
)

add_library(
  import
  coordinates.cpp
  coordinates_dlpoly.cpp
  coordinates_epsr.cpp
  coordinates_moscito.cpp
  coordinates_xyz.cpp
  data1d.cpp
  data1d_histo.cpp
  data1d_mint.cpp
  data1d_xy.cpp
  data2d.cpp
  data2d_cartesian.cpp
  data3d.cpp
  forces.cpp
  forces_dlpoly.cpp
  forces_moscito.cpp
  forces_simple.cpp
  trajectory.cpp
  trajectory_dlpoly.cpp
  values.cpp
  coordinates.h
  data1d.h
  data2d.h
  data3d.h
  forces.h
  trajectory.h
  values.h
  CIFImportErrorListeners.cpp
  CIFImportVisitor.cpp
  ${ANTLR_CIFImportGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_CIFImportGrammarParser_CXX_OUTPUTS}
)

include_directories(
  import
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${ANTLRRUNTIME_INCLUDE_DIRS}
  ${ANTLR_CIFImportGrammarLexer_OUTPUT_DIR}
  ${ANTLR_CIFImportGrammarParser_OUTPUT_DIR}
)
target_link_libraries(import PRIVATE base)
