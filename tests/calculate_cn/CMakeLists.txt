add_test(NAME cn
  COMMAND $<TARGET_FILE:${target_name}> -n 95 -x -i cn.txt
  WORKING_DIRECTORY ../tests/calculate_cn)
