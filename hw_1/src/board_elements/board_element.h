#ifndef BOARD_ELEMENT_H
#define BOARD_ELEMENT_H


class BoardElement {
public:
    virtual ~BoardElement() = default;

private:
    virtual char getSymbol() const = 0;
};

#endif //BOARD_ELEMENT_H
