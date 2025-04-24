//
// Created by rachel on 4/2/2025.
//

#ifndef PLAYER1_ALGO_H
#define PLAYER1_ALGO_H

#include "algo/algorithm.h"

class Player1Algo final : public Algorithm {
public:
    explicit Player1Algo(GameBoard &board): Algorithm(board) {
    }

    Action getNextAction() override { return FORWARD; }
};

#endif //PLAYER1_ALGO_H
