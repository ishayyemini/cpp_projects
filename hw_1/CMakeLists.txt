cmake_minimum_required(VERSION 3.30)
project(hw1)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(tanks_game src/main.cpp
        src/utils/file_utils.h
        src/utils/file_utils.cpp
        src/board/game_board.h
        src/board/game_board.cpp
        src/board_elements/board_element_factory.h
        src/board_elements/board_element_factory.cpp
        src/board_elements/board_element.h
        src/board_elements/empty_cell.h
        src/board_elements/mine.h
        src/board_elements/tank.h
        src/board_elements/tank.cpp
        src/board_elements/wall.h
)
