# Target 'gui'
add_library(gui-qml)

target_include_directories(gui-qml PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src Qt6::Core)

target_link_libraries(gui-qml PRIVATE models base Qt6::QuickWidgets)
