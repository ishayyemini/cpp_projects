#ifndef WALL_H
#define WALL_H
#include "board_element.h"

static constexpr int max_health = 3;

class Wall final : public BoardElement {
    int health = max_health;

public:
    char getSymbol() const override { return '#'; }

    void takeDamage(int amount){
        health -= amount;
    }

    bool isDestroyed() const {
        return health <= 0;
    }

};

#endif //WALL_H
