//
// Created by rachel on 4/2/2025.
//

#ifndef SHELL_H
#define SHELL_H
#include "board/direction.h"

class Shell final : public BoardElement {
    std::pair<int, int> position;
    Direction::DirectionType direction;

public:
    explicit Shell(const std::pair<int, int> &position, Direction::DirectionType direction): position(position),
        direction(direction) {
    }

    [[nodiscard]] std::string getSymbol() const override { return "☄️"; }

    [[nodiscard]] Direction::DirectionType getDirection() const { return direction; }
};

#endif //SHELL_H
