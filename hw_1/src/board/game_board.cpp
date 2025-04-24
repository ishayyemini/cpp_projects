#include <iostream>

#include "board/game_board.h"
#include "board_elements/board_element_factory.h"
#include "board_elements/mine.h"
#include "board_elements/wall.h"

GameBoard::GameBoard() : height(0), width(0) {
}

GameBoard::GameBoard(const int width, const int height)
    : height(height), width(width), board(std::vector<std::vector<std::unique_ptr<BoardElement> > >(height)) {
}

int GameBoard::getHeight() const { return height; }
int GameBoard::getWidth() const { return width; }

Tank *GameBoard::getPlayerTank(const int player_id, const int tank_id) const {
    std::map<int, std::pair<int, int> > pos;
    if (player_id == 1) {
        pos = player_1_tank_pos;
    } else {
        pos = player_2_tank_pos;
    }
    if (!pos.contains(tank_id)) {
        return nullptr;
    }
    BoardElement *b = getBoardElement(pos[tank_id]);
    if (const auto t = dynamic_cast<Tank *>(b)) {
        return t;
    }
    return nullptr;
}

// This takes into account there being only one tank per player
Tank *GameBoard::getPlayerTank(const int player_id) const {
    std::map<int, std::pair<int, int> > pos;
    if (player_id == 1) {
        pos = player_1_tank_pos;
    } else {
        pos = player_2_tank_pos;
    }
    if (pos.empty()) return nullptr;
    BoardElement *b = getBoardElement(pos.begin()->second);
    if (const auto t = dynamic_cast<Tank *>(b)) return t;
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

bool GameBoard::moveShell(const int shell_index, const std::pair<int, int> &new_pos) {
    const std::pair mod_pos = {(new_pos.first % height + height) % height, (new_pos.second % width + width) % width};
    if (shell_index < 0 || shell_index >= shells.size()) {
        std::cerr << "Bad shell index" << std::endl;
        return false;
    }
    Shell *shell = getShell(shell_index);
    if (shell == nullptr) {
        std::cerr << "Bad shell index" << std::endl;
        return false;
    }

    if (!shells_pos.contains(shell->getPosition())) {
        std::cerr << "Can't find shell position" << std::endl;
        return false;
    }

    if (BoardElement *collision = board[mod_pos.first][mod_pos.second].get()) {
        if (dynamic_cast<Mine *>(collision)) {
            // Mine? We don't care.
        }
        if (const auto wall = dynamic_cast<Wall *>(collision)) {
            // Wall? Weaken it and destroy the shell.
            wall->takeDamage();
            if (wall->getHealth() == 0) {
                board[wall->getPosition().first][wall->getPosition().second] = nullptr;
            }
            shells_pos.erase(shell->getPosition());
            shells.erase(shells.begin() + shell_index);
            return true;
        }
        if (const auto tank = dynamic_cast<Tank *>(collision)) {
            // Tank? This is AFTER the tanks moved for this step. So let's handle it!
            tank->setDestroyed();
            if (tank->getPlayerId() == 1) {
                player_1_tank_pos.erase(tank->getTankId());
            } else {
                player_2_tank_pos.erase(tank->getTankId());
            }
            board[tank->getPosition().first][tank->getPosition().second] = nullptr;
            shells_pos.erase(shell->getPosition());
            shells.erase(shells.begin() + shell_index);
            return true;
        }
    }

    if (shells_pos.contains(mod_pos)) {
        // Shell? We delete both of them
        shells_pos.erase(shell->getPosition());
        shells.erase(shells.begin() + shell_index);
        const int other_shell_index = shells_pos[mod_pos];
        shells_pos.erase(mod_pos);
        shells.erase(shells.begin() + other_shell_index - 1);
        return true;
    }

    shells_pos.erase(shell->getPosition());
    shells_pos[mod_pos] = shell_index;
    shell->setPosition(mod_pos);

    return true;
}

void GameBoard::displayBoard() const {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (Shell *shell = getShell({i, j})) {
                std::cout << *shell;
            } else if (board[i][j] != nullptr) {
                std::cout << *board[i][j];
            } else {
                std::cout << "[     ]";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void GameBoard::addShell(std::unique_ptr<Shell> shell) {
    shells_pos[shell->getPosition()] = shells.size();
    shells.push_back(std::move(shell));
}

Shell *GameBoard::getShell(const int i) const {
    if (i >= shells.size()) {
        return nullptr;
    }
    return shells[i].get();
}

Shell *GameBoard::getShell(const std::pair<int, int> &pos) const {
    if (!shells_pos.contains(pos)) {
        return nullptr;
    }
    return getShell(shells_pos.at(pos));
}
