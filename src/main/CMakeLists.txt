add_library(
  main
  comms.cpp
  compatibility.cpp
  cli.cpp
  dissolve.cpp
  io.cpp
  keywords.cpp
  keywords_configuration.cpp
  keywords_layer.cpp
  keywords_master.cpp
  keywords_module.cpp
  keywords_pairPotentials.cpp
  pairPotentials.cpp
  simulation.cpp
  version.cpp
  cli.h
  compatibility.h
  dissolve.h
  keywords.h
  version.h
)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})

target_link_libraries(main PRIVATE fmt::fmt CLI11::CLI11 toml11::toml11)
