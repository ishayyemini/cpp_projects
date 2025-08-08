cmake_minimum_required(VERSION 3.10)
project(tanks_game VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add compiler warnings and make them errors
add_compile_options(-Wall -Wextra -Werror -pedantic)

# Export symbols from main executable for dlopen
set(CMAKE_ENABLE_EXPORTS ON)

# Add subdirectories
add_subdirectory(simulator)
add_subdirectory(algorithm)
add_subdirectory(gameManager)
