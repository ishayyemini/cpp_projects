#ifndef TANK_STATE_H
#define TANK_STATE_H

#include <utility>
#include "board/direction.h"


struct TankState {
    std::pair<int, int> position;
    Direction::DirectionType cannon_dir;

    bool operator==(const TankState& other) const {
        return position == other.position && cannon_dir == other.cannon_dir;
    }
};

struct TankStateHash {
    std::size_t operator()(const TankState& s) const {
        std::size_t h1 = std::hash<int>()(s.position.first);
        std::size_t h2 = std::hash<int>()(s.position.second);
        std::size_t h3 = std::hash<int>()(static_cast<int>(s.cannon_dir));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

#endif //TANK_STATE_H
