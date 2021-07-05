cmake_minimum_required(VERSION 3.16)
project(yaucl)

set(MY_LIBRARY_VERSION_MAJOR 0 CACHE STRING "major version" FORCE)
set(MY_LIBRARY_VERSION_MINOR 1 CACHE STRING "minor version" FORCE)
set(MY_LIBRARY_VERSION ${MY_LIBRARY_VERSION_MAJOR}.${MY_LIBRARY_VERSION_MINOR} CACHE STRING "version" FORCE)


include_directories(include)
set(CMAKE_CXX_STANDARD 20)

add_library(yaucl_algorithms STATIC src/algorithms/binsearch.cpp)

add_library(yaucl_fs STATIC include/yaucl/fs/mmapFile.h
                            src/fs/mmapFile.cpp include/yaucl/java/legacy_graph_algo_names.h src/fs/frwite.cpp)
target_include_directories(yaucl_fs PUBLIC ${CMAKE_SOURCE_DIR}/include/yaucl/fs/ ${CMAKE_CURRENT_BINARY_DIR})

add_library(yaucl_graphs src/graphs/primary_memory_graph.cpp include/yaucl/graphs/primary_memory_graph.h src/graphs/adjacency_list_entry.cpp include/yaucl/graphs/adjacency_list_entry.h src/graphs/algorithms/tarjan.cpp include/yaucl/graphs/algorithms/tarjan.h nesting.cpp nesting.h)

add_library(yaucl_memory STATIC include/yaucl/memory/new_iovec.h
                                src/memory/new_iovec.cpp
                                include/yaucl/memory/smart_malloc.h
                                src/memory/smart_malloc.cpp src/memory/memory_utils.cpp include/yaucl/memory/smart_index_pointer.h src/memory/smart_index_pointer.cpp src/memory/index.cpp include/yaucl/memory/index.h)
target_include_directories(yaucl_memory PUBLIC ${CMAKE_SOURCE_DIR}/include/yaucl/memory/ ${CMAKE_CURRENT_BINARY_DIR})

add_library(yaucl_hashing STATIC include/yaucl/hashing/evaluateHashes.h
                                 src/hashing/evaluateHashes.c src/hashing/hash_combine.cpp include/yaucl/hashing/hash_combine.h src/hashing/hashing.cpp include/yaucl/hashing/hashing.h include/yaucl/hashing/pair_hash.h include/yaucl/hashing/vector_hash.h include/yaucl/hashing/uset_hash.h)
target_include_directories(yaucl_hashing PUBLIC ${CMAKE_SOURCE_DIR}/include/yaucl/hashing/ ${CMAKE_CURRENT_BINARY_DIR})

add_library(yaucl_strings STATIC include/yaucl/strings/stringutils.h src/strings/stringutils.cpp include/yaucl/strings/StringBuilder.h)
target_include_directories(yaucl_strings PUBLIC ${CMAKE_SOURCE_DIR}/include/yaucl/strings/ ${CMAKE_CURRENT_BINARY_DIR})

add_executable(yaucl_test main.cpp )
target_link_libraries(yaucl_test yaucl_fs yaucl_memory)


INSTALL (
        DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")

install (TARGETS yaucl_algorithms
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )

install (TARGETS yaucl_fs
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )

install (TARGETS yaucl_memory
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )

install (TARGETS yaucl_hashing
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )

install (TARGETS yaucl_strings
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )