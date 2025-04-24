#include "board_elements/tank.h"

Tank::Tank(const int player_id, const Direction::DirectionType direction,
           const std::pair<int, int> &position) : BoardElement(position, direction),
                                                  player_id(player_id),
                                                  tank_id(tank_count++) {
}

char Tank::getSymbol() const {
    return player_id == 1 ? '1' : '2';
}

int Tank::getTankId() const {
    return tank_id;
}

int Tank::getRemainingShell() const {
    return shell;
}

int Tank::decreaseShell() {
    shell--;
    return shell;
}

