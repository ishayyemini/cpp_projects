#ifndef TANK_H
#define TANK_H
#include "GameObject.h"
#include "Direction.h"
#include "Action.h"

constexpr int MAX_SHELL = 16;

class Tank final : public GameObject {
    int player_id;
    int backwards_counter = 3;
    int shooting_cooldown = 0;
    int shell = MAX_SHELL;

public:
    explicit Tank(Position position, int player_id): GameObject(position, player_id == 1
                                                                              ? Direction::LEFT
                                                                              : Direction::RIGHT),
                                                     player_id(player_id) {
    }

    [[nodiscard]] char getSymbol() const override { return player_id == 1 ? '1' : '2'; }

    int getAmmunition() const { return shell; }

    [[nodiscard]] int getPlayerId() const { return player_id; }

    int getBackwardsCounter() const { return backwards_counter; }

    void setBackwardsCounter(const int cnt) { backwards_counter = cnt; };

    [[nodiscard]] int getCooldown() const { return shooting_cooldown; }

    void setCooldown(const int cooldown) { shooting_cooldown = cooldown; }

    void decreaseShootingCooldown() { if (shooting_cooldown > 0) shooting_cooldown--; }

    void shoot() { shooting_cooldown = 4; }

    void decrementAmmunition() { if (shell > 0) shell--; }

    bool isTank() const override { return true; }

    // void queueAction(Action action) { queued_action = action; }
    // Action getQueuedAction() const { return queued_action; }
};


#endif //TANK_H
