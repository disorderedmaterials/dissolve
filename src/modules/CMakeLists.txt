add_library(modules
module.cpp

module.h

atomshake.cpp
#energy.cpp
fq.cpp
#forces.cpp
#grainshake.cpp
#intrashake.cpp
#md.cpp
#twist.cpp

atomshake.h
#energy.h
fq.h
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
