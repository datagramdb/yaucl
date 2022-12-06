cmake_minimum_required(VERSION 3.20)
project(yaucl)
set(CMAKE_CXX_STANDARD 23)
# For Windows: Prevent overriding the parent project's compiler/linker settings


include_directories(submodules/CRoaring/include)
include_directories(submodules/CRoaring/cpp)
add_subdirectory(submodules/CRoaring)
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


add_library(yaucl_stringutils OBJECT
        src/yaucl/strings/string_utils.cpp include/yaucl/strings/string_utils.h)

add_library(yaucl_data OBJECT
        src/yaucl/data/new_iovec.cpp
        src/yaucl/data/MemoryMappingFile.cpp
        src/yaucl/data/mmapFile.cpp
        src/yaucl/data/xml.cpp include/yaucl/data/json.h src/yaucl/data/VarSizeNDPSorter.cpp)

add_library(yaucl_structures OBJECT src/yaucl/structures/StringPrevNext.cpp include/yaucl/structures/StringPrevNext.h src/yaucl/structures/DoublePrevNext.cpp include/yaucl/structures/DoublePrevNext.h include/yaucl/structures/setoids/set_comparators.h include/yaucl/structures/setoids/set_partitioning.h include/yaucl/structures/setoids/basics.h)

add_library(yaucl_bpm OBJECT
        src/yaucl/bpm/algos/atomization/AtomizingPipeline.cpp
        src/yaucl/bpm/structures/commons/DeclareDataAware.cpp
        #src/knobab/server/declare/CNFDeclareDataAware.cpp
        #src/yaucl/bpm/structures/log/trace_visitor.cpp
        #src/yaucl/bpm/structures/log/DataTraceParse.cpp
        src/yaucl/bpm/structures/commons/DataPredicate.cpp
        src/yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.cpp
         src/yaucl/bpm/structures/commons/testing_predicates.cpp
        src/yaucl/bpm/structures/ltlf/easy_prop.cpp
        src/yaucl/bpm/structures/ltlf/ltlf.cpp)


set(CMAKE_CXX_STANDARD 23)






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
        src/yaucl/graphs/graph_join_pm_algorithms.cpp src/yaucl/graphs/minimizeDFA.cpp include/yaucl/graphs/algorithms/minimizeDFA.h include/yaucl/graphs/algorithms/all_cycles.h include/yaucl/graphs/algorithms/AllDirectedPaths.h)
target_link_libraries(yaucl_graph yaucl_hashing roaring)
