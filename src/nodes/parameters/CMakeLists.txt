add_library(
  parameters
  configuration.cpp
  configuration.h
  double.cpp
  double.h
  integer.cpp
  integer.h
  optionalDouble.cpp
  optionalDouble.h
)

target_include_directories(parameters PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(parameters PUBLIC base ${THREADING_LINK_LIBS})
