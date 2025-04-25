#ifndef WALL_H
#define WALL_H

static constexpr int max_health = 2;

class Wall final : public GameObject {
    int health = max_health;

public:
    explicit Wall(const Position position): GameObject(position) {
    }

    [[nodiscard]] char getSymbol() const override { return '#'; }

    void takeDamage(const int amount = 1) {
        if (destroyed) return;
        health -= amount;
        if (health == 0) destroyed = true;
    }

    [[nodiscard]] int getHitCount() const { return health - max_health; }
};

#endif //WALL_H
