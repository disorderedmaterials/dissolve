add_test(NAME avgmol
  COMMAND $<TARGET_FILE:${target_name}> -n 95 -x -i avgmol.txt
  WORKING_DIRECTORY ../tests/calculate_avgmol)
