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
        if (health > 0) health -= amount;
    }

    [[nodiscard]] int getHitCount() const { return max_health - health; }

    int getHealth() const { return health; }

    bool isDestroyed() const override { return health == 0; }

    void destroy() override { health = 0; }
};

#endif //WALL_H
