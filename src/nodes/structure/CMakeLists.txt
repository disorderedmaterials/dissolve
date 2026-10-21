add_library(
  nodes_structure
  atomicSpecies.cpp
  atomicSpecies.h
  calculateBonding.cpp
  calculateBonding.h
  clearBonding.cpp
  clearBonding.h
  configuration.cpp
  configuration.h
  detectMolecules.cpp
  detectMolecules.h
  insertRandom.cpp
  insertRandom.h
  instantiate.cpp
  instantiate.h
  replicatedConfiguration.cpp
  replicatedConfiguration.h
  setBox.cpp
  setBox.h
  setCoordinates.cpp
  setCoordinates.h
  speciesBase.cpp
  speciesBase.h
  species.cpp
  species.h
)

target_include_directories(nodes_structure PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_structure PUBLIC base ${THREADING_LINK_LIBS})
