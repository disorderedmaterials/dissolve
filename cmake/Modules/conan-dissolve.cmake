# Fetch and include Conan-cmake integration if it doesn't exist
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Set basic requirements and options
set(_conan_requires
    cli11/1.9.1
    fmt/8.1.1
    pugixml/1.11
    onetbb/2021.10.0
    onedpl/2022.3.0
    toml11/3.7.0
    antlr4-cppruntime/4.13.1
    ${EXTRA_CONAN_REQUIRES}
)
set(_conan_options
    fmt:header_only=True
    pugixml:header_only=False
    antlr4-cppruntime:shared=True
    ${EXTRA_CONAN_OPTIONS}
)

# Handle platform-specific requirements
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND _conan_requires freetype/2.13.2)
endif()

# Add in testing dependencies?
if(BUILD_TESTS)
    list(APPEND _conan_requires gtest/1.10.0)
endif(BUILD_TESTS)

conan_cmake_configure(
  REQUIRES
    ${_conan_requires}
  GENERATORS cmake_find_package deploy
  OPTIONS
    ${_conan_options}
)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})
