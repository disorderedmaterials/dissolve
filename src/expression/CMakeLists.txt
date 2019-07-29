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

  expression.h
  function.h
  functions.h
  generator.h
  node.h
  value.h
  variable.h
  variablevalue.h
)

target_include_directories(expression PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
)
