add_library(
  main
  atomtypes.cpp
  comms.cpp
  configurations.cpp
  cli.cpp
  dissolve.cpp
  io.cpp
  keywords.cpp
  keywords_configuration.cpp
  keywords_layer.cpp
  keywords_master.cpp
  keywords_module.cpp
  keywords_pairpotentials.cpp
  keywords_simulation.cpp
  layers.cpp
  masterterms.cpp
  objects.cpp
  pairpotentials.cpp
  simulation.cpp
  species.cpp
  version.cpp
  cli.h
  dissolve.h
  keywords.h
  version.h
)

include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src ${CONAN_INCLUDE_DIRS})
if(CONAN)
  target_link_libraries(main PRIVATE CONAN_PKG::fmt CONAN_PKG::cli11)
else(CONAN)
  target_link_libraries(main PRIVATE fmt::fmt CLI11::CLI11)
endif(CONAN)
