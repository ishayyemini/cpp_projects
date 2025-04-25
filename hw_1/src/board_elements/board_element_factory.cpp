#include <ostream>

#include "board_elements/board_element_factory.h"
#include "board_elements/wall.h"
#include "board_elements/mine.h"
#include "board_elements/tank.h"


std::unique_ptr<GameObject> BoardElementFactory::create(const char symbol, std::pair<int, int> position) noexcept {
    switch (symbol) {
        case '#': return std::make_unique<Wall>(position);
        case '@': return std::make_unique<Mine>(position);
        case '1': return std::make_unique<Tank>(1, Direction::LEFT, position);
        case '2': return std::make_unique<Tank>(2, Direction::RIGHT, position);
        default: return nullptr;
    }
}

