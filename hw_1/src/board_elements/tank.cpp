#include "tank.h"

Tank::Tank(const int player_id, const Direction direction) : tank_id(++tank_count), player_id(player_id), cannon_direction(direction) {
}

std::string Tank::getSymbol() const {
    return player_id == 1 ? "1️⃣" : "2️⃣";
}

void Tank::setCannonDirection(Direction cannonDirection) {
    this->cannonDirection = cannonDirection;
}

Direction Tank::getCannonDirection() const {
    return cannonDirection;
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
