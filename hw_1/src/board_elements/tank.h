#ifndef TANK_H
#define TANK_H
#include "board_element.h"


class Tank final : public BoardElement {
private:
    int id;

public:
    explicit Tank(int id); //we want to prevent bugs
    char getSymbol() const override;
};


#endif //TANK_H
