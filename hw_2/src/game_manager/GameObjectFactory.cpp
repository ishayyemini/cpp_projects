#include "GameObjectFactory.h"
#include "Wall.h"
#include "Mine.h"
#include "Tank.h"


std::unique_ptr<GameObject> GameObjectFactory::create(const char symbol, const Position position) {
    switch (symbol) {
        case '#': return std::make_unique<Wall>(position);
        case '@': return std::make_unique<Mine>(position);
        case '1': return std::make_unique<Tank>(position, 1);
        case '2': return std::make_unique<Tank>(position, 2);
        default: return nullptr;
    }
}

