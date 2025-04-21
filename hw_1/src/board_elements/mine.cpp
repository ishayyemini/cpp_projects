#include "mine.h"

Mine::Mine() = default;

char Mine::getSymbol() const {
    return '@';
}

ElementType Mine::getType() const {
    return ElementType::Mine;
}
