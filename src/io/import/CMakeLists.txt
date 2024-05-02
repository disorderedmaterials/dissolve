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
  cif.cpp
  cifClasses.cpp
  coordinates.cpp
  coordinates_dlpoly.cpp
  coordinates_epsr.cpp
  coordinates_moscito.cpp
  coordinates_xyz.cpp
  data1D.cpp
  data1D_histo.cpp
  data1D_mint.cpp
  data1D_xy.cpp
  data2D.cpp
  data2D_cartesian.cpp
  data3D.cpp
  data3D_pdens.cpp
  forces.cpp
  forces_dlpoly.cpp
  forces_moscito.cpp
  forces_simple.cpp
  species.cpp
  species_xyz.cpp
  trajectory.cpp
  trajectory_dlpoly.cpp
  values.cpp
  cif.h
  cifClasses.h
  coordinates.h
  data1D.h
  data2D.h
  data3D.h
  forces.h
  species.h
  trajectory.h
  values.h
  CIFImportErrorListeners.cpp
  CIFImportVisitor.cpp
  ${ANTLR_CIFImportGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_CIFImportGrammarParser_CXX_OUTPUTS}
)

target_include_directories(
  import PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${ANTLR_CIFImportGrammarLexer_OUTPUT_DIR}
                 ${ANTLR_CIFImportGrammarParser_OUTPUT_DIR}
)

target_link_libraries(import base)
