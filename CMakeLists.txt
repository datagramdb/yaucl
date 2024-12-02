cmake_minimum_required(VERSION 3.20)
project(yaucl)
set(CMAKE_CXX_STANDARD 23)
# For Windows: Prevent overriding the parent project's compiler/linker settings
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
SET(CMAKE_CXX_FLAGS " -pthread") 
add_subdirectory(submodules/eigen)
include_directories(submodules/eigen)
include_directories(submodules/date/include)
include_directories(submodules/rapidcsv/src)
include_directories(submodules/json/include)
include_directories(submodules/magic_enum/include)
include_directories(submodules/CRoaring/include)
include_directories(submodules/CRoaring/cpp)
add_subdirectory(submodules/CRoaring)
add_subdirectory(submodules/antlr4/runtime/Cpp)
include_directories(submodules/antlr4/runtime/Cpp/runtime/src)
include_directories(antlr4/cpp/antlr4)
include_directories(include/)
include_directories(submodules/l-bfgs-b/include)

include_directories(include)

add_library(yaucl_learning OBJECT
        src/yaucl/learning/dt/ForTheWin.cpp
        src/yaucl/learning/dt/dt_predicate.cpp
        src/yaucl/learning/dt/utils.cpp
        src/yaucl/learning/dt/structures/find_argument.cpp
        src/yaucl/learning/dt/structures/n_vector.cpp
        include/yaucl/learning/dt/structures/n_vector.h
        src/yaucl/learning/dt/structures/Nodes.cpp
        include/yaucl/learning/dt/structures/Nodes.h
        src/yaucl/learning/dt/wodt/WODT.cpp
        src/yaucl/learning/dt/structures/DataRepo.cpp
        include/yaucl/learning/dt/structures/DataRepo.h
        src/yaucl/learning/DecisionTree.cpp
)


#add_library(yaucl_learning OBJECT
#        src/yaucl/learning/ForTheWin.cpp
#        src/yaucl/learning/dt_predicate.cpp
#        src/yaucl/learning/DecisionTree.cpp)
add_executable(yaucl_learning_decision_tree src/examples/decision_tree_example.cpp)
target_link_libraries(yaucl_learning_decision_tree yaucl_learning)
#add_executable(yaucl_learning_decision_tree_refinement src/examples/decision_tree_refinement_example.cpp)
#target_link_libraries(yaucl_learning_decision_tree_refinement yaucl_learning)

add_executable(yaucl_learning_mcl src/examples/mcl_example.cpp)

add_library(yaucl_hashing OBJECT
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
        src/yaucl/data/VariadicSizeArrayElements.cpp
        src/yaucl/data/VarSizeNDPSorter.cpp
        src/yaucl/data/xml.cpp
        include/yaucl/data/json.h
        src/yaucl/data/VarSizeNDPSorter.cpp
        src/yaucl/data/malloc_freelist.c
        src/yaucl/data/CSV2.cpp
        include/yaucl/data/CSV2.h
        src/yaucl/data/SecondaryMemoryMalloc.cpp
        include/yaucl/data/SecondaryMemoryMalloc.h include/yaucl/data/smart_malloc.h src/yaucl/data/smart_malloc.cpp include/yaucl/data/smart_index_pointer.h src/yaucl/data/smart_index_pointer.cpp)

add_library(yaucl_structures OBJECT src/yaucl/structures/StringPrevNext.cpp include/yaucl/structures/StringPrevNext.h src/yaucl/structures/DoublePrevNext.cpp include/yaucl/structures/DoublePrevNext.h include/yaucl/structures/setoids/set_comparators.h include/yaucl/structures/setoids/set_partitioning.h include/yaucl/structures/setoids/basics.h include/yaucl/structures/indexed/BlockOfRecordsIndexedTable.h src/yaucl/structures/RoaringBitmapWrapper.cpp src/yaucl/structures/NAryTree.cpp include/yaucl/structures/NAryTree.h include/yaucl/structures/RelationalTables.h)
target_link_libraries(yaucl_structures roaring)

add_library(yaucl_bpm OBJECT
        src/yaucl/graphs/flloat/FLLOATPropLexer.cpp
        src/yaucl/graphs/flloat/FLLOATPropParser.cpp
        src/yaucl/graphs/flloat/FLLOATPropVisitor.cpp
        src/yaucl/graphs/flloat/ParseFFLOATDot.cpp
        include/yaucl/graphs/flloat/ParseFFLOATDot.h
        src/yaucl/graphs/flloat/FLLOATSimplePropParser.cpp
        include/yaucl/graphs/flloat/FLLOATSimplePropParser.h
        SimpleXESSerializer.cpp
        src/yaucl/bpm/structures/log/data_loader.cpp
        src/yaucl/bpm/structures/log/DataTraceParse.cpp
                src/yaucl/bpm/structures/log/TracesParser.cpp
                ./src/yaucl/bpm/structures/log/TracesLexer.cpp
                ./src/yaucl/bpm/structures/log/TracesBaseListener.cpp
                ./src/yaucl/bpm/structures/log/TracesBaseVisitor.cpp
                ./src/yaucl/bpm/structures/log/TracesListener.cpp
                ./src/yaucl/bpm/structures/log/TracesVisitor.cpp
        src/yaucl/bpm/structures/commons/DeclareDataAware.cpp
        src/yaucl/bpm/structures/commons/DataPredicate.cpp
        src/yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.cpp
         src/yaucl/bpm/structures/commons/testing_predicates.cpp
        src/yaucl/bpm/structures/ltlf/easy_prop.cpp
        src/yaucl/bpm/structures/ltlf/ltlf.cpp
        #
        )

add_library(yaucl_bpm_new_atomization
        src/yaucl/bpm/algos/atomization/AtomizingPipeline.cpp)

set(CMAKE_CXX_STANDARD 23)



add_library(yaucl_numeric STATIC
        src/yaucl/numeric/numeric_base.cpp
        include/yaucl/numeric/numeric_base.h)


add_library(yaucl_graph OBJECT
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
        src/yaucl/graphs/graph_join_pm_algorithms.cpp
        src/yaucl/graphs/minimizeDFA.cpp
        include/yaucl/graphs/algorithms/minimizeDFA.h
        include/yaucl/graphs/algorithms/all_cycles.h
        include/yaucl/graphs/algorithms/AllDirectedPaths.h
        src/yaucl/graphs/algorithms/prim.cpp
        include/yaucl/graphs/algorithms/prim.h
        src/yaucl/graphs/algorithms/connected_components.cpp
        include/yaucl/graphs/algorithms/connected_components.h
        src/yaucl/graphs/dot/DOTLexer.cpp
        src/yaucl/graphs/dot/DOTParser.cpp
        src/yaucl/graphs/dot/DOTVisitor.cpp
)
target_link_libraries(yaucl_graph  yaucl_hashing roaring antlr4_static)

add_executable(natural_join_examples src/examples/natural_join_examples.cpp include/yaucl/data/smart_malloc.h)
