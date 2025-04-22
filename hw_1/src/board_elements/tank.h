#ifndef TANK_H
#define TANK_H
#include "board_elements/board_element.h"
#include "board/direction.h"

constexpr int MAX_SHELL = 16;
static int tank_count = 0;

class Tank final : public BoardElement {
    std::pair<int, int> position;
    int player_id;
    const int tank_id;
    Direction::DirectionType cannon_direction;
    int backwards_counter = 2;
    int shell = MAX_SHELL;

public:
    explicit Tank(int player_id, Direction::DirectionType cannon_direction, const std::pair<int, int> &position);

    [[nodiscard]] std::string getSymbol() const override;

    void setCannonDirection(Direction::DirectionType cannonDirection);

    [[nodiscard]] Direction::DirectionType getCannonDirection() const;

    [[nodiscard]] int getTankId() const;

    [[nodiscard]] int getRemainingShell() const;

    [[nodiscard]] int getPlayerId() const { return player_id; }

    int getBackwardsCounter() const { return backwards_counter; }

    void setBackwardsCounter(const int cnt) { backwards_counter = cnt; };

    int decreaseShell();
};


#endif //TANK_H
