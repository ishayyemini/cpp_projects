#ifndef TANK_H
#define TANK_H
#include "board_elements/board_element.h"
#include "board/direction.h"
#include "game/action.h"

constexpr int MAX_SHELL = 16;
static int tank_count = 0;

class Tank final : public BoardElement {
    int player_id;
    const int tank_id;
    int backwards_counter = 3;
    int shooting_cooldown = 0;
    int shell = MAX_SHELL;
    Action queued_action = NONE;

public:
    explicit Tank(int player_id, Direction::DirectionType direction, const std::pair<int, int> &position);

    [[nodiscard]] char getSymbol() const override;

    [[nodiscard]] int getTankId() const;

    [[nodiscard]] int getRemainingShell() const;

    [[nodiscard]] int getPlayerId() const { return player_id; }

    int getBackwardsCounter() const { return backwards_counter; }

    void setBackwardsCounter(const int cnt) { backwards_counter = cnt; };

    [[nodiscard]] int getShootingCooldown() const { return shooting_cooldown; }

    void decreaseShootingCooldown() { if (shooting_cooldown > 0) shooting_cooldown--; }

    void shoot() { shooting_cooldown = 4; }

    int decreaseShell();

    void queueAction(Action action) { queued_action = action; }
    Action getQueuedAction() const { return queued_action; }
};


#endif //TANK_H
