#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <list>

#include "Algorithm.h"
#include "Board.h"
#include "Tank.h"

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
    Board &board;
    int empty_countdown = -1;
    Algorithm algo1;
    Algorithm algo2;
    std::vector<std::string> step_history;

    Position calcNextPos(Position pos, const Direction::DirectionType &dir) const;

    Position calcNextPos(Position pos, int dir_i) const;

    bool moveForward(Tank &tank);

    bool moveBackward(Tank &tank);

    bool rotate(Tank &tank, int turn);

    bool shoot(Tank &tank);

    void shellsTurn();

    Winner checkDeaths() const;

    // void tanksTurn(Player1Algo &algo1, Player2Algo &algo2);

    void tankAction(Tank &tank, Action action);

public:
    explicit GameManager(Board &board): board(board) {
    }

    void setAlgorithm1(Algorithm &algo) { algo1 = algo; }

    void setAlgorithm2(Algorithm &algo) { algo2 = algo; }

    bool isGameOver() const { return game_over; }

    std::vector<std::string> &getStepHistory() { return step_history; }

    void processStep();

    std::string getGameResult() const;

    // In game_manager.h, add these new methods and data structures


private:
    struct MovingObject {
        enum Type { TANK, SHELL } type;

        int id; // Tank player ID or shell index
        Position from;
        Position to;
    };

    void processMovements();

    void detectAndHandleCollisions(std::vector<MovingObject> &movements);

    void applyMovements(const std::vector<MovingObject> &valid_movements);
};

#endif //GAMEMANAGER_H
