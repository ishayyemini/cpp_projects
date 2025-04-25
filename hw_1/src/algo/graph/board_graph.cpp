#include "board_graph.h"
#include "board/game_board.h"
#include "board_elements/mine.h"
#include "board_elements/wall.h"

BoardGraph::BoardGraph(const GameBoard &board): board(board),
                                                board_size(board.getHeight(), board.getWidth()),
                                                nodes(std::map<std::pair<int, int>, std::unique_ptr<
                                                    BoardNode> >()),
                                                graph(std::map<std::pair<int, int>, std::vector<
                                                    std::unique_ptr<
                                                        BoardNode> > >()) {
    initGraph(board);
}

void BoardGraph::initGraph(const GameBoard &board) {
    initNodes(board);
    addEdges();
}

void BoardGraph::initNodes(const GameBoard &board) {
    for (int i = 0; i < board_size.first; ++i) {
        for (int j = 0; j < board_size.second; ++j) {
            std::pair pos = {i, j};
            GameObject *element = board.getBoardElement(i, j);
            nodes[pos] = std::make_unique<BoardNode>(pos, element);
        }
    }
}

void BoardGraph::addEdges() {
    for (int i = 0; i < board_size.first; ++i) {
        for (int j = 0; j < board_size.second; ++j) {
            std::pair pos = {i, j};
            for (int d = 0; d < Direction::getDirectionSize(); ++d) {
                std::pair<int, int> offset = Direction::getOffset(Direction::getDirection(d));
                int ni = (i + offset.first + board_size.first) % board_size.first;
                int nj = (j + offset.second + board_size.second) % board_size.second;

                graph[pos].push_back(std::move(nodes[{ni, nj}]));
            }
        }
    }
}

bool BoardGraph::isValidCell(const std::pair<int, int> &pos) const {
    // Check if the cell is traversable
    const GameObject *element = board.getBoardElement(pos);
    if (element == nullptr) {
        return true;
    }

    if (dynamic_cast<const Wall *>(element) != nullptr) {
        return false; // Walls are not traversable
    }
    if (dynamic_cast<const Mine *>(element) != nullptr) {
        return false; // Mines are not traversable
    }

    return true; // The cell is valid and traversable
}

//todo: add bfs logic + update the graph to be steps graph.
//todo: add update graph so we don't need to init it from the beggining each time

