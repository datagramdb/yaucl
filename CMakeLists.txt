cmake_minimum_required(VERSION 3.16)
project(yaucl_graph)
set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_library(yaucl_hashing STATIC
        include/yaucl/hashing/evaluateHashes.h
        src/yaucl/hashing/evaluateHashes.c
        src/yaucl/hashing/hash_combine.cpp
        include/yaucl/hashing/hash_combine.h
        src/yaucl/hashing/hashing.cpp
        include/yaucl/hashing/hashing.h
        include/yaucl/hashing/pair_hash.h
        include/yaucl/hashing/vector_hash.h
        include/yaucl/hashing/uset_hash.h)
target_include_directories(yaucl_hashing PUBLIC ${CMAKE_SOURCE_DIR}/include/yaucl/hashing/ ${CMAKE_CURRENT_BINARY_DIR})


add_library(yaucl_graph STATIC
        src/yaucl/graphs/adjacency_graph.cpp
        include/yaucl/graphs/adjacency_graph.h
        include/yaucl/graphs/FlexibleGraph.h
        include/yaucl/graphs/NodeLabelBijectionGraph.h
        src/yaucl/graphs/graph_join_pm_algorithms.cpp
        src/yaucl/graphs/graph_join_pm_conversion.cpp
        include/yaucl/graphs/NodeLabelBijectionFA.h
        src/yaucl/graphs/adjacency_entry.cpp
        include/yaucl/graphs/adjacency_entry.h
        src/yaucl/graphs/graph_join_pm.cpp
        include/yaucl/graphs/graph_join_pm.h
        include/yaucl/graphs/graph_join_pm.h
        src/yaucl/graphs/graph_join_pm_conversion.cpp
        src/yaucl/graphs/graph_join_pm_algorithms.cpp src/yaucl/graphs/minimizeDFA.cpp include/yaucl/graphs/algorithms/minimizeDFA.h)

