#ifndef MINE_H
#define MINE_H

#include "GameObject.h"

class Mine final : public GameObject {
public:
    explicit Mine(const Position position): GameObject(position) {
    }

    [[nodiscard]] char getSymbol() const override { return '@'; }
};

#endif //MINE_H
