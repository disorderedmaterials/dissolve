add_library(export data2D.cpp data3D.cpp pairPotential.cpp data2D.h data3D.h pairPotential.h)

target_include_directories(export PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(export PRIVATE base)
