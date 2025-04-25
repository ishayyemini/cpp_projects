#include "graph_node.h"



BoardNode::BoardNode(std::pair<int, int> pos, GameObject* elem): position(std::move(pos)), node_element(*elem) {}
