#include "board_elements/tank.h"

Tank::Tank(const int player_id, const Direction::DirectionType cannon_direction,
           const std::pair<int, int> &position) : position(position),
                                                  tank_id(tank_count++),
                                                  player_id(player_id),
                                                  cannon_direction(cannon_direction) {
}

std::string Tank::getSymbol() const {
    return player_id == 1 ? "1️⃣" : "2️⃣";
}

void Tank::setCannonDirection(const Direction::DirectionType cannonDirection) {
    this->cannon_direction = cannonDirection;
}

Direction::DirectionType Tank::getCannonDirection() const {
    return cannon_direction;
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

void Tank::destroy() {
    destroyed = true;
}

bool Tank::getDestroyed() const {
    return destroyed;
}

