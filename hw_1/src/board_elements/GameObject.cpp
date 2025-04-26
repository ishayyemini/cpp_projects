#include "../include/GameObject.h"

#include <iosfwd>

#include "Wall.h"

std::ostream &operator<<(std::ostream &os, const GameObject &element) {
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
                os << "[  🧱 " << wall->getHitCount() << "]";
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
