#ifndef BOARD_ELEMENT_FACTORY_H
#define BOARD_ELEMENT_FACTORY_H

#include "GameObject.h"

class BoardElementFactory {
public:
    static std::unique_ptr<GameObject> create(char symbol, Position position);
};


#endif //BOARD_ELEMENT_FACTORY_H
