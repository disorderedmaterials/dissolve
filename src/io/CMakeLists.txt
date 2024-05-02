add_library(io fileAndFormat.cpp fileAndFormat.h)

target_include_directories(io PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(io PRIVATE base)

# Subdirectories
add_subdirectory(export)
add_subdirectory(import)
