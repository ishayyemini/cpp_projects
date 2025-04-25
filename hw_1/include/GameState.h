#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Board.h"

class GameState {
    Board &board;
    int player_id;

public:
    explicit GameState(Board &board, const int player_id): board(board), player_id(player_id) {
    }

    Board &getBoard() const { return board; }

    Tank *getPlayerTank() const { return board.getPlayerTank(player_id); }

    Tank *getEnemyTank() const { return board.getPlayerTank(player_id == 1 ? 2 : 1); }

    bool isSafePosition(Position position) const;

    std::vector<Position> getNearbyEmptyPositions(Position position) const;

    std::vector<Position> getNearbyEmptyPositions() const {
        return getNearbyEmptyPositions(getPlayerTank()->getPosition());
    }

    bool isShellApproaching();

    bool isInLineOfSight(Position position) const;
};

#endif //GAMESTATE_H
