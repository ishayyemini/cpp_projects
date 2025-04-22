#ifndef DIRECTION_H
#define DIRECTION_H

#include <utility>
#include <array>

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
        return directionOffsets[static_cast<std::size_t>(dir)];
    }

private:
    static constexpr std::size_t kDirectionCount = 8;

    static constexpr std::array<std::pair<int, int>, kDirectionCount> directionOffsets{
        {
            {-1, 0}, // UP
            {-1, 1}, // UP_RIGHT
            {0, 1}, // RIGHT
            {1, 1}, // DOWN_RIGHT
            {1, 0}, // DOWN
            {1, -1}, // DOWN_LEFT
            {0, -1}, // LEFT
            {-1, -1} // UP_LEFT
        }
    };

    Direction() = delete; // Prevent instantiation
};

#endif // DIRECTION_H
