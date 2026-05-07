file(
  GLOB_RECURSE node_sources
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./*.cpp"
)

file(
  GLOB_RECURSE node_headers
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./*.h"
)

# CIF/ANTLR
set(CIF_IO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cif/io")
set(ANTLR_CIF_PREFIX "NodeCIFImport")

# CIFImport ANTLR Lexer/Parser
antlr_target(CIFImportGrammarLexer "${CIF_IO_DIR}/CIFImportLexer.g4" LEXER)
# PACKAGE CIFImportGrammar)
antlr_target(
  CIFImportGrammarParser
  "${CIF_IO_DIR}/CIFImportParser.g4"
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

add_library(nodes ${node_sources} ${node_headers} ${ANTLR_CIFImportGrammarLexer_CXX_OUTPUTS} ${ANTLR_CIFImportGrammarParser_CXX_OUTPUTS})

message(
  STATUS
    "ANTLR generated products located in the following directories: ${ANTLR_CIFImportGrammarLexer_CXX_OUTPUTS}, ${ANTLR_CIFImportGrammarParser_CXX_OUTPUTS}"
)

target_include_directories(
  nodes PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${ANTLR_CIFImportGrammarLexer_OUTPUT_DIR}
                ${ANTLR_CIFImportGrammarParser_OUTPUT_DIR}
)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})
