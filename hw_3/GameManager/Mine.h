#ifndef MINE_H
#define MINE_H

#include "GameObject.h"

namespace GameManager_322868852_340849710 {
    class Mine final : public GameObject {
    public:
        explicit Mine(const Position position) : GameObject(position) {
        }

        [[nodiscard]] char getSymbol() const override { return '@'; }

        bool isMine() const override { return true; }
    };
}

#endif //MINE_H
