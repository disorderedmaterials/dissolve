add_library(
  nodes_export
  blockData.cpp
  blockData.h
  data.cpp
  data.h
  dlpolyConfiguration.cpp
  dlpolyConfiguration.h
  dlputilsPDensData.cpp
  dlputilsPDensData.h
  xyzConfiguration.cpp
  xyzConfiguration.h
  xyzTrajectory.cpp
  xyzTrajectory.h
)

target_include_directories(nodes_export PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_export PUBLIC base ${THREADING_LINK_LIBS})
