//
// Created by rachel on 4/4/2025.
//

#ifndef EMPTY_CELL_H
#define EMPTY_CELL_H

class EmptyCell final : public BoardElement {
public:
    char getSymbol() const override { return ' '; }
};

#endif //EMPTY_CELL_H
