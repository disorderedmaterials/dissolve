add_library(
  nodes_correlation
  bragg.cpp
  bragg.h
  gr.cpp
  gr.h
  neutronSQ.cpp
  neutronSQ.h
  sq.cpp
  sq.h
  xRaySQ.cpp
  xRaySQ.h
)

target_include_directories(nodes_correlation PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_correlation PUBLIC base ${THREADING_LINK_LIBS})
