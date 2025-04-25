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

    // In game_manager.h, add these new methods and data structures
private:
    struct MovingObject {
        enum Type { TANK, SHELL } type;

        int id; // Tank player ID or shell index
        std::pair<int, int> from;
        std::pair<int, int> to;
    };

    void processMovements();

    void detectAndHandleCollisions(std::vector<MovingObject> &movements);

    void applyMovements(const std::vector<MovingObject> &valid_movements);
};

#endif //GAME_MANAGER_H
