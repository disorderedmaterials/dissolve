add_library(
  base
  applicative.cpp
  cbor.cpp
  enumOption.h
  enumOptions.h
  enumOptionsBase.cpp
  enumOptionsBase.h
  geometry.cpp
  geometry.h
  lock.cpp
  lock.h
  messenger.cpp
  messenger.h
  outputHandler.cpp
  outputHandler.h
  parserLibrary.cpp
  serialiser.h
  serialiser.cpp
  sysFunc.cpp
  sysFunc.h
  timer.cpp
  timer.h
  units.cpp
  units.h
  version.cpp
  version.h
)

target_include_directories(base PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(base PUBLIC ${THREADING_LINK_LIBS})
