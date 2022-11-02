include(GoogleTest)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(bop_test basic_operators_testing.cpp)
target_link_libraries(bop_test gtest_main Threads::Threads antlr4_static yaucl_bpm yaucl_structures
        yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
gtest_discover_tests(bop_test)

add_executable(fsm_test fuzzy_string_matching_test.cpp )
target_link_libraries(fsm_test gtest_main Threads::Threads antlr4_static yaucl_bpm yaucl_structures
        yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
gtest_discover_tests(fsm_test)

add_executable(gb_test group_by_test.cpp )
target_link_libraries(gb_test gtest_main Threads::Threads antlr4_static yaucl_bpm yaucl_structures
        yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
gtest_discover_tests(gb_test)