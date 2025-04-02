cmake_minimum_required(VERSION 3.30)
project(hw1)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(tanks_game src/main.cpp)
