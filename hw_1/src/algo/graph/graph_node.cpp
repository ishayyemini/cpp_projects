#include "graph_node.h"



BoardNode::BoardNode(std::pair<int, int> pos, BoardElement* elem): position(std::move(pos)), node_element(*elem) {}
