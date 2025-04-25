#include "board_graph.h"
#include "board/game_board.h"

BoardGraph::BoardGraph(const GameBoard& board): graph(), board_size(board.getHeight(), board.getWidth()), nodes() {
    initGraph(board);
}

void BoardGraph::initGraph(const GameBoard& board) {
    initNodes(board);
    addEdges();
}

void BoardGraph::initNodes(const GameBoard& board) {
    for (int i = 0; i < board_size.first; ++i) {
        for (int j = 0; j < board_size.second; ++j) {
            std::pair pos = {i, j};
            BoardElement* element = board.getBoardElement(i, j);
            nodes[pos] = std::make_unique<BoardNode>(pos, element);
        }
    }
}

void BoardGraph::addEdges() {
    for (int i = 0; i < board_size.first; ++i) {
        for (int j = 0; j < board_size.second; ++j) {
            std::pair pos = {i, j};
            for (int d = 0; d < Direction::getDirectionSize(); ++d) {
                std::pair<int, int> offset = Direction::getOffset(d);
                int ni = (i + offset.first + board_size.first) % board_size.first;
                int nj = (j + offset.second + board_size.second) % board_size.second;

                graph[pos].push_back(nodes[{ni, nj}]);
            }
        }
    }
}



//todo: add bfs logic + update the graph to be steps graph.
//todo: add update graph so we don't need to init it from the beggining each time

