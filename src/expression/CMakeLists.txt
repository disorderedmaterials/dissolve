bison_target(ExpressionParser expression_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/expression_grammar.cc COMPILE_FLAGS -y )

add_library(expression
  ${BISON_ExpressionParser_OUTPUTS}
  expression.cpp
  expression_lexer.cpp
  function.cpp
  functions.cpp
  node.cpp
  value.cpp
  variable.cpp

  expression.h
  function.h
  functions.h
  node.h
  value.h
  variable.h
)

target_include_directories(expression PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)
