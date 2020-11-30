# Atomshake tests
add_test(NAME atomshake
  COMMAND $<TARGET_FILE:${target_name}> -n 100 -x -i singlewater.txt
  WORKING_DIRECTORY ../tests/atomshake)
