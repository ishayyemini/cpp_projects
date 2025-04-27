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

    int getPlayerId() const { return player_id; }

    Tank *getPlayerTank() const;

    bool canShoot() const;

    Tank *getEnemyTank() const;

    bool isSafePosition(Position position, bool immediate_safe = false) const;

    bool isInLineOfSight(Position target) const;

    bool isInLineOfSight(Position from, Direction::DirectionType dir, Position target) const;

    std::vector<Position> getNearbyEmptyPositions(Position position) const;

    std::vector<Direction::DirectionType> getSafeDirections(Position position) const;

    bool canShootEnemy() const;

    bool canShootEnemy(Position from) const;

    bool canShootEnemy(Direction::DirectionType dir) const;

    bool canShootEnemy(Position from, Direction::DirectionType dir) const;

    Action rotateTowards(Direction::DirectionType to) const;

    Action rotateTowards(Position to) const;

    Action rotateTowards(Direction::DirectionType from, Direction::DirectionType to) const;

    bool isShellApproaching() const;

    bool isShellApproaching(Position position) const;

    bool isEnemyNearby() const;

    // Action getActionToPosition(Position target_position) const;
    //
    // bool isShellApproaching(int threat_threshold = -1) const;
    //
    // bool isObjectInLine(Position object_position, int distance) const;
    //
    // bool canRotateToFaceEnemy() const;
    //
    // bool isEmptyPosition(Position position) const;
    //
    // std::vector<Position> getApproachingShellsPosition(int threat_threshold = -1, bool get_closest = false) const;
    //
    // int getEnemyDistance() const;
    //
    // int getEnemyDistance(Position position) const;
    //
    // bool isFacingWall() const;
    //
    // bool isWallInDirection(Position position, Direction::DirectionType direction) const;
    //
    // Action rotateTowardsWall() const;
    //
    // Position calcNextPosition(Position position, Direction::DirectionType direction) const;
    //
    // bool canShootWall() const;
    //
    //
    // bool isLineOfSightClear(Position start, Position end, Direction::DirectionType direction) const;
    //
    // bool areObjectsInLine(Position obj1, Direction::DirectionType obj1_direction, Position obj2,
    //                       int max_distance) const;
};

#endif //GAMESTATE_H
