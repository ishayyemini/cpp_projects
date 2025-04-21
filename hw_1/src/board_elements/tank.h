#ifndef TANK_H
#define TANK_H
#include "board_element.h"
#include "board/direction.h"

constexpr int MAX_SHELL = 16;
static int tank_count = 0;

class Tank final : public BoardElement {
    const int tank_id;
    int player_id;
    Direction cannonDirection;
    int shell = MAX_SHELL;

public:
    explicit Tank(int player_id, Direction cannon_direction);

    std::string getSymbol() const override;

    void setCannonDirection(Direction cannonDirection);

    Direction getCannonDirection() const;

    int getTankId() const;

    int getRemainingShell() const;

    int decreaseShell();
};


#endif //TANK_H
