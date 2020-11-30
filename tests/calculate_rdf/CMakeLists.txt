add_test(NAME calculate_rdf
  COMMAND $<TARGET_FILE:${target_name}> -n 95 -x -i rdf.txt
  WORKING_DIRECTORY ../tests/calculate_rdf)
