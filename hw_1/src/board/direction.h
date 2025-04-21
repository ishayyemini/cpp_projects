#ifndef DIRECTION_H
#define DIRECTION_H
#include <utility>




class Direction {

public:
    enum DirectionType {
        UP,
        UP_RIGHT,
        RIGHT,
        DOWN_RIGHT,
        DOWN,
        DOWN_LEFT,
        LEFT,
        UP_LEFT,
    };

    static constexpr std::pair<int, int> getOffset(DirectionType dir) {
        return directionOffsets[static_cast<size_t>(dir)];
    }

private:
    static constexpr size_t directions_size = 8;
    static constexpr std::array<std::pair<int, int>, directions_size> directionOffsets {{
        {0, 1},    // UP
        {1, 1},    // UP_RIGHT
        {1, 0},    // RIGHT
        {1, -1},   // DOWN_RIGHT
        {0, -1},   // DOWN
        {-1, -1},  // DOWN_LEFT
        {-1, 0},   // LEFT
        {-1, 1}    // UP_LEFT
    }};

    Direction() = delete;
};


#endif //DIRECTION_H
