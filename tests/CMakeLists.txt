# Testing Code
enable_testing()

function(dissolve_system_test directory file_name count)
  if(PARALLEL)
    foreach(nproc 1 2 3 4)
      set(test_target_executable ${CMAKE_BINARY_DIR}/bin/${target_name})
      add_test(
        NAME ${directory}-${file_name}-${nproc}
        COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${nproc} ${test_target_executable} -n ${count} -x ${file_name}.txt ${ARGN}
        WORKING_DIRECTORY ../tests/${directory}
      )
      set_property(TEST ${directory}-${file_name}-${nproc} PROPERTY LABELS ${directory})
      set_property(TEST ${directory}-${file_name}-${nproc} PROPERTY PROCESSORS ${nproc})
    endforeach()
  else()
    set(test_target_executable ${CMAKE_BINARY_DIR}/bin/${target_name})
    add_test(
      NAME ${directory}-${file_name}
      COMMAND ${test_target_executable} -n ${count} -x ${file_name}.txt ${ARGN}
      WORKING_DIRECTORY ../tests/${directory}
    )
    set_property(TEST ${directory}-${file_name} PROPERTY LABELS ${directory})
  endif()
endfunction()

add_subdirectory(atomshake)
add_subdirectory(broadening)
add_subdirectory(calculate_avgmol)
add_subdirectory(calculate_cn)
add_subdirectory(calculate_dangle)
add_subdirectory(calculate_rdf)
add_subdirectory(calculate_sdf)
add_subdirectory(correlations)
add_subdirectory(energyforce1)
add_subdirectory(energyforce2)
add_subdirectory(energyforce3)
add_subdirectory(energyforce4)
add_subdirectory(epsr)
add_subdirectory(exchangeable)
add_subdirectory(ff)
add_subdirectory(md)
add_subdirectory(molshake)
add_subdirectory(rdfmethod)
add_subdirectory(restart)
add_subdirectory(xray)
