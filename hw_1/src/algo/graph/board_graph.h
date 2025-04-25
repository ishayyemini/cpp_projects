#ifndef BOARDGRAPH_H
#define BOARDGRAPH_H

#include <unordered_map>
#include <vector>
#include <utility>
#include <memory>

#include "graph_node.h"
#include "board/game_board.h"

class BoardGraph {
public:
    explicit BoardGraph(const GameBoard &board);

    bool isValidCell(const std::pair<int, int> &pos) const;

    // BoardNode* getNode(const std::pair<int, int>& pos) const;

private:
    const GameBoard &board;
    std::pair<int, int> board_size;
    std::map<std::pair<int, int>, std::unique_ptr<BoardNode> > nodes;
    std::map<std::pair<int, int>, std::vector<std::unique_ptr<BoardNode> > > graph;

    void initGraph(const GameBoard &board);

    void updateGraph(const GameBoard &board);

    void initNodes(const GameBoard &board);

    void addEdges();
};

#endif // BOARDGRAPH_H

