#ifndef TANKSTATE_H
#define TANKSTATE_H
#include <Direction.h>

struct TankState {
    Position pos;
    Direction::DirectionType dir;

    bool operator==(const TankState &other) const {
        return pos == other.pos && dir == other.dir;
    }
};

// Hash function for TankState
namespace std {
    template<>
    struct hash<TankState> {
        size_t operator()(const TankState &state) const noexcept {
            return (hash<int>()(state.pos.x) ^ (hash<int>()(state.pos.y) << 1)) ^ (hash<int>()(static_cast<int>(state.dir)) << 2);
        }
    };
}



#endif //TANKSTATE_H
