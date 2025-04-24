#ifndef BOARD_ELEMENT_H
#define BOARD_ELEMENT_H

#include "board/direction.h"


class BoardElement {
protected:
    std::pair<int, int> position;
    Direction::DirectionType direction = Direction::UP;
    bool destroyed = false;

public:
    explicit BoardElement(const std::pair<int, int> &position,
                          const Direction::DirectionType direction): position(position),
                                                                     direction(direction) {
    }

    explicit BoardElement(const std::pair<int, int> &position): position(position) {
    }

    virtual ~BoardElement() = default;

    [[nodiscard]] virtual char getSymbol() const { return ' '; }

    [[nodiscard]] virtual std::pair<int, int> getPosition() const { return position; }

    virtual void setPosition(const std::pair<int, int> &pos) { position = pos; }

    [[nodiscard]] virtual Direction::DirectionType getDirection() const { return direction; }

    virtual void setDirection(const Direction::DirectionType &dir) { direction = dir; }

    virtual void setDestroyed() { destroyed = true; }

    [[nodiscard]] virtual bool isDestroyed() const { return destroyed; }

    friend std::ostream &operator<<(std::ostream &os, const BoardElement &element) {
        switch (element.getSymbol()) {
            case '1':
                os << "[" << element.getDirection() << "🚘1]";
                break;
            case '2':
                os << "[" << element.getDirection() << "🚘2]";
                break;
            case '*':
                os << "[" << element.getDirection() << "☄️ ]";
                break;
            case '#':
                os << "[  🧱 ]";
                break;
            case '@':
                os << "[  💣 ]";
                break;
            default: ;
        }
        return os;
    }
};

#endif //BOARD_ELEMENT_H
