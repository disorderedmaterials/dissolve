# Target 'gui'
add_library(gui-qml)

target_include_directories(
  gui-qml
  PRIVATE ${PROJECT_SOURCE_DIR}/src
          ${PROJECT_BINARY_DIR}/src
          Qt6::Core
)

target_link_libraries(
  gui-qml
  PRIVATE models
          base
          Qt6::QuickWidgets
)

if(APPLE)
  #Don't link dead code on Apple
  target_link_options(gui-qml PRIVATE --unresolved-symbols=ignore-all)
endif(APPLE)
