#ifndef ALGO_H
#define ALGO_H

#include "board/game_board.h"
#include "game/action.h"

class Algorithm {
protected:
    GameBoard &board;

public:
    explicit Algorithm(GameBoard &board): board(board) {
    }

    virtual ~Algorithm() = default;

    virtual Action getNextAction() { return NONE; }
};


#endif //ALGO_H
