add_library(
        task
        context.cpp
        task.cpp
        context.h
        task.h
)

target_include_directories(task PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(task PRIVATE base)
