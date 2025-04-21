#include <ostream>

#include "board_element_factory.h"
#include "wall.h"
#include "mine.h"
#include "tank.h"


std::unique_ptr<BoardElement> BoardElementFactory::create(const char symbol) noexcept {
    switch (symbol) {
        case '#': return std::make_unique<Wall>();
        case '@': return std::make_unique<Mine>();
        case '1': return std::make_unique<Tank>(1);
        case '2': return std::make_unique<Tank>(2);
        default: return nullptr;
    }
}

