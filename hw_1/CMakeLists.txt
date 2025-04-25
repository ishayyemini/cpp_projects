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
        src/board_elements/mine.h
        src/board_elements/tank.h
        src/board_elements/tank.cpp
        src/board_elements/wall.h
        src/board_elements/wall.cpp
        src/game/action.h
        src/game/game_manager.h
        src/game/game_manager.cpp
        src/game/game_manager.cpp
        src/algo/algorithm.h
        src/algo/algorithm.cpp
        src/algo/player1_algo.h
        src/algo/player1_algo.cpp
        src/algo/player2_algo.h
        src/algo/player2_algo.cpp
        src/algo/graph/graph_node.h
        src/algo/graph/graph_node.cpp
        src/algo/graph/board_graph.h
        src/algo/graph/board_graph.cpp
        src/board_elements/board_element.cpp
)
