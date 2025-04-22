#ifndef BOARD_ELEMENT_H
#define BOARD_ELEMENT_H
#include <string>


class BoardElement {
protected:
    std::pair<int, int> position;

public:
    virtual ~BoardElement() = default;

    [[nodiscard]] virtual std::string getSymbol() const { return ""; }

    [[nodiscard]] std::pair<int, int> getPosition() const { return position; }

    void setPosition(const std::pair<int, int> &pos) { position = pos; }
};

#endif //BOARD_ELEMENT_H
