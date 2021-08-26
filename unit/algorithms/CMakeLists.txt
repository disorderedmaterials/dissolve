# Get list of source files and register tests
file(GLOB_RECURSE tests_SRC "*.cpp")
foreach(SRCFILE ${tests_SRC})
  dissolve_unit_test(${SRCFILE})
endforeach(SRCFILE)
