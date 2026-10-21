add_library(
  nodes_export
  exportBlockData.cpp
  exportBlockData.h
  exportData.cpp
  exportData.h
  exportDLPOLYConfiguration.cpp
  exportDLPOLYConfiguration.h
  exportDLPUtilsPDensData.cpp
  exportDLPUtilsPDensData.h
  exportXYZConfiguration.cpp
  exportXYZConfiguration.h
  exportXYZTrajectory.cpp
  exportXYZTrajectory.h
)

target_include_directories(nodes_export PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_export PUBLIC base ${THREADING_LINK_LIBS})
