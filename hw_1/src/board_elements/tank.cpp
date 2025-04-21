#include "tank.h"

Tank::Tank(const int id) : id(id) {
}

std::string Tank::getSymbol() const {
    return id == 1 ? "1️⃣" : "2️⃣";
}
