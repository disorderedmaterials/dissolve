# Testing Code
enable_testing()

function(dissolve_system_test file_name count directory)
  add_test(NAME ${directory}-${file_name}
    COMMAND $<TARGET_FILE:${target_name}> -n ${count} -x -i ${file_name}.txt
    WORKING_DIRECTORY ../tests/${directory})
  set_property(TEST ${directory}-${file_name}
    PROPERTY
    LABELS ${directory})
endfunction()

add_subdirectory(atomshake)
add_subdirectory(broadening)
add_subdirectory(calculate_avgmol)
add_subdirectory(calculate_cn)
add_subdirectory(calculate_dangle)
add_subdirectory(calculate_rdf)
add_subdirectory(calculate_sdf)
add_subdirectory(correlations)
add_subdirectory(ff)
