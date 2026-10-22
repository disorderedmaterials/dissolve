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

# Node subdirectories
message(STATUS "Looking for nodes in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
  GLOB children
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*"
)

foreach(child ${children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
    list(APPEND NODE_DIRS ${child})
    message(STATUS " ... Adding node directory '${child}'...")
    add_subdirectory(${child})

    # Add to global link list
    set(NODE_LINK_LIBS
        ${NODE_LINK_LIBS} "nodes_${child}"
        CACHE INTERNAL "" FORCE
    )
  endif()
endforeach()

foreach(subdir ${NODE_DIRS})

endforeach()

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src ${ANTLR_OUTPUT_DIRS})
target_link_libraries(nodes PUBLIC base ${NODE_LINK_LIBS} ${THREADING_LINK_LIBS})
