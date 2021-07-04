cmake_minimum_required(VERSION 3.16)
project(yaucl)
include_directories(include)
set(CMAKE_CXX_STANDARD 20)

add_library(yaucl_algorithms STATIC src/algorithms/binsearch.cpp include/yaucl/fs/fwrite.h)

add_library(yaucl_fs STATIC include/yaucl/fs/mmapFile.h
                            src/fs/mmapFile.cpp include/yaucl/java/legacy_graph_algo_names.h src/fs/frwite.cpp)
add_library(yaucl_memory STATIC include/yaucl/memory/new_iovec.h
                                src/memory/new_iovec.cpp
                                include/yaucl/memory/smart_malloc.h
                                src/memory/smart_malloc.cpp src/memory/memory_utils.cpp)

add_library(yaucl_hashing STATIC include/yaucl/hashing/evaluateHashes.h
                                 src/hashing/evaluateHashes.c)

add_library(yaucl_strings STATIC include/yaucl/strings/stringutils.h src/strings/stringutils.cpp)

add_executable(yaucl_test main.cpp )
target_link_libraries(yaucl_test yaucl_fs yaucl_memory)