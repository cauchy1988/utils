cmake_minimum_required(VERSION 2.8)
project(tc_utils)

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -O3 -Wno-sign-compare -fpermissive -fopenmp")

include_directories (${CMAKE_SOURCE_DIR}/include)

add_executable(test-utils test/main.cpp)
