add_test(NAME sdf
  COMMAND $<TARGET_FILE:${target_name}> -n 95 -x -i sdf.txt
  WORKING_DIRECTORY ../tests/calculate_sdf)
