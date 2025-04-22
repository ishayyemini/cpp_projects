#ifndef BOARD_ELEMENT_FACTORY_H
#define BOARD_ELEMENT_FACTORY_H

#include "board_elements/board_element.h"

class BoardElementFactory {
public:
    static std::unique_ptr<BoardElement> create(char symbol, std::pair<int, int> position) noexcept;
};


#endif //BOARD_ELEMENT_FACTORY_H
