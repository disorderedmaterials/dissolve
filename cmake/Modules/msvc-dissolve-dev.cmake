    # Find and install dependencies for a Visual Studio 2022 dev environment
    function(setup_msvc)
        set(SEARCH_IN "${CMAKE_BINARY_DIR}")
        set(INSTALLATION_DIR "${SEARCH_IN}")

        set(ALL_INSTALL_FILES "")

        set(CONAN_DEPS ${_conan_requires})
        list(APPEND CONAN_DEPS ${EXTRA_CONAN_REQUIRES})

        # Find exe, dll and lib paths from Conan packages
        foreach (DEPENDENCY IN LISTS CONAN_DEPS)
            string(FIND ${DEPENDENCY} "/" SUBSTRING_AT)
            string(SUBSTRING ${DEPENDENCY} 0 "${SUBSTRING_AT}" SUB_DIR)

            set(SEARCH_IN ${CMAKE_BINARY_DIR}/${SUB_DIR})

            file(GLOB CONAN_BIN "${SEARCH_IN}/bin/*.exe" "${SEARCH_IN}/bin/*.dll")
            file(GLOB CONAN_LIB "${SEARCH_IN}/lib/*.lib")

            set(CONAN_FILES ${CONAN_BIN})
            list(APPEND CONAN_FILES ${CONAN_LIB})

            list(APPEND ALL_INSTALL_FILES ${CONAN_FILES})
        endforeach()

        # Find Conan2 packages
        if(EXISTS "${PROJECT_SOURCE_DIR}/conan" AND IS_DIRECTORY "${PROJECT_SOURCE_DIR}/conan")
            file(GLOB_RECURSE CONAN2_DLL "${PROJECT_SOURCE_DIR}/conan/*.dll")
            list(APPEND ALL_INSTALL_FILES ${CONAN2_DLL})
        endif()

        # Install all dependencies
        foreach(DEPENDENCY IN LISTS ALL_INSTALL_FILES)
            file(COPY ${DEPENDENCY} DESTINATION "${INSTALLATION_DIR}")
        endforeach()
    endfunction()