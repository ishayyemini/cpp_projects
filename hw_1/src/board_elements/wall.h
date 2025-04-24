#ifndef WALL_H
#define WALL_H
#include "board_elements/board_element.h"

static constexpr int max_health = 2;

class Wall final : public BoardElement {
    int health = max_health;

public:
    explicit Wall(const std::pair<int, int> &position);

    [[nodiscard]] char getSymbol() const override { return '#'; }

    void takeDamage(const int amount = 1) {
        if (destroyed) return;
        health -= amount;
        if (health == 0) destroyed = true;
    }

    int getHealth() const { return health; }
};

#endif //WALL_H
