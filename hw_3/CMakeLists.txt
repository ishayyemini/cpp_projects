cmake_minimum_required(VERSION 3.10)
project(tanks_game VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add compiler warnings and make them errors
add_compile_options(-Wall -Wextra -Werror -pedantic)

# Create position-independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src/algo ${PROJECT_SOURCE_DIR}/src/game_manager ${PROJECT_SOURCE_DIR}/common)

# Add all source files (excluding main.cpp)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.h")
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Add the main executable
add_executable(tanks_game ${SOURCES} "${PROJECT_SOURCE_DIR}/src/main.cpp")

# Add the shared library for Python
add_library(tanks_game_lib SHARED
        ${SOURCES}
)

# Set output directory
set_target_properties(tanks_game
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Set library name and properties
set_target_properties(tanks_game_lib
        PROPERTIES
        OUTPUT_NAME "tanks_game"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_command(
        TARGET tanks_game
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tanks_game> ${PROJECT_SOURCE_DIR}
        COMMENT "Copying tanks_game to ${PROJECT_SOURCE_DIR}"
)
