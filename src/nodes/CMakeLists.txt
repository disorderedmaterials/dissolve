add_library(
  nodes
  constants.h
  dissolve.cpp
  dissolve.h
  edge.cpp
  edge.h
  energy.cpp
  energy.h
  epsr.cpp
  epsr.h
  epsrHelpers.cpp
  forcefield.cpp
  forcefield.h
  graph.cpp
  graph.h
  inputs.cpp
  inputs.h
  iterableGraph.cpp
  iterableGraph.h
  loopBack.cpp
  loopBack.h
  node.cpp
  node.h
  number.cpp
  number.h
  outputs.cpp
  outputs.h
  parameter.cpp
  parameter.h
  registry.cpp
  registry.h
  serialisableData.h
)

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})

# Node subdirectories
message(STATUS "Looking for nodes in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)
set(NODE_DIRS "")
foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
    list(APPEND NODE_DIRS ${child})
  endif()
endforeach()

foreach(subdir ${NODE_DIRS})
  message(STATUS " ... Adding node directory '${subdir}'...")
  add_subdirectory(${subdir})

  # Add to global link list
  set(NODES_LINK_LIBS
      ${NODES_LINK_LIBS} "nodes_${subdir}"
      CACHE INTERNAL "" FORCE
  )
endforeach()
