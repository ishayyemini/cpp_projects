#ifndef DIRECTION_H
#define DIRECTION_H

#include <array>
#include <string>
#include <ostream>
#include "ActionRequest.h"

class Direction;
enum class DirectionType : int;

struct Position {
    int x;
    int y;

    bool operator==(const Position &pos2) const {
        return x == pos2.x && y == pos2.y;
    }

    bool operator<(const Position &other) const {
        if (x == other.x) {
            return y < other.y;
        }
        return x < other.x;
    }

    Position operator+(const Position &pos2) const {
        return Position(x + pos2.x, y + pos2.y);
    }

    Position operator-(const Position &pos2) const {
        return Position(x - pos2.x, y - pos2.y);
    }

    Position operator*(const int by) const {
        return Position(x * by, y * by);
    }

    Position operator/(const int by) const {
        return Position(x / by, y / by);
    }

    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    Position operator+(int dirValue) const;

    Position operator-(int dirValue) const;

    constexpr Position(): x(0), y(0) {}

    constexpr Position(const int row, const int col): x(row), y(col) {}

    constexpr Position(const size_t row, const size_t col): x(row), y(col) {}
};


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

    static std::string directionToString(const DirectionType dir) {
        switch (dir) {
            case UP: return "UP";
            case UP_RIGHT: return "UP_RIGHT";
            case RIGHT: return "RIGHT";
            case DOWN_RIGHT: return "DOWN_RIGHT";
            case DOWN: return "DOWN";
            case DOWN_LEFT: return "DOWN_LEFT";
            case LEFT: return "LEFT";
            case UP_LEFT: return "UP_LEFT";
        }
        return "UNKNOWN";
    }

    static constexpr Position getDirectionDelta(const DirectionType dir) {
        return directionOffsets[dir / 45];
    }

    static constexpr DirectionType getDirection(const int dir) {
        int mod_dir = (dir % 360 + 360) % 360;
        if (mod_dir % 45 != 0) return UP;
        return static_cast<DirectionType>(mod_dir);
    }

    static constexpr DirectionType getDirectionFromIndex(const int index) {
        return getDirection(index * 45);
    }

    static constexpr int getDirectionSize() {
        return kDirectionCount;
    }

    static DirectionType rotateDirection(const DirectionType dir, const bool clockwise, const bool full) {
        const int turn = full ? 90 : 45;
        const int mul = clockwise ? 1 : -1;
        const int mod_dir = ((dir + mul * turn) % 360 + 360) % 360;
        return getDirection(mod_dir);
    }

    static DirectionType getDirectionAfterAction(const DirectionType dir, const ActionRequest action) {
        switch (action) {
            case ActionRequest::RotateLeft45:
                return rotateDirection(dir, false, false);
            case ActionRequest::RotateRight45:
                return rotateDirection(dir, true, false);
            case ActionRequest::RotateLeft90:
                return rotateDirection(dir, false, true);
            case ActionRequest::RotateRight90:
                return rotateDirection(dir, true, true);
            default:
                return dir;
        }
    }

    static constexpr DirectionType getDirectionTo(const Position from, const Position to) {
        const int dx = to.x - from.x;
        const int dy = to.y - from.y;

        if (dx == 0 && dy < 0) return UP;
        if (dx > 0 && dy < 0) return UP_RIGHT;
        if (dx > 0 && dy == 0) return RIGHT;
        if (dx > 0) return DOWN_RIGHT;
        if (dx == 0 && dy > 0) return DOWN;
        if (dx < 0 && dy > 0) return DOWN_LEFT;
        if (dx < 0 && dy == 0) return LEFT;
        if (dx < 0) return UP_LEFT;

        return UP;
    }

    friend DirectionType operator-(const DirectionType dir) {
        return getDirection(dir + 180);
    }

    friend DirectionType operator+(const DirectionType dir, const DirectionType other) {
        return getDirection(static_cast<int>(dir) + static_cast<int>(other));
    }

    friend DirectionType operator-(const DirectionType dir, const DirectionType other) {
        return getDirection(static_cast<int>(dir) - static_cast<int>(other));
    }

    friend DirectionType operator+(const DirectionType dir, const int degrees) {
        return dir + getDirection(degrees);
    }

    friend DirectionType operator-(const DirectionType dir, const int degrees) {
        return dir - getDirection(degrees);
    }

    friend std::ostream &operator<<(std::ostream &os, const DirectionType dir) {
        static const std::array<std::string, kDirectionCount> directionStrings = {
            "⬆️", "↗️", "➡️", "↘️", "⬇️", "↙️", "⬅️", "↖️"
        };

        return os << directionStrings[dir / 45];
    }

private:
    static constexpr std::size_t kDirectionCount = 8;

    static constexpr std::array<Position, kDirectionCount> directionOffsets{
        Position(0, -1), // UP
        Position(1, -1), // UP_RIGHT
        Position(1, 0), // RIGHT
        Position(1, 1), // DOWN_RIGHT
        Position(0, 1), // DOWN
        Position(-1, 1), // DOWN_LEFT
        Position(-1, 0), // LEFT
        Position(-1, -1), // UP_LEFT
    };

    Direction() = delete; // Prevent instantiation
};

inline Position Position::operator+(const int dirValue) const {
    if (dirValue % 45 != 0 || dirValue < 0 || dirValue > 360) return Position(x, y);
    return Position(x, y) + Direction::getDirectionDelta(static_cast<Direction::DirectionType>(dirValue));
}

inline Position Position::operator-(const int dirValue) const {
    if (dirValue % 45 != 0 || dirValue < 0 || dirValue > 360) return Position(x, y);
    return Position(x, y) - Direction::getDirectionDelta(static_cast<Direction::DirectionType>(dirValue));
}

#endif // DIRECTION_H
