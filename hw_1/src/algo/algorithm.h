#ifndef ALGO_H
#define ALGO_H

#include "game/action.h"
#include "board/game_board.h"

class Algorithm {
protected:
    GameBoard* board;

public:
    explicit Algorithm(GameBoard* board) : board(board) {
       if (!board) {
        throw std::invalid_argument("Algorithm constructor received nullptr GameBoard");
    	}
    }
    virtual Action getNextAction() = 0;
    virtual ~Algorithm() = default;
};


#endif //ALGO_H
