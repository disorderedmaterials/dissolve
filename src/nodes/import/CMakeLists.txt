add_library(
  nodes_import
  dlpolyStructure.cpp
  dlpolyStructure.h
  dlpolyTrajectory.cpp
  dlpolyTrajectory.h
  dlputilsPDens.cpp
  dlputilsPDens.h
  dlputilsSurface.cpp
  dlputilsSurface.h
  epsrAtoStructure.cpp
  epsrAtoStructure.h
  moscitoStructure.cpp
  moscitoStructure.h
  species.cpp
  species.h
  xyData.cpp
  xyData.h
  xyzStructure.cpp
  xyzStructure.h
  xyzTrajectory.cpp
  xyzTrajectory.h
)

target_include_directories(nodes_import PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_import PUBLIC base ${THREADING_LINK_LIBS})
