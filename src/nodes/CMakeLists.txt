add_library(nodes atomShake.cpp atomShake.h node.cpp node.h parameter.cpp parameter.h)

target_include_directories(nodes PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes PUBLIC base ${THREADING_LINK_LIBS})

