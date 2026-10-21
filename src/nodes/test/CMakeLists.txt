add_library(nodes_test test.cpp test.h)

target_include_directories(nodes_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_test PUBLIC base ${THREADING_LINK_LIBS})
