add_library(
  base
  cbor.cpp
  enumOptionsBase.cpp
  geometry.cpp
  lineParser.cpp
  lock.cpp
  messenger.cpp
  outputHandler.cpp
  sysFunc.cpp
  timer.cpp
  units.cpp
  version.cpp
  enumOption.h
  enumOptionsBase.h
  enumOptions.h
  geometry.h
  lineParser.h
  lock.h
  messenger.h
  outputHandler.h
  sysFunc.h
  timer.h
  units.h
  version.h
)

target_include_directories(base PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(base PUBLIC ${THREADING_LINK_LIBS})
