//
// Created by rachel on 4/2/2025.
//

#ifndef MINE_H
#define MINE_H

class Mine final : public BoardElement {
public:
    Mine() = default;

    [[nodiscard]] std::string getSymbol() const override { return "💣"; }
};

#endif //MINE_H
