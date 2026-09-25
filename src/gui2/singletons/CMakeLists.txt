# Singletons ###
set(dissolveSingletons_MOC_HDRS utility.h)

set(dissolveSingletons_SRCS utility.cpp)

# QML Types ###
set(# QML type register
    types_SRCS types.h types.cpp
)

qt6_wrap_cpp(dissolveSingletons_MOC_SRCS ${dissolveSingletons_MOC_HDRS})

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

add_library(dissolveSingletons ${dissolveSingletons_SRCS} ${dissolveSingletons_MOC_SRCS} ${types_SRCS})

target_include_directories(
  dissolveSingletons PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${CONAN_INCLUDE_DIRS_PUGIXML} ${Qt6Widgets_INCLUDE_DIRS}
                             ${FREETYPE_INCLUDE_DIRS} ${Qt6Quick_INCLUDE_DIRS}
)

target_link_libraries(dissolveSingletons PRIVATE Qt6::Core Qt6::Qml)

# Because Qt has its own way of checking for C++17
if(MSVC)
  target_compile_options(dissolveSingletons PUBLIC /Zc:__cplusplus)
endif(MSVC)
