#include "board/game_board.h"
#include <iostream>

#include "board_elements/board_element_factory.h"

GameBoard::GameBoard(const int width, const int height)
    : height(height), width(width), board(std::vector<std::vector<std::unique_ptr<BoardElement> > >(height)) {
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
        std::cerr << "Invalid board position" << std::endl;
        return false;
    }
    // if not a valid symbol we leave it empty as default!
    board[row].push_back(BoardElementFactory::create(symbol));
    return true;
}

// TODO: change so board_element overrides the << operator
void GameBoard::displayBoard() const {
    for (const auto &row: board) {
        for (const auto &cell: row) {
            if (cell != nullptr) {
                std::cout << cell->getSymbol();
            } else {
                std::cout << "◼️️";
            }
        }
        std::cout << '\n';
    }
}
