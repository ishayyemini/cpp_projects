#include "wall.h"

Wall::Wall() = default;

char Wall::getSymbol() const {
    return '#';
}

ElementType Wall::getType() const {
    return ElementType::Wall;
}
