//
// Created by rachel on 4/2/2025.
//

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "board/game_board.h"

enum Winner {
    TIE,
    PLAYER_1,
    PLAYER_2,
    NO_WINNER
};

class GameManager {
    int game_step = 0;
    bool game_over = false;
    Winner winner = NO_WINNER;
    GameBoard board;
    int empty_countdown = -1;

    bool moveForward(int player_id, int tank_id);

    bool moveBackward(int player_id, int tank_id);

    bool rotate(int player_id, int tank_id, int turn);

    bool shoot(int player_id, int tank_id);

public:
    explicit GameManager(const std::string &input_file_path);

    Winner start_game();
};

#endif //GAME_MANAGER_H
