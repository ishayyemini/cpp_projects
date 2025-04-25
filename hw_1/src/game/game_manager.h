//
// Created by rachel on 4/2/2025.
//

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "algo/player1_algo.h"
#include "algo/player2_algo.h"
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

    std::pair<int, int> calcNextPos(const std::pair<int, int> &pos, const Direction::DirectionType &dir) const;

    std::pair<int, int> calcNextPos(const std::pair<int, int> &pos, int dir_i) const;

    bool moveForward(Tank &tank);

    bool moveBackward(Tank &tank);

    bool rotate(Tank &tank, int turn);

    bool shoot(Tank &tank);

    void shellsTurn();

    Winner checkDeaths() const;

    void tanksTurn(Player1Algo &algo1, Player2Algo &algo2);

    void tankAction(Tank &tank, Action action);

public:
    explicit GameManager(const std::string &input_file_path);

    Winner startGame();
};

#endif //GAME_MANAGER_H
