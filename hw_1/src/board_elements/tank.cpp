#include "tank.h"

Tank::Tank(int id) : id(id) {
}

char Tank::getSymbol() const {
    return id == 1 ? '1' : '2';
}
