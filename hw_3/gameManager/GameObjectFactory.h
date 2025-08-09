#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <memory>

#include "GameObject.h"

namespace GameManager_322868852_340849710 {
    class GameObjectFactory {
        static int tank_algo_count;

    public:
        static std::unique_ptr<GameObject> create(char symbol, Position position, size_t num_shells);

        static void reset() { tank_algo_count = 0; }
    };
}

#endif //GAMEOBJECTFACTORY_H
