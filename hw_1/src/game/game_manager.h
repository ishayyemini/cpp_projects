//
// Created by rachel on 4/2/2025.
//

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "board/direction.h"
#include "board/game_board.h"
#include "board_elements/tank.h"

enum Winner {
    TIE,
    PLAYER_1,
    PLAYER_2,
    NONE
};

class GameManager {
    int game_step = 0;
    bool game_over = false;
    Winner winner = NONE;
    GameBoard board;

    bool moveForward(int player_id, int tank_id);

    bool moveBackward(int player_id, int tank_id);

    bool rotate(int player_id, int tank_id, int turn);

public:
    explicit GameManager(const std::string &input_file_path);

    Winner start_game();
};

#endif //GAME_MANAGER_H
