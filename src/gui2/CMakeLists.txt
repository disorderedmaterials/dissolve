# Target 'gui'
add_library(gui2)

target_include_directories(gui2 PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

target_link_libraries(gui2 PRIVATE models2 base Qt6::QuickWidgets)

add_subdirectory(models)
add_subdirectory(qml/icons)