add_test(NAME dangle
  COMMAND $<TARGET_FILE:${target_name}> -n 95 -x -i dangle.txt
  WORKING_DIRECTORY ../tests/calculate_dangle)
