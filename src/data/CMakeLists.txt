add_library(
  data
  atomicMasses.cpp
  atomicRadii.cpp
  elementColours.cpp
  elements.cpp
  formFactorData.cpp
  formFactors.cpp
  formFactors_dummy.cpp
  formFactors_WK1995.cpp
  isotopes.cpp
  spaceGroups.cpp
  structureFactors.cpp
  atomicMasses.h
  atomicRadii.h
  elementColours.h
  elements.h
  formFactorData.h
  formFactors.h
  formFactors_dummy.h
  formFactors_WK1995.h
  isotopes.h
  spaceGroups.h
  structureFactors.h
)

target_include_directories(data PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(data PRIVATE base)

add_subdirectory(ff)
add_subdirectory(sginfo)
