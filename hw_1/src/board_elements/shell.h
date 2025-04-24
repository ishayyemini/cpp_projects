//
// Created by rachel on 4/2/2025.
//

#ifndef SHELL_H
#define SHELL_H
#include "board/direction.h"
#include "board_elements/board_element.h"

class Shell final : public BoardElement {
public:
    explicit Shell(const std::pair<int, int> &position, Direction::DirectionType direction): BoardElement(
        position, direction) {
    }

    [[nodiscard]] char getSymbol() const override { return '*'; }
};

#endif //SHELL_H
