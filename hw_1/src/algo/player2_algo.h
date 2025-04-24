//
// Created by rachel on 4/2/2025.
//

#ifndef PLAYER2_ALGO_H
#define PLAYER2_ALGO_H

#include "algo/algorithm.h"
#include "board/game_board.h"

class Player2Algo final : public Algorithm {
public:
    explicit Player2Algo(GameBoard &board): Algorithm(board) {
    }

    Action getNextAction() override;

    static int calcDistance(const std::pair<int, int> &pos1, const std::pair<int, int> &pos2);

    [[nodiscard]] std::pair<int, int> calcNextPos(const std::pair<int, int> &pos,
                                                  const Direction::DirectionType &dir) const;
};

#endif //PLAYER2_ALGO_H
