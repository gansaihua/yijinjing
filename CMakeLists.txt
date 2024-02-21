cmake_minimum_required(VERSION 3.10)

project(yijinjing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")

add_subdirectory(yijinjing)
add_subdirectory(demo)
