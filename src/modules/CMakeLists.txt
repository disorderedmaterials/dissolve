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
forces.cpp
import.cpp
md.cpp
partials.cpp
skeleton.cpp
test.cpp
#grainshake.cpp
#intrashake.cpp
#twist.cpp

atomshake.h
energy.h
export.h
forces.h
import.h
md.h
partials.h
skeleton.h
test.h
#grainshake.h
#intrashake.h
#twist.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
