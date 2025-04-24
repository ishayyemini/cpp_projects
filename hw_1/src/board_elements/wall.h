#ifndef WALL_H
#define WALL_H
#include "board_elements/board_element.h"

static constexpr int max_health = 3;

class Wall final : public BoardElement {
    std::pair<int, int> position;
    int health = max_health;

public:
    explicit Wall(const std::pair<int, int> &position);

    [[nodiscard]] char getSymbol() const override { return '#'; }

    void takeDamage(const int amount) {
        health -= amount;
    }

    [[nodiscard]] bool isDestroyed() const override {
        return health <= 0;
    }
};

#endif //WALL_H
