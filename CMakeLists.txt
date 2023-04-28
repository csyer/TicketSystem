cmake_minimum_required(VERSION 3.0)

project(TicketSystem)

include_directories(src/stl)
include_directories(src/bplus_tree)
include_directories(src/save)
include_directories(src/exceptions)

add_subdirectory(src)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(main main.cpp)
