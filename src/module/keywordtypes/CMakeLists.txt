add_library(keywordtypes
  analyser.cpp
  atomtypeselection.cpp
  bool.cpp
  broadeningfunction.cpp
  charstring.cpp
  complex.cpp
  datastore.cpp
  double.cpp
  int.cpp
  isotopologuelist.cpp
  pairbroadeningfunction.cpp
  windowfunction.cpp

  analyser.h
  atomtypeselection.h
  bool.h
  broadeningfunction.h
  charstring.h
  complex.h
  datastore.h
  double.h
  int.h
  isotopologuelist.h
  pairbroadeningfunction.h
  windowfunction.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

