add_library(keywordtypes
  atomtypeselection.cpp
  bool.cpp
  broadeningfunction.cpp
  charstring.cpp
  complex.cpp
  double.cpp
  int.cpp
  isotopologuelist.cpp
  pairbroadeningfunction.cpp
  windowfunction.cpp
  xydatastore.cpp

  atomtypeselection.h
  bool.h
  broadeningfunction.h
  charstring.h
  complex.h
  double.h
  int.h
  isotopologuelist.h
  pairbroadeningfunction.h
  windowfunction.h
  xydatastore.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

