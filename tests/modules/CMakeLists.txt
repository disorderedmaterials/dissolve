dissolve_unit_test(SRC epsr.cpp USE_TEST_DIRECTORY unit)
dissolve_unit_test(SRC forces.cpp USE_TEST_DIRECTORY energyforce2)

# List individual module test subdirectories
message(STATUS "Looking for individual module tests in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(
        GLOB children
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*"
)

set(MODULE_TEST_DIRS "")
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
        list(APPEND MODULE_TEST_DIRS ${child})
    endif()
endforeach()

foreach(subdir ${MODULE_TEST_DIRS})
    add_subdirectory(${subdir})
endforeach()
