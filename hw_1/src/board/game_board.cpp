#include "board/game_board.h"
#include <iostream>

#include "board_elements/board_element_factory.h"

GameBoard::GameBoard(int width, int height)
    : height(height), width(width), board(height, std::vector<std::unique_ptr<BoardElement> >(width)) {
}

int GameBoard::getHeight() const { return height; }
int GameBoard::getWidth() const { return width; }

const BoardElement &GameBoard::getBoardElement(int row, int col) const {
    if (row < 0 || row >= height || col < 0 || col >= width) {
        throw std::out_of_range("getBoardElement: invalid index");
    }
    return *board[row][col];
}


bool GameBoard::updateBoardElement(int row, int col, char symbol) {
    if (row < 0 || row >= height || col < 0 || col >= width) {
        std::cout << "Invalid board position";
        return false;
    }
    board[row][col] = BoardElementFactory::create(symbol); //if not a valid symbol we leave it empty as default!
    return true;
}

// TODO: change so board_element overrides the << operator
void GameBoard::displayBoard() const {
    for (const auto& row : board) {
        for (const auto& cell : row) {
            std::cout << cell->getSymbol();
        }
        std::cout << '\n';
    }
}
