#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Direction.h"

static int static_id = 0;

class GameObject {
protected:
    int id;
    Position position;
    Direction::DirectionType direction = Direction::UP;
    bool destroyed = false;

public:
    explicit GameObject(const Position position,
                        const Direction::DirectionType direction): id(static_id++), position(position),
                                                                   direction(direction) {
    }

    explicit GameObject(const Position position): id(static_id++), position(position) {
    }

    virtual ~GameObject() = default;

    [[nodiscard]] virtual char getSymbol() const { return ' '; }

    virtual int getId() const { return id; }

    [[nodiscard]] virtual Position getPosition() const { return position; }

    virtual void setPosition(const Position pos) { position = pos; }

    [[nodiscard]] virtual Direction::DirectionType getDirection() const { return direction; }

    virtual void setDirection(const Direction::DirectionType &dir) { direction = dir; }

    virtual void destroy() { destroyed = true; }

    [[nodiscard]] virtual bool isDestroyed() const { return destroyed; }

    virtual bool isWall() const { return false; }

    virtual bool isShell() const { return false; }

    virtual bool isMine() const { return false; }

    virtual bool isCollision() const { return false; }

    virtual bool isTank() const { return false; }

    friend std::ostream &operator<<(std::ostream &os, const GameObject &element);
};

#endif //GAME_OBJECT_H
