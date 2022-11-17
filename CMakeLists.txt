cmake_minimum_required(VERSION 3.20)
project(knobab_server)
enable_testing()
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
set(CMAKE_CXX_STANDARD 20)
set(WITH_DEMO false)
set(ANTLR_BUILD_CPP_TESTS false)
include_directories(include)
include_directories(submodules/httplib)
include_directories(submodules/magic_enum/include)
include_directories(submodules/date/include)
include_directories(submodules/json/include)

include_directories(
        submodules/antlr4/runtime/Cpp/runtime/src
        submodules/antlr4/runtime/Cpp/runtime/src/atn
        submodules/antlr4/runtime/Cpp/runtime/src/dfa
        submodules/antlr4/runtime/Cpp/runtime/src/misc
        submodules/antlr4/runtime/Cpp/runtime/src/support
        submodules/antlr4/runtime/Cpp/runtime/src/tree
        submodules/antlr4/runtime/Cpp/runtime/src/tree/pattern
        submodules/antlr4/runtime/Cpp/runtime/src/tree/xpath
)


file(GLOB libantlrcpp_SRC
        "submodules/antlr4/runtime/Cpp/runtime/src/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/atn/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/dfa/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/misc/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/support/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/tree/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/tree/pattern/*.cpp"
        "submodules/antlr4/runtime/Cpp/runtime/src/tree/xpath/*.cpp"
        )

add_library(antlr4_shared SHARED ${libantlrcpp_SRC})
add_library(antlr4_static STATIC ${libantlrcpp_SRC})

include_directories(antlr4/cpp/antlr4)
include_directories(submodules/thread-pool)

set(ANTLR_BUILD_CPP_TESTS OFF)
set(WITH_DEMO OFF)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_library(yaucl_numeric OBJECT
        src/yaucl/numeric/numeric_base.cpp
        include/yaucl/numeric/numeric_base.h)

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
        src/yaucl/graphs/graph_join_pm_algorithms.cpp src/yaucl/graphs/minimizeDFA.cpp include/yaucl/graphs/algorithms/minimizeDFA.h)

add_library(yaucl_stringutils OBJECT
        src/yaucl/strings/string_utils.cpp include/yaucl/strings/string_utils.h)

add_library(yaucl_data OBJECT
        src/yaucl/data/new_iovec.cpp
        src/yaucl/data/MemoryMappingFile.cpp
        src/yaucl/data/mmapFile.cpp
        src/yaucl/data/xml.cpp include/yaucl/data/json.h src/yaucl/data/VarSizeNDPSorter.cpp)

add_library(yaucl_structures OBJECT src/yaucl/structures/StringPrevNext.cpp include/yaucl/structures/StringPrevNext.h src/yaucl/structures/DoublePrevNext.cpp include/yaucl/structures/DoublePrevNext.h)

add_library(yaucl_bpm OBJECT
        src/knobab/server/declare/CNFDeclareDataAware.cpp
        src/yaucl/bpm/structures/log/trace_visitor.cpp
        src/yaucl/bpm/structures/log/DataTraceParse.cpp
        src/yaucl/bpm/structures/commons/DataPredicate.cpp
        src/yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.cpp
         src/yaucl/bpm/structures/commons/testing_predicates.cpp
        src/yaucl/bpm/structures/ltlf/easy_prop.cpp
        src/yaucl/bpm/structures/ltlf/ltlf.cpp)

add_executable(knobab_client src/knobab/client/client.cpp)

add_library(knobab OBJECT
        QueryBuilder.cpp
        QueryBuilder.h
        src/knobab/server/query_manager/KnoBABQueryBaseListener.cpp
        src/SimplifiedFuzzyStringMatching.cpp
        src/knobab/server/query_manager/KnoBABQueryBaseVisitor.cpp
        src/knobab/server/declare/DeclareDataAware.cpp
        src/yaucl/bpm/structures/log/data_loader.cpp
        src/knobab/server/query_manager/KnoBABQueryLexer.cpp
        src/knobab/server/query_manager/KnoBABQueryListener.cpp
        src/knobab/server/query_manager/KnoBABQueryParser.cpp
        src/knobab/server/query_manager/KnoBABQueryVisitor.cpp
        src/knobab/server/query_manager/ServerQueryManager.cpp
        src/knobab/server/algorithms/grounding/grounding.cpp
        src/knobab/server/tables/CountTable.cpp
        src/knobab/server/tables/ActTable.cpp
        src/knobab/server/tables/AttributeTable.cpp
        src/yaucl/bpm/structures/log/TracesParser.cpp
        ./src/yaucl/bpm/structures/log/TracesLexer.cpp
        ./src/yaucl/bpm/structures/log/TracesBaseListener.cpp
        ./src/yaucl/bpm/structures/log/TracesBaseVisitor.cpp
        ./src/yaucl/bpm/structures/log/TracesListener.cpp
        ./src/yaucl/bpm/structures/log/TracesVisitor.cpp
        src/knobab/server/declare/DisjunctiveDeclareDataAware.cpp
        src/knobab/server/dataStructures/oid.cpp
        src/knobab/server/tables/CountTable.cpp
        src/knobab/server/tables/ActTable.cpp
        src/knobab/server/tables/AttributeTable.cpp
        src/knobab/server/dataStructures/marked_event.cpp
        src/knobab/server/tables/KnowledgeBase.cpp
        src/knobab/server/algorithms/grounding/kb_grounding.cpp
        src/knobab/server/query_manager/Environment.cpp
        src/knobab/server/algorithms/atomization/AtomizingPipeline.cpp
        src/knobab/server/benchmarking/LoggerInformation.cpp
        src/knobab/server/algorithms/querymanager/MAXSatPipeline.cpp
        src/knobab/server/operators/base_ltlf.cpp
        src/knobab/server/ndp/KnowledgeBaseNDPLoader.cpp
        src/knobab/server/algorithms/querymanager/LTLfQueryManager.cpp
        src/knobab/server/ndp/NDPFuzzyStringMatching.cpp include/knobab/server/query_manager/NDPFuzzyStringMatching.h src/knobab/server/ndp/NDPAttributeTable.cpp include/knobab/server/ndp/NDPAttributeTable.h include/yaucl/bpm/structures/ltlf/ltlf.h include/knobab/server/algorithms/querymanager/DataQuery.h src/knobab/server/algorithms/querymanager/DataQuery.cpp include/knobab/server/algorithms/querymanager/LTLfQuery.h src/knobab/server/algorithms/querymanager/LTLfQuery.cpp src/knobab/algorithms/inference/clause_inference.cpp include/knobab/algorithms/inference/clause_inference.h)

add_executable(knobab_server src/knobab/server/server.cpp)


add_library(knobab_ndp
        src/knobab/server/ndp/count_table_rcx.cpp
        src/knobab/server/ndp/KnowledgeBaseNDPLoader.cpp
        include/yaucl/data/FixedSizeNDPSorter.h
        src/knobab/server/ndp/act_table_rcx.cpp
        include/knobab/server/ndp/act_table_rcx.h
        src/knobab/server/ndp/KnowledgeBaseNDPReader.cpp
        src/yaucl/data/VariadicSizeArrayElements.cpp include/yaucl/data/VariadicSizeArrayElements.h)

add_executable(benchmarking benchmarking/benchmarking.cpp benchmarking/benchmarking.cpp include/knobab/algorithms/ModelViewKnoBAB.h)

add_executable(mining
        src/knobab/mining/pattern_mining.cpp
        include/knobab/mining/pattern_mining.h
        src/knobab/mining/mining.cpp
        src/knobab/mining/CountTableFPTree.cpp PatternMiningConfiguration.cpp PatternMiningConfiguration.h )

if(MSVC OR MSYS OR MINGW)
    target_link_libraries(knobab_server Threads::Threads antlr4_shared yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
    target_link_libraries(benchmarking knobab antlr4_shared knobab_ndp
            yaucl_stringutils yaucl_numeric yaucl_hashing yaucl_data yaucl_bpm yaucl_graph yaucl_structures)
    target_link_libraries(mining Threads::Threads antlr4_static yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)

else()
    target_link_libraries(knobab_server Threads::Threads antlr4_static yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
    target_link_libraries(mining Threads::Threads antlr4_static yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
    target_link_libraries(benchmarking knobab antlr4_static knobab_ndp
            yaucl_stringutils yaucl_numeric yaucl_hashing yaucl_data yaucl_bpm yaucl_graph yaucl_structures knobab_ndp)
endif()



add_subdirectory(src/knobab/tests)