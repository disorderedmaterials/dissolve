add_test(NAME kulmala2010
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 kulmala2010.txt
  WORKING_DIRECTORY ../tests/ff)

add_test(NAME oplsaa2005-alcohols
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 oplsaa2005-alcohols.txt
  WORKING_DIRECTORY ../tests/ff)

add_test(NAME oplsaa2005-alkanes
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 oplsaa2005-alkanes.txt
  WORKING_DIRECTORY ../tests/ff)

add_test(NAME oplsaa2005-aromatics
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 oplsaa2005-aromatics.txt
  WORKING_DIRECTORY ../tests/ff)

add_test(NAME pcl2019-anions
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 pcl2019-anions.txt
  WORKING_DIRECTORY ../tests/ff)

add_test(NAME pcl2019-cations
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 pcl2019-cations.txt
  WORKING_DIRECTORY ../tests/ff)

add_test(NAME spcfw
  COMMAND $<TARGET_FILE:${target_name}> -x -i -n 1 spcfw.txt
  WORKING_DIRECTORY ../tests/ff)
