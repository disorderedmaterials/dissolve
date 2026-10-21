add_library(
  nodes_evolution
  atomicMC.cpp
  atomicMC.h
  intraMC.cpp
  intraMC.h
  mc.cpp
  mc.h
  md.cpp
  md.h
)

target_include_directories(nodes_evolution PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_evolution PUBLIC base ${THREADING_LINK_LIBS})
