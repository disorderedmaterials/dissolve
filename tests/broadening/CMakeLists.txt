# Broadening tests
add_test(NAME broadening-qdep-1
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i argon_qdep0.1.txt
  WORKING_DIRECTORY ../tests/broadening)

add_test(NAME broadening-qdep-2
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i argon_qdep0.2.txt
  WORKING_DIRECTORY ../tests/broadening)

add_test(NAME broadening-indep-1
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i argon_qindep0.1.txt
  WORKING_DIRECTORY ../tests/broadening)

add_test(NAME broadening-indep-2
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i argon_qindep0.2.txt
  WORKING_DIRECTORY ../tests/broadening)


add_test(NAME broadening-dep-2-indep-1
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i argon_dep0.2indep0.1.txt
  WORKING_DIRECTORY ../tests/broadening)

add_test(NAME broadening-dep-1-indep-2
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i argon_dep0.1indep0.2.txt
  WORKING_DIRECTORY ../tests/broadening)
