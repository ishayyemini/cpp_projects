#include "GameObjectFactory.h"
#include "Wall.h"
#include "Mine.h"
#include "Tank.h"

int GameObjectFactory::tank_algo_count = 0;

std::unique_ptr<GameObject> GameObjectFactory::create(const char symbol, const Position position) {
    switch (symbol) {
        case '#': return std::make_unique<Wall>(position);
        case '@': return std::make_unique<Mine>(position);
        case '1': return std::make_unique<Tank>(position, 1, tank_algo_count++);
        case '2': return std::make_unique<Tank>(position, 2, tank_algo_count++);
        default: return nullptr;
    }
}

