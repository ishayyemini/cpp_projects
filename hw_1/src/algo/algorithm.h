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
    virtual Action getNextAction(std::pair<int, int> tank_position) = 0;
    virtual ~Algorithm() = default;
    bool isActionValid(); //todo: maybe delete this - i don't think we will need this
    bool isTankThreaten(std::pair<int, int> tank_position);
    bool isShellDangerous(std::pair<int, int> tank_position);
    bool exists_threatening_shells(std::pair<int, int> tank_position);
    bool is_enemy_tank_threatening(std::pair<int, int> tank_position);
};


#endif //ALGO_H
