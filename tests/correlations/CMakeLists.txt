# Atomshake tests
add_test(NAME correlations
  COMMAND $<TARGET_FILE:${target_name}> -n 1 -x -i sq.txt
  WORKING_DIRECTORY ../tests/correlations)
