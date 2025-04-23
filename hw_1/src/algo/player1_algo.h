#ifndef PLAYER1_ALGO_H
#define PLAYER1_ALGO_H

#include "algo.h"
#include "game/action.h"
#include "board/game_board.h"


class Player1Algo: public Algorithm {
public:
    explicit Player1Algo(GameBoard* board) : Algorithm(board) {}

    Action getNextAction() override;

    ~Player1Algo() override = default;
};



}



#endif //PLAYER1_ALGO_H
