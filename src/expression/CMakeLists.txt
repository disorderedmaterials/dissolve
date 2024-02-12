# Expression ANTLR Lexer/Parser
antlr_target(ExpressionGrammarLexer ExpressionLexer.g4 LEXER)
# PACKAGE ExpressionGrammar)
antlr_target(
  ExpressionGrammarParser
  ExpressionParser.g4
  PARSER
  # PACKAGE ExpressionGrammar
  DEPENDS_ANTLR
  ExpressionGrammarLexer
  COMPILE_FLAGS
  -no-listener
  -visitor
  -lib
  ${ANTLR_ExpressionGrammarLexer_OUTPUT_DIR}
)

# Append path to ANTLR parser output, and set cache variable
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_ExpressionGrammarLexer_OUTPUT_DIR})
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_ExpressionGrammarParser_OUTPUT_DIR})
set(ANTLR_OUTPUT_DIRS
    ${ANTLR_OUTPUT_DIRS}
    CACHE INTERNAL ""
)

add_library(
  expression
  ${ANTLR_ExpressionGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_ExpressionGrammarParser_CXX_OUTPUTS}
  ExpressionErrorListeners.cpp
  ExpressionVisitor.cpp
  binary.cpp
  expression.cpp
  function.cpp
  node.cpp
  number.cpp
  reference.cpp
  root.cpp
  unary.cpp
  value.cpp
  variable.cpp
  ExpressionErrorListeners.h
  ExpressionVisitor.h
  binary.h
  expression.h
  function.h
  node.h
  number.h
  reference.h
  root.h
  unary.h
  value.h
  variable.h
)

target_include_directories(
  expression PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${ANTLR_ExpressionGrammarLexer_OUTPUT_DIR}
                     ${ANTLR_ExpressionGrammarParser_OUTPUT_DIR}
)

target_link_libraries(expression base)
