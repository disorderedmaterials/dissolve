add_library(
  import
)

target_include_directories(import PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(import base nodes)
