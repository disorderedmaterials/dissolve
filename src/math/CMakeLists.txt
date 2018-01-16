add_library(math
  broadeningfunction.cpp
  cuboid.cpp
  doubleexp.cpp
  mathfunc.cpp
  matrix3.cpp
  matrix4.cpp
  windowfunction.cpp

  broadeningfunction.h
  constants.h
  cuboid.h
  doubleexp.h
  mathfunc.h
  matrix3.h
  matrix4.h
  windowfunction.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
