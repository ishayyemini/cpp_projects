#ifndef PLAYER1_ALGO_H
#define PLAYER1_ALGO_H

#include "algorithm.h"
#include "game/action.h"
#include "board/game_board.h"
#include "graph/board_graph.h"


class Player1Algo: public Algorithm {
public:
    explicit Player1Algo(GameBoard* board);

    Action getNextAction(std::pair<int, int> tank_position) override;

    ~Player1Algo() override = default;

private:
    BoardGraph board_graph;
    Action escape();
    Action chase(); //todo: add tankid as param
    Action suicide_mission();
    Action calcActionBFS(const std::pair<int, int>& tank_position);


};



#endif //PLAYER1_ALGO_H
