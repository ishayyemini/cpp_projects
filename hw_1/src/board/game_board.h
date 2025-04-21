//
// Created by rachel on 4/2/2025.
//

#ifndef GAME_BOARD_H
#define GAME_BOARD_H
#include <memory>
#include <vector>
#include "board_elements/board_element.h"


class GameBoard {
    int height;
    int width;
    std::vector<std::vector<std::unique_ptr<BoardElement> > > board;

public:
    GameBoard(int width, int height);

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] const BoardElement &getBoardElement(int row, int col) const;

    bool updateBoardElement(int row, int col, char symbol);

    void displayBoard() const;
};

#endif //GAME_BOARD_H
