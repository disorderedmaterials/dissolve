add_library(nodes_utility delay.cpp delay.h)

target_include_directories(nodes_utility PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_utility PUBLIC base ${THREADING_LINK_LIBS})
