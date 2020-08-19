add_library(ff_pcl2019
  # Core
  intramolecular.cpp
  base.cpp

  base.h

  # Species
  anions.cpp
  cations.cpp

  anions.h
  cations.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
