#ifndef WALL_H
#define WALL_H
#include "board_element.h"

static constexpr int max_health = 3;

class Wall final : public BoardElement {
    int health = max_health;

public:
    Wall() = default;

    [[nodiscard]] std::string getSymbol() const override { return "⛔"; }

    void takeDamage(const int amount) {
        health -= amount;
    }

    [[nodiscard]] bool isDestroyed() const {
        return health <= 0;
    }
};

#endif //WALL_H
