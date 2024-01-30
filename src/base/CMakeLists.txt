add_library(
  base
  enumOptionsBase.cpp
  geometry.cpp
  lineParser.cpp
  lock.cpp
  messenger.cpp
  outputHandler.cpp
  processGroup.cpp
  processPool.cpp
  randomBuffer.cpp
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
  processGroup.h
  processPool.h
  randomBuffer.h
  sysFunc.h
  timer.h
  units.h
  version.h
)

target_include_directories(base PRIVATE ${PROJECT_SOURCE_DIR}/src)
if(CONAN)
  target_link_libraries(base PUBLIC ${THREADING_LINK_LIBS})
else(CONAN)
  target_link_libraries(base PUBLIC fmt::fmt ${THREADING_LINK_LIBS})
endif(CONAN)
