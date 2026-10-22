add_library(
  nodes_math
  add.cpp
  add.h
  derivative.cpp
  derivative.h
  dotProduct.cpp
  dotProduct.h
  integrator.cpp
  integrator.h
  multiply.cpp
  multiply.h
  number.cpp
  number.h
  subtract.cpp
  subtract.h
  vector3Assemble.cpp
  vector3Assemble.h
  vector3Decompose.cpp
  vector3Decompose.h
)

target_include_directories(nodes_math PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_math PUBLIC base ${THREADING_LINK_LIBS})
