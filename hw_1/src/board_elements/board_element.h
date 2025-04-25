#ifndef BOARD_ELEMENT_H
#define BOARD_ELEMENT_H

#include "board/direction.h"


class GameObject {
protected:
    std::pair<int, int> position;
    Direction::DirectionType direction = Direction::UP;
    bool destroyed = false;

public:
    explicit GameObject(const std::pair<int, int> &position,
                          const Direction::DirectionType direction): position(position),
                                                                     direction(direction) {
    }

    explicit GameObject(const std::pair<int, int> &position): position(position) {
    }

    virtual ~GameObject() = default;

    [[nodiscard]] virtual char getSymbol() const { return ' '; }

    [[nodiscard]] virtual std::pair<int, int> getPosition() const { return position; }

    virtual void setPosition(const std::pair<int, int> &pos) { position = pos; }

    [[nodiscard]] virtual Direction::DirectionType getDirection() const { return direction; }

    virtual void setDirection(const Direction::DirectionType &dir) { direction = dir; }

    virtual void destroy() { destroyed = true; }

    [[nodiscard]] virtual bool isDestroyed() const { return destroyed; }

    friend std::ostream &operator<<(std::ostream &os, const GameObject &element);
};

#endif //BOARD_ELEMENT_H
