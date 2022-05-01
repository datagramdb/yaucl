cmake_minimum_required(VERSION 3.20)
project(knobab_server)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(submodules/httplib)
include_directories(submodules/magic_enum/include)
include_directories(submodules/date/include)
include_directories(submodules/json/include)
add_subdirectory(submodules/antlr4/runtime/Cpp)
include_directories(submodules/antlr4/runtime/Cpp/runtime/src)
include_directories(antlr4/cpp/antlr4)

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
        src/yaucl/data/MemoryMappingFile.cpp
        src/yaucl/data/mmapFile.cpp
        src/yaucl/data/xml.cpp include/yaucl/data/json.h)

add_library(yaucl_structures OBJECT src/yaucl/structures/StringPrevNext.cpp include/yaucl/structures/StringPrevNext.h src/yaucl/structures/DoublePrevNext.cpp include/yaucl/structures/DoublePrevNext.h)

add_library(yaucl_bpm OBJECT
        src/knobab/server/declare/CNFDeclareDataAware.cpp
        src/yaucl/bpm/structures/log/trace_visitor.cpp
        src/yaucl/bpm/structures/log/DataTraceParse.cpp
        src/yaucl/bpm/structures/commons/DataPredicate.cpp
        src/yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.cpp
         src/yaucl/bpm/structures/commons/testing_predicates.cpp
        src/yaucl/bpm/structures/ltlf/easy_prop.cpp
        )

add_executable(knobab_client src/knobab/client/client.cpp)

add_library(knobab OBJECT
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
        src/knobab/server/dataStructures/DataQuery.cpp
        src/knobab/server/dataStructures/marked_event.cpp
        src/knobab/server/tables/KnowledgeBase.cpp
        src/knobab/server/algorithms/grounding/kb_grounding.cpp
        src/knobab/server/query_manager/Environment.cpp
        src/knobab/server/algorithms/atomization/AtomizingPipeline.cpp
        src/knobab/server/benchmarking/LoggerInformation.cpp
        src/knobab/server/algorithms/querymanager/MAXSatPipeline.cpp
        src/knobab/server/operators/LTLfQuery.cpp
        src/knobab/server/ndp/KnowledgeBaseNDPLoader.cpp
        src/knobab/server/algorithms/querymanager/LTLfQueryManager.cpp
        )

add_executable(knobab_server src/knobab/server/server.cpp)
add_executable(tests_main src/knobab/tests/main.cpp)

add_library(knobab_ndp src/knobab/server/ndp/count_table_rcx.cpp src/knobab/server/ndp/KnowledgeBaseNDPLoader.cpp include/yaucl/data/FixedSizeNDPSorter.h src/knobab/server/ndp/attribute_table_rcx.cpp include/knobab/server/ndp/attribute_table_rcx.h)


if(MSVC OR MSYS OR MINGW)
    target_link_libraries(knobab_server antlr4_shared yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
    target_link_libraries(tests_main antlr4_shared yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
else()
    target_link_libraries(knobab_server antlr4_static yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
    target_link_libraries(tests_main antlr4_static yaucl_bpm yaucl_structures
            yaucl_numeric yaucl_data yaucl_graph yaucl_hashing knobab knobab_ndp)
endif()
