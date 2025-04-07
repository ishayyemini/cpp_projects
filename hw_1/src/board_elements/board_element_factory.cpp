#include <ostream>

#include "board_element_factory.h"
#include "wall.h"
#include "mine.h"
#include "tank.h"
#include "empty_cell.h"


std::unique_ptr<BoardElement> BoardElementFactory::create(char symbol) {
    switch (symbol) {
        case '#': return std::make_unique<Wall>();
        case '@': return std::make_unique<Mine>();
        case '1': return std::make_unique<Tank>(1);
        case '2': return std::make_unique<Tank>(2);
        default: return std::make_unique<EmptyCell>();
    }
}

