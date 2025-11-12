file(
  GLOB_RECURSE node_sources
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./*.cpp"
)


file(
  GLOB_RECURSE node_headers
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./*.h"
)

add_library(nodes ${node_sources} ${node_headers})

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})
