#ifndef SHELL_H
#define SHELL_H

#include "Direction.h"
#include "GameObject.h"

class Shell final : public GameObject {
    int owner_id;

public:
    explicit Shell(const Position position, const Direction::DirectionType direction, const int owner_id): GameObject(
            position, direction), owner_id(owner_id) {
    }

    int getOwnerId() const { return owner_id; }

    [[nodiscard]] char getSymbol() const override { return '*'; }

    bool isShell() const override { return true; }
};

#endif //SHELL_H
