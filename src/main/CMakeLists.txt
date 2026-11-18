add_library(
  main
  compatibility.cpp
  cli.cpp
  version.cpp
  cli.h
  compatibility.h
  version.h
)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

target_link_libraries(main PRIVATE CLI11::CLI11 toml11::toml11)
