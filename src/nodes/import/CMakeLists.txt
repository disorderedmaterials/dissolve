# cif ANTLR Lexer/Parser
antlr_target(CIFGrammarLexer CIFLexer.g4 LEXER)
# PACKAGE cifGrammar)
antlr_target(
  CIFGrammarParser
  CIFParser.g4
  PARSER
  # PACKAGE cifGrammar
  DEPENDS_ANTLR
  CIFGrammarLexer
  COMPILE_FLAGS
  -no-listener
  -visitor
  -lib
  ${ANTLR_CIFGrammarLexer_OUTPUT_DIR}
)

# Append path to ANTLR parser output, and set cache variable
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_CIFGrammarLexer_OUTPUT_DIR})
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_CIFGrammarParser_OUTPUT_DIR})
set(ANTLR_OUTPUT_DIRS
    ${ANTLR_OUTPUT_DIRS}
    CACHE INTERNAL ""
)

add_library(
  nodes_import
  ${ANTLR_CIFGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_CIFGrammarParser_CXX_OUTPUTS}
  cifClasses.cpp
  cifClasses.h
  cifErrorListeners.h
  cifStructure.cpp
  cifStructure.h
  cifErrorListeners.cpp
  cifVisitor.cpp
  cifVisitor.h
  dlpolyStructure.cpp
  dlpolyStructure.h
  dlpolyTrajectory.cpp
  dlpolyTrajectory.h
  dlputilsPDens.cpp
  dlputilsPDens.h
  dlputilsSurface.cpp
  dlputilsSurface.h
  epsrAtoStructure.cpp
  epsrAtoStructure.h
  moscitoStructure.cpp
  moscitoStructure.h
  species.cpp
  species.h
  xyData.cpp
  xyData.h
  xyzStructure.cpp
  xyzStructure.h
  xyzTrajectory.cpp
  xyzTrajectory.h
)

target_include_directories(
  nodes_import PUBLIC ${PROJECT_SOURCE_DIR}/src ADD ${ANTLR_CIFGrammarLexer_OUTPUT_DIR} ${ANTLR_CIFGrammarParser_OUTPUT_DIR}
)
target_link_libraries(nodes_import PUBLIC base ${THREADING_LINK_LIBS})
