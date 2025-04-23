#ifndef DIRECTION_H
#define DIRECTION_H

#include <utility>
#include <array>

class Direction {
public:
    enum DirectionType {
        UP = 0,
        UP_RIGHT = 45,
        RIGHT = 90,
        DOWN_RIGHT = 135,
        DOWN = 180,
        DOWN_LEFT = 225,
        LEFT = 270,
        UP_LEFT = 315,
    };

    static constexpr std::pair<int, int> getOffset(DirectionType direction) {
        return directionOffsets[static_cast<std::size_t>(direction)/45];
    }

    static constexpr std::pair<int, int> getOffset(int direction_index) {
        return directionOffsets[direction_index];
    }

    static constexpr int getDirectionSize() {
        return kDirectionCount;
    }

    static constexpr DirectionType getDirectionType(int index) {
        //assuming index in valid
        return static_cast<DirectionType>(index);
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
