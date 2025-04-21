#ifndef TANK_H
#define TANK_H
#include "board_element.h"


class Tank final : public BoardElement {
    int id;

public:
    explicit Tank(int id); //we want to prevent bugs
    [[nodiscard]] std::string getSymbol() const override;
};


#endif //TANK_H
