#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <utility>
#include "board_elements/board_element.h"

class BoardNode {
public:
    std::pair<int, int> position;
    BoardElement& node_element; //todo: check if two elements can be on the same cell

    explicit BoardNode(std::pair<int, int> pos, BoardElement* elem);

};

#endif // GRAPHNODE_H
