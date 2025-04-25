#ifndef DIRECTION_H
#define DIRECTION_H

#include <utility>
#include <array>
#include <ostream>

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

    static constexpr std::pair<int, int> getOffset(const DirectionType dir) {
        return directionOffsets[dir / 45];
    }

    static constexpr DirectionType getDirection(const int dir) {
        int mod_dir = (dir % 360 + 360) % 360;
        if (mod_dir % 45 != 0) return UP;
        return static_cast<DirectionType>((dir % 360 + 360) % 360);
    }

    static constexpr int getDirectionSize() {
        return kDirectionCount;
    }

    friend std::ostream &operator<<(std::ostream &os, const DirectionType dir) {
        std::string dir_string = "";
        switch (dir) {
            case UP:
                dir_string = "⬆️";
                break;
            case UP_RIGHT:
                dir_string = "↗️";
                break;
            case RIGHT:
                dir_string = "➡️";
                break;
            case DOWN_RIGHT:
                dir_string = "↘️";
                break;
            case DOWN:
                dir_string = "⬇️";
                break;
            case DOWN_LEFT:
                dir_string = "↙️";
                break;
            case LEFT:
                dir_string = "⬅️";
                break;
            case UP_LEFT:
                dir_string = "↖️";
                break;
        }
        os << dir_string;
        return os;
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
