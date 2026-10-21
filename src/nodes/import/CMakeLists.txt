add_library(
  nodes_import
  importDLPOLYStructure.cpp
  importDLPOLYStructure.h
  importDLPOLYTrajectory.cpp
  importDLPOLYTrajectory.h
  importDLPUtilsPDens.cpp
  importDLPUtilsPDens.h
  importDLPUtilsSurface.cpp
  importDLPUtilsSurface.h
  importEPSRAtoStructure.cpp
  importEPSRAtoStructure.h
  importMoscitoStructure.cpp
  importMoscitoStructure.h
  importSpecies.cpp
  importSpecies.h
  importXYData.cpp
  importXYData.h
  importXYZStructure.cpp
  importXYZStructure.h
  importXYZTrajectory.cpp
  importXYZTrajectory.h
)

target_include_directories(nodes_import PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_import PUBLIC base ${THREADING_LINK_LIBS})
