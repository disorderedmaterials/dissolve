add_library(modules
module.h
module.cpp

atomshake.h
energy.h
forces.h
grainshake.h
intrashake.h
md.h
twist.h

atomshake.cpp
energy.cpp
forces.cpp
grainshake.cpp
intrashake.cpp
md.cpp
twist.cpp
}

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
