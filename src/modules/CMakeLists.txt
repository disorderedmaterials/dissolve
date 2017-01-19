add_library(modules
module.cpp
modulelist.cpp
moduleregistry.cpp

module.h
modulelist.h
moduleregistry.h

atomshake.cpp
energy.cpp
export.cpp
partials.cpp
skeleton.cpp
#forces.cpp
#grainshake.cpp
#intrashake.cpp
#md.cpp
#twist.cpp

atomshake.h
energy.h
export.h
partials.h
skeleton.h
#forces.h
#grainshake.h
#intrashake.h
#md.h
#twist.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
