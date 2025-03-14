add_library(
  nodes
  add.h
  atomShake.cpp
  atomShake.h
  derivative.cpp
  derivative.h
  dissolve.h
  dotProduct.cpp
  dotProduct.h
  integrator.cpp
  integrator.h
  graph.cpp
  graph.h
  multiply.h
  node.cpp
  node.h
  subtract.h
  vec3Assembly.cpp
  vec3Assembly.h
  vec3Decomposition.cpp
  vec3Decomposition.h
  parameter.cpp
  parameter.h
  parameterLink.cpp
  parameterLink.h
  registry.h
)

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})
