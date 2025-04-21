#ifndef BOARD_ELEMENT_H
#define BOARD_ELEMENT_H
#include <string>


class BoardElement {
public:
    virtual ~BoardElement() = default;

    [[nodiscard]] virtual std::string getSymbol() const = 0;
};

#endif //BOARD_ELEMENT_H
