#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <memory>

#include "GameObject.h"

class GameObjectFactory {
public:
    static std::unique_ptr<GameObject> create(char symbol, Position position);
};


#endif //GAMEOBJECTFACTORY_H
