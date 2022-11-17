add_executable(gtr basic_operators_testing.cpp)
target_link_libraries(gtr gtest Threads::Threads antlr4_static yaucl_bpm yaucl_structures
        yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)

include(GoogleTest)
gtest_discover_tests(gtr)