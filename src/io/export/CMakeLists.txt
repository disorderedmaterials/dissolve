add_library(export pairPotential.cpp pairPotential.h)

target_include_directories(export PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(export PRIVATE base)
