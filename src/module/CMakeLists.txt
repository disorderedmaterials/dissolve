add_library(modules
  module.cpp
  keywordbase.cpp
  keywordlist.cpp
  list.cpp
  registry.cpp
  modulekeyword_bool.cpp
  modulekeyword_broadeningfunction.cpp
  modulekeyword_charstring.cpp
  modulekeyword_complex.cpp
  modulekeyword_double.cpp
  modulekeyword_int.cpp

  module.h
  list.h
  registry.h
  keywordbase.h
  keyworddata.h
  keywordlist.h
  list.h
  registry.h
  modulekeyword_bool.h
  modulekeyword_broadeningfunction.h
  modulekeyword_charstring.h
  modulekeyword_complex.h
  modulekeyword_double.h
  modulekeyword_int.h
  keywords.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

# Subdirectories
add_subdirectory(keywordtypes)
