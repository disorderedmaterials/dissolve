add_library(math
doubleexp.cpp
mathfunc.cpp
matrix3.cpp
matrix4.cpp
constants.h
doubleexp.h
mathfunc.h
matrix3.h
matrix4.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
