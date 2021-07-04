cmake_minimum_required(VERSION 3.16)
project(yaucl)
include_directories(include)
set(CMAKE_CXX_STANDARD 20)

add_library(yaucl_fs STATIC include/yaucl/fs/mmapFile.h
                            src/fs/mmapFile.cpp)
add_library(yaucl_memory STATIC include/yaucl/memory/new_iovec.h
                                src/memory/new_iovec.cpp
                                include/yaucl/memory/smart_malloc.h
                                src/memory/smart_malloc.cpp)

add_executable(yaucl_test main.cpp )
target_link_libraries(yaucl_test yaucl_fs yaucl_memory)