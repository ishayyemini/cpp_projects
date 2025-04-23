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
        COUNT
    };

    static constexpr std::pair<int, int> getOffset(DirectionType direction) {
        return directionOffsets[static_cast<std::size_t>(direction)];
    }

    static constexpr std::pair<int, int> getOffset(int direction_index) {
        return directionOffsets[direction_index];
    }

    static constexpr int getDirectionSize() {
        return static_cast<int>(COUNT);
    }

    static constexpr DirectionType getDirectionType(int index) {
        //assuming index in valid
        return static_cast<DirectionType>(index);
    }

private:
    static constexpr std::size_t kDirectionCount = 8;

    static constexpr std::array<std::pair<int, int>, kDirectionCount> directionOffsets {{
        {  0,  1 },  // UP
        {  1,  1 },  // UP_RIGHT
        {  1,  0 },  // RIGHT
        {  1, -1 },  // DOWN_RIGHT
        {  0, -1 },  // DOWN
        { -1, -1 },  // DOWN_LEFT
        { -1,  0 },  // LEFT
        { -1,  1 }   // UP_LEFT
    }};

    Direction() = delete; // Prevent instantiation
};

#endif // DIRECTION_H
