# Get list of sources, from which we can construct the exe / test list.
file(GLOB_RECURSE tests_SRC "*.cpp")
foreach(SRCFILE ${tests_SRC})
  dissolve_unit_test(${SRCFILE})
endforeach(SRCFILE)
