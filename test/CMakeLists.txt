set(TEST_SRCS
  test_parse.cpp
  test_solve.cpp
  test_util.cpp
  test_sat_solver.cpp
)

add_executable(tests ${TEST_SRCS})

target_link_libraries(tests PRIVATE xcc)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
