bison_target(NETAGenerator neta_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/neta_grammar.cc COMPILE_FLAGS -y )

add_library(neta
  ${BISON_NETAGenerator_OUTPUTS}
  connection.cpp
  generator.cpp
  logic.cpp
  neta.cpp
  node.cpp
  presence.cpp
  ring.cpp
  root.cpp

  connection.h
  generator.h
  logic.h
  neta.h
  node.h
  presence.h
  ring.h
  root.h
)

target_include_directories(neta PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
)
