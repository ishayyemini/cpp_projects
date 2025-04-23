//
// Created by rachel on 4/2/2025.
//

#ifndef MINE_H
#define MINE_H
#include <utility>

#include "board_elements/board_element.h"

class Mine final : public BoardElement {
    std::pair<int, int> position;

public:
    explicit Mine(const std::pair<int, int> &position): position(position) {
    }

    [[nodiscard]] std::string getSymbol() const override { return "💣"; }
};

#endif //MINE_H
