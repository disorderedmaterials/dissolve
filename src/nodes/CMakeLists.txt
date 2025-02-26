add_library(nodes atomShake.cpp atomShake.h node.h)

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})

add_subdirectory(parameters)
