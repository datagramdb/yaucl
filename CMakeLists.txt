cmake_minimum_required(VERSION 3.20)
project(knobab_server)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include_directories(submodules/magic_enum/include)
include_directories(submodules/date/include)
include_directories(submodules/json/include)
add_subdirectory(submodules/antlr4/runtime/Cpp)
include_directories(submodules/antlr4/runtime/Cpp/runtime/src)
include_directories(antlr4/cpp/antlr4)

add_library(yaucl_numeric STATIC
        src/yaucl/numeric/numeric_base.cpp
        include/yaucl/numeric/numeric_base.h)

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

add_library(yaucl_stringutils STATIC
        src/yaucl/strings/string_utils.cpp include/yaucl/strings/string_utils.h)

add_library(yaucl_data STATIC
        src/yaucl/data/xml.cpp include/yaucl/data/json.h)

add_library(yaucl_structures STATIC src/yaucl/structures/StringPrevNext.cpp include/yaucl/structures/StringPrevNext.h src/yaucl/structures/DoublePrevNext.cpp include/yaucl/structures/DoublePrevNext.h)

add_library(yaucl_bpm STATIC
        #src/yaucl/bpm/structures/ltlf/ltlf.cpp
        #include/yaucl/bpm/structures/ltlf/ltlf.h
        src/knobab/server/declare/CNFDeclareDataAware.cpp
        include/knobab/server/declare/CNFDeclareDataAware.h
#        src/yaucl/bpm/structures/declare/DeclareModelParse.cpp
#        include/yaucl/bpm/structures/declare/DeclareModelParse.h
        src/yaucl/bpm/structures/log/trace_visitor.cpp
        include/yaucl/bpm/structures/log/trace_visitor.h
        #src/yaucl/bpm/structures/log/data_loader.cpp
        src/yaucl/bpm/structures/log/DataTraceParse.cpp
        src/yaucl/bpm/structures/commons/DataPredicate.cpp
        include/yaucl/bpm/structures/commons/DataPredicate.h
        src/yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.cpp
        include/yaucl/bpm/structures/ltlf/PropositionalizedAtomsSet.h
#        src/yaucl/bpm/structures/declare/DADBaseListener.cpp
#        src/yaucl/bpm/structures/declare/DADBaseVisitor.cpp
#        src/yaucl/bpm/structures/declare/DADListener.cpp
#        src/yaucl/bpm/structures/declare/DADLexer.cpp
#        src/yaucl/bpm/structures/declare/DADParser.cpp
#        src/yaucl/bpm/structures/declare/DADVisitor.cpp
        #src/knobab/server/declare/DeclareDataAware.cpp
        #include/knobab/server/declare/DeclareDataAware.h
        #src/yaucl/bpm/algos/transformations/declare_to_dfa/DeclareNoDataTemplateCollect.cpp
        #include/yaucl/bpm/algos/transformations/declare_to_dfa/DeclareNoDataTemplateCollect.h
        #src/yaucl/bpm/algos/transformations/declare_to_dfa/TemplateCollectResult.cpp
        #include/yaucl/bpm/algos/transformations/declare_to_dfa/TemplateCollectResult.h
         src/yaucl/bpm/structures/commons/testing_predicates.cpp include/yaucl/bpm/structures/commons/testing_predicates.h src/yaucl/bpm/structures/ltlf/easy_prop.cpp include/yaucl/bpm/structures/commons/easy_prop.h)
target_link_libraries(yaucl_bpm antlr4_static yaucl_numeric yaucl_data yaucl_graph yaucl_hashing) #aaltaf)




add_executable(knobab_server src/knobab/server/server.cpp
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
        include/knobab/server/query_manager/ServerQueryManager.h
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
        src/knobab/server/declare/DisjunctiveDeclareDataAware.cpp include/knobab/server/declare/DisjunctiveDeclareDataAware.h src/knobab/server/dataStructures/oid.cpp include/knobab/server/dataStructures/oid.h include/knobab/server/tables/ActTable.h include/knobab/server/tables/AttributeTable.h include/knobab/server/tables/CountTemplate.h src/knobab/server/tables/CountTable.cpp src/knobab/server/tables/ActTable.cpp src/knobab/server/tables/AttributeTable.cpp include/knobab/server/dataStructures/DataQuery.h src/knobab/server/dataStructures/DataQuery.cpp src/knobab/server/dataStructures/marked_event.cpp include/knobab/server/dataStructures/marked_event.h include/knobab/server/dataStructures/TraceData.h src/knobab/server/tables/KnowledgeBase.cpp include/knobab/server/tables/KnowledgeBase.h include/knobab/server/query_manager/Environment.h include/knobab/server/algorithms/grounding/kb_grounding.h src/knobab/server/algorithms/grounding/kb_grounding.cpp src/knobab/server/query_manager/Environment.cpp include/knobab/server/dataStructures/atomization_structures.h include/knobab/server/algorithms/atomization/AtomizingPipeline.h src/knobab/server/algorithms/atomization/AtomizingPipeline.cpp include/knobab/server/benchmarking/LoggerInformation.h src/knobab/server/benchmarking/LoggerInformation.cpp include/knobab/server/algorithms/querymanager/MAXSatPipeline.h src/knobab/server/algorithms/querymanager/MAXSatPipeline.cpp include/knobab/server/operators/LTLfQuery.h src/knobab/server/operators/LTLfQuery.cpp include/knobab/server/operators/simple_ltlf_operators.h include/knobab/server/operators/fast_ltlf_operators.h include/knobab/server/algorithms/querymanager/LTLfQueryManager.h src/knobab/server/algorithms/querymanager/LTLfQueryManager.cpp include/knobab/server/operators/ltlf_operators.h)
target_link_libraries(knobab_server antlr4_static yaucl_bpm yaucl_structures)