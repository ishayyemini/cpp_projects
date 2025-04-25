#include "board/game_board.h"
#include <iostream>

#include "board_elements/board_element_factory.h"

GameBoard::GameBoard() : height(0), width(0) {
}

GameBoard::GameBoard(const int width, const int height)
    : height(height), width(width), board(std::vector<std::vector<std::unique_ptr<BoardElement> > >(height)) {
}

int GameBoard::getHeight() const { return height; }
int GameBoard::getWidth() const { return width; }

Tank *GameBoard::getPlayerTank(const int player_id, const int tank_id) const {
    std::map<int, std::pair<int, int> > pos = (player_id == 1)? player_1_tank_pos : pos = player_2_tank_pos;

    if (!pos.contains(tank_id)) {
        return nullptr;
    }
    BoardElement *b = getBoardElement(pos[tank_id]);
    if (const auto t = dynamic_cast<Tank *>(b)) {
        return t;
    }
    return nullptr;
}

BoardElement *GameBoard::getBoardElement(const int row, const int col) const noexcept {
    if (row < 0 || row >= height || col < 0 || col >= width) {
        return nullptr;
    }
    return board[row][col].get();
}

BoardElement *GameBoard::getBoardElement(const std::pair<int, int> &pos) const noexcept {
    return getBoardElement(pos.first, pos.second);
}

bool GameBoard::pushSymbol(const int row, const int col, const char symbol) {
    // if not a valid symbol we leave it empty as default!
    std::unique_ptr<BoardElement> newElement = BoardElementFactory::create(symbol, {row, col});

    board[row].push_back(std::move(newElement));
    if (const auto tank = dynamic_cast<Tank *>(getBoardElement(row, col))) {
        if (tank->getPlayerId() == 1) {
            player_1_tank_pos[tank->getTankId()] = {row, col};
        } else {
            player_2_tank_pos[tank->getTankId()] = {row, col};
        }
    }

    return true;
}

bool GameBoard::moveBoardElement(const std::pair<int, int> &old_pos, const std::pair<int, int> &new_pos) {
    const std::pair mod_pos = {(new_pos.first % height + height) % height, (new_pos.second % width + width) % width};
    if (board[mod_pos.first][mod_pos.second] != nullptr) {
        std::cerr << "Can't move to non-empty space" << std::endl;
        return false;
    }

    board[mod_pos.first][mod_pos.second] = std::move(board[old_pos.first][old_pos.second]);

    if (const auto t = dynamic_cast<Tank *>(getBoardElement(mod_pos))) {
        if (t->getPlayerId() == 1) {
            player_1_tank_pos[t->getTankId()] = {mod_pos.first, mod_pos.second};
        } else {
            player_2_tank_pos[t->getTankId()] = {mod_pos.first, mod_pos.second};
        }
    }

    board[mod_pos.first][mod_pos.second].get()->setPosition(mod_pos);
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

void GameBoard::addShell(std::unique_ptr<Shell> shell) {
    shells.push_back(std::move(shell));
}

