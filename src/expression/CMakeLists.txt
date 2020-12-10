# Expression ANTLR Lexer/Parser
antlr_target(ExpressionGrammarLexer ExpressionLexer.g4 LEXER)
#              PACKAGE ExpressionGrammar)
antlr_target(ExpressionGrammarParser ExpressionParser.g4 PARSER
#              PACKAGE ExpressionGrammar
             DEPENDS_ANTLR ExpressionGrammarLexer
             COMPILE_FLAGS -no-listener -visitor -lib ${ANTLR_ExpressionGrammarLexer_OUTPUT_DIR})

# Append path to ANTLR parser output, and set cache variable
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_ExpressionGrammarLexer_OUTPUT_DIR})
list(APPEND ANTLR_OUTPUT_DIRS ${ANTLR_ExpressionGrammarParser_OUTPUT_DIR})
set(ANTLR_OUTPUT_DIRS ${ANTLR_OUTPUT_DIRS} CACHE INTERNAL "")

# Old Bison target
bison_target(ExpressionGenerator generator_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/generator_grammar.cc COMPILE_FLAGS -y )

add_library(expression
  ${BISON_ExpressionGenerator_OUTPUTS}
  expression.cpp
  function.cpp
  functions.cpp
  generator.cpp
  node.cpp
  value.cpp
  variable.cpp
  variablevalue.cpp
  ExpressionErrorListeners.cpp
  ExpressionVisitor.cpp

  expression.h
  function.h
  functions.h
  generator.h
  node.h
  value.h
  variable.h
  variablevalue.h
  ExpressionErrorListeners.h
  ExpressionVisitor.h

  ${ANTLR_ExpressionGrammarLexer_CXX_OUTPUTS}
  ${ANTLR_ExpressionGrammarParser_CXX_OUTPUTS}

  ExpressionErrorListeners.cpp
  ExpressionVisitor.cpp
  binary.cpp
  expressionNEW.cpp
  functionNEW.cpp
  nodeNEW.cpp
  number.cpp
  reference.cpp
  root.cpp
  unary.cpp

  ExpressionErrorListeners.h
  ExpressionVisitor.h
  binary.h
  expressionNEW.h
  functionNEW.h
  nodeNEW.h
  number.h
  reference.h
  root.h
  unary.h
)

target_include_directories(expression PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${ANTLRRUNTIME_INCLUDE_DIRS}
  ${ANTLR_ExpressionGrammarLexer_OUTPUT_DIR}
  ${ANTLR_ExpressionGrammarParser_OUTPUT_DIR}
)






