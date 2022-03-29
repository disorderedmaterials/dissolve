add_library(io fileandformat.cpp fileandformat.h)

target_include_directories(io PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(io PRIVATE base)

# Subdirectories
add_subdirectory(export)
add_subdirectory(import)
