//
// Created by rachel on 4/2/2025.
//

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <map>
#include <memory>
#include <vector>
#include <board_elements/shell.h>

#include "board_elements/board_element.h"
#include "board_elements/shell.h"
#include "board_elements/tank.h"


class GameBoard {
    int height;
    int width;
    std::vector<std::vector<std::unique_ptr<BoardElement> > > board;
    std::map<int, std::pair<int, int> > player_1_tank_pos;
    std::map<int, std::pair<int, int> > player_2_tank_pos;
    std::vector<std::unique_ptr<Shell> > shells;
    std::map<std::pair<int, int>, int> shells_pos;

public:
    GameBoard();

    GameBoard(int width, int height);

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] Tank *getPlayerTank(int player_id, int tank_id) const;

    [[nodiscard]] Tank *getPlayerTank(int player_id) const;

    [[nodiscard]] BoardElement *getBoardElement(int row, int col) const noexcept;

    [[nodiscard]] BoardElement *getBoardElement(const std::pair<int, int> &pos) const noexcept;

    bool pushSymbol(int row, int col, char symbol);

    bool moveTank(const std::pair<int, int> &old_pos, const std::pair<int, int> &new_pos);

    bool moveShell(int shell_index, const std::pair<int, int> &new_pos);

    void displayBoard() const;

    void addShell(std::unique_ptr<Shell> shell);

    [[nodiscard]] Shell *getShell(int i) const;

    [[nodiscard]] Shell *getShell(const std::pair<int, int> &pos) const;

    void removeElement(const std::pair<int, int> &pos);

    void removeShell(int shell_index);

    int getShellsCount() const { return shells.size(); }
};

#endif //GAME_BOARD_H
