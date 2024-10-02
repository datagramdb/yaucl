cmake_minimum_required(VERSION 3.28)
project(DT)

set(CMAKE_CXX_STANDARD 20)

include_directories(include/)
include_directories(submodules/eigen/)
include_directories(submodules/lbfgsb/include)

add_executable(DT main.cpp
        src/dt/ForTheWin.cpp
        src/dt/dt_predicate.cpp
        src/dt/utils.cpp
src/dt/structures/find_argument.cpp
        src/dt/structures/n_vector.cpp
        include/dt/structures/n_vector.h
        src/dt/structures/Nodes.cpp
        include/dt/structures/Nodes.h)
