add_library(keywordtypes
  bool.cpp
  broadeningfunction.cpp
  charstring.cpp
  complex.cpp
  double.cpp
  int.cpp

  bool.h
  broadeningfunction.h
  charstring.h
  complex.h
  double.h
  int.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

