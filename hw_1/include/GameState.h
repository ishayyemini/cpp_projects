#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "Board.h"

class GameState {
    Board &board;
    int player_id;

public:
    explicit GameState(Board &board, int player_id);

    Board &getBoard() const;

    Tank *getPlayerTank() const;

    Tank *getEnemyTank() const;

    bool isSafePosition(Position position, bool immediate_safe = false) const;

    std::vector<Direction::DirectionType> getSafeDirections(Position position) const;

    std::vector<Position> getNearbyEmptyPositions(Position position, int steps_num = 1) const;

    Action getActionToPosition(Position target_position) const;

    bool isShellApproaching(int threat_threshold = -1) const;

    bool isObjectInLine(Position object_position, int distance) const;

    bool canRotateToFaceEnemy() const;

    bool isEmptyPosition(Position position) const;

    std::vector<Position> getApproachingShellsPosition(int threat_threshold = -1, bool get_closest = false) const;

    bool isInLineOfSight(Position position) const;

    int getEnemyDistance() const;

    int getEnemyDistance(Position position) const;

    bool doesPlayerTankFacingWall() const;

    bool isWallInDirection(Position position, Direction::DirectionType direction) const;

    Action rotateTowardsWall() const;

    Position calcNextPosition(Position position, Direction::DirectionType direction) const;

private:
    int getObjectsDistance(Position obj1, Position obj2) const;

    bool areObjectsInLine(Position obj1, Direction::DirectionType obj1_direction, Position obj2,
                          int max_distance) const;
};

#endif //GAMESTATE_H
