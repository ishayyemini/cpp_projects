
#ifndef PLAYER1_ALGO_H
#define PLAYER1_ALGO_H

#include "algo/algorithm.h"
#include "game/action.h"
#include "board/game_board.h"
#include "graph/board_graph.h"


class Player1Algo final : public Algorithm {
    struct Cell {
        std::pair<int, int> position;
        Direction::DirectionType parentDir;
    };

public:
    explicit Player1Algo(GameBoard &board);

    Action getNextAction() override;

    ~Player1Algo() override = default;

private:
    BoardGraph board_graph;
    std::pair<int, int> last_enemy_pos = {-1, -1};
    Direction::DirectionType cached_direction = Direction::UP;

    Action escape();

    Action chase(); //todo: add tankid as param
    Action suicide_mission();

    Direction::DirectionType bfsToOpponent(const std::pair<int, int> &tankPos,
                                           const std::pair<int, int> &targetPos) const;

    bool hasLineOfSightToEnemy() const;

    bool canShootWall() const;

    bool isTankThreaten(const std::pair<int, int> &tank_position) const;
};


#endif //PLAYER1_ALGO_H
