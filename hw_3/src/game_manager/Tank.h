#ifndef TANK_H
#define TANK_H
#include "GameObject.h"
#include "Direction.h"

constexpr int MAX_PLAYERS = 9;
static auto tank_count = std::array<int, MAX_PLAYERS>{};

//todo: we support only two players - make it more modular
class Tank final : public GameObject {
    int player_index;
    int tank_index;
    int tank_algo_index;
    int backwards_counter = 3;
    int shooting_cooldown = 0;
    int shell;

public:
    explicit Tank(Position position, int player_id, int tank_algo_index, size_t num_shells): GameObject(position,
            player_id == 1
                ? Direction::LEFT
                : Direction::RIGHT),
        player_index(player_id),
        tank_index(tank_count[player_id]++),
        tank_algo_index(tank_algo_index),
        shell(num_shells) {
    }

    [[nodiscard]] char getSymbol() const override { return player_index == 1 ? '1' : '2'; }

    int getAmmunition() const { return shell; }

    [[nodiscard]] int getPlayerIndex() const { return player_index; }

    int getTankIndex() const { return tank_index; }

    int getTankAlgoIndex() const { return tank_algo_index; }

    int getBackwardsCounter() const { return backwards_counter; }

    void setBackwardsCounter(const int cnt) { backwards_counter = cnt; };

    [[nodiscard]] int getCooldown() const { return shooting_cooldown; }

    void setCooldown(const int cooldown) { shooting_cooldown = cooldown; }

    void decreaseShootingCooldown() { if (shooting_cooldown > 0) shooting_cooldown--; }

    void shoot() { shooting_cooldown = 4; }

    void decrementAmmunition() { if (shell > 0) shell--; }

    bool isTank() const override { return true; }
};


#endif //TANK_H
