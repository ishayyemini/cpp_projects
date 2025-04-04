#ifndef BOARD_ELEMENT_FACTORY_H
#define BOARD_ELEMENT_FACTORY_H

#include <memory>

#include "board_element.h"

class BoardElementFactory {
public:
    static std::unique_ptr<BoardElement> create(char symbol);
};


#endif //BOARD_ELEMENT_FACTORY_H
