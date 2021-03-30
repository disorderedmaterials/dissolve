# NETA ANTLR Lexer/Parser
antlr_target(NETAGrammarLexer NETALexer.g4 LEXER)
# PACKAGE NETAGrammar)
antlr_target(
  NETAGrammarParser
  NETAParser.g4
  PARSER
  # PACKAGE NETAGrammar
  DEPENDS_ANTLR
  NETAGrammarLexer
  COMPILE_FLAGS
  -no-listener
  -visitor
  -lib
  ${ANTLR_NETAGrammarLexer_OUTPUT_DIR}
)

# Append path to ANTLR parser output, and set cache variable
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_NETAGrammarLexer_OUTPUT_DIR})
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_NETAGrammarParser_OUTPUT_DIR})
set(ANTLR_OUTPUT_DIRS
    ${ANTLR_OUTPUT_DIRS}
    CACHE INTERNAL ""
)

add_library(
  neta
  ${ANTLR_NETAGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_NETAGrammarParser_CXX_OUTPUTS}
  connection.cpp
  neta.cpp
  node.cpp
  or.cpp
  presence.cpp
  ring.cpp
  root.cpp
  NETAErrorListeners.cpp
  NETAVisitor.cpp
  connection.h
  neta.h
  node.h
  or.h
  presence.h
  ring.h
  root.h
  NETAErrorListeners.h
  NETAVisitor.h
)

target_include_directories(
  neta PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${ANTLRRUNTIME_INCLUDE_DIRS} ${ANTLR_NETAGrammarLexer_OUTPUT_DIR}
               ${ANTLR_NETAGrammarParser_OUTPUT_DIR}
)

target_link_libraries(neta PRIVATE base)
