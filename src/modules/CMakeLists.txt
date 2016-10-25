add_library(modules
module.h
atomshake.h
energy.h
forces.h
grainshake.h
intrashake.h
md.h
perturb.h
twist.h

atomshake.cpp
energy.cpp
forces.cpp
grainshake.cpp
intrashake.cpp
md.cpp
perturb.cpp
twist.cpp
}

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
