bison_target(ExpressionParser expression_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/expression_grammar.cc COMPILE_FLAGS -y )

add_library(expression
  ${BISON_ExpressionParser_OUTPUTS}
  expression.cpp
  expression_lexer.cpp
  functionnode.cpp
  functions.cpp
  node.cpp
  variable.cpp
  variablenode.cpp
  expression.h
  functionnode.h
  functions.h
  node.h
  variable.h
  variablenode.h
)

target_include_directories(expression PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)
