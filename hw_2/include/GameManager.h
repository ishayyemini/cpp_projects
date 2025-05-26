#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <list>

#include "MyTankAlgorithm.h"
#include "Board.h"
#include "PathfindingAlgorithm.h"
#include "SimpleAlgorithm.h"
#include "Tank.h"

enum Winner {
    TIE_AMMO,
    TIE,
    PLAYER_1,
    PLAYER_2,
    NO_WINNER
};

class GameManager {
    bool visual;
    int game_step = 0;
    bool game_over = false;
    Winner winner = NO_WINNER;
    Board &board;
    int empty_countdown = -1;
    std::vector<std::string> step_history;
    std::vector<std::unique_ptr<TankAlgorithm> > tanks;

    bool moveForward(Tank &tank);

    bool moveBackward(Tank &tank);

    bool rotate(Tank &tank, int turn);

    bool shoot(Tank &tank);

    void shellsTurn() const;

    void checkDeaths();

    void tanksTurn();

    bool tankAction(Tank &tank, ActionRequest action);

public:
    explicit GameManager(Board &board): visual(false), board(board) {
    }

    explicit GameManager(Board &board, const bool visual): visual(visual), board(board) {
    }

    bool isGameOver() const { return game_over; }

    std::vector<std::string> &getStepHistory() { return step_history; }

    void processStep();

    std::string getGameResult() const;
};

#endif //GAMEMANAGER_H
