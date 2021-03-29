add_library(
  base
  geometry.cpp
  lineparser.cpp
  lock.cpp
  messenger.cpp
  outputhandler.cpp
  processgroup.cpp
  processpool.cpp
  sysfunc.cpp
  timer.cpp
  units.cpp
  version.cpp
  enumoption.h
  enumoptionsbase.h
  enumoptions.h
  geometry.h
  lineparser.h
  lock.h
  messenger.h
  outputhandler.h
  processgroup.h
  processpool.h
  sysfunc.h
  timer.h
  units.h
  version.h
)

include_directories(base PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(base PUBLIC CONAN_PKG::fmt)
