//
// Created by Ishay Yemini on 24/04/2025.
//

#include "board_element.h"
#include "wall.h"

std::ostream &operator<<(std::ostream &os, const BoardElement &element) {
    switch (element.getSymbol()) {
        case '1':
            os << "[" << element.getDirection() << " 🚘 1]";
            break;
        case '2':
            os << "[" << element.getDirection() << " 🚘 2]";
            break;
        case '*':
            os << "[" << element.getDirection() << " ☄️  ]";
            break;
        case '#':
            if (const auto wall = dynamic_cast<const Wall *>(&element)) {
                os << "[  🧱 " << wall->getHealth() << "]";
            } else {
                os << "[  🧱  ]";
            }
            break;
        case '@':
            os << "[  💣  ]";
            break;
        default: ;
    }
    return os;
}
