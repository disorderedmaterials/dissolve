add_library(
  nodes
  add.cpp
  add.h
  atomShake.cpp
  atomShake.h
  constants.h
  derivative.cpp
  derivative.h
  dissolve.h
  dotProduct.cpp
  dotProduct.h
  edge.cpp
  edge.h
  integrator.cpp
  integrator.h
  graph.cpp
  graph.h
  multiply.cpp
  multiply.h
  node.cpp
  node.h
  number.cpp
  number.h
  subtract.cpp
  subtract.h
  vec3Assembly.cpp
  vec3Assembly.h
  vec3Decomposition.cpp
  vec3Decomposition.h
  parameter.cpp
  parameter.h
  registry.h
)

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})
