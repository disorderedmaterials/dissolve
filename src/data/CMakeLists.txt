add_library(
  data
  atomicmasses.cpp
  atomicradii.cpp
  elementcolours.cpp
  elements.cpp
  formfactordata.cpp
  formfactors.cpp
  formfactors_dummy.cpp
  formfactors_wk1995.cpp
  isotopes.cpp
  spacegroups.cpp
  structurefactors.cpp
  atomicmasses.h
  atomicradii.h
  elementcolours.h
  elements.h
  formfactordata.h
  formfactors.h
  formfactors_dummy.h
  formfactors_wk1995.h
  isotopes.h
  spacegroups.h
  structurefactors.h
)

include_directories(data PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(data PRIVATE base)

add_subdirectory(ff)
add_subdirectory(sg)
