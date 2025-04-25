#include "Tank.h"

Tank::Tank(const Position position, const int player_id) : GameObject(position, player_id == 1
                                                                              ? Direction::LEFT
                                                                              : Direction::RIGHT),
                                                           player_id(player_id) {
}

char Tank::getSymbol() const {
    return player_id == 1 ? '1' : '2';
}

int Tank::decrementAmmunition() {
    shell--;
    return shell;
}

